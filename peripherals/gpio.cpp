/**
 * \file gpio.cpp
 * \brief GPIO driver.
 *
 * GPIO default configuration and function for configuring a pin
 *
 * chip: STM32L1xx; prefix: gpio
 *
 * \author: Mazeryt Freager
 * \date 2014-07-20
 */

#include <stdint.h>

#include "stm32l152xb.h"

#include "hdr/hdr_gpio.h"

#include "gpio.h"

/*---------------------------------------------------------------------------------------------------------------------+
| local defines
+---------------------------------------------------------------------------------------------------------------------*/

#define GPIO_GET_MODER(combination)			(((combination) & 0xF) >> 0)
#define GPIO_GET_OTYPER(combination)		(((combination) & 0xF0) >> 4)
#define GPIO_GET_OSPEEDR(combination)		(((combination) & 0xF00) >> 8)
#define GPIO_GET_PUPDR(combination)			(((combination) & 0xF000) >> 12)
#define GPIO_GET_AFR(combination)			(((combination) & 0xF0000) >> 16)

/*---------------------------------------------------------------------------------------------------------------------+
| global functions
+---------------------------------------------------------------------------------------------------------------------*/

/**
 * \brief GPIO initialization.
 *
 * Enables all GPIO ports.
 */

void gpioInitialize(void)
{
	RCC->AHBENR |= RCC_AHBENR_GPIOAEN | RCC_AHBENR_GPIOBEN | RCC_AHBENR_GPIOCEN | RCC_AHBENR_GPIODEN |
			RCC_AHBENR_GPIOEEN | RCC_AHBENR_GPIOHEN;
}

/**
 * \brief Configures pin.
 *
 * Configures one pin in one port.
 *
 * \param [in] port points to the configuration structure of desired port
 * \param [in] pin selects one pin, allowed values {GPIO_PIN_[0; 15]}
 * \param [in] configuration is a combined value of MODER, OTYPER, OSPEEDR, PUPDR and AFRx register bitfields, allowed
 * values
 * {GPIO_IN_FLOATING, GPIO_IN_PULL_UP, GPIO_IN_PULL_DOWN, GPIO_OUT_{PP, OD}_{400kHz, 2MHz, 10MHz, 40MHz},
 * GPIO_OUT_{PP, OD}_{400kHz, 2MHz, 10MHz, 40MHz}_{PULL_UP, PULL_DOWN},
 * GPIO_[AF0; AF15]_{PP, OD}_{400kHz, 2MHz, 10MHz, 40MHz},
 * GPIO_[AF0; AF15]_{PP, OD}_{400kHz, 2MHz, 10MHz, 40MHz}_{PULL_UP, PULL_DOWN}, GPIO_ANALOG}
 */

void gpioConfigurePin(GPIO_TypeDef *port, enum GpioPin pin, enum GpioConfiguration configuration)
{
	uint32_t moder, otyper, ospeedr, pupdr, afr, afrx;

	moder = port->MODER;				// localize
	moder &= ~(GPIO_MODER_mask << (pin * 2));	// clear current setting
	moder |= (GPIO_GET_MODER(configuration) << (pin * 2));	// apply new setting
	port->MODER = moder;				// save back

	otyper = port->OTYPER;				// localize
	otyper &= ~(GPIO_OTYPER_mask << pin);	// clear current setting
	otyper |= (GPIO_GET_OTYPER(configuration) << pin);	// apply new setting
	port->OTYPER = otyper;				// save back

	ospeedr = port->OSPEEDR;			// localize
	ospeedr &= ~(GPIO_OSPEEDR_mask << (pin * 2));	// clear current setting
	ospeedr |= (GPIO_GET_OSPEEDR(configuration) << (pin * 2));	// apply new setting
	port->OSPEEDR = ospeedr;			// save back

	pupdr = port->PUPDR;				// localize
	pupdr &= ~(GPIO_PUPDR_mask << (pin * 2));	// clear current setting
	pupdr |= (GPIO_GET_PUPDR(configuration) << (pin * 2));	// apply new setting
	port->PUPDR = pupdr;				// save back

	if (pin < 8)							// determine which AFR register should be used
		afrx = 0;							// AFRL - pins [0; 7]
	else
	{
		afrx = 1;							// AFRH - pins [8; 15]
		pin = (enum GpioPin)(((uint32_t)pin) - 8);	// trim pin number
	}

	afr = port->AFR[afrx];				// localize
	afr &= ~(GPIO_AFRx_mask << (pin * 4));	// clear current setting
	afr |= (GPIO_GET_AFR(configuration) << (pin * 4));	// apply new setting
	port->AFR[afrx] = afr;				// save back
}
