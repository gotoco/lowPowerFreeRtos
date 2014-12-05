/**
 * \file i2c.cpp
 * \brief I2C driver
 *
 * Functions for I2C control
 *
 * chip: STM32L1xx; prefix: i2c
 *
 * \author Freddie Chopin, http://www.freddiechopin.info http://www.distortec.com
 * \date 2012-09-06
 */

#include <stdint.h>

#include "inc/stm32l1xx.h"

#include "hdr/hdr_rcc.h"
#include "hdr/hdr_i2c.h"

#include "config.h"

#include "i2c.h"
#include "gpio.h"
#include "rcc.h"

/*---------------------------------------------------------------------------------------------------------------------+
| global functions
+---------------------------------------------------------------------------------------------------------------------*/

/**
 * \brief Initializes I2C module
 *
 * Configures I/Os of I2C, enable clock for I2C module and configures it.
 */

void i2cInitialize(void)
{
	gpioConfigurePin(I2Cx_SCL_GPIO, I2Cx_SCL_PIN, I2Cx_SCL_CONFIGURATION);
	gpioConfigurePin(I2Cx_SDA_GPIO, I2Cx_SDA_PIN, I2Cx_SDA_CONFIGURATION);

	RCC_APBxENR_I2CxEN_bb = 1;				// enable clock for I2C module

	uint32_t frequency = rccGetCoreFrequency();

	I2Cx_CR1_SWRST_bb(I2Cx) = 1;			// force software reset of I2C peripheral
	I2Cx_CR1_SWRST_bb(I2Cx) = 0;

	I2Cx->TRISE = frequency / 1000000 + 1;	// limit slope (standard mode)
	I2Cx->CCR = frequency / I2C_FREQUENCY / 2;	// setup clock
	I2Cx->CR2 = (frequency / 1000000) << I2C_CR2_FREQ_bit;	// config I2C module's frequency
	I2Cx_CR1_PE_bb(I2Cx) = 1;				// enable peripheral
}

/**
 * \brief Reads a block of data from I2C.
 *
 * Reads a block of data from I2C.
 *
 * \param [in] address is the address of the slave chip
 * \param [in] data points to buffer for data that will be read
 * \param [in] length is the length of the data block to be received
 *
 * \return pointer to received data block (data)
 */
uint8_t* i2cRead(uint8_t address, uint8_t *data, size_t length)
{
	uint8_t *data_copy = data;

	I2Cx_CR1_START_bb(I2Cx) = 1;			// request a start
	while (I2Cx_SR1_SB_bb(I2Cx) == 0);		// wait for start to finish
	I2Cx->SR1;								// read of SR1 clears the flag
	address = address << 1;
	I2Cx->DR = address | 1;					// transfer address (LSB set - read)
	while (I2Cx_SR1_ADDR_bb(I2Cx) == 0);	// wait for address transfer
	I2Cx->SR1;								// clear the flag
	I2Cx->SR2;

	while (length--)
	{
		I2Cx_CR1_ACK_bb(I2Cx) = (length == 0 ? 0 : 1);	// acknowledge all but last byte

		while (I2Cx_SR1_RxNE_bb(I2Cx) == 0);	// wait for DR not-empty
		*data++ = I2Cx->DR;
	}

	I2Cx_CR1_STOP_bb(I2Cx) = 1;				// request a stop

	return data_copy;
}

/**
 * \brief Transfers a block of data through I2C.
 *
 * Transfers a block of data through I2C.
 *
 * \param [in] address is the address of the slave chip
 * \param [in] data points to data block that will be sent
 * \param [in] length is the length of the data block
 */

void i2cWrite(uint8_t address, const uint8_t *data, size_t length)
{
	I2Cx_CR1_START_bb(I2Cx) = 1;			// request a start
	while (I2Cx_SR1_SB_bb(I2Cx) == 0);		// wait for start to finish
	I2Cx->SR1;								// read of SR1 clears the flag
	address = address << 1;
	I2Cx->DR = (address & ~1);				// transfer address (LSB cleared - write)
	while (I2Cx_SR1_ADDR_bb(I2Cx) == 0);	// wait for address transfer
	I2Cx->SR1;								// clear the flag
	I2Cx->SR2;

	while (length--)
	{
		while (I2Cx_SR1_TxE_bb(I2Cx) == 0);	// wait for DR empty
		I2Cx->DR = *data++;
	}

	while (I2Cx_SR1_TxE_bb(I2Cx) == 0 || I2Cx_SR1_BTF_bb(I2Cx) == 1);	// wait for bus not-busy
	I2Cx_CR1_STOP_bb(I2Cx) = 1;				// request a stop
}

/**
 * \brief Transfers one byte of data through I2C without sending stop signal.
 *
 * Transfers one byte of data through I2C without sending stop signal.
 *
 * \param [in] address is the address of the slave chip
 * \param [in] byte to send
 */
void i2cWriteOneByteWhithoutStop(uint8_t address, uint8_t data)
{
	I2Cx_CR1_START_bb(I2Cx) = 1;			// request a start
	while (I2Cx_SR1_SB_bb(I2Cx) == 0);		// wait for start to finish
	I2Cx->SR1;								// read of SR1 clears the flag
	address = address << 1;
	I2Cx->DR = (address & ~1);				// transfer address (LSB cleared - write)
	while (I2Cx_SR1_ADDR_bb(I2Cx) == 0);	// wait for address transfer
	I2Cx->SR1;								// clear the flag
	I2Cx->SR2;

	while (I2Cx_SR1_TxE_bb(I2Cx) == 0);	// wait for DR empty
	I2Cx->DR = data;

	while (I2Cx_SR1_TxE_bb(I2Cx) == 0 || I2Cx_SR1_BTF_bb(I2Cx) == 1);	// wait for bus not-busy
}







