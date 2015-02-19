/**
 * \file usart.h
 * \brief Header for usart.cpp
 * \author Freddie Chopin, http://www.freddiechopin.info http://www.distortec.com
 * \date 2012-08-30
 */

#ifndef USART_H_
#define USART_H_

#include "FreeRTOS.h"

#include "error.h"

/*---------------------------------------------------------------------------------------------------------------------+
| global functions' prototypes
+---------------------------------------------------------------------------------------------------------------------*/

enum Error usartInitialize(void);
enum Error usartPrintf(portTickType ticks_to_wait, const char *format, ...);
enum Error usartSendString(const char *string, portTickType ticks_to_wait);

int usartPutCharacter(int c);

#ifdef __cplusplus
extern "C" {
#endif

void usartSendCharacter(char c);
void usartSendDebugMsg(const char *string);

#ifdef __cplusplus
}
#endif

#endif /* USART_H_ */
