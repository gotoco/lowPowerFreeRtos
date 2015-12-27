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
#include "stm32l1xx_hal.h"
#include "usb_device.h"
#include "hdr/hdr_rcc.h"
#include "rcc.h"
#include "config.h"
#include "hdr_gpio.h"
#include "stm32l152xc.h"

#include "io_syscalls.h"

/* Private variables ---------------------------------------------------------*/
#include "command.hpp"
#include "etrx2.hpp"
#include "etrx2_cli.hpp"
#include "data_producer.hpp"
#include "usart.h"

#include <new>

#include <cstdio>
#include <cstring>
#include <cassert>

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void GPIO_Init(void);
static void _sysInit(void);
static void _setVCore(void);
static void _configureHSE(void);
static void _configureHSI(void);
static void _startPLL(void);

#if !defined  (HSE_VALUE)
  #define HSE_VALUE    ((uint32_t)8000000) /*!< Value of the External oscillator in Hz */
#endif /* HSE_VALUE */

#if !defined  (HSI_VALUE)
  #define HSI_VALUE    ((uint32_t)16000000) /*!< Value of the Internal oscillator in Hz*/
#endif /* HSI_VALUE */

static void _heartbeatTask(void *parameters);
static enum Error _initializeHeartbeatTask(void);

int main(void)
{
  /* Configure the system clock */
  _sysInit();

  /* Initialize all configured peripherals */
  GPIO_Init();
  //  USB_DEVICE_Init();

  _initializeHeartbeatTask();

  /* Infinite loop */
  vTaskStartScheduler();
  while (1){

  }

  return 0;
}


static void _heartbeatTask(void *parameters)
{
	(void)parameters;						// suppress warning
	FILE *  uart1_rx;
	FILE *  uart1_tx;
	portTickType xLastHeartBeat;

	xLastHeartBeat = xTaskGetTickCount();
	int ret = ioSyscallsInitialize();

	uart1_tx = fopen("/dev/uart0", "w");
	setvbuf(uart1_tx, nullptr, _IOFBF, STREAM_BUFFER_SIZE);

	uart1_rx = fopen("/dev/uart0", "r");
	setvbuf(uart1_rx, nullptr, _IOLBF, STREAM_BUFFER_SIZE);

	char buffer[10];
	size_t size = 10;
	for(;;){

		int read_size = fread(buffer, sizeof(char), size, uart1_rx);
		int length = strlen(buffer);

		fwrite(buffer, sizeof(buffer[0]), length, uart1_tx);
		fflush(uart1_tx);

		vTaskDelay(40/portTICK_RATE_MS);	//Then go sleep
	}

}

static enum Error _initializeHeartbeatTask(void)
{
	portBASE_TYPE ret = xTaskCreate(_heartbeatTask, (signed char*)"heartbeat", 128, NULL,
			HEARTBEAT_TASK_PRIORITY, NULL);

	return errorConvert_portBASE_TYPE(ret);
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

	/**
	 * System Clock Configuration
	 */
	_configureHSI();

	rccStartPll(RCC_PLL_INPUT_HSI, HSI_VALUE, FREQUENCY);

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

