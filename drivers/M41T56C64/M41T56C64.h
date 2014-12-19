/*
 * M41T56C64.h
 *
 *  Created on: 8 gru 2014
 *      Author: Adrian
 */



#ifndef M41T56C64_H_
#define M41T56C64_H_

/*---------------------------------------------------------------------------------------------------------------------+
| global defines
+---------------------------------------------------------------------------------------------------------------------*/

#define M41T56_SlaveAddress 0b1101000

#define M41T56_SECONDS  0
#define M41T56_MINUTES  1
#define M41T56_HOURS	2
#define M41T56_DAY		3
#define M41T56_DATE		4
#define M41T56_MONTH	5
#define M41T56_YEAR		6
#define M41T56_CONTROL  7

#define M41T56_OUT		7
#define M41T56_FT		6
#define M41T56_S		5
#define M41T56_CALMASK	0x1F

/*---------------------------------------------------------------------------------------------------------------------+
| global functions' prototypes
+---------------------------------------------------------------------------------------------------------------------*/

void M41T56C64_Init(uint8_t* time);

void M41T56C64_ReadTime(uint8_t* tab);

void M41T56C64_ConvertToInt(uint8_t* tab);

void M41T56C64_ConvertToString(uint8_t* tab, char* text);


#endif /* M41T56C64_H_ */
