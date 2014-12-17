/*
 * timer.cpp
 *
 *  Created on: 7 sie 2014
 *      Author: uBirds_ARM
 */


#include <stdint.h>
#include <stddef.h>
#include "stm32l1xx.h"

#include "rcc.h"
#include "timer.h"

volatile static uint8_t counter;

void timerChannelSet(TIM_TypeDef* timer, uint8_t channel_number, uint32_t pulse_us)
{
	timer->CCER  |= (1<<(channel_number-1)*4);
	uint32_t clock_speed=rccGetCoreFrequency(), speed_hz;
	speed_hz=clock_speed/(timer->PSC+1);
	switch(channel_number)
	{
	case 1:
	{
		timer->CCR1=(speed_hz*pulse_us)/100000;
		break;
	}
	case 2:
	{
		timer->CCR2=(speed_hz*pulse_us)/100000;
		break;
	}
	case 3:
	{
		timer->CCR3=(speed_hz*pulse_us)/100000;
		break;
	}
	case 4:
	{
		timer->CCR4=(speed_hz*pulse_us)/100000;
		break;
	}
	default:
		break;
	}
}

void timerChannelConfig(TIM_TypeDef* timer, uint8_t channel_number,uint8_t mode, uint32_t pulse_us)
{
	switch(channel_number)
	{
		case 1:
		{
			timer->DIER |= TIM_DIER_CC1IE;
			if(mode==0) //compare mode
			timer->CCMR1 = TIM_CCMR1_OC1M_0 | TIM_CCMR1_OC1M_1;
			else //pwm mode
			{
				timer->CCMR1 = TIM_CCMR1_OC1M_2 | TIM_CCMR1_OC1M_1;
				timer->CR1|=TIM_CR1_ARPE;
			}
			break;
		}
		case 2:
		{
			timer->DIER |= TIM_DIER_CC2IE;
			if(mode==0) //compare mode
			timer->CCMR1 = TIM_CCMR1_OC2M_0 | TIM_CCMR1_OC2M_1;
			else //pwm mode
			{
				timer->CCMR1 = TIM_CCMR1_OC2M_2 | TIM_CCMR1_OC2M_1;
				timer->CR1|=TIM_CR1_ARPE;
			}
			break;
		}
		case 3:
		{
			timer->DIER |= TIM_DIER_CC3IE;
			if(mode==0) //compare mode
			timer->CCMR2 = TIM_CCMR2_OC3M_0 | TIM_CCMR2_OC3M_1;
			else //pwm mode
			{
				timer->CCMR2 = TIM_CCMR2_OC3M_2 | TIM_CCMR2_OC3M_1;
				timer->CR1|=TIM_CR1_ARPE;
			}
			break;
		}
		case 4:
		{
			timer->DIER |= TIM_DIER_CC4IE;
			if(mode==0) //compare mode
			timer->CCMR2 = TIM_CCMR2_OC4M_0 | TIM_CCMR2_OC4M_1;
			else //pwm mode
			{
			timer->CCMR2 = TIM_CCMR2_OC4M_2 | TIM_CCMR2_OC4M_1;
			timer->CR1|=TIM_CR1_ARPE;
			}
			break;
		}
	}
	timerChannelSet(timer, channel_number, pulse_us);
}

inline void timerBaseConfig(TIM_TypeDef* timer, uint32_t speed_hz, uint32_t period_us)
{
	uint32_t clock_speed=rccGetCoreFrequency();
	float tmp= (float)speed_hz*period_us/1000000;
	timer->PSC  = clock_speed/speed_hz -1;    // Set prescaler to defined speed in hz
	timer->ARR 	= (uint16_t)tmp;;	// Auto reload value
	timer->DIER |= TIM_DIER_UIE;
	timer->CR1  |= TIM_CR1_CEN;// Enable timer
}

void timerInit(uint8_t timer_number, uint32_t speed_hz, uint32_t period_ms)
{
	switch(timer_number)
	{
		case 2:
		{
			RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;
			timerBaseConfig(TIM2, speed_hz ,period_ms);
			NVIC_EnableIRQ(TIM2_IRQn);
			break;
		}
		case 3:
		{
			RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;
			timerBaseConfig(TIM3, speed_hz ,period_ms);
			NVIC_EnableIRQ(TIM3_IRQn);
			break;
		}
		case 4:
		{
			RCC->APB1ENR |= RCC_APB1ENR_TIM4EN;
			timerBaseConfig(TIM4, speed_hz ,period_ms);
			NVIC_EnableIRQ(TIM4_IRQn);
			break;
		}
		case 7:
		{
			RCC->APB1ENR |= RCC_APB1ENR_TIM7EN;
			timerBaseConfig(TIM7, speed_hz ,period_ms);
			NVIC_EnableIRQ(TIM4_IRQn);
			break;
		}
		case 9:
		{
			RCC->APB2ENR |= RCC_APB2ENR_TIM9EN;
			timerBaseConfig(TIM9, speed_hz ,period_ms);
			NVIC_EnableIRQ(TIM9_IRQn);
			break;
		}
		case 10:
		{
			RCC->APB2ENR |= RCC_APB2ENR_TIM10EN;
			timerBaseConfig(TIM10, speed_hz ,period_ms);
			NVIC_EnableIRQ(TIM10_IRQn);
			break;
		}
		case 11:
		{
			RCC->APB2ENR |= RCC_APB2ENR_TIM11EN;
			timerBaseConfig(TIM11, speed_hz ,period_ms);
			NVIC_EnableIRQ(TIM11_IRQn);
			break;
		}
		default:
			break;
	}
}



