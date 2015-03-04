#include <cstdint>
#include <cstddef>
#include <stdint.h>
#include <string.h>
#include <stdarg.h>
#include <stdio.h>

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
#include "spi_dma_rtos.h"

/*---------------------------------------------------------------------------------------------------------------------+
| local functions' declarations
+---------------------------------------------------------------------------------------------------------------------*/

static void spiTxTask(void *parameters);
static void spiTestTask(void *parameters);
static void spiTransferTask(void *parameters);

/*---------------------------------------------------------------------------------------------------------------------+
 | local variables' types
 +---------------------------------------------------------------------------------------------------------------------*/

struct spiRxMessage
{
	uint8_t *rx;
	uint32_t length;
};

struct spiTxMessage
{
	uint8_t *tx;
	uint32_t length;
};

struct spiRxCountMessage
{
	uint32_t length;
};

struct spiMessage
{
	uint32_t length;
	uint8_t * data;
	uint8_t direction;
};

/*---------------------------------------------------------------------------------------------------------------------+
 | local defines
 +---------------------------------------------------------------------------------------------------------------------*/

#define SPI_INPUT_BUFFER_SIZE					128


/*---------------------------------------------------------------------------------------------------------------------+
 | local variables
 +---------------------------------------------------------------------------------------------------------------------*/

extern char __ram_start[];					// imported from linker script

static uint8_t spiInputBuffer[SPI_INPUT_BUFFER_SIZE];
static uint8_t spiInputBufferIndex=0;

static xQueueHandle spiRxQueue;
static xQueueHandle spiRxCountQueue;
static xQueueHandle spiTxQueue;

static xQueueHandle spiMessageQueue;

static xSemaphoreHandle spiTxDmaSemaphore;
static xSemaphoreHandle spiRxDmaSemaphore;


/*---------------------------------------------------------------------------------------------------------------------+
 | global functions
 +---------------------------------------------------------------------------------------------------------------------*/

