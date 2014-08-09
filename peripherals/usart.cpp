/**
 * \file usart.cpp
 * \brief USART driver
 *
 * Functions for USART control
 *
 * chip: STM32L1xx; prefix: usart
 *
 * \author Freddie Chopin, http://www.freddiechopin.info http://www.distortec.com
 * \date 2012-08-30
 */

#include <stdint.h>
#include <string.h>
#include <stdarg.h>
#include <stdio.h>

#include "inc/stm32l1xx.h"

#include "config.h"

#include "hdr/hdr_usart.h"
#include "hdr/hdr_gpio.h"
#include "hdr/hdr_rcc.h"
#include "hdr/hdr_dma.h"

#include "gpio.h"
#include "rcc.h"
#include "usart.h"
#include "command.h"
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
 | local functions' declarations
 +---------------------------------------------------------------------------------------------------------------------*/

static void _rxTask(void *parameters);
static void _txTask(void *parameters);

/*---------------------------------------------------------------------------------------------------------------------+
 | local defines
 +---------------------------------------------------------------------------------------------------------------------*/

#define _INPUT_BUFFER_SIZE					128
#define _OUTPUT_BUFFER_SIZE					512

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
 | global functions
 +---------------------------------------------------------------------------------------------------------------------*/

/**
 * \brief Initializes USART
 *
 * Initializes USART
 *
 * \return ERROR_NONE if the semaphore and queues were successfully created and
 * tasks were successfully created and added to a ready list, otherwise an error
 * code defined in the file error.h
 */

enum Error usartInitialize(void)
{
	gpioConfigurePin(USARTx_TX_GPIO, USARTx_TX_PIN, USARTx_TX_CONFIGURATION);
	gpioConfigurePin(USARTx_RX_GPIO, USARTx_RX_PIN, USARTx_RX_CONFIGURATION);

	RCC_APBxENR_USARTxEN_bb = 1;			// enable USART in RCC

	USARTx->BRR = (rccGetCoreFrequency() + USARTx_BAUDRATE / 2)
			/ USARTx_BAUDRATE;	// calculate baudrate (with rounding)
	// enable peripheral, transmitter and receiver, enable RXNE interrupt
	USARTx->CR1 = USART_CR1_UE | USART_CR1_RXNEIE | USART_CR1_TE | USART_CR1_RE;
	USARTx->CR3 = USART_CR3_DMAT;

	NVIC_SetPriority(USARTx_IRQn, USARTx_IRQ_PRIORITY);	// set USART priority
	NVIC_EnableIRQ(USARTx_IRQn);				// enable USART IRQ

	RCC_AHBENR_DMAxEN_bb = 1;				// enable DMA

	NVIC_SetPriority(USARTx_DMAx_TX_CH_IRQn, USARTx_DMAx_TX_CH_IRQ_PRIORITY);// set DMA IRQ priority
	NVIC_EnableIRQ(USARTx_DMAx_TX_CH_IRQn);	// enable IRQ

	vSemaphoreCreateBinary(_dmaTxSemaphore);

	if (_dmaTxSemaphore == NULL)			// semaphore not created?
		return ERROR_FreeRTOS_errCOULD_NOT_ALLOCATE_REQUIRED_MEMORY;// return with error

	_txQueue = xQueueCreate(USARTx_TX_QUEUE_LENGTH, sizeof(struct _TxMessage));

	if (_txQueue == NULL)					// queue not created?
		return ERROR_FreeRTOS_errCOULD_NOT_ALLOCATE_REQUIRED_MEMORY;// return with error

	_rxQueue = xQueueCreate(USARTx_RX_QUEUE_LENGTH, sizeof(struct _RxMessage));

	if (_rxQueue == NULL)					// queue not created?
		return ERROR_FreeRTOS_errCOULD_NOT_ALLOCATE_REQUIRED_MEMORY;// return with error

	portBASE_TYPE ret = xTaskCreate(_txTask, (signed char* )"USART TX",
			USART_TX_STACK_SIZE, NULL, USART_TX_TASK_PRIORITY, NULL);

	enum Error error = errorConvert_portBASE_TYPE(ret);

	if (error != ERROR_NONE)
		return error;

	ret = xTaskCreate(_rxTask, (signed char* )"USART RX", USART_RX_STACK_SIZE,
			NULL, USART_RX_TASK_PRIORITY, NULL);

