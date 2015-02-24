
#include "power_management.h"

//Third party software
#include "FreeRTOS.h"
#include "task.h"
#include "FreeRTOSConfig.h"

#include "gpio.h"
#include "rtc.h"
#include "nvic.h"
#include "exti.h"
#include "st_rcc.h"
#include "pwr.h"
#include "stm32l1xx_gpio.h"
#include "config.h"
#include "bsp.h"

GPIOInitTypeDef GPIOInitStructure;

void GPIO_SHOUTDOWN(void);


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

/**
 * \brief power save task to run when there is no other work to do by system
 */
void power_save_task(void *parameters)
{


	  while (1)
	  {
	    /* Enable Wakeup Counter */
	    RTC_WakeUpCmd(ENABLE);
//	    GPIO_SHOUTDOWN();

	    /* Enter Stop Mode */
	    /* After this microcontroller will be invisible for debuger and will be able
	     * to wakup only by RESET PIN or RTC wakeup
	     */
	    PWR_EnterSTOPMode(PWR_Regulator_LowPower, PWR_STOPEntry_WFI);

	    /* Enable Wakeup Counter */
	    RTC_WakeUpCmd(DISABLE);

	    /* After wake-up from STOP reconfigure the system clock */
		_configureHSI();

		rccStartPll(RCC_PLL_INPUT_HSI, HSI_VALUE, FREQUENCY);

		gpioInitialize();
		gpioConfigurePin(LED_GPIO, LED_pin_1, GPIO_OUT_PP_2MHz);

		for(int i=0; i<3; i++){
			for(int j=0; j<100000; j++);
		}

		vTaskDelay(10/portTICK_RATE_MS);//CONTEXT_SWITCH();
	  }
}


void GPIO_SHOUTDOWN(void)
{
	  /* Configure all GPIO as analog to reduce current consumption on non used IOs */
	  /* Enable GPIOs clock */
	  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA | RCC_AHBPeriph_GPIOB | RCC_AHBPeriph_GPIOC |
	                        RCC_AHBPeriph_GPIOD | RCC_AHBPeriph_GPIOE | RCC_AHBPeriph_GPIOH, ENABLE);

	  GPIOInitStructure.GPIO_Mode = GPIO_Mode_AN;
	  GPIOInitStructure.GPIO_Speed = GPIO_Speed_40MHz;
	  GPIOInitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	  GPIOInitStructure.GPIO_Pin = GPIO_Pin_All;
	  GPIO_Init(GPIOC, &GPIOInitStructure);
	  GPIO_Init(GPIOD, &GPIOInitStructure);
	  GPIO_Init(GPIOE, &GPIOInitStructure);
	  GPIO_Init(GPIOH, &GPIOInitStructure);
	  GPIO_Init(GPIOA, &GPIOInitStructure);
	  GPIO_Init(GPIOB, &GPIOInitStructure);

	  /* Disable GPIOs clock */
	  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA | RCC_AHBPeriph_GPIOB | RCC_AHBPeriph_GPIOC |
	                        RCC_AHBPeriph_GPIOD | RCC_AHBPeriph_GPIOE | RCC_AHBPeriph_GPIOH, DISABLE);
}
