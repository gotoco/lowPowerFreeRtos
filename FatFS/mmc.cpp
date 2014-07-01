/*----------------------------------------------------------------------------------------------------------------------
 Original MMCv3/SDv1/SDv2 (in SPI mode) control module  (C)ChaN, 2007
 FreeRTOS+IO version by Real Time Engineers ltd (http://www.FreeRTOS.org)
 Heavily modified by Freddie Chopin, http://www.freddiechopin.info http://www.distortec.com

 Note:  While this file has been reworked as a demonstration of how to use
 FreeRTOS+IO, it does *not* fully comply with the FreeRTOS coding standard.
----------------------------------------------------------------------------------------------------------------------*/

#include <stdbool.h>

#include "diskio.h"
#include "ff.h"

/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "task.h"

#include "config.h"

#include "gpio.h"
#include "spi.h"

/* Place holder for calls to ioctl that don't use the value parameter. */
#define mmcPARAMETER_NOT_USED			( ( void * ) 0 )

/* Clock speed to use before the card type is determined. */
#define mmcSD_INTERFACE_SLOW_CLOCK		100000UL

/* Clock speed to use after the card type has been determined. */
#define mmcSD_INTERFACE_FAST_CLOCK		10000000UL

/* Misc constants required by the MMC SPI protocol. */
#define mmc80_CLOCKS_IN_BYTES			( 10 )
#define mmcCOMMAND_LENGTH_BYTES			6
#define mmcOCR_LENGTH					4
#define mmcMAX_QUERY_STRING_BYTES		16
#define mmcSECTOR_SIZE					512
#define mmcDATA_BLOCK_SIZE				mmcSECTOR_SIZE

/* Definitions for MMC/SDC command */
#define mmcCMD0_SOFTWARE_RESET				( 0x40+0 )	/* GO_IDLE_STATE */
#define mmcCMD1_START_INITIALISE_MMC		( 0x40+1 )	/* SEND_OP_COND (MMC) */
#define	mmcACMD41_START_INIT_SDC			( 0xC0+41 )	/* SEND_OP_COND (SDC) */
#define mmcCMD8_CHECK_VOLTAGE_RANGE			( 0x40+8 )	/* SEND_IF_COND */
#define mmcCMD9_READ_CSD_REGISTER			( 0x40+9 )	/* SEND_CSD */
#define mmcCMD10_READ_CID_REGISTER			( 0x40+10 )	/* SEND_CID */
#define mmcCMD12_STOP						( 0x40+12 )	/* STOP_TRANSMISSION */
#define mmcACMD13_READ_SD_STATUS			( 0xC0+13 )	/* SD_STATUS (SDC) */
#define mmcCMD16_CHANGE_RW_BLOCK_SIZE		( 0x40+16 )	/* SET_BLOCKLEN */
#define mmcCMD17_READ_SINGLE_BLOCK			( 0x40+17 )	/* READ_SINGLE_BLOCK */
#define mmcCMD18_READ_MULTI_BLOCK			( 0x40+18 )	/* READ_MULTIPLE_BLOCK */
#define mmcCMD23_SET_BLOCK_COUNT			( 0x40+23 )	/* SET_BLOCK_COUNT (MMC) */
#define	mmcACMD23_SET_ERASE_COUNT			( 0xC0+23 )	/* SET_WR_BLK_ERASE_COUNT (SDC) */
#define mmcCMD24_WRITE_SINGLE_BLOCK			( 0x40+24 )	/* WRITE_BLOCK */
#define mmcCMD25_WRITE_MULTI_BLOCK			( 0x40+25 )	/* WRITE_MULTIPLE_BLOCK */
#define mmcCMD55_LEADING_COMMAND_OF_ACMD	( 0x40+55 )	/* APP_CMD */
#define mmcCMD58_READ_OCR					( 0x40+58 )	/* READ_OCR */

/* A block time of 500ms, converted to ticks. */
#define mmc500ms		( ( void * ) ( 500UL / portTICK_RATE_MS ) )

/*-----------------------------------------------------------*/

/*
 * Send the command cCommand, with argument xArgument (if any) to the card.
 */
static BYTE prvSendCommand( BYTE cCommand,	DWORD xArgument	);

