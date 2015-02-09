/*
 * LIS35DE.cpp
 *
 *  Created on: 22 gru 2014
 *      Author: Adrian
 */

#include "spi.h"
#include "i2c.h"
#include "gpio.h"
#include "config.h"
#include "LIS35DE.h"

/*---------------------------------------------------------------------------------------------------------------------+
| global functions' definitions
+---------------------------------------------------------------------------------------------------------------------*/

uint8_t LIS35DE_Init()
{
	if(COMMAND_LIS35DE_I2C)
	{
		return LIS35DE_I2C_Init();
	}
	else if(COMMAND_LIS35DE_SPI)
	{
		return LIS35DE_SPI_Init();
	}
	return LIS35DE_INITIALIZE_ERROR;
}

void LIS35DE_Read(int8_t *x, int8_t *y, int8_t *z)
{
	if(COMMAND_LIS35DE_I2C)
	{
		LIS35DE_I2C_Read(x, y, z);
	}
	else if(COMMAND_LIS35DE_SPI)
	{
		LIS35DE_SPI_Read(x, y, z);
	}
}

/*---------------------------------------------------------------------------------------------------------------------+
| private functions' definitions
+---------------------------------------------------------------------------------------------------------------------*/

static void LIS35DE_SPI_Reboot()
{
	uint8_t tx[2];

	tx[0] = LIS35DE_SPI_WRITE | LIS35DE_SPI_ADDRESS_UNCHAGED | LIS35DE_CTRL_REG2;   // 33
	tx[1] = LIS35DE_CTRL_REG2_BOOT;   // 64

	// Writing 0 to CS pin - starting transmission
	SPIx_SSB_bb = SPIx_SSB_START;

	spiWrite(tx,2);

	// Writing 1 to CS pin - transmission stopped
	SPIx_SSB_bb = SPIx_SSB_END;
}

static uint8_t LIS35DE_SPI_Init()
{
	uint8_t tx[2];

	spiInitialize();

	spiSetBaudRate(SPIx_BOUDRATE);

	gpioConfigurePin(SPIx_SSB_GPIO, SPIx_SSB_PIN, SPIx_SSB_CONFIGURATION);   // Configuring CS pin

	for(int i=0;i<1000000;i++);  // waiting for SPI initialize end

	SPIx_SSB_bb = SPIx_SSB_START;  // sending random byte, because first byte can't be sent properly
	tx[0]=123;
	spiWrite(tx,1);
	SPIx_SSB_bb = SPIx_SSB_END;

	LIS35DE_SPI_Reboot();  // Calibrating accelerometer to fabric settings

	for(int i=0;i<1000000;i++);  // waiting for accelerometer reset

	SPIx_SSB_bb = SPIx_SSB_START;  // Writing 0 to CS pin - starting transmission

	tx[0] = LIS35DE_SPI_WRITE | LIS35DE_SPI_ADDRESS_UNCHAGED | LIS35DE_CTRL_REG1;  // 32
	tx[1] = LIS35DE_CTRL_REG1_PD | LIS35DE_CTRL_REG1_ZEN | LIS35DE_CTRL_REG1_YEN | LIS35DE_CTRL_REG1_XEN;   // 71

	spiWrite(tx,1);
	spiWrite(&tx[1],1);

	SPIx_SSB_bb = SPIx_SSB_END;  // Writing 1 to CS pin - transmission stopped

	// Checking whether power is set
	tx[0] = LIS35DE_SPI_READ | LIS35DE_SPI_ADDRESS_UNCHAGED | LIS35DE_CTRL_REG1;  // 160

	SPIx_SSB_bb = SPIx_SSB_START;

	spiWrite(tx,1);
	spiRead(tx,1);

	SPIx_SSB_bb = SPIx_SSB_END;

	if(tx[0]==tx[1]) return 0;
	else return 1;
}

static void LIS35DE_SPI_Read(int8_t *x, int8_t *y, int8_t *z)
{
	SPIx_SSB_bb = SPIx_SSB_START;

	uint8_t tx = LIS35DE_SPI_READ | LIS35DE_SPI_ADDRESS_INCREMENTED | LIS35DE_OUT_X;  // 233

	spiWrite(&tx,1);

	spiRead(&tx,1);  // reading x
	*x=(int8_t)tx;

	spiRead(&tx,1);

	spiRead(&tx,1);	 // reading y
	*y=(int8_t)tx;

	spiRead(&tx,1);

	spiRead(&tx,1);  // reading z
	*z=(int8_t)tx;

	SPIx_SSB_bb = SPIx_SSB_END;
}

static uint8_t LIS35DE_I2C_Init()
{
	i2cInitialize();

	LIS35DE_I2C_Reboot();

	uint8_t tab[2];
	tab[0] = LIS35DE_CTRL_REG1 | LIS35DE_I2C_SINGLE_BYTE;  // 32
	tab[1] = LIS35DE_CTRL_REG1_PD | LIS35DE_CTRL_REG1_ZEN | LIS35DE_CTRL_REG1_YEN | LIS35DE_CTRL_REG1_XEN;   // 71

	i2cWrite(LIS35DE_I2C_SLAVE_ADDRESS, tab, 2);  // 56

	i2cWriteOneByteWhithoutStop(LIS35DE_I2C_SLAVE_ADDRESS, LIS35DE_CTRL_REG1 | LIS35DE_I2C_SINGLE_BYTE);  // 56,32
	i2cRead(LIS35DE_I2C_SLAVE_ADDRESS, tab, 1);  // 57

	if(tab[0]==tab[1]) return 0;
	return 1;
}

static void LIS35DE_I2C_Reboot()
{
	uint8_t tab[2];

	tab[0] = LIS35DE_CTRL_REG2 | LIS35DE_I2C_SINGLE_BYTE;   // 33
	tab[1] = LIS35DE_CTRL_REG2_BOOT;   // 64

	i2cWrite(LIS35DE_I2C_SLAVE_ADDRESS, tab, 2);
}

static void LIS35DE_I2C_Read(int8_t *x, int8_t *y, int8_t *z)
{
	uint8_t rx[5];

	i2cWriteOneByteWhithoutStop(LIS35DE_I2C_SLAVE_ADDRESS, LIS35DE_OUT_X | LIS35DE_I2C_MULTIPLE_BYTES);  // 56,169
	i2cRead(LIS35DE_I2C_SLAVE_ADDRESS, rx, 5);  // 57

	// reading x
	*x=(int8_t)rx[0];

	// reading y
	*y=(int8_t)rx[2];

	// reading z
	*z=(int8_t)rx[4];
}


