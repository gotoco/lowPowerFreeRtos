/**
 * \file sys_fd.h
 * \brief Low-level I/O syscalls for operations on file descriptors.
 *
 * errno values are based on:
 * http://www.gnu.org/software/libc/manual/html_node/Low_002dLevel-I_002fO.html#Low_002dLevel-I_002fO
 * Additionally errno can be ENOTRECOVERABLE when mutex handling fails
 *
 * \author: Mazeryt Freager, http://www.gotoc.co
 * \date 2015-12-12
 */

#ifndef SYS_FD_H_
#define SYS_FD_H_

#include <stdio.h>

/*---------------------------------------------------------------------------------------------------------------------+
| global functions' declarations
+---------------------------------------------------------------------------------------------------------------------*/

int uart_open_r(struct _reent *r, const char *filename, int flags, int mode);
ssize_t uart_read_r(struct _reent *r, int filedes, void *buffer, size_t size);
ssize_t uart_write_r(struct _reent *r, int filedes, const void *buffer, size_t size);

#ifdef __cplusplus
extern "C"
{
#endif	// __cplusplus



#ifdef __cplusplus
}	// extern "C"
#endif	// __cplusplus

#endif	// UART_STREAM_H_
