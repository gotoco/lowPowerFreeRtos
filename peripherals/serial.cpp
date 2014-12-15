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
	RCC->AHBENR |= RCC_AHBENR_GPIOAEN;
	RCC->APB2ENR|=RCC_APB2ENR_SYSCFGEN;

	gpioConfigurePin(SERIALx_TX_GPIO, SERIALx_TX_PIN, SERIALx_TX_CONFIGURATION);
	gpioConfigurePin(SERIALx_RX_GPIO, SERIALx_RX_PIN, SERIALx_RX_CONFIGURATION);

	RCC_APBxENR_SERIALxEN_bb = 1;			// enable USART in RCC

	SERIALx->BRR = (rccGetCoreFrequency() + SERIALx_BAUDRATE / 2)/ SERIALx_BAUDRATE;	// calculate baudrate (with rounding)
	// enable peripheral, transmitter and receiver, enable RXNE interrupt
	SERIALx->CR1 = USART_CR1_UE | USART_CR1_TE | USART_CR1_RE;

	/*
	SERIALx->CR1 |= USART_CR1_RXNEIE;
	NVIC_SetPriority(SERIALx_IRQn, SERIALx_IRQ_PRIORITY);	// set USART priority
	NVIC_EnableIRQ(SERIALx_IRQn);				// enable USART IRQ
	*/
}

void serialDeinit(void)
{
	RCC_APBxENR_SERIALxEN_bb = 0;			// disable USART in RCC
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
	SERIALx->DR = c;
}

void serialSendString(char *s)
{
	while(*s != '\0')
	{
		while (!(USARTx_SR_TXE_bb(SERIALx)));
		serialSendCharacter(*s);
		s++;
	}
}
