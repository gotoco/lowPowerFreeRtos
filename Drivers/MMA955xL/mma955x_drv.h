#ifndef ACC_DRV_H_
#define ACC_DRV_H_

/**
 * \brief Driver for ACC - MMA955xL
 *
 * Complete driver for accelerometer which support a Mailbox communication via SPI.
 *
 * \author DuMaM
 * \date 04.08.2014
 * \version 0.1
 * \copyright uBirds
 * \warning Undone.
 */

#include <stdint.h>

#include "mma955x_def.h"

struct acc_AFE_config_t {
	acc_AFE_Full_Scale_t scale;
	acc_AFE_ADC_Resolution_t resolution;
	acc_AFE_SFD_Rate_t SFD_rate;
	acc_AFE_HPF_Cut_off_frequency_t HPF_Cut_off_frequency;
	acc_AFE_LPF_Cut_off_frequency_t LPF_Cut_off_frequency;
	acc_AFE_LPF_Cut_off_frequency_t temp_LPF_Cut_off_frequency;

	acc_Task_Activity_t AFE_task_activity;

	int16_t offset[3];
};

struct acc_Pedometer_config_t {
	uint16_t sleep_minimum;
	uint16_t sleep_maximum;
	uint16_t sleep_count_threshold;

	uint8_t step_length;
	uint8_t height;
	uint8_t weight;
	uint8_t filter_step_count;
	Gender_t gender;
	uint8_t filter_time;
	uint8_t speed_period;
	uint8_t step_coalescence;

	uint16_t activity_count_threshold;
};

struct acc_t
{
	/* private data */
	const char * driver_name;

	acc_AFE_config_t AFE_config;
	acc_AFE_config_t normal_mode_AFE_config, low_power_mode_AFE_config;
	acc_Pedometer_config_t pedometer_config;
	acc_Power_Mode_t power_mode;

	uint8_t buffer[16];

	/* public data */
	uint16_t dt;
	int16_t acceleration[3];
	int16_t LPFacceleration[3];
	int16_t HPFacceleration[3];

	acc_Activity_t activity;
	uint16_t stepCount;
	uint16_t distance;
	uint16_t speed;
	uint16_t calories;

	/**
	 * \brief (Re)initialize the accelerometer driver.
	 * Resets the device and configures it based on the configuration stored in the driver structure.
	 * \param [in] Pointer to the driver structure.
	 */
	void (* acc_init)(struct acc_t *self);

	/**
	 * \brief Reads firmware and hardware versions of the device.
	 * \param [in] Pointer to the driver structure.
	 * \param [out] Pointer to the buffer to which the data will be written (minimum 12 bytes).
	 * \return Communication error code.
	 * After a successful attempt, the buffer contains the following information:
	 * Bytes 1-4	Device Identifier
	 * Bytes 5-6	ROM version
	 * Bytes 7-8	Firmware version
	 * Bytes 9-10	Hardware version
	 * Bytes 11-12	Build version
	 */
	acc_Error_t (* acc_checkVersion)(struct acc_t *self, uint8_t *buffer);

	/**
	 * \brief Updates low-pass and high-pass filtered acceleration data stored in the driver structure.
	 * \param [in] Pointer to the driver structure.
	 */
	void (* acc_updateLPFHPFAcceleration)(struct acc_t *self);

	/**
	 * \brief Updates acceleration data stored in the driver structure.
	 * Only specified dataset (low-pass/high-pass/unfiltered) is downloaded.
	 * \param [in] Pointer to the driver structure.
	 * \param [in] Offset of the data register in the AFE application.
	 */
	void (* acc_getAcceleration)(struct acc_t *self, uint8_t AFE_offset);

	/**
	 * \brief Updates pedometer data (activity, step count, distance, speed, calories) stored in the driver structure.
	 * \param [in] Pointer to the driver structure.
	 */
	void (* acc_updatePedometerData)(struct acc_t *self);

