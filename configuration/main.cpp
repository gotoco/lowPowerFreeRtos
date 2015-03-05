/** \file main.cpp
 * \brief Main code block
 *
 * Main code block
 *
 * project: mg-stm32l_acquisition_supervisor; chip: STM32L152RB
 *
 * \author Mazeryt Freager
 * \author Adrian Ratajczyk
 * \date 2012-09-06
 */

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "stm32l152xc.h"
#include "stm32l1xx_hal.h"

//Third party software
#include "FreeRTOS.h"
#include "task.h"

#include "ff.h"

//configuration package
#include "power_management.h"
#include "command.h"
#include "config.h"
#include "error.h"
#include "bsp.h"

//hardware and peripherals
#include "hdr/hdr_rcc.h"
#include "hdr/hdr_gpio.h"

#include "gpio.h"
#include "rcc.h"
#include "helper.h"
#include "usart.h"
#include "rtc.h"
#include "nvic.h"
#include "exti.h"
#include "st_rcc.h"
#include "pwr.h"
#include "stm32l1xx_gpio.h"
#include "i2c_by_PG.h"
#include "MCP980x_drv_by_PG.h"

/* Private variables ---------------------------------------------------------*/


/* Private function prototypes -----------------------------------------------*/
static void _sysInit(void);
static void _setVCore(void);
static void _configureHSE(void);
static void _configureHSI(void);
static void _startPLL(void);

/*---------------------------------------------------------------------------------------------------------------------+
| local functions' declarations
+---------------------------------------------------------------------------------------------------------------------*/

static enum Error _dirHandler(const char **arguments_array, uint32_t arguments_count, char *output_buffer,
		size_t output_buffer_length);
static void _heartbeatTask(void *parameters);
static enum Error _initializeHeartbeatTask(void);
static enum Error _initializePowerSaveTask(void);
static enum Error _runtimestatsHandler(const char **arguments_array, uint32_t arguments_count, char *output_buffer,
		size_t output_buffer_length);
static enum Error _tasklistHandler(const char **arguments_array, uint32_t arguments_count, char *output_buffer,
		size_t output_buffer_length);

void myTask();
void initializeMyTask();
void ButtonIRQInit();

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

void rtcConfig(void);

/*---------------------------------------------------------------------------------------------------------------------+
| root task
+---------------------------------------------------------------------------------------------------------------------*/
int main(void)
{
	/* Configure the system clock and pll*/
	_sysInit();
	//ButtonIRQInit();


	gpioInitialize();
	gpioConfigurePin(GPIOA, BUTTON_PIN, BUTTON_CONFIGURATION);

	enum Error error = usartInitialize();
	//??
	//i2cInitialize();

	FRESULT fresult = f_mount(0, &_fileSystem);	// try mounting the filesystem on SD card
	ASSERT("f_mount()", fresult == FR_OK);

	error = _initializeHeartbeatTask();
	ASSERT("_initializeHeartbeatTask()", error == ERROR_NONE);

	error = _initializePowerSaveTask();
	ASSERT("_initializeHeartbeatTask()", error == ERROR_NONE);

	initializeMyTask();
	gpioInitialize();



	/*	Special delay for debugging because after scheduler start
	 * 	it may be hard to catch core in run mode to connect to debugger
	 */
	for(int i=0; i<10; i++){ LED1_bb ^= 1; for(int i=0; i<1000000; i++); }

	commandRegister(&_dirCommandDefinition);
	commandRegister(&_runtimestatsCommandDefinition);
	commandRegister(&_tasklistCommandDefinition);

	vTaskStartScheduler();

	while (1)
	{ }

}

void GPIO_Init(void)
{

  /* GPIO Ports Clock Enable */
	RCC->AHBENR |= RCC_AHBENR_GPIOAEN | RCC_AHBENR_GPIOBEN | RCC_AHBENR_GPIOCEN | RCC_AHBENR_GPIODEN |
			RCC_AHBENR_GPIOEEN | RCC_AHBENR_GPIOHEN;

}

static void _sysInit(void)
{
	_setVCore();

	/* System Clock Configuration */
	_configureHSI();

	rccStartPll(RCC_PLL_INPUT_HSI, HSI_VALUE, FREQUENCY);

	rtcConfig();
}

static void _setVCore(void)
{
	RCC_APB1ENR_PWREN_bb = 1;
	PWR->CR = (PWR->CR & (~PWR_CR_VOS)) | PWR_CR_VOS_0;	// set VCORE voltage range 1 (1.8V)
	while((PWR->CSR & PWR_CSR_VOSF) != 0);				// wait for regulator ready
}

static void _configureHSE(void)
{
	RCC_CR_HSEON_bb = 1;					// enable HSE oscillator
	while (RCC_CR_HSERDY_bb == 0);			// wait till READY
}

static void _configureHSI(void)
{
	RCC_CR_HSION_bb = 1;					// enable HSI oscillator
	while (RCC_CR_HSIRDY_bb == 0);			// wait till READY
}

