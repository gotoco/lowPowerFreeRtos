/*
    FreeRTOS V7.2.0 - Copyright (C) 2012 Real Time Engineers Ltd.
	

    ***************************************************************************
     *                                                                       *
     *    FreeRTOS tutorial books are available in pdf and paperback.        *
     *    Complete, revised, and edited pdf reference manuals are also       *
     *    available.                                                         *
     *                                                                       *
     *    Purchasing FreeRTOS documentation will not only help you, by       *
     *    ensuring you get running as quickly as possible and with an        *
     *    in-depth knowledge of how to use FreeRTOS, it will also help       *
     *    the FreeRTOS project to continue with its mission of providing     *
     *    professional grade, cross platform, de facto standard solutions    *
     *    for microcontrollers - completely free of charge!                  *
     *                                                                       *
     *    >>> See http://www.FreeRTOS.org/Documentation for details. <<<     *
     *                                                                       *
     *    Thank you for using FreeRTOS, and thank you for your support!      *
     *                                                                       *
    ***************************************************************************


    This file is part of the FreeRTOS distribution.

    FreeRTOS is free software; you can redistribute it and/or modify it under
    the terms of the GNU General Public License (version 2) as published by the
    Free Software Foundation AND MODIFIED BY the FreeRTOS exception.
    >>>NOTE<<< The modification to the GPL is included to allow you to
    distribute a combined work that includes FreeRTOS without being obliged to
    provide the source code for proprietary components outside of the FreeRTOS
    kernel.  FreeRTOS is distributed in the hope that it will be useful, but
    WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
    or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
    more details. You should have received a copy of the GNU General Public
    License and the FreeRTOS license exception along with FreeRTOS; if not it
    can be viewed here: http://www.freertos.org/a00114.html and also obtained
    by writing to Richard Barry, contact details for whom are available on the
    FreeRTOS WEB site.

    1 tab == 4 spaces!
    
    ***************************************************************************
     *                                                                       *
     *    Having a problem?  Start by reading the FAQ "My application does   *
     *    not run, what could be wrong?                                      *
     *                                                                       *
     *    http://www.FreeRTOS.org/FAQHelp.html                               *
     *                                                                       *
    ***************************************************************************

    
    http://www.FreeRTOS.org - Documentation, training, latest information, 
    license and contact details.
    
    http://www.FreeRTOS.org/plus - A selection of FreeRTOS ecosystem products,
    including FreeRTOS+Trace - an indispensable productivity tool.

    Real Time Engineers ltd license FreeRTOS to High Integrity Systems, who sell 
    the code with commercial support, indemnification, and middleware, under 
    the OpenRTOS brand: http://www.OpenRTOS.com.  High Integrity Systems also
    provide a safety engineered and independently SIL3 certified version under 
    the SafeRTOS brand: http://www.SafeRTOS.com.
*/

#ifndef FREERTOS_CONFIG_H
#define FREERTOS_CONFIG_H

// needed for __NVIC_PRIO_BITS
#include "stm32l1xx.h"

// needed for rccGetCoreFrequency()
#include "rcc.h"

// needed for runtimestats
#include "helper.h"

/*-----------------------------------------------------------
 * Application specific definitions.
 *
 * These definitions should be adjusted for your particular hardware and
 * application requirements.
 *
 * THESE PARAMETERS ARE DESCRIBED WITHIN THE 'CONFIGURATION' SECTION OF THE
 * FreeRTOS API DOCUMENTATION AVAILABLE ON THE FreeRTOS.org WEB SITE.
 *
 * See http://www.freertos.org/a00110.html.
 *----------------------------------------------------------*/

