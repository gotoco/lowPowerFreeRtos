/**
 * \file spi.h
 * \brief Header for spi.cpp
 * \author grochu
 * \date 2012-08-30
 */

#ifndef SPI_H_
#define SPI_H_

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

#endif /* SPI_H_ */
