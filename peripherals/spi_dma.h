#ifndef SPI_DMA_H_
#define SPI_DMA_H_

void spiDmaInitialize(void);

uint32_t spiDmaSetBaudRate(uint32_t baud_rate);

void spiDmaStart();

void spiDmaStop();

void spiDmaSend(uint8_t *tx, uint32_t length);

void spiDmaSendDummy(uint8_t *dummy, uint32_t length);

void spiDmaRead(uint8_t *rx, uint32_t length);

#endif /* SPI_DMA_H_ */