#define configUSE_PREEMPTION			1
#define configUSE_IDLE_HOOK				0
#define configUSE_TICK_HOOK				0
#define configCPU_CLOCK_HZ				rccGetCoreFrequency()
#define configTICK_RATE_HZ				( ( portTickType ) 1000 )
#define configMAX_PRIORITIES			( ( unsigned portBASE_TYPE ) 5 )
#define configMINIMAL_STACK_SIZE		( ( unsigned short ) 64 )
#define configMAX_TASK_NAME_LEN			12
#define configUSE_TRACE_FACILITY		1
#define configUSE_16_BIT_TICKS			0
#define configIDLE_SHOULD_YIELD			1
#define configUSE_MUTEXES				1
#define configQUEUE_REGISTRY_SIZE		5
#define configGENERATE_RUN_TIME_STATS	1
#define configCHECK_FOR_STACK_OVERFLOW	0
#define configUSE_RECURSIVE_MUTEXES		0
#define configUSE_MALLOC_FAILED_HOOK	0
#define configUSE_APPLICATION_TASK_TAG	0

/* Co-routine definitions. */
#define configUSE_CO_ROUTINES 		0
#define configMAX_CO_ROUTINE_PRIORITIES ( 2 )

/* Set the following definitions to 1 to include the API function, or zero
to exclude the API function. */

#define INCLUDE_vTaskPrioritySet		1
#define INCLUDE_uxTaskPriorityGet		1
#define INCLUDE_vTaskDelete				1
#define INCLUDE_vTaskCleanUpResources	0
#define INCLUDE_vTaskSuspend			1
#define INCLUDE_vTaskDelayUntil			1
#define INCLUDE_vTaskDelay				1
#define INCLUDE_xTaskGetSchedulerState  1

/* Use the system definition, if there is one */
#ifdef __NVIC_PRIO_BITS
	#define configPRIO_BITS       __NVIC_PRIO_BITS
#else
	#define configPRIO_BITS       4        /* 15 priority levels */
#endif

#define configLIBRARY_LOWEST_INTERRUPT_PRIORITY			15
#define configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY	5

/* The lowest priority. */
#define configKERNEL_INTERRUPT_PRIORITY 	( configLIBRARY_LOWEST_INTERRUPT_PRIORITY << (8 - configPRIO_BITS) )
/* Priority 5, or 95 as only the top four bits are implemented. */
/* !!!! configMAX_SYSCALL_INTERRUPT_PRIORITY must not be set to zero !!!!
See http://www.FreeRTOS.org/RTOS-Cortex-M3-M4.html. */
#define configMAX_SYSCALL_INTERRUPT_PRIORITY 	( configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY << (8 - configPRIO_BITS) )

/*---------------------------------------------------------------------------------------------------------------------+
| Assign FreeRTOS kernel functions to ISRs
+---------------------------------------------------------------------------------------------------------------------*/

#define vPortSVCHandler						SVC_Handler
#define xPortPendSVHandler					PendSV_Handler
//#define xPortSysTickHandler					SysTick_Handler //commented because of usage ST_HAL libraries

/*---------------------------------------------------------------------------------------------------------------------+
| Priorities and stacks for tasks in the system
+---------------------------------------------------------------------------------------------------------------------*/

// heartbeat task
#define HEARTBEAT_TASK_PRIORITY				tskIDLE_PRIORITY
#define HEARTBEAT_STACK_SIZE				256

// USART TX task
#define USART_TX_TASK_PRIORITY				(tskIDLE_PRIORITY + 1)
#define USART_TX_STACK_SIZE					256

// USART RX task
#define USART_RX_TASK_PRIORITY				(tskIDLE_PRIORITY + 1)
#define USART_RX_STACK_SIZE					256

// i2c tasks
#define I2C_TASK_PRIORITY					(tskIDLE_PRIORITY + 1)
#define I2C_TASK_STACK_SIZE					128

// SPI TX task
#define SPI_TX_TASK_PRIORITY				(tskIDLE_PRIORITY + 1)
#define SPI_TX_STACK_SIZE					128

/// priority of console task
enum { CONSOLE_TASK_PRIORITY = 1 };

/// stack size of console task, words (4 bytes each)
enum { CONSOLE_TASK_STACK_SIZE = 512 };

/// priority of DataConsumer task
enum { DATA_CONSUMER_TASK_PRIORITY = 1 };

/// stack size of DataConsumer task, words (4 bytes each)
enum { DATA_CONSUMER_TASK_STACK_SIZE = 512 };

/// priority of DataProducer task
enum { DATA_PRODUCER_TASK_PRIORITY = 1 };

