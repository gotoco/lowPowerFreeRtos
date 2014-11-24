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

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

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
#include "usart.h"
#include "i2c.h"

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


/*---------------------------------------------------------------------------------------------------------------------+
| global functions
+---------------------------------------------------------------------------------------------------------------------*/

/**
 * \brief System root Task
 */
int main(void)
{
	enum Error sysError = _sysInit();
	if (sysError != ERROR_NONE){
		goto sys_panic;
	}

	sysError = _peripLauncher();
	if (sysError != ERROR_NONE){
		goto sys_panic;
	}

	_schedulerInitAndRun();

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

	gpioInitialize();

	i2cInitialize();

	error = usartInitialize();

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
	gpioConfigurePin(LED_GPIO, LED_pin, GPIO_OUT_PP_2MHz);

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

extern "C" void EXTI0_IRQHandler(void) __attribute((interrupt));
void EXTI0_IRQHandler(void)
{
	//Do something

	//Clear flags
	EXTI->PR=EXTI_PR_PR0;
}

