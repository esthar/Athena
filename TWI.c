/*
 * TWI.c
 *
 * Amherst College Electronics Club - Nov 2013
 *  Author: André Lucas Antunes de Sá
 */ 

#include "TWI.h"

volatile uint8_t TWI_error = 0;

void TWIInit(void) // Function to initialize master
{
	TWBR=0x54;    // Set bit rate
	TWSR=(0<<TWPS1)|(0<<TWPS0);    // Set prescalar bits
	//This gives us a frequency of ~20MHz/200=100KHz
}

void TWIStart(void)
{
	TWCR= (1<<TWINT)|(1<<TWSTA)|(1<<TWEN);	//Clear TWI interrupt flag, Put start condition on SDA, Enable TWI
	
 	uint8_t i = 255U;

	while((TWCR & (1<<TWINT)) == 0){	//Wait for transmission or bailout
	 	if(i==0)
	 	{
		 	TWI_error = 1;
		 	break;
	 	}
		--i;
		_delay_us(1);
	}
}

void TWIWrite(const char data)
{
	TWDR = data;
	TWCR = (1<<TWINT)|(1<<TWEN);
	
 	uint8_t i = 255U;

 	while((TWCR & (1<<TWINT)) == 0){
	 	if(i==0)
	 	{
		 	TWI_error = 1;
		 	break;
	 	}
	 	--i;
	 	_delay_us(1);
 	}
}


char TWIRead(void)
{
	TWCR = (1<<TWINT)|(1<<TWEN);
	
 	uint8_t i = 255U;

 	while((TWCR & (1<<TWINT)) == 0){
	 	if(i==0)
	 	{
		 	TWI_error = 1;
		 	break;
	 	}
	 	--i;
	 	_delay_us(1);
 	}
	 
	return TWDR;
}

void TWIStop(void)
{
	TWCR = (1<<TWINT)|(1<<TWEN)|(1<<TWSTO);		//Clear TWI interrupt flag, Put stop condition on SDA, Enable TWI
	
 	uint8_t i = 255U;

 	while((TWCR & (1<<TWSTO)) == 0){
	 	if(i==0)
		{
			TWI_error = 1;
			break;
		}
	 	--i;
	 	_delay_us(1);
 	}
}