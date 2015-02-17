/*
 * FC30.cpp
 *
 *  Created on: 28 lis 2014
 *      Author: Adrian
 */

#include "stm32l1xx.h"
#include "i2c.h"
#include "FC30.h"


/*---------------------------------------------------------------------------------------------------------------------+
| global functions' declarations
+---------------------------------------------------------------------------------------------------------------------*/

void FC30_Read(uint8_t SlaveAddress, uint8_t SubAddress, uint8_t* DestinationValue)
{
	// setting address to read
	i2cWriteOneByteWhithoutStop(SlaveAddress, SubAddress);

	// reading a value from setted address
	//i2cReadOneByte(SlaveAddress, DestinationValue);
}

void FC30_Write(uint8_t SlaveAddress, uint8_t SubAddress, uint8_t SourceValue)
{
	uint8_t tab[]={SubAddress, SourceValue};

	// writing SourceValue to perpih
	i2cWrite(SlaveAddress, tab, 2);
}

void FC30_Init()
{
	uint8_t Value_1;

	i2cInitialize();

	FC30_Read(0x5D, 0x16,&Value_1);
	FC30_Write(0x5D, 0x16, Value_1 & 0x1F);
	FC30_Write(0x1D, 0x3B, 0x77);
	FC30_Write(0x1D, 0x3C, 0x07);
	FC30_Write(0x1D, 0x3D, 0xFE);
	FC30_Write(0x1D, 0x3E, 0x10);
	FC30_Write(0x1D, 0x3F, 0xFF);
	FC30_Write(0x1D, 0x38, 0x15);
	FC30_Write(0x1D, 0x20, 0x47);
	FC30_Write(0x1D, 0x22, 0x07);
}