	error = errorConvert_portBASE_TYPE(ret);

	return error;
}

/**
 * \brief Sends one formatted string via USART.
 *
 * Sends one formatted string via USART.
 *
 * \param [in] ticks_to_wait is the amount of time the call should block while waiting for the operation to finish, use
 * portMAX_DELAY to suspend
 * \param [in] string is a format string, printf() style
 *
 * \return ERROR_NONE on success, otherwise an error code defined in the file error.h
 */

enum Error usartPrintf(portTickType ticks_to_wait, const char *format, ...)
{
	char *buffer = (char*) pvPortMalloc(strlen(format) * 2);

	if (buffer == NULL)
		return ERROR_FreeRTOS_errCOULD_NOT_ALLOCATE_REQUIRED_MEMORY;

	va_list args;

	va_start(args, format);

	vsprintf(buffer, format, args);

	enum Error error = usartSendString(buffer, ticks_to_wait);

	vPortFree(buffer);

	return error;
}

/**
 * \brief Low-level character print.
 *
 * Low-level character print. Should be used for debugging only.
 *
 * \param [in] c is the character that will be printed
 */

void usartSendCharacter(char c) {
	while (!(USARTx_SR_TXE_bb(USARTx)));
	USARTx->DR = c;
}

/**
 * \brief Adds one string to UART TX queue.
 *
 * Adds one string to UART TX queue. Calling task should have read-only access to shared_data section.
 *
 * \param [in] string is the pointer to zero terminated string
 * \param [in] ticks_to_wait is the amount of time the call should block while waiting for the operation to finish, use
 * portMAX_DELAY to suspend
 *
 * \return ERROR_NONE on success, otherwise an error code defined in the file error.h
 */

enum Error usartSendString(const char *string, portTickType ticks_to_wait)
{
	struct _TxMessage message;

	message.length = strlen(string);

	if (message.length == 0)
		return ERROR_NONE;

	if (string >= __ram_start)				// is the string in RAM?
			{
		message.string = (char*) pvPortMalloc(message.length);
		if (message.string == NULL)
			return ERROR_FreeRTOS_errCOULD_NOT_ALLOCATE_REQUIRED_MEMORY;

		memcpy(message.string, string, message.length);
	} else
		message.string = (char*) string;// no, string in ROM - just use the address

	portBASE_TYPE ret = xQueueSend(_txQueue, &message, ticks_to_wait);

	enum Error error = errorConvert_portBASE_TYPE(ret);

	if (error != ERROR_NONE)
		if (message.string >= __ram_start)				// is the string in RAM?
			vPortFree(message.string);

	return error;
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

	(void) parameters;						// suppress warning

	while (1) {
		struct _RxMessage message;

		xQueueReceive(_rxQueue, &message, portMAX_DELAY);

		if ((_INPUT_BUFFER_SIZE - 1) < (message.length + input_length))	// does input fit into buffer?
				{									// no - reset sequence
			usartSendString(
					"ERROR: input is longer than buffer length! (" __FILE__ ":" STRINGIZE(__LINE__) ")\r\n",
					0);
			input_length = 0;
			continue;
		}

		memcpy(&_inputBuffer[input_length], message.string, message.length);
		input_length += message.length;

		if (message.status == RX_STATUS_HAD_CR_LF)// is the message complete (terminated with "\r\n" sequence)?
				{									// yes - start processing
			_inputBuffer[input_length] = '\0';	// terminate input string

			enum Error error = commandProcessInput(_inputBuffer, _outputBuffer,
			_OUTPUT_BUFFER_SIZE);	// process input

			if (error == ERROR_NONE)		// input processed successfully?
				usartSendString(_outputBuffer, 0);
			else
				// input processing error
				usartPrintf(0,
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

	(void) parameters;						// suppress warning

	while (1) {
		struct _TxMessage message;

		xQueueReceive(_txQueue, &message, portMAX_DELAY);	// get data to send

		xSemaphoreTake(_dmaTxSemaphore, portMAX_DELAY);	// wait for DMA to be free

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
 | ISRs
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

	xSemaphoreGiveFromISR(_dmaTxSemaphore, &higher_priority_task_woken);

	USARTx_DMAx_TX_IFCR_CTCIFx_bb = 1;			// clear interrupt flag

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

	while (USARTx_SR_RXNE_bb(USARTx))		// loop while data is available
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
