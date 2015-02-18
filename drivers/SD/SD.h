/*
 * SD.h
 *
 *  Created on: 12 lut 2015
 *      Author: Adrian
 */

#ifndef SD_H_
#define SD_H_

/*---------------------------------------------------------------------------------------------------------------------+
| global defines
+---------------------------------------------------------------------------------------------------------------------*/

#define SD_DUMMY_BYTE					0xFF


typedef enum
{
	/**
	 * @brief  SD reponses and error flags
	 */
	SD_RESPONSE_NO_ERROR      = (0x00),
	SD_IN_IDLE_STATE          = (0x01),
	SD_ERASE_RESET            = (0x02),
	SD_ILLEGAL_COMMAND        = (0x04),
	SD_COM_CRC_ERROR          = (0x08),
	SD_ERASE_SEQUENCE_ERROR   = (0x10),
	SD_ADDRESS_ERROR          = (0x20),
	SD_PARAMETER_ERROR        = (0x40),
	SD_RESPONSE_FAILURE       = (0xFF),

	/**
	 * @brief  Data response error
	 */
	SD_DATA_OK                = (0x05),
	SD_DATA_CRC_ERROR         = (0x0B),
	SD_DATA_WRITE_ERROR       = (0x0D),
	SD_DATA_OTHER_ERROR       = (0xFF)
} SD_Error;

/**
  * @brief  Start Data tokens:
  *         Tokens (necessary because at nop/idle (and CS active) only 0xff is
  *         on the data/command line)
  */
#define SD_START_DATA_SINGLE_BLOCK_READ    0xFE  /*!< Data token start byte, Start Single Block Read */
#define SD_START_DATA_MULTIPLE_BLOCK_READ  0xFE  /*!< Data token start byte, Start Multiple Block Read */
#define SD_START_DATA_SINGLE_BLOCK_WRITE   0xFE  /*!< Data token start byte, Start Single Block Write */
#define SD_START_DATA_MULTIPLE_BLOCK_WRITE 0xFD  /*!< Data token start byte, Start Multiple Block Write */
#define SD_STOP_DATA_MULTIPLE_BLOCK_WRITE  0xFD  /*!< Data toke stop byte, Stop Multiple Block Write */


/**
  * @brief  Commands: CMDxx = CMD-number | 0x40
  */
#define SD_CMD_GO_IDLE_STATE          	0   /*!< CMD0 = 0x40 */
#define SD_CMD_SEND_OP_COND         	1   /*!< CMD1 = 0x41 */
#define SD_CMD_SEND_CSD             	9   /*!< CMD9 = 0x49 */
#define SD_CMD_SEND_CID             	10  /*!< CMD10 = 0x4A */
#define SD_CMD_STOP_TRANSMISSION     	12  /*!< CMD12 = 0x4C */
#define SD_CMD_SEND_STATUS          	13  /*!< CMD13 = 0x4D */
#define SD_CMD_SET_BLOCKLEN          	16  /*!< CMD16 = 0x50 */
#define SD_CMD_READ_SINGLE_BLOCK     	17  /*!< CMD17 = 0x51 */
#define SD_CMD_READ_MULT_BLOCK       	18  /*!< CMD18 = 0x52 */
#define SD_CMD_SET_BLOCK_COUNT        	23  /*!< CMD23 = 0x57 */
#define SD_CMD_WRITE_SINGLE_BLOCK    	24  /*!< CMD24 = 0x58 */
#define SD_CMD_WRITE_MULT_BLOCK       	25  /*!< CMD25 = 0x59 */
#define SD_CMD_PROG_CSD               	27  /*!< CMD27 = 0x5B */
#define SD_CMD_SET_WRITE_PROT         	28  /*!< CMD28 = 0x5C */
#define SD_CMD_CLR_WRITE_PROT         	29  /*!< CMD29 = 0x5D */
#define SD_CMD_SEND_WRITE_PROT        	30  /*!< CMD30 = 0x5E */
#define SD_CMD_SD_ERASE_GRP_START     	32  /*!< CMD32 = 0x60 */
#define SD_CMD_SD_ERASE_GRP_END       	33  /*!< CMD33 = 0x61 */
#define SD_CMD_UNTAG_SECTOR           	34  /*!< CMD34 = 0x62 */
#define SD_CMD_ERASE_GRP_START        	35  /*!< CMD35 = 0x63 */
#define SD_CMD_ERASE_GRP_END          	36  /*!< CMD36 = 0x64 */
#define SD_CMD_UNTAG_ERASE_GROUP      	37  /*!< CMD37 = 0x65 */
#define SD_CMD_ERASE                  	38  /*!< CMD38 = 0x66 */

/*---------------------------------------------------------------------------------------------------------------------+
| global functions' prototypes
+---------------------------------------------------------------------------------------------------------------------*/

SD_Error SD_Init(void);

uint32_t SD_spiSetBaudRate(uint32_t baud_rate);

void SD_WriteByte(uint8_t Data);

uint8_t SD_ReadByte(void);

void SD_SendCommand(uint8_t Cmd, uint32_t Arg, uint8_t Crc);

SD_Error SD_GetResponse(uint8_t Response);

uint8_t SD_GetDataResponse(void);

SD_Error SD_GoIdleState(void);

SD_Error SD_WriteBlock(uint8_t* pBuffer, uint32_t WriteAddr, uint16_t BlockSize);

SD_Error SD_ReadBlock(uint8_t* pBuffer, uint32_t ReadAddr, uint16_t BlockSize);


#endif /* SD_H_ */
