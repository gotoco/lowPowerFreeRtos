/*
 * timer.cpp
 *
 *  Created on: 5 gru 2014
 *      Author: Adrian
 */

#include "timer.h"
#include "inc/stm32l1xx.h"

#include <stdint.h>
#include "config.h"
#include "hdr/hdr_rcc.h"
#include "rcc.h"

/**
 * \brief Initialises Timer2 for down counting mode. When counter reach zero, it generates interrupt signal.
 * 		Each interrupt signal appears after 1ms. To switch on the interrupts, you have to use NVIC functions:
 * 		NVIC_EnableIRQ(SysTick_IRQn);
 *	    NVIC_SetPriority(SysTick_IRQn, 1);
 */
void Timer_Init()
{
	// enable clock for timer
	RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;

	// clock source enable
	TIM2->CR1 |= TIM_CR1_CEN;

	// set prescaller to 1ms resolution
	TIM2->PSC = (rccGetCoreFrequency() / 1000) - 1;

	// set auto reload value
	TIM2->ARR = 1000;

	// set down counting
	TIM2->CCR1 |= TIM_CR1_DIR;

	// stop timer when core is halted while debugging
	DBGMCU->APB1FZ |= DBGMCU_APB1_FZ_DBG_TIM2_STOP;

	// enable update interrupt
	TIM2->DIER = TIM_DIER_UIE;

	// clear flags
	TIM2->SR = 0;
}



