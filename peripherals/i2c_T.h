/*
 * i2c_T.h
 *\brief I2C driver function declaration based on asynchronous task
 *
 *  Created on: 1 mar 2015
 *      Author: pgielmuda
 */
#ifdef _PG_DRV
#ifndef PERIPHERALS_I2C_BY_PG_H_
#define PERIPHERALS_I2C_BY_PG_H_
#include "FreeRTOS.h"

#include "error.h"

/*---------------------------------------------------------------------------------------------------------------------+
| global functions' prototypes
+---------------------------------------------------------------------------------------------------------------------*/
/**
 * \brief Initializes I2C module
 *
 * Configures I/Os of I2C, enable clock for I2C module and configures it.
 * Also starts a task, initialize RX/TX queue and Semaphore
 */
enum Error i2cInitializeT(void);
/**
 * \brief Adds one message to send by I2C .
 *
 * Adds one message to send by I2C. Calling task should have read-only access to shared_data section.
 *
 * \param [in] address of I2C slave
 * \param [in] string is the pointer to zero terminated string
 * \param [in] lenght is a lenght of string without '\0' termination
 * \param [in] ticks_to_wait is the amount of time the call should block while waiting for the operation to finish, use
 * portMAX_DELAY to suspend
 *
 * \return ERROR_NONE on success, otherwise an error code defined in the file error.h
 */

enum Error i2cWriteToSlaveT(uint16_t addr, const char *string, uint8_t lenght, portTickType ticks_to_wait);
/**
 * \brief Adds one request for reading from I2C slave.
 *
 * Adds one one request for reading from I2C slave.
 *
 * \param [in] address of I2C slave
 * \param [out] string is the pointer to readed data string, calling function should know how many bytes want to read from slave and should give a pointer to a memory for readed data.
 * \param [in] lenght is a lenght of string without '\0' termination
 * \param [in] ticks_to_wait is the amount of time the call should block while waiting for the operation to finish, use
 * portMAX_DELAY to suspend
 *
 * \return ERROR_NONE on success, otherwise an error code defined in the file error.h
 */

enum Error i2cReadFromSlaveT(uint16_t addr, char *string, uint8_t lenght, portTickType ticks_to_wait);


#endif /* PERIPHERALS_I2C_BY_PG_H_ */
#endif /* _PG_DRV */
