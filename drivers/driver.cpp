#include <stdio.h>

#include <stdlib.h>

#include "driver.h"

#define NO_ERROR 0


int init_driver (struct driver_t * self, int id, int buffer_size)
{
	printf ("init_driver \n");

	return NO_ERROR;
}

int release_driver (struct driver_t * self)
{
	printf ("release_driver \n");

	return NO_ERROR;
}

int send_command (struct driver_t * self, char command)
{
	printf ("send_command \n");

	return NO_ERROR;
}

int recive_command (struct driver_t * self)
{
	printf ("recive_command \n");

	return NO_ERROR;
}

int flush_buffer (struct driver_t * self)
{
	printf ("flush_buffer \n");

	return NO_ERROR;
}

int redirect_output (struct driver_t * self, char* external_buffer)
{
	external_buffer[0] = 'A';

	printf ("redirect_output \n");

	return NO_ERROR;
}

int stop_redirect (struct driver_t * self)
{

	printf ("stop_redirect \n");

	return NO_ERROR;
}


struct driver_t * new_driver(struct driver_configuration * config)
{
	printf (config->driver_name);
	static struct driver_t * drv = NULL;

	if (drv == NULL)
	{
		drv = (struct driver_t*) malloc (sizeof (struct driver_t) );
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
