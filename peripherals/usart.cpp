/**
 * \file usart.cpp
 * \brief USART driver
 *
 * Functions for USART control
 *
 * chip: STM32L1xx; prefix: usart
 *
 * \author Freddie Chopin, http://www.freddiechopin.info http://www.distortec.com
 * \author Mazeryt Freager, http://www.gotoc.co
 * \date 2012-08-30
 */

#include <stdint.h>
#include <string.h>
#include <stdarg.h>
#include <stdio.h>

#include "stm32l152xc.h"

#include "config.h"

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

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

/*---------------------------------------------------------------------------------------------------------------------+
 | local variables' types
 +---------------------------------------------------------------------------------------------------------------------*/

/// list of possible statuses of RX queue message
enum _RxStatus {
	RX_STATUS_HAD_NONE,						///< there was/is no "\r\n" sequence
	RX_STATUS_HAD_CR,						///< there was/is '\r'
	RX_STATUS_HAD_CR_LF,					///< there is a "\r\n" sequence
};

/// message for UART RX queue
struct _RxMessage {
	size_t length;							///< length of string
	enum _RxStatus status;					///< status of received data
	char string[USARTx_RX_QUEUE_BUFFER_LENGTH];	///< string buffer, not null-terminated!
};

/// message for USART TX queue
struct _TxMessage {
	size_t length;			///< length of string, not including trailing '\0'
	char *string;///< pointer to null-terminated string, either in flash or in dynamic buffer
};

/*---------------------------------------------------------------------------------------------------------------------+
 | local defines
 +---------------------------------------------------------------------------------------------------------------------*/

#define _INPUT_BUFFER_SIZE					128
#define _OUTPUT_BUFFER_SIZE					512


/*---------------------------------------------------------------------------------------------------------------------+
 | local functions' declarations
 +---------------------------------------------------------------------------------------------------------------------*/

static void _rxTask(void *parameters);
static void _txTask(void *parameters);


/*---------------------------------------------------------------------------------------------------------------------+
 | local variables
 +---------------------------------------------------------------------------------------------------------------------*/

extern char __ram_start[];					// imported from linker script

static xQueueHandle _rxQueue;
static xQueueHandle _txQueue;
static xSemaphoreHandle _dmaTxSemaphore;

static char _inputBuffer[_INPUT_BUFFER_SIZE];
static char _outputBuffer[_OUTPUT_BUFFER_SIZE];

/*---------------------------------------------------------------------------------------------------------------------+
 | Peripherals IRQ-es
 +---------------------------------------------------------------------------------------------------------------------*/
extern "C" void USARTx_IRQHandler(void) __attribute((interrupt));
extern "C" void USARTx_DMAx_TX_CH_IRQHandler(void) __attribute__ ((interrupt));

/*---------------------------------------------------------------------------------------------------------------------+
 | Used USART peripherals definitions
 +---------------------------------------------------------------------------------------------------------------------*/

const usart_def_t usart1_t = {
	._USARTx 			= USART1,
	.usart_rx_gpio 		= GPIOA,
	.usart_rx_pin 		= GPIO_PIN10,
	.usart_tx_gpio 		= GPIOA,
	.usart_tx_pin 		= GPIO_PIN9,
	.usart_cs_gpio      = NULL,
	.usart_cs_pin		= NULL,
	.usart_rs_gpio		= NULL,
	.usart_rs_pin		= NULL,

	.usart_pin_conf = GPIO_AF7_PP_40MHz_PULL_UP,

	._RCC_APBxENR_ADDR_bb 	= &RCC->APB2ENR,
	._ENR_USARTxEN 			= RCC_APB2ENR_USART1EN_bit,
	._ENR_DMAxEN			= RCC_AHBENR_DMA1EN_bit,
	._RCC_AHBENR_DMA 	    = &RCC->AHBENR,

	._USARTx_BAUDRATE		= 115200,

	._USARTx_DMAx_TX_CH		= DMA1_Channel4,
	._USARTx_DMAx_TX_CH_IRQn= DMA1_Channel4_IRQn,
	._USARTx_DMAx_TX_CH_IRQHandler	= USARTx_DMAx_TX_CH_IRQHandler,

	._USARTx_DMAx_TX		= DMA_IFCR_CTCIF4_bit,
	._IFCR_CTCIFx_bb		= &(DMA1)->IFCR,

	._USARTx_DMAx_TX_CH_IRQ_PRIORITY= USARTx_DMAx_TX_CH_IRQ_PRIORITY,
	._USARTx_IRQ_PRIORITY 			= USARTx_IRQ_PRIORITY,

	._USARTx_IRQn			= USART1_IRQn,
	._USARTx_IRQHandler		= USARTx_IRQHandler,

	._USARTx_RX_QUEUE_LENGTH= USARTx_RX_QUEUE_LENGTH,
	._USARTx_RX_QUEUE_BUFFER_LENGTH = USARTx_RX_QUEUE_BUFFER_LENGTH,
	._USARTx_TX_QUEUE_LENGTH= USARTx_TX_QUEUE_LENGTH,

	._USART_TX_TASK_PRIORITY= USART_TX_TASK_PRIORITY,
	._USART_TX_STACK_SIZE   = USART_TX_STACK_SIZE,

	._USART_RX_TASK_PRIORITY= USART_RX_TASK_PRIORITY,
	._USART_RX_STACK_SIZE	= USART_RX_STACK_SIZE,
};

