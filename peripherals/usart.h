/**
 * \file usart.h
 * \brief Header for usart.cpp
 * \author Freddie Chopin, http://www.freddiechopin.info http://www.distortec.com
 * \author Mazeryt Freager, http://www.gotoc.co
 * \date 2012-08-30
 */

#ifndef USART_H_
#define USART_H_

#include "FreeRTOS.h"
#include "config.h"
#include "error.h"

#include "hdr/hdr_usart.h"
#include "hdr/hdr_gpio.h"
#include "hdr/hdr_rcc.h"
#include "hdr/hdr_dma.h"
#include "hdr/hdr_bitband.h"

#include "gpio.h"
#include "rcc.h"
#include "usart.h"
#include "helper.h"
#include "error.h"

#include "task.h"
#include "queue.h"
#include "semphr.h"

#define _INPUT_BUFFER_SIZE					128
#define _OUTPUT_BUFFER_SIZE					512

/*---------------------------------------------------------------------------------------------------------------------+
| USART peripheral definition
+---------------------------------------------------------------------------------------------------------------------*/

typedef struct usart_definition{

	USART_TypeDef * _USARTx;

	GPIO_TypeDef * usart_rx_gpio;
	enum GpioPin usart_rx_pin;
	GPIO_TypeDef * usart_tx_gpio;
	enum GpioPin usart_tx_pin;
	GPIO_TypeDef * usart_cs_gpio;
	enum GpioPin usart_cs_pin;
	GPIO_TypeDef * usart_rs_gpio;
	enum GpioPin usart_rs_pin;

	enum GpioConfiguration usart_pin_conf;

	unsigned long _RCC_APBxENR_ADDR_bb;
	unsigned long _ENR_USARTxEN;
	unsigned long _ENR_DMAxEN;
	unsigned long _RCC_AHBENR_DMA;

	unsigned long _USARTx_BAUDRATE;

	DMA_Channel_TypeDef * _USARTx_DMAx_TX_CH;
	IRQn_Type _USARTx_DMAx_TX_CH_IRQn;
	void (*_USARTx_DMAx_TX_CH_IRQHandler)();

	unsigned long _USARTx_DMAx_TX;
	unsigned long _IFCR_CTCIFx_bb;

	unsigned long _USARTx_DMAx_TX_CH_IRQ_PRIORITY;
	unsigned long _USARTx_IRQ_PRIORITY;

	IRQn_Type _USARTx_IRQn;
	void (*_USARTx_IRQHandler)();

	unsigned long _USARTx_RX_QUEUE_LENGTH;
	unsigned long _USARTx_RX_QUEUE_BUFFER_LENGTH;
	unsigned long _USARTx_TX_QUEUE_LENGTH;
	unsigned long _USARTx_BUF_READ_QUEUE_LENGTH;

	unsigned long _USART_TX_TASK_PRIORITY;
	unsigned long _USART_TX_STACK_SIZE;

	unsigned long _USART_RX_TASK_PRIORITY;
	unsigned long _USART_RX_STACK_SIZE;

}usart_def_t;

/*---------------------------------------------------------------------------------------------------------------------+
| USART peripheral driver
|
| Contain
| - RX/TX queues
| - r/w methods
| - IO buffers
| - rx/tx Tasks
+---------------------------------------------------------------------------------------------------------------------*/

typedef struct usart_driver{

	uint32_t (* read)(struct usart_driver * self, portTickType ticks_to_wait, char *c, size_t size);
	uint32_t (* write)(struct usart_driver * self, portTickType ticks_to_wait, const char *c);
	uint32_t (* printf)(struct usart_driver * self, portTickType ticks_to_wait, const char *format, ...);

	xQueueHandle _rxQueue;
	xQueueHandle _txQueue;
	xQueueHandle _readBuffer;
	xSemaphoreHandle _dmaTxSemaphore;

	char _inputBuffer[_INPUT_BUFFER_SIZE];
	char _outputBuffer[_OUTPUT_BUFFER_SIZE];

	void (*_rxTask)(void *parameters);
	void (*_txTask)(void *parameters);

}usart_driver_t;

/*---------------------------------------------------------------------------------------------------------------------+
| global functions' prototypes
+---------------------------------------------------------------------------------------------------------------------*/
void usart_low_level_put(char c);

//Drivers functions
uint32_t usart_read(usart_driver_t * drv, portTickType ticks_to_wait, char *c, size_t length);
uint32_t usart_write(usart_driver_t * drv, portTickType ticks_to_wait, const char *c);
enum Error usart_printf(usart_driver_t * drv, portTickType ticks_to_wait, const char *format, ...);

//C style functions for standard lib
#ifdef __cplusplus
extern "C" {
#endif

enum Error usart_initialize(usart_def_t * u, usart_driver_t * drv);

#ifdef __cplusplus
}
#endif

/*---------------------------------------------------------------------------------------------------------------------+
 | Used USART peripherals declarations
 +---------------------------------------------------------------------------------------------------------------------*/
extern const usart_def_t usart1_t;
extern const usart_driver_t usart1_handler;



#endif /* USART_H_ */
