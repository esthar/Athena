/*
 * Cellular.c
 *
 * Amherst College Electronics Club - Nov 2013
 *  Author: André Lucas Antunes de Sá, Henry Laney and Thais Correia
 */ 

#include "Cellular.h"

void cellularTurnOn()
{
	if (((PIND & (1<<6))>>6)==0)
	{
		DDRD |= (1<<PORTD7);
		_delay_ms(2000);
		DDRD &= ~(1<<PORTD7);
	}
}

void cellularTurnOff()
{
	if (((PIND & (1<<6))>>6)==1)
	{
		DDRD |= (1<<PORTD7);
		_delay_ms(2000);
		DDRD &= ~(1<<PORTD7);
	}
}

uint8_t cellularReady()
{
	return (((PIND & (1<<6))>>6));
}

uint8_t cellularNetworkReady()
{
	return (((PINB & (1<<2))>>2));
}

void cellularSend(const char *number, const char *text1, const char *text2, const char *text3, const char *text4, const char *text5)
{
	_delay_ms(300);
	PORTD &= ~(1<<PORTD4);
	PORTD |= (1<<PORTD3);

	USARTPrint("AT+CMGF=1\r");
	
	for (int16_t i=0xFFFF; !(UCSR0A & (1<<RXC0)) & (i!=0); --i){
		_delay_us(100);
	}
	_delay_ms(10);

	USARTPrint("AT+CMGS=\"");	//Sets number
	USARTPrint(number);
	USARTPrint("\"\r");
		
	for (int16_t i=0xFFFF; !(UCSR0A & (1<<RXC0)) & (i!=0); --i){
		_delay_us(100);
	}
	_delay_ms(10);
		
	USARTPrint(text1);
	USARTPrint(text2);
	USARTPrint(text3);
	USARTPrint(text4);
	USARTPrint(text5);
		
	for (int16_t i=0xFFFF; !(UCSR0A & (1<<RXC0)) & (i!=0); --i){
		_delay_us(100);
	}
	_delay_ms(10);
	
	USARTWriteChar(0x1A);	//Sends ctrl-z
		
	for (int16_t i=0xFFFF; !(UCSR0A & (1<<RXC0)) & (i!=0); --i){
		_delay_us(100);
	}
	_delay_ms(10);
}