/*
 * Deselect the card and release the SPI bus.
 */
static void prvDeselectCard( void );

/*
 * Wait for the card to be read.
 */
static BYTE prvWaitForCardReady( void );

/*
 * Select the card and wait for it to be ready.
 */
static bool prvSelectCard( void );

/*
 * Receive a data packet from the card in response to a query command.
 */
static bool prvQueryCard( BYTE ucCommand, DWORD ulCommandArgument, BYTE *pucRxBuffer, UINT uiRxLength );

/*
 * Receive a block of data read from the card.
 */
static bool prvReceiveDataBlock( BYTE *pcBuffer, UINT xBytesToRead );

/*
 * Write a block of data to the card.
 */
static bool prvWriteDataBlock( const BYTE *pcBuffer, BYTE cToken );

/*
 * Check the card is inserted, and set the STA_NODISK bits in the xDiskStatus
 * byte accordingly.  Return pdTRUE if the card is present, and pdFALSE if the
 * card is not present.
 */
static portBASE_TYPE prvIsDiskInserted( void );

/*-----------------------------------------------------------*/

/* Bitwise status for STA_NODISK and STA_NOINIT status of the card. */
static volatile DSTATUS xDiskStatus = STA_NOINIT;

/* Stores the card type discovered during the initialisation process. */
static BYTE ucInsertedCardType = 0U;

/*-----------------------------------------------------------*/

/*-----------------------------------------------------------------------*/
/* Initialize Disk Drive                                                 */
/*-----------------------------------------------------------------------*/
DSTATUS disk_initialize ( BYTE cDriveNumber )
{
BYTE cCommand, cCardType;
DSTATUS xReturn = 0;
BYTE ucBuffer[ mmc80_CLOCKS_IN_BYTES + 1 ];
portTickType xTimeAtStart;
const portTickType xTimeOut = 1000UL / portTICK_RATE_MS;

	static bool initialized = false;

	if (initialized == false)
	{
		gpioConfigurePin(SD_CS_GPIO, SD_CS_PIN, GPIO_OUT_PP_2MHz);
		SD_CS_bb = SD_CS_DEASSERTED;

		spiInitialize();

		initialized = true;
	}

	if( cDriveNumber != 0 )
	{
		/* This driver only supports a single drive. */
		xReturn = STA_NOINIT;
	}
	else if( ( xDiskStatus & STA_NOINIT ) != 0U )
	{
		/* Always start with the slow SPI clock. */
		spiSetBaudRate(mmcSD_INTERFACE_SLOW_CLOCK);

		/* Wait the obligatory 80 clocks. */
		spiRead(ucBuffer, mmc80_CLOCKS_IN_BYTES);

		cCardType = 0;
		if( prvSendCommand( mmcCMD0_SOFTWARE_RESET, 0 ) == 1 )
		{
			/* Enter Idle state.  Initialization timeout of 1000 msec */
			xTimeAtStart = xTaskGetTickCount();

			if( prvSendCommand( mmcCMD8_CHECK_VOLTAGE_RANGE, 0x1AA ) == 1 )
			{
				/* SDHC */
				/* Get trailing return value of R7 resp */
				spiRead(ucBuffer, mmcOCR_LENGTH);

				if( ( ucBuffer[ 2 ] == 0x01 ) && ( ucBuffer[ 3 ] == 0xAA ) )
				{
					/* The card can work at vdd range of 2.7-3.6V */

					/* Wait for leaving idle state (mmcACMD41_START_INIT_SDC
					with HCS bit) */
					while( ( ( xTaskGetTickCount() - xTimeAtStart ) < xTimeOut ) && ( prvSendCommand( mmcACMD41_START_INIT_SDC, 1UL << 30 ) != 0 ) )
					{
						/* Nothing to do here. */
					}

					if( ( ( xTaskGetTickCount() - xTimeAtStart ) < xTimeOut ) && ( prvSendCommand( mmcCMD58_READ_OCR, 0 ) == 0 ) )
					{
						/* Check CCS bit in the OCR */
						spiRead(ucBuffer, mmcOCR_LENGTH);

						/* SDv2 */
						if( ( ucBuffer[ 0 ] & 0x40 ) != 0 )
						{
							cCardType = CT_SD2 | CT_BLOCK;
						}
						else
						{
							cCardType = CT_SD2;
						}
					}
				}
			}
			else
			{
				/* SDSC or MMC */
				if( prvSendCommand( mmcACMD41_START_INIT_SDC, 0 ) <= 1 )
				{
					/* SDv1 */
					cCardType = CT_SD1;
					cCommand = mmcACMD41_START_INIT_SDC;
				}
				else
				{
					/* MMCv3 */
					cCardType = CT_MMC;
					cCommand = mmcCMD1_START_INITIALISE_MMC;
				}

				while( ( ( xTaskGetTickCount() - xTimeAtStart ) < xTimeOut ) && ( prvSendCommand( cCommand, 0 ) != 0 ) )
				{
					/* Wait for leaving idle state */
				}

				if( ( ( xTaskGetTickCount() - xTimeAtStart ) >= xTimeOut ) || ( prvSendCommand( mmcCMD16_CHANGE_RW_BLOCK_SIZE, mmcSECTOR_SIZE ) != 0 ) )
				{
					/* Set R/W block length to mmcSECTOR_SIZE */
					cCardType = 0;
				}
			}
		}

		ucInsertedCardType = cCardType;
		prvDeselectCard();

		if( cCardType != 0 )
		{
			/* Initialization succeeded.  Clear STA_NOINIT */
			xDiskStatus &= ~STA_NOINIT;
			spiSetBaudRate(mmcSD_INTERFACE_FAST_CLOCK);
		}

		xReturn = xDiskStatus;
	}

	return xReturn;
}

