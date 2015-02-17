/*
 * SD.cpp
 *
 *  Created on: 12 lut 2015
 *      Author: Adrian
 */

#include <cstdint>
#include <cstddef>

#include "SD.h"

#include "inc/stm32l1xx.h"
#include "hdr/hdr_spi.h"
#include "gpio.h"
#include "config.h"

SD_Error SD_Init()
{
	// Configuring GPIO
	gpioConfigurePin(SD_MISO_GPIO, SD_MISO_PIN, SD_MISO_CONFIGURATION);
	gpioConfigurePin(SD_MOSI_GPIO, SD_MOSI_PIN, SD_MOSI_CONFIGURATION);
	gpioConfigurePin(SD_SCK_GPIO, SD_SCK_PIN, SD_SCK_CONFIGURATION);
	gpioConfigurePin(SD_CS_GPIO, SD_CS_PIN, SD_CS_CONFIGURATION);

	// Configuring SPI
	RCC_APBxENR_SD_SPIxEN_bb = 1;
	SD_SPI->CR1 = SPI_CR1_SSM | SPI_CR1_SSI | SPI_CR1_SPE | SPI_CR1_MSTR | SPI_CR1_CPOL | SPI_CR1_CPHA;	// software slave management, enable SPI, master mode
	SD_SPI->CRCPR = 7;

	return (SD_GoIdleState());
}

void SD_WriteByte(uint8_t Data)
{
	//while (!SPIx_SR_TXE_bb(SD_SPI));
	SD_SPI->DR = Data;
	while (!SD_SPI_SR_RXNE_bb);
	Data = SD_SPI->DR;
}

uint8_t SD_ReadByte()
{
	//while (!SPIx_SR_TXE_bb(SD_SPI));
	SD_SPI->DR = SD_DUMMY_BYTE;
	while (!SPIx_SR_RXNE_bb(SD_SPI));
	return (uint8_t)(SD_SPI->DR);
}

void SD_SendCommand(uint8_t Cmd, uint32_t Arg, uint8_t Crc)
{
	uint8_t Frame[6];

	Frame[0] = (Cmd | 0x40); /*!< Construct byte 1 */
	Frame[1] = (uint8_t)(Arg >> 24); /*!< Construct byte 2 */
	Frame[2] = (uint8_t)(Arg >> 16); /*!< Construct byte 3 */
	Frame[3] = (uint8_t)(Arg >> 8); /*!< Construct byte 4 */
	Frame[4] = (uint8_t)(Arg); /*!< Construct byte 5 */
	Frame[5] = (Crc); /*!< Construct CRC: byte 6 */

	for (uint8_t i = 0; i < 6; i++)
	{
		SD_WriteByte(Frame[i]); /*!< Send the Cmd bytes */
	}
}

SD_Error SD_GetResponse(uint8_t Response)
{
	uint16_t Count = 0xFFF;

	/*!< Check if response is got or a timeout is happen */
	while ((SD_ReadByte() != Response) && Count) Count--;

	/*!< After time out */
	if (Count == 0) return SD_RESPONSE_FAILURE;

	/*!< Right response got */
	else return SD_RESPONSE_NO_ERROR;
}

uint8_t SD_GetDataResponse(void)
{
	uint32_t i = 0;
	uint8_t response, rvalue;

	while (i <= 64)
	{
		/*!< Read resonse */
		response = SD_ReadByte();
		/*!< Mask unused bits */
		response &= 0x1F;
		switch (response)
		{
			case SD_DATA_OK:
			{
				rvalue = SD_DATA_OK;
				break;
			}
			case SD_DATA_CRC_ERROR:
				return SD_DATA_CRC_ERROR;
			case SD_DATA_WRITE_ERROR:
				return SD_DATA_WRITE_ERROR;
			default:
			{
				rvalue = SD_DATA_OTHER_ERROR;
				break;
			}
		}
		/*!< Exit loop in case of data ok */
		if (rvalue == SD_DATA_OK) break;

		/*!< Increment loop counter */
		i++;
  }

  /*!< Wait null data */
  while (SD_ReadByte() == 0);

  /*!< Return response */
  return response;
}