const struct usart_driver usart1_handler;

/*---------------------------------------------------------------------------------------------------------------------+
 | global functions
 +---------------------------------------------------------------------------------------------------------------------*/

enum Error usart_initialize(usart_def_t * u, usart_driver_t * drv)
{
	if(u->usart_rx_pin != NULL)
		gpioConfigurePin(u->usart_rx_gpio, u->usart_rx_pin, u->usart_pin_conf);
	if(u->usart_tx_pin != NULL)
		gpioConfigurePin(u->usart_tx_gpio, u->usart_tx_pin, u->usart_pin_conf);
	if(u->usart_cs_pin != NULL)
			gpioConfigurePin(u->usart_cs_gpio, u->usart_cs_pin, u->usart_pin_conf);
	if(u->usart_rs_pin != NULL)
			gpioConfigurePin(u->usart_rs_gpio, u->usart_rs_pin, u->usart_pin_conf);

	drv->write = &usart_write;
	drv->read  = &usart_read;
	drv->printf= &usart_printf;
	drv->_rxTask = &_rxTask;
	drv->_txTask = &_txTask;

	//USARTx configuration
	BITBAND(u->_RCC_APBxENR_ADDR_bb, u->_ENR_USARTxEN) = 1;

	u->_USARTx->BRR = (rccGetCoreFrequency() + u->_USARTx_BAUDRATE / 2)
					/ u->_USARTx_BAUDRATE;	// calculate baudrate (with rounding)

	// enable peripheral, transmitter and receiver, enable RXNE interrupt
	u->_USARTx->CR1 = USART_CR1_UE | USART_CR1_RXNEIE | USART_CR1_TE | USART_CR1_RE;
	u->_USARTx->CR3 = USART_CR3_DMAT;

	NVIC_SetPriority(u->_USARTx_IRQn, u->_USARTx_IRQ_PRIORITY);	// set USART priority
	NVIC_EnableIRQ(u->_USARTx_IRQn);				// enable USART IRQ

	BITBAND(u->_RCC_AHBENR_DMA, u->_ENR_DMAxEN) = 1; // enable DMA

	NVIC_SetPriority(u->_USARTx_DMAx_TX_CH_IRQn, u->_USARTx_DMAx_TX_CH_IRQ_PRIORITY);// set DMA IRQ priority
	NVIC_EnableIRQ(u->_USARTx_DMAx_TX_CH_IRQn);	// enable IRQ

	//RTOS configuration
	vSemaphoreCreateBinary(drv->_dmaTxSemaphore);

	if (drv->_dmaTxSemaphore == NULL)			// semaphore not created?
		return ERROR_FreeRTOS_errCOULD_NOT_ALLOCATE_REQUIRED_MEMORY;// return with error

	drv->_txQueue = xQueueCreate(u->_USARTx_TX_QUEUE_LENGTH, sizeof(struct _TxMessage));

	if (drv->_txQueue == NULL)					// queue not created?
		return ERROR_FreeRTOS_errCOULD_NOT_ALLOCATE_REQUIRED_MEMORY;// return with error

	drv->_rxQueue = xQueueCreate(u->_USARTx_RX_QUEUE_LENGTH, sizeof(struct _RxMessage));

	if (drv->_rxQueue == NULL)					// queue not created?
		return ERROR_FreeRTOS_errCOULD_NOT_ALLOCATE_REQUIRED_MEMORY;// return with error

	portBASE_TYPE ret = xTaskCreate(drv->_txTask, (signed char* )"USART TX",
			u->_USART_TX_STACK_SIZE, (void *)drv, u->_USART_TX_TASK_PRIORITY, NULL);

	enum Error error = errorConvert_portBASE_TYPE(ret);

	if (error != ERROR_NONE)
		return error;

	ret = xTaskCreate(drv->_rxTask, (signed char* )"USART RX", u->_USART_RX_STACK_SIZE,
			(void*)drv, u->_USART_RX_TASK_PRIORITY, NULL);

