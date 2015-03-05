#ifndef SPI_DMA_RTOS_H_
#define SPI_DMA_RTOS_H_

#define SPI_DIRECTION_READ				1
#define SPI_DIRECTION_SEND				2

void spiDmaInitialize(void);

uint32_t spiDmaSetBaudRate(uint32_t baud_rate);

void spiDmaStart();

void spiDmaStop();

void spiDmaSend(uint8_t *tx, uint32_t length, portTickType ticks_to_wait);

void spiDmaSendDummy(uint8_t *dummy, uint32_t length);

void spiDmaRead2Buffer(uint32_t length, portTickType ticks_to_wait);

struct spiRxMessage spiDmaReadFromBuffer();

#endif /* SPI_DMA_RTOS_H_ */
