/*
 * Sensors.h
 *
 * Amherst College Electronics Club - Nov 2013
 *  Author: André Lucas Antunes de Sá
			 Angelina Guan (ADC)
			 Emily Masten (ADC)
			 Jacob Pfau (Temperature Sensor)

 */ 


#ifndef SENSORS_H_
#define SENSORS_H_

#define F_CPU 18432000
#define ADCREF 3.29

#include <avr/io.h>
#include <util/delay.h>
#include <inttypes.h>
#include "USART.h"
#include "TWI.h"

//ADC Measurement
float humidityRead(void);
float battery_voltRead(void);

//I2C
////Temperature sensor
float tempRead(const uint8_t option); //Read Temperature...... 0 for internal, 1 for external.

#define TEMP_EXTERNAL_READ	0x93
#define TEMP_EXTERNAL_WRITE	0x92
#define TEMP_INTERNAL_READ	0x97
#define TEMP_INTERNAL_WRITE	0x96
#define TEMP_REG 	0x00

////External Pressure sensor (Bigger range)
float ext_baroRead();

#define BARO_EXTERNAL_READ	0x71

//Geiger Counter
uint8_t geigerParse(char *str);	//Returns 0 if successful or 1 if not

#endif /* SENSORS_H_ */