/*
 * LIS35DE.h
 *
 *  Created on: 22 gru 2014
 *      Author: Adrian
 */

#ifndef LIS35DE_H_
#define LIS35DE_H_

/*---------------------------------------------------------------------------------------------------------------------+
| global defines
+---------------------------------------------------------------------------------------------------------------------*/

#define LIS35DE_INITIALIZE_ERROR			1

/******************  definition for i2c communication  ***************/
#define LIS35DE_I2C_SLAVE_ADDRESS			0b00011100

#define LIS35DE_I2C_MULTIPLE_BYTES			(1 << 7)
#define LIS35DE_I2C_SINGLE_BYTE				(0 << 7)


/******************  definition for spi communication  ***************/
#define LIS35DE_SPI_READ					(1 << 7)
#define LIS35DE_SPI_WRITE					(0 << 7)

#define LIS35DE_SPI_ADDRESS_UNCHAGED		(0 << 6)
#define LIS35DE_SPI_ADDRESS_INCREMENTED		(1 << 6)


/******************  definition of registers adress  ***************/
#define LIS35DE_CTRL_REG1					0x20
#define LIS35DE_CTRL_REG2					0x21
#define LIS35DE_CTRL_REG3					0x22
#define LIS35DE_HP_FILTER_RESET				0x23
#define LIS35DE_STATUS_REG					0x27
#define LIS35DE_OUT_X						0x29
#define LIS35DE_OUT_Y						0x2B
#define LIS35DE_OUT_Z						0x2D


/******************  Bit definition for CTRL_REG1 register  ***************/
#define LIS35DE_CTRL_REG1_DR				(1 << 7)
#define LIS35DE_CTRL_REG1_PD				(1 << 6)
#define LIS35DE_CTRL_REG1_FS				(1 << 5)
#define LIS35DE_CTRL_REG1_ZEN				(1 << 2)
#define LIS35DE_CTRL_REG1_YEN				(1 << 1)
#define LIS35DE_CTRL_REG1_XEN				(1 << 0)


/******************  Bit definition for CTRL_REG2 register  ***************/
#define LIS35DE_CTRL_REG2_BOOT				(1 << 6)


/*---------------------------------------------------------------------------------------------------------------------+
| global functions' prototypes
+---------------------------------------------------------------------------------------------------------------------*/

uint8_t LIS35DE_Init();
void LIS35DE_Read(int8_t *x, int8_t *y, int8_t *z);

/*---------------------------------------------------------------------------------------------------------------------+
| private functions' prototypes
+---------------------------------------------------------------------------------------------------------------------*/

/******************  SPI functions ***************/
static uint8_t LIS35DE_SPI_Init();
static void LIS35DE_SPI_Reboot();
static void LIS35DE_SPI_Read(int8_t *x, int8_t *y, int8_t *z);


/******************  I2C functions ***************/
static uint8_t LIS35DE_I2C_Init();
static void LIS35DE_I2C_Reboot();
static void LIS35DE_I2C_Read(int8_t *x, int8_t *y, int8_t *z);

#endif /* LIS35DE_H_ */
