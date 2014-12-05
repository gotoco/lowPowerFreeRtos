/*
 * TEM.cpp
 *
 *  Created on: 4 gru 2014
 *      Author: Adrian
 */

#include "stm32l1xx.h"
#include "i2c.h"
#include "MCP980x\MCP980x.h"

/*---------------------------------------------------------------------------------------------------------------------+
| global functions
+---------------------------------------------------------------------------------------------------------------------*/

/**
 * \brief	Initialises MCP980x temperature sensor for shutdown mode. Writes 1 on bit 0 in CONFIG register.
 */
void MCP980x_Init(void)
{
	i2cInitialize();	// i2c interface init

	uint8_t tab[]={TEM_CONFIG_REG, 0b00000001};

	i2cWrite(TEM_SLAVE_ADDRESS, tab, 2);	// shoot down mode
}

/**
 * \brief	Doing a single measure by setting bit7 (ONE SHOT) at the CONFIG register. Then waiting for clear this bit,
 * 			reading two bytes from TA register and converts it to float.
 *
 * \return	Value of actual temperature in float.
 */
float MCP980x_Single_Measure()
{
	uint8_t tab[]={TEM_CONFIG_REG, 0b10000001};
	uint8_t config_reg;

	// writing SourceValue to perpih
	i2cWrite(TEM_SLAVE_ADDRESS, tab, 2);

	// setting the address of register to read
	i2cWriteOneByteWhithoutStop(TEM_SLAVE_ADDRESS, TEM_CONFIG_REG);
	// reading data from the register setted address
	i2cRead(TEM_SLAVE_ADDRESS, &config_reg, 1);
	// wait while one shot bit is not cleared
	while(config_reg & 128){
		i2cRead(TEM_SLAVE_ADDRESS, &config_reg, 1);
	}


	// setting the address of TA register
	i2cWriteOneByteWhithoutStop(TEM_SLAVE_ADDRESS, TEM_TA_REG);
	// reading data from TA register
	i2cRead(TEM_SLAVE_ADDRESS, tab, 2);


	return MCP980x_Convert(tab);
}

/**
 * \brief	Converts two bytes: MSB and LSB from TA register to one float.
 *
 * \param	Pointer to table which contains MSB and LSB value of TA register.
 * \return	Actual temperature in float.
 */
static float MCP980x_Convert(uint8_t* wsk)
{
	int8_t MSB= (int8_t)*wsk;
	wsk++;
	float LSB=0;

	if((MSB >> 7)==1) MSB*=(-1);

	if(*wsk & 128) LSB+=0.5;
	if(*wsk & 64) LSB+=0.25;
	if(*wsk & 32) LSB+=0.125;
	if(*wsk & 16) LSB+=0.0625;

	return (float)MSB + LSB;
}