		/**
	 * \brief Configures the device with the low power mode configuration stored in the driver structure.
	 * \param [in] Pointer to the driver structure.
	 */
	void (* acc_lowPowerModeConfiguration)(struct acc_t *self);

	/**
	 * \brief Configures the device with the normal mode configuration stored in the driver structure.
	 * \param [in] Pointer to the driver structure.
	 */
	void (* acc_normalModeConfiguration)(struct acc_t *self);

	/**
	 * \brief Restores default values to the configuration sets stored in the driver structure.
	 * \param [in] Pointer to the driver structure.
	 */
	void (* acc_defaultConfiguration)(struct acc_t *self);

	/**
	 * \brief Modifies the offset values saved in the driver's structure and in the device's configuration registers.
	 * Values can provided manually or can be estimated through a calibration process.
	 * In order for the device to be calibrated successfully, it should lay flat on a stable surface,
	 * and the accelerometer chip Z axis should be pointing upwards.
	 * The calibration process requires the device to stay motionless for about a second.
	 * \param [in] Pointer to the driver structure.
	 * \param [in] X axis offset value (ignored if calibrate is not 0).
	 * \param [in] Y axis offset value (ignored if calibrate is not 0).
	 * \param [in] Z axis offset value (ignored if calibrate is not 0).
	 * \param [in] Decides whether calibration will be performed.
	 */
	void (* acc_setOffset)(struct acc_t *self, uint16_t X, uint16_t Y, uint16_t Z, uint8_t calibrate);

	/**
	 * \brief Configures the Analog Front-End application using provided config structure.
	 * \param [in] Pointer to the driver structure.
	 * \param [in] Pointer to the AFE configuration structure.
	 */
	void (* acc_configAFE)(struct acc_t *self, struct acc_AFE_config_t *config);

	/**
	 * \brief Configures the pedometer application using provided config structure.
	 * \param [in] Pointer to the driver structure.
	 * \param [in] Pointer to the pedometer configuration structure.
	 */
	void (* acc_configPedometer)(struct acc_t *self, struct acc_Pedometer_config_t *config);

	/**
	 * \brief Resets an application.
	 * \param [in] Pointer to the driver structure.
	 * \param [in] ID of the application to be reset.
	 */
	void (* acc_appReset)(struct acc_t *self, acc_App_ID_t id);

	/**
	 * \brief Suspends an application.
	 * \param [in] Pointer to the driver structure.
	 * \param [in] ID of the application to be suspended.
	 */
	void (* acc_appSuspend)(struct acc_t *self, acc_App_ID_t id);

	/**
	 * \brief Unsuspends an application.
	 * \param [in] Pointer to the driver structure.
	 * \param [in] ID of the application to be unsuspended.
	 */
	void (* acc_appUnsuspend)(struct acc_t *self, acc_App_ID_t id);

	/**
	 * \brief Wakes up the device.
	 * \param [in] Pointer to the driver structure.
	 */
	void (* acc_wakeUp)(struct acc_t *self);

	/**
	 * \brief Configures the specified application's priority and activity settings in the scheduler application.
	 * The activity setting can be:
	 * 0x03	- application will be executed during high and low activity
	 * 0x02	- application will be executed only during high activity
	 * 0x01	- application will be executed only during low activity
	 * 0x00	- application will never be executed
	 * The priority setting can be:
	 * 0x17	- task running at 488 Hz - highest priority
	 * 0x16	- task running at 244 Hz
	 * 0x15	- task running at 122 Hz
	 * 0x14	- task running at 61 Hz
	 * 0x13	- task running at 30 Hz
	 * 0x12	- task running at 15 Hz
	 * 0x11	- task running at 7 Hz
	 * 0x10	- task running at 3 Hz
	 * other value - user specified priority
	 * \param [in] Pointer to the driver structure.
	 * \param [in] Application offset in the scheduler application.
	 * \param [in] Application priority setting.
	 * \param [in] Application activity setting.
	 */
	void (* acc_schedulerParamSet)(struct acc_t *self, acc_Scheduler_Params_App_Offset_t app_offset, acc_Task_Priority_t priority, acc_Task_Activity_t activity);