SD_Error SD_GoIdleState()
{
	SD_CS_bb=0;
	SD_SendCommand(SD_CMD_GO_IDLE_STATE, 0, 0x95);

	/*!< Wait for In Idle State Response (R1 Format) equal to 0x01 */
	if (SD_GetResponse(SD_IN_IDLE_STATE))
	{
		/*!< No Idle State Response: return response failue */
		return SD_RESPONSE_FAILURE;
	}

	/*----------Activates the card initialization process-----------*/
	do
	{
		SD_CS_bb=1;
	    SD_WriteByte(SD_DUMMY_BYTE);
	    SD_CS_bb=0;

	    /*!< Send CMD1 (Activates the card process) until response equal to 0x0 */
	    SD_SendCommand(SD_CMD_SEND_OP_COND, 0, 0xFF);
	    /*!< Wait for no error Response (R1 Format) equal to 0x00 */
	}
	while (SD_GetResponse(SD_RESPONSE_NO_ERROR));

	SD_CS_bb=1;
	SD_WriteByte(SD_DUMMY_BYTE);

	return SD_RESPONSE_NO_ERROR;
}

SD_Error SD_WriteBlock(uint8_t* pBuffer, uint32_t WriteAddr, uint16_t BlockSize)
{
	uint32_t i = 0;
	SD_Error rvalue = SD_RESPONSE_FAILURE;

	SD_CS_bb=0;

	/*!< Send CMD24 (SD_CMD_WRITE_SINGLE_BLOCK) to write multiple block */
	SD_SendCommand(SD_CMD_WRITE_SINGLE_BLOCK, WriteAddr, 0xFF);

	/*!< Check if the SD acknowledged the write block command: R1 response (0x00: no errors) */
	if (!SD_GetResponse(SD_RESPONSE_NO_ERROR))
	{
		SD_WriteByte(SD_DUMMY_BYTE);

		/*!< Send the data token to signify the start of the data */
		SD_WriteByte(0xFE);

		/*!< Write the block data to SD : write count data by block */
		for (i = 0; i < BlockSize; i++)
		{
			/*!< Send the pointed byte */
			SD_WriteByte(*pBuffer);
			/*!< Point to the next location where the byte read will be saved */
			pBuffer++;
		}
		/*!< Put CRC bytes (not really needed by us, but required by SD) */
		SD_ReadByte();
		SD_ReadByte();

		/*!< Read data response */
		if (SD_GetDataResponse() == SD_DATA_OK)
		{
			rvalue = SD_RESPONSE_NO_ERROR;
		}
	}
	SD_CS_bb=1;
	SD_WriteByte(SD_DUMMY_BYTE);

	return rvalue;
}

SD_Error SD_ReadBlock(uint8_t* pBuffer, uint32_t ReadAddr, uint16_t BlockSize)
{
	uint32_t i = 0;
	SD_Error rvalue = SD_RESPONSE_FAILURE;

	SD_CS_bb=0;

	/*!< Send CMD17 (SD_CMD_READ_SINGLE_BLOCK) to read one block */
	SD_SendCommand(SD_CMD_READ_SINGLE_BLOCK, ReadAddr, 0xFF);

	/*!< Check if the SD acknowledged the read block command: R1 response (0x00: no errors) */
	if (!SD_GetResponse(SD_RESPONSE_NO_ERROR))
	{
		/*!< Now look for the data token to signify the start of the data */
		if (!SD_GetResponse(SD_START_DATA_SINGLE_BLOCK_READ))
		{
			/*!< Read the SD block data : read NumByteToRead data */
			for (i = 0; i < BlockSize; i++)
			{
				/*!< Save the received data */
				*pBuffer = SD_ReadByte();

				/*!< Point to the next location where the byte read will be saved */
				pBuffer++;
			}
			/*!< Get CRC bytes (not really needed by us, but required by SD) */
			SD_ReadByte();
			SD_ReadByte();
			/*!< Set response value to success */
			rvalue = SD_RESPONSE_NO_ERROR;
		}
	}
	SD_CS_bb=1;
	SD_WriteByte(SD_DUMMY_BYTE);

	return rvalue;
}