/*-----------------------------------------------------------------------*/
/* Get Disk Status                                                       */
/*-----------------------------------------------------------------------*/
DSTATUS disk_status( BYTE cDriveNumber )
{
DSTATUS xReturn;

	/* This call will update xDiskStatus. */
	prvIsDiskInserted();

	if( cDriveNumber != 0 )
	{
		xReturn = STA_NOINIT;
	}
	else
	{
		xReturn = xDiskStatus;
	}

	return xReturn;
}

/*-----------------------------------------------------------------------*/
/* Read Sector(s)                                                        */
/*-----------------------------------------------------------------------*/
DRESULT disk_read
		(
			BYTE	cDriveNumber,	/* Physical drive nmuber (0) */
			BYTE	*pcBuffer,		/* Pointer to the data buffer to store read data */
			DWORD	ulSector, 		/* Start ulSector number (LBA) */
			BYTE	xCount			/* Sector xCount (1..255) */
		)
{
DRESULT xReturn;

	if( ( cDriveNumber != 0 ) || ( xCount == 0 ) )
	{
		xReturn = RES_PARERR;
	}
	else if( ( ( xDiskStatus & STA_NOINIT ) != 0 ) || ( prvIsDiskInserted() != pdTRUE ) )
	{
		xReturn = RES_NOTRDY;
	}
	else
	{
		if( ( ucInsertedCardType & CT_BLOCK ) == 0U )
		{
			/* Convert to byte address if needed */
			ulSector *= mmcSECTOR_SIZE;
		}

		if( xCount == 1 )
		{
			/* Single block read */
			if( prvSendCommand( mmcCMD17_READ_SINGLE_BLOCK, ulSector ) == 0 )
			{
				if( prvReceiveDataBlock( pcBuffer, mmcSECTOR_SIZE ) == true )
				{
					xCount = 0;
				}
			}
		}
		else
		{
			/* Multiple block read */
			if( prvSendCommand( mmcCMD18_READ_MULTI_BLOCK, ulSector ) == 0 )
			{
				do
				{
					if( prvReceiveDataBlock( pcBuffer, mmcSECTOR_SIZE ) == false )
					{
						break;
					}

					pcBuffer += mmcSECTOR_SIZE;
				} while( --xCount );

				prvSendCommand( mmcCMD12_STOP, 0 );
			}
		}

		prvDeselectCard();

		if( xCount > 0 )
		{
			xReturn = RES_ERROR;
		}
		else
		{
			xReturn = RES_OK;
		}
	}

	return xReturn;
}