	/**
	 * \brief Configures the device's communication mode.
	 * Possible communication modes:
	 * Normal - communication with the accelerometer is performed using data requests.
	 * Legacy - communication is based on quick read mailboxes.
	 * \param [in] Pointer to the driver structure.
	 * \param [in] New communication mode.
	 */
	void (* acc_setCommunicationMode)(struct acc_t *self, acc_Communication_mode_t mode);

	/**
	 * \brief Makes changes to an application's config registers using the mailbox application.
	 * \param [in] Pointer to the driver itself.
	 * \param [in] Configured application's ID.
	 * \param [in] Configuration register offset.
	 * \param [in] Pointer to the buffer containing configuration data to be written.
	 * \param [in] Size of data in bytes.
	 */
	void (* acc_mailboxWriteConfRequest)(struct acc_t *self, acc_App_ID_t id, uint16_t offset, uint8_t *data, uint8_t size);

	/**
	 * \brief Sends a read request for an application's configuration register data.
	 * \param [in] Pointer to the driver itself.
	 * \param [in] ID of the application data will be read from.
	 * \param [in] Configuration register offset.
	 * \param [in] Size of data in bytes.
	 */
	void (* acc_mailboxReadConfRequest)(struct acc_t *self, acc_App_ID_t id, uint16_t offset, uint8_t size);

	/**
	 * \brief Sends a read request for an application's status register data.
	 * \param [in] Pointer to the driver itself.
	 * \param [in] ID of the application data will be read from.
	 * \param [in] Configuration register offset.
	 * \param [in] Size of data in bytes.
	 */
	void (* acc_mailboxReadRequest)(struct acc_t *self, acc_App_ID_t id, uint16_t offset, uint8_t size);

	/**
	 * \brief Reads previously request data stored in the mailbox registers.
	 * \param [in] Pointer to the driver itself.
	 * \param [out] Pointer to the buffer to which the data will be written.
	 * \param [in] Size of data in bytes.
	 * \return Communication error code.
	 */
	acc_Error_t (* acc_mailboxReadData)(struct acc_t *self, uint8_t *buffer, uint8_t size);

	/**
	 * \brief Reads data from the accelerometer.
	 * Reads data from the accelerometer's status/config registers located at the provided address.
	 * \param [in] Pointer to the driver structure.
	 * \param [in] Address of the register data will be read from in the peripheral's memory.
	 * \param [out] Pointer to the buffer to which the data will be written.
	 * \param [in] Size of data in bytes.
	 */
	void (* acc_readData)(struct acc_t *self, uint8_t address, uint8_t *data, uint8_t size);

	/**
	 * \brief Writes data to the accelerometer.
	 * Writes data to the accelerometer's status/config registers located at the provided address.
	 * \param [in] Pointer to the driver structure.
	 * \param [in] Address of the register data will be written to in the peripheral's memory.
	 * \param [in] Pointer to the buffer containing data to be written.
	 * \param [in] Size of data in bytes.
	 */
	void (* acc_writeData)(struct acc_t *self, uint8_t address, uint8_t *data, uint8_t size);

	/**
	 * \brief Performs a hard reset of the accelerometer.
	 * \param [in] Pointer to the driver structure.
	 */
	void (* acc_hardReset)(struct acc_t *self);
};

extern "C" {
/**
 * \brief Creates and allocates the accelerometer's driver structure.
 * \return Pointer to the driver structure.
 */
struct acc_t * new_acc(void);

/**
 * \brief Deletes the driver structure.
 * \param [in] Pointer to the driver structure.
 */
void delete_acc(struct acc_t **drv);
}

#endif




