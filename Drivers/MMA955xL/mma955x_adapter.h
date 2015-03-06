#ifndef ACC_ADAPTER_H_
#define ACC_ADAPTER_H_

#include "portmacro.h"

#include "mma955x_drv.h"

#define __MMA955x_VIA_SPI		0
#define __MMA955x_VIA_I2C		1

#define __MMA955x_COMM_USES		__MMA955x_VIA_SPI

/**
 * \brief Initializes the accelerometer driver and device-related structures and peripherals.
 */
void mma955x_low_level_init(void);

/**
 * \brief Portable delay function.
 * \param [in] Delay time in milliseconds.
 */
void mma955x_delay(portTickType delay_in_ms);

/**
 * \brief Portable semaphore-based function. Returns when the data-ready interrupt is asserted
 * or when the waiting time runs out.
 * \param [in] Maximum waiting time for the interrupt in milliseconds.
 * \return Information whether interrupted was asserted in the desired time window.
 */
signed portBASE_TYPE mma955x_waitForData(portTickType timeout_in_ms);

/**
 * \brief Configures the data-ready interrupt in the MCU.
 */
void _mma955x_configure_interrupt(void);

void mma955x_mutex_lock(void);

void mma955x_mutex_unlock(void);

/**
 * \brief Wait for COmmand COmplete
 */
void mma955x_wait_for_coco(void);

int mma955x_read_data(uint8_t address, uint8_t *data, uint8_t size);

int mma955x_write_data(uint8_t address, uint8_t *data, uint8_t size);

#endif
