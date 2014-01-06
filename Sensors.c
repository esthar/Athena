/*
 * Sensors.c
 *
 * Amherst College Electronics Club - Nov 2013
 *  Author: André Lucas Antunes de Sá
			Angelina Guan (ADC)
			Emily Masten (ADC)
			Jacob Pfau (Temperature Sensor)
 */

#include "Sensors.h"

float humidityRead(void)
{
	uint8_t ADClow = 0;
	uint8_t ADChigh = 0;
	int16_t result = 0;
	float humidity = 0;
	
	ADMUX &= ~((1<<MUX1) | (1<<MUX2) | (1<<MUX3)); //Choose ADC1
	ADMUX |= 1<<MUX0;
	ADCSRA |= 1<<ADSC; //Do conversion.
	_delay_us(100);
	
	ADClow = ADCL;
	ADChigh = ADCH;
	
	result = (int16_t)(ADChigh<<8 | ADClow);
	humidity = (float)result*(ADCREF/1023.0)*1.2237; //This lines returns the voltage output of the sensor. The last number is for the voltage divider. Let's hope that the temperature coeff for the resistors is indeed the same and their ratio stays the same!
	//Convert voltage values
	humidity = (humidity - 0.958)/0.0307;
	
	return humidity;
}

float battery_voltRead(void)
{
	uint8_t ADClow = 0;
	uint8_t ADChigh = 0;
	int16_t result;
	float voltage = 0;
	
	ADMUX &= ~((1<<MUX0) | (1<<MUX1) | (1<<MUX2) | (1<<MUX3)); //Choose ADC0
	ADCSRA |= 1<<ADSC; //Do conversion.
	_delay_us(100);
	
	ADClow = ADCL;
	ADChigh = ADCH;
	
	result = (int16_t)(ADChigh<<8 | ADClow);
	voltage = (float)result*(ADCREF/1023.0)*4.0565; //The last number is for the voltage divider. Let's hope that the temperature coeff for the resistors is indeed the same and their ratio stays the same!
	
	return voltage;
}

float tempRead(uint8_t option)
{
	uint8_t h = 0;
	uint8_t l = 0;
	int16_t tmp = 0;
	
	if (option==0)
	{
		TWIStart();
		TWIWrite(TEMP_INTERNAL_WRITE);
		TWIWrite(TEMP_REG);
		TWIStart();
		TWIWrite(TEMP_INTERNAL_READ);
		h = TWIRead();
		l = TWIRead();
		TWIStop();
		
		tmp = (int16_t)(h<<8 | l)>>4;

		return (float)tmp/16; //return Celsius
	} 
	else
	{
		TWIStart();
		TWIWrite(TEMP_EXTERNAL_WRITE);
		TWIWrite(TEMP_REG);
		TWIStart();
		TWIWrite(TEMP_EXTERNAL_READ);
		h = TWIRead();
		l = TWIRead();
		TWIStop();
		
		tmp = (int16_t)(h<<8 | l)>>4;

		return (float)tmp/16; //return Celsius
	}
}

float ext_baroRead()
{
	uint8_t h = 0;
	uint8_t l = 0;
	float tmp = 0;

	TWIStart();
	TWIWrite(BARO_EXTERNAL_READ);
	h = TWIRead();
	l = TWIRead();
	TWIStop();
		
	tmp = (int16_t)(h<<8 | l);
	tmp = (tmp/16383 - .1)*1292.767;

	return tmp;
}

uint8_t geigerParse(char *str)
{
	char data = 0;
	uint8_t str_pos = 0;
	
	PORTC |= (1<<PORTC2) | (1<<PORTC3);
	data = USARTReadChar(); //Clear UDR register, in case the first character contains trash from the transition.
	
	for (uint16_t i=1000U; i!=0; --i)
	{
		data = USARTReadChar();
		if (data == 'C')
		{
			data = USARTReadChar(); //Should be a P
			data = USARTReadChar();
			if (data == 'M')
			{
				for (uint8_t j=90U; j!=0; --j)	//We'll hopefully never finish the loop normally
				{
					for (uint8_t i=255; (i!=0) & (data!=' '); --i)
					{
						data = USARTReadChar();
					}
					
					do
					{
						str[str_pos]=USARTReadChar();
						++str_pos;
					} while (str[str_pos-1]!=',');	//CPM
					data = USARTReadChar(); //Get rid of space
					data = USARTReadChar(); //Get rid of space
										
					for (uint8_t i=255U; (i!=0) & (data!=' '); --i)
					{
						data = USARTReadChar();
					}

					do
					{
						str[str_pos]=USARTReadChar();
						++str_pos;
					} while (str[str_pos-1]!=',');	//uSv/Hr
					
					data = USARTReadChar(); //Get rid of space
					str[str_pos] = USARTReadChar();
					
					str[str_pos+1] = 0;
					
					return 0;
					
				}
			}
		}

		_delay_ms(1);
	}
	
	return 1;
}