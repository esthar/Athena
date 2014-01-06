/*
 * GPS.c
 *
 * Amherst College Electronics Club - Nov 2013
 *  Author: André Lucas Antunes de Sá, Henry Laney and Thais Correia
 */ 

#include "GPS.h"

uint8_t GPSParse(char *str)
{
	char data = 0;
	uint8_t str_pos = 3;
	uint8_t checksum = 0x50; //Checksum of the first three int8_tacters, GPG
	uint8_t messages_extracted = 0;
	char heading[10];
	char speed[10];
	
	str[0] = 'G';
	str[1] = 'P';
	str[2] = 'G';
	
	PORTC &= ~((1<<PORTC2) | (1<<PORTC3));
	data = USARTReadChar(); //Clear UDR register, in case the first character contains trash from the transition.
		
	for (int16_t i=1000U; i!=0; --i)
	{	
		data = USARTReadChar();
		if (data == '$')
		{
			data = USARTReadChar(); //Should be a G
			data = USARTReadChar(); //Should be a P
			data = USARTReadChar();
			if ((data == 'G') & ((messages_extracted & (1<<0)) == 0))
			{
				uint8_t GPSCheckSum[3];
				messages_extracted |= 1<<0;

				for (uint8_t j=90U; j!=0; j--)	//We'll hopefully never finish the loop normally but instead break when the asterisk for the checksum comes in.
				{
					data = USARTReadChar();
					if (data == '*') break;
					
					str[str_pos] = data;
					++str_pos;
					
					checksum ^= data;					
				}
				
				str[str_pos] = 0;
				
				GPSCheckSum[0] = USARTReadChar();
				GPSCheckSum[1] = USARTReadChar();
				GPSCheckSum[2] = 0;
				
				GPSCheckSum[2] |= (GPSCheckSum[0] - '0') << 4;
				if (GPSCheckSum[1]<'A')
				{
					GPSCheckSum[2] |= (GPSCheckSum[1] - '0');
				} 
				else
				{
					GPSCheckSum[2] |= (GPSCheckSum[1] - 'A' + 10);
				}
				
				
				if (checksum != GPSCheckSum[2])
				{
					return 2;
				}
			}
			else
			{
				if ((messages_extracted & (1<<1)) == 0)
				{
					messages_extracted |= 1<<1;
					
					for (uint8_t i=255U; (i!=0) & (data!=','); --i)
					{
						data = USARTReadChar();
					}
					
					for (uint8_t i=0; i!=9U; ++i)	//Heading
					{
						heading[i] = USARTReadChar();
						if (heading[i]==',')
						{
							++i;
							heading[i]=0;
							break;
						}
					}					
					
					for (uint8_t j=255U; j!=250U; j--)
					{
						for (uint8_t i=255U, data=0; (i!=0) & (data!=','); --i)
						{
							data = USARTReadChar();
						}					
					}
					
					for (uint8_t i=0; i!=9U; ++i)	//Speed
					{
						speed[i] = USARTReadChar();
						if (speed[i]==',')
						{
							speed[i]=0;
							break;
						}
					}				
				}
			}
		}
		
		if (messages_extracted == 3)
		{	
			uint8_t j = 0;
			str_pos = 0;
			
			while (str[str_pos]!=',') ++str_pos;
			++str_pos;
			
			do 
			{
				str[j] = str[str_pos];
				++str_pos;
				++j;
			} while (str[str_pos-1]!=',');	//UTC

			do
			{
				str[j] = str[str_pos];
				++str_pos;
				++j;
			} while (str[str_pos-1]!=',');	//Latitude

			while (str[str_pos]!=',') ++str_pos;
			++str_pos;
			
			do
			{
				str[j] = str[str_pos];
				++str_pos;
				++j;
			} while (str[str_pos-1]!=',');	//Longitude	
			
			while (str[str_pos]!=',') ++str_pos;
			++str_pos;
			
			do
			{
				str[j] = str[str_pos];
				++str_pos;
				++j;
			} while (str[str_pos-1]!=',');	//Position Fix

			do
			{
				str[j] = str[str_pos];
				++str_pos;
				++j;
			} while (str[str_pos-1]!=',');	//Satellites

			do
			{
				str[j] = str[str_pos];
				++str_pos;
				++j;
			} while (str[str_pos-1]!=',');	//HDOP

			do
			{
				str[j] = str[str_pos];
				++str_pos;
				++j;
			} while (str[str_pos-1]!=',');	//Altitude
			
			for (uint8_t i=0; heading[i]!=0; ++j, ++i)
			{
				str[j] = heading[i];
			}

			for (uint8_t i=0; speed[i]!=0; ++j, ++i)
			{
				str[j] = speed[i];
			}

			str[j]=0;
																	
			return 0;
		}
		
		_delay_ms(1);
	}
	
	return 1;
}