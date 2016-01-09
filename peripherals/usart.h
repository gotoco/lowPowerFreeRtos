/**
* \file usart.h
* \brief Header for usart.cpp
* \author: Maciek Grochowski
* \date 2014-07-30
*/

#ifndef USART_H_
#define USART_H_

#include "FreeRTOS.h"

#include "error.h"

/*---------------------------------------------------------------------------------------------------------------------+
| global functions' prototypes
+---------------------------------------------------------------------------------------------------------------------*/


enum Error usartPrintf(portTickType ticks_to_wait, const char *format, ...);
enum Error putMsgToUartQueue(const char *data, size_t length, portTickType ticks_to_wait);
void usart_low_level_put(char c);

#ifdef __cplusplus
extern "C" {
#endif

enum Error usartSendString(const char *string, portTickType ticks_to_wait);
enum Error usartSendBytes(const char *data, size_t length, portTickType ticks_to_wait);
void usartSendDebugMsg(const char *string);
enum Error usartInitialize(void);

#ifdef __cplusplus
}
#endif

#endif /* USART_H_ */
