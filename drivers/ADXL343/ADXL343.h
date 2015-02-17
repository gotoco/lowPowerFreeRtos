/*
 * ADXL343.h
 *
 *  Created on: 13 sty 2015
 *      Author: Adrian
 */

#ifndef ADXL343_H_
#define ADXL343_H_


/*---------------------------------------------------------------------------------------------------------------------+
| global defines
+---------------------------------------------------------------------------------------------------------------------*/

#define ADXL343_READ					(1 << 7)
#define ADXL343_WRITE					(0 << 7)

#define ADXL343_MULTIPLE_BYTE			(1 << 6)
#define ADXL343_SINGLE_BYTE				(0 << 6)

#define ADXL343_DEV_ID					0x00

#define ADXL343_POWER_CTL				0x2D
#define ADXL343_POWER_CTL_SLEEP			(1 << 2)
#define ADXL343_POWER_CTL_MEASURE		(1 << 3)
#define ADXL343_POWER_CTL_AUTO_SLEEP	(1 << 4)
#define ADXL343_POWER_CTL_LINK			(1 << 5)

#define ADXL343_DATAX0					0x32
#define ADXL343_DATAX1					0x33
#define ADXL343_DATAY0					0x34
#define ADXL343_DATAY1					0x35
#define ADXL343_DATAZ0					0x36
#define ADXL343_DATAZ1					0x37


/*---------------------------------------------------------------------------------------------------------------------+
| global functions' prototypes
+---------------------------------------------------------------------------------------------------------------------*/

void ADXL343_Init(void);

void ADXL343_Read(int16_t *x, int16_t *y, int16_t *z);


#endif /* ADXL343_H_ */
