/*
 * acc.h
 *
 *  Created on: Jul 4, 2014
 *      Author: uBirds_ARM
 */

#ifndef ACC_DEF_H_
#define ACC_DEF_H_

#define ACC_PEDOMETER_UPDATE_RATE	30
#define ACC_AFE_MAXIMUM_UPDATE_RATE	488

typedef enum {
	acc_Power_Mode_normal		= 0,
	acc_Power_Mode_low_power	= 1,
} acc_Power_Mode_t;

/*---------------------------------------------------------------------------------------------------------------------+
 | ACC Definitions for SPI
 +---------------------------------------------------------------------------------------------------------------------*/

typedef enum {
	acc_SPI_Register_Address_Mailbox					= 0x00,
	acc_SPI_Register_Address_Quick_Read_Mailbox			= 0x14,
	acc_SPI_Register_Address_Binary_Semaphore			= 0x20,
	acc_SPI_Register_Address_Status_and_Control			= 0x23,
	acc_SPI_Register_Address_Write_Status_0				= 0x24,
	acc_SPI_Register_Address_Write_Status_1				= 0x25,
	acc_SPI_Register_Address_Write_Status_2				= 0x26,
	acc_SPI_Register_Address_Write_Status_3				= 0x27,
	acc_SPI_Register_Address_Read_Status_0				= 0x28,
	acc_SPI_Register_Address_Read_Status_1				= 0x29,
	acc_SPI_Register_Address_Read_Status_2				= 0x2A,
	acc_SPI_Register_Address_Read_Status_3				= 0x2B,
	acc_SPI_Register_Address_Mutex_Timeout				= 0x2C,
	acc_SPI_Register_Address_Output_Interrupt_Control	= 0x2E,
} acc_SPI_Register_Address_t;

/*---------------------------------------------------------------------------------------------------------------------+
| ACC Aplications IDs
+---------------------------------------------------------------------------------------------------------------------*/

typedef enum {
	acc_Scheduler_Params_Version_Offset					= 0x00,
	acc_Scheduler_Params_Scheduler_Offset				= 0x2C,
	acc_Scheduler_Params_Communications_Offset			= 0x2E,
	acc_Scheduler_Params_GPIO_Offset					= 0x2F,
	acc_Scheduler_Params_Mailbox_Offset					= 0x30,
	acc_Scheduler_Params_AFE_Offset						= 0x32,
	acc_Scheduler_Params_Portrait_Landscape_Offset		= 0x33,
	acc_Scheduler_Params_High_G_Offset					= 0x34,
	acc_Scheduler_Params_Low_G_Offset					= 0x35,
	acc_Scheduler_Params_Tap_Offset						= 0x36,
	acc_Scheduler_Params_Tilt_Offset					= 0x37,
	acc_Scheduler_Params_Data_FIFO_Offset				= 0x3B,
	acc_Scheduler_Params_Event_Queue_Offset				= 0x3C,
	acc_Scheduler_Params_Status_Register_Offset			= 0x3D,
	acc_Scheduler_Params_Wake_Sleep_Offset				= 0x3E,
	acc_Scheduler_Params_Reset_Suspend_Clear_Offset		= 0x43,
	acc_Scheduler_Params_Mailbox_Configuration_Offset	= 0x44,
} acc_Scheduler_Params_App_Offset_t;

typedef enum {
	acc_App_ID_Version				= 0x00,
	acc_App_ID_Scheduler			= 0x01,
	acc_App_ID_Communications		= 0x02,
	acc_App_ID_GPIO					= 0x03,
	acc_App_ID_Mailbox				= 0x04,
	acc_App_ID_AFE					= 0x06,
	acc_App_ID_Portrait_Landscape	= 0x07,
	acc_App_ID_High_G				= 0x08,
	acc_App_ID_Low_G				= 0x09,
	acc_App_ID_Tap					= 0x0A,
	acc_App_ID_Tilt					= 0x0B,
	acc_App_ID_Frame_Counter		= 0x0E,
	acc_App_ID_Data_FIFO			= 0x0F,
	acc_App_ID_Event_Queue			= 0x10,
	acc_App_ID_Status_Register		= 0x11,
	acc_App_ID_Sleep_Wake			= 0x12,
	acc_App_ID_Pedometer			= 0x15,
	acc_App_ID_Reset_Suspend_Clear	= 0x17,
	acc_App_ID_Mailbox_Config		= 0x18

} acc_App_ID_t;

