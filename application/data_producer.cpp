/**
 * \file data_producer.cpp
 * \brief DataProducer class implementation
 *
 * hardware: Telegesis ETRX2 module
 *
 * \author Freddie Chopin, http://www.freddiechopin.info http://www.distortec.com
 * \author: Mazeryt Freager, http://www.gotoc.co
 */

#include "data_producer.hpp"
#include "etrx2.hpp"
#include "etrx2_event.hpp"
#include "command.hpp"

#include "config.h"

#include "FreeRTOS.h"
#include "task.h"

#include <cerrno>
#include <cstdlib>
#include <cstring>
#include <cassert>

/*---------------------------------------------------------------------------------------------------------------------+
| public functions
+---------------------------------------------------------------------------------------------------------------------*/

/**
 * \brief Initializes DataProducer object.
 *
 * Creates internal task, event queue and registers event callback in Etrx2.
 *
 * \return 0 on success, negated errno code otherwise (errno not set)
 */

int DataProducer::initialize()
{
	const portBASE_TYPE ret2 = xTaskCreate(trampoline_, reinterpret_cast<const signed char *>("producer"),
			DATA_PRODUCER_TASK_STACK_SIZE, const_cast<DataProducer *>(this), DATA_PRODUCER_TASK_PRIORITY, nullptr);
	int ret = ret2 == pdPASS ? 0 : -ENOMEM;

	if (ret == 0)
	{
		eventQueue_ = xQueueCreate(DATA_PRODUCER_EVENT_QUEUE_SIZE, sizeof(const Etrx2Event *));
		ret = eventQueue_ != nullptr ? 0 : -ENOMEM;
	}

	if (ret == 0)
	{
		dataProducer_ = this;
		etrx2_.registerEventCallback(eventCallbackTrampoline_);
		ret = commandRegister(producerStatsCommandDefinition_);
	}

	return ret;
}

/*---------------------------------------------------------------------------------------------------------------------+
| private functions
+---------------------------------------------------------------------------------------------------------------------*/

/**
 * \brief Event callback.
 *
 * If event is a Etrx2MessageEvent it is sent to DataProducer::task_().
 *
 * \param [in] event is a unique_ptr to received event
 *
 * \return true is event was consumed, false otherwise
 */

bool DataProducer::eventCallback_(std::unique_ptr<const Etrx2Event> event)
{
	bool consumed = false;

	if (event->getType() == Etrx2Event::Type::MESSAGE)
	{
		const Etrx2Event * const event_ptr = event.get();
		portBASE_TYPE ret = xQueueSend(eventQueue_, &event_ptr, 0);
		if (ret == pdTRUE)	// event successfully sent?
		{
			event.release();	// unique_ptr no longer owns the memory
			consumed = true;
		}
	}

	return consumed;
}

/**
 * \brief Processes events in the eventQueue_.
 *
 * \param [in] ticks_to_wait is the amount of ticks the function should wait in xQueueReceive for the first time
 * \param [out] consumers is a reference to forward_list with consumers
 */

void DataProducer::processEvents_(portTickType ticks_to_wait, std::forward_list<Consumer_> &consumers)
{
	const Etrx2MessageEvent *message_event_ptr;

	while (xQueueReceive(eventQueue_, &message_event_ptr, ticks_to_wait) == pdTRUE)	// receive everything from the queue
	{
		ticks_to_wait = 0;	// only the first call will actually wait/block
		std::unique_ptr<const Etrx2MessageEvent> message_event(message_event_ptr);
		processEvent_(*message_event, consumers);
	}
}

/**
 * \brief Processes single event.
 *
 * If received message is "subscribe" the the sender is added to the forward_list of consumers (if not already present).
 *
 * \param [in] message_event is a reference to Etrx2MessageEvent that will be processed
 * \param [out] consumers is a reference to forward_list with consumers
 */

void DataProducer::processEvent_(const Etrx2MessageEvent &message_event, std::forward_list<Consumer_> &consumers)
{
	uint64_t address;
	const char *data;
	message_event.getParameters(nullptr, &address, nullptr, &data);
	if (strcmp(data, "subscribe") == 0)
	{
		size_t size = 0;
		for (const Consumer_ &consumer : consumers)
		{
			size++;								// count elements on the forward_list
			if (consumer.address == address)	// consumer already subscribed?
				return;
		}

		if (size < DATA_PRODUCER_MAX_SUBSCRIBERS)
		{
			consumers.push_front({address, 0});	// new consumer - add it to the forward_list
			subscriptionsCount_++;
			size++;
		}

		consumerCount_ = size;
	}
}

