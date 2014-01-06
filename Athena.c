/*
 * Athena.c
 *
 * Amherst College Electronics Club - Nov 2013
 *  Author: André Lucas Antunes de Sá
 
  This program is the firmware for the Athena Computer, one of the flight computers on board the payload of the Electronics Club's high altitude balloon project, codename Daedalus.
  Athena's purposes are: collecting data from sensor; translating this data into appropriate message formats for radio and SMS transmission; sending SMS texts following certain events;
  sending radio messages to ground relayed by the Hermes computer; generating status flags and communicating these to Hermes.
  
  Note that I have decided to do floating-point calculations instead of fixed point for precision and clarity. The serial communication is the bottleneck of the system anyway, and it's 
  where the main loop spends most of its time. On average, we go through 2 loops in one second.
  
  The green LED is turned on while initializing and should turn off once the main loop starts. If the barometric sensor does not register a good value when initializing, the green LED will not turn off.
  While in the loop, the red led should burst every 10 seconds or so.
  
 */ 

//Calibrate Geiger!!!!!

#include "Athena.h"

extern uint8_t TWI_error;		//Declared on TWI.c. Set to 1 after an unsuccessful TWI operation 

volatile float time = 0;		//Time elapsed since initialization

ISR(TIMER1_COMPA_vect)
{
	time += 0.1;				//Timer overflows every tenth of a second
}

