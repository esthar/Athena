/*
 * TWI.h
 *
 * Amherst College Electronics Club - Nov 2013
 *  Author: André Lucas Antunes de Sá
 */ 


#ifndef TWI_H_
#define TWI_H_

#define F_CPU 18432000

#include <util/twi.h>
#include <util/delay.h>
#include <inttypes.h>

void TWIInit(void);
void TWIStart(void);
void TWIWrite(const char data);
char TWIRead(void);
void TWIStop(void);

#endif /* TWI_H_ */