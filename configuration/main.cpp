/** \file main.cpp
 * \brief Main code block
 *
 * Main code block
 *
 * project: mg-stm32l_acquisition_supervisor; chip: STM32L152RB
 *
 * \author Mazeryt Freager
 * \date 2012-09-06
 */

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "FreeRTOS.h"
#include "task.h"

#include "ff.h"

#include "hdr/hdr_rcc.h"

// ./
#include "config.h"
#include "bsp.h"
#include "error.h"
#include "command.h"

//Peripherials
#include "gpio.h"
#include "rcc.h"
#include "helper.h"
#include "i2c.h"
#include "serial.h"
#include "usart.h"
#include "lcd.h"
#include "timer.h"

//Drivers
#include "FC30.h"
#include "MCP980x.h"
#include "M41T56C64.h"

//Application
#include "lcdprinter.h"
#include "button.h"
#include "service.h"

#define ACC_INT_PIN							GPIO_PIN_0
#define ACC_INT_GPIO						GPIOA
#define ACC_INT_CONFIGURATION				GPIO_IN_PULL_DOWN

/*---------------------------------------------------------------------------------------------------------------------+
| local functions' declarations
+---------------------------------------------------------------------------------------------------------------------*/

static void _heartbeatTask(void *parameters);
static enum Error _initializeHeartbeatTask(void);
static enum Error _initializeGuardianTask(void);
static enum Error _peripLauncher(void);
static enum Error _sysInit(void);
static enum Error _schedulerInitAndRun(void);
static void delay(uint32_t ms);
static void countTicks(void);


/*---------------------------------------------------------------------------------------------------------------------+
| extern functions' declarations
+---------------------------------------------------------------------------------------------------------------------*/


/*---------------------------------------------------------------------------------------------------------------------+
| local variables
+---------------------------------------------------------------------------------------------------------------------*/

static FATFS _fileSystem;

/*---------------------------------------------------------------------------------------------------------------------+
| global variables
+---------------------------------------------------------------------------------------------------------------------*/

uint32_t delayTicks=0;
uint32_t ticks=0;
uint8_t delayFlag=0;
uint8_t buttonFlag=0;
uint8_t msFlag=0;
uint8_t serviceFlag=0;
char rx;

/*---------------------------------------------------------------------------------------------------------------------+
| global functions
+---------------------------------------------------------------------------------------------------------------------*/

/**
 * \brief System root Task
 */
int main(void)
{
	float themperature=0;

	uint8_t time[3];

	char string[20];

	enum Error sysError = _sysInit();
	if (sysError != ERROR_NONE){
		goto sys_panic;
	}

	sysError = _peripLauncher();
	if (sysError != ERROR_NONE){
		goto sys_panic;
	}

	while(1)
	{
		if(serviceFlag){
			serviceMode();
			serviceFlag=0;
		}

		else if(msFlag){
			msFlag=0;

			M41T56C64_ReadTime(time);
			M41T56C64_ConvertToInt(time);
			LCD_WriteTime(time);
		}

		/*if(buttonFlag==1){
			themperature=MCP980x_Single_Measure();
			LCD_WriteFloat(&themperature,2,1);
		}
		else{
			strcpy(string,"ub.ir.ds");
			LCD_WriteString(string);
		}*/
	}

	goto sys_panic;

	sys_panic:
		while (1);
}

/*---------------------------------------------------------------------------------------------------------------------+
| local functions implementation
+---------------------------------------------------------------------------------------------------------------------*/

/**
 *  \brief Initialize uC Core and sysCLK
 *
 *  \return ERROR_NONE on success, otherwise appropriate error code from error.h
 */
static enum Error _sysInit()
{
	Error error = ERROR_NONE;

	RCC_APB1ENR_PWREN_bb = 1;
	PWR->CR = (PWR->CR & (~PWR_CR_VOS)) | PWR_CR_VOS_0;	// set VCORE voltage range 1 (1.8V)
	while((PWR->CSR & PWR_CSR_VOSF) != 0);	// wait for regulator ready

	RCC_CR_HSION_bb = 1;					// enable HSI oscillator
	while (RCC_CR_HSIRDY_bb == 0);

	rccStartPll(RCC_PLL_INPUT_HSI, HSI_VALUE, FREQUENCY);
	Timer_Init();
	return error;
}

/**
 *  \brief Initialize tasks and run scheduler.
 *  At this point RTOS expropriates control from root task.
 *
 *  \return: This function should not never escape, if so that something goes wrong and scheduler fail.
 */
static enum Error _schedulerInitAndRun(){
	Error error = ERROR_FreeRTOS_errSCHEDULER_FAIL;

	FRESULT fresult = f_mount(0, &_fileSystem);	// try mounting the filesystem on SD card
	ASSERT("f_mount()", fresult == FR_OK);

	error = _initializeHeartbeatTask();
	ASSERT("_initializeHeartbeatTask()", error == ERROR_NONE);