int main(void)
{
	char buffer[256];
	char TWI_TXBUFFER[256];
	uint8_t TWI_BufferPtr;		//Pointer for reading characters from Hermes
	uint8_t fallStat = 0;		//Set to 1 when falling (Accelerometer in X direction)
	uint8_t SMSchoice = 0;
	uint8_t Athena_Status = 0;	//Status tells the states of Athena. The lowest bits concern the GPS. Bit 2 correspond to the Geiger counter status, bit 3 to the TWI, bit 4 set after started falling.
	float baro_gnd = 0;			//Cal value for ground level pressure
	
	int32_t var = 0;
	float fvar= 0.0;
	
	_delay_ms(1000);
	timerInit();
	ioInit();
	analogInit();
	TWIInit();
	USARTInit(119);
	SDLog("Serial Initialized\n\r");
	sei();	//Enable interrupts
	
	accelInit();
	SDLog("Accel Initialized\n\r");
	gyroInit();
	SDLog("Gyro Initialized\n\r");
	baroInit();
	SDLog("Baro Initialized\n\r");
	magInit();
	SDLog("Magnetometer Initialized\n\r");
	cellularTurnOn();
	SDLog("Cellular Module Power-Up\n\r");
	
	baro_gnd = baroRead() - 30.0;
	floatToASCII(buffer, baro_gnd);
	SDLog("Ground Pressure set at:");
	SDLog(buffer);
	SDLog("\n\r");
	
	SDLog("Time(s), AccelX(mg), AccelY(mg), AccelZ(mg), MagX(mG), MagY(mG), MagZ(mG), GyroX(dps), GyroY(dps), GyroZ(dps),"
" Baro(mbar), ExtBaro(mbar), TmpIn(C), TmpOut(C), TmpIMU(C), Humid(%), Batt(V), UTC, Lat, Long, Fix, Sat, HDOP, Alt, Heading, Speed,"
" CPM, uSv/Hr, S/F, Athena-Status, Hermes-Time(s), Hermes-BatVolt(V), Hermes-RadioRX, Hermes-Status\n\r");
	
	if ((baro_gnd > 950.0)|(baro_gnd < 1030.0))	// Do not turn off green LED if the cal baro value does not make sense
	{
		PORTB &= ~(1<<PORTB0);
	}
	
    while(1)	//Main while loop
    {
		TWI_BufferPtr = 1;				//Reset buffer pointer (0 is special and contains fall status which we address manually)
		Athena_Status &= 0b00110000;	//Maintain fall status and message status, reset everything else
		
		if ((uint32_t)time%10UL == 0)
		{
			PORTB |= 1<<PORTB1;
			_delay_ms(100);
			PORTB &= ~(1<<PORTB1);
		}
		
		if ((SMSchoice==0) && (time > WARM_UP_TIME))	//Queue first SMS message to be sent regarding successful run after the warm-up time
		{
			Athena_Status |= 1<<5;
		}
		
		////////////////////////////////////////////////----Time
				
		floatToASCII(buffer,time);						//Log time on card and put it in the TWI write buffer for Hermes
		SDLog(buffer);
		SDLogChar(',');
		
		TWI_TXBUFFER[TWI_BufferPtr] = ',';
		TWI_BufferPtr++;
		PrintToStr(buffer, TWI_TXBUFFER, &TWI_BufferPtr);
		TWI_TXBUFFER[TWI_BufferPtr] = ',';
		
		///////////////////////////////////////////////----AccelX

		var = accelXRead();						//Log accelX
		numberToASCII(buffer, var);
		SDLog(buffer);
		SDLogChar(',');
		
		/* 
			Figure out if payload is falling. For every measurement where the reading of the accel is 
			above the threshold the variable fallStat increases by 1 until 25 and it decreases otherwise.
			The payload is believe to be falling if fallStat is bigger than 14. (Thus it will roughly take 5 seconds
			for it to change from falling to not-falling and vice-versa)
		*/
		
		if (time > FALLING_THRESHOLD_TIME)
		{
			if (var < FALLING_THRESHOLD_ACCEL)
			{
				if (fallStat != 25U)
				{
					++fallStat;
				}
			} 
			else
			{
				if (fallStat != 0)
				{
					--fallStat;
				}
			}
			
			if (fallStat > 14U)
			{
				Athena_Status |= (1<<4);
			}
			else
			{
				Athena_Status &= ~(1<<4);
			}
		}
		TWI_TXBUFFER[0] = ((Athena_Status & (1<<4))>>4) +'0';	//Put status on TWI buffer and accel value

		TWI_TXBUFFER[TWI_BufferPtr] = ',';
		TWI_BufferPtr++;
		PrintToStr(buffer, TWI_TXBUFFER, &TWI_BufferPtr);
		TWI_TXBUFFER[TWI_BufferPtr] = ',';
		TWI_BufferPtr++;		
		
		///////////////////////////////////////////////----AccelY
		
		var = accelYRead();
		numberToASCII(buffer, var);
		SDLog(buffer);
		SDLogChar(',');

		PrintToStr(buffer, TWI_TXBUFFER, &TWI_BufferPtr);
		TWI_TXBUFFER[TWI_BufferPtr] = ',';
		TWI_BufferPtr++;		

		///////////////////////////////////////////////----AccelZ

		var = accelZRead();
		numberToASCII(buffer, var);
		SDLog(buffer);
		SDLogChar(',');
		
		PrintToStr(buffer, TWI_TXBUFFER, &TWI_BufferPtr);
		TWI_TXBUFFER[TWI_BufferPtr] = ',';
		TWI_BufferPtr++;
		
		///////////////////////////////////////////////----MagX
				
		fvar = magXRead();
		floatToASCII(buffer, fvar);
		SDLog(buffer);
		SDLogChar(',');

		///////////////////////////////////////////////----MagY

		fvar = magYRead();
		floatToASCII(buffer, fvar);
		SDLog(buffer);
		SDLogChar(',');
		
		///////////////////////////////////////////////----MagZ
		
		fvar = magZRead();
		floatToASCII(buffer, fvar);
		SDLog(buffer);
		SDLogChar(',');
		
		///////////////////////////////////////////////----GyroX	
			
		fvar = gyroXRead();
		floatToASCII(buffer,fvar);
		SDLog(buffer);
		SDLogChar(',');

		PrintToStr(buffer, TWI_TXBUFFER, &TWI_BufferPtr);
		TWI_TXBUFFER[TWI_BufferPtr] = ',';
		TWI_BufferPtr++;
		
		///////////////////////////////////////////////----GyroY
		
		fvar = gyroYRead();
		floatToASCII(buffer,fvar);
		SDLog(buffer);
		SDLogChar(',');
		
		///////////////////////////////////////////////----GyroZ

		fvar = gyroZRead();
		floatToASCII(buffer,fvar);
		SDLog(buffer);
		SDLogChar(',');
		
		///////////////////////////////////////////////----Baro
		
		fvar = baroRead();
		floatToASCII(buffer,fvar);
		SDLog(buffer);
		SDLogChar(',');

		PrintToStr(buffer, TWI_TXBUFFER, &TWI_BufferPtr);
		TWI_TXBUFFER[TWI_BufferPtr] = ',';
		TWI_BufferPtr++;
		
		if ((fvar < 900.0) && (SMSchoice == 0x00) && (time > 300.0))	//Send message about rising if pressure decreases below 900 mbar
		{
			Athena_Status |= 1<<5;
			SMSchoice = 0x01;
		}
		
		if ((fvar < 400.0) && (SMSchoice == 0x01) && (time > 300.0))	//Send message about rising if pressure decreases below 400 mbar
		{
			Athena_Status |= 1<<5;
			SMSchoice = 0x02;
		}
		
		if ((fvar < 200.0) && (SMSchoice == 0x02) && (time > 300.0))	//Send message about rising if pressure decreases below 200 mbar
		{
			Athena_Status |= 1<<5;
			SMSchoice = 0x03;
		}
		
		if ((fvar > 300.0) && ((uint32_t)time%60UL==0) && ((SMSchoice == 0x04)|(SMSchoice == 0x05)))	//Keep sending messages with GPS location while falling every minute
		{
			
			Athena_Status |= 1<<5;
			SMSchoice = 0x05;
		}		
		
		if ((fvar > baro_gnd) && (SMSchoice != 0x07) && (time > FALLEN_THRESHOLD_BARO_TIME))	//Send final messages after pressure is close to ground pressure
		{
			Athena_Status |= 1<<5;
			SMSchoice = 0x06;
		}		
		
		///////////////////////////////////////////////----extBaro
		
		fvar = ext_baroRead();
		floatToASCII(buffer,fvar);
		SDLog(buffer);
		SDLogChar(',');

		PrintToStr(buffer, TWI_TXBUFFER, &TWI_BufferPtr);
		TWI_TXBUFFER[TWI_BufferPtr] = ',';
		TWI_BufferPtr++;
		
		///////////////////////////////////////////////----intTmp
				
		fvar = tempRead(0);
		floatToASCII(buffer,fvar);
		SDLog(buffer);
		SDLogChar(',');
		
		PrintToStr(buffer, TWI_TXBUFFER, &TWI_BufferPtr);
		TWI_TXBUFFER[TWI_BufferPtr] = ',';
		TWI_BufferPtr++;
		
		///////////////////////////////////////////////----extTmp		
		
		fvar = tempRead(1);
		floatToASCII(buffer,fvar);
		SDLog(buffer);
		SDLogChar(',');
		
		PrintToStr(buffer, TWI_TXBUFFER, &TWI_BufferPtr);
		TWI_TXBUFFER[TWI_BufferPtr] = ',';
		TWI_BufferPtr++;
		
		///////////////////////////////////////////////----IMUTmp		
		
		fvar = IMUtempRead();
		floatToASCII(buffer,fvar);
		SDLog(buffer);
		SDLogChar(',');		
		
		///////////////////////////////////////////////----humidity
				
		fvar = humidityRead();
		floatToASCII(buffer,fvar);
		SDLog(buffer);
		SDLogChar(',');
		
		///////////////////////////////////////////////----battery
		
		fvar = battery_voltRead();
		floatToASCII(buffer,fvar);
		SDLog(buffer);
		SDLogChar(',');
		
		PrintToStr(buffer, TWI_TXBUFFER, &TWI_BufferPtr);
		TWI_TXBUFFER[TWI_BufferPtr] = ',';
		TWI_BufferPtr++;
		
		///////////////////////////////////////////////

		if (TWI_error==1)			//If any TWI devices could not be talked to, initialize everything again
		{
			Athena_Status |= 1<<3;
			TWI_error = 0;
			for (uint8_t i=255U; i!=200U;--i)
			{
				_delay_ms(10);
				accelInit();
				magInit();
				gyroInit();
				baroInit();
			}
		}


////////////GPS

		if (1)			//Had GPS parsing only on even time but decided to have it always. (uint32_t)time%2==0
		{
			Athena_Status |= GPSParse(buffer);		//Put GPS status on bit 0 and 1 of status
			if((Athena_Status & 0x03) == 0)			//Only print buffer if successfully retrieved info
			{
				SDLog(buffer);
				SDLogChar(',');
				
				PrintToStr(buffer, TWI_TXBUFFER, &TWI_BufferPtr);
				
				TWI_TXBUFFER[TWI_BufferPtr] = ',';
				++TWI_BufferPtr;
			}
		} 
		else
		{
			for (uint8_t i=255U; i!=246U; --i)
			{
				SDLogChar(',');
				
				TWI_TXBUFFER[TWI_BufferPtr] = ',';
				++TWI_BufferPtr;				
			}
		}
			
///////////Geiger

		if ((uint32_t)time%10UL==0)
		{
			Athena_Status |= (geigerParse(buffer)<<2);	//Put Geiger status on bit 2 of status
			if((Athena_Status & 1<<2) == 0)				//Only print buffer if successfully retrieved info
			{
				SDLog(buffer);
				PrintToStr(buffer, TWI_TXBUFFER, &TWI_BufferPtr);
			}
			else
			{
				SDLogChar(',');
				SDLogChar(',');
				
				TWI_TXBUFFER[TWI_BufferPtr] = ',';
				++TWI_BufferPtr;
				TWI_TXBUFFER[TWI_BufferPtr] = ',';
				++TWI_BufferPtr;				
			}
			SDLogChar(',');
			
			TWI_TXBUFFER[TWI_BufferPtr] = ',';
			++TWI_BufferPtr;	
		}
		else
		{
			SDLogChar(',');
			SDLogChar(',');
			SDLogChar(',');	
			
			TWI_TXBUFFER[TWI_BufferPtr] = ',';
			++TWI_BufferPtr;			
			TWI_TXBUFFER[TWI_BufferPtr] = ',';
			++TWI_BufferPtr;			
			TWI_TXBUFFER[TWI_BufferPtr] = ',';
			++TWI_BufferPtr;				
		}		
///////////
		
		///////////////////////////////////////////////----AthenaStatus
		
		Athena_Status |= (cellularReady()<<6) | (cellularNetworkReady()<<7); //Get cellular status
		
		numberToASCII(buffer,Athena_Status);
		SDLog(buffer);
		SDLogChar(',');
		
		PrintToStr(buffer, TWI_TXBUFFER, &TWI_BufferPtr);
		
		///////////////////////////////////////////////----Write TWI buffer to Hermes
		
		TWI_TXBUFFER[TWI_BufferPtr] = 0;		
		HermesWrite(TWI_TXBUFFER);
		
		///////////////////////////////////////////////----Read TWI buffer from Hermes
		
		TWI_error = 0;
		HermesRead(buffer);
		
		if (TWI_error==0)
		{
			SDLog(buffer);
		}
		TWI_error = 0;
		
		SDLog("\n\r");	//End of logging issue a newline
		
		///////////////////////////////////////////////----Send SMS if needed
		
		if ((SMSchoice < 0x04) && ((Athena_Status & (1<<4))>>4))
		{
			Athena_Status |= (1<<5);
			SMSchoice = 0x04;
		}
		
		if ((SMSchoice == 0x07) && ((uint32_t)time%60UL == 0))
		{
			Athena_Status |= (1<<5);
		}
		
		
		if ((Athena_Status & (1<<5))>>5)
		{
			if (cellularNetworkReady()==1)
			{
				switch(SMSchoice)
				{
					case 0x00:
					GPSParse(buffer);
					fvar = baroRead();
					floatToASCII(TWI_TXBUFFER,fvar);
								
					cellularSend(BASE_NUMBER,"All systems ready for launch! ", buffer, ",", TWI_TXBUFFER,". Sent by Athena");
					_delay_ms(4000);
					cellularSend(TWITTER_NUMBER,"All systems ready for launch!", "", "", ""," Sent by Athena");
					cellularTurnOff();
					break;
								
					case 0x01:
					GPSParse(buffer);
					fvar = baroRead();
					floatToASCII(TWI_TXBUFFER,fvar);
								
					cellularSend(BASE_NUMBER,"Gaining altitude! ", buffer, ",", TWI_TXBUFFER,"");
					_delay_ms(4000);
					cellularSend(TWITTER_NUMBER,"Gaining altitude! ~1km up.", "", "", ""," Sent by Athena");
					cellularTurnOff();
					break;
								
					case 0x02:
					GPSParse(buffer);
					fvar = baroRead();
					floatToASCII(TWI_TXBUFFER,fvar);
								
					cellularSend(BASE_NUMBER,"Gaining altitude! ", buffer, ",", TWI_TXBUFFER,"");
					_delay_ms(4000);
					cellularSend(TWITTER_NUMBER,"Gaining altitude! ~7km up.", "", "", ""," Sent by Athena");
					cellularTurnOff();
					break;
								
					case 0x03:
					GPSParse(buffer);
					fvar = baroRead();
					floatToASCII(TWI_TXBUFFER,fvar);
								
					cellularSend(BASE_NUMBER,"Gaining altitude! ", buffer, ",", TWI_TXBUFFER,"");
					_delay_ms(4000);
					cellularSend(TWITTER_NUMBER,"Gaining altitude! ~12km up.", "", "", ""," Sent by Athena");
					cellularTurnOff();
					break;
								
					case 0x04:
					GPSParse(buffer);
					fvar = baroRead();
					floatToASCII(TWI_TXBUFFER,fvar);
								
					cellularSend(BASE_NUMBER,"Balloon burst!", buffer, ",", TWI_TXBUFFER,"");
					_delay_ms(4000);
					cellularSend(TWITTER_NUMBER,"Balloon burst! Coming back.", "", "", ""," Sent by Athena");
					cellularTurnOff();
					break;
								
					case 0x05:
					GPSParse(buffer);
					fvar = baroRead();
					floatToASCII(TWI_TXBUFFER,fvar);
								
					cellularSend(BASE_NUMBER,"Descending", buffer, ",", TWI_TXBUFFER,"");
					break;
								
					case 0x06:
					GPSParse(buffer);
					fvar = baroRead();
					floatToASCII(TWI_TXBUFFER,fvar);
								
					cellularSend(BASE_NUMBER,"Land! ", buffer, TWI_TXBUFFER,"","");
					_delay_ms(4000);
					cellularSend(TWITTER_NUMBER,"Olympus, we have landed!", "", "", ""," Sent by Athena");
					SMSchoice++;
					break;
								
					case 0x07:
					GPSParse(buffer);
					fvar = battery_voltRead();
					floatToASCII(TWI_TXBUFFER,fvar);
								
					cellularSend(BASE_NUMBER,"Come pick me up! ", buffer, TWI_TXBUFFER,"","");
					break;
								
					default:
					break;
				}
				Athena_Status &= ~(1<<5);
			}
			else
			{
				if (!cellularReady())
				{
					cellularTurnOn();
				}
			}
		}
	}
}

