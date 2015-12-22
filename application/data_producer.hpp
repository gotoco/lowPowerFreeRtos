/**
 * \file data_producer.hpp
 * \brief DataProducer class header
 * \author Freddie Chopin, http://www.freddiechopin.info http://www.distortec.com
 * \author: Mazeryt Freager, http://www.gotoc.co
 */

#ifndef DATA_PRODUCER_HPP_
#define DATA_PRODUCER_HPP_

#include "FreeRTOS.h"
#include "queue.h"

#include <cstdio>

#include <memory>
#include <forward_list>

class CommandDefinition;
class Etrx2;
class Etrx2Event;
class Etrx2MessageEvent;

/// DataProducer class is just sending some random data over ZigBee
class DataProducer
{
public:

	/**
	 * \brief DataProducer constructor - just sets internal variables.
	 *
	 * \param [in] etrx2 is a reference to Etrx2 object used for communication
	 */

	constexpr DataProducer(Etrx2 &etrx2) :
			etrx2_(etrx2),
			eventQueue_(nullptr),
			measurementsCount_(),
			removalsCount_(),
			subscriptionsCount_(),
			transmissionsCount_(),
			consumerCount_()
	{};

	int initialize();

private:

	/// subscribed consumer
	struct Consumer_
	{
		/// address of consumer
		const uint64_t address;

		/// count of NACKs in a row
		uint8_t notAcknowledgedCount;
	};

	bool eventCallback_(std::unique_ptr<const Etrx2Event>);

	void processEvents_(portTickType ticks_to_wait, std::forward_list<Consumer_> &consumers);

	void processEvent_(const Etrx2MessageEvent &message_event, std::forward_list<Consumer_> &consumers);

	void task_();

	/// Etrx2 object used for communication
	Etrx2 &etrx2_;

	/// queue for received events
	xQueueHandle eventQueue_;

	/// total "measurements"
	uint32_t measurementsCount_;

	/// total removals (unsubscriptions)
	uint32_t removalsCount_;

	/// total subscriptions
	uint32_t subscriptionsCount_;

	/// total transmissions
	uint32_t transmissionsCount_;

	/// current number of consumers
	uint8_t consumerCount_;

	static bool eventCallbackTrampoline_(std::unique_ptr<const Etrx2Event> event);

	static int producerStatsHandler_(const char **, uint32_t, FILE * const output_stream);

	/**
	 * \brief Trampoline for task_()
	 *
	 * \param [in] that is a pointer to DataProducer object
	 */

	static void trampoline_(void *that) { static_cast<DataProducer *>(that)->task_(); };

	/// object used by eventCallbackTrampoline_() to call member function eventCallback_()
	static DataProducer *dataProducer_;

	static const CommandDefinition producerStatsCommandDefinition_;
};

#endif	// DATA_PRODUCER_HPP_
