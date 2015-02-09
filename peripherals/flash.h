/**
* \file st_flash.h
* \brief   This file provides simple interface to
*           STM32 Flash peripherial and use ST HAL Lib macros
* \date 16-09-2014
* \author: Maciek Grochowski
*/
#ifndef ST_FLASH_H
#define ST_FLASH_H

#include "stm32l1xx.h"
#include "config.h"

#ifdef __cplusplus
 extern "C" {
#endif

/******************  Bit definition for FLASH_SR register  *******************/
#define  FLASH_SR_BSY                        ((uint32_t)0x00000001)        /*!< Busy */
#define  FLASH_SR_EOP                        ((uint32_t)0x00000002)        /*!< End Of Programming*/
#define  FLASH_SR_ENHV                       ((uint32_t)0x00000004)        /*!< End of high voltage */
#define  FLASH_SR_READY                      ((uint32_t)0x00000008)        /*!< Flash ready after low power mode */

#define  FLASH_SR_WRPERR                     ((uint32_t)0x00000100)        /*!< Write protected error */
#define  FLASH_SR_PGAERR                     ((uint32_t)0x00000200)        /*!< Programming Alignment Error */
#define  FLASH_SR_SIZERR                     ((uint32_t)0x00000400)        /*!< Size error */
#define  FLASH_SR_OPTVERR                    ((uint32_t)0x00000800)        /*!< Option validity error */
#define  FLASH_SR_OPTVERRUSR                 ((uint32_t)0x00001000)        /*!< Option User validity error */

/* Private define ------------------------------------------------------------*/
#define DATA_EEPROM_START_ADDR     0x08080000
#define DATA_EEPROM_END_ADDR       0x080803FF
#define DATA_EEPROM_PAGE_SIZE      0x8

/**
* @brief  FLASH Error structure definition
*/
typedef enum
{
    FLASH_ERROR_SIZE      = 0x01,
    FLASH_ERROR_OPTV      = 0x02,
    FLASH_ERROR_OPTVUSR   = 0x04,
    FLASH_ERROR_PGA       = 0x08,
    FLASH_ERROR_WRP       = 0x10,
    FLASH_ERROR_RD        = 0x20,
    FLASH_ERROR_OPERATION = 0x40
}FLASH_ErrorTypeDef;


/**
* @brief  FLASH Procedure structure definition
*/
typedef enum
{
    FLASH_PROC_NONE = 0,
    FLASH_PROC_PAGEERASE,
    FLASH_PROC_PROGRAM
} FLASH_ProcedureTypeDef;

typedef FLASH_ErrorTypeDef FLASH_Status;

/**
* FLASH_Flags
*/

#define FLASH_FLAG_BSY                 FLASH_SR_BSY        /*!< FLASH Busy flag */
#define FLASH_FLAG_EOP                 FLASH_SR_EOP        /*!< FLASH End of Programming flag */

#define FLASH_FLAG_READY               FLASH_SR_READY      /*!< FLASH Ready flag after low power mode */
#define FLASH_FLAG_WRPERR              FLASH_SR_WRPERR     /*!< FLASH Write protected error flag */
#define FLASH_FLAG_PGAERR              FLASH_SR_PGAERR     /*!< FLASH Programming Alignment error flag */
#define FLASH_FLAG_SIZERR              FLASH_SR_SIZERR     /*!< FLASH Size error flag  */
#define FLASH_FLAG_OPTVERR             FLASH_SR_OPTVERR    /*!< FLASH Option Validity error flag  */
#define FLASH_FLAG_OPTVERRUSR          FLASH_SR_OPTVERRUSR /*!< FLASH Option User Validity error flag  */

#define FLASH_BUSY            (FLASH_ErrorTypeDef)1
#define FLASH_ERROR_WRP       (FLASH_ErrorTypeDef)2
#define FLASH_ERROR_PROGRAM   (FLASH_ErrorTypeDef)3
#define FLASH_COMPLETE        (FLASH_ErrorTypeDef)4
#define FLASH_TIMEOUT         (FLASH_ErrorTypeDef)5

/**
* \brief FLASH_Keys FLASH Keys
*/

#define FLASH_PDKEY1               ((uint32_t)0x04152637) /*!< Flash power down key1 */
#define FLASH_PDKEY2               ((uint32_t)0xFAFBFCFD) /*!< Flash power down key2: used with FLASH_PDKEY1
                                                              to unlock the RUN_PD bit in FLASH_ACR */

#define FLASH_PEKEY1               ((uint32_t)0x89ABCDEF) /*!< Flash program erase key1 */
#define FLASH_PEKEY2               ((uint32_t)0x02030405) /*!< Flash program erase key: used with FLASH_PEKEY2
                                                               to unlock the write access to the FLASH_PECR register and
                                                               data EEPROM */

#define FLASH_PRGKEY1              ((uint32_t)0x8C9DAEBF) /*!< Flash program memory key1 */
#define FLASH_PRGKEY2              ((uint32_t)0x13141516) /*!< Flash program memory key2: used with FLASH_PRGKEY2
                                                               to unlock the program memory */

#define FLASH_OPTKEY1              ((uint32_t)0xFBEAD9C8) /*!< Flash option key1 */
#define FLASH_OPTKEY2              ((uint32_t)0x24252627) /*!< Flash option key2: used with FLASH_OPTKEY1 to
                                                              unlock the write access to the option byte block */

/**
* Timeout_definition
*/
#define FLASH_ER_PRG_TIMEOUT         ((uint32_t)0x8000)

/* FLASH Interface configuration functions ************************************/

/**
 * \brief  Sets the code latency value.
 * \param  FLASH_Latency: specifies the FLASH Latency value.
 *   This parameter can be one of the following values:
 *     \arg FLASH_Latency_0: FLASH Zero Latency cycle.
 *     \arg FLASH_Latency_1: FLASH One Latency cycle.
 * \retval None
 */
void FLASH_SetLatency(uint32_t FLASH_Latency);

/**
 * \brief  Enable or disable the power down mode during Sleep mode.
 * \note   This function is used to power down the FLASH when the system is in SLEEP LP mode.
 * \param  NewState: new state of the power down mode during sleep mode.
 *   This parameter can be: ENABLE or DISABLE.
 * \retval None
 */
void FLASH_SLEEPPowerDownCmd(FunctionalState NewState);

/**
 * \brief  Unlock and clean pending flash flags.
 * 		   After this flash should be ready to write/read data.
 * \param  None
 * \retval None
 */
void FLASH_PrepareDefaultConf(void);

/* FLASH Memory Programming functions *****************************************/

/**
 * \brief  Unlocks the FLASH control register and program memory access.
 * \param  None
 * \retval None
 */
void FLASH_Unlock(void);

/**
 * \brief  Locks the Program memory access.
 * \param  None
 * \retval None
 */
void FLASH_Lock(void);

/**
 * \brief  Erases a specified page in program memory.
 * \note   To correctly run this function, the FLASH_Unlock() function
 *         must be called before.
 *         Call the FLASH_Lock() to disable the flash memory access
 *         (recommended to protect the FLASH memory against possible unwanted operation)
 * \param  Page_Address: The page address in program memory to be erased.
 * \note   A Page is erased in the Program memory only if the address to load
 *         is the start address of a page (multiple of 256 bytes).
 * \retval FLASH Status: The returned value can be:
 *   FLASH_ERROR_PROGRAM, FLASH_ERROR_WRP, FLASH_COMPLETE or FLASH_TIMEOUT.
 */
FLASH_Status FLASH_ErasePage(uint32_t Page_Address);

/**
 * \brief  Programs a word at a specified address in program memory.
 * \note   To correctly run this function, the FLASH_Unlock() function
 *         must be called before.
 *         Call the FLASH_Lock() to disable the flash memory access
 *         (recommended to protect the FLASH memory against possible unwanted operation).
 * \param  Address: specifies the address to be written.
 * \param  Data: specifies the data to be written.
 * \retval FLASH Status: The returned value can be:
 *   FLASH_ERROR_PROGRAM, FLASH_ERROR_WRP, FLASH_COMPLETE or FLASH_TIMEOUT.
 */
FLASH_Status FLASH_FastProgramWord(uint32_t Address, uint32_t Data);

/* DATA EEPROM Programming functions ******************************************/

/**
 * \brief  Unlock and clean pending flash flags.
 * 		   After this flash should be ready to write/read data.
 * \param  None
 * \retval None
 */
void DATA_EEPROM_Unlock(void);

/**
 * \brief  Locks the Data memory and FLASH_PECR register access.
 * \param  None
 * \retval None
 */
void DATA_EEPROM_Lock(void);

/**
 * \brief  Erase a word in data memory.
 * \param  Address: specifies the address to be erased.
 * \note   For STM32L1XX_MD, A data memory word is erased in the data memory only
 *         if the address to load is the start address of a word (multiple of a word).
 * \note   To correctly run this function, the DATA_EEPROM_Unlock() function
 *         must be called before.
 *         Call the DATA_EEPROM_Lock() to disable the data EEPROM access
 *         and Flash program erase control register access(recommended to protect
 *         the DATA_EEPROM against possible unwanted operation).
 * \retval FLASH Status: The returned value can be:
 *   FLASH_ERROR_PROGRAM, FLASH_ERROR_WRP, FLASH_COMPLETE or FLASH_TIMEOUT.
 */
FLASH_Status DATA_EEPROM_EraseWord(uint32_t Address);

/**
 * \brief  Programs a word at a specified address in data memory.
 * \note   To correctly run this function, the DATA_EEPROM_Unlock() function
 *         must be called before.
 *         Call the DATA_EEPROM_Lock() to disable the data EEPROM access
 *         and Flash program erase control register access(recommended to protect
 *         the DATA_EEPROM against possible unwanted operation).
 * \param  Address: specifies the address to be written.
 * \param  Data: specifies the data to be written.
 * \note   This function assumes that the is data word is already erased.
 * \retval FLASH Status: The returned value can be:
 *         FLASH_ERROR_PROGRAM, FLASH_ERROR_WRP, FLASH_COMPLETE or FLASH_TIMEOUT.
 */
FLASH_Status DATA_EEPROM_FastProgramWord(uint32_t Address, uint32_t Data);

/**
* \brief  Programs a word at a specified address in data memory without erase.
* \note   To correctly run this function, the DATA_EEPROM_Unlock() function
*         must be called before.
*         Call the DATA_EEPROM_Lock() to disable the data EEPROM access
*         and Flash program erase control register access(recommended to protect
*         the DATA_EEPROM against possible unwanted operation).
* \note   The function  DATA_EEPROM_FixedTimeProgramCmd() can be called before
*         this function to configure the Fixed Time Programming.
* \param  Address: specifies the address to be written.
* \param  Data: specifies the data to be written.
* \retval FLASH Status: The returned value can be:
*   FLASH_ERROR_PROGRAM, FLASH_ERROR_WRP, FLASH_COMPLETE or  FLASH_TIMEOUT.
*/
FLASH_Status DATA_EEPROM_ProgramWord(uint32_t Address, uint32_t Data);

/**
* \brief  Waits for a FLASH operation to complete or a TIMEOUT to occur.
* \param  Timeout: FLASH programming Timeout.
* \retval FLASH Status: The returned value can be: FLASH_BUSY,
*   FLASH_ERROR_PROGRAM, FLASH_ERROR_WRP, FLASH_COMPLETE or FLASH_TIMEOUT.
*/
FLASH_Status FLASH_WaitTillLastOperationFinish(uint32_t Timeout);

/* Functional Interface*/

/**
 * \brief   Simply write to whole FLASH region with 0xff
 *           without check of data Correctness.
 * \param  None
 * \retval None
 */
void FLASH_FastErase(void);

/**
* \brief  Clears the FLASH's pending flags.
* \param  FLASH_FLAG: specifies the FLASH flags to clear.
*   This parameter can be any combination of the following values:
*     \arg FLASH_FLAG_EOP: FLASH End of Operation flag
*     \arg FLASH_FLAG_WRPERR: FLASH Write protected error flag
*     \arg FLASH_FLAG_PGAERR: FLASH Programming Alignment error flag
*     \arg FLASH_FLAG_SIZERR: FLASH size error flag
*     \arg FLASH_FLAG_OPTVERR: FLASH Option validity error flag
*     \arg FLASH_FLAG_OPTVERRUSR: FLASH Option User validity error flag
*     \arg FLASH_FLAG_RDERR: FLASH Read protected error flag (available only in STM32L1XX_MDP devices)
* \retval None
*/
void FLASH_ClearFlag(uint32_t FLASH_FLAG);

/**
* \brief  Returns the FLASH Status.
* \param  None
* \retval FLASH Status: The returned value can be:
*   FLASH_BUSY, FLASH_ERROR_PROGRAM, FLASH_ERROR_WRP or FLASH_COMPLETE.
*/
FLASH_Status FLASH_GetStatus(void);

/**
 * \brief Easy way to get word from flash memory
 * \param address to read
 *
 * \return value from given address
 */
inline uint32_t readFromFlashAddr(uint32_t addressToRead){
	return  *(__IO uint32_t*) addressToRead;
}

#ifdef __cplusplus
 }
#endif

#endif //ST_FLASH_H
