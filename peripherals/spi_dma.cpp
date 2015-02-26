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
#include "error.h"
#include "task.h"
#include "spi_dma.h"

void spiDmaInitialize(void)
{
	// Configuring GPIO
	gpioConfigurePin(SPIx_MISO_GPIO, SPIx_MISO_PIN, SPIx_MISO_CONFIGURATION);
	gpioConfigurePin(SPIx_MOSI_GPIO, SPIx_MOSI_PIN, SPIx_MOSI_CONFIGURATION);
	gpioConfigurePin(SPIx_SCK_GPIO, SPIx_SCK_PIN, SPIx_SCK_CONFIGURATION);
	gpioConfigurePin(SPIx_CS_GPIO, SPIx_CS_PIN, SPIx_CS_CONFIGURATION);

	// Configuring SPI
	RCC_APBxENR_SPIxEN_bb = 1;

	SPIx->CR1 |= SPI_CR1_SSM | SPI_CR1_SSI | SPI_CR1_SPE | SPI_CR1_MSTR;	// enable SPI, master mode
	SPIx->CR2 |= SPI_CR2_TXDMAEN | SPI_CR2_RXDMAEN;  	// enable Tx buffer DMA and Rx buffer DMA

	spiDmaStop();

	// DMA
	RCC_AHBENR_SPIx_DMAxEN_bb = 1;				// enable DMA

	// DMA interrupts
	NVIC_SetPriority(SPIx_DMAx_TX_CH_IRQn, SPIx_DMAx_TX_CH_IRQ_PRIORITY);	// set DMA IRQ priority
	NVIC_EnableIRQ(SPIx_DMAx_TX_CH_IRQn);				// enable DMA TX IRQ

	NVIC_SetPriority(SPIx_DMAx_RX_CH_IRQn, SPIx_DMAx_RX_CH_IRQ_PRIORITY);	// set DMA IRQ priority
	NVIC_EnableIRQ(SPIx_DMAx_RX_CH_IRQn);				// enable DMA RX IRQ

	/*vSemaphoreCreateBinary(_spiDmaTxSemaphore);
	//vSemaphoreCreateBinary(_spiDmaRxSemaphore);

	_spiTxQueue = xQueueCreate(SPIx_TX_QUEUE_LENGTH, sizeof(struct _spiTxMessage));
	_spiRxQueue = xQueueCreate(SPIx_RX_QUEUE_LENGTH, sizeof(uint8_t));*/
}

uint32_t spiDmaSetBaudRate(uint32_t baud_rate)
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

void spiDmaStart()
{
	SPIx_CS_bb = 0;
}

void spiDmaStop()
{
	SPIx_CS_bb = 1;
}

void spiDmaSend(uint8_t *tx, uint32_t length)
{
	// trzeba zabrac semafor

	uint32_t temp = 0;

	SPIx_DMAx_TX_CH->CCR = 0;			// reset dma tx channel (disabling tx channel)

	SPIx_DMAx_TX_CH->CNDTR = length; 	// nuber of data to transfer (can be written only when channel is disabled)

	SPIx_DMAx_TX_CH->CMAR = (uint32_t)tx;			// memory address

	SPIx_DMAx_TX_CH->CPAR = (uint32_t) & SPIx->DR; 	// peripheral address

	temp |= DMA_CCR1_PL;		// channel priority very high
	temp |= DMA_CCR1_MINC;		// memory increment mode enabled
	temp |= DMA_CCR1_DIR; 		// read from memory
	temp |= DMA_CCR1_TCIE;		// transfer complete interrupt enable
	temp |= DMA_CCR1_EN;		// channel enable

	spiDmaStart();

	SPIx_DMAx_TX_CH->CCR = temp;
}

void spiDmaRead(uint8_t *rx, uint32_t length)
{
	//trzeba zabrac semafor

	uint32_t temp = 0;

	SPIx_DMAx_RX_CH->CCR = 0;			// reset dma rx channel (disabling rx channel)

	SPIx_DMAx_TX_CH->CNDTR = length; 	// nuber of data to transfer (can be written only when channel is disabled)

	SPIx_DMAx_TX_CH->CMAR = (uint32_t)rx;			// memory address

	SPIx_DMAx_TX_CH->CPAR = (uint32_t) & SPIx->DR; 	// peripheral address

	temp |= DMA_CCR1_PL;		// channel priority very high
	temp |= DMA_CCR1_MINC;		// memory increment mode enabled
	temp |= DMA_CCR1_TCIE;		// transfer complete interrupt enable
	temp |= DMA_CCR1_EN;		// channel enable

	SPIx_DMAx_RX_CH->CCR = temp;
}

extern "C" void SPIx_DMAx_TX_CH_IRQHandler(void) __attribute__ ((interrupt));
void SPIx_DMAx_TX_CH_IRQHandler(void)
{
	// tutaj oddac semafor

	spiDmaStop();

	SPIx_DMAx_TX_IFCR_CTCIFx_bb = 0; // clear interrupt flag
}

extern "C" void SPIx_DMAx_RX_CH_IRQHandler(void) __attribute__ ((interrupt));
void SPIx_DMAx_RX_CH_IRQHandler(void)
{
	// tutaj oddac semafor
}

