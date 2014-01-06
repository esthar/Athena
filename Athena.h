/*
 * Athena.h
 *
 * Amherst College Electronics Club - Nov 2013
 *  Author: André Lucas Antunes de Sá
 */ 


#ifndef ATHENA_H_
#define ATHENA_H_

#define WARM_UP_TIME 180.0
#define FALLING_THRESHOLD_ACCEL 700
#define FALLING_THRESHOLD_TIME 900.0
#define FALLEN_THRESHOLD_BARO_TIME 3600.0

#define F_CPU 18432000
#define TWITTER_NUMBER "+14138629545"
#define BASE_NUMBER "+14138629545"
#define HERMES_WRITE 0xFE
#define HERMES_READ 0xFF

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <inttypes.h>
#include "Cellular.h"
#include "GPS.h"
#include "IMU.h"
#include "Sensors.h"
#include "USART.h"
#include "TWI.h"

void ioInit(void);
void analogInit(void);
void timerInit(void);

void SDLog(char * const data);
void SDLogChar(const char data);
void HermesWrite(char * const str);
void HermesRead(char * buffer);
void PrintToStr (char * const buffer, char * str, uint8_t * ptr);
void numberToASCII(char * str, int32_t number);
void floatToASCII(char * str, float number); //Up to 3 decimal places. Careful with numbers larger than 100000


/*Pin Definitions
TX S0 - PD4
TX S1 - PD3
RX S0 - PC3
RX S1 - PC2
CEL_ON - PD7
CEL_Ready - PD6
CEL_Network - PB2
Batt - ADC0
Humid - ADC1
Green Led - PB0
Red Led - PB1
*/

#endif /*ATHENA_H_*/