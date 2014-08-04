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
 struct acc_t {
    /**
     */
    int (* acc_init) (struct acc_t * , int, int);
    /**
     */
    int (* acc_release) (struct acc_t * );
    /**
     */
    inline void (* acc_WriteData) ( struct acc_t *, uint8_t* data, uint8_t number_bytes);
    /**
     */
    inline uint8_t* (* acc_ReadData) ( struct acc_t *, int8_t* data, uint8_t number_bytes );
    /**
     */
    int (* acc_MailboxSendConfig) ( struct acc_t *, uint8_t app_id, uint8_t* config_data, uint8_t size_config, uint8_t offset );
    /**
     */
    int (* acc_MailboxReadData) ( struct acc_t *, uint8_t app_id, uint8_t *buffor, uint8_t size, uint8_t offset );
    /**
     */
    int (* acc_MailboxReadAsk) ( struct acc_t *, uint8_t app_id, uint8_t size, uint16_t offset );
    /**
     */
    int (* acc_MailboxReadConf) ( struct acc_t *, uint8_t app_id, uint8_t *buffor, uint8_t size, uint8_t offset);
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




 void 		acc_MailboxSendConfig(uint8_t app_id, uint8_t* config_data, uint8_t size_config, uint8_t offset);
 void		acc_MailboxReadData(uint8_t app_id, uint8_t *buffor, uint8_t size, uint8_t offset);
 void		acc_MailboxReadAsk(uint8_t app_id, uint8_t size, uint16_t offset);
 void		acc_MailboxReadConf(uint8_t app_id, uint8_t *buffor, uint8_t size, uint8_t offset);

 uint16_t* 	acc_GetPoss();
 uint8_t 	acc_GetTap();
 void		acc_WakeUp();

 void		acc_InitAFE();
 void		acc_InitTap();
 void		acc_InitPedometr(uint8_t height, uint8_t weight, enum Gender Man);




 extern "C"  struct acc_t * new_driver(struct driver_configuration * );

#endif