void ioInit(void)
{
	DDRB |= (1<<PORTB0)|(1<<PORTB1);	//LEDs
	DDRC |= (1<<PORTC2)|(1<<PORTC3);	//RXMUX
	DDRD |= (1<<PORTD3)|(1<<PORTD4);	//TXMUX
	
	PORTB |= 1<<PORTB0;					//Start-up with Green Led on
}

void analogInit(void)
{		
	ADMUX |= (1<<REFS0);				//Voltage Reference Selection: AVcc with External capacitor at Aref

	ADCSRA |= (1<<ADPS2)|(1<<ADPS1)|(1<<ADPS0);	//Select clock for the ADC: Main clock/1024 (Approx 20KHz)
	ADCSRA |= (1<<ADEN);				//Enable the ADC
	ADCSRA |= (1<<ADSC);				//Start a conversion to initialize the ADC
}

void timerInit(void)
{
	TCCR1B |= 1<<WGM12;					//Set timer to overflow once it reaches OCR1A
	TCCR1B |= (1<<CS12) | (1<<CS10);	//Set clock to be prescaled to F_CPU/1024
	OCR1AH = 1800>>8;					//Set top of the counter to 1800 (This gives as an overflow every tenth of a second)
	OCR1AL = 1800 & 0xFF;
	TIMSK1 |= 1<<OCIE1A;				//Enable the timer overflow interrupt
}

