
#include <stddef.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include "acc_def.h"
#include "config.h"
#include "stm32l1xx.h"
#include "gpio.h"
#include "peripherals/spi.h"
#include "acc.h"
#include "hdr/hdr_spi.h"
#include "config.h"
#include "bsp.h"

#define NO_ERROR 0

void acc_InitTap(struct acc_t *self);

uint8_t command;

void acc_Init(struct acc_t *self, uint8_t work_mode, acc_config_t *config)
{
	acc_InitTap(self);
}

uint8_t*
acc_ReadData(struct acc_t *self, uint8_t* data, uint8_t size)
{
	SPIx_SSB_bb=SPIx_SSB_START;
	uint8_t addres=AAC_SPI_READ_ADDRESS;
	spiTransfer(&addres, NULL , 1);
	spiTransfer(NULL, data, size);
	SPIx_SSB_bb=SPIx_SSB_END;
	return data;
}

void
acc_WriteData(struct acc_t *self, uint8_t* data, uint8_t size)
{
	SPIx_SSB_bb=SPIx_SSB_START;
	uint8_t address=ACC_SPI_WRITE_ADDRESS;
	spiTransfer(&address, NULL , 1);
	spiTransfer(data, NULL , size);
	SPIx_SSB_bb=SPIx_SSB_END;
}

void acc_MailboxSendConfig( struct acc_t *self, uint8_t app_id, uint8_t* config_data, uint8_t size_config, uint8_t offset )
{
	uint8_t mail[5], address=ACC_SPI_WRITE_ADDRESS;
	uint16_t czekaj=ACC_SPI_READ_DEALY;

	mail[0]=address;
	mail[1]=app_id;
	mail[2]=(ACC_COMMAND_WRITE_CONF<<4 | (uint8_t)(offset>>8));
	mail[3]=(uint8_t)offset;
	mail[4]=size_config;

	SPIx_SSB_bb=SPIx_SSB_START;
	spiTransfer(mail, 0 , 5);
	spiTransfer(config_data, NULL , size_config);
	SPIx_SSB_bb=SPIx_SSB_END;
	while(czekaj--);
}

uint8_t* acc_MailboxReadData( struct acc_t *self, uint8_t app_id, uint8_t *buffer, uint8_t size, uint8_t offset )
{
	uint8_t data_buff[4], addres=AAC_SPI_READ_ADDRESS;
	self->acc_MailboxReadAsk(self, app_id,  size,  offset);

	uint16_t czekaj=ACC_SPI_READ_DEALY;
	while(czekaj--);  // do poprawy
	while(1)
	{
		SPIx_SSB_bb=SPIx_SSB_START;
		spiTransfer(&addres, NULL , 1);
		spiTransfer(NULL, data_buff, 4);
		if(data_buff[1]==0x80)
		{
			break;
		}  // dopisac elsa by obs³ugiwa³ b³êdy gdy dane nie zostan¹ poprawnie odebrane
	}
	spiTransfer(NULL, buffer, size);
	SPIx_SSB_bb=SPIx_SSB_END;
	return buffer;
}

void acc_MailboxReadAsk( struct acc_t *self, uint8_t app_id, uint8_t amount, uint16_t offset )
{
	uint8_t mail[4], command=ACC_COMMAND_READ_CONF;

	mail[0]=app_id;
	mail[1]=(command<<4 | (uint8_t)(offset>>8));
	mail[2]=(uint8_t)offset;
	mail[3]=amount;
	self->acc_WriteData(self, mail , 4);
}

void
acc_MailboxConfAsk(struct acc_t *self, uint8_t app_id, uint8_t size, uint16_t offset)
{
	uint8_t mail[4];
	mail[0]=app_id;
	mail[1]=(ACC_COMMAND_READ_CONF<<4 | (uint8_t)(offset>>8));
	mail[2]=(uint8_t)offset;
	mail[3]=size;
	self->acc_WriteData(self,mail , 4);
}

void acc_MailboxReadConf( struct acc_t *self, uint8_t app_id, uint8_t *buffer, uint8_t size, uint8_t offset)
{
	self->acc_MailboxConfAsk(self, app_id,  size,  offset);
	self->acc_ReadData(self, buffer, size);
}

void acc_WakeUp( struct acc_t *self)
{
  	uint8_t offset=0x06, mail[5];

  	mail[0]=ACC_ID_SLEEP;
  	mail[1]=(ACC_COMMAND_WRITE_CONF<<4 | (uint8_t)(offset>>8));
  	mail[2]=(uint8_t)offset;
  	mail[3]=1;
  	mail[4]=0x00;
  	self->acc_WriteData(self, mail , 5);
}

uint16_t* acc_GetPoss(struct acc_t *self)
{
	uint8_t poss_buffer[6];
	self->acc_MailboxReadData(self, ACC_ID_AFE, poss_buffer, 6, 0);
	self->position[0]=(poss_buffer[0]<<8)| (poss_buffer[1]);
	self->position[1]=(poss_buffer[2]<<8)| (poss_buffer[3]);
	self->position[2]=(poss_buffer[6]<<8)| (poss_buffer[5]);
	return self->position;
}

uint8_t  acc_GetTap( struct acc_t *self )
{
	self->acc_MailboxReadData(self, ACC_ID_TAP, self->tap, 2, 0);
	if(self->tap[0] & 0x80)
	{
		return (uint8_t)1;
	}
	else if(self->tap[1] & 0x80)
	{
		return (uint8_t)2;
	}
	else
		return 0;
}

