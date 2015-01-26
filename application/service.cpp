/*
 * service.cpp
 *
 *  Created on: 15 gru 2014
 *      Author: Adrian
 */


#include <string.h>

#include "stm32l1xx.h"

// Peripherals
#include "serial.h"
#include "lcd.h"

// Interface
#include "service.h"

// Drivers
#include "M41T56C64.h"

/**
 * \brief	Disabling LCD, enabling UART. Waiting for number 49 '1'. Then downloading actual clock
 * 			from user using UART, and setting RTC to desired clock. At the end disabling UART,
 * 		 	and enabling LCD.
 */
void serviceMode()
{
	uint8_t time[3];

	char string[20];

	LCD_Deinit();

	// UART Initialize
	serialInitialize();

	// Waiting for char '1' (49) on UART
	while(USART1->DR!=49);

	strcpy(string,"\r\nUart enable\r\n");
	serialSendString(string);
	strcpy(string,"\r\nService mode\r\n");
	serialSendString(string);
	strcpy(string,"\r\nLCD disable\r\n");

	strcpy(string,"Aktualna godzina: ");
	serialSendString(string);

	M41T56C64_ReadTime(time);
	M41T56C64_ConvertToInt(time);
	M41T56C64_ConvertToString(time,string);
	serialSendString(string);

	strcpy(string,"\r\nPodaj godzine");
	serialSendString(string);

	USART1->DR;

	while(!(USART1->SR & USART_SR_RXNE));
	time[0]=USART1->DR;

	strcpy(string,"\r\nPodaj minute");
	serialSendString(string);

	while(!(USART1->SR & USART_SR_RXNE));
	time[1]=USART1->DR;

	strcpy(string,"\r\nPodaj sekunde");
	serialSendString(string);

	while(!(USART1->SR & USART_SR_RXNE));
	time[2]=USART1->DR;

	M41T56C64_Init(time);

	strcpy(string,"\r\nSetting new clock for RTC\r\n");
	serialSendString(string);
	strcpy(string,"\r\nSetting to normal mode\r\n");
	serialSendString(string);
	strcpy(string,"\r\nInitializing LCD\r\n");
	serialSendString(string);
	strcpy(string,"\r\nDisabling UART\r\n");
	serialSendString(string);

	serialDeinit();

	LCD_Init();
}


