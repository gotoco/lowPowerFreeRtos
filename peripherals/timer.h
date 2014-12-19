/*
 * timer.h
 *
 *  Created on: 7 sie 2014
 *      Author: uBirds_ARM
 */

#ifndef TIMER_H_
#define TIMER_H_

#include <stdint.h>
#include "stm32l152xb.h"

void timerInit(uint8_t timer_number, uint32_t speed_hz, uint32_t period_ms);
void timerChannelConfig(TIM_TypeDef* timer, uint8_t channel_number,uint8_t mode, uint32_t pulse_us);

#endif /* TIMER_H_ */
