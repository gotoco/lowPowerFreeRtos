#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include "mma955x_adapter.h"
#include "mma955x_def.h"
#include "mma955x_drv.h"
#include "bsp.h"
#include "config.h"
#include "misc.h"
#include "spi.h"
#include "gpio.h"

void _mma955x_put_mbox_legacy_mode(struct acc_t *self) {
	uint8_t data [] = {0x18, 0x20, 0x00, 0x01, 0x10};

	spiStart();
	spiTransfer(data, NULL, 5);
	spiStop();
}

void acc_init(struct acc_t *self) {
	self->acc_hardReset(self);
	self->acc_wakeUp(self);

	uint8_t config = acc_Mailbox_Config_INT_O_EN | acc_Mailbox_Config_INT_O_FRAME_EN | acc_Mailbox_Config_INT_O_Legacy_Mode | acc_Mailbox_Config_INT_UpdMode_Always;
	self->acc_mailboxWriteConfRequest(self, acc_App_ID_Mailbox_Config, 0, &config, 1);

	self->acc_schedulerParamSet(self, acc_Scheduler_Params_Wake_Sleep_Offset, acc_Task_Priority_30Hz, acc_Task_Activity_Always);//sleep-wakeup
	self->acc_schedulerParamSet(self, acc_Scheduler_Params_Reset_Suspend_Clear_Offset, acc_Task_Priority_30Hz, acc_Task_Activity_Always);//reset/suspend/clear

	self->acc_configPedometer(self, &self->pedometer_config);

	_mma955x_put_mbox_legacy_mode(self);

//	if(self->power_mode == acc_Power_Mode_normal)
//		self->acc_configAFE(self, &self->normal_mode_AFE_config);
//	else
//		self->acc_configAFE(self, &self->low_power_mode_AFE_config);

	mma955x_delay(100);
}

acc_Error_t acc_checkVersion(struct acc_t *self, uint8_t *buffer) {
	acc_Error_t error;
	uint8_t error_counter = 10;

	self->buffer[0] = acc_App_ID_Version;
	self->buffer[1] = (ACC_COMMAND_READ_VER << 4);
	self->buffer[2] = 0;
	self->buffer[3] = 12;

	while(--error_counter) {
		self->acc_writeData(self, acc_SPI_Register_Address_Mailbox, self->buffer, 4);
		error = self->acc_mailboxReadData(self, buffer, 12);
		if(error == acc_Error_None) {
			return acc_Error_None;
		}
	}
	return error;
}

void acc_updateLPFHPFAcceleration(struct acc_t *self) {
	uint8_t buffer[12];

	acc_Error_t error;
	self->acc_mailboxReadRequest(self, acc_App_ID_AFE, ACC_SAFE_LPF, sizeof(buffer));
	do {
		error = self->acc_mailboxReadData(self, buffer, sizeof(buffer));
	} while(error != acc_Error_None);

	self->LPFacceleration[0] = ((buffer[0] << 8) | (buffer[1]));
	self->LPFacceleration[1] = ((buffer[2] << 8) | (buffer[3]));
	self->LPFacceleration[2] = ((buffer[4] << 8) | (buffer[5]));
	self->HPFacceleration[0] = ((buffer[6] << 8) | (buffer[7]));
	self->HPFacceleration[1] = ((buffer[8] << 8) | (buffer[9]));
	self->HPFacceleration[2] = ((buffer[10] << 8) | (buffer[11]));
}

void acc_getAcceleration(struct acc_t *self, uint8_t AFE_offset)
{
	uint8_t buffer[6];
	acc_Error_t error;

	mma955x_mutex_lock();

	self->acc_mailboxReadRequest(self, acc_App_ID_AFE, AFE_offset, sizeof(buffer));
	do {
		error = self->acc_mailboxReadData(self, buffer, 6);
	} while(error != acc_Error_None);
	self->acceleration[0] = ((buffer[0] << 8) | (buffer[1]));
	self->acceleration[1] = ((buffer[2] << 8) | (buffer[3]));
	self->acceleration[2] = ((buffer[4] << 8) | (buffer[5]));
}

