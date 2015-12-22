/**
 * \file sys/io_syscalls.c
 * \brief Low-level I/O syscalls for operations on file descriptors.
 *
 * errno values are based on:
 * http://www.gnu.org/software/libc/manual/html_node/Low_002dLevel-I_002fO.html#Low_002dLevel-I_002fO
 * Additionally errno can be ENOTRECOVERABLE when mutex handling fails
 *
 * \author Freddie Chopin, http://www.freddiechopin.info http://www.distortec.com
 * \author: Mazeryt Freager, http://www.gotoc.co
 * \date 2013-04-17
 */

#include "io_syscalls.h"
#include "sys_fd.h"
#include "usart.h"

#include "config.h"

#include "FreeRTOS.h"
#include "semphr.h"

#include <stdio.h>
#include <sys/stat.h>
#include <string.h>
#include <errno.h>

/*---------------------------------------------------------------------------------------------------------------------+
| local defines
+---------------------------------------------------------------------------------------------------------------------*/

// configuration of the module
#define IO_SYSCALLS_HAVE_CLOSE_R_			1
#define IO_SYSCALLS_HAVE_EXECVE_R_		0
#define IO_SYSCALLS_HAVE_EXIT_			0
#define IO_SYSCALLS_HAVE_FORK_R_		0
#define IO_SYSCALLS_HAVE_FSTAT_R_		0
#define IO_SYSCALLS_HAVE_GETPID_R_		0
#define IO_SYSCALLS_HAVE_ISATTY_R_		0
#define IO_SYSCALLS_HAVE_KILL_R_		0
#define IO_SYSCALLS_HAVE_LINK_R_		0
#define IO_SYSCALLS_HAVE_LSEEK_R_		0
#define IO_SYSCALLS_HAVE_OPEN_R_			1
#define IO_SYSCALLS_HAVE_READ_R_			1
#define IO_SYSCALLS_HAVE_SBRK_R_		0
#define IO_SYSCALLS_HAVE_STAT_R_		0
#define IO_SYSCALLS_HAVE_TIMES_R_		0
#define IO_SYSCALLS_HAVE_UNLINK_R_		0
#define IO_SYSCALLS_HAVE_WAIT_R_		0
#define IO_SYSCALLS_HAVE_WRITE_R_			1

#if IO_SYSCALLS_HAVE_CLOSE_R_ == 1
#	define SET_CLOSE_R_(f)					._close_r = f,
#else
#	define SET_CLOSE_R_(f)
#endif

#if IO_SYSCALLS_HAVE_FSTAT_R_ == 1
#	define SET_FSTAT_R_(f)					._fstat_r = f,
#else
#	define SET_FSTAT_R_(f)
#endif

#if IO_SYSCALLS_HAVE_ISATTY_R_ == 1
#	define SET_ISATTY_R_(f)					._isatty_r = f,
#else
#	define SET_ISATTY_R_(f)
#endif

#if IO_SYSCALLS_HAVE_LINK_R_ == 1
#	define SET_LINK_R_(f)					._link_r = f,
#else
#	define SET_LINK_R_(f)
#endif

#if IO_SYSCALLS_HAVE_LSEEK_R_ == 1
#	define SET_LSEEK_R_(f)					._lseek_r = f,
#else
#	define SET_LSEEK_R_(f)
#endif

#if IO_SYSCALLS_HAVE_OPEN_R_ == 1
#	define SET_OPEN_R_(f)					._open_r = f,
#else
#	define SET_OPEN_R_(f)
#endif

#if IO_SYSCALLS_HAVE_READ_R_ == 1
#	define SET_READ_R_(f)					._read_r = f,
#else
#	define SET_READ_R_(f)
#endif

#if IO_SYSCALLS_HAVE_STAT_R_ == 1
#	define SET_STAT_R_(f)					._stat_r = f,
#else
#	define SET_STAT_R_(f)
#endif

#if IO_SYSCALLS_HAVE_UNLINK_R_ == 1
#	define SET_UNLINK_R_(f)					._unlink_r = f,
#else
#	define SET_UNLINK_R_(f)
#endif

