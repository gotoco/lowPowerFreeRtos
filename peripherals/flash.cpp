/**
* \file st_flash.cpp
* \brief   This file provides implementation to
*           STM32 Flash peripherial interface and use ST HAL Lib&Macros
* \date 16-09-2014
* \author: Maciek Grochowski
*/
#include "stm32l1xx.h"
#include "flash.h"

/**
* \brief  Unlock and clean pending flash flags.
* 		   After this flash should be ready to write/read data.
*/
void FLASH_PrepareDefaultConf(void)
{
    /* Unlock the FLASH PECR register and Data EEPROM memory */
    DATA_EEPROM_Unlock();

    /* Clear all pending flags */
    FLASH_ClearFlag(  FLASH_FLAG_EOP | FLASH_FLAG_WRPERR | FLASH_FLAG_PGAERR
            | FLASH_FLAG_SIZERR | FLASH_FLAG_OPTVERR
            | FLASH_FLAG_OPTVERRUSR);
}

/**
* \brief  Unlocks the data memory and FLASH_PECR register access.
* \param  None
* \retval None
*/
void DATA_EEPROM_Unlock(void)
{
    if((FLASH->PECR & FLASH_PECR_PELOCK) != RESET)
    {
        /* Unlocking the Data memory and FLASH_PECR register access*/
        FLASH->PEKEYR = FLASH_PEKEY1;
        FLASH->PEKEYR = FLASH_PEKEY2;
    }
}

/**
* \brief  Locks the Data memory and FLASH_PECR register access.
* \param  None
* \retval None
*/
void DATA_EEPROM_Lock(void)
{
    /* Set the PELOCK Bit to lock the data memory and FLASH_PECR register access */
    FLASH->PECR |= FLASH_PECR_PELOCK;
}

/**
*  \brief   Simply write to whole FLASH region with 0xff
*           without check of data Correctness.
*/
void FLASH_FastErase(void)
{
    uint32_t pivot = DATA_EEPROM_START_ADDR;

    while (pivot <= DATA_EEPROM_END_ADDR) {
        FLASH_Status fstat = DATA_EEPROM_ProgramWord(pivot, 0xffffffff);

        if (fstat == FLASH_COMPLETE) {
            pivot = pivot + 4;
        } else {
            FLASH_ClearFlag(
                    FLASH_FLAG_EOP | FLASH_FLAG_WRPERR | FLASH_FLAG_PGAERR
                            | FLASH_FLAG_SIZERR | FLASH_FLAG_OPTVERR);
        }
    }
}

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
FLASH_Status DATA_EEPROM_FastProgramWord(uint32_t Address, uint32_t Data)
{
    FLASH_Status status = FLASH_COMPLETE;

    /* Wait for last operation to be completed */
    status = FLASH_WaitTillLastOperationFinish(FLASH_ER_PRG_TIMEOUT);

    if(status == FLASH_COMPLETE)
    {
        /* Clear the FTDW bit */
        FLASH->PECR &= (uint32_t)(~((uint32_t)FLASH_PECR_FTDW));

        /* If the previous operation is completed, proceed to program the new data */
        *(__IO uint32_t *)Address = Data;

        /* Wait for last operation to be completed */
        status = FLASH_WaitTillLastOperationFinish(FLASH_ER_PRG_TIMEOUT);
    }
    /* Return the Write Status */
    return status;
}

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
FLASH_Status DATA_EEPROM_EraseWord(uint32_t Address)
{
    FLASH_Status status = FLASH_COMPLETE;

    /* Wait for last operation to be completed */
    status = FLASH_WaitTillLastOperationFinish(FLASH_ER_PRG_TIMEOUT);

    if(status == FLASH_COMPLETE)
    {
        /* Write "00000000h" to valid address in the data memory" */
        *(__IO uint32_t *) Address = 0x00000000;
    }

    /* Return the erase status */
    return status;
}

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
FLASH_Status DATA_EEPROM_ProgramWord(uint32_t Address, uint32_t Data)
{
    FLASH_Status status = FLASH_COMPLETE;

    /* Wait for last operation to be completed */
    status = FLASH_WaitTillLastOperationFinish(FLASH_ER_PRG_TIMEOUT);

    if(status == FLASH_COMPLETE)
    {
        *(__IO uint32_t *)Address = Data;

        /* Wait for last operation to be completed */
        status = FLASH_WaitTillLastOperationFinish(FLASH_ER_PRG_TIMEOUT);
    }
    /* Return the Write Status */
    return status;
}