	error = errorConvert_portBASE_TYPE(ret);

	return error;
}


/**
 * \brief Low-level character print.
 *
 * Low-level character print. Should be used for debugging only.
 *
 * \param [in] c is the character that will be printed
 */

void usart_low_level_put(char c) {
	while (!(USARTx_SR_TXE_bb(USARTx)));
	USARTx->DR = c;
}

/*---------------------------------------------------------------------------------------------------------------------+
 | local functions
 +---------------------------------------------------------------------------------------------------------------------*/

/**
 * \brief USART RX task.
 *
 * USART RX task - handles input.
 */

static void _rxTask(void *parameters)
{
	size_t input_length = 0;

	usart_driver_t * drv = (usart_driver_t *) parameters;

	while (1) {
		struct _RxMessage message;

		xQueueReceive(drv->_rxQueue, &message, portMAX_DELAY);

		if ((_INPUT_BUFFER_SIZE - 1) < (message.length + input_length))	// does input fit into buffer?
				{									// no - reset sequence
			drv->write( drv,
					"ERROR: input is longer than buffer length! (" __FILE__ ":" STRINGIZE(__LINE__) ")\r\n",
					0);
			input_length = 0;
			continue;
		}

		memcpy(&drv->_inputBuffer[input_length], message.string, message.length);
		input_length += message.length;

		if (message.status == RX_STATUS_HAD_CR_LF)// is the message complete (terminated with "\r\n" sequence)?
				{									// yes - start processing
			_inputBuffer[input_length] = '\0';	// terminate input string

			enum Error error = ERROR_NONE;
//					commandProcessInput(_inputBuffer, _outputBuffer, _OUTPUT_BUFFER_SIZE);	// process input

			if (error == ERROR_NONE)		// input processed successfully?
				drv->write(drv, drv->_outputBuffer, 0);
			else
				// input processing error
				drv->printf(drv, 0,
						"ERROR: command handler execution failed with code %d! (" __FILE__ ":" STRINGIZE(__LINE__) ")\r\n",
						error);

			input_length = 0;				// reset sequence
		}

	}
}

/**
 * \brief USART TX task.
 *
 * USART TX task - handles output.
 */

static void _txTask(void *parameters)
{
	char *previous_string = NULL;

	usart_driver_t * drv = (usart_driver_t *) parameters;

	while (1) {
		struct _TxMessage message;

		xQueueReceive(drv->_txQueue, &message, portMAX_DELAY);	// get data to send

		xSemaphoreTake(drv->_dmaTxSemaphore, portMAX_DELAY);	// wait for DMA to be free

		if (previous_string >= __ram_start)	// was the previously used string in RAM?
			vPortFree(previous_string);		// yes - free the temporary buffer

		USARTx_DMAx_TX_CH->CCR = 0;				// disable channel
		USARTx_DMAx_TX_CH->CMAR = (uint32_t) message.string;	// source
		USARTx_DMAx_TX_CH->CPAR = (uint32_t) & USARTx->DR;	// destination
		USARTx_DMAx_TX_CH->CNDTR = message.length;	// length
		// low priority, 8-bit source and destination, memory increment mode, memory to peripheral, transfer complete
		// interrupt enable, enable channel
		USARTx_DMAx_TX_CH->CCR = DMA_CCR_PL_LOW | DMA_CCR_MSIZE_8
				| DMA_CCR_PSIZE_8 | DMA_CCR_MINC | DMA_CCR_DIR |
				DMA_CCR_TCIE | DMA_CCR_EN;

		// save address for next iteration so that it could be freed after transfer (if it's in RAM);
		previous_string = message.string;
	}
}

/*---------------------------------------------------------------------------------------------------------------------+
 | ISRs Section for USARTs peripherals
 +---------------------------------------------------------------------------------------------------------------------*/


/*---------------------------------------------------------------------------------------------------------------------+
 | USARTx ISRs
 +---------------------------------------------------------------------------------------------------------------------*/

/**
 * \brief DMA channel interrupt handler
 *
 * DMA channel interrupt handler
 */

extern "C" void USARTx_DMAx_TX_CH_IRQHandler(void) __attribute__ ((interrupt));
void USARTx_DMAx_TX_CH_IRQHandler(void)
{
	signed portBASE_TYPE higher_priority_task_woken;

	xSemaphoreGiveFromISR(usart1_handler._dmaTxSemaphore, &higher_priority_task_woken);

	//	USARTx_DMAx_TX_IFCR_CTCIFx_bb = 1;			// clear interrupt flag
	BITBAND(usart1_t._IFCR_CTCIFx_bb, DMA_IFCR_CTCIF4_bit) = 1; // clear interrupt flag

	portEND_SWITCHING_ISR(higher_priority_task_woken);
}