/*---------------------------------------------------------------------------------------------------------------------+
| ACC Transmission Commands
+---------------------------------------------------------------------------------------------------------------------*/

#define ACC_COMMAND_READ_VER		0
#define ACC_COMMAND_READ_CONF		1
#define ACC_COMMAND_WRITE_CONF		2
#define ACC_COMMAND_READ_STAT		3

/*---------------------------------------------------------------------------------------------------------------------+
| ACC Scheduler Commands
+---------------------------------------------------------------------------------------------------------------------*/

typedef enum {
	acc_Task_Priority_488Hz	= 0x17,
	acc_Task_Priority_244Hz	= 0x16,
	acc_Task_Priority_122Hz	= 0x15,
	acc_Task_Priority_61Hz	= 0x14,
	acc_Task_Priority_30Hz	= 0x13,
	acc_Task_Priority_15Hz	= 0x12,
	acc_Task_Priority_7Hz	= 0x11,
	acc_Task_Priority_3Hz	= 0x10,
} acc_Task_Priority_t;

typedef enum {
	acc_Task_Activity_Always	= 0x03,
	acc_Task_Activity_High		= 0x02,
	acc_Task_Activity_Low		= 0x01,
	acc_Task_Activity_Never		= 0x00,
} acc_Task_Activity_t;

#define ACC_SCH_ALWAYS			0x03 // execute application during high and low activity
#define ACC_SCH_ACTIVE			0x02 // execute application only during high activity
#define ACC_SCH_INACTIVE		0x01 // execute application only during low activity
#define ACC_SCH_NEVER			0x00 // never execute application

/*---------------------------------------------------------------------------------------------------------------------+
| ACC Transmission Errors
+---------------------------------------------------------------------------------------------------------------------*/

typedef enum {
	acc_Error_None				= 0x00,
	acc_Error_Param				= 0x04,
	acc_Error_Invalid_Count		= 0x19,
	acc_Error_Command			= 0x1C,
	acc_Error_Invalid_Length	= 0x21,
	acc_Error_FIFO_Busy			= 0x22,
	acc_Error_FIFO_Allocated	= 0x23,
	acc_Error_FIFO_Oversize		= 0x24,
} acc_Error_t;

/*---------------------------------------------------------------------------------------------------------------------+
| ACC AFE Config Registers
+---------------------------------------------------------------------------------------------------------------------*/

#define ACC_CAFE_CSR			0x00
#define ACC_CAFE_xOFFSET		0x02
#define ACC_CAFE_yOFFSET		0x04
#define ACC_CAFE_zOFFSET		0x06
#define ACC_CAFE_HPF			0x08
#define ACC_CAFE_LPF			0x09
#define ACC_CAFE_TEMP_LPF		0x0A
#define ACC_CAFE_EICPF			0x0B
#define ACC_CAFE_SFDRATE		0x0C

/*---------------------------------------------------------------------------------------------------------------------+
| ACC AFE Config Values
+---------------------------------------------------------------------------------------------------------------------*/

typedef enum {
	acc_AFE_SFD_Rate_488Hz	= 0x07,
	acc_AFE_SFD_Rate_244Hz	= 0x08,
	acc_AFE_SFD_Rate_122Hz	= 0x09,
	acc_AFE_SFD_Rate_61Hz	= 0x0A,
	acc_AFE_SFD_Rate_30Hz	= 0x0B,
	acc_AFE_SFD_Rate_15Hz	= 0x0C,
	acc_AFE_SFD_Rate_7Hz	= 0x0D,
	acc_AFE_SFD_Rate_3Hz	= 0x0E,
} acc_AFE_SFD_Rate_t;

