/**
 * \file serial.h
 * \brief Dummy Serial driver for tests
 *
 * Functions for Serial-USART control
 *
 * chip: STM32L1xx; prefix: usart
 *
 * \author grochu
 * \date 2014-07-08
 */

#ifndef SERIAL_H_
#define SERIAL_H_

#include "error.h"

/*---------------------------------------------------------------------------------------------------------------------+
| global functions' prototypes
+---------------------------------------------------------------------------------------------------------------------*/

enum Error serialInitialize(void);

#ifdef __cplusplus
extern "C" {
#endif

void serialSendCharacter(char c);
//void serialReciveCharacted();

#ifdef __cplusplus
}
#endif

#endif /* SERIAL_H_ */
