/**
 * \file spi.h
 * \brief Header for spi.cpp
 * \author grochu
 * \date 2012-08-30
 */

#ifndef SPI_H_
#define SPI_H_

#include "portmacro.h"
#include "error.h"

/*---------------------------------------------------------------------------------------------------------------------+
 | global variables' types
 +---------------------------------------------------------------------------------------------------------------------*/

/// message for SPI TX queue
struct _spiTxMessage
{
	size_t length;			///< number of uint8_t to send
	uint8_t *tx;			///< pointer to table of uints, either in flash or in dynamic buffer
};

/*---------------------------------------------------------------------------------------------------------------------+
| global defines
+---------------------------------------------------------------------------------------------------------------------*/

#define spiRead(rx, length)					spiTransfer(nullptr, (rx), (length))
#define spiWrite(tx, length)				spiTransfer((tx), nullptr, (length))

/*---------------------------------------------------------------------------------------------------------------------+
| global functions' prototypes
+---------------------------------------------------------------------------------------------------------------------*/

void spiInitialize(void);

uint32_t spiSetBaudRate(uint32_t baud_rate);

size_t spiTransfer(const uint8_t *tx, uint8_t *rx, size_t length);

void spiSend(_spiTxMessage *message, portTickType ticks_to_wait);

enum Error _initializeSpiDmaTask(void);

enum Error _initializeSpiDmaTestTask(void);

#endif /* SPI_H_ */
