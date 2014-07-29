/**
 * \file rtc.cpp
 * \brief RTC driver.
 *
 * RCC default configuration and functions
 *
 * chip: STM32L1xx; prefix: rtc
 *
 * \author Mazeryt Freager
 * \date 2014-07-29
 */

#include <stdint.h>

#include "stm32l1xx.h"

#include "hdr/hdr_rcc.h"
#include "hdr/hdr_bitband.h"

#include "config.h"

#include "rcc.h"
#include "rtc.h"

void rtcInitialize(void){

	  RCC->APB1ENR |= (3<<27); //Włączenie CLK dla PWR i BKP
	  PWR->CR |= 0x0100; //ZEZWOLENIE na zapis do BKP registers BDP =1

	  RCC->BDCR |= (1<<16);//Reset rejestrów Backup na to trzeba uważać
	  RCC->BDCR &= ~(1<<16);

	  RCC->BDCR |= (1<<0); //LSE ON (33...kHz)
	  while(!(RCC->BDCR & (1<<1)));//External low-speed oscillator ready

	  RCC->BDCR |= (1<<8); //LSE zrodłem RTC
	  RCC->BDCR |= (1<<15);  //RTC ENABLE

	  //---------------------------------------------------conf mode
	  while(!(RTC->CRL & 0x0020));//wait for RTOFF
	  RTC->CRL |= 0x0010; //Enter conf mode
	  RTC->PRLL = 0x7fff;//prescaler 32767+1, signal period 1s
	  RTC->CRH |= 0x0001;//second interrupt enable
	  RTC->CRL &= ~0x0010; //exit conf mode
	  while(!(RTC->CRL & 0x0020));
}
//TODO : change result to structure
uint_32t rtcRead(){
	uint_32t counterH = RTC->CNTH;
	uint_32t counterL = RTC->CNTL;

	return counterH;
}