/**
 * \brief Task of DataProducer object.
 *
 * This task only sends random data over ZigBee.
 */

void DataProducer::task_()
{
	etrx2_.disconnect();
	etrx2_.sRegisterAccess(0x0a, "4000", "password", nullptr, 0);	// Sleepy End Device

	bool connected = false;

	while (!connected)	// connect with any PAN found
	{
		const int ret = etrx2_.connect(nullptr, nullptr, nullptr);
		if (ret == 0)
			connected = true;
		else																			// connecting failed...
			vTaskDelay(DATA_PRODUCER_CONNECTION_RETRY_DELAY * 1000 / portTICK_RATE_MS);	// ... wait and try again
	}

	std::forward_list<Consumer_> consumers;
	portTickType ticks_to_wait = portMAX_DELAY;
	portTickType last_wake_time;

	while (1)	// keep sending something
	{
		processEvents_(ticks_to_wait, consumers);

		if (!consumers.empty())	// any consumers subscribed?
		{
			if (ticks_to_wait == portMAX_DELAY)	// first subscription?
			{
				ticks_to_wait = 0;
				last_wake_time = xTaskGetTickCount();
				srand(last_wake_time);
			}

			uint8_t data[16];
			for (uint8_t &element : data)	// fill the data[] with random values
				element = rand();

			char buffer[64];	// max 5 + 16 * 3 + 1 = 54
			int ret = siprintf(buffer,
					"data:%hhx,%hhx,%hhx,%hhx,%hhx,%hhx,%hhx,%hhx,%hhx,%hhx,%hhx,%hhx,%hhx,%hhx,%hhx,%hhx",
					data[0], data[1], data[2], data[3], data[4], data[5], data[6], data[7], data[8], data[9], data[10],
					data[11], data[12], data[13], data[14], data[15]);
			assert(ret > 0);

			measurementsCount_++;

			for (Consumer_ &consumer : consumers)
			{
				bool acknowledged;
				ret = etrx2_.transmitUnicast(consumer.address, buffer, nullptr, &acknowledged);
				transmissionsCount_++;
				if (ret == 0 && acknowledged)	// transmission successful and unicast acknowledged?
					consumer.notAcknowledgedCount = 0;	// clear NACK counter
				else	// transmission failed or unicast was not acknowledged
					consumer.notAcknowledgedCount++;
			}

			// remove all consumers that have too many NACKs in a row (use lambda function for that)
			consumers.remove_if([this] (const Consumer_ &consumer)
			{
				if (consumer.notAcknowledgedCount >= DATA_PRODUCER_MAX_NACK)
				{
					consumerCount_--;
					removalsCount_++;
					return true;
				}
				else
					return false;
			} );

			vTaskDelayUntil(&last_wake_time, DATA_PRODUCER_TRANSFER_PERIOD_MS / portTICK_RATE_MS);
		}
		else
			ticks_to_wait = portMAX_DELAY;	// list of consumers is empty, so just block/wait for new subscription
	}
}

/*---------------------------------------------------------------------------------------------------------------------+
| private static functions
+---------------------------------------------------------------------------------------------------------------------*/

/// \brief Trampoline for eventCallback_() member function.

bool DataProducer::eventCallbackTrampoline_(std::unique_ptr<const Etrx2Event> event)
{
	return dataProducer_->eventCallback_(std::move(event));
}

/**
 * \brief Handler of "producer_stats" command.
 *
 * Displays DataProducer statistics.
 *
 * \param [out] output_stream is the stream used for output
 *
 * \return 0 on success, negated errno code otherwise (errno not set)
 */

int DataProducer::producerStatsHandler_(const char **, uint32_t, FILE * const output_stream)
{
	const int ret = fiprintf(output_stream, "\"Measurements\" = %lu\nTransmissions = %lu\nSubscriptions = %lu\n"
			"Removals = %lu\nConsumers = %hhu\n", dataProducer_->measurementsCount_,
			dataProducer_->transmissionsCount_, dataProducer_->subscriptionsCount_, dataProducer_->removalsCount_,
			dataProducer_->consumerCount_);
	return ret >= 0 ? 0 : -EIO;
}

/*---------------------------------------------------------------------------------------------------------------------+
| private static variables
+---------------------------------------------------------------------------------------------------------------------*/

DataProducer *DataProducer::dataProducer_;

/// definition of "producer_stats" command
const CommandDefinition DataProducer::producerStatsCommandDefinition_ =
{
		"producer_stats",		// command string
		0,						// maximum number of arguments
		DataProducer::producerStatsHandler_,	// handler function
		"producer_stats: displays DataProducer statistics\n",	// string displayed by help function
};
