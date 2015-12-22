/**
 * \file data_consumer.hpp
 * \brief DataConsumer class header
 * \author Freddie Chopin, http://www.freddiechopin.info http://www.distortec.com
 * \author: Mazeryt Freager, http://www.gotoc.co
 */

#ifndef DATA_CONSUMER_HPP_
#define DATA_CONSUMER_HPP_

#include "FreeRTOS.h"
#include "queue.h"

#include <cstdio>

#include <memory>
#include <forward_list>

class CommandDefinition;
class Etrx2;
class Etrx2Event;
class Etrx2MessageEvent;

/// DataConsumer class is receiving data from data producers
class DataConsumer
{
public:

	/**
	 * \brief DataConsumer constructor - just sets internal variables.
	 *
	 * \param [in] etrx2 is a reference to Etrx2 object used for communication
	 */

	constexpr DataConsumer(Etrx2 &etrx2) :
			etrx2_(etrx2),
			eventQueue_(nullptr),
			producersCount_(),
			subscribeRequestsCount_(),
			transfersCount_()
	{};

	int initialize();

private:

	bool eventCallback_(std::unique_ptr<const Etrx2Event> event);

	void processEvents_(portTickType ticks_to_wait, std::forward_list<uint64_t> &producers);

	void processEvent_(const Etrx2MessageEvent &message_event, std::forward_list<uint64_t> &producers);

	void task_();

	/// Etrx2 object used for communication
	Etrx2 &etrx2_;

	/// queue for received events
	xQueueHandle eventQueue_;

	/// number of unique producers
	uint32_t producersCount_;

	/// number of subscribe requests sent
	uint32_t subscribeRequestsCount_;

	/// number of received "data:..." transfers
	uint32_t transfersCount_;

	static int consumerStatsHandler_(const char **, uint32_t, FILE * const output_stream);

	static bool eventCallbackTrampoline_(std::unique_ptr<const Etrx2Event> event);

	/**
	 * \brief Trampoline for task_()
	 *
	 * \param [in] that is a pointer to DataConsumer object
	 */

	static void trampoline_(void *that) { static_cast<DataConsumer *>(that)->task_(); };

	static const CommandDefinition consumerStatsCommandDefinition_;

	/// object used by eventCallbackTrampoline_() to call member function eventCallback_()
	static DataConsumer *dataConsumer_;
};

#endif	// DATA_CONSUMER_HPP_