#if IO_SYSCALLS_HAVE_WRITE_R_ == 1
#	define SET_WRITE_R_(f)					._write_r = f,
#else
#	define SET_WRITE_R_(f)
#endif

/// macro used to setup entries in _drivers[] and for 3 standard *_driver
#define SET_DRIVER_(filename, close, fstat, isatty, link, lseek, open, read, stat, unlink, write)				\
	{filename, SET_CLOSE_R_(close) SET_FSTAT_R_(fstat) SET_ISATTY_R_(isatty) SET_LINK_R_(link)			\
	SET_LSEEK_R_(lseek) SET_OPEN_R_(open) SET_READ_R_(read) SET_STAT_R_(stat) SET_UNLINK_R_(unlink) SET_WRITE_R_(write)}

/*---------------------------------------------------------------------------------------------------------------------+
| local variables' types
+---------------------------------------------------------------------------------------------------------------------*/

/// describes an I/O driver
struct Driver_
{
	const char *filename;

#if IO_SYSCALLS_HAVE_CLOSE_R_ == 1
	int (*_close_r)(struct _reent *r, int filedes);
#endif

#if IO_SYSCALLS_HAVE_FSTAT_R_ == 1
	int (*_fstat_r)(struct _reent *r, int filedes, struct stat *st);
#endif

#if IO_SYSCALLS_HAVE_ISATTY_R_ == 1
	int (*_isatty_r)(struct _reent *r, int filedes);
#endif

#if IO_SYSCALLS_HAVE_LINK_R_ == 1
	int (*_link_r)(struct _reent *r, const char *oldname, const char *newname);
#endif

#if IO_SYSCALLS_HAVE_LSEEK_R_ == 1
	off_t (*_lseek_r)(struct _reent *r, int filedes, off_t offset, int whence);
#endif

#if IO_SYSCALLS_HAVE_OPEN_R_ == 1
	int (*_open_r)(struct _reent *r, const char *filename, int flags, int mode);
#endif

#if IO_SYSCALLS_HAVE_READ_R_ == 1
	ssize_t (*_read_r)(struct _reent *r, int filedes, void *buffer, size_t size);
#endif

#if IO_SYSCALLS_HAVE_STAT_R_ == 1
	int (*_stat_r)(struct _reent *r, const char *filename, struct stat *buf);
#endif

#if IO_SYSCALLS_HAVE_UNLINK_R_ == 1
	int (*_unlink_r)(struct _reent *r, const char *filename);
#endif

#if IO_SYSCALLS_HAVE_WRITE_R_ == 1
	ssize_t (*_write_r)(struct _reent *r, int filedes, const void *buffer, size_t size);
#endif
};

/// describes an opened file descriptor
struct OpenedFiledes_
{
	const struct Driver_ *driver;
	int privateFiledes;
};

/*---------------------------------------------------------------------------------------------------------------------+
| local variables
+---------------------------------------------------------------------------------------------------------------------*/

/// table with all supported I/O drivers
static const struct Driver_ drivers_[] =
{
		// filename, _close_r, _fstat_r, _isatty_t, _link_r, _lseek_r, _open_r, _read_r, _stat_r, _unlink_r, _write_r
		SET_DRIVER_("/dev/null", NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL),
		SET_DRIVER_("/dev/uart", NULL, NULL, NULL, NULL, NULL, &uart_open_r, &uart_read_r, NULL, NULL, &uart_write_r),
};

