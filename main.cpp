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

#include "stm32l1xx.h"

#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

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
#include "SD.h"
#include "spi.h"
//#include "SD_test.h"

/*---------------------------------------------------------------------------------------------------------------------+
| local functions' declarations
+---------------------------------------------------------------------------------------------------------------------*/

static enum Error _dirHandler(const char **arguments_array, uint32_t arguments_count, char *output_buffer,
		size_t output_buffer_length);

static void _led1Task(void *parameters);
static enum Error _initializeLed1Task(void);

static void _led2Task(void *parameters);
static enum Error _initializeLed2Task(void);

static void _led3Task(void *parameters);
static enum Error _initializeLed3Task(void);

static void _buttonPressTask(void);
static enum Error _initializeHeartbeatTask(void);

static void _heartbeatTask(void *parameters);
static enum Error _initializeButtonPressTask(void);

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

// semafor
xSemaphoreHandle xSemaphoreSW0 = NULL;

// kolejka
xQueueHandle xQueue2 = NULL;




typedef enum {FAILED = 0, PASSED = !FAILED} TestStatus;

#define  BufferSize     512

uint8_t Buffer_Block_Tx[BufferSize], Buffer_Block_Rx[BufferSize];
TestStatus TransferStatus1 = FAILED;
uint8_t Status = 0;

void Fill_Buffer(uint8_t *pBuffer, uint16_t BufferLenght, uint8_t Offset);
TestStatus Buffercmp(uint8_t* pBuffer1, uint8_t* pBuffer2, uint16_t BufferLength);



int main(void)
{
	RCC_APB1ENR_PWREN_bb = 1;
	PWR->CR = (PWR->CR & (~PWR_CR_VOS)) | PWR_CR_VOS_0;	// set VCORE voltage range 1 (1.8V)
	while((PWR->CSR & PWR_CSR_VOSF) != 0);	// wait for regulator ready

	RCC_CR_HSION_bb = 1;					// enable HSI oscillator
	while (RCC_CR_HSIRDY_bb == 0);

	rccStartPll(RCC_PLL_INPUT_HSI, HSI_VALUE, FREQUENCY);

	gpioInitialize();

	spiInitialize();

	uint8_t tx=65;

	while(1)
	{
		spiWrite(&tx,1);
	}

	//i2cInitialize();

	//usartInitialize();

	xQueue2 = xQueueCreate(1, sizeof(uint8_t));

	enum Error error;// = usartInitialize();

	Status = SD_Init();

	while(1)
	{
		SD_WriteByte(65);
	}

	//Status = SD_test(Buffer_Block_Tx, Buffer_Block_Rx, BufferSize);
	Fill_Buffer(&Buffer_Block_Tx[0], BufferSize, 0x0);
	SD_WriteBlock(&Buffer_Block_Tx[0], 0, BufferSize);
	SD_ReadBlock(&Buffer_Block_Rx[0], 0, BufferSize);
	TransferStatus1 = Buffercmp(Buffer_Block_Tx, Buffer_Block_Rx, BufferSize);
	/*if (TransferStatus1 == PASSED)
	{
		return 0;
	}
	else
	{
	    return 1;
	}*/

//
//	FRESULT fresult = f_mount(0, &_fileSystem);	// try mounting the filesystem on SD card
//	ASSERT("f_mount()", fresult == FR_OK);
//
	error = _initializeHeartbeatTask();
	ASSERT("_initializeHeartbeatTask()", error == ERROR_NONE);

	error = _initializeButtonPressTask();
	ASSERT("_initializeButtonPressTask()", error == ERROR_NONE);

	error = _initializeLed1Task();
	ASSERT("_initializeLed1Task()", error == ERROR_NONE);

	error = _initializeLed2Task();
	ASSERT("_initializeLed2Task()", error == ERROR_NONE);

	error = _initializeLed3Task();
	ASSERT("_initializeLed3Task()", error == ERROR_NONE);

//	commandRegister(&_dirCommandDefinition);
//	commandRegister(&_runtimestatsCommandDefinition);
//	commandRegister(&_tasklistCommandDefinition);


	// uruchomienie zadan
	//vStartLDTasks(TASK_PRIORITY);

	// uruchomienie planisty
	vTaskStartScheduler();

	while (1)
	{ }
}

void Fill_Buffer(uint8_t *pBuffer, uint16_t BufferLenght, uint8_t Offset)
{
	uint16_t IndexTmp;

	/* Put in global buffer same values */
	for( IndexTmp = 0; IndexTmp < BufferLenght; IndexTmp++ )
	{
		pBuffer[IndexTmp] = IndexTmp + Offset;
	}
}

TestStatus Buffercmp(uint8_t* pBuffer1, uint8_t* pBuffer2, uint16_t BufferLength)
{
	while (BufferLength--)
	{
		if (*pBuffer1 != *pBuffer2)
		{
			return FAILED;
	    }

	    pBuffer1++;
	    pBuffer2++;
	}
	return PASSED;
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

	uint8_t ile;

	char string[10];

	for(;;){
		vTaskDelay(1000/portTICK_RATE_MS);

		xQueueReceive(xQueue2, &ile, (portTickType) 0);

		if(ile<7) ile++;
		else	ile=0;

		xQueueSend(xQueue2, ( void * ) &ile, (portTickType) 0 );

		strcpy(string,"uBirds\r\n");
		usartSendString(string, 10);

		//Test interface
		//LED2_bb ^= 1;
		//LED1_bb ^= LED_bb;
		//LED3_bb ^= LED2_bb;

		//const char* twochars = "test_";
		//usartSendString(twochars, 100);
	}

}

