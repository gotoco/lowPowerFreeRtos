/** \file main.cpp
 * \brief Main code block
 *
 * Main code block
 *
 * project: mg-stm32l_acquisition_supervisor; chip: STM32L152RB
 *
 * \author grochu
 * \date 2012-09-06
 */

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "FreeRTOS.h"
#include "task.h"

#include "ff.h"

#include "hdr/hdr_rcc.h"

#include "config.h"

#include "error.h"

#include "gpio.h"
#include "rcc.h"
#include "helper.h"
#include "usart.h"
#include "command.h"
#include "i2c.h"
#include "comm-cmd.h"
#include "serial.h"

/*---------------------------------------------------------------------------------------------------------------------+
| local functions' declarations
+---------------------------------------------------------------------------------------------------------------------*/

static enum Error _dirHandler(const char **arguments_array, uint32_t arguments_count, char *output_buffer,
		size_t output_buffer_length);
static void _heartbeatTask(void *parameters);
static enum Error _initializeHeartbeatTask(void);
static enum Error _runtimestatsHandler(const char **arguments_array, uint32_t arguments_count, char *output_buffer,
		size_t output_buffer_length);
static enum Error _tasklistHandler(const char **arguments_array, uint32_t arguments_count, char *output_buffer,
		size_t output_buffer_length);

/*---------------------------------------------------------------------------------------------------------------------+
| local variables
+---------------------------------------------------------------------------------------------------------------------*/

static const struct CommandDefinition _dirCommandDefinition =
{
		"dir",						// command string
		1,							// maximum number of arguments
		_dirHandler,				// callback function
		"dir: prints contents of selected directory on SD card\r\n"
				"\t\tusage: dir [path]\r\n",	// string displayed by help function
};

static const struct CommandDefinition _runtimestatsCommandDefinition =
{
		"runtimestats",						// command string
		0,									// maximum number of arguments
		_runtimestatsHandler,				// callback function
		"runtimestats: lists all tasks with their runtime stats\r\n",	// string displayed by help function
};

static const struct CommandDefinition _tasklistCommandDefinition =
{
		"tasklist",							// command string
		0,									// maximum number of arguments
		_tasklistHandler,					// callback function
		"tasklist: lists all tasks with their info\r\n",	// string displayed by help function
};

static FATFS _fileSystem;

/*---------------------------------------------------------------------------------------------------------------------+
| global functions
+---------------------------------------------------------------------------------------------------------------------*/

/**
 * \brief Main code block
 *
 * Main code block
 */
const unsigned char dana[] = {0x50};
const uint8_t adresMSP = 0x48;
uint8_t danaZMSP = 0x30;
uint8_t * pointer = (uint8_t*)pvPortMalloc(129*sizeof(uint8_t));


int main(void)
{
	RCC_APB1ENR_PWREN_bb = 1;
	PWR->CR = (PWR->CR & (~PWR_CR_VOS)) | PWR_CR_VOS_0;	// set VCORE voltage range 1 (1.8V)
	while((PWR->CSR & PWR_CSR_VOSF) != 0);	// wait for regulator ready

	RCC_CR_HSION_bb = 1;					// enable HSI oscillator
	while (RCC_CR_HSIRDY_bb == 0);

	rccStartPll(RCC_PLL_INPUT_HSI, HSI_VALUE, FREQUENCY);

	gpioInitialize();

	i2cInitialize();

	enum Error error = usartInitialize();
//
//	FRESULT fresult = f_mount(0, &_fileSystem);	// try mounting the filesystem on SD card
//	ASSERT("f_mount()", fresult == FR_OK);
//
	error = _initializeHeartbeatTask();
	ASSERT("_initializeHeartbeatTask()", error == ERROR_NONE);
//
//	commandRegister(&_dirCommandDefinition);
//	commandRegister(&_runtimestatsCommandDefinition);
//	commandRegister(&_tasklistCommandDefinition);

	vTaskStartScheduler();

	while (1)
	{ }
}

/*---------------------------------------------------------------------------------------------------------------------+
| local functions
+---------------------------------------------------------------------------------------------------------------------*/

/**
 * \brief Handler of 'dir' command.
 *
 * Handler of 'dir' command. Displays contents of selected directory.
 *
 * \param [in] arguments_array is the array with arguments, first elements is the command identification string - "dir"
 * \param [in] arguments_count is the number of arguments in arguments_array
 * \param [out] output_buffer is the pointer to output buffer
 * \param [in] output_buffer_length is the size of output buffer
 *
 * \return ERROR_NONE on success, otherwise an error code defined in the file error.h
 */

static enum Error _dirHandler(const char **arguments_array, uint32_t arguments_count, char *output_buffer, size_t output_buffer_length)
{
	static const char *root_path = "/";
	const char *path;

	if (arguments_count > 1)				// was argument passed to command?
		path = arguments_array[1];			// yes - use it as a path
	else
		path = root_path;					// no - use root path

	DIR dir;

	FRESULT fresult = f_opendir(&dir, path);
	enum Error error = errorConvert_FRESULT(fresult);

	if (error != ERROR_NONE)
		return error;

	FILINFO filinfo;

