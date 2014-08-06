#ifndef ACC_H_
#define ACC_H_

/**
 * \brief Driver for ACC - MMA955xL
 *
 * Complete driver for accelerometer which support a Mailbox communication via SPI.
 *
 * \author DuMaM
 * \date 04.08.2014
 * \version 0.1
 * \copyright uBirds
 * \warning Undone.
 */


typedef enum {RANDOM, IMMEDIATE, SEARCH} strategy_t; // \todo Do czego ten typ?
typedef enum {Female, Male} Gender_t;


//Driver configuration

struct acc_config_t
{
	const char * driver_name;
	Gender_t Gender;
	uint8_t user_height;
	uint8_t user_weight;
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
	/* private data */
    int id;
    strategy_t driver_strategy;
    uint16_t frame;

    /* publice data */
	static Gender_t Gender;
    static uint8_t user_height;
    static uint8_t user_weight;
    static uint16_t position[3];
    static uint8_t tap[2];

    /**
     * \brief Initiate and configure a ACC.
     * It sends settings to ACC and configure proper peripherals.
     * \param [in] Mode in which ACC will be ruing (pedometr or gesture measure)
     * \param [in] Structure which store data about user to proper set ACC.
     *
     * @todo sprawdzic konfiguracje dla pedometru czy jest poprawna, ogolnie dopiescic
     */
    void (* acc_Init) (struct acc_t *, uint8_t work_mode, acc_config_t *config);


     /**
     */
    int (* acc_Release) (struct acc_t * );


    /**
     * \brief Basic function to send data to ACC.
     * It sends all data ACC via SPI. It's important to placed a Application ID, write command,
     * register offset and number of 8-bit data we send, because ACC ignore our data.
     *
     * \param [in] Sending data
     * \param [in] Size of (data + transfer informations)
     *
     * @todo errors
     */
    void (* acc_WriteData) ( struct acc_t *, uint8_t* data, uint8_t number_bytes);


    /**
     * \brief Basic function to read data from ACC.
     * It receives all data from ACC via SPI. User must take care for size of buffer,
     * because ACC gives Application ID, number of bytes and needed data etc.
     * It's important to send a few bytes bigger buffer(4) then it's needed.
     *
     * \param [in] Buffer for receiving data
     * \param [in] Size of buffer
     * \return Pointer to table which we sent.
     *
     * @todo errors
     */
    uint8_t* (* acc_ReadData) ( struct acc_t *, uint8_t* data, uint8_t size );


    /**
     * \brief Send configuration to specific ACC application.
     * It sends configuration to selected register, using a Mailbox application format.
     * User doesn't have to take care about additional information for Mailbox, because
     * function is doing it for him. It only required to send a clear configuration data.
     *
     * \param [in] Application ID to which we're sending configuration.
     * \param [in] Our configuration
     * \param [in] Size of sending configuration
     * \param [in] Reading register offset
     *
     * @todo errors
     */
    void (* acc_MailboxSendConfig) ( struct acc_t *, uint8_t app_id, uint8_t* config_data, uint8_t size_config, uint8_t offset );


    /**
     * \brief Receive data from specific ACC application.
     * It sends read data command to ACC and prepares clear data by itself.
     * User sends only information about registers from which he want to read data.
     *
     * \param [in] Application ID from which we want read data.
     * \param [in] Buffer for our data
     * \param [in] Size of buffer
     * \param [in] Reading register offset
     *
     * \return Pointer to our buffer
     * @todo errors
     * @todo opoznienie w odczycie, najlepiej na przerwaniach licznika, b¹dŸ ustawienie
     * przerwania od ACC, które œwiadczy o gotowoœci danych
     *
     */
    uint8_t* (* acc_MailboxReadData) ( struct acc_t *, uint8_t app_id, uint8_t *buffer, uint8_t size, uint8_t offset );


    /**
     * \brief Ask for data from specific ACC application.
     * It sends read command to ACC. User sends information about registers
     * from which he want to read data.
     *
     * \param [in] Application ID from which we want read data.
     * \param [in] Amount of data we want to read
     * \param [in] Reading register offset
     *
     * @todo errors
     */
    void (* acc_MailboxReadAsk) ( struct acc_t *, uint8_t app_id, uint8_t amount, uint16_t offset );

	/**
		* \brief Ask for data from specific ACC application.
		* It sends read command to ACC. User sends information about registers
		* from which he want to read data.
		*
		* \param [in] Application ID from which we want read data.
		* \param [in] Amount of data we want to read
		* \param [in] Reading register offset
		*
		* @todo errors
		*/
    void( *acc_MailboxConfAsk) (struct acc_t *self, uint8_t app_id, uint8_t size, uint16_t offset);

   /**
     * \brief Read configuration from specific ACC application register.
     * It sends read config command to ACC. User sends information about registers
     * from which he want to read data. Buffer got only data from config so user
     * doesn't have to take care about informations which ACC returns
     * during transmission.
     *
     * \param [in] Application ID from which we want read configuration.
     * \param [in] Buffer for configuration
     * \param [in] Amount of data we want to read
     * \param [in] Reading register offset
     *
     * @todo errors
     */
    void (* acc_MailboxReadConf) ( struct acc_t *, uint8_t app_id, uint8_t *buffer, uint8_t size, uint8_t offset);


    /**
     * \brief Wakes up ACC.
     * It's waking up ACC, from auto sleeping mode. It is need to be send, before
     * any actions. ACC can auto-wake up only if it registers a motion and defined g-level.
     */
    void (* acc_WakeUp) ( struct acc_t * );


    /**
     * \brief Gives a XYZ data.
     * It communicate with ACC and receives position data from AFE
     *
     * \return Pointer to position table from driver structure. Data in table are in format {X, Y, Z}
     */
    uint16_t*  (* acc_GetPoss) ( struct acc_t * );


    /**
     * \brief Detect single and double tap.
     * It communicate with ACC and receives data from Tap Application.
     * Format single byte:
     * __   __   __   __   __   __   __   __
     * TAP  —    ZDir ZEv  YDir YEv  XDir XEv
     *
     * TAP - tap detected
     * Dir - direction of tap (0 positive, 1 negative)
     * EV  - reports whether a axis event has been detected
     *
     * \return Pointer to table where 0 cell are single tap bytes, 1 cell are double tap bytes
     * @todo sprawdzic czy dziala poprawnie
     */
    uint8_t  (* acc_GetTap) ( struct acc_t * );


    /**
     *
     *
     *
     */
    int (* flush_buffer) ( struct acc_t * );


    /**
     */
    int (* redirect_output) ( struct acc_t * , char*);
    /**
     *
     */
    int (* stop_redirect) ( struct acc_t * );



};


 extern "C"  struct acc_t * new_acc_driver(struct acc_config_t * );

#endif




