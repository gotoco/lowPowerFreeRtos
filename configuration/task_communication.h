/**
 * \file task_communication.h
 * \brief configuration of queues and semaphores to communicate inside OS.
 *
 * This file based on RTOS and deliver flexible structure to communicate with tasks.
 *
 * project: mg-stm32l_acquisition_supervisor; chip: STM32L152RB
 *
 * \author Mazeryt Freager
 * \date 2014-07-29
 */

#ifndef TSK_COMM_H_
#define TSK_COMM_H_

#include "queue.h"
#include "semphr.h"

#define COMMON_DATA_QUEUE_LENGTH		10
#define DATA_SENDER_BLE_QUEUE_LENGTH 	5
#define DATA_SAVER_FLASH_QUEUE			5

/*---------------------------------------------------------------------------------------------------------------------+
| global variables
+---------------------------------------------------------------------------------------------------------------------*/

//cross-tasks communication
xQueueHandle dataSenderBLEQueue;
xQueueHandle dataSaverFLASHQueue;
xQueueHandle commonDataQueue;

//ISR communication
xSemaphoreHandle xSemaphoreForAccISR;

#endif //TSK_COMM_H_
