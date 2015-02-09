/*
 * timer.cpp
 *
 *  Created on: 5 gru 2014
 *      Author: Adrian
 */

#include "timer.h"
#include "stm32l152xc.h"

#include <stdint.h>
#include "config.h"
#include "hdr/hdr_rcc.h"
#include "rcc.h"

/**
 * \brief Initialises Timer2 for down counting mode. When counter reach zero, it generates interrupt signal.
 * 		Each interrupt signal appears after 1ms.
 */
void Timer_Init()
{
	// enable clock for timer
	RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;

	// clock source enable
	TIM2->CR1 |= TIM_CR1_CEN;

	// set prescaller to 1ms resolution
	TIM2->PSC = 0;

	// set auto reload value
	TIM2->ARR = 32000;

	// set down counting
	TIM2->CCR1 |= TIM_CR1_DIR;

	// stop timer when core is halted while debugging
	DBGMCU->APB1FZ |= DBGMCU_APB1_FZ_DBG_TIM2_STOP;

	// enable update interrupt
	TIM2->DIER = TIM_DIER_UIE;

	// clear flags
	TIM2->SR = 0;

	// enabling interrupt
	NVIC_EnableIRQ(TIM2_IRQn);
	NVIC_SetPriority(TIM2_IRQn, 1);
}

/**
 * \brief Initialises SysTick timer to interrupt every 1 ms.
 */
void Systick_Init()
{
	SysTick_Config(rccGetCoreFrequency()/1000);
	NVIC_EnableIRQ(SysTick_IRQn);
	NVIC_SetPriority(SysTick_IRQn, 1);
}