/**
 * \brief USART interrupt handler
 *
 * USART interrupt handler
 */

extern "C" void USARTx_IRQHandler(void) __attribute((interrupt));
void USARTx_IRQHandler(void)
{
	portBASE_TYPE higher_priority_task_woken = pdFALSE;
	static struct _RxMessage message;

	while (USARTx_SR_RXNE_bb(usart1_t._USARTx))		// loop while data is available
	{
		char c = USARTx->DR;
		message.string[message.length++] = c;	// get char to buffer

		// check for "\r\n" sequence in the string
		if ((message.status == RX_STATUS_HAD_CR) && (c == '\n'))
			message.status = RX_STATUS_HAD_CR_LF;
		else if (c == '\r')
			message.status = RX_STATUS_HAD_CR;
		else
			message.status = RX_STATUS_HAD_NONE;

		// transfer block only if out of space or "\r\n" sequence was found
		if ((message.length >= USARTx_RX_QUEUE_BUFFER_LENGTH)
				|| (message.status == RX_STATUS_HAD_CR_LF)) {
			xQueueSendFromISR(_rxQueue, &message, &higher_priority_task_woken);

			message.length = 0;

			if (message.status == RX_STATUS_HAD_CR_LF)
				message.status = RX_STATUS_HAD_NONE;
		}
	}

	portEND_SWITCHING_ISR(higher_priority_task_woken);
}

/**
 * \brief Tries to read one received character.
 *
 * Characters are received in interrupt and transfered via FreeRTOS queue.
 *
 * \param [in] usart_driver_t usart driver
 * \param [in] ticks_to_wait is the number of RTOS ticks that the function should block
 * \param [out] c is a reference to char variable that will hold the result
 *
 * \return number of read characters - 1 on success, 0 if queue was empty
 */

uint32_t usart_read(usart_driver_t * drv, const portTickType ticks_to_wait, char *c)
{
	uint32_t count = 0;

	if (xQueueReceive(drv->_rxQueue, c, ticks_to_wait) == pdTRUE)
		count = 1;

	return count;
}

/**
 * \brief Adds one string to UART TX queue.
 *
 * Adds one string to UART TX queue. Calling task should have read-only access to shared_data section.
 *
 * \param [in] usart_driver_t usart driver
 * \param [in] string is the pointer to zero terminated string
 * \param [in] ticks_to_wait is the amount of time the call should block while waiting for the operation to finish, use
 * portMAX_DELAY to suspend
 *
 * \return length on success, otherwise an error code defined in the file error.h
 */

uint32_t usart_write(usart_driver_t * drv, portTickType ticks_to_wait, const char *string)
{
	struct _TxMessage message;

	message.length = strlen(string);

	if (message.length == 0)
		return -1;

	if (string >= __ram_start)				// is the string in RAM?
			{
		message.string = (char*) pvPortMalloc(message.length);
		if (message.string == NULL)
			return ERROR_FreeRTOS_errCOULD_NOT_ALLOCATE_REQUIRED_MEMORY;

		memcpy(message.string, string, message.length);
	} else
		message.string = (char*) string;// no, string in ROM - just use the address

	portBASE_TYPE ret = xQueueSend(drv->_txQueue, &message, ticks_to_wait);

	enum Error error = errorConvert_portBASE_TYPE(ret);

	if (error != ERROR_NONE)
		if (message.string >= __ram_start)				// is the string in RAM?
			vPortFree(message.string);

	return message.length;
}

/**
 * \brief Sends one formatted string via USART.
 *
 * Sends one formatted string via USART.
 *
 * \param [in] drv  is driver to usart itself
 * \param [in] ticks_to_wait is the amount of time the call should block while waiting for the operation to finish, use
 * portMAX_DELAY to suspend
 * \param [in] string is a format string, printf() style
 *
 * \return ERROR_NONE on success, otherwise an error code defined in the file error.h
 */

enum Error usart_printf(usart_driver_t * drv, portTickType ticks_to_wait, const char *format, ...)
{
	char *buffer = (char*) pvPortMalloc(strlen(format) * 2);

	if (buffer == NULL)
		return ERROR_FreeRTOS_errCOULD_NOT_ALLOCATE_REQUIRED_MEMORY;

	va_list args;

	va_start(args, format);

	vsprintf(buffer, format, args);

	enum Error error = drv->write(drv, buffer, ticks_to_wait);

	vPortFree(buffer);

	return error;
}

/*---------------------------------------------------------------------------------------------------------------------+
| global functions
+---------------------------------------------------------------------------------------------------------------------*/


