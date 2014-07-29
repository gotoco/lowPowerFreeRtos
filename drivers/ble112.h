/**
 * \file ble112.h
 * \brief Dummy functions set to communicate with BLE112 module
 *
 * Functions for BLE112 external module
 *
 * chip: STM32L1xx; external
 *
 * \author Mazeryt Freager
 * \date 2014-07-08
 */

#ifndef BLE112_H_
#define BLE112_H_

#include "error.h"
#include "usart.h"

/*---------------------------------------------------------------------------------------------------------------------+
| global functions' prototypes
+---------------------------------------------------------------------------------------------------------------------*/

/**
 *
/+InitDriver(bufferSize: int): int/
/+ReleaseDriver(): int/
/+SendCommand(command : char): int/
/+ReciveCommand(): char/
/+FlushBuffer(): int/
/+RedirectOutput(extBuffer: char*): int/
/+StopRedirect(): int/
 */

 struct ble112_ATdriver__vtable {
    /**
     */
    int (* init_driver) (int, int);
    /**
     */
    int (* release_driver) ();
    /**
     */
    int (* send_command) (char);
    /**
     */
    int (* recive_command) ();
    /**
     */
    int (* flush_buffer) ();
    /**
     */
    int (* redirect_output) (char*);
    /**
     */
    int (* stop_redirect) ();

	/* private data */
    int id;
    long time_out;
    char* buffer;
};

struct ble112_ATdriver__vtable * new_driver(void);

#endif /* BLE112_H_ */
