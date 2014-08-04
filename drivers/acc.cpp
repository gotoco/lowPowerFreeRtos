
#include "stm32l1xx.h"
#include "gpio.h"
#include "spi.h"
#include "acc.h"
#include "hdr_spi.h"

#include "bsp.h"

#define NO_ERROR 0


int init_driver (struct acc_t * self, int id, int buffer_size){
	printf ("init_driver \n");

	return NO_ERROR;
}

int release_driver (struct acc_t * self){
	printf ("release_driver \n");

	return NO_ERROR;
}

int send_command (struct acc_t * self, char command){
	printf ("send_command \n");

	return NO_ERROR;
}

int recive_command (struct acc_t * self){
	printf ("recive_command \n");

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


struct acc_t * new_driver(struct driver_configuration * config)
{
	printf (config->driver_name);
	static struct acc_t * drv = NULL;

	if (drv == NULL)
	{
		drv = (struct acc_t*) malloc (sizeof (struct acc_t) );
		drv->init_driver = &init_driver;
		drv->release_driver = &release_driver;
		drv->send_command   = &send_command;
		drv->recive_command = &recive_command;
		drv->flush_buffer   = &flush_buffer;
		drv->redirect_output= &redirect_output;
		drv->stop_redirect  = &stop_redirect;
		drv->driver_strategy = config->driver_strategy;
	}
	return drv;
}




/*
 * acc.c
 *
 *  Created on: Jul 4, 2014
 *      Author: uBirds_ARM
 */


uint8_t command;


/**
 * \brief Reading data from ACC.
 *
 *  Basic function for reading data from ACC. It recives all data from Mailbox
 *
 * \param [in] Buffor for data
 * \param [in] Buffor size */

inline uint8_t*
acc_ReadData(uint8_t* data, uint8_t number_bytes)
{
	SPIx_SSB_bb=SPIx_SSB_START;
	uint8_t addres=AAC_SPI_READ_ADDRESS;
	spiTransfer(&addres, NULL , 1);
	spiTransfer(NULL, data, number_bytes);
	SPIx_SSB_bb=SPIx_SSB_END;
	return data;
}

inline void
acc_WriteData(uint8_t* data, uint8_t number_bytes)
{
	SPIx_SSB_bb=SPIx_SSB_START;
	uint8_t address=ACC_SPI_WRITE_ADDRESS;
	spiTransfer(&address, NULL , 1);
	spiTransfer(data, NULL , number_bytes);
	SPIx_SSB_bb=SPIx_SSB_END;
}



void
acc_WakeUp()
{
	uint8_t offset=0x06, mail[5];
	command=ACC_COMMAND_WRITE_CONF;

	mail[0]=ACC_ID_SLEEP;
	mail[1]=(command<<4 | (uint8_t)(offset>>8));
	mail[2]=(uint8_t)offset;
	mail[3]=1;
	mail[4]=0x00;
	acc_WriteData(mail , 5);
}

void
acc_MailboxReadData(uint8_t app_id, uint8_t *data, uint8_t size, uint8_t offset)
{
	uint8_t data_buff[4], addres=AAC_SPI_READ_ADDRESS;
	acc_MailboxReadAsk( app_id,  size,  offset);

	uint16_t czekaj=ACC_SPI_READ_DEALY;
	while(czekaj--);
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
	spiTransfer(NULL, data, size);
	SPIx_SSB_bb=SPIx_SSB_END;
}

void
acc_MailboxConfAsk(uint8_t app_id, uint8_t size, uint16_t offset)
{
	uint8_t mail[4], command=ACC_COMMAND_READ_CONF;

	mail[0]=app_id;
	mail[1]=(command<<4 | (uint8_t)(offset>>8));
	mail[2]=(uint8_t)offset;
	mail[3]=size;
	acc_WriteData(mail , 4);
}

void
acc_MailboxReadConf(uint8_t app_id, uint8_t *buffor, uint8_t size, uint8_t offset)
{
	acc_MailboxConfAsk( app_id,  size,  offset);
	acc_ReadData(buffor, size);
}

void
acc_MailboxSendConfig(uint8_t app_id, uint8_t* config_data, uint8_t size_config, uint8_t offset)
{
	uint8_t mail[5], address=ACC_SPI_WRITE_ADDRESS;
	uint16_t czekaj=ACC_SPI_READ_DEALY;
	command=ACC_COMMAND_WRITE_CONF;

	mail[0]=address;
	mail[1]=app_id;
	mail[2]=(command<<4 | (uint8_t)(offset>>8));
	mail[3]=(uint8_t)offset;
	mail[4]=size_config;

	SPIx_SSB_bb=SPIx_SSB_START;
	spiTransfer(mail, 0 , 5);
	spiTransfer(config_data, NULL , size_config);
	SPIx_SSB_bb=SPIx_SSB_END;
	while(czekaj--);

}

void
acc_MailboxReadAsk(uint8_t app_id, uint8_t size, uint16_t offset)
{
	uint8_t mail[4], command=ACC_COMMAND_READ_STAT;

	mail[0]=app_id;
	mail[1]=(command<<4 | (uint8_t)(offset>>8));
	mail[2]=(uint8_t)offset;
	mail[3]=size;
	acc_WriteData(mail , 4);
}

// funkcje do sprawdzenia

uint16_t* acc_GetPoss()
{
	uint8_t poss_buffor[6];
	acc_MailboxReadData(ACC_ID_AFE, poss_buffor, 6, 0);
	poss[0]=(poss_buffor[0]<<8)| (poss_buffor[1]);
	poss[1]=(poss_buffor[2]<<8)| (poss_buffor[3]);
	poss[2]=(poss_buffor[6]<<8)| (poss_buffor[5]);
	return poss;
}

uint8_t acc_GetTap()
{
	uint8_t tap_buffor[2];
	acc_MailboxReadData(ACC_ID_TAP, tap_buffor, 2, 0);
	if(tap_buffor[0] & 0x80)
	{
		return 1;
	}
	else if(tap_buffor[1] & 0x80)
	{
		return 2;
	}
	else
		return 0;
}

uint16_t acc_GetFarmeCount()
{
	uint8_t frame_buffor[2];
	acc_MailboxReadData(ACC_ID_FRAMEC, frame_buffor, 2, 0);
	frame=(frame_buffor[0]<<8)| (frame_buffor[1]);
	return frame;
}



void
acc_InitAFE()
{
	uint8_t conf2[8], conf1, buffor[2];


	conf1=0x40;
	acc_MailboxSendConfig(ACC_ID_AFE, &conf1 , 1, 0);

	acc_ReadData(buffor, 2);

	conf2[0]=0x03;
	conf2[1]=0x03;
	conf2[2]=0x00;
	conf2[3]=0x00;
	conf2[4]=0x08;

	acc_MailboxSendConfig(ACC_ID_AFE, conf2 , 5, 0x08);
	acc_ReadData(buffor, 2);
}

void
acc_InitTap()
{
	uint8_t conf[9], buffor[2];


	conf[0]=0x0B;
	conf[1]=0xB8;

	conf[2]=0x0A;
	conf[3]=0x78;
	conf[4]=0xFF;

	conf[5]=0x05;
	conf[6]=0x04;

	conf[7]=0x07;

	conf[8]=0x00;

	acc_MailboxSendConfig(ACC_ID_TAP, conf , 9, 0x00);
	acc_ReadData(buffor, 2);
}

void
acc_InitPedometr(uint8_t height, uint8_t weight, enum Gender Man )
{

	uint8_t config_data[16];

	if(height!=0)
		user_height=height;
	else
		user_height=ACC_PEDO_DEF_HEIGHT;
	if(weight!=0)
		user_weight=weight;
	else
		user_weight=ACC_PEDO_DEF_WEIGHT;

	config_data[0]= (ACC_PEDO_SLEEP_MIN >> 8);
	config_data[1]= (uint8_t)ACC_PEDO_SLEEP_MIN;
	config_data[2]= (ACC_PEDO_SLEEP_MAX>>8);
	config_data[3]= (uint8_t)ACC_PEDO_SLEEP_MAX;
	config_data[4]= (ACC_PEDO_SLEEP_THRES>>8);
	config_data[5]= (uint8_t)ACC_PEDO_SLEEP_THRES;
	config_data[6]= (ACC_PEDO_CONF<<7) | (ACC_PEDO_ACT<<6) | (ACC_PEDO_SLP<<5);
	config_data[7]=ACC_PEDO_STEPLEN;
	config_data[8]=user_height;
	config_data[9]=user_weight;
	config_data[10]=ACC_PEDO_FILSTEP;
	config_data[11]=(Man<<7) | ACC_PEDO_FILTTIME;
	config_data[12]=ACC_PED_SPEED_PERIOD;
	config_data[13]=ACC_PEDO_SPEED_COUNT;

	//acc_MailboxSendConfig(ACC_ID_PEDOMETR, config_data,13 ,0);
}

void acc_ResetApp(uint8_t app_id)
{
	uint8_t reset_buff[4];
	acc_MailboxReadConf(app_id, reset_buff, 3,0);
	reset_buff[app_id%4]|=(1<<(app_id%8));
	acc_MailboxSendConfig(app_id, reset_buff, 3,0);
}

void acc_ResetAll()
{
	uint8_t reset_buff[4];
	acc_MailboxReadConf(ACC_ID_RESET_CLC, reset_buff, 3,0);
	reset_buff[2]|=1;
	acc_MailboxSendConfig(ACC_ID_RESET_CLC, reset_buff, 3,0);
}


void acc_SuspendApp()
{
	uint8_t reset_buff[4];
	acc_MailboxReadConf(app_id, reset_buff, 3, 4);
	reset_buff[app_id%4]|=(1<<(app_id%8));
	acc_MailboxSendConfig(app_id, reset_buff, 3,0);
}






