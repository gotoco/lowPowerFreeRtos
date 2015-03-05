/*
 * i2c_by_PG.c
 *
 *  Created on: 1 mar 2015
 *      Author: pgielmuda
 */
#include "stm32l152xc.h"

#include "hdr/hdr_rcc.h"
#include "hdr/hdr_i2c.h"

#include "config.h"
#include "i2c_by_PG.h"
#include "gpio.h"
#include "rcc.h"
#include "error.h"

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"


#include <stdint.h>
#include <string.h>
#include <stdarg.h>
#include <stdio.h>

static xSemaphoreHandle _i2cHrdSemaphore;
static xQueueHandle _rxTxQueue;
extern char __ram_start[];


static void _i2cTask();

typedef enum
{
  TX_FLAG = 0,
  RX_FLAG
}RXTX_FLAG;

/// message for I2C RX/TX queue
struct _i2cMessage {
	RXTX_FLAG rxFlag; ///< RX or TX flag
	uint16_t addr; ///< slave address
	size_t length;	///< length of string, not including trailing '\0'
	char *string; ///< pointer to null-terminated string, either in flash or in dynamic buffer
	xTaskHandle sendMessTaskHandle; ///< pointer to TaskHandle of the task which send message
};


/*---------------------------------------------------------------------------------------------------------------------+
| global functions
+---------------------------------------------------------------------------------------------------------------------*/

/**
 * \brief Initializes I2C module
 *
 * Configures I/Os of I2C, enable clock for I2C module and configures it.
 * Also starts a task, initialize RX/TX queue and Semaphore
 */

enum Error i2cInitialize(void)
{
	// configure I2C
	gpioConfigurePin(I2Cx_SCL_GPIO, I2Cx_SCL_PIN, I2Cx_SCL_CONFIGURATION);
	gpioConfigurePin(I2Cx_SDA_GPIO, I2Cx_SDA_PIN, I2Cx_SDA_CONFIGURATION);

	RCC_APBxENR_I2CxEN_bb = 1;				// enable clock for I2C module

	uint32_t frequency = rccGetCoreFrequency();

	I2Cx_CR1_SWRST_bb(I2Cx) = 1;			// force software reset of I2C peripheral
	I2Cx_CR1_SWRST_bb(I2Cx) = 0;

	I2Cx->TRISE = frequency / 1000000 + 1;	// limit slope (standard mode)
	I2Cx->CCR = frequency / I2C_FREQUENCY / 2;	// setup clock
	I2Cx->CR2 = (frequency / 1000000) << I2C_CR2_FREQ_bit;	// config I2C module's frequency
	I2Cx_CR1_PE_bb(I2Cx) = 1;				// enable peripheral

	// create semaphore, queue, task
	vSemaphoreCreateBinary(_i2cHrdSemaphore);

	if (_i2cHrdSemaphore == NULL)			// semaphore not created?
		return ERROR_FreeRTOS_errCOULD_NOT_ALLOCATE_REQUIRED_MEMORY;// return with error

	_rxTxQueue = xQueueCreate(I2C_RXTX_QUEUE_LENGTH, sizeof(struct _i2cMessage));

	if (_rxTxQueue == NULL)					// queue not created?
		return ERROR_FreeRTOS_errCOULD_NOT_ALLOCATE_REQUIRED_MEMORY;// return with error

	portBASE_TYPE ret = xTaskCreate(_i2cTask, (signed char* )"I2C TASK",
			I2C_TASK_STACK_SIZE, NULL, I2C_TASK_PRIORITY, NULL);

	enum Error error = errorConvert_portBASE_TYPE(ret);


	return error;
}


/**
 * \brief Adds one message to I2C RXTX queue.
 *
 * Adds one message to I2C RXTX queue. Calling task should have read-only access to shared_data section.
 *
 * \param [in] address of I2C slave
 * \param [in] string is the pointer to zero terminated string
 * \param [in] lenght is a lenght of string without '\0' termination
 * \param [in] ticks_to_wait is the amount of time the call should block while waiting for the operation to finish, use
 * portMAX_DELAY to suspend
 *
 * \return ERROR_NONE on success, otherwise an error code defined in the file error.h
 */

enum Error i2cWriteToSlave(uint16_t addr, const char *string, uint8_t lenght, portTickType ticks_to_wait)
{
	struct _i2cMessage message;

	message.length = lenght;
	message.addr=addr;
	message.rxFlag=TX_FLAG;
	message.sendMessTaskHandle=xTaskGetCurrentTaskHandle();

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

	portBASE_TYPE ret = xQueueSend(_rxTxQueue, &message, ticks_to_wait);

	enum Error error = errorConvert_portBASE_TYPE(ret);


	return error;
}

