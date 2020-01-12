/*
  glcd_io_i2c.h - openGLCD library support
  Copyright (c) 2020 PavelK (http://pavelk.ru)
  
  vi:ts=1

 ------------------------------------------------------------------------

  This file is not a part of the openGLCD library project

  openGLCD is free software: you can redistribute it and/or modify
  it under the terms of version 3 of the GNU General Public License
  as published by the Free Software Foundation.
 
  openGLCD is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with openGLCD.  If not, see <http://www.gnu.org/licenses/>.

 ------------------------------------------------------------------------

  This file maps abstract io requests from glcd_Device.
   
 */
 
#ifndef	GLCD_IO_I2C_H
#define GLCD_IO_I2C_H

#include <Wire.h>

#ifndef OUTPUT
	#define OUTPUT 1
#endif

#ifndef LOW
	#define LOW 0
#endif

#ifndef HIGH
	#define HIGH 1
#endif


static uint8_t glcdio_i2c_convertors[GLCDCFG_CONVERTORS_COUNT] =  GLCDCFG_CONVERTORS;
static uint8_t glcdio_i2c_port[GLCDCFG_CONVERTORS_COUNT] = {0}; //-- Cache port values for devices


static void glcdio_i2c_init()
{
	Wire.begin();
	Wire.setClock(380000);
}

/**
* @brief Convert pin number to device id
*/
static uint8_t glcdio_i2c_pin2convertor(uint8_t &pin)
{
	uint8_t deviceID = 0;
	
	if ( pin>=20 ) {
		pin -= 20;
		deviceID = 1;
	} else	
	if ( pin>=10 ) {
		pin -= 10;
		deviceID = 0;
	}
	
	return deviceID;
}

/**
* @brief Read byte from device
* @param dID device ID
*/
static uint8_t glcdio_i2cReadByte(uint8_t dID)
{
	uint8_t address = glcdio_i2c_convertors[dID];
	
	Wire.requestFrom(address, (uint8_t)0x01);
	while ( Wire.available()<1 );
	uint8_t val = Wire.read();
	
	glcdio_i2c_port[dID] = val;
	return val;
}

static void glcdio_i2cWriteByte(uint8_t dID, uint8_t val)
{
	uint8_t address = glcdio_i2c_convertors[dID];
	glcdio_i2c_port[dID] = val;
	
	Wire.beginTransmission(address);
	Wire.write(val);
	Wire.endTransmission();
}

static uint8_t glcdio_i2cReadPin(uint8_t pin)
{
	uint8_t dID = glcdio_i2c_pin2convertor(pin);
	
	glcdio_i2cReadByte(dID);
		
	return (glcdio_i2c_port[dID]&(1<< pin))? HIGH : LOW;
}

static void glcdio_i2cWritePin(uint8_t pin, uint8_t val)
{
	uint8_t dID = glcdio_i2c_pin2convertor(pin);
	
	uint8_t b = glcdio_i2c_port[dID];
	
	if ( val ) {
		b |= (1 << pin);
	} else {
		b &= ~(1 << pin);
	}
	
	glcdio_i2cWriteByte(dID, b);
}

static void glcdio_i2cPinMode(uint8_t pin, uint8_t mode)
{
	switch (mode) {
		case INPUT:
		case INPUT_PULLUP:
			glcdio_i2cWritePin(pin, 1);
			break;
		case OUTPUT:		
			glcdio_i2cWritePin(pin, 0);
			break;		
	}
}

#define glcdio_ReadPin(pin)				glcdio_i2cReadPin(pin)
#define glcdio_WritePin(pin, pinval)	glcdio_i2cWritePin(pin, pinval)
#define glcdio_PinMode(pin, mode)		glcdio_i2cPinMode(pin, mode)


#define glcdio_WriteByte(data) glcdio_i2cWriteByte(0, data)
#define glcdio_ReadByte() glcdio_i2cReadByte(0)

#define	glcdio_DataDirOut() glcdio_i2cWriteByte(0, 0x00)
#define	glcdio_DataDirIn(_ENpullups) glcdio_i2cWriteByte(0, 0xff)
	



#endif //GLCD_IO_I2C_H