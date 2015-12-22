/**
 * \file data_consumer.cpp
 * \brief DataConsumer class implementation
 *
 * hardware: Telegesis ETRX2 module
 *
 * \author Freddie Chopin, http://www.freddiechopin.info http://www.distortec.com
 * \author: Mazeryt Freager, http://www.gotoc.co
 */

#include "data_consumer.hpp"
#include "etrx2.hpp"
#include "etrx2_event.hpp"
#include "command.hpp"

#include "config.h"

#include "FreeRTOS.h"
#include "task.h"

#include <cerrno>
#include <cstring>

/*---------------------------------------------------------------------------------------------------------------------+
| public functions
+---------------------------------------------------------------------------------------------------------------------*/

/**
 * \brief Initializes DataConsumer object.
 *
 * Creates internal task, event queue and registers event callback in Etrx2.
 *
 * \return 0 on success, negated errno code otherwise (errno not set)
 */

int DataConsumer::initialize()
{
	const portBASE_TYPE ret2 = xTaskCreate(trampoline_, reinterpret_cast<const signed char *>("consumer"),
			DATA_CONSUMER_TASK_STACK_SIZE, this, DATA_CONSUMER_TASK_PRIORITY, nullptr);
	int ret = ret2 == pdPASS ? 0 : -ENOMEM;

	if (ret == 0)
	{
		eventQueue_ = xQueueCreate(DATA_CONSUMER_EVENT_QUEUE_SIZE, sizeof(const Etrx2Event *));
		ret = eventQueue_ != nullptr ? 0 : -ENOMEM;
	}

	if (ret == 0)
	{
		dataConsumer_ = this;
		etrx2_.registerEventCallback(eventCallbackTrampoline_);
		ret = commandRegister(consumerStatsCommandDefinition_);
	}

	return ret;
}

/*---------------------------------------------------------------------------------------------------------------------+
| private functions
+---------------------------------------------------------------------------------------------------------------------*/

/**
 * \brief Event callback.
 *
 * If event is a Etrx2MessageEvent it is sent to DataConsumer::task_().
 *
 * \param [in] event is a unique_ptr to received event
 *
 * \return true is event was consumed, false otherwise
 */

bool DataConsumer::eventCallback_(std::unique_ptr<const Etrx2Event> event)
{
	bool consumed = false;

	if (event->getType() == Etrx2Event::Type::MESSAGE)
	{
		const Etrx2Event * const event_ptr = event.get();
		const portBASE_TYPE ret = xQueueSend(eventQueue_, &event_ptr, 0);
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
 * Retrieves the events from the event queue and passes each to processEvent_().
 *
 * \param [in] ticks_to_wait is the amount of ticks the function should wait in xQueueReceive for the first time
 * \param [in, out] producers is a reference to forward_list with addresses of producers
 */

void DataConsumer::processEvents_(portTickType ticks_to_wait, std::forward_list<uint64_t> &producers)
{
	const Etrx2MessageEvent *message_event_ptr;

	while (xQueueReceive(eventQueue_, &message_event_ptr, ticks_to_wait) == pdTRUE)	// receive everything from the queue
	{
		ticks_to_wait = 0;	// only the first call will actually wait/block
		std::unique_ptr<const Etrx2MessageEvent> message_event(message_event_ptr);
		processEvent_(*message_event, producers);
	}
}

/**
 * \brief Processes single event.
 *
 * Just counts the "data:..." transfers and adds the addresses to the forward_list (if not already present).
 *
 * \param [in] message_event is a reference to received Etrx2MessageEvent
 * \param [in, out] producers is a reference to forward_list with addresses of producers
 */

void DataConsumer::processEvent_(const Etrx2MessageEvent &message_event, std::forward_list<uint64_t> &producers)
{
	uint64_t address;
	const char *data;
	message_event.getParameters(nullptr, &address, nullptr, &data);
	if (strncmp(data, "data:", strlen("data:")) == 0)
	{
		transfersCount_++;

		size_t size = 0;
		for (const uint64_t &producer : producers)
		{
			size++;						// count elements on the forward_list
			if (address == producer)	// address already on the forward_list?
				return;
		}

		if (size < DATA_CONSUMER_MAX_PRODUCERS)
		{
			producers.push_front(address);	// new address - add it to the forward_list
			size++;
		}

		producersCount_ = size;
	}
}

/**
 * \brief Task of DataConsumer object.
 *
 * This task receives data from data producers.
 */

void DataConsumer::task_()
{
	etrx2_.disconnect();
	etrx2_.sRegisterAccess(0x0a, "0000", "password", nullptr, 0);	// Coordinator / Router

	std::vector<Etrx2::FoundNetwork> found_networks;
	found_networks.reserve(16);

	while (etrx2_.searchNetworks(found_networks) != 0)
		found_networks.clear();

	bool connected = false;

	while (!connected)
	{
		const int ret = found_networks.size() == 0 ? etrx2_.establishNetwork(nullptr, nullptr, nullptr) :
				etrx2_.connect(nullptr, nullptr, nullptr);
		if (ret == 0)
			connected = true;
	}

	const portTickType subscribe_period_ticks = DATA_CONSUMER_SUBSCRIBE_PERIOD * 1000 / portTICK_RATE_MS;
	portTickType deadline = xTaskGetTickCount() + subscribe_period_ticks;
	std::forward_list<uint64_t> producers;

	while (1)
	{
		etrx2_.transmitBroadcast(0, "subscribe");	// this will be sent every subscribe_period_ticks
		subscribeRequestsCount_++;

		portTickType now;
		while ((now = xTaskGetTickCount()) < deadline)	// in the meantime process incoming events
			processEvents_(deadline - now, producers);

		deadline += subscribe_period_ticks;
	}
}

/*---------------------------------------------------------------------------------------------------------------------+
| private static functions
+---------------------------------------------------------------------------------------------------------------------*/

/**
 * \brief Handler of "consumer_stats" command.
 *
 * Displays DataConsumer statistics.
 *
 * \param [out] output_stream is the stream used for output
 *
 * \return 0 on success, negated errno code otherwise (errno not set)
 */

int DataConsumer::consumerStatsHandler_(const char **, uint32_t, FILE * const output_stream)
{
	const int ret = fiprintf(output_stream, "Subscribe requests = %lu\nReceived \"data:...\" transfers = %lu\n"
			"Unique producers = %lu\n", dataConsumer_->subscribeRequestsCount_, dataConsumer_->transfersCount_,
			dataConsumer_->producersCount_);
	return ret >= 0 ? 0 : -EIO;
}

/// \brief Trampoline for eventCallback_() member function.

bool DataConsumer::eventCallbackTrampoline_(std::unique_ptr<const Etrx2Event> event)
{
	return dataConsumer_->eventCallback_(std::move(event));
}

/*---------------------------------------------------------------------------------------------------------------------+
| private static variables
+---------------------------------------------------------------------------------------------------------------------*/

DataConsumer *DataConsumer::dataConsumer_;

/// definition of "consumer_stats" command
const CommandDefinition DataConsumer::consumerStatsCommandDefinition_ =
{
		"consumer_stats",		// command string
		0,						// maximum number of arguments
		DataConsumer::consumerStatsHandler_,	// handler function
		"consumer_stats: displays DataConsumer statistics\n",	// string displayed by help function
};