void SDLog(char * const data)
{
	PORTD &= ~(1<<PORTD3);	//Change MUX channel and print data through serial
	PORTD |= (1<<PORTD4);
	USARTPrint(data);
}

void SDLogChar(const char data)
{
	PORTD &= ~(1<<PORTD3);
	PORTD |= (1<<PORTD4);
	USARTWriteChar(data);
}

void HermesWrite(char * const str)
{
	for (uint8_t i=10U; i!=0; --i)
	{
		TWI_error = 0;
		
		TWIStart();
		TWIWrite(HERMES_WRITE);
		for (uint8_t i=0; ; ++i)
		{
			TWIWrite(str[i]);
			if (str[i]==0) break;
		}
		TWIStop();
		
		if (TWI_error==0) break;
		
		_delay_ms(10);
	}
}

void HermesRead(char * buffer)
{
	for (uint8_t i=10U; i!=0; --i)
	{
		TWI_error = 0;
		
		TWIStart();
		TWIWrite(HERMES_WRITE);
		TWIWrite(0xFF);
		for (uint8_t i=0; i!=31U; ++i)
		{
			TWIStart();
			TWIWrite(HERMES_READ);
			buffer[i] = TWIRead();
			if (buffer[i]==0) break;
		}
		TWIStop();
		
		if (TWI_error==0) break;
		
		_delay_ms(10);
	}	
}