/*-----------------------------------------------------------------------*/
/* Write Sector(s)                                                       */
/*-----------------------------------------------------------------------*/
#if _READONLY == 0
DRESULT disk_write
		(
			BYTE		cDriveNumber,	/* Physical drive nmuber (0) */
			const BYTE	*pcBuffer,		/* Pointer to the data to be written */
			DWORD		ulSector, 		/* Start ulSector number (LBA) */
			BYTE		xCount			/* Sector xCount (1..255) */
		)
{
DRESULT xReturn;

	if( ( cDriveNumber != 0 ) || ( xCount == 0 ) )
	{
		xReturn = RES_PARERR;
	}
	else if( ( ( xDiskStatus & STA_NOINIT ) != 0 ) || ( prvIsDiskInserted() != pdTRUE ) )
	{
		xReturn = RES_NOTRDY;
	}
	else if( ( xDiskStatus & STA_PROTECT ) != 0 )
	{
		xReturn = RES_WRPRT;
	}
	else
	{
		if( ( ucInsertedCardType & CT_BLOCK ) == 0 )
		{
			/* Convert to byte address if needed */
			ulSector *= mmcSECTOR_SIZE;
		}

		if( xCount == 1 )
		{
			/* Single block write */
			if( prvSendCommand( mmcCMD24_WRITE_SINGLE_BLOCK, ulSector ) == 0 )
			{
				if( prvWriteDataBlock( pcBuffer, 0xFE ) == true )
				{
					xCount = 0;
				}
			}
		}
		else
		{
			/* Multiple block write */
			if( ucInsertedCardType & CT_SDC )
			{
				prvSendCommand( mmcACMD23_SET_ERASE_COUNT, xCount );
			}

			if( prvSendCommand( mmcCMD25_WRITE_MULTI_BLOCK, ulSector ) == 0 )
			{
				do
				{
					if( prvWriteDataBlock( pcBuffer, 0xFC ) != true )
					{
						break;
					}

					pcBuffer += mmcSECTOR_SIZE;

				} while( --xCount );

				if( prvWriteDataBlock( NULL, 0xFD ) != true )
				{
					/* STOP_TRAN token */
					xCount = 1;
				}
			}
		}

		prvDeselectCard();

		if( xCount > 0 )
		{
			xReturn = RES_ERROR;
		}
		else
		{
			xReturn = RES_OK;
		}
	}

	return xReturn;
}
#endif /* _READONLY == 0 */



/*-----------------------------------------------------------------------*/
/* Miscellaneous Functions                                               */
/*-----------------------------------------------------------------------*/