typedef enum {
	acc_AFE_LPF_Cut_off_frequency_SFD_Rate_div_by_9			= 0x01,
	acc_AFE_LPF_Cut_off_frequency_SFD_Rate_div_by_20		= 0x02,
	acc_AFE_LPF_Cut_off_frequency_SFD_Rate_div_by_50		= 0x03,
	acc_AFE_LPF_Cut_off_frequency_SFD_Rate_div_by_100		= 0x04,
	acc_AFE_LPF_Cut_off_frequency_SFD_Rate_div_by_200		= 0x05,
	acc_AFE_LPF_Cut_off_frequency_SFD_Rate_div_by_400		= 0x06,
	acc_AFE_LPF_Cut_off_frequency_SFD_Rate_div_by_800		= 0x07,
	acc_AFE_LPF_Cut_off_frequency_SFD_Rate_div_by_1600		= 0x08,
	acc_AFE_LPF_Cut_off_frequency_SFD_Rate_div_by_3200		= 0x09,
	acc_AFE_LPF_Cut_off_frequency_SFD_Rate_div_by_6500		= 0x0A,
	acc_AFE_LPF_Cut_off_frequency_SFD_Rate_div_by_12500		= 0x0B,
	acc_AFE_LPF_Cut_off_frequency_SFD_Rate_div_by_25000		= 0x0C,
	acc_AFE_LPF_Cut_off_frequency_SFD_Rate_div_by_50000		= 0x0D,
	acc_AFE_LPF_Cut_off_frequency_SFD_Rate_div_by_100000	= 0x0E,
	acc_AFE_LPF_Cut_off_frequency_SFD_Rate_div_by_200000	= 0x0F,
} acc_AFE_LPF_Cut_off_frequency_t;

typedef enum {
	acc_AFE_HPF_Cut_off_frequency_SFD_Rate_div_by_15		= 0x01,
	acc_AFE_HPF_Cut_off_frequency_SFD_Rate_div_by_30		= 0x02,
	acc_AFE_HPF_Cut_off_frequency_SFD_Rate_div_by_50		= 0x03,
	acc_AFE_HPF_Cut_off_frequency_SFD_Rate_div_by_100		= 0x04,
	acc_AFE_HPF_Cut_off_frequency_SFD_Rate_div_by_200		= 0x05,
	acc_AFE_HPF_Cut_off_frequency_SFD_Rate_div_by_400		= 0x06,
	acc_AFE_HPF_Cut_off_frequency_SFD_Rate_div_by_800		= 0x07,
	acc_AFE_HPF_Cut_off_frequency_SFD_Rate_div_by_1600		= 0x08,
	acc_AFE_HPF_Cut_off_frequency_SFD_Rate_div_by_3200		= 0x09,
	acc_AFE_HPF_Cut_off_frequency_SFD_Rate_div_by_6500		= 0x0A,
	acc_AFE_HPF_Cut_off_frequency_SFD_Rate_div_by_12500		= 0x0B,
	acc_AFE_HPF_Cut_off_frequency_SFD_Rate_div_by_25000		= 0x0C,
	acc_AFE_HPF_Cut_off_frequency_SFD_Rate_div_by_50000		= 0x0D,
	acc_AFE_HPF_Cut_off_frequency_SFD_Rate_div_by_100000	= 0x0E,
	acc_AFE_HPF_Cut_off_frequency_SFD_Rate_div_by_200000	= 0x0F,
} acc_AFE_HPF_Cut_off_frequency_t;

typedef enum {
	acc_AFE_ADC_Resolution_16bits	= 0x00,
	acc_AFE_ADC_Resolution_14bits	= 0x01,
	acc_AFE_ADC_Resolution_12bits	= 0x02,
	acc_AFE_ADC_Resolution_10bits	= 0x03,
} acc_AFE_ADC_Resolution_t;

