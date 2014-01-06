/*
 * IMU.h
 *
 * Amherst College Electronics Club - Nov 2013
 *  Author: André Lucas Antunes de Sá
			 Nigel Mevana (Accel)
			 Patrick DeVivo (Gyro)
			 Sam Tang (Baro)
 */ 


#ifndef IMU_H_
#define IMU_H_

#define F_CPU 18432000

#include <util/delay.h>
#include <inttypes.h>
#include "TWI.h"

#define ACCEL_SCALE 2 //Range from +-4g
#define MAG_SCALE_XY 1.1  //Divide by this. Range +-1.3 mG
#define MAG_SCALE_Z 0.98  //Divide by this. Range +-1.3 mG
#define IMU_TMP_SCALE 8.0  //Divide by this
#define GYRO_SCALE 0.00875 //Range from +-250mdps
#define BARO_SCALE 4096.0 //Divide by this


void accelInit(void);
int16_t accelXRead(void); //Gives reading in mili g
int16_t accelYRead(void); //Gives reading in mili g
int16_t accelZRead(void); //Gives reading in mili g

void magInit(void);
float magXRead(void);
float magYRead(void);
float magZRead(void);

float IMUtempRead(void);

void gyroInit(void);
float gyroXRead(void); //Gives reading in dps
float gyroYRead(void); //Gives reading in dps
float gyroZRead(void); //Gives reading in dps

void baroInit(void);
float baroRead(void); //Gives reading in mbar

///////////////////////////////////////////////////////////////////////////
//Accelerometer Definitions

#define ACCEL_READ 0x33
#define ACCEL_WRITE 0x32

#define ACCEL_CTRL_REG1_A 0x20
#define ACCEL_CTRL_REG4_A 0x23

#define ACCEL_X_AXIS_OUT_LOW 0x28
#define ACCEL_X_AXIS_OUT_HIGH 0x29

#define ACCEL_Y_AXIS_OUT_LOW 0x2A
#define ACCEL_Y_AXIS_OUT_HIGH 0x2B

#define ACCEL_Z_AXIS_OUT_LOW 0x2C
#define ACCEL_Z_AXIS_OUT_HIGH 0x2D

//Magnetometer definitions

#define MAG_READ 0x3D
#define MAG_WRITE 0x3C

#define CRA_REG_M 0x00
#define CRB_REG_M 0x01
#define MR_REG_M 0x02

#define MAG_X_AXIS_OUT_LOW 0x04
#define MAG_X_AXIS_OUT_HIGH 0x03

#define MAG_Y_AXIS_OUT_LOW 0x06
#define MAG_Y_AXIS_OUT_HIGH 0x05

#define MAG_Z_AXIS_OUT_LOW 0x08
#define MAG_Z_AXIS_OUT_HIGH 0x07

//Temperature

#define IMU_TMP_HIGH 0x31
#define IMU_TMP_LOW 0x32

//Gyroscope Definitions

#define GYRO_READ 0xD7
#define GYRO_WRITE 0xD6

#define GYYRO_CTRL_REG_1 0x20

#define GYRO_X_AXIS_OUT_LOW 0x28
#define GYRO_X_AXIS_OUT_HIGH 0x29

#define GYRO_Y_AXIS_OUT_LOW 0x2A
#define GYRO_Y_AXIS_OUT_HIGH 0x2B

#define GYRO_Z_AXIS_OUT_LOW 0x2C
#define GYRO_Z_AXIS_OUT_HIGH 0x2D

//Pressure Sensor Definitions

#define BARO_READ  0xBB
#define BARO_WRITE 0xBA

#define BARO_RES_CONF 0x10
#define BARO_CTRL_REG1 0x20

#define BARO_PRESS_OUT_XL 0x28 //lower
#define BARO_PRESS_OUT_L 0x29 //middle
#define BARO_PRESS_OUT_H 0x2A //higher

#endif /* IMU_H_ */