/// stack size of DataProducer task, words (4 bytes each)
enum { DATA_PRODUCER_TASK_STACK_SIZE = 512 };

/*---------------------------------------------------------------------------------------------------------------------+
| UARTs
+---------------------------------------------------------------------------------------------------------------------*/

/// size of FreeRTOS queue (in chars) for UART
enum { UART_QUEUE_SIZE = 128 };

/// priority of UART's interrupts
#define UART_INTERRUPT_PRIORITY		configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY

/*---------------------------------------------------------------------------------------------------------------------+
| UART stream
+---------------------------------------------------------------------------------------------------------------------*/

/// default UART baudrate used when opening device
enum { UART_STREAM_BAUDRATE = 19200 };

/*---------------------------------------------------------------------------------------------------------------------+
| I/O syscalls
+---------------------------------------------------------------------------------------------------------------------*/

/// how long should open and close block on mutex operations (in RTOS ticks)
#define	STREAM_MUTEX_TICKS_TO_WAIT			portMAX_DELAY

/*---------------------------------------------------------------------------------------------------------------------+
| console
+---------------------------------------------------------------------------------------------------------------------*/

/// size of input buffer for console task
enum { CONSOLE_INPUT_BUFFER_SIZE = 128 };

/*---------------------------------------------------------------------------------------------------------------------+
| UART assert
+---------------------------------------------------------------------------------------------------------------------*/

/// default UART baudrate used for assert()
enum { UART_ASSERT_BAUDRATE = UART_STREAM_BAUDRATE };

/*---------------------------------------------------------------------------------------------------------------------+
| ETRX2
+---------------------------------------------------------------------------------------------------------------------*/

/// size of buffer for rx task of ETRX2 module
enum { ETRX2_BUFFER_SIZE = 128 };

/*---------------------------------------------------------------------------------------------------------------------+
| streams
+---------------------------------------------------------------------------------------------------------------------*/

/// size of buffer used for stdio streams
enum { STREAM_BUFFER_SIZE = 128 };

/*---------------------------------------------------------------------------------------------------------------------+
| DataConsumer
+---------------------------------------------------------------------------------------------------------------------*/

/// size of event queue (number of elements)
enum { DATA_CONSUMER_EVENT_QUEUE_SIZE = 16 };

/// max number of producers on the forward_list
enum { DATA_CONSUMER_MAX_PRODUCERS = 16 };

/// period of subscribe broadcasts, seconds
enum { DATA_CONSUMER_SUBSCRIBE_PERIOD = 60 };

/*---------------------------------------------------------------------------------------------------------------------+
| DataProducer
+---------------------------------------------------------------------------------------------------------------------*/

/// delay of connection retry, seconds
enum { DATA_PRODUCER_CONNECTION_RETRY_DELAY = 10 };

/// size of event queue (number of elements)
enum { DATA_PRODUCER_EVENT_QUEUE_SIZE = 16 };

/// max number of NACKs in a row
enum { DATA_PRODUCER_MAX_NACK = 3 };

/// max number of subscribers
enum { DATA_PRODUCER_MAX_SUBSCRIBERS = 16 };

/// period of transfers, milliseconds
enum { DATA_PRODUCER_TRANSFER_PERIOD_MS = 500 };

/*---------------------------------------------------------------------------------------------------------------------+
| ETRX
+---------------------------------------------------------------------------------------------------------------------*/

/// priority of ETRX2 rx task
enum { ETRX2_RX_TASK_PRIORITY = 1 };

/// stack size of ETRX2 rx task, words (4 bytes each)
enum { ETRX2_RX_TASK_STACK_SIZE = 512 };

/*---------------------------------------------------------------------------------------------------------------------+
| Runtime stats configuration
+---------------------------------------------------------------------------------------------------------------------*/

#define portCONFIGURE_TIMER_FOR_RUN_TIME_STATS()	configureTimerForRuntimestats()
#define portGET_RUN_TIME_COUNTER_VALUE()	((tim6OverflowCount << 16) | TIM6->CNT)

#endif /* FREERTOS_CONFIG_H */