typedef enum {
	acc_AFE_Full_Scale_2G	= 0x01,
	acc_AFE_Full_Scale_4G	= 0x02,
	acc_AFE_Full_Scale_8G	= 0x03,
} acc_AFE_Full_Scale_t;

/*---------------------------------------------------------------------------------------------------------------------+
| ACC AFE Status Registers
+---------------------------------------------------------------------------------------------------------------------*/

#define ACC_SAFE_STAGE0_REG 	0x00	// normalized with 100hz bandwidth
#define ACC_SAFE_STAGE1_REG 	0x06	// normalized with 50hz bandwidth
#define ACC_SAFE_ABS0			0x0C  	// absolute value normalized 100hz
#define ACC_SAFE_GM0			0x12	// raw data
#define ACC_SAFE_LPF			0x18	// low pass filter cutoff
#define ACC_SAFE_HPF			0x1E	// high pass filter cutoff
#define ACC_SAFE_TEMP			0x24	// sensor temperature

/*---------------------------------------------------------------------------------------------------------------------+
| ACC Tap Status Registers
+---------------------------------------------------------------------------------------------------------------------*/

#define ACC_STAP_SINGLE_REG		0x00
// Bits definitions
#define ACC_STAP_SING_EVENT			7
#define ACC_STAP_SING_ZDIR			5
#define ACC_STAP_SING_ZEVENT		4
#define ACC_STAP_SING_YDIR			3
#define ACC_STAP_SING_YEVENT		2
#define ACC_STAP_SING_XDIR			1
#define ACC_STAP_SING_XEVENT		0

#define ACC_STAP_DOUBLE_REG		0x01
//Bits definitions
#define ACC_STAP_DOUB_EVENT			7
#define ACC_STAP_DOUB_ZDIR			5
#define ACC_STAP_DOUB_ZEVENT		4
#define ACC_STAP_DOUB_YDIR			3
#define ACC_STAP_DOUB_YEVENT		2
#define ACC_STAP_DOUB_XDIR			1
#define ACC_STAP_DOUB_XEVENT		0

/*---------------------------------------------------------------------------------------------------------------------+
| ACC Tilt Config Registers
+---------------------------------------------------------------------------------------------------------------------*/

#define ACC_CTILT_K_LP			0x00
#define ACC_CTILT_CFG1			0x01
#define ACC_CTILT_EVENT_MASK	0x02

/*---------------------------------------------------------------------------------------------------------------------+
| ACC Tilt Config Values
+---------------------------------------------------------------------------------------------------------------------*/

typedef enum {
	acc_Tilt_XY_Plane_Sensing_Enabled		= 0b01000000,
	acc_Tilt_YZ_Plane_Sensing_Enabled		= 0b00100000,
	acc_Tilt_XZ_Plane_Sensing_Enabled		= 0b00010000,
} acc_Tilt_Plane_Sensing_Config_t;

typedef enum {
	acc_Tilt_Quadrant_Change_Registered		= 0b10000000,
	acc_Tilt_Quadrant_Change_Not_Registered	= 0b00000000,
} acc_Tilt_Quadrant_Change_Registered_t;

/*---------------------------------------------------------------------------------------------------------------------+
| ACC Data FIFO Config Registers
+---------------------------------------------------------------------------------------------------------------------*/

#define ACC_CFIFO_CONFIG_BYTE			0x00
#define ACC_CFIFO_SIZE_WORD				0x04
#define ACC_CFIFO_APP_ID				0x06
#define ACC_CFIFO_WATERMARK				0x08

/*---------------------------------------------------------------------------------------------------------------------+
| ACC Data FIFO Config Values
+---------------------------------------------------------------------------------------------------------------------*/

typedef enum {
	acc_FIFO_Payload_Size_One_Byte		= 0b00000000,
	acc_FIFO_Payload_Size_Two_Bytes		= 0b00000100,
	acc_FIFO_Payload_Size_Four_Bytes	= 0b00001000,
	acc_FIFO_Payload_Size_Six_Bytes		= 0b00001100,

} acc_FIFO_Payload_Size_t;

