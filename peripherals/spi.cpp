/**
 * \file spi.cpp
 * \brief SPI driver
 *
 * Functions for SPI control
 *
 * chip: STM32L1xx; prefix: spi
 *
 * \author grochu
 * \date 2012-08-30
 */

#include <cstdint>
#include <cstddef>

#include "inc/stm32l1xx.h"

#include "hdr/hdr_rcc.h"
#include "hdr/hdr_spi.h"
#include "hdr/hdr_dma.h"

#include "config.h"

#include "portmacro.h"
#include "gpio.h"
#include "rcc.h"
#include "spi.h"
#include "FreeRTOS.h"
#include "queue.h"
#include "semphr.h"

/*---------------------------------------------------------------------------------------------------------------------+
 | local functions' declarations
 +---------------------------------------------------------------------------------------------------------------------*/

static void _spiRxTask(void *parameters);
static void _spiTxTask(void *parameters);

/*---------------------------------------------------------------------------------------------------------------------+
 | local defines
 +---------------------------------------------------------------------------------------------------------------------*/

//#define _INPUT_BUFFER_SIZE					128
//#define _OUTPUT_BUFFER_SIZE					512

/*---------------------------------------------------------------------------------------------------------------------+
 | local variables
 +---------------------------------------------------------------------------------------------------------------------*/

extern char __ram_start[];					// imported from linker script

static xQueueHandle _spiRxQueue;
static xQueueHandle _spiTxQueue;
static xSemaphoreHandle _spiDmaTxSemaphore;

//static char _inputBuffer[_INPUT_BUFFER_SIZE];
//static char _outputBuffer[_OUTPUT_BUFFER_SIZE];

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
	// Configuring GPIO
	gpioConfigurePin(SPIx_MISO_GPIO, SPIx_MISO_PIN, SPIx_MISO_CONFIGURATION);
	gpioConfigurePin(SPIx_MOSI_GPIO, SPIx_MOSI_PIN, SPIx_MOSI_CONFIGURATION);
	gpioConfigurePin(SPIx_SCK_GPIO, SPIx_SCK_PIN, SPIx_SCK_CONFIGURATION);

	// Configuring SPI
	RCC_APBxENR_SPIxEN_bb = 1;
	SPIx->CR1 = SPI_CR1_SSM | SPI_CR1_SSI | SPI_CR1_SPE | SPI_CR1_MSTR;	// software slave management, enable SPI, master mode

	// DMA
	RCC_AHBENR_SPIx_DMAxEN_bb = 1;				// enable DMA

	NVIC_SetPriority(SPIx_DMAx_TX_CH_IRQn, SPIx_DMAx_TX_CH_IRQ_PRIORITY);	// set DMA IRQ priority
	NVIC_EnableIRQ(SPIx_DMAx_TX_CH_IRQn);				// enable DMA TX IRQ

	NVIC_SetPriority(SPIx_DMAx_RX_CH_IRQn, SPIx_DMAx_RX_CH_IRQ_PRIORITY);// set DMA IRQ priority
	NVIC_EnableIRQ(SPIx_DMAx_RX_CH_IRQn);	// enable DMA RX IRQ

	//vSemaphoreCreateBinary(_dmaTxSemaphore);

	_spiTxQueue = xQueueCreate(SPIx_TX_QUEUE_LENGTH, 8);
	_spiRxQueue = xQueueCreate(SPIx_RX_QUEUE_LENGTH, 8);
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

void spiSend(uint8_t *tx, size_t length, portTickType ticks_to_wait)
{
	while(length--)
	{
		xQueueSend(_spiTxQueue, tx++, ticks_to_wait);
	}
}

static void _spiTxTask(void *parameters)
{
	(void) parameters;
	uint8_t tx;

	while(1)
	{
		xQueueReceive(_spiTxQueue, &tx, portMAX_DELAY);	// get data to send

		SPIx_DMAx_TX_CH->CCR = 0;				// disable channel
		SPIx_DMAx_TX_CH->CMAR = (uint32_t) tx;	// source
		SPIx_DMAx_TX_CH->CPAR = (uint32_t) & SPIx->DR;	// destination
		SPIx_DMAx_TX_CH->CNDTR = 1;	// length
		// low priority, 8-bit source and destination, memory increment mode, memory to peripheral, transfer complete
		// interrupt enable, enable channel
		USARTx_DMAx_TX_CH->CCR = DMA_CCR_PL_LOW | DMA_CCR_MSIZE_8
				| DMA_CCR_PSIZE_8 | DMA_CCR_MINC | DMA_CCR_DIR |
				DMA_CCR_TCIE | DMA_CCR_EN;
	}
}