/// standard input
static const struct Driver_ stdinDriver_ =
		SET_DRIVER_("stdin", NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
/// standard output
static const struct Driver_ stdoutDriver_ =
		SET_DRIVER_("stdout", NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL);
/// standard error
static const struct Driver_ stderrDriver_ =
		SET_DRIVER_("stderr", NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL);

/// table which holds all currently open file descriptors
static struct OpenedFiledes_ openedFiledeses_[FOPEN_MAX] =
{
		{&stdinDriver_, 0},		// 0
		{&stdoutDriver_, 0},	// 1
		{&stderrDriver_, 0},	// 2
};

/// holds the number of opened file descriptors
static uint8_t totalOpenedFiledeses_ = 3;

/// mutex for guarding openedFiledeses_[] and totalOpenedFiledeses_
static xSemaphoreHandle mutex_;

/*---------------------------------------------------------------------------------------------------------------------+
| global functions
+---------------------------------------------------------------------------------------------------------------------*/

/**
 * \brief Initializes I/O syscalls module.
 *
 * Creates the mutex.
 *
 * \return 0 on success, -ENOMEM on failure (errno not set)
 */

int ioSyscallsInitialize(void)
{
	int ret = 0;
	mutex_ = xSemaphoreCreateMutex();
	if (mutex_ == NULL)
		ret = -ENOMEM;

	return ret;
}

/*---------------------------------------------------------------------------------------------------------------------+
| global functions - I/O syscalls
+---------------------------------------------------------------------------------------------------------------------*/

#if IO_SYSCALLS_HAVE_CLOSE_R_ == 1

/**
 * \brief Close a file.
 *
 * \param [in] filedes indicates the file descriptor to close.
 *
 * \return -1 for failure or 0 for success
 */

int _close_r(struct _reent * const r, const int filedes)
{
	if (xSemaphoreTake(mutex_, STREAM_MUTEX_TICKS_TO_WAIT) == pdFALSE)
	{
		errno = ENOTRECOVERABLE;
		return -1;
	}

	struct OpenedFiledes_ opened_filedes = openedFiledeses_[filedes];	// save current filedes configuration locally
	openedFiledeses_[filedes].driver = NULL;	// delete filedes configuration from table
	totalOpenedFiledeses_--;

	if (xSemaphoreGive(mutex_) == pdFALSE)
	{
		errno = ENOTRECOVERABLE;
		return -1;
	}

	int ret = 0;

	if (opened_filedes.driver != NULL)	// is such file descriptor opened?
	{
		if (opened_filedes.driver->_close_r != NULL)	// does it have "close" function?
			ret = opened_filedes.driver->_close_r(r, opened_filedes.privateFiledes);	// yes - execute
	}
	else	// file descriptor not opened
	{
		errno = EBADF;	// "The filedes argument is not a valid file descriptor."
		ret = -1;
	}

	return ret;
}

#endif

#if IO_SYSCALLS_HAVE_FSTAT_R_ == 1

/**
 * \brief Status of an open file.
 *
 * \param [in] filedes is a file descriptor referring to a file for which status is returned.
 * \param [out] buf points to a stat structure where status information about the file is to be placed.
 *
 * \return 0 for success, -1 for failure
 */

int _fstat_r(struct _reent * const r, const int filedes, struct stat * const buf)
{
	if (openedFiledeses_[filedes].driver != NULL)	// is such file descriptor opened?
	{
		if (openedFiledeses_[filedes].driver->_fstat_r != NULL)	// does it have "fstat" function?
			// yes - execute
			return openedFiledeses_[filedes].driver->_fstat_r(r, openedFiledeses_[filedes].privateFiledes, buf);
		else
		{
			buf->st_mode = S_IFCHR;	// it's a character-oriented device file
			return 0;
		}
	}
	else
	{
		errno = EBADF;	// "The filedes is not a valid file descriptor."
		return -1;
	}
}

#endif

#if IO_SYSCALLS_HAVE_LINK_R_ == 1

/**
 * \brief Establish a new name for an existing file.
 *
 * \param [in] oldname points to a path name that names an existing file.
 * \param [in] newname points to a path name that names the new directory entry to be created.
 *
 * \return 0 for success, -1 for failure.
 */

int _link_r(struct _reent * const r, const char * const oldname, const char * const newname)
{
	(void)r;	// suppress warning

	// search for "oldname" in _drivers[]
	size_t i = 0;
	while ((i < sizeof(drivers_) / sizeof(drivers_[0])) &&
			(strncmp(oldname, drivers_[i].filename, strlen(drivers_[i].filename)) != 0))
		i++;

	if (i == sizeof(drivers_) / sizeof(drivers_[0]))	// nothing found?
	{
		errno = ENOENT;	// "The named file does not exist"
		return -1;
	}
	else
	{
		if (drivers_[i]._link_r != NULL)
			return drivers_[i]._link_r(r, oldname, newname);
		else
		{
			errno = EACCES;
			return -1;
		}
	}
}

#endif

#if IO_SYSCALLS_HAVE_LSEEK_R_ == 1

/**
 * \brief Set position in a file.
 *
 * \param [in] filedes is the file descriptor of an open file.
 * \param [in] offset specifies the number of bytes to offset the file pointer from a specified file origin.
 * \param [in] whence specifies the location from which to start seeking.
 *
 * \return -1 for failure, resulting file position, measured in bytes from the beginning of the file for success
 */

off_t _lseek_r(struct _reent * const r, const int filedes, const off_t offset, const int whence)
{
	if (openedFiledeses_[filedes].driver != NULL)	// is such file descriptor opened?
	{
		if (openedFiledeses_[filedes].driver->_lseek_r != NULL)	// does it have "lseek" function?
			// yes - execute
			return openedFiledeses_[filedes].driver->_lseek_r(r, openedFiledeses_[filedes].privateFiledes, offset,
					whence);
		else
		{
			// "The filedes corresponds to an object that cannot be positioned, such as a pipe, FIFO or terminal device.
			errno = ESPIPE;
			return -1;
		}
	}
	else
	{
		errno = EBADF;	// "The filedes is not a valid file descriptor."
		return -1;
	}
}

#endif

#if IO_SYSCALLS_HAVE_OPEN_R_ == 1

/**
 * \brief Opens a file.
 *
 * \param [in] filename is the name of the file to open.
 * \param [in] flags is the bitwise inclusive-OR of the file access modes and file status flags.
 * \param [in] mode specifies what permissions the file has when it is created.
 *
 * \return -1 for failure or file descriptor on success
 */

int _open_r(struct _reent * const r, const char * const filename, const int flags, const int mode)
{
	if (xSemaphoreTake(mutex_, STREAM_MUTEX_TICKS_TO_WAIT) == pdFALSE)
	{
		errno = ENOTRECOVERABLE;
		return -1;
	}

	int ret = 0;

	if (totalOpenedFiledeses_ >= FOPEN_MAX)
	{
		errno = EMFILE;	// "The process has too many files open."
		ret = -1;
	}
	else
		totalOpenedFiledeses_++;

	if (xSemaphoreGive(mutex_) == pdFALSE)
	{
		errno = ENOTRECOVERABLE;
		ret = -1;
	}

	if (ret == -1)
		return ret;

	// search for "filename" in _drivers[]
	size_t i = 0;
	while ((i < sizeof(drivers_) / sizeof(drivers_[0])) &&
			(strncmp(filename, drivers_[i].filename, strlen(drivers_[i].filename)) != 0))
		i++;

	if (i == sizeof(drivers_) / sizeof(drivers_[0]))	// nothing found?
	{
		errno = ENOENT;	// "The named file does not exist, and O_CREAT is not specified."
		ret = -1;
	}

	if (ret != -1)
		if (drivers_[i]._open_r != NULL)	// is there an "open" function?
			ret = drivers_[i]._open_r(r, filename, flags, mode);	// try executing it

	if (xSemaphoreTake(mutex_, STREAM_MUTEX_TICKS_TO_WAIT) == pdFALSE)
	{
		errno = ENOTRECOVERABLE;
		return -1;
	}

	// search for free index in openedFiledeses_[] - the search cannot fail, as totalOpenedFiledeses_ was updated before
	size_t j = 3;	// start with 3, as 0, 1 and 2 are always open
	while ((j < sizeof(openedFiledeses_) / sizeof(openedFiledeses_[0])) &&
			(openedFiledeses_[j].driver != NULL))
		j++;

	if (ret != -1)	// open succeeded?
	{
		openedFiledeses_[j].driver = &drivers_[i];	// assign opened file descriptor to free index in openedFiledeses_[]
		openedFiledeses_[j].privateFiledes = ret;
		ret = j;	// success - return file descriptor
	}
	else
		totalOpenedFiledeses_--;	// on fail - decrement number of open files

	if (xSemaphoreGive(mutex_) == pdFALSE)
	{
		errno = ENOTRECOVERABLE;
		ret = -1;
	}

	return ret;
}

#endif

#if IO_SYSCALLS_HAVE_READ_R_ == 1

/**
 * \brief Read from a file.
 *
 * \param [in] filedes is the file descriptor that references an open file.
 * \param [in] buffer points to the buffer to place the read information into.
 * \param [in] size specifies the maximum number of bytes to attempt to read.
 *
 * \return -1 for failure or number of read bytes for success
 */

ssize_t _read_r(struct _reent * const r, const int filedes, void * const buffer, const size_t size)
{
	// is such file descriptor opened? does it have "read" function?
	if (openedFiledeses_[filedes].driver != NULL && openedFiledeses_[filedes].driver->_read_r != NULL)
		// yes - execute
		return openedFiledeses_[filedes].driver->_read_r(r, openedFiledeses_[filedes].privateFiledes, buffer, size);
	else
	{
		errno = EBADF;	// "The filedes argument is not a valid file descriptor, or is not open for reading."
		return -1;
	}
}

#endif

#if IO_SYSCALLS_HAVE_STAT_R_ == 1

/**
 * \brief Status of a file (by name).
 *
 * \param [in] filename points to a string that names a file.
 * \param [out] buf points to a stat structure where status information about the file is to be placed.
 *
 * \return 0 for success, -1 for failure
 */

int _stat_r(struct _reent * const r, const char * const filename, struct stat * const buf)
{
	(void)r;	// suppress warning

	// search for "filename" in _drivers[]
	size_t i = 0;
	while ((i < sizeof(drivers_) / sizeof(drivers_[0])) &&
			(strncmp(filename, drivers_[i].filename, strlen(drivers_[i].filename)) != 0))
		i++;

	if (i == sizeof(drivers_) / sizeof(drivers_[0]))	// nothing found?
	{
		errno = ENOENT;	// "The named file does not exist"
		return -1;
	}
	else
	{
		if (drivers_[i]._stat_r)
			return drivers_[i]._stat_r(r, filename, buf);
		else
		{
			buf->st_mode = S_IFCHR;
			return 0;
		}
	}
}

#endif

#if IO_SYSCALLS_HAVE_UNLINK_R_ == 1

/**
 * \brief Remove a file's directory entry.
 *
 * \param [in] filename points to the path name that names the file to be unlinked.
 *
 * \return 0 for success, -1 for failure
 */

int _unlink_r(struct _reent * const r, const char * const filename)
{
	(void)r;	// suppress warning

	// search for "filename" in _drivers[]
	size_t i = 0;
	while ((i < sizeof(drivers_) / sizeof(drivers_[0])) &&
			(strncmp(filename, drivers_[i].filename, strlen(drivers_[i].filename)) != 0))
		i++;

	if (i == sizeof(drivers_) / sizeof(drivers_[0]))	// nothing found?
	{
		errno = ENOENT;	// "The named file does not exist"
		return -1;
	}
	else
	{
		if (drivers_[i]._unlink_r)
			return drivers_[i]._unlink_r(r, filename);
		else
		{
			errno = EACCES;
			return -1;
		}
	}
}

#endif

#if IO_SYSCALLS_HAVE_WRITE_R_ == 1

/**
 * \brief Write to a file.
 *
 * \param [in] filedes is the file descriptor of an open file to write to.
 * \param [in] buffer is an array of data to write to the open file.
 * \param [in] size is the number of bytes to write to the file.
 *
 * \return -1 for failure or number of written bytes for success
 */

ssize_t _write_r(struct _reent * const r, const int filedes, const void * const buffer, const size_t size)
{
	// is such file descriptor opened? does it have "write" function?
	if (openedFiledeses_[filedes].driver != NULL && openedFiledeses_[filedes].driver->_write_r != NULL)
		// yes - execute
		return openedFiledeses_[filedes].driver->_write_r(r, openedFiledeses_[filedes].privateFiledes, buffer, size);
	else
	{
		errno = EBADF;	// "The filedes argument is not a valid filedes descriptor, or is not open for writing."
		return -1;
	}
}

#endif
