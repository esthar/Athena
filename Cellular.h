/*
 * Cellular.h
 *
 * Amherst College Electronics Club - Nov 2013
 *  Author: André Lucas Antunes de Sá, Henry Laney and Thais Correia
 */ 


#ifndef CELLULAR_H_
#define CELLULAR_H_

#define F_CPU 18432000

#include <avr/io.h>
#include <util/delay.h>
#include <inttypes.h>
#include "USART.h"

void cellularTurnOn();
void cellularTurnOff();
uint8_t cellularReady();
uint8_t cellularNetworkReady();
void cellularSend(const char *number, const char *text1, const char *text2, const char *text3, const char *text4, const char *text5);



#endif /* CELLULAR_H_ */