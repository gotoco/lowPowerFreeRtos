#ifndef APP_MSG_H_
#define APP_MSG_H_

typedef enum {SINGLE_VALUE, POINTER_TO_BLOCK} transfer_data_type_t;

/*---------------------------------------------------------------------------------------------------------------------+
| Common data message
+---------------------------------------------------------------------------------------------------------------------*/
#define COMMON_MESSAGE_DATA_SIZE 5

typedef enum {ACC_DATA} common_data_types_t;

typedef struct common_message_t {

	uint32_t commMessageCount;
	common_data_types_t dataType;
	int32_t data[COMMON_MESSAGE_DATA_SIZE];

}commonMessage;


/*---------------------------------------------------------------------------------------------------------------------+
| BLE Messages
+---------------------------------------------------------------------------------------------------------------------*/
#define BLE_MESSAGE_DATA_SIZE 5

typedef struct ble_message_t {

	uint32_t bleMessageCount;
	transfer_data_type_t dataType;
	int32_t bleData[BLE_MESSAGE_DATA_SIZE];

}bleMessage;


/*---------------------------------------------------------------------------------------------------------------------+
| Flash Messages
+---------------------------------------------------------------------------------------------------------------------*/
#define FLASH_MESSAGE_DATA_SIZE 5

typedef struct flash_message_t {

	uint32_t flashMessageCount;
	transfer_data_type_t dataType;
	int32_t flashData[FLASH_MESSAGE_DATA_SIZE];

}flashMessage;


#endif //APP_MSG_H_