typedef enum {
	acc_FIFO_Mode_Free_Run		= 0b00000000,
	acc_FIFO_Mode_Overflow_Mode	= 0b00000001,

} acc_FIFO_Mode_t;

/*---------------------------------------------------------------------------------------------------------------------+
| ACC FIFO Status Registers
+---------------------------------------------------------------------------------------------------------------------*/

#define ACC_SFIFO_RECORDS_NUMBER		0x00
#define ACC_SFIFO_ENTRY_SIZE			0x02
#define ACC_SFIFO_STATUS				0x03

/*---------------------------------------------------------------------------------------------------------------------+
| ACC Other Registers
+---------------------------------------------------------------------------------------------------------------------*/

#define ACC_CSLEEP_CFG			0x06

#define ACC_SFRAMEC_REG			0x00

/*---------------------------------------------------------------------------------------------------------------------+
| ACC Pedometr Status Registers
+---------------------------------------------------------------------------------------------------------------------*/

#define ACC_SPEDO_STATUS		0x00
#define ACC_SPEDO_STEPCNT		0x02
#define ACC_SPEDO_DIST			0x04
#define ACC_SPEDO_SPEED			0x06
#define ACC_SPEDO_CALORIES		0x08

typedef enum {Female, Male} Gender_t;

/*---------------------------------------------------------------------------------------------------------------------+
 | Default Pedometer Configuration
 +---------------------------------------------------------------------------------------------------------------------*/

#define ACC_CPEDO_SLEEP_MIN				8000
#define ACC_CPEDO_SLEEP_MAX				10000
#define ACC_CPEDO_SLEEP_THRES			90
#define ACC_CPEDO_CONF					1
#define ACC_CPEDO_ACT					1
#define ACC_CPEDO_SLP					1
#define ACC_CPEDO_STEPLEN				0 	//Step length in centimeters, but if 0 it will calculate from height and weight
#define ACC_CPEDO_DEF_HEIGHT			175
#define ACC_CPEDO_DEF_WEIGHT			7

#define ACC_CPEDO_FILSTEP				4	//range 0:6
#define ACC_CPEDO_FILTTIME				3	//range 0:127
#define ACC_CPEDO_SPEED_PERIOD			5	//range 2:5
#define ACC_CPEDO_STEPCOL				1	//range 0:255
#define ACC_CPEDO_ACTTHD				0   //range 0:0xFFFF

/*---------------------------------------------------------------------------------------------------------------------+
| ACC Pedometr Activity State
+---------------------------------------------------------------------------------------------------------------------*/

typedef enum {
	acc_Activity_Unknown	= 0x00,
	acc_Activity_Rest		= 0x01,
	acc_Activity_Walking	= 0x02,
	acc_Activity_Jogging	= 0x03,
	acc_Activity_Running	= 0x04,
} acc_Activity_t;

/*---------------------------------------------------------------------------------------------------------------------+
| ACC Mailbox Config Values
+---------------------------------------------------------------------------------------------------------------------*/

typedef enum {
	acc_Mailbox_Config_INT_O_EN									= 0b10000000,
	acc_Mailbox_Config_INT_O_POL								= 0b01000000,
	acc_Mailbox_Config_INT_O_FRAME_EN							= 0b00100000,
	acc_Mailbox_Config_INT_O_Legacy_Mode						= 0b00010000,
	acc_Mailbox_Config_INT_UpdMode_Always						= 0b00000000,
	acc_Mailbox_Config_INT_UpdMode_Port_Inactive				= 0b00000100,
	acc_Mailbox_Config_INT_UpdMode_Port_Inactive_With_Interrupt	= 0b00001100,

} acc_Mailbox_Config_t;

typedef enum {
	acc_Communication_mode_Normal = 0,
	acc_Communication_mode_Legacy = 1,
} acc_Communication_mode_t;


#endif /* ACC_DEF_H_ */
