/*
 * IMU.c
 *
 * Amherst College Electronics Club - Nov 2013
 *  Author: André Lucas Antunes de Sá
			Nigel Mevana (Accel)
			Patrick DeVivo (Gyro)
			Sam Tang (Baro)
 */ 

#include "IMU.h"

void accelInit(void)
{	
	TWIStart();
	TWIWrite(ACCEL_WRITE);
	TWIWrite(ACCEL_CTRL_REG1_A);
	TWIWrite(0b01010111);	//Turn on device 100Hz
	TWIStop();
	
	TWIStart();
	TWIWrite(ACCEL_WRITE);
	TWIWrite(ACCEL_CTRL_REG4_A);
	TWIWrite(0b00011000);	//Set scale FS bits to 01 and enable high-resolution. 01 gives range +-4g
	TWIStop();
}

int16_t accelXRead()
{
	uint8_t xh=0;
	uint8_t xl=0;
	int16_t x=0;
	
	TWIStart();
	TWIWrite(ACCEL_WRITE);
	TWIWrite(ACCEL_X_AXIS_OUT_LOW);
	TWIStart();
	TWIWrite(ACCEL_READ);
	xl = TWIRead();
	TWIStop();
	
	TWIStart();
	TWIWrite(ACCEL_WRITE);
	TWIWrite(ACCEL_X_AXIS_OUT_HIGH);
	TWIStart();
	TWIWrite(ACCEL_READ);
	xh = TWIRead();
	TWIStop();
	
	x = ((int16_t)(xh<<8 | xl))/16;	//The datasheet lies... the device does not output 16-bit but only 12-bit left-adjusted. Thus we need to shift it to the right 4 bits. The product description says its 12-bit! In order not to right-shift a signed variable, which is implementation-defined, we divide by 2^4 which will yield the same and I trust the compiler will optimize for us as a right-shift.
	
	return x*ACCEL_SCALE;
}

int16_t accelYRead()
{
	uint8_t yh=0;
	uint8_t yl=0;
	int16_t y=0;
	
	TWIStart();
	TWIWrite(ACCEL_WRITE);
	TWIWrite(ACCEL_Y_AXIS_OUT_LOW);
	TWIStart();
	TWIWrite(ACCEL_READ);
	yl = TWIRead();
	TWIStop();

	TWIStart();
	TWIWrite(ACCEL_WRITE);
	TWIWrite(ACCEL_Y_AXIS_OUT_HIGH);
	TWIStart();
	TWIWrite(ACCEL_READ);
	yh = TWIRead();
	TWIStop();
	
	y = ((int16_t)(yh<<8 | yl))/16;
	
	return y*ACCEL_SCALE;
}

int16_t accelZRead()
{
	uint8_t zh=0;
	uint8_t zl=0;
	int16_t z=0;
	
	TWIStart();
	TWIWrite(ACCEL_WRITE);
	TWIWrite(ACCEL_Z_AXIS_OUT_LOW);
	TWIStart();
	TWIWrite(ACCEL_READ);
	zl = TWIRead();
	TWIStop();

	TWIStart();
	TWIWrite(ACCEL_WRITE);
	TWIWrite(ACCEL_Z_AXIS_OUT_HIGH);
	TWIStart();
	TWIWrite(ACCEL_READ);
	zh = TWIRead();
	TWIStop();
	
	z = ((int16_t)(zh<<8 | zl))/16;
	
	return z*ACCEL_SCALE;
}

void magInit(void)
{
	TWIStart();
	TWIWrite(MAG_WRITE);
	TWIWrite(CRA_REG_M);
	TWIWrite(0b10011000); //Update at 75Hz
	TWIStop();
	
	TWIStart();
	TWIWrite(MAG_WRITE);
	TWIWrite(CRB_REG_M);
	TWIWrite(0b00100000); //Gain
	TWIStop();
}

float magXRead()
{
	uint8_t xh=0;
	uint8_t xl=0;
	int16_t x=0;
	
	TWIStart();
	TWIWrite(MAG_WRITE);
	TWIWrite(MR_REG_M);
	TWIWrite(0x01);
	TWIStop();
	
	_delay_ms(10);
	
	TWIStart();
	TWIWrite(MAG_WRITE);
	TWIWrite(MAG_X_AXIS_OUT_HIGH);
	TWIStart();
	TWIWrite(MAG_READ);
	xh = TWIRead();
	TWIStop();	
	
	TWIStart();
	TWIWrite(MAG_WRITE);
	TWIWrite(MAG_X_AXIS_OUT_LOW);
	TWIStart();
	TWIWrite(MAG_READ);
	xl = TWIRead();
	TWIStop();
	
	x = (int16_t)(xh<<8 | xl);
	
	return x/MAG_SCALE_XY;
}

float magYRead()
{
	uint8_t yh=0;
	uint8_t yl=0;
	int16_t y=0;

	TWIStart();
	TWIWrite(MAG_WRITE);
	TWIWrite(MR_REG_M);
	TWIWrite(0x01);
	TWIStop();
	
	_delay_ms(10);
	
	TWIStart();
	TWIWrite(MAG_WRITE);
	TWIWrite(MAG_Y_AXIS_OUT_HIGH);
	TWIStart();
	TWIWrite(MAG_READ);
	yh = TWIRead();
	TWIStop();

	TWIStart();
	TWIWrite(MAG_WRITE);
	TWIWrite(MAG_Y_AXIS_OUT_LOW);
	TWIStart();
	TWIWrite(MAG_READ);
	yl = TWIRead();
	TWIStop();	
	
	y = (int16_t)(yh<<8 | yl);
	
	return y/MAG_SCALE_XY;
}