#if _USE_IOCTL != 0
DRESULT disk_ioctl
		(
			BYTE	cDriveNumber,	/* Physical drive nmuber (0) */
			BYTE	cControlCode,	/* Control code */
			void	*pvBuffer		/* Buffer to send/receive control data */
		)
{
DRESULT xResult = RES_ERROR;
BYTE	n, pcTempBuffer[ mmcMAX_QUERY_STRING_BYTES ], *pcExternalBuffer = (BYTE*)pvBuffer;
WORD	usSize;

	/* Only one drive is supported. */
	configASSERT( cDriveNumber == 0 );

	/* Remove compiler warnings when configASSERT() is not defined. */
	( void ) cDriveNumber;

	if( ( ( xDiskStatus & STA_NOINIT ) != 0 ) || ( prvIsDiskInserted() != pdTRUE ) )
	{
		xResult = RES_NOTRDY;
	}
	else
	{
		switch( cControlCode )
		{
			case CTRL_SYNC:

				/* Make sure that no pending write process. Do not remove
				this or written ulSector might not left updated. */
				if( prvSelectCard() == true )
				{
					xResult = RES_OK;
					prvDeselectCard();
				}
				break;


			case GET_SECTOR_COUNT:

				/* Get number of sectors on the disk (DWORD) */
				if( prvQueryCard( mmcCMD9_READ_CSD_REGISTER, 0, pcTempBuffer, 16 ) == true )
				{
					if( ( pcTempBuffer[ 0 ] >> 6 ) == 1 )
					{
						/* SDC ver 2.00 */
						usSize = pcTempBuffer[ 9 ] + ( ( ( WORD ) pcTempBuffer[ 8 ] << 8 ) + 1 );
						*( DWORD * ) pvBuffer = ( DWORD ) usSize << 10UL;
					}
					else
					{
						/* SDC ver 1.XX or MMC */
						n = ( pcTempBuffer[ 5 ] & 15 ) + ( (pcTempBuffer[ 10 ] & 128 ) >> 7 ) + ( (pcTempBuffer[ 9 ] & 3 ) << 1 ) + 2;
						usSize = ( pcTempBuffer[ 8 ] >> 6 ) + ( ( WORD ) pcTempBuffer[ 7 ] << 2 ) + ( ( WORD ) ( pcTempBuffer[ 6 ] & 3 ) << 10 ) + 1;
						*( DWORD * ) pvBuffer = ( DWORD ) usSize << ( n - 9 );
					}

					xResult = RES_OK;
				}
				break;


			case GET_SECTOR_SIZE:

				/* Get R/W ulSector size (WORD) */
				* ( WORD * ) pvBuffer = mmcSECTOR_SIZE;
				xResult = RES_OK;
				break;


			case GET_BLOCK_SIZE:

				/* Get erase block size in unit of ulSector (DWORD) */
				if( ( ucInsertedCardType & CT_SD2 ) != 0 )
				{
					/* SDC ver 2.00 */
					if( prvQueryCard( mmcACMD13_READ_SD_STATUS, 0, pcTempBuffer, mmcMAX_QUERY_STRING_BYTES ) == true )
					{
						* ( DWORD * ) pvBuffer = 16UL << ( pcTempBuffer[ 10 ] >> 4 );
						xResult = RES_OK;

						/* Only 16 of the 64 bytes have been read.  Purge
						the remaining bytes.  Read bytes 16 to 32. */
						spiRead(pcTempBuffer, mmcMAX_QUERY_STRING_BYTES);

						/* Read bytes 32 to 48. */
						spiRead(pcTempBuffer, mmcMAX_QUERY_STRING_BYTES);

						/* Read bytes 48 to 64. */
						spiRead(pcTempBuffer, mmcMAX_QUERY_STRING_BYTES);
					}
				}
				else
				{
					/* SDC ver 1.XX or MMC */
					if( prvQueryCard( mmcCMD9_READ_CSD_REGISTER, 0, pcTempBuffer, 16 ) == true )
					{
						/* Read CSD */
						if( ( ucInsertedCardType & CT_SD1 ) != 0 )
						{
							/* SDC ver 1.XX */
							*( DWORD * ) pvBuffer = ( ( ( pcTempBuffer[ 10 ] & 63 ) << 1 ) + ( ( WORD ) ( pcTempBuffer[ 11 ] & 128 ) >> 7 ) + 1 ) << ( ( pcTempBuffer[ 13 ] >> 6 ) - 1 );
						}
						else
						{
							/* MMC */
							*( DWORD * ) pvBuffer = ( ( WORD ) ( ( pcTempBuffer[ 10 ] & 124 ) >> 2 ) + 1 ) * ( ( ( pcTempBuffer[ 11 ] & 3 ) << 3 ) + ( ( pcTempBuffer[ 11 ] & 224 ) >> 5 ) + 1 );
						}

						xResult = RES_OK;
					}
				}
				break;


			case MMC_GET_TYPE:

				/* Get card type flags (1 byte) */
				*pcExternalBuffer = ucInsertedCardType;
				xResult = RES_OK;
				break;


			case MMC_GET_CSD:

				/* Receive CSD as a data block (16 bytes) */
				if( prvQueryCard( mmcCMD9_READ_CSD_REGISTER, 0, pcExternalBuffer, 16 ) == true )
				{
					xResult = RES_OK;
				}
				break;


			case MMC_GET_CID:

				/* Receive CID as a data block (16 bytes) */
				if( prvQueryCard( mmcCMD10_READ_CID_REGISTER, 0, pcExternalBuffer, 16 ) == true )
				{
					xResult = RES_OK;
				}
				break;


			case MMC_GET_OCR:

				/* Receive OCR as an R3 resp (4 bytes) */
				if( prvSendCommand( mmcCMD58_READ_OCR, 0 ) == 0 )
				{
					/* READ_OCR */
					spiRead(pcExternalBuffer, mmcOCR_LENGTH);

					xResult = RES_OK;
				}
				break;


			case MMC_GET_SDSTAT:

				/* Receive SD status as a data block (64 bytes) */
				if( prvQueryCard( mmcACMD13_READ_SD_STATUS, 0, pcExternalBuffer, 64 ) == true )
				{
					xResult = RES_OK;
				}

				break;


			default:

				xResult = RES_PARERR;
				break;
		}
	}

	prvDeselectCard();

	return xResult;
}
#endif /* _USE_IOCTL != 0 */
/*-----------------------------------------------------------*/

