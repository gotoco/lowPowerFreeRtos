/**
 * \file i2c.h
 * \brief Header for i2c.cpp
 * \author: Mazeryt Freager
 * \date 2014-07-20
 */

#ifndef I2C_H_
#define I2C_H_

#include <cstdint>
#include <cstddef>

/*---------------------------------------------------------------------------------------------------------------------+
| global functions' prototypes
+---------------------------------------------------------------------------------------------------------------------*/

void i2cInitialize(void);
uint8_t* i2cRead(uint8_t address, uint8_t *data, size_t length);
void i2cWrite(uint8_t address, const uint8_t *data, size_t length);
void i2cWriteOneByteWhithoutStop(uint8_t address, uint8_t data);
void i2cReadOneByte(uint8_t address, uint8_t *data);

#endif /* I2C_H_ */
