/*
 * SD_test.cpp
 *
 *  Created on: 12 lut 2015
 *      Author: Adrian
 */

#include <cstdint>
#include <cstddef>
#include "SD_test.h"
#include "SD.h"

/*---------------------------------------------------------------------------------------------------------------------+
| global variables
+---------------------------------------------------------------------------------------------------------------------*/



/*---------------------------------------------------------------------------------------------------------------------+
| private function implementations
+---------------------------------------------------------------------------------------------------------------------*/

static void Fill_Buffer(uint8_t *pBuffer, uint16_t BufferLenght, uint8_t Offset)
{
	uint16_t IndexTmp;

	/* Put in global buffer same values */
	for( IndexTmp = 0; IndexTmp < BufferLenght; IndexTmp++ )
	{
		*pBuffer++ =IndexTmp + Offset;
	}
}

static TestStatus Buffercmp(uint8_t* pBuffer1, uint8_t* pBuffer2, uint16_t BufferLength)
{
	while (BufferLength--)
	{
		if (*pBuffer1 != *pBuffer2)
		{
			return FAILED;
	    }

	    pBuffer1++;
	    pBuffer2++;
	}
	return PASSED;
}

uint8_t SD_test(uint8_t* pBuffer1, uint8_t* pBuffer2, uint16_t BufferLength)
{
	TestStatus TransferStatus1;
	Fill_Buffer(pBuffer1, BufferLength, 0x0);
	SD_WriteBlock(pBuffer1, 0, BufferLength);
	SD_ReadBlock(pBuffer2, 0, BufferLength);
	TransferStatus1 = Buffercmp(pBuffer1, pBuffer2, BufferLength);
	if (TransferStatus1 == PASSED)
	{
		return 0;
	}
	else
	{
	    return 1;
	}
}


