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
#include "error.h"
#include "task.h"


/*---------------------------------------------------------------------------------------------------------------------+
 | local functions' declarations
 +---------------------------------------------------------------------------------------------------------------------*/

static void _spiRxTask(void *parameters);
static void _spiTxTask(void *parameters);
static void _spiTxTestTask(void *parameters);

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
static xSemaphoreHandle _spiDmaRxSemaphore;

static uint8_t rx_flag=0;

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
	gpioConfigurePin(SPIx_CS_GPIO, SPIx_CS_PIN, SPIx_CS_CONFIGURATION);

	// Configuring SPI
	RCC_APBxENR_SPIxEN_bb = 1;
	SPIx->CR1 |= /*SPI_CR1_SSM | SPI_CR1_SSI |*/ SPI_CR1_SPE | SPI_CR1_MSTR;	// software slave management, enable SPI, master mode
	//SPIx->CR1 |= SPI_CR1_SSM | SPI_CR1_SSI;
	SPIx->CR2 |= SPI_CR2_TXDMAEN | SPI_CR2_TXEIE | SPI_CR2_RXNEIE | SPI_CR2_SSOE;  	// enable Tx buffer DMA

	// DMA
	RCC_AHBENR_SPIx_DMAxEN_bb = 1;				// enable DMA

	NVIC_SetPriority(SPIx_DMAx_TX_CH_IRQn, SPIx_DMAx_TX_CH_IRQ_PRIORITY);	// set DMA IRQ priority
	NVIC_EnableIRQ(SPIx_DMAx_TX_CH_IRQn);				// enable DMA TX IRQ

	//NVIC_SetPriority(SPIx_DMAx_RX_CH_IRQn, SPIx_DMAx_RX_CH_IRQ_PRIORITY);// set DMA IRQ priority
	//NVIC_EnableIRQ(SPIx_DMAx_RX_CH_IRQn);	// enable DMA RX IRQ

	vSemaphoreCreateBinary(_spiDmaTxSemaphore);
	//vSemaphoreCreateBinary(_spiDmaRxSemaphore);

	_spiTxQueue = xQueueCreate(SPIx_TX_QUEUE_LENGTH, sizeof(struct _spiTxMessage));
	_spiRxQueue = xQueueCreate(SPIx_RX_QUEUE_LENGTH, sizeof(uint8_t));
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

void spiDmaSend(_spiTxMessage *message, portTickType ticks_to_wait)
{
	xQueueSend(_spiTxQueue, message, ticks_to_wait);
}

void spiDmaSendDummy(uint32_t length, portTickType ticks_to_wait)
{
	_spiTxMessage struktura;
	for(int i=0;i<length;i++)
	{
		struktura.tx=255;
	}
	struktura.length=length;
	struktura.dummy=1;
	spiDmaSend(&struktura, ticks_to_wait);
}

void spiDmaRead(uint8_t *rx, uint32_t length, portTickType ticks_to_wait)
{
	// tu odczyt po DMA (trzeba ładować w przerwaniu do struktury, a nie prosto do kolejki)
	// Na razie odczyt bez DMA
	for(int i=0;i<length;i++)
	{
		xQueueReceive(_spiRxQueue, rx++, portMAX_DELAY);	// get data from queue
	}
}

enum Error _initializeSpiDmaTask(void)
{
	spiInitialize();

	spiSetBaudRate(SPIx_BAUDRATE);

	portBASE_TYPE ret = xTaskCreate(_spiTxTask, (signed char*)"spiTxTask", SPI_TX_STACK_SIZE, NULL,
			SPI_TX_TASK_PRIORITY, NULL);

	return errorConvert_portBASE_TYPE(ret);
}

static void _spiTxTask(void *parameters)
{
	(void) parameters;

	while(1)
	{
		struct _spiTxMessage struktura;

		xQueueReceive(_spiTxQueue, &struktura, portMAX_DELAY);	// get data to send

		xSemaphoreTake(_spiDmaTxSemaphore, portMAX_DELAY);	// wait for DMA to be free

		if(struktura.dummy)
		{
			rx_flag=1;
		}

		SPIx_DMAx_TX_CH->CCR = 0;				// disable channel
		SPIx_DMAx_TX_CH->CMAR = (uint32_t) struktura.tx;	// source
		SPIx_DMAx_TX_CH->CPAR = (uint32_t) & SPIx->DR;	// destination
		SPIx_DMAx_TX_CH->CNDTR = struktura.length;	// length
		// low priority, 8-bit source and destination, memory increment mode, memory to peripheral, transfer complete
		// interrupt enable, enable channel
		SPIx_DMAx_TX_CH->CCR = DMA_CCR_PL_LOW | DMA_CCR_MSIZE_8
				| DMA_CCR_PSIZE_8 | DMA_CCR_MINC | DMA_CCR_DIR |
				DMA_CCR_TCIE | DMA_CCR_EN;
	}
}


extern "C" void SPIx_DMAx_TX_CH_IRQHandler(void) __attribute__ ((interrupt));
void SPIx_DMAx_TX_CH_IRQHandler(void)
{
	signed portBASE_TYPE higher_priority_task_woken;

	rx_flag=0;

	xSemaphoreGiveFromISR(_spiDmaTxSemaphore, &higher_priority_task_woken);

	SPIx_DMAx_TX_IFCR_CTCIFx_bb = 1;			// clear interrupt flag

	portEND_SWITCHING_ISR(higher_priority_task_woken);
}

enum Error _initializeSpiDmaTestTask(void)
{
	portBASE_TYPE ret = xTaskCreate(_spiTxTestTask, (signed char*)"spiTxTestTask", SPI_TX_STACK_SIZE, NULL,
			SPI_TX_TASK_PRIORITY, NULL);

	return errorConvert_portBASE_TYPE(ret);
}

static void _spiTxTestTask(void *parameters)
{
	(void) parameters;
	uint8_t bufor[10];
	for(int i=0;i<10;i++)
	{
		bufor[i]=i+1;
	}
	static struct _spiTxMessage struktura;
	struktura.length=10;
	struktura.tx=bufor;

	portTickType xLastHeartBeat;

	xLastHeartBeat = xTaskGetTickCount();

	while(1)
	{
		vTaskDelay(1000/portTICK_RATE_MS);
		spiDmaSend(&struktura, 100);
	}
}

extern "C" void SPIx_IRQHandler(void) __attribute__ ((interrupt));
void SPIx_IRQHandler(void)
{
	uint8_t rx;
	portBASE_TYPE higher_priority_task_woken = pdFALSE;
	while(SPIx_SR_RXNE_bb(SPIx))
	{
		if(rx_flag)
		{
			rx=SPIx->DR;
			xQueueSendFromISR(_spiRxQueue, &rx, &higher_priority_task_woken);
		}
		else
		{
			rx=SPIx->DR;
		}
	}
}