/**
* \brief  Waits for a FLASH operation to complete or a TIMEOUT to occur.
* \param  Timeout: FLASH programming Timeout.
* \retval FLASH Status: The returned value can be: FLASH_BUSY,
*   FLASH_ERROR_PROGRAM, FLASH_ERROR_WRP, FLASH_COMPLETE or FLASH_TIMEOUT.
*/
FLASH_Status FLASH_WaitTillLastOperationFinish(uint32_t Timeout)
{
    __IO FLASH_Status status = FLASH_COMPLETE;

    /* Check for the FLASH Status */
    status = FLASH_GetStatus();

    /* Wait for a FLASH operation to complete or a TIMEOUT to occur */
    while((status == FLASH_BUSY) && (Timeout != 0x00))
    {
        status = FLASH_GetStatus();
        Timeout--;
    }

    if(Timeout == 0x00 ) {
        status = FLASH_TIMEOUT;
    }
    /* Return the operation status */
    return status;
}

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
FLASH_Status FLASH_FastProgramWord(uint32_t Address, uint32_t Data)
{
    FLASH_Status status = FLASH_COMPLETE;

    /* Wait for last operation to be completed */
    status = FLASH_WaitTillLastOperationFinish(FLASH_ER_PRG_TIMEOUT);

    if(status == FLASH_COMPLETE)
    {
        /* If the previous operation is completed, proceed to program the new  word */
        *(__IO uint32_t *)Address = Data;

        /* Wait for last operation to be completed */
        status = FLASH_WaitTillLastOperationFinish(FLASH_ER_PRG_TIMEOUT);
    }
    /* Return the Write Status */
    return status;
}

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
FLASH_Status FLASH_ErasePage(uint32_t Page_Address)
{
    FLASH_Status status = FLASH_COMPLETE;

    /* Wait for last operation to be completed */
    status = FLASH_WaitTillLastOperationFinish(FLASH_ER_PRG_TIMEOUT);

    if(status == FLASH_COMPLETE)
    {
        /* If the previous operation is completed, proceed to erase the page */

        /* Set the ERASE bit */
        FLASH->PECR |= FLASH_PECR_ERASE;

        /* Set PROG bit */
        FLASH->PECR |= FLASH_PECR_PROG;

        /* Write 00000000h to the first word of the program page to erase */
        *(__IO uint32_t *)Page_Address = 0x00000000;

        /* Wait for last operation to be completed */
        status = FLASH_WaitTillLastOperationFinish(FLASH_ER_PRG_TIMEOUT);

        /* If the erase operation is completed, disable the ERASE and PROG bits */
        FLASH->PECR &= (uint32_t)(~FLASH_PECR_PROG);
        FLASH->PECR &= (uint32_t)(~FLASH_PECR_ERASE);
    }
    /* Return the Erase Status */
    return status;
}

/**
* \brief  Locks the Program memory access.
* \param  None
* \retval None
*/
void FLASH_Lock(void)
{
    /* Set the PRGLOCK Bit to lock the program memory access */
    FLASH->PECR |= FLASH_PECR_PRGLOCK;
}

/**
* \brief  Unlocks the FLASH control register and program memory access.
* \param  None
* \retval None
*/
void FLASH_Unlock(void)
{
    if((FLASH->PECR & FLASH_PECR_PRGLOCK) != RESET)
    {
        /* Unlocking the data memory and FLASH_PECR register access */
        DATA_EEPROM_Unlock();

        /* Unlocking the program memory access */
        FLASH->PRGKEYR = FLASH_PRGKEY1;
        FLASH->PRGKEYR = FLASH_PRGKEY2;
    }
}

/**
* \brief  Enable or disable the power down mode during Sleep mode.
* \note   This function is used to power down the FLASH when the system is in SLEEP LP mode.
* \param  NewState: new state of the power down mode during sleep mode.
*   This parameter can be: ENABLE or DISABLE.
* \retval None
*/
void FLASH_SLEEPPowerDownCmd(FunctionalState NewState)
{

    if (NewState != DISABLE)
    {
        /* Set the SLEEP_PD bit to put Flash in power down mode during sleep mode */
        FLASH->ACR |= FLASH_ACR_SLEEP_PD;
    }
    else
    {
        /* Clear the SLEEP_PD bit in to put Flash in idle mode during sleep mode */
        FLASH->ACR &= (uint32_t)(~((uint32_t)FLASH_ACR_SLEEP_PD));
    }
}

/**
* \brief  Sets the code latency value.
* \param  FLASH_Latency: specifies the FLASH Latency value.
*   This parameter can be one of the following values:
*     \arg FLASH_Latency_0: FLASH Zero Latency cycle.
*     \arg FLASH_Latency_1: FLASH One Latency cycle.
* \retval None
*/
void FLASH_SetLatency(uint32_t FLASH_Latency)
{
    uint32_t tmpreg = 0;

    /* Read the ACR register */
    tmpreg = FLASH->ACR;

    /* Sets the Latency value */
    tmpreg &= (uint32_t) (~((uint32_t)FLASH_ACR_LATENCY));
    tmpreg |= FLASH_Latency;

    /* Write the ACR register */
    FLASH->ACR = tmpreg;
}

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
void FLASH_ClearFlag(uint32_t FLASH_FLAG) {

    /* Clear the flags */
    FLASH->SR = FLASH_FLAG;
}

/**
* \brief  Returns the FLASH Status.
* \param  None
* \retval FLASH Status: The returned value can be:
*   FLASH_BUSY, FLASH_ERROR_PROGRAM, FLASH_ERROR_WRP or FLASH_COMPLETE.
*/
FLASH_Status FLASH_GetStatus(void) {
    FLASH_Status FLASHstatus = FLASH_COMPLETE;

    if ((FLASH->SR & FLASH_FLAG_BSY) == FLASH_FLAG_BSY) {
        FLASHstatus = FLASH_BUSY;
    }
    else {
        if ((FLASH->SR & (uint32_t) FLASH_FLAG_WRPERR) != (uint32_t) 0x00) {
            FLASHstatus = FLASH_ERROR_WRP;
        }
        else {
            if ((FLASH->SR & (uint32_t) 0x1E00) != (uint32_t) 0x00) {
                FLASHstatus = FLASH_ERROR_PROGRAM;
            }
            else {
                FLASHstatus = FLASH_COMPLETE;
            }
        }
    }
    /* Return the FLASH Status */
    return FLASHstatus;
}