void PrintToStr (char * const buffer, char * str, uint8_t * ptr)
{
	for (uint8_t i=0; buffer[i]!=0; ++i, ++(*ptr))
	{
		str[*ptr] = buffer[i];
	}
}

void numberToASCII(char * str, int32_t number) //Prints the value of a variable number into a a string of ASCII characters (My own simplified version of the same feature in the printf function)
{
	uint32_t tens = 1000000000UL;
	uint8_t digits[10];
	uint8_t i = 0;
	uint8_t j = 0;

	if (number < 0)
	{
		number *= -1;
		str[j]='-';
		++j;
	}
	
	while (i<10U)
	{
		digits[i] = number/tens;
		number = number%tens;
		tens /= 10U;
		++i;
	}

	for (i=0; i<10U; ++i)
	{
		if (digits[i]!=0)
		{
			break;
		}
	}
	
	if (i!=10U)
	{
			for (; i<10U; ++i, ++j)
			{
				str[j] = digits[i] + '0';
			}
	}
	else
	{
		str[j]='0';
		++str;
	}
	
	str[j]=0;
	
}

/*
Same as above but for float numbers
Up to 3 decimal place
Be careful with numbers larger than 100000
*/

void floatToASCII(char * str, float number) //This function can be greatly optimized for speed. Using an array for the tens variable would greatly improve it at the cost of some memory.
{
	float tens = 1000000000.0;
	float rational = 0.0;
	uint8_t digits[13];
	uint8_t i = 0;
	uint8_t j = 0;
	
	if (number < 0.0)
	{
		number *= -1.0;
		str[j]='-';
		++j;
	}
	
	rational = number - (int32_t)number;
	
	while (i<10U)
	{
		digits[i] = number/tens;
		number = (int32_t)number%(int32_t)tens;
		tens /= 10.0;
		++i;
	}

	for (i=0; i<10U; ++i)
	{
		if (digits[i]!=0)
		{
			break;
		}
	}
	
	if (i!=10U)
	{
		for (; i<10U; ++i, ++j)
		{
			str[j] = digits[i] + '0';
		}
	}
	else
	{
		str[j]='0';
		++str;
	}
	//////////// Up to here the result was the same as the function above but below we also take care of digits to the right of the decimal point
	
	str[j] = '.';
	++str;
	
	i = 10U;
	
	while (i<13U)
	{
		tens = 10.0;
		rational *= tens;
		digits[i] = rational;
		rational -= digits[i];
		tens *= 10.0;
		++i;
	}

	for (i=12U; i>9U; --i)
	{
		if (digits[i]!=0)
		{
			break;
		}
	}
		
	if (i!=9U)
	{
		for (uint8_t k=10U; k<(i+1); ++k,++j)
		{
			str[j] = digits[k] + '0';
		}
	}
	else
	{
		str[j]='0';
		++str;
	}
	
	str[j]=0;
	
}