/**
 * \file ble112.c
 * \brief BLE O
 *
 * Command interpreter.
 *
 * prefix: command
 *
 * \author Mazeryt Freager
 * \date 2012-08-30
 */

#include "driver.h"

#define NO_ERROR 0


int init_driver (int id, int buffer_size){
	puts ("init_driver ");

	return NO_ERROR;
}

int release_driver (){
	puts ("release_driver ");

	return NO_ERROR;
}

int send_command (char command){
	puts ("send_command ");

	return NO_ERROR;
}

int recive_command (){
	puts ("recive_command ");

	return NO_ERROR;
}

int flush_buffer (){
	puts ("flush_buffer ");


	return NO_ERROR;
}

int redirect_output (char* external_buffer){
	external_buffer[0] = 'A';

	puts ("redirect_output ");


	return NO_ERROR;
}

int stop_redirect (){

	puts ("stop_redirect ");


	return NO_ERROR;
}


struct driver_interface_vtable * new_driver(void)
{

	static struct driver_interface_vtable * drv = NULL;

	if (drv == NULL)
	{
		drv = (driver_interface_vtable*) malloc (sizeof (struct driver_interface_vtable) );
		drv->init_driver = &init_driver;
		drv->release_driver = &release_driver;
		drv->send_command   = &send_command;
		drv->recive_command = &recive_command;
		drv->flush_buffer   = &flush_buffer;
		drv->redirect_output= &redirect_output;
		drv->stop_redirect  = &stop_redirect;

	}
	return drv;
}