void
acc_InitAFE(struct acc_t *self)
{
	uint8_t conf2[8], conf1, buffer[2];
	conf1=0x40;
	self->acc_MailboxSendConfig(self, ACC_ID_AFE, &conf1 , 1, 0);
	self->acc_ReadData(self, buffer, 2);

	conf2[0]=0x03;
	conf2[1]=0x03;
	conf2[2]=0x00;
	conf2[3]=0x00;
	conf2[4]=0x08;

	self->acc_MailboxSendConfig(self, ACC_ID_AFE, conf2 , 5, 0x08);
	self->acc_ReadData(self, buffer, 2);
}

void
acc_InitTap(struct acc_t *self)
{
	uint8_t conf[9], buffer[2];


	conf[0]=0x0B;
	conf[1]=0xB8;

	conf[2]=0x0A;
	conf[3]=0x78;
	conf[4]=0xFF;

	conf[5]=0x05;
	conf[6]=0x04;

	conf[7]=0x07;

	conf[8]=0x00;

	self->acc_MailboxSendConfig(self, ACC_ID_TAP, conf , 9, 0x00);
	self->acc_ReadData(self, buffer, 2);
}

void
acc_InitPedometr(struct acc_t *self, uint8_t height, uint8_t weight, Gender_t Man )
{

	uint8_t config_data[16];

	if(height!=0)
		self->user_height=height;
	else
		self->user_height=ACC_PEDO_DEF_HEIGHT;
	if(weight!=0)
		self->user_weight=weight;
	else
		self->user_weight=ACC_PEDO_DEF_WEIGHT;

	config_data[0]= (ACC_PEDO_SLEEP_MIN >> 8);
	config_data[1]= (uint8_t)ACC_PEDO_SLEEP_MIN;
	config_data[2]= (ACC_PEDO_SLEEP_MAX>>8);
	config_data[3]= (uint8_t)ACC_PEDO_SLEEP_MAX;
	config_data[4]= (ACC_PEDO_SLEEP_THRES>>8);
	config_data[5]= (uint8_t)ACC_PEDO_SLEEP_THRES;
	config_data[6]= (ACC_PEDO_CONF<<7) | (ACC_PEDO_ACT<<6) | (ACC_PEDO_SLP<<5);
	config_data[7]=ACC_PEDO_STEPLEN;
	config_data[8]=self->user_height;
	config_data[9]=self->user_weight;
	config_data[10]=ACC_PEDO_FILSTEP;
	config_data[11]=(Man<<7) | ACC_PEDO_FILTTIME;
	config_data[12]=ACC_PED_SPEED_PERIOD;
	config_data[13]=ACC_PEDO_SPEED_COUNT;

	//acc_MailboxSendConfig(ACC_ID_PEDOMETR, config_data,13 ,0);
}

uint16_t acc_GetFarmeCount(struct acc_t *self)
{
	uint8_t frame_buffer[2];
	self->acc_MailboxReadData(self, ACC_ID_FRAMEC, frame_buffer, 2, 0);
	self->frame=(frame_buffer[0]<<8)| (frame_buffer[1]);
	return self->frame;
}

void acc_ResetApp(struct acc_t *self, uint8_t app_id)
{
	uint8_t reset_buff[4];
	self->acc_MailboxReadConf(self, app_id, reset_buff, 3,0);
	reset_buff[app_id%4]|=(1<<(app_id%8));
	self->acc_MailboxSendConfig(self, app_id, reset_buff, 3,0);
}

void acc_ResetAll(struct acc_t *self)
{
	uint8_t reset_buff[4];
	self->acc_MailboxReadConf(self, ACC_ID_RESET_CLC, reset_buff, 3,0);
	reset_buff[2]|=1;
	self->acc_MailboxSendConfig(self, ACC_ID_RESET_CLC, reset_buff, 3,0);
}

void acc_SuspendApp(struct acc_t *self, uint8_t app_id)
{
	uint8_t reset_buff[4];
	self->acc_MailboxReadConf(self, app_id, reset_buff, 3, 4);
	reset_buff[app_id%4]|=(1<<(app_id%8));
	self->acc_MailboxSendConfig(self, app_id, reset_buff, 3,0);
}


int acc_Release(struct acc_t * self){
	printf ("release_driver \n");

	return NO_ERROR;
}

int flush_buffer (struct acc_t * self){
	printf ("flush_buffer \n");

	return NO_ERROR;
}


int redirect_output (struct acc_t * self, char* external_buffer){
	external_buffer[0] = 'A';

	printf ("redirect_output \n");

	return NO_ERROR;
}

int stop_redirect (struct acc_t * self){

	printf ("stop_redirect \n");

	return NO_ERROR;
}

struct acc_t * new_acc_driver(struct acc_config_t * config)
{
	printf (config->driver_name);
	static struct acc_t * drv = NULL;

	if (drv == NULL)
	{
		drv = (struct acc_t*) malloc (sizeof (struct acc_t) );
		drv->acc_Init = &acc_Init;
		drv->acc_Release = &acc_Release;
		drv->acc_WriteData = &acc_WriteData;
		drv->acc_ReadData = &acc_ReadData;
		drv->acc_MailboxSendConfig = &acc_MailboxSendConfig;
		drv->acc_MailboxReadData = &acc_MailboxReadData;
		drv->acc_MailboxReadAsk = &acc_MailboxReadAsk;
		drv->acc_MailboxReadConf = &acc_MailboxReadConf;
		drv->acc_WakeUp = &acc_WakeUp;
		drv->acc_GetPoss = &acc_GetPoss;
		drv->acc_GetTap = &acc_GetTap;

		drv->flush_buffer   = &flush_buffer;
		drv->redirect_output= &redirect_output;
		drv->stop_redirect  = &stop_redirect;
		drv->driver_strategy = config->driver_strategy;
	}
	return drv;
}