void acc_updatePedometerData(struct acc_t *self) {
	uint8_t buffer[10];

	acc_Error_t error;
	self->acc_mailboxReadRequest(self, acc_App_ID_Pedometer, ACC_SPEDO_STATUS, sizeof(buffer));
	do {
		error = self->acc_mailboxReadData(self, buffer, sizeof(buffer));
	} while(error != acc_Error_None);

	self->activity = (acc_Activity_t)(buffer[0] & 0x07);
	self->stepCount = (buffer[2] << 8) | (buffer[3]);
	self->distance = (buffer[4] << 8) | (buffer[5]);
	self->speed = (buffer[6] << 8) | (buffer[7]);
	self->calories = (buffer[8] << 8) | (buffer[9]);
}

void acc_lowPowerModeConfiguration(struct acc_t *self) {
	mma955x_low_level_init();

	self->power_mode = acc_Power_Mode_low_power;
	self->acc_configAFE(self, &self->low_power_mode_AFE_config);
	self->acc_appSuspend(self, acc_App_ID_Pedometer);
}

void acc_normalModeConfiguration(struct acc_t *self) {
	self->power_mode = acc_Power_Mode_normal;
	self->acc_configAFE(self, &self->normal_mode_AFE_config);
	self->acc_appUnsuspend(self, acc_App_ID_Pedometer);
}

void acc_defaultConfiguration(struct acc_t *self) {
	self->normal_mode_AFE_config.scale = acc_AFE_Full_Scale_4G;
	self->normal_mode_AFE_config.resolution = acc_AFE_ADC_Resolution_10bits;
	self->normal_mode_AFE_config.SFD_rate = acc_AFE_SFD_Rate_244Hz;
	self->normal_mode_AFE_config.HPF_Cut_off_frequency = acc_AFE_HPF_Cut_off_frequency_SFD_Rate_div_by_15;
	self->normal_mode_AFE_config.LPF_Cut_off_frequency = acc_AFE_LPF_Cut_off_frequency_SFD_Rate_div_by_9;
	self->normal_mode_AFE_config.temp_LPF_Cut_off_frequency = acc_AFE_LPF_Cut_off_frequency_SFD_Rate_div_by_9;

	self->normal_mode_AFE_config.AFE_task_activity = acc_Task_Activity_Always;

	self->normal_mode_AFE_config.offset[0] = 0;
	self->normal_mode_AFE_config.offset[1] = 0;
	self->normal_mode_AFE_config.offset[2] = 0;

	self->low_power_mode_AFE_config.scale = acc_AFE_Full_Scale_2G;
	self->low_power_mode_AFE_config.resolution = acc_AFE_ADC_Resolution_10bits;
	self->low_power_mode_AFE_config.SFD_rate = acc_AFE_SFD_Rate_7Hz;
	self->low_power_mode_AFE_config.HPF_Cut_off_frequency = acc_AFE_HPF_Cut_off_frequency_SFD_Rate_div_by_15;
	self->low_power_mode_AFE_config.LPF_Cut_off_frequency = acc_AFE_LPF_Cut_off_frequency_SFD_Rate_div_by_9;
	self->low_power_mode_AFE_config.temp_LPF_Cut_off_frequency = acc_AFE_LPF_Cut_off_frequency_SFD_Rate_div_by_9;

	self->low_power_mode_AFE_config.AFE_task_activity = acc_Task_Activity_Always;

	self->low_power_mode_AFE_config.offset[0] = 0;
	self->low_power_mode_AFE_config.offset[1] = 0;
	self->low_power_mode_AFE_config.offset[2] = 0;

	self->pedometer_config.sleep_minimum = ACC_CPEDO_SLEEP_MIN;
	self->pedometer_config.sleep_maximum = ACC_CPEDO_SLEEP_MAX;
	self->pedometer_config.sleep_count_threshold = ACC_CPEDO_SLEEP_THRES;

	self->pedometer_config.step_length = ACC_CPEDO_STEPLEN;
	self->pedometer_config.height = 175;
	self->pedometer_config.weight = 80;
	self->pedometer_config.filter_step_count = ACC_CPEDO_FILSTEP;
	self->pedometer_config.gender = Male;
	self->pedometer_config.filter_time = ACC_CPEDO_FILTTIME;
	self->pedometer_config.speed_period = ACC_CPEDO_SPEED_PERIOD;
	self->pedometer_config.step_coalescence = ACC_CPEDO_STEPCOL;

	self->pedometer_config.activity_count_threshold = ACC_CPEDO_ACTTHD;

	self->power_mode = acc_Power_Mode_normal;
}