static void _startPLL(void)
{
	if(CLOCK_SOURCE == USING_HSI){
		rccStartPll(RCC_PLL_INPUT_HSI, HSI_VALUE, FREQUENCY);
	}
	else if(CLOCK_SOURCE == USING_HSE) {
		rccStartPll(RCC_PLL_INPUT_HSE, HSE_VALUE, FREQUENCY);
	}

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
 * Moja własna funkcja, sterowanie drugim ledem
 */
void myTask()
{

	float temp;
	initMCP980x(MCP_MSB_ADDR);
	setConfRegister(0x61,MCP_MSB_ADDR);
	for(;;)
	{
		int bv=1;
		gpioInitialize();
		gpioConfigurePin(LED_GPIO, LED_pin, GPIO_OUT_PP_2MHz);
		gpioConfigurePin(GPIOA, BUTTON_PIN, BUTTON_CONFIGURATION);

		bv=(int)(HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_0 ));
		//odczytanie pozycji przycisku
		if(bv){
			setConfRegister(0x01, MCP_MSB_ADDR);
			LED_bb |=0x01;
			readTemperature(&temp, 0x48);
		}	else
			LED_bb &=~0x01;
		//włączenie diody jeśli przycisk jest włączony
		vTaskDelay(100/portTICK_RATE_MS);
	}
}

void initializeMyTask()
{
	xTaskHandle xHandleMyTask;

	xTaskCreate(myTask, (signed char*)"myTask", HEARTBEAT_STACK_SIZE, NULL, HEARTBEAT_TASK_PRIORITY, &xHandleMyTask );


}


/**
 * \brief Heartbeat task that simulate real system behavior
 *
 * This is a main task in this device
 */

static void _heartbeatTask(void *parameters)
{
	(void)parameters;						// suppress warning

	portTickType xLastHeartBeat;

	xLastHeartBeat = xTaskGetTickCount();
	float temp;

	for(;;){
		int a = 0,b = 0;
		gpioInitialize();
		gpioConfigurePin(LED_GPIO, LED_pin_1, GPIO_OUT_PP_2MHz);

		LED1_bb ^= 1;
		for(int i=0; i<700000; i++) a = 2*b+1; //do some fake calculations
		LED1_bb ^= 1;
		readTemperature(&temp, 0x48);

		vTaskDelay(100/portTICK_RATE_MS);	//Then go sleep
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
	portBASE_TYPE ret = xTaskCreate(_heartbeatTask, (signed char*)"heartbeat", HEARTBEAT_STACK_SIZE, NULL,
			HEARTBEAT_TASK_PRIORITY, NULL);

	return errorConvert_portBASE_TYPE(ret);
}

/**
 * \brief Initialization of power save task
 *
 * \return ERROR_NONE if the task was successfully created and added to a ready list, otherwise an error code defined in
 * the file error.h
 */

static enum Error _initializePowerSaveTask(void)
{
	portBASE_TYPE ret = xTaskCreate(system_idle_task, (signed char*)"system_idle_task", POWER_OFF_TASK_STACK_SIZE, NULL,
			POWER_OFF_TASK_PRIORITY, NULL);

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

/**
  * @brief  Configures the RTC Wakeup.
  * @param  None
  * @retval None
  */
void rtcConfig(void)
{
  NVIC_InitTypeDef  NVIC_InitStructure;
  EXTI_InitTypeDef  EXTI_InitStructure;

  RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);

  /*!< Allow access to RTC */
  PWR_RTCAccessCmd(ENABLE);

  /*!< Reset RTC Domain */
  RCC_RTCResetCmd(ENABLE);
  RCC_RTCResetCmd(DISABLE);

	/* The RTC Clock may varies due to LSI frequency dispersion. */
	/* Enable the LSI OSC */
	RCC_LSICmd(ENABLE);

	/* Wait till LSI is ready */
	while (RCC_GetFlagStatus(RCC_FLAG_LSIRDY) == RESET) {
	}

	/* Select the RTC Clock Source */
	RCC_RTCCLKConfig(RCC_RTCCLKSource_LSI);


  /*!< Enable the RTC Clock */
  RCC_RTCCLKCmd(ENABLE);

  /*!< Wait for RTC APB registers synchronisation */
  RTC_WaitForSynchro();

  /* EXTI configuration *******************************************************/
  EXTI_ClearITPendingBit(EXTI_Line20);
  EXTI_InitStructure.EXTI_Line = EXTI_Line20;
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  EXTI_Init(&EXTI_InitStructure);

  /* Enable the RTC Wakeup Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = RTC_WKUP_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);

  /* RTC Wakeup Interrupt Generation: Clock Source: RTCDiv_16, Wakeup Time Base: 4s */
  RTC_WakeUpClockConfig(RTC_WakeUpClock_RTCCLK_Div4);
  RTC_SetWakeUpCounter(0x1FFF); //(0x320) ;

  /* Enable the Wakeup Interrupt */
  RTC_ITConfig(RTC_IT_WUT, ENABLE);
}