static enum Error _initializeHeartbeatTask(void)
{
	gpioConfigurePin(LED_GPIO, LED_pin, GPIO_OUT_PP_2MHz);
	gpioConfigurePin(LED_GPIO, LED_pin_1, GPIO_OUT_PP_2MHz);
	gpioConfigurePin(LED2_GPIO, LED_pin_2, GPIO_OUT_PP_2MHz);
	//gpioConfigurePin(LED_GPIO, LED_pin_2, GPIO_OUT_PP_2MHz);
	//gpioConfigurePin(LED_GPIO, LED_pin_3, GPIO_OUT_PP_2MHz);

	LED_bb = 0;
	LED1_bb = 0;
	LED2_bb = 0;

	uint8_t ile=0;
	ile=0;

	xQueueSend(xQueue2, ( void * ) &ile, (portTickType) 0 );

	portBASE_TYPE ret = xTaskCreate(_heartbeatTask, (signed char*)"heartbeat", HEARTBEAT_STACK_SIZE, NULL,
			HEARTBEAT_TASK_PRIORITY, NULL);

	return errorConvert_portBASE_TYPE(ret);
}

static void _led1Task(void *parameters)
{
	(void)parameters;						// suppress warning

	portTickType xLastLedBlink;

	xLastLedBlink = xTaskGetTickCount();

	uint8_t ile;

	for(;;)
	{
		xQueueReceive(xQueue2, &ile, (portTickType) 0);

		if(ile & 1) LED_bb = 1;
		else 		LED_bb = 0;

		xQueueSend(xQueue2, ( void * ) &ile, (portTickType) 0 );
		vTaskDelay(1000/portTICK_RATE_MS);
	}
}

static enum Error _initializeLed1Task(void)
{
	portBASE_TYPE ret = xTaskCreate(_led1Task, (signed char*)"led1", LED_STACK_SIZE, NULL,
			LED_TASK_PRIORITY+1, NULL);

	return errorConvert_portBASE_TYPE(ret);
}

static void _led2Task(void *parameters)
{
	(void)parameters;						// suppress warning

	portTickType xLastLedBlink;

	xLastLedBlink = xTaskGetTickCount();

	uint8_t ile;

	for(;;)
	{
		xQueueReceive(xQueue2, &ile, (portTickType) 0);

		if(ile & 2) LED1_bb = 1;
		else 		LED1_bb = 0;

		xQueueSend(xQueue2, ( void * ) &ile, (portTickType) 0 );
		vTaskDelay(1000/portTICK_RATE_MS);
	}
}

static enum Error _initializeLed2Task(void)
{
	portBASE_TYPE ret = xTaskCreate(_led2Task, (signed char*)"led2", LED_STACK_SIZE, NULL,
			LED_TASK_PRIORITY+2, NULL);

		return errorConvert_portBASE_TYPE(ret);
}

static void _led3Task(void *parameters)
{
	(void)parameters;						// suppress warning

	portTickType xLastLedBlink;

	xLastLedBlink = xTaskGetTickCount();

	uint8_t ile;

	for(;;)
	{
		xQueueReceive(xQueue2, &ile, (portTickType) 0);

		if(ile & 4) LED2_bb = 1;
		else 		LED2_bb = 0;

		xQueueSend(xQueue2, ( void * ) &ile, (portTickType) 0);
		vTaskDelay(1000/portTICK_RATE_MS);
	}
}

static enum Error _initializeLed3Task(void)
{
	portBASE_TYPE ret = xTaskCreate(_led3Task, (signed char*)"led3", LED_STACK_SIZE, NULL,
			LED_TASK_PRIORITY+3, NULL);

	return errorConvert_portBASE_TYPE(ret);
}

/**
 * \brief Initialization of heartbeat task - setup GPIO, create task.
 *
 * Initialization of heartbeat task - setup GPIO, create task.
 *
 * \return ERROR_NONE if the task was successfully created and added to a ready list, otherwise an error code defined in
 * the file error.h
 */


static void _buttonPressTask(void * pvParameters)
{
	vSemaphoreCreateBinary(xSemaphoreSW0);

	for(;;)
	{
		if(xSemaphoreTake(xSemaphoreSW0, 0) == pdTRUE)
		{
			//vhToggleLED1();
			LED1_bb = 1-LED1_bb;
		}
	}
}

static enum Error _initializeButtonPressTask(void)
{
	gpioConfigurePin(BUTTON_GPIO,BUTTON_PIN,BUTTON_CONFIGURATION);

	//setting interrupt for PA0;
	EXTI->IMR|=EXTI_EMR_MR0;
	EXTI->RTSR|=EXTI_RTSR_TR0;
	EXTI->FTSR&= ~(EXTI_FTSR_TR0);

	//enabling interrupt
	RCC->APB2ENR|=RCC_APB2ENR_SYSCFGEN;
	SYSCFG->EXTICR[0]=SYSCFG_EXTICR1_EXTI0_PA;
	NVIC_SetPriority(EXTI0_IRQn, 2);
	NVIC_EnableIRQ(EXTI0_IRQn);

	portBASE_TYPE ret = xTaskCreate(_buttonPressTask, (signed char*)"buttonpress", BUTTON_STACK_SIZE, NULL,
			BUTTON_TASK_PRIORITY, NULL);

	return errorConvert_portBASE_TYPE(ret);
}

extern "C" void EXTI0_IRQHandler(void) __attribute((interrupt));
void EXTI0_IRQHandler(void)
{
	static portBASE_TYPE xHigherPriorityTaskWoken;
	xHigherPriorityTaskWoken = pdFALSE;

	//if(EXTI_GetITStatus(EXTI_Line0) != RESET)
	//{
		xSemaphoreGiveFromISR(xSemaphoreSW0, &xHigherPriorityTaskWoken);

		EXTI->PR=EXTI_PR_PR0;
	//}
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
