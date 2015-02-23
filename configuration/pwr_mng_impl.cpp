
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

void rtcConfig(void);
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
	    GPIO_SHOUTDOWN();

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

		for(int i=0; i<3; i++){
			LED1_bb ^= 1;

			for(int j=0; j<100000; j++);
		}

		CONTEXT_SWITCH();
	  }
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
  RTC_WakeUpClockConfig(RTC_WakeUpClock_RTCCLK_Div16);
  RTC_SetWakeUpCounter(0x1FFF);

  /* Enable the Wakeup Interrupt */
  RTC_ITConfig(RTC_IT_WUT, ENABLE);
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
