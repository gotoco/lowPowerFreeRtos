#ifndef ACC_H_
#define ACC_H_


#include <stddef.h>
#include <stdlib.h>
#include <stdint.h>

#include "acc_def.h"

typedef enum {RANDOM, IMMEDIATE, SEARCH} strategy_t;

//Driver configuration

struct acc_configuration {
	const char * driver_name;

	strategy_t driver_strategy;
};

/**
/+InitDriver(bufferSize: int): int/
/+ReleaseDriver(): int/
/+SendCommand(command : char): int/
/+ReciveCommand(): char/
/+FlushBuffer(): int/
/+RedirectOutput(extBuffer: char*): int/
/+StopRedirect(): int/
 */

 struct acc_t
 {
    /**
     */
    int (* acc_Init) (struct acc_t *);
    /**
     */
    int (* acc_Release) (struct acc_t * );


    /**
     * \brief Basic function to send data to ACC.
     * It sends all data ACC via SPI. It's important to placed a Application ID, write command,
     * register offset and number of 8-bit data we send, because ACC ignore our data.
     * \param [in] Sending data
     * \param [in] Size of (data + transfer informations)
     *
     * @todo errors
     */
    inline void (* acc_WriteData) ( struct acc_t *, uint8_t* data, uint8_t number_bytes);


    /**
     * \brief Basic function to read data from ACC.
     * It receives all data from ACC via SPI. User must take care for size of buffer,
     * because ACC gives Application ID, number of bytes and needed data etc.
     * It's important to send a few bytes bigger buffer then it's needed.
     * \param [in] Buffer for receiving data
     * \param [in] Size of buffer
     * \return Pointer to table which we sent.
     *
     * @todo errors
     */
    inline uint8_t* (* acc_ReadData) ( struct acc_t *, uint8_t* data, uint8_t size );


    /**
     * \brief Send configuration to specific ACC application.
     * It sends configuration to selected register, using a Mailbox application format.
     * User doesn't have to take care about additional information for Mailbox, because
     * function is doing it for him. It only required to send a clear configuration data.
     * \param [in] Application ID to which we're sending configuration.
     * \param [in] Our configuration
     * \param [in] Size of sending configuration
     * \param [in] Register offset
     *
     * @\todo errors
     */
    void (* acc_MailboxSendConfig) ( struct acc_t *, uint8_t app_id, uint8_t* config_data, uint8_t size_config, uint8_t offset );


    /**
     * \brief Receive data from specific ACC application.
     * It sends configuration to selected register.
     * \param [in] Application ID to which we're sending configuration.
     * \param [in] Our configuration
     * \param [in] Size of sending configuration
     *
     * @\todo errors
     */
    int (* acc_MailboxReadData) ( struct acc_t *, uint8_t app_id, uint8_t *buffer, uint8_t size, uint8_t offset );
    /**
     */
    int (* acc_MailboxReadAsk) ( struct acc_t *, uint8_t app_id, uint8_t size, uint16_t offset );
    /**
     */
    int (* acc_MailboxReadConf) ( struct acc_t *, uint8_t app_id, uint8_t *buffer, uint8_t size, uint8_t offset);
    /**
     */
    int (* flush_buffer) ( struct acc_t * );
    /**
     */
    int (* redirect_output) ( struct acc_t * , char*);
    /**
     */
    int (* stop_redirect) ( struct acc_t * );

	/* private data */
    int id;
    long time_out;
    char* buffer;
    strategy_t driver_strategy;
    uint16_t poss[3];
    uint16_t frame;
    static uint8_t user_height=ACC_PEDO_DEF_HEIGHT;
    static uint8_t user_weight=ACC_PEDO_DEF_WEIGHT;
};

 enum Gender {Female, Male};


 uint16_t* 	acc_GetPoss();
 uint8_t 	acc_GetTap();
 void		acc_WakeUp();

 void		acc_InitAFE();
 void		acc_InitTap();
 void		acc_InitPedometr(uint8_t height, uint8_t weight, enum Gender Man);




 extern "C"  struct acc_t * new_driver(struct driver_configuration * );

#endif