static portBASE_TYPE prvIsDiskInserted( void )
{
	portBASE_TYPE xReturn;

	xDiskStatus &= ~STA_NODISK;
	xReturn = pdTRUE;

	return xReturn;
}
/*-----------------------------------------------------------*/

static BYTE prvSendCommand( BYTE cCommand,	DWORD xArgument	)
{
BYTE cResult = 0, n;
BYTE cCommandString[ mmcCOMMAND_LENGTH_BYTES ];

	if( ( cCommand & 0x80 ) != 0 )
	{
		/* ACMD<n> is the command sequence of mmcCMD55_LEADING_COMMAND_OF_ACMD-CMD<n> */
		cCommand &= 0x7F;
		cResult = prvSendCommand( mmcCMD55_LEADING_COMMAND_OF_ACMD, 0 );
	}

	if( cResult <= 1 )
	{
		cResult = 0xff;

		/* Select the card and wait for ready */
		prvDeselectCard();
		if( prvSelectCard() == true )
		{
			/* Send command packet */
			cCommandString[ 0 ] = cCommand;
			cCommandString[ 1 ] = ( BYTE ) ( ( xArgument >> 24UL ) & 0xffUL );
			cCommandString[ 2 ] = ( BYTE ) ( ( xArgument >> 16UL ) & 0xffUL );
			cCommandString[ 3 ] = ( BYTE ) ( ( xArgument >> 8UL ) & 0xffUL );
			cCommandString[ 4 ] = ( BYTE ) ( xArgument & 0xffUL );

			if( cCommand == mmcCMD0_SOFTWARE_RESET )
			{
				/* Valid CRC for mmcCMD0_SOFTWARE_RESET(0) */
				cCommandString[ 5 ] = 0x95;
			}
			else if( cCommand == mmcCMD8_CHECK_VOLTAGE_RANGE )
			{
				/* Valid CRC for mmcCMD8_CHECK_VOLTAGE_RANGE(0x1AA) */
				cCommandString[ 5 ] = 0x87;
			}
			else
			{
				/* Dummy CRC and stop. */
				cCommandString[ 5 ] = 0x01;
			}

			if (spiWrite(cCommandString, mmcCOMMAND_LENGTH_BYTES) == mmcCOMMAND_LENGTH_BYTES)
			{
				/* Receive command response */
				if( cCommand == mmcCMD12_STOP )
				{
					/* Skip a stuff byte when stop reading */
					spiRead(&cResult, sizeof(cResult));
				}

				/* Wait for a valid response. */
				for( n = 0; n < 10; n++ )
				{
					if(spiRead(&cResult, sizeof(cResult)) == sizeof(cResult))
					{
						if( ( cResult & 0x80 ) == 0 )
						{
							break;
						}
					}
				}
			}
		}
	}

	return cResult;
}
/*-----------------------------------------------------------*/

static BYTE prvWaitForCardReady (void)
{
portTickType xTimeOnEntering;
const portTickType xMaxTimeToWait_ms = 1000 / portTICK_RATE_MS;
BYTE cDummy;

	xTimeOnEntering = xTaskGetTickCount();

	spiRead(&cDummy, sizeof(cDummy));
	do
	{
		spiRead(&cDummy, sizeof(cDummy));
	}
	while( ( cDummy != 0xFF ) && ( ( xTaskGetTickCount() - xTimeOnEntering ) < xMaxTimeToWait_ms ) );

	return cDummy;
}
/*-----------------------------------------------------------*/

static void prvDeselectCard( void )
{
	uint8_t cDummy;

	SD_CS_bb = SD_CS_DEASSERTED;
	spiRead(&cDummy, sizeof(cDummy));
}
/*-----------------------------------------------------------*/

