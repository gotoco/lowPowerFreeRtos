#ifndef DRIVER_H_
#define DRIVER_H_



typedef enum {RANDOM, IMMEDIATE, SEARCH} strategy_t;

//Driver configuration

struct driver_configuration {
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
 struct driver_t {
    /**
     * \brief Initialization of driver
     */
    int (* init_driver) (struct driver_t * , int, int);
    /**
     * \brief Cleanup for driver
     */
    int (* release_driver) (struct driver_t ** );
    /**
     * \brief Dummy method
     */
    int (* send_command) ( struct driver_t * , char);
    /**
     * \brief Dummy method
     */
    int (* recive_command) ( struct driver_t * );
    /**
     * \brief Dummy method
     */
    int (* flush_buffer) ( struct driver_t * );

	/* private data */
    int id;
    long time_out;
    char* buffer;
    /*
     * \brief Configuration of driver
     */
    strategy_t driver_strategy;
};

 /*
  * \brief Constructor of driver
  */
 extern "C"  struct driver_t * new_driver(struct driver_configuration * );

 /*
  * \brief Getter of driver (in example when driver is global, or if we need to inject it)
  */
 extern "C"  struct driver_t * get_driver(void);

#endif




