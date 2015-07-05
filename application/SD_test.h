/*
 * SD_test.h
 *
 *  Created on: 12 lut 2015
 *      Author: Adrian
 */

#ifndef SD_TEST_H_
#define SD_TEST_H_

/*---------------------------------------------------------------------------------------------------------------------+
| global defines
+---------------------------------------------------------------------------------------------------------------------*/

typedef enum {FAILED = 0, PASSED = !FAILED} TestStatus;

#define  BufferSize     512

/*---------------------------------------------------------------------------------------------------------------------+
| Private function prototypes
+---------------------------------------------------------------------------------------------------------------------*/

static void Fill_Buffer(uint8_t *pBuffer, uint16_t BufferLenght, uint8_t Offset);
static TestStatus Buffercmp(uint8_t* pBuffer1, uint8_t* pBuffer2, uint16_t BufferLength);
uint8_t SD_test(uint8_t* pBuffer1, uint8_t* pBuffer2, uint16_t BufferLength);

#endif /* SD_TEST_H_ */
