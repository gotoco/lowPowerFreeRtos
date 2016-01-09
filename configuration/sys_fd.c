/**
 * \file sys_fd.c
 * \brief Low-level I/O syscalls for operations on file descriptors.
 *
 * errno values are based on:
 * http://www.gnu.org/software/libc/manual/html_node/Low_002dLevel-I_002fO.html#Low_002dLevel-I_002fO
 * Additionally errno can be ENOTRECOVERABLE when mutex handling fails
 *
 * \author: Mazeryt Freager, http://www.gotoc.co
 * \date 2015-12-12
 */

#include <stdbool.h>
#include <stdio.h>
#include <sys/stat.h>
#include <string.h>
#include <errno.h>


#include "sys_fd.h"
#include "bsp.h"
#include "usart.h"


//#include <cstring>
//#include <fcntl.h>

/*---------------------------------------------------------------------------------------------------------------------+
| local variables
+---------------------------------------------------------------------------------------------------------------------*/

// object for UART0

// object for UART1
//extern const usart_driver_t usart1_handler;
//extern const usart_def_t usart1_t;

/// array with available UARTs
uint32_t *uarts_[] =
{
		0,
};

/// array with available UARTs
uint32_t *uarts_confs[] =
{
		0,
};

// true if given device is already initialized - it's not re-initialized in that case
bool initialized_[sizeof(uarts_) / sizeof(*uarts_)];

/*---------------------------------------------------------------------------------------------------------------------+
| global functions
+---------------------------------------------------------------------------------------------------------------------*/

/**
 * \brief Opens a file.
 *
 * Last character of filename must contain the UART port number - '0' or '1'.
 *
 * \param [in] filename is the name of the file to open.
 *
 * \return -1 for failure or file descriptor on success
 */

int uart_open_r(struct _reent *r, const char *filename, int flags, int mode)
{
	int ret = -1;
	const int file_descriptor = filename[strlen(filename) - 1] - '0';	// get UART port from last character

	// found descriptor must be in valid range
	if (file_descriptor < 0 || (unsigned int)(file_descriptor) >= sizeof(uarts_) / sizeof(*uarts_))
		errno = ENOENT;
	else
	{
		if (!initialized_[file_descriptor])	// device not yet initialized?
		{
			errno = usartInitialize();

			if (errno != 0)	// initialize() failed?
				return ret;
			else
			{
				initialized_[file_descriptor] = true;	// mark device as initialized
				ret = file_descriptor;	// open() successful
			}
		}
		else	// device already initialized
			ret = file_descriptor;
	}

	return ret;
}

/**
 * \brief Read from a file.
 *
 * Read of first character is blocking, following reads are non-blocking.
 *
 * \param [in] file_descriptor is the file descriptor that references an open file.
 * \param [in] buffer points to the buffer to place the read information into.
 * \param [in] size specifies the maximum number of bytes to attempt to read.
 *
 * \return -1 for failure or number of read bytes for success
 */

ssize_t uart_read_r(struct _reent * r, int file_descriptor, void * buffer, size_t size)
{
	size_t count = 0;
//	portTickType ticks_to_wait = portMAX_DELAY;
//	usart_driver_t * d = uarts_[file_descriptor];
//	count = d->read(d, ticks_to_wait, buffer, size);

	return count;
}

/**
 * \brief Write to a file.
 *
 * Write of first character is blocking, following writes are non-blocking.
 *
 * \param [in] file_descriptor is the file descriptor that references an open file.
 * \param [in] buffer is an array of data to write to the open file.
 * \param [in] size is the number of bytes to write to the file.
 *
 * \return -1 for failure or number of written bytes for success
 */

ssize_t uart_write_r(struct _reent * r, int file_descriptor, const void * buffer, size_t size)
{
	portTickType ticks_to_wait = portMAX_DELAY;
	uint32_t uart_nr = uarts_[file_descriptor];
	size_t length = strlen(buffer);

	if(uart_nr == 0){
		if(size == length){
			usartSendString(buffer, ticks_to_wait);
		} else {
			char * new_str = pvPortMalloc((size+1)*sizeof(char));
			memcpy(new_str, buffer, size);
			*(new_str+length) = '\0';

			usartSendBytes(new_str, size, ticks_to_wait);
			vPortFree(new_str);
		}
	}
	return size;
}


