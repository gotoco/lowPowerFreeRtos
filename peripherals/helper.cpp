/**
 * \file helper.cpp
 * \brief Helper functions
 *
 * Helper functions
 *
 * chip: STM32L1xx
 *
 * \author Freddie Chopin, http://www.freddiechopin.info http://www.distortec.com
 * \date 2012-08-30
 */

#include <stdint.h>

#include "inc/stm32l1xx.h"

#include "config.h"

#include "hdr/hdr_rcc.h"

#include "rcc.h"
#include "helper.h"

/*---------------------------------------------------------------------------------------------------------------------+
| global variables
+---------------------------------------------------------------------------------------------------------------------*/

volatile uint16_t tim6OverflowCount;

/*---------------------------------------------------------------------------------------------------------------------+
| global functions
+---------------------------------------------------------------------------------------------------------------------*/

/**
 * \brief Configures TIM6 for runtime stats.
 *
 * Configures TIM6 for runtime stats.
 */

void configureTimerForRuntimestats(void)
{
	RCC_APB1ENR_TIM6EN_bb = 1;				// enable timer

	DBGMCU->APB1FZ |= DBGMCU_APB1_FZ_DBG_TIM6_STOP;

	TIM6->CNT = 0;							// clear the timer
	TIM6->PSC = (rccGetCoreFrequency() / 10000) - 1;	// 100us resolution
	TIM6->ARR = 0xFFFF;						// max autoreload
	TIM6->DIER = TIM_DIER_UIE;				// enable update interrupt
	TIM6->CR1 = TIM_CR1_CEN;				// enable ARR buffering, enable timer

	NVIC_SetPriority(TIM6_IRQn, TIM6_IRQ_PRIORITY);
	NVIC_EnableIRQ(TIM6_IRQn);
}

/*---------------------------------------------------------------------------------------------------------------------+
| ISRs
+---------------------------------------------------------------------------------------------------------------------*/

/**
 * \brief TIM6_IRQHandler
 *
 * TIM6_IRQHandler
 */

extern "C" void TIM6_IRQHandler(void) __attribute__ ((interrupt));
void TIM6_IRQHandler(void)
{
	tim6OverflowCount++;

	TIM6->SR = 0;							// clear UIF which is only bit in this register
}
