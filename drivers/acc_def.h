/*
 * acc.h
 *
 *  Created on: Jul 4, 2014
 *      Author: uBirds_ARM
 */

#ifndef ACC_DEF_H_
#define ACC_DEF_H_

#define ACC_I2C_SLAVE_ADDRESS	0x4C

/*---------------------------------------------------------------------------------------------------------------------+
| ACC Aplications IDs
+---------------------------------------------------------------------------------------------------------------------*/

#define ACC_ID_VERSION		0x00
#define ACC_ID_SCHEDULER	0x01
#define ACC_ID_GPIO			0x03
#define ACC_ID_MAILBOX		0x04
#define ACC_ID_AFE			0x06
#define ACC_ID_LANDD		0x07
#define ACC_ID_HIGHG		0x08
#define ACC_ID_LOWG			0x09
#define ACC_ID_TAP			0x0A
#define ACC_ID_TILT			0x0B
#define ACC_ID_FRAMEC		0x0E
#define ACC_ID_FIFO			0x0F
#define ACC_ID_EVENTQ		0x10
#define ACC_ID_STREG		0x11
#define ACC_ID_SLEEP		0x12
#define ACC_ID_PEDOMETR		0x15
#define ACC_ID_RESET_CLC	0x17
#define ACC_ID_MAILCONF		0x18

/*---------------------------------------------------------------------------------------------------------------------+
| ACC I2C Transmission Commands
+---------------------------------------------------------------------------------------------------------------------*/

#define ACC_COMMAND_READ_VER		0
#define ACC_COMMAND_READ_CONF		1
#define ACC_COMMAND_WRITE_CONF		2
#define ACC_COMMAND_READ_STAT		3

/*---------------------------------------------------------------------------------------------------------------------+
| ACC AFE Status Registers
+---------------------------------------------------------------------------------------------------------------------*/

#define ACC_AFE_CSR_MS		0x00
#define ACC_AFE_CSR_LS		0x01
#define ACC_AFE_xOFFSET_MS	0x02
#define ACC_AFE_xOFFSET_LS	0x03
#define ACC_AFE_yOFFSET_MS	0x04
#define ACC_AFE_yOFFSET_LS	0x05
#define ACC_AFE_zOFFSET_MS	0x06
#define ACC_AFE_zOFFSET_LS	0x07
#define ACC_AFE_HPF_MS		0x08
#define ACC_AFE_HPF_LS		0x09
#define ACC_AFE_LPF_MS		0x0A
#define ACC_AFE_EICPF		0x0B
#define ACC_AFE_SFDRATE		0x0C

/*---------------------------------------------------------------------------------------------------------------------+
| ACC Tap Status Registers
+---------------------------------------------------------------------------------------------------------------------*/

#define ACC_TAP_SINGLE_REG		0x00
// Bits definitions
#define ACC_TAPS_EVENT					7
#define ACC_TAPS_ZDIR					5
#define ACC_TAPS_ZEVENT					4
#define ACC_TAPS_YDIR					3
#define ACC_TAPS_YEVENT					2
#define ACC_TAPS_XDIR					1
#define ACC_TAPS_XEVENT					0

#define ACC_TAP_DOUBLE_REG		0x01
//Bits definitions
#define ACC_TAPD_EVENT					7
#define ACC_TAPD_ZDIR					5
#define ACC_TAPD_ZEVENT					4
#define ACC_TAPD_YDIR					3
#define ACC_TAPD_YEVENT					2
#define ACC_TAPD_XDIR					1
#define ACC_TAPD_XEVENT					0


 /*---------------------------------------------------------------------------------------------------------------------+
 | Pedometr Configuration
 +---------------------------------------------------------------------------------------------------------------------*/

 #define ACC_PEDO_SLEEP_MIN				400
 #define ACC_PEDO_SLEEP_MAX				1000
 #define ACC_PEDO_SLEEP_THRES			1
 #define ACC_PEDO_CONF					1
 #define ACC_PEDO_ACT					0
 #define ACC_PEDO_SLP					1
 #define ACC_PEDO_STEPLEN				0 	//Step length in centimeters, but if 0 it will calculate from height and weight
 #define ACC_PEDO_DEF_HEIGHT			175
 #define ACC_PEDO_DEF_WEIGHT			70
 #define ACC_PEDO_FILSTEP				4	//range 0:6
 #define ACC_PEDO_FILTTIME				3
 #define ACC_PED_SPEED_PERIOD			3	//range 2:5
 #define ACC_PEDO_SPEED_COUNT			1	//range 0:255

 /*---------------------------------------------------------------------------------------------------------------------+
 | ACC Definitions for SPI
 +---------------------------------------------------------------------------------------------------------------------*/

#define AAC_SPI_READ_ADDRESS				0x00
#define ACC_SPI_WRITE_ADDRESS				0x80
#define ACC_SPI_READ_DEALY					36000



#endif /* ACC_DEF_H_ */
