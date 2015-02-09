/**
 * \file spi.cpp
 * \brief SPI driver
 *
 * Functions for SPI control
 *
 * chip: STM32L1xx; prefix: spi
 *
 * \author: Mazeryt Freager
 * \date 2012-08-30
 */

#include <cstdint>
#include <cstddef>

#include "stm32l152xc.h"

#include "hdr/hdr_rcc.h"
#include "hdr/hdr_spi.h"

#include "config.h"

#include "gpio.h"
#include "rcc.h"
#include "spi.h"

/*---------------------------------------------------------------------------------------------------------------------+
| global functions
+---------------------------------------------------------------------------------------------------------------------*/

/**
 * \brief Initializes SPI.
 *
 * Initializes SPI. SPI clock is set to max value, it should be changed later with spiSetBaudRate().
 */

void spiInitialize(void)
{
	gpioConfigurePin(SPIx_MISO_GPIO, SPIx_MISO_PIN, SPIx_MISO_CONFIGURATION);
	gpioConfigurePin(SPIx_MOSI_GPIO, SPIx_MOSI_PIN, SPIx_MOSI_CONFIGURATION);
	gpioConfigurePin(SPIx_SCK_GPIO, SPIx_SCK_PIN, SPIx_SCK_CONFIGURATION);
	gpioConfigurePin(SPIx_SSB_GPIO, SPIx_SSB_PIN, SPIx_SSB_CONFIGURATION);

	SPIx_SSB_bb=SPIx_SSB_END;

	RCC_APBxENR_SPIxEN_bb = 1;

	SPIx->CR1 = SPI_CR1_SSM | SPI_CR1_SSI | SPI_CR1_SPE | SPI_CR1_MSTR;	// software slave management, enable SPI, master mode

	spiSetBaudRate(SPIx_BOUDRATE);
}

/**
 * \brief Sets SPI baud rate.
 *
 * Sets SPI baud rate.
 *
 * \param [in] baud_rate is desired baud rate in Hz
 *
 * \return real achieved baudrate
 */

uint32_t spiSetBaudRate(uint32_t baud_rate)
{
	uint32_t real_baud_rate = rccGetCoreFrequency() / 2;	// max baud rate is f_PCLK / 2
	uint32_t br = 0;

	while (real_baud_rate > baud_rate && br < 7)	// max br value is 7, so enter the loop only if br is lower
	{
		real_baud_rate /= 2;
		br++;
	}

	SPIx->CR1 = (SPIx->CR1 & ~SPI_CR1_BR) | (br << SPI_CR1_BR_bit);

	return real_baud_rate;
}

/**
 * \brief Transfers data through SPI.
 *
 * Bidirectional transfer of data through SPI (simultaneous tx and rx).
 *
 * \param [in] tx is the pointer to transferred data buffer, nullptr if blank bytes should be transferred (0xFF)
 * \param [out] rx is the pointer to received data buffer, nullptr if received data should be discarded
 * \param [in] length is the length of transfer in bytes
 *
 * \return transfer length in bytes, should be equal to parameter length
 */

size_t spiTransfer(const uint8_t *tx, uint8_t *rx, size_t length)
{
	size_t rx_length = 0;
	uint8_t tx_byte = 0xFF;

	while(length--)
	{
		if (tx != nullptr)					// should data be transfered?
			tx_byte = *tx++;				// yes - get another byte
		SPIx->DR = tx_byte;					// send data
		while (!SPIx_SR_RXNE_bb(SPIx));		// wait for transfer end
		uint8_t rx_byte = SPIx->DR;			// receive data
		if (rx != nullptr)					// should data be received?
			*rx++ = rx_byte;				// yes - store received byte
		rx_length++;						// increment counter
	}

	return rx_length;
}

void spiStart(void) {
	while(SPIx_SSB_bb != SPIx_SSB_START)
		SPIx_SSB_bb = SPIx_SSB_START;
}

void spiStop(void) {
	while(SPIx_SSB_bb != SPIx_SSB_END)
		SPIx_SSB_bb = SPIx_SSB_END;
}