	error = _initializeGuardianTask();

	vTaskStartScheduler();

	return error;
}

/**
 *  \brief Launcher for used peripherals
 *
 *  \return ERROR_NONE on success, otherwise appropriate error code from error.h
 */
static enum Error _peripLauncher()
{
	Error error = ERROR_NONE;

	char string[20];

	gpioInitialize();

	uint8_t time[3];

	// LED Initialise
	gpioConfigurePin(LED_GPIO, LED_pin, GPIO_OUT_PP_2MHz);
	LED_bb = 1;
	gpioConfigurePin(LED_GPIO, LED_pin_1, GPIO_OUT_PP_2MHz);
	LED1_bb = 1;

	// LCD Initialize
	LCD_Init();

	LCD_WriteString_example();

	time[0]=15;
	time[1]=51;
	time[2]=20;

	// RTC Initialise
	M41T56C64_Init(time);

	M41T56C64_ReadTime(time);

	M41T56C64_ConvertToInt(time);

	LCD_WriteTime(time);

	// Thermometer Initialize
	//MCP980x_Init();


	// Timer2 Initialize
	Timer_Init();

	// Blue button Initialize
	//_initializeGuardianTask();

	ServicePin_Init();

	out:
		return error;
}


/*---------------------------------------------------------------------------------------------------------------------+
| System Tasks
+---------------------------------------------------------------------------------------------------------------------*/


/**
 * \brief Heartbeat task that read measured data from slaves
 *
 * This is a main task in this device
 */

static void _heartbeatTask(void *parameters)
{
	(void)parameters;						// suppress warning

	portTickType xLastHeartBeat;

	Error error = ERROR_NONE;

	xLastHeartBeat = xTaskGetTickCount();

	for(;;) {

		vTaskDelayUntil(&xLastHeartBeat, 250 / portTICK_RATE_MS);

		LED_bb ^= 1;

	}

}


/**
 * \brief Initialization of guardian task - setup GPIO, create task.
 *
 * Initialization of Interrupt case PA0 is pulled up
 *
 * \return ERROR_NONE if the task was successfully created and added to a ready list, otherwise an error code defined in
 * the file error.h
 */

static enum Error _initializeGuardianTask(void)
{
	//gpioConfigurePin(LED_GPIO, LED_pin, GPIO_OUT_PP_2MHz);

	gpioConfigurePin(ACC_INT_GPIO, ACC_INT_PIN, ACC_INT_CONFIGURATION);

	//setting interupt for PA0;
	EXTI->IMR|=EXTI_EMR_MR0;
	EXTI->RTSR|=EXTI_RTSR_TR0;
	EXTI->FTSR&= ~(EXTI_FTSR_TR0);

	//enabling interupt
	RCC->APB2ENR|=RCC_APB2ENR_SYSCFGEN;
	SYSCFG->EXTICR[0]=SYSCFG_EXTICR1_EXTI0_PA;
	NVIC_EnableIRQ(EXTI0_IRQn);

	return ERROR_NONE;
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
	gpioConfigurePin(LED_GPIO, LED_pin, GPIO_OUT_PP_2MHz);

	portBASE_TYPE ret = xTaskCreate(_heartbeatTask, (signed char*)"heartbeat", HEARTBEAT_STACK_SIZE, NULL,
			HEARTBEAT_TASK_PRIORITY, NULL);

	return errorConvert_portBASE_TYPE(ret);
}

/**
 * \brief Counts ticks
 */
static void countTicks()
{
	delayTicks++;
}

/**
 * \brief Delays designed time in milliseconds
 *
 * \param Number of milliseconds to wait
 */
static void delay(uint32_t ms)
{
	delayTicks=0;
	delayFlag=1;
	while(delayTicks<ms);
	delayFlag=0;
}

extern "C" void EXTI0_IRQHandler(void) __attribute((interrupt));
void EXTI0_IRQHandler(void)
{
	//Do something
	buttonFlag^=1;

	//Clear flags
	EXTI->PR=EXTI_PR_PR0;
}

extern "C" void EXTI4_IRQHandler(void) __attribute((interrupt));
void EXTI4_IRQHandler(void)
{
	//Do something
	serviceFlag=1;

	//Clear flags
	EXTI->PR=EXTI_PR_PR4;
}

extern "C" void TIM2_IRQHandler(void) __attribute((interrupt));
void TIM2_IRQHandler(void)
{
	//Do something
	if(delayFlag==1) countTicks();

	ticks++;
	if(ticks>=1000){
		ticks=0;
		msFlag=1;
	}

	//Clear flags
	TIM2->SR = 0;
}

extern "C" void SysTick_Handler(void) __attribute((interrupt));
void SysTick_Handler(void)
{
	if(delayFlag==1) countTicks();
}

extern "C" void USART1_IRQHandler(void) __attribute((interrupt));
void USART1_IRQHandler(void)
{

}