float magZRead()
{
	uint8_t zh=0;
	uint8_t zl=0;
	int16_t z=0;

	TWIStart();
	TWIWrite(MAG_WRITE);
	TWIWrite(MR_REG_M);
	TWIWrite(0x01);
	TWIStop();
	
	_delay_ms(10);

	TWIStart();
	TWIWrite(MAG_WRITE);
	TWIWrite(MAG_Z_AXIS_OUT_HIGH);
	TWIStart();
	TWIWrite(MAG_READ);
	zh = TWIRead();
	TWIStop();
	
	TWIStart();
	TWIWrite(MAG_WRITE);
	TWIWrite(MAG_Z_AXIS_OUT_LOW);
	TWIStart();
	TWIWrite(MAG_READ);
	zl = TWIRead();
	TWIStop();	
	
	z = (int16_t)(zh<<8 | zl);
	
	return z/MAG_SCALE_Z;
}

float IMUtempRead()
{
	uint8_t h=0;
	uint8_t l=0;
	int16_t tmp=0;
	
	TWIStart();
	TWIWrite(MAG_WRITE);
	TWIWrite(MR_REG_M);
	TWIWrite(0x01);
	TWIStop();

	TWIStart();
	TWIWrite(MAG_WRITE);
	TWIWrite(IMU_TMP_HIGH);
	TWIStart();
	TWIWrite(MAG_READ);
	h = TWIRead();
	TWIStop();

	TWIStart();
	TWIWrite(MAG_WRITE);
	TWIWrite(IMU_TMP_LOW);
	TWIStart();
	TWIWrite(MAG_READ);
	l = TWIRead();
	TWIStop();
	
	tmp = ((int16_t)((h<<8 | l)))/16;
	
	return tmp/IMU_TMP_SCALE + 16.25;
}


void gyroInit(void)
{
	TWIStart();
	TWIWrite(GYRO_WRITE);
	TWIWrite(GYYRO_CTRL_REG_1);
	TWIWrite(0b00101111);	//Set data output rate to 95Hz and power-up
	TWIStop();
	// We leave the scaling such that the range is from 0 to 250dps
}

float gyroXRead() 
{
	uint8_t xh=0;
	uint8_t xl=0;
	int16_t x=0;
	
	TWIStart();
	TWIWrite(GYRO_WRITE);
	TWIWrite(GYRO_X_AXIS_OUT_LOW);
	TWIStart();
	TWIWrite(GYRO_READ);
	xl = TWIRead();
	TWIStop();

	TWIStart();
	TWIWrite(GYRO_WRITE);
	TWIWrite(GYRO_X_AXIS_OUT_HIGH);
	TWIStart();
	TWIWrite(GYRO_READ);
	xh = TWIRead();
	TWIStop();
	
	x = (int16_t)(xh<<8 | xl);
	
	return x*GYRO_SCALE;
}

float gyroYRead()
{
	uint8_t yh=0;
	uint8_t yl=0;
	int16_t y=0;
	
	TWIStart();
	TWIWrite(GYRO_WRITE);
	TWIWrite(GYRO_Y_AXIS_OUT_LOW);
	TWIStart();
	TWIWrite(GYRO_READ);
	yl = TWIRead();
	TWIStop();

	TWIStart();
	TWIWrite(GYRO_WRITE);
	TWIWrite(GYRO_Y_AXIS_OUT_HIGH);
	TWIStart();
	TWIWrite(GYRO_READ);
	yh = TWIRead();
	TWIStop();
	
	y = (int16_t)(yh<<8 | yl);
	
	return y*GYRO_SCALE;
}


float gyroZRead() 
{
	uint8_t zh=0;
	uint8_t zl=0;
	int16_t z=0;
	
	TWIStart();
	TWIWrite(GYRO_WRITE);
	TWIWrite(GYRO_Z_AXIS_OUT_LOW);
	TWIStart();
	TWIWrite(GYRO_READ);
	zl = TWIRead();
	TWIStop();
	
	TWIStart();
	TWIWrite(GYRO_WRITE);
	TWIWrite(GYRO_Z_AXIS_OUT_HIGH);
	TWIStart();
	TWIWrite(GYRO_READ);
	zh = TWIRead();
	TWIStop();
	
	z = (int16_t)(zh<<8 | zl);
	
	return z*GYRO_SCALE;
}

void baroInit(void)
{
	TWIStart();
	TWIWrite(BARO_WRITE); 
	TWIWrite(BARO_CTRL_REG1); 
	TWIWrite(0b11100000);  //Turn-on device and set to continuous mode 12.5Hz
	TWIStop();
	
	
	TWIStart();
	TWIWrite(BARO_WRITE);
	TWIWrite(BARO_RES_CONF);
	TWIWrite(0x7A); //512 average
	TWIStop();
}


float baroRead()
{
	uint8_t xl = 0;
	uint8_t  l = 0;
	uint8_t  h = 0;
	uint32_t pressure=0;
	
	TWIStart();
	TWIWrite(BARO_WRITE);
	TWIWrite(BARO_PRESS_OUT_XL);
	TWIStart();
	TWIWrite(BARO_READ);
	xl = TWIRead();
	TWIStop();

	TWIStart();
	TWIWrite(BARO_WRITE);
	TWIWrite(BARO_PRESS_OUT_L);
	TWIStart();
	TWIWrite(BARO_READ);
	l = TWIRead();
	TWIStop();
	
	TWIStart();
	TWIWrite(BARO_WRITE);
	TWIWrite(BARO_PRESS_OUT_H);
	TWIStart();
	TWIWrite(BARO_READ);
	h = TWIRead();
	TWIStop();
	
	pressure = (uint32_t)h<<16 | (uint32_t)l<<8 | (uint32_t)xl;
	
	return pressure/BARO_SCALE;
}