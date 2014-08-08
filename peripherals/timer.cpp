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



void
timerChannelSet(TIM_TypeDef* timer, uint8_t channel_number, uint32_t pulse_us)
{

	timer->CCER  |= (1<<channel_number*4), TIM_CCER_CC3E;
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

void
timerChannelConfig(TIM_TypeDef* timer, uint8_t channel_number,uint8_t mode, uint32_t pulse_us)
{
	if(mode==0) //compare mode
	{
		if(channel_number>2)
		timer->CCMR2 |=(1<<((channel_number%2)*8+4)) | (2<<((channel_number%2)*8+4)); // TIM_CCMR2_OCxM_0 | TIM_CCMR2_OCxM_1
		else
		timer->CCMR1 |=(1<<((channel_number%2)*8+4)) | (2<<((channel_number%2)*8+4)); // TIM_CCMR1_OCxM_0 | TIM_CCMR1_OCxM_1
		timerChannelSet(timer, channel_number, pulse_us);
	}
	else	//pwm
	{
		if(channel_number>2)
		timer->CCMR2 |=(4<<((channel_number%2)*8+4)) | (2<<((channel_number%2)*8+4)); // TIM_CCMR2_OCxM_0 | TIM_CCMR2_OCxM_1
		else
		timer->CCMR1 |=(4<<((channel_number%2)*8+4)) | (2<<((channel_number%2)*8+4)); // TIM_CCMR1_OCxM_0 | TIM_CCMR1_OCxM_1
		timerChannelSet(timer, channel_number, pulse_us);
	}
}

inline void
timerBaseConfig(TIM_TypeDef* timer, uint32_t speed_hz, uint32_t period_ms)
{
	uint32_t clock_speed=rccGetCoreFrequency();
	timer->PSC = clock_speed/speed_hz -1;	    // Set prescaler to defined speed in hz
	timer->ARR = (speed_hz/1000)*period_ms;	 	// Auto reload value // @error trzba dojœc co zle liczy - dziwne zachowanie debuga
	timer->DIER = TIM_DIER_UIE;
	timer->CR1 = TIM_CR1_CEN;// Enable timer
}

void
timerInit(uint8_t timer_number, uint32_t speed_hz, uint32_t period_ms)
{
	switch(timer_number)
	{
		case 2:
		{
			RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;
			NVIC_EnableIRQ(TIM2_IRQn);
			timerBaseConfig(TIM2, speed_hz ,period_ms);
			break;
		}
		case 3:
		{
			RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;
			NVIC_EnableIRQ(TIM3_IRQn);
			timerBaseConfig(TIM3, speed_hz ,period_ms);
			break;
		}
		case 4:
		{
			RCC->APB1ENR |= RCC_APB1ENR_TIM4EN;
			NVIC_EnableIRQ(TIM4_IRQn);
			timerBaseConfig(TIM4, speed_hz ,period_ms);
			break;
		}
		case 7:
		{
			RCC->APB1ENR |= RCC_APB1ENR_TIM7EN;
			NVIC_EnableIRQ(TIM4_IRQn);
			timerBaseConfig(TIM7, speed_hz ,period_ms);
			break;
		}
		case 9:
		{
			RCC->APB2ENR |= RCC_APB2ENR_TIM9EN;
			NVIC_EnableIRQ(TIM9_IRQn);
			timerBaseConfig(TIM9, speed_hz ,period_ms);
			break;
		}
		case 10:
		{
			RCC->APB2ENR |= RCC_APB2ENR_TIM10EN;
			NVIC_EnableIRQ(TIM10_IRQn);
			timerBaseConfig(TIM10, speed_hz ,period_ms);
			break;
		}
		case 11:
		{
			RCC->APB2ENR |= RCC_APB2ENR_TIM11EN;
			NVIC_EnableIRQ(TIM11_IRQn);
			timerBaseConfig(TIM11, speed_hz ,period_ms);
			break;
		}
		default:
			break;
	}
}

