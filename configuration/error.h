/**
 * \file error.h
 * \brief Error codes used in program, functions for error handling.
 *
 * Error codes used in program, functions for error handling.
 *
 * prefix: error
 *
 * \author: Freddie Chopin, http://www.freddiechopin.info http://www.distortec.com
 * \date 2012-08-30
 */

#ifndef ERROR_H_
#define ERROR_H_

//#include <cstdint>

#include "FreeRTOS.h"
#include "projdefs.h"

#include "ff.h"

/*---------------------------------------------------------------------------------------------------------------------+
| global variables' types
+---------------------------------------------------------------------------------------------------------------------*/

/// error (and result) values for whole project. It is assumed that 0 means "no error", negative values are for fatal
/// errors, while positive are for non-fatal ones
enum Error
{
	// --- negative values ---
	ERROR_FreeRTOS_errSCHEDULER_FAIL,		// Scheduler leak

	// FreeRTOS errors from projdefs.h

	ERROR_FreeRTOS_errQUEUE_YIELD = -6,		// errQUEUE_YIELD == -5 in FreeRTOS
	ERROR_FreeRTOS_errQUEUE_BLOCKED,		// errQUEUE_BLOCKED == -4 in FreeRTOS
	// -3 is not assigned to any error in FreeRTOS
	ERROR_FreeRTOS_errNO_TASK_TO_RUN = ERROR_FreeRTOS_errQUEUE_BLOCKED + 2,	// errNO_TASK_TO_RUN == -2 in FreeRTOS
	ERROR_FreeRTOS_errCOULD_NOT_ALLOCATE_REQUIRED_MEMORY,	// errCOULD_NOT_ALLOCATE_REQUIRED_MEMORY == -1 in FreeRTOS
	// pdFAIL == errQUEUE_EMPTY == errQUEUE_FULL == 0 in FreeRTOS
	ERROR_FreeRTOS_pdFAIL, ERROR_FreeRTOS_errQUEUE_EMPTY = ERROR_FreeRTOS_pdFAIL, ERROR_FreeRTOS_errQUEUE_FULL = ERROR_FreeRTOS_pdFAIL,

	// --- NOERROR success value 0 ---

	ERROR_NONE = 0,

	ERROR_FreeRTOS_pdPASS = ERROR_NONE, // pdPASS == 1 in FreeRTOS
	ERROR_FatFS_FR_OK = ERROR_NONE,									/* (0) Succeeded */

	// FatFS results from ff.h

	ERROR_FatFS_FR_DISK_ERR = FR_DISK_ERR,							/* (1) A hard error occured in the low level disk I/O layer */
	ERROR_FatFS_FR_INT_ERR = FR_INT_ERR,							/* (2) Assertion failed */
	ERROR_FatFS_FR_NOT_READY = FR_NOT_READY,						/* (3) The physical drive cannot work */
	ERROR_FatFS_FR_NO_FILE = FR_NO_FILE,							/* (4) Could not find the file */
	ERROR_FatFS_FR_NO_PATH = FR_NO_PATH,							/* (5) Could not find the path */
	ERROR_FatFS_FR_INVALID_NAME = FR_INVALID_NAME,					/* (6) The path name format is invalid */
	ERROR_FatFS_FR_DENIED = FR_DENIED,								/* (7) Acces denied due to prohibited access or directory full */
	ERROR_FatFS_FR_EXIST = FR_EXIST,								/* (8) Acces denied due to prohibited access */
	ERROR_FatFS_FR_INVALID_OBJECT = FR_INVALID_OBJECT,				/* (9) The file/directory object is invalid */
	ERROR_FatFS_FR_WRITE_PROTECTED = FR_WRITE_PROTECTED,			/* (10) The physical drive is write protected */
	ERROR_FatFS_FR_INVALID_DRIVE = FR_INVALID_DRIVE,				/* (11) The logical drive number is invalid */
	ERROR_FatFS_FR_NOT_ENABLED = FR_NOT_ENABLED,					/* (12) The volume has no work area */
	ERROR_FatFS_FR_NO_FILESYSTEM = FR_NO_FILESYSTEM,				/* (13) There is no valid FAT volume */
	ERROR_FatFS_FR_MKFS_ABORTED = FR_MKFS_ABORTED,					/* (14) The f_mkfs() aborted due to any parameter error */
	ERROR_FatFS_FR_TIMEOUT = FR_TIMEOUT,							/* (15) Could not get a grant to access the volume within defined period */
	ERROR_FatFS_FR_LOCKED = FR_LOCKED,								/* (16) The operation is rejected according to the file shareing policy */
	ERROR_FatFS_FR_NOT_ENOUGH_CORE = FR_NOT_ENOUGH_CORE,			/* (17) LFN working buffer could not be allocated */
	ERROR_FatFS_FR_TOO_MANY_OPEN_FILES = FR_TOO_MANY_OPEN_FILES,	/* (18) Number of open files > _FS_SHARE */
	ERROR_FatFS_FR_INVALID_PARAMETER = FR_INVALID_PARAMETER,		/* (19) Given parameter is invalid */

	// --- PERIPHERALS errors and issues

	// --- communications on MainBus <I2C> errors ---
	ERROR_MAINBUSS_TARGET_UNREACHABLE,
	ERROR_MAINBUSS_MODULE_NOT_READY,
	ERROR_MAINBUSS_DATA_NOT_READY,
	ERROR_MAINBUSS_BUS_CORRUPTION,

	// --- END OF PERIPHERALS

	// --- positive values ---

	// commands

	ERROR_COMMAND_NOT_FOUND,

	// various

	ERROR_BUFFER_OVERFLOW,

};

/*---------------------------------------------------------------------------------------------------------------------+
| global functions
+---------------------------------------------------------------------------------------------------------------------*/

/**
 * \brief Converts FatFS FRESULT to enum Error.
 *
 * Converts FatFS FRESULT to enum Error.
 *
 * \param [in] fresult is a FRESULT from FatFS
 *
 * \return fresult converted to enum Error
 */

static inline enum Error errorConvert_FRESULT(FRESULT fresult)
{
	return (enum Error)(fresult + ERROR_FatFS_FR_OK);
}

/**
 * \brief Converts FreeRTOS portBASE_TYPE to enum Error.
 *
 * Converts FreeRTOS portBASE_TYPE to enum Error.
 *
 * \param [in] ret is a portBASE_TYPE return value from FreeRTOS
 *
 * \return ret converted to enum Error
 */

static inline enum Error errorConvert_portBASE_TYPE(portBASE_TYPE ret)
{
	return (enum Error)(ret - pdPASS + (int32_t)ERROR_FreeRTOS_pdPASS);
}

#endif /* ERROR_H_ */