void acc_setOffset(struct acc_t *self, uint16_t X, uint16_t Y, uint16_t Z, uint8_t calibrate) {
	//TODO
	//0x7FFF is a middle of a offset range
	uint16_t buffor;
	//setting offset
	self->normal_mode_AFE_config.offset[0] = X;
	self->normal_mode_AFE_config.offset[1] = Y;
	self->normal_mode_AFE_config.offset[2] = Z;

	self->low_power_mode_AFE_config.offset[0] = X;
	self->low_power_mode_AFE_config.offset[1] = Y;
	self->low_power_mode_AFE_config.offset[2] = Z;

	if(calibrate != 0) {
		self->acc_wakeUp(self);
		self->acc_getAcceleration(self, ACC_SAFE_STAGE0_REG);

		for(uint8_t i = 2; i < 3; --i) {
			buffor = self->acceleration[i];
			if (buffor > 0x7FFF) {
				buffor = 0xFFFF - buffor;
				buffor += 0x7FFF;
			} else {
				buffor = 0x7FFF - buffor;
			}
			self->normal_mode_AFE_config.offset[i] = self->low_power_mode_AFE_config.offset[i] = buffor;
		}
	}
	mma955x_delay(100);
}

void acc_configAFE(struct acc_t *self, struct acc_AFE_config_t *config) {
	uint8_t conf[13];
	self->acc_wakeUp(self);
	self->acc_appReset(self, acc_App_ID_AFE);

	memcpy(&self->AFE_config, config, sizeof(*config));

	self->dt = 1000 * (1 << (self->AFE_config.SFD_rate - acc_AFE_SFD_Rate_488Hz)) / ACC_AFE_MAXIMUM_UPDATE_RATE;

	conf[0] = (self->AFE_config.scale << 6) | (self->AFE_config.resolution << 2);
	conf[1] = 0;

	memcpy(conf + 2, config->offset, 6);

	conf[8] = self->AFE_config.HPF_Cut_off_frequency;
	conf[9] = self->AFE_config.LPF_Cut_off_frequency;
	conf[10] = self->AFE_config.temp_LPF_Cut_off_frequency;
	conf[11] = 0;
	conf[12] = self->AFE_config.SFD_rate;

	self->acc_mailboxWriteConfRequest(self, acc_App_ID_AFE, 0, conf, sizeof(conf));
	self->acc_schedulerParamSet(self, acc_Scheduler_Params_AFE_Offset, (acc_Task_Priority_t)((uint8_t)acc_Task_Priority_488Hz + (uint8_t)acc_AFE_SFD_Rate_488Hz - (uint8_t)self->AFE_config.SFD_rate), self->AFE_config.AFE_task_activity);

	mma955x_delay(100);
}