/**
 * \brief Adds one request for reading from I2C slave.
 *
 * Adds one one request for reading from I2C slave. 
 *
 * \param [in] address of I2C slave
 * \param [in] string is the pointer to readed data string, calling function should know how many bytes want to read from slave and should give a pointer to a memory for readed data.
 * \param [in] lenght is a lenght of string without '\0' termination
 * \param [in] ticks_to_wait is the amount of time the call should block while waiting for the operation to finish, use
 * portMAX_DELAY to suspend
 *
 * \return ERROR_NONE on success, otherwise an error code defined in the file error.h
 */

enum Error i2cReadFromSlave(uint16_t addr, char *string, uint8_t lenght, portTickType ticks_to_wait)
{
	struct _i2cMessage message;
	//lets initialize new message
	message.length = lenght; 
	message.addr=addr; 
	message.rxFlag=RX_FLAG;
	message.sendMessTaskHandle=xTaskGetCurrentTaskHandle();
	message.string=string; 

	if (message.length == 0)
		return ERROR_NONE;

	portBASE_TYPE ret = xQueueSend(_rxTxQueue, &message, ticks_to_wait);//add new message to a RXTX queue
	enum Error error = errorConvert_portBASE_TYPE(ret);

	vTaskSuspend(NULL);//a task which is reading something from I2C slave need to wait for an answer
	return error;
}



/*---------------------------------------------------------------------------------------------------------------------+
| local functions
+---------------------------------------------------------------------------------------------------------------------*/

/**
 * \brief Reads a block of data from I2C.
 *
 * Reads a block of data from I2C.
 *
 * \param [in] message- a pointer to a _i2cMessage structure, which has all information needed for a I2C transfer
 *
 */
static void _i2cRead(_i2cMessage* message)
{
	I2Cx_CR1_START_bb(I2Cx) = 1;			// request a start
	while (I2Cx_SR1_SB_bb(I2Cx) == 0);		// wait for start to finish
	I2Cx->SR1;								// read of SR1 clears the flag
	message->addr = message->addr << 1;
	I2Cx->DR = message->addr | 1;					// transfer address (LSB set - read)
	while (I2Cx_SR1_ADDR_bb(I2Cx) == 0);	// wait for address transfer
	I2Cx->SR1;								// clear the flag
	I2Cx->SR2;


	while (message->length--)
	{
		I2Cx_CR1_ACK_bb(I2Cx) = (message->length == 0 ? 0 : 1);	// acknowledge all but last byte

		while (I2Cx_SR1_RxNE_bb(I2Cx) == 0);	// wait for DR not-empty
		*message->string++ = I2Cx->DR;
	}

	I2Cx_CR1_STOP_bb(I2Cx) = 1;				// request a stop
	vTaskResume(message->sendMessTaskHandle); //some task now have the answer for its request
}

/**
 * \brief Writes a block of data from I2C.
 *
 * Writes a block of data from I2C.
 *
 * \param [in] message- a pointer to a _i2cMessage structure, which has all information needed for a I2C transfer
 *
 */
 static void _i2cWrite(_i2cMessage* message)
{
	char* lastString=message->string;
	I2Cx_CR1_START_bb(I2Cx) = 1;			// request a start
	while (I2Cx_SR1_SB_bb(I2Cx) == 0);		// wait for start to finish
	I2Cx->SR1;								// read of SR1 clears the flag
	message->addr = message->addr << 1;
	I2Cx->DR = (message->addr & ~1);				// transfer address (LSB cleared - write)
	while (I2Cx_SR1_ADDR_bb(I2Cx) == 0);	// wait for address transfer
	I2Cx->SR1;								// clear the flag
	I2Cx->SR2;

	while (message->length--){
		while (I2Cx_SR1_TxE_bb(I2Cx) == 0);	// wait for DR empty
		I2Cx->DR = *message->string++;
	}

	while (I2Cx_SR1_TxE_bb(I2Cx) == 0 || I2Cx_SR1_BTF_bb(I2Cx) == 1);	// wait for bus not-busy
	I2Cx_CR1_STOP_bb(I2Cx) = 1;	// request a stop

	//some memory shouldn't leak out
	if (lastString >= __ram_start)				// is the string in RAM?
			vPortFree(lastString);

}

/**
 * \brief Transfers a block of data through I2C.
 *
 * Transfers a block of data through I2C from RXTX queue.
 */

static void _i2cTask(void)
{

	while(1){

		struct _i2cMessage message;

		xQueueReceive(_rxTxQueue, &message, portMAX_DELAY);	// get data to send

		xSemaphoreTake(_i2cHrdSemaphore, portMAX_DELAY);	// wait for DMA to be free, useful think for a future

		if(message.rxFlag){
			_i2cRead(&message);
		}else{
			_i2cWrite(&message);
		}


		xSemaphoreGive(_i2cHrdSemaphore);
		vTaskDelay(10/portTICK_RATE_MS);	//Then go sleep
	}
}

