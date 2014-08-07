/*
 * timer.cpp
 *
 *  Created on: 7 sie 2014
 *      Author: uBirds_ARM
 */


#include <stdint.h>
#include <stddef.h>
#include "stm32l1xx.h"

#include "timer.h"


void
timer_BaseConfig(TIM_TypeDef* timer)
{
	 timer->CR1=1<<7 |00<<5; //ARPE, CMS=00
	 timer->CCMR2=011<<4; //OC3M=011
	 timer->CCER=1<<8; //CC3E
	 timer->PSC=0;
	 timer->ARR=1;
	 timer->CCR3=200;

	 timer->CR1|= 1<<0;
}


void
timer_Init(uint8_t timer_number)
{
	switch(timer_number)
	{
		case 2:
		{
			RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;
			timer_BaseConfig(TIM2);
			break;
		}
		case 3:
		{
			RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;
			timer_BaseConfig(TIM3);
			break;
		}
		case 4:
		{
			RCC->APB1ENR |= RCC_APB1ENR_TIM4EN;
			timer_BaseConfig(TIM4);
			break;
		}
		case 7:
		{
			RCC->APB1ENR |= RCC_APB1ENR_TIM7EN;
			timer_BaseConfig(TIM7);
			break;
		}
		case 9:
		{
			RCC->APB2ENR |= RCC_APB2ENR_TIM9EN;
			timer_BaseConfig(TIM9);
			break;
		}
		case 10:
		{
			RCC->APB2ENR |= RCC_APB2ENR_TIM10EN;
			timer_BaseConfig(TIM10);
			break;
		}
		case 11:
		{
			RCC->APB2ENR |= RCC_APB2ENR_TIM11EN;
			timer_BaseConfig(TIM11);
			break;
		}
		default:
			break;
	}
}

