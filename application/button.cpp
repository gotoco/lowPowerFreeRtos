/*
 * button.cpp
 *
 *  Created on: 12 gru 2014
 *      Author: Adrian
 */

#include "stm32l1xx.h"
#include "gpio.h"
#include "button.h"
#include "bsp.h"

/**
 * \brief	Initializes interrupt of button
 */
void Button_Init()
{
	gpioConfigurePin(BUTTON_GPIO,BUTTON_PIN,BUTTON_CONFIGURATION);

	//setting interupt for PA0;
	EXTI->IMR|=EXTI_EMR_MR0;
	EXTI->RTSR|=EXTI_RTSR_TR0;
	EXTI->FTSR&= ~(EXTI_FTSR_TR0);

	//enabling interupt
	RCC->APB2ENR|=RCC_APB2ENR_SYSCFGEN;
	SYSCFG->EXTICR[0]=SYSCFG_EXTICR1_EXTI0_PA;
	NVIC_SetPriority(EXTI0_IRQn, 2);
	NVIC_EnableIRQ(EXTI0_IRQn);
}

/**
 * \brief	Initializes interrupt on the falling edge at service pin
 */
void ServicePin_Init()
{
	gpioConfigurePin(SERVICE_PIN_GPIO,SERVICE_PIN_PIN,SERVICE_PIN_CONFIGURATION);

	//setting interupt for PA0;
	EXTI->IMR|=EXTI_EMR_MR4;
	EXTI->RTSR&= ~(EXTI_RTSR_TR4);	// interrupt off at rising edge
	EXTI->FTSR|=EXTI_FTSR_TR4;		// interrupt on at falling edge

	//enabling interupt
	RCC->APB2ENR|=RCC_APB2ENR_SYSCFGEN;
	SYSCFG->EXTICR[1]=SYSCFG_EXTICR2_EXTI4_PA;
	NVIC_EnableIRQ(EXTI4_IRQn);
}