void acc_configPedometer(struct acc_t *self, struct acc_Pedometer_config_t *config) {
	uint8_t conf[16];
	self->acc_wakeUp(self);

	memcpy(conf, &config->sleep_minimum, 2);
	memcpy(conf + 2, &config->sleep_maximum, 2);
	memcpy(conf + 4, &config->sleep_count_threshold, 2);

	conf[6] = 0b11100000;
	conf[7] = config->step_length;
	conf[8] = config->height;
	conf[9] = config->weight;
	conf[10] = config->filter_step_count;
	conf[11] = (config->gender << 7) | min(config->filter_time, 0x7F);
	conf[12] = config->speed_period;
	conf[13] = config->step_coalescence;
	memcpy(conf + 14, &config->activity_count_threshold, 2);

	self->acc_mailboxWriteConfRequest(self, acc_App_ID_Pedometer, 0, conf, sizeof(conf));

	mma955x_delay(100);
}

void acc_appReset(struct acc_t *self, acc_App_ID_t id) {
	uint8_t buff[4];
	self->acc_mailboxReadConfRequest(self, acc_App_ID_Reset_Suspend_Clear, 0, 4);
	self->acc_mailboxReadData(self, buff, 4);
	buff[3 - (uint8_t)(id / 8)] |= (1 << (id % 8));
	self->acc_mailboxWriteConfRequest(self, acc_App_ID_Reset_Suspend_Clear, 0, buff, 4);
}

void acc_appSuspend(struct acc_t *self, acc_App_ID_t id) {
	uint8_t buff[4];
	self->acc_mailboxReadConfRequest(self, acc_App_ID_Reset_Suspend_Clear, 4, 4);
	self->acc_mailboxReadData(self, buff, 4);
	buff[3 - (uint8_t)(id / 8)] |= (1 << (id % 8));
	self->acc_mailboxWriteConfRequest(self, acc_App_ID_Reset_Suspend_Clear, 4, buff, 4);
}

void acc_appUnsuspend(struct acc_t *self, acc_App_ID_t id) {
	uint8_t buff[4];
	self->acc_mailboxReadConfRequest(self, acc_App_ID_Reset_Suspend_Clear, 4, 4);
	self->acc_mailboxReadData(self, buff, 4);
	buff[3 - (uint8_t)(id / 8)] &= ~(1 << (id % 8));
	self->acc_mailboxWriteConfRequest(self, acc_App_ID_Reset_Suspend_Clear, 4, buff, 4);
}

void acc_wakeUp(struct acc_t *self) {
	uint8_t conf = 0;
	self->acc_mailboxWriteConfRequest(self, acc_App_ID_Sleep_Wake, ACC_CSLEEP_CFG, &conf, 1);

	mma955x_delay(10);
}

void acc_schedulerParamSet(struct acc_t *self, acc_Scheduler_Params_App_Offset_t app_offset, acc_Task_Priority_t priority, acc_Task_Activity_t activity) {
	uint8_t parameters = (activity << 6) | priority;
	self->acc_mailboxWriteConfRequest(self, acc_App_ID_Scheduler, app_offset, &parameters, 1);

	mma955x_delay(10);
}

void acc_mailboxWriteConfRequest(struct acc_t *self, acc_App_ID_t id, uint16_t offset, uint8_t *data, uint8_t size) {
	self->buffer[0] = id;
	self->buffer[1] = (ACC_COMMAND_WRITE_CONF << 4 | (uint8_t)(offset >> 8));
	self->buffer[2] = (uint8_t)offset;
	self->buffer[3] = size;

	spiStart();
	uint8_t address = acc_SPI_Register_Address_Mailbox | 0b10000000;
	spiTransfer(&address, NULL, 1);
	spiTransfer(self->buffer, NULL, 4);
	spiTransfer(data, NULL, size);
	spiStop();
	mma955x_delay(1);
}

void acc_mailboxReadConfRequest(struct acc_t *self, acc_App_ID_t id, uint16_t offset, uint8_t size) {
	self->buffer[0] = id;
	self->buffer[1] = (ACC_COMMAND_READ_CONF << 4 | (uint8_t)(offset >> 8));
	self->buffer[2] = (uint8_t)offset;
	self->buffer[3] = size;
	self->acc_writeData(self, acc_SPI_Register_Address_Mailbox, self->buffer, 4);
}