	while (error == ERROR_NONE)
	{
		fresult = f_readdir(&dir, &filinfo);
		error = errorConvert_FRESULT(fresult);

		if (error != ERROR_NONE)
			return error;

		size_t length = strlen(filinfo.fname);

		if (length == 0)					// end of directory?
			return error;

		 length += 2 + 1;					// include size of newline (2) and size of optional directory marker '/' (1)

		if (length > output_buffer_length - 1)	// will it fit into buffer (leave space for '\0')?
			return ERROR_BUFFER_OVERFLOW;

		output_buffer_length -= length;

		output_buffer = stpcpy(output_buffer, filinfo.fname);

		if (filinfo.fattrib & AM_DIR)		// is this a directory?
			output_buffer = stpcpy(output_buffer, "/");	// append trailing slash to indicate that

		output_buffer = stpcpy(output_buffer, "\r\n");
	}

	return error;
}

/**
 * \brief Heartbeat task that read measured data from slaves
 *
 * This is a main task in this device
 */

static void _heartbeatTask(void *parameters)
{
	(void)parameters;						// suppress warning

	portTickType xLastHeartBeat;

	xLastHeartBeat = xTaskGetTickCount();

	for(;;){
		vTaskDelay(500/portTICK_RATE_MS);
		LED1_bb ^= 1;
		LED2_bb ^= LED1_bb;;
		LED3_bb ^= LED2_bb;


		const char* twochars = "test_";
		usartSendString(twochars, 100);

	}

}

/**
 * \brief Initialization of heartbeat task - setup GPIO, create task.
 *
 * Initialization of heartbeat task - setup GPIO, create task.
 *
 * \return ERROR_NONE if the task was successfully created and added to a ready list, otherwise an error code defined in
 * the file error.h
 */

static enum Error _initializeHeartbeatTask(void)
{
	gpioConfigurePin(LED_GPIO, LED_pin_1, GPIO_OUT_PP_2MHz);
	gpioConfigurePin(LED_GPIO, LED_pin_2, GPIO_OUT_PP_2MHz);
	gpioConfigurePin(LED_GPIO, LED_pin_3, GPIO_OUT_PP_2MHz);

	portBASE_TYPE ret = xTaskCreate(_heartbeatTask, (signed char*)"heartbeat", HEARTBEAT_STACK_SIZE, NULL,
			HEARTBEAT_TASK_PRIORITY, NULL);

	return errorConvert_portBASE_TYPE(ret);
}

/**
 * \brief Handler of 'runtimestats' command.
 *
 * Handler of 'runtimestats' command. Displays all tasks with their info.
 *
 * \param [out] output_buffer is the pointer to output buffer
 * \param [in] output_buffer_length is the size of output buffer
 *
 * \return ERROR_NONE on success, otherwise an error code defined in the file error.h
 */

static enum Error _runtimestatsHandler(const char **arguments_array, uint32_t arguments_count, char *output_buffer, size_t output_buffer_length)
{
	static const char header[] = "Task\t\tAbs Time\t% Time\r\n--------------------------------------";

	(void)arguments_array;					// suppress warning
	(void)arguments_count;					// suppress warning

	char *buffer = (char*)pvPortMalloc(1024);

	if (buffer == NULL)
		return ERROR_FreeRTOS_errCOULD_NOT_ALLOCATE_REQUIRED_MEMORY;

	vTaskGetRunTimeStats((signed char*)buffer);

	size_t length = strlen(buffer);

	enum Error error = ERROR_NONE;

	if ((length + sizeof (header)) < output_buffer_length)
	{
		memcpy(output_buffer, header, sizeof(header) - 1);
		memcpy(output_buffer + sizeof(header) - 1, buffer, length + 1);
		error = ERROR_NONE;
	}
	else
		error = ERROR_BUFFER_OVERFLOW;

	vPortFree(buffer);

	return error;
}

/**
 * \brief Handler of 'tasklist' command.
 * Handler of 'tasklist' command. Displays all tasks with their info.
 *
 * \param [out] output_buffer is the pointer to output buffer
 * \param [in] output_buffer_length is the size of output buffer
 *
 * \return ERROR_NONE on success, otherwise an error code defined in the file error.h
 */

static enum Error _tasklistHandler(const char **arguments_array, uint32_t arguments_count, char *output_buffer, size_t output_buffer_length)
{
	static const char header[] = "Task\t\tState\tPri.\tStack\t##\r\n------------------------------------------";

	(void)arguments_array;					// suppress warning
	(void)arguments_count;					// suppress warning

	char *buffer = (char*)pvPortMalloc(1024);

	if (buffer == NULL)
		return ERROR_FreeRTOS_errCOULD_NOT_ALLOCATE_REQUIRED_MEMORY;

	vTaskList((signed char*)buffer);

	size_t length = strlen(buffer);

	enum Error error = ERROR_NONE;

	if ((length + sizeof (header)) < output_buffer_length)
	{
		memcpy(output_buffer, header, sizeof(header) - 1);
		memcpy(output_buffer + sizeof(header) - 1, buffer, length + 1);
		error = ERROR_NONE;
	}
	else
		error = ERROR_BUFFER_OVERFLOW;

	vPortFree(buffer);

	return error;
}
