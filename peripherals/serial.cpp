/**
 * \file serial.c
 * \brief Dummy Serial driver for tests
 *
 * Functions for Serial-USART control
 *
 * chip: STM32L1xx; prefix: usart
 *
 * \author Mazeryt Freager
 * \date 2014-07-08
 */

#include <stdint.h>
#include <string.h>
#include <stdarg.h>
#include <stdio.h>

#include "inc/stm32l1xx.h"

#include "config.h"
#include "serial.h"

#include "hdr/hdr_usart.h"
#include "hdr/hdr_usart.h"
#include "hdr/hdr_gpio.h"
#include "hdr/hdr_rcc.h"
#include "hdr/hdr_dma.h"

#include "gpio.h"
#include "rcc.h"
#include "usart.h"
#include "command.h"
#include "helper.h"
#include "error.h"


/**
 * \brief Initializes USART
 *
 * Initializes USART
 *
 * \return ERROR_NONE if the semaphore and queues were successfully created and
 * tasks were successfully created and added to a ready list, otherwise an error
 * code defined in the file error.h
 */

enum Error serialInitialize(void)
{
	RCC->APB1ENR |= RCC_APB1ENR_USART2EN;
	gpioConfigurePin(GPIOA, GPIO_PIN_2, GPIO_AF7_PP_40MHz_PULL_UP);
	gpioConfigurePin(GPIOA, GPIO_PIN_3, GPIO_AF7_PP_40MHz_PULL_UP);

	RCC_APB1ENR_USART2EN_bb = 1;			// enable USART in RCC

	USART2->BRR = (rccGetCoreFrequency() + SERIALx_BAUDRATE / 2) / SERIALx_BAUDRATE;	// calculate baudrate (with rounding)
	// enable peripheral, transmitter and receiver, enable RXNE interrupt
	USART2->CR1 = USART_CR1_UE | USART_CR1_TE | USART_CR1_RE;
}

/**
 * \brief Low-level character print.
 *
 * Low-level character print. Should be used for debugging only.
 *
 * \param [in] c is the character that will be printed
 */

void serialSendCharacter(char c)
{
	while (!(USARTx_SR_TXE_bb(SERIALx)));
	USART2->DR = c;
}