void acc_mailboxReadRequest(struct acc_t *self, acc_App_ID_t id, uint16_t offset, uint8_t size) {
	self->buffer[0] = id;
	self->buffer[1] = (ACC_COMMAND_READ_STAT << 4 | (uint8_t)(offset >> 8));
	self->buffer[2] = (uint8_t)offset;
	self->buffer[3] = size;
	self->acc_writeData(self, acc_SPI_Register_Address_Mailbox, self->buffer, 4);
}

acc_Error_t acc_mailboxReadData(struct acc_t *self, uint8_t *buffer, uint8_t size) {
	uint8_t address = acc_SPI_Register_Address_Mailbox;
	acc_Error_t error = acc_Error_None;

	spiStart();
	spiTransfer(&address, NULL, 1);
	spiTransfer(NULL, self->buffer, 4);

	if (self->buffer[1] & 0x80)				// data ready (COCO set) and no errors
		spiTransfer(NULL, buffer, size);	// receiving data
	else
		error = (acc_Error_t)(self->buffer[1] & 0x7f);

	spiStop();
	return error;
}

void acc_readData(struct acc_t *self, uint8_t address, uint8_t *data, uint8_t size) {
	//mma955x_read_data();
	spiStart();
	spiTransfer(&address, NULL, 1);
	spiTransfer(NULL, data, size);
	spiStop();
}

void acc_writeData(struct acc_t *self, uint8_t address, uint8_t *data, uint8_t size) {
	//mma955x_write_data();
	address |= 0b10000000;
	spiStart();
	spiTransfer(&address, NULL, 1);
	spiTransfer(data, NULL, size);
	spiStop();
	mma955x_delay(1);
}

void acc_hardReset(struct acc_t *self) {
	SPIx_SSB_bb = 0;
	ACC_RESET_bb = 0;
	mma955x_delay(100);
	ACC_RESET_bb = 1;
	mma955x_delay(100);
	SPIx_SSB_bb = 1;
}

struct acc_t * new_acc(void) {
	static struct acc_t * drv = NULL;

	if(drv == NULL) {
		drv = (struct acc_t *) malloc(sizeof(struct acc_t));

		drv->driver_name = "MMA9953L";

		drv->acc_init = &acc_init;
		drv->acc_checkVersion = &acc_checkVersion;

		drv->acc_updateLPFHPFAcceleration = &acc_updateLPFHPFAcceleration;
		drv->acc_getAcceleration = &acc_getAcceleration;
		drv->acc_updatePedometerData = &acc_updatePedometerData;

		drv->acc_lowPowerModeConfiguration = &acc_lowPowerModeConfiguration;
		drv->acc_normalModeConfiguration = &acc_normalModeConfiguration;
		drv->acc_defaultConfiguration = &acc_defaultConfiguration;
		drv->acc_setOffset = &acc_setOffset;

		drv->acc_configAFE = &acc_configAFE;
		drv->acc_configPedometer = &acc_configPedometer;

		drv->acc_appReset = &acc_appReset;
		drv->acc_appSuspend = &acc_appSuspend;
		drv->acc_appUnsuspend = &acc_appUnsuspend;
		drv->acc_wakeUp = &acc_wakeUp;

		drv->acc_schedulerParamSet = &acc_schedulerParamSet;

		drv->acc_mailboxWriteConfRequest = &acc_mailboxWriteConfRequest;
		drv->acc_mailboxReadConfRequest = &acc_mailboxReadConfRequest;
		drv->acc_mailboxReadRequest = &acc_mailboxReadRequest;
		drv->acc_mailboxReadData = &acc_mailboxReadData;
		drv->acc_readData = &acc_readData;
		drv->acc_writeData = &acc_writeData;

		drv->acc_hardReset = &acc_hardReset;

		drv->acc_defaultConfiguration(drv);
		drv->acc_lowPowerModeConfiguration(drv);
	}
	return drv;
}

void delete_acc(struct acc_t **drv) {
	free(*drv);
	*drv = NULL;
}
