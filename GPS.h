/*
 * GPS.h
 *
 * Amherst College Electronics Club - Nov 2013
 *  Author: André Lucas Antunes de Sá, Henry Laney and Thais Correia
 */ 


#ifndef GPS_H_
#define GPS_H_

#define F_CPU 18432000

#include <avr/io.h>
#include <util/delay.h>
#include <inttypes.h>
#include "USART.h"
#include "TWI.h"

uint8_t GPSParse(char *str); //Returns 0 if successful, 1 if it times out or 2 if checksum is invalid



#endif /* GPS_H_ */