void spiDmaInitialize(void)
{
	// Configuring GPIO
	gpioConfigurePin(SPIx_CS_GPIO, SPIx_CS_PIN, SPIx_CS_CONFIGURATION);
	gpioConfigurePin(SPIx_MISO_GPIO, SPIx_MISO_PIN, SPIx_MISO_CONFIGURATION);
	gpioConfigurePin(SPIx_MOSI_GPIO, SPIx_MOSI_PIN, SPIx_MOSI_CONFIGURATION);
	gpioConfigurePin(SPIx_SCK_GPIO, SPIx_SCK_PIN, SPIx_SCK_CONFIGURATION);

	// Configuring SPI
	RCC_APBxENR_SPIxEN_bb = 1;

	SPIx->CR1 |= /*SPI_CR1_SSM | SPI_CR1_SSI |*/ SPI_CR1_SPE | SPI_CR1_MSTR;	// enable SPI, master mode
	SPIx->CR2 |= SPI_CR2_TXDMAEN | SPI_CR2_RXDMAEN | SPI_CR2_SSOE;  	// enable Tx buffer DMA and Rx buffer DMA

	for(int i=0;i<1000;i++);

	// DMA
	RCC_AHBENR_SPIx_DMAxEN_bb = 1;				// enable DMA

	// DMA interrupts
	NVIC_SetPriority(SPIx_DMAx_TX_CH_IRQn, SPIx_DMAx_TX_CH_IRQ_PRIORITY);	// set DMA IRQ priority
	NVIC_EnableIRQ(SPIx_DMAx_TX_CH_IRQn);				// enable DMA TX IRQ

	NVIC_SetPriority(SPIx_DMAx_RX_CH_IRQn, SPIx_DMAx_RX_CH_IRQ_PRIORITY);	// set DMA IRQ priority
	NVIC_EnableIRQ(SPIx_DMAx_RX_CH_IRQn);				// enable DMA RX IRQ

	vSemaphoreCreateBinary(spiTxDmaSemaphore);
	vSemaphoreCreateBinary(spiRxDmaSemaphore);

	spiTxQueue = xQueueCreate(SPIx_TX_QUEUE_LENGTH, sizeof(struct spiTxMessage));
	spiRxCountQueue = xQueueCreate(SPIx_RX_QUEUE_LENGTH, sizeof(struct spiRxCountMessage));
	spiRxQueue = xQueueCreate(SPIx_RX_QUEUE_LENGTH, sizeof(struct spiRxMessage));

//	xTaskCreate(spiTxTask, (signed char* )"SPI TX", SPI_TX_STACK_SIZE,
//				NULL, SPI_TX_TASK_PRIORITY, NULL);

	xTaskCreate(spiTestTask, (signed char* )"SPI TEST", SPI_TX_STACK_SIZE,
					NULL, SPI_TX_TASK_PRIORITY, NULL);

	spiMessageQueue = xQueueCreate(SPIx_TX_QUEUE_LENGTH, sizeof(struct spiMessage));

	xTaskCreate(spiTransferTask, (signed char* )"SPI TRANSFER", SPI_TX_STACK_SIZE,
						NULL, SPI_TX_TASK_PRIORITY, NULL);
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

void spiDmaSend(uint8_t *tx, uint32_t length, portTickType ticks_to_wait)
{
	//struct spiTxMessage Message;
	struct spiMessage Message;

	Message.length=length;
	Message.direction=SPI_DIRECTION_SEND;

//	if (tx >= __ram_start)				// is the string in RAM?
//	{
		//Message.tx = (uint8_t*) pvPortMalloc(Message.length);
		Message.data = (uint8_t*) pvPortMalloc(Message.length);
		//memcpy(Message.tx, tx, Message.length);
		memcpy(Message.data, tx, Message.length);
//	}
//	else
//	{
//		Message.tx = (uint8_t*) tx;// no, string in ROM - just use the address
//	}

	//xQueueSend(spiTxQueue, &Message, ticks_to_wait);
	xQueueSend(spiMessageQueue, &Message, ticks_to_wait);

//	if (Message.tx >= __ram_start)	// is the string in RAM?
//	{
//		vPortFree(Message.tx);
//	}
}

static void spiTxTask(void *parameters)
{
	uint8_t *previous_tx = NULL;

	(void) parameters;						// suppress warning

	while(1)
	{
		struct spiTxMessage Message;

		xQueueReceive(spiTxQueue, &Message, portMAX_DELAY);	// get data to send

		xSemaphoreTake(spiTxDmaSemaphore, portMAX_DELAY);	// wait for SPI to be free

		if (previous_tx != NULL)	// if previous message wasn't null
					vPortFree(previous_tx);		// yes - free the temporary buffer

		uint32_t temp = 0;

		SPIx_DMAx_TX_CH->CCR = 0;			// reset dma tx channel (disabling tx channel)

		SPIx_DMAx_TX_CH->CNDTR = Message.length; 	// nuber of data to transfer (can be written only when channel is disabled)

		SPIx_DMAx_TX_CH->CMAR = (uint32_t)Message.tx;			// memory address

		SPIx_DMAx_TX_CH->CPAR = (uint32_t) & SPIx->DR; 	// peripheral address

		temp |= DMA_CCR1_PL;		// channel priority very high
		temp |= DMA_CCR1_MINC;		// memory increment mode enabled
		temp |= DMA_CCR1_DIR; 		// read from memory
		temp |= DMA_CCR1_TCIE;		// transfer complete interrupt enable
		temp |= DMA_CCR1_EN;		// channel enable

		SPIx_DMAx_TX_CH->CCR = temp;

		// save address for next iteration so that it could be freed after transfer
		previous_tx=Message.tx;
	}
}

static void spiTransferTask(void *parameters)
{
	uint8_t *previous_tx = NULL;

	(void) parameters;						// suppress warning

	while(1)
	{
		struct spiMessage Message;

		xQueueReceive(spiMessageQueue, &Message, portMAX_DELAY);	// get data to send

		xSemaphoreTake(spiTxDmaSemaphore, portMAX_DELAY);	// wait for SPI TX channel to be free

		if (previous_tx != NULL)	// if previous message wasn't null
			vPortFree(previous_tx);		// yes - free the temporary buffer

		if(Message.direction == SPI_DIRECTION_SEND)   // when message to transmit
		{
			uint32_t temp = 0;

			SPIx_DMAx_TX_CH->CCR = 0;			// reset dma tx channel (disabling tx channel)

			SPIx_DMAx_TX_CH->CNDTR = Message.length; 	// nuber of data to transfer (can be written only when channel is disabled)

			SPIx_DMAx_TX_CH->CMAR = (uint32_t)Message.data;			// memory address

			SPIx_DMAx_TX_CH->CPAR = (uint32_t) & SPIx->DR; 	// peripheral address

			temp |= DMA_CCR1_PL;		// channel priority very high
			temp |= DMA_CCR1_MINC;		// memory increment mode enabled
			temp |= DMA_CCR1_DIR; 		// read from memory
			temp |= DMA_CCR1_TCIE;		// transfer complete interrupt enable
			temp |= DMA_CCR1_EN;		// channel enable

			SPIx_DMAx_TX_CH->CCR = temp;

			// save address for next iteration so that it could be freed after transfer
			previous_tx=Message.data;
		}

		else		// when message to read
		{
			xSemaphoreTake(spiRxDmaSemaphore, portMAX_DELAY);	// wait for SPI DMA RX channel to be free

			uint32_t temp = spiInputBufferIndex;
			uint8_t index = 0;

			if(spiInputBufferIndex + Message.length >= SPI_INPUT_BUFFER_SIZE)
			{
				index = 0;
			}
			else
			{
				index = spiInputBufferIndex;
			}

			SPIx_DMAx_RX_CH->CCR = 0;			// reset dma rx channel (disabling rx channel)

			SPIx_DMAx_RX_CH->CNDTR = Message.length; 	// nuber of data to transfer (can be written only when channel is disabled)

			SPIx_DMAx_RX_CH->CMAR = (uint32_t) &spiInputBuffer[index];			// memory address

			SPIx_DMAx_RX_CH->CPAR = (uint32_t) &SPIx->DR; 	// peripheral address

			temp |= DMA_CCR1_PL;		// channel priority very high
			temp |= DMA_CCR1_MINC;		// memory increment mode enabled
			temp |= DMA_CCR1_TCIE;		// transfer complete interrupt and transfer error interrupt enable
			temp |= DMA_CCR1_EN;		// channel enable

			SPIx_DMAx_RX_CH->CCR = temp;

			spiInputBufferIndex += Message.length;

			uint8_t dummy = 255;

			struct spiRxMessage RxMessage;
			RxMessage.length = Message.length;
			RxMessage.rx = &spiInputBuffer[index];

			xQueueSend(spiRxQueue, &RxMessage, portMAX_DELAY);

			spiDmaSendDummy(&dummy, Message.length);
		}
	}
}

void spiDmaSendDummy(uint8_t *dummy, uint32_t length)
{
	uint32_t temp = 0;

	SPIx_DMAx_TX_CH->CCR = 0;			// reset dma tx channel (disabling tx channel)

	SPIx_DMAx_TX_CH->CNDTR = length; 	// nuber of data to transfer (can be written only when channel is disabled)

	SPIx_DMAx_TX_CH->CMAR = (uint32_t)dummy;			// memory address

	SPIx_DMAx_TX_CH->CPAR = (uint32_t) & SPIx->DR; 	// peripheral address

	temp |= DMA_CCR1_PL;		// channel priority very high
	temp |= DMA_CCR1_DIR; 		// read from memory
	temp |= DMA_CCR1_EN;		// channel enable

	SPIx_DMAx_TX_CH->CCR = temp;
}

static void spiRxTask(void *parameters)
{

}



void spiDmaRead2Buffer(uint32_t length, portTickType ticks_to_wait)
{
	struct spiMessage Message;

	Message.length=length;
	Message.direction=SPI_DIRECTION_READ;
	Message.data=NULL;

	xQueueSend(spiMessageQueue, &Message, ticks_to_wait);
}

static void spiTestTask(void *parameters)
{
	uint8_t tx[10];
	for(int i=0;i<10;i++)
	{
		tx[i]=i+1;
	}

	while(1)
	{
		//spiDmaSend(tx,10,10);
		//vTaskDelay(1000/portTICK_RATE_MS);
		spiDmaRead2Buffer(5,10);
		vTaskDelay(1000/portTICK_RATE_MS);
	}
}

extern "C" void SPIx_DMAx_TX_CH_IRQHandler(void) __attribute__ ((interrupt));
void SPIx_DMAx_TX_CH_IRQHandler(void)
{
	signed portBASE_TYPE higher_priority_task_woken;

	xSemaphoreGiveFromISR(spiTxDmaSemaphore, &higher_priority_task_woken);

	SPIx_DMAx_TX_IFCR_CTCIFx_bb = 1; // clear interrupt flag

	portEND_SWITCHING_ISR(higher_priority_task_woken);
}

extern "C" void SPIx_DMAx_RX_CH_IRQHandler(void) __attribute__ ((interrupt));
void SPIx_DMAx_RX_CH_IRQHandler(void)
{
	signed portBASE_TYPE higher_priority_task_woken;

	xSemaphoreGiveFromISR(spiRxDmaSemaphore, &higher_priority_task_woken);

	SPIx_DMAx_RX_IFCR_CTCIFx_bb = 1; // clear interrupt flag

	portEND_SWITCHING_ISR(higher_priority_task_woken);
}