static bool prvSelectCard( void )
{
bool xReturn = true;

	SD_CS_bb = SD_CS_ASSERTED;
	if( prvWaitForCardReady() != 0xFF )
	{
		prvDeselectCard();
		xReturn = false;
	}

	return xReturn;
}
/*-----------------------------------------------------------*/

static bool prvQueryCard( BYTE ucCommand, DWORD ulCommandArgument, BYTE *pucRxBuffer, UINT uiRxLength )
{
bool xReturn = false;
portTickType xTimeAtStart;
const portTickType xTimeOut = 200 / portTICK_RATE_MS;
uint8_t cDummy[ 2 ];
BYTE cTimedOut = ( BYTE ) pdFALSE;

	/* The length must be a multiple of 4. */
	configASSERT( ( uiRxLength % 4 ) == 0U );
	configASSERT( xSPIPort );

	if( prvSendCommand( ucCommand, ulCommandArgument ) <= 1 )
	{
		xTimeAtStart = xTaskGetTickCount();

		do
		{
			spiRead(pucRxBuffer, sizeof(*pucRxBuffer));

			if( ( xTaskGetTickCount() - xTimeAtStart ) > xTimeOut )
			{
				cTimedOut = ( BYTE ) pdTRUE;
			}

		} while( ( ( *pucRxBuffer ) != 0xfeU ) && ( cTimedOut == pdFALSE ) );

		if( cTimedOut != pdTRUE )
		{
			/* Read data. */
			if(spiRead(pucRxBuffer, uiRxLength) == uiRxLength )
			{
				/* Read and discard CRC. */
				spiRead(cDummy, sizeof(cDummy));
				xReturn = true;
			}
		}
	}

	return xReturn;
}
/*-----------------------------------------------------------*/

static bool prvReceiveDataBlock( BYTE *pcBuffer, UINT xBytesToRead )
{
bool xReturn = false;
portTickType xTimeOnEntering;
BYTE pcToken[ 2 ] = { 0x00, 0x00 };
const portTickType xMaxTimeToWait_ms = 200 / portTICK_RATE_MS;

	xTimeOnEntering = xTaskGetTickCount();

	/* Wait with timeout for a valid byte. */
	do
	{
		spiRead(pcToken, sizeof(*pcToken));
	} while( ( *pcToken == 0xff ) && ( ( xTaskGetTickCount() - xTimeOnEntering ) < xMaxTimeToWait_ms ) );

	/* Was the byte as expected? */
	if( *pcToken == 0xfe )
	{
		/* Receive the data block. */
		if(spiRead(pcBuffer, xBytesToRead) == xBytesToRead )
		{
			/* Read and discard the CRC. */
			spiRead(pcToken, sizeof(pcToken));
			xReturn = true;
		}
	}

	return xReturn;
}
/*-----------------------------------------------------------*/

#if _READONLY == 0
static bool prvWriteDataBlock( const BYTE *pcBuffer, BYTE cToken )
{
/* Two dummy CRC bytes, and one space to receive the response. */
const BYTE cCRCDummy[ 2 ] = { 0xff, 0xff };
volatile bool bReturn = false;

	if( prvWaitForCardReady() == 0xff )
		/* Transmit the token. */
		if( spiWrite(&cToken, sizeof( cToken ) ) == 1 )
		{
			if( cToken != 0xFD )
			{
				/* Write the data block. */
				if( spiWrite(pcBuffer, mmcDATA_BLOCK_SIZE ) == mmcDATA_BLOCK_SIZE )
					/* Write the CRC. */
					if( spiWrite(cCRCDummy, sizeof( cCRCDummy ) ) == sizeof( cCRCDummy ) )
						/* Receive response. */
						if(spiRead(&cToken, sizeof(cToken)) == sizeof(cToken))
							if( ( cToken & 0x1f ) == 0x05 )
								bReturn = true;
			}
			else
				bReturn = true;
		}

	configASSERT( bReturn );
	return bReturn;
}
#endif /* _READONLY */
/*-----------------------------------------------------------*/

DWORD get_fattime(void)
{
	return 0;								// no time is implemented
}
