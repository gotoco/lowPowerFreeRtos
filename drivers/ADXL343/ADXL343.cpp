/*
 * ADXL343.cpp
 *
 *  Created on: 13 sty 2015
 *      Author: Adrian
 */

#include "spi.h"
#include "gpio.h"
#include "config.h"
#include "ADXL343.h"

void ADXL343_Init(void)
{
	spiInitialize();

	spiSetBaudRate(SPIx_BOUDRATE);

	gpioConfigurePin(SPIx_SSB_GPIO, SPIx_SSB_PIN, SPIx_SSB_CONFIGURATION);   // Configuring CS pin

	for(int i=0;i<1000000;i++);  // waiting for SPI initialize end

	uint8_t tx[2];

	SPIx_SSB_bb = SPIx_SSB_START;  // sending random byte, because first byte can't be sent properly
	tx[0]=123;
	spiWrite(tx,1);
	SPIx_SSB_bb = SPIx_SSB_END;


	tx[0] = ADXL343_READ | ADXL343_SINGLE_BYTE | ADXL343_DEV_ID;

	SPIx_SSB_bb = SPIx_SSB_START;
	spiWrite(tx,1);
	spiRead(tx,1);
	SPIx_SSB_bb = SPIx_SSB_END;

	tx[0] = ADXL343_WRITE | ADXL343_SINGLE_BYTE | ADXL343_POWER_CTL;
	tx[1] = ADXL343_POWER_CTL_MEASURE;

	SPIx_SSB_bb = SPIx_SSB_START;
	spiWrite(tx,2);
	SPIx_SSB_bb = SPIx_SSB_END;

	tx[0] = ADXL343_READ | ADXL343_SINGLE_BYTE | ADXL343_POWER_CTL;
	SPIx_SSB_bb = SPIx_SSB_START;
	spiWrite(tx,1);
	spiRead(tx,1);
	SPIx_SSB_bb = SPIx_SSB_END;
}

void ADXL343_Read(int16_t *x, int16_t *y, int16_t *z)
{
	uint8_t bufor[2];

	bufor[0] =	ADXL343_READ | ADXL343_MULTIPLE_BYTE | ADXL343_DATAX0;

	SPIx_SSB_bb = SPIx_SSB_START;
	spiWrite(bufor,1);
	spiRead(bufor,2);
	SPIx_SSB_bb = SPIx_SSB_END;

	*x = ( (int16_t)((bufor[1]) << 8) | (int16_t)bufor[0]);


	while(1);
}


