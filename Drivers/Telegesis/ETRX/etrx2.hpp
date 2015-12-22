/**
 * \file etrx2.hpp
 * \brief Etrx2 class header
 * \author Freddie Chopin, http://www.freddiechopin.info http://www.distortec.com
 * \author: Mazeryt Freager, http://www.gotoc.co
 */

#ifndef ETRX2_HPP_
#define ETRX2_HPP_

#include "FreeRTOS.h"
#include "queue.h"
#include "semphr.h"

#include <cstdio>
#include <cstdint>

#include <memory>
#include <vector>

class Etrx2Event;
class Etrx2Request;

/// Etrx2 class is an abstraction over Telegesis ETRX2 module
class Etrx2
{
public:

	/// energy on channel
	struct ChannelEnergy
	{
		uint8_t channel;	///< channel number, [11; 26]
		uint8_t energy;		///< average energy
	};

	/// network function of the device
	enum class NetworkFunction
	{
		NONE,		///< none, device is not connected to any network, thus has no function
		COO,		///< "Coordinator"
		FFD,		///< "Router"
		MED,		///< "Mobile Sleepy End Device"
		SED,		///< "Sleepy End Device"
		ZED,		///< "End Device"
	};

	/// ZigBee Stack Profile
	enum class ZigbeeStackProfile
	{
		CUSTOM,			///< Custom
		ZIGBEE,			///< ZigBee
		ZIGBEE_PRO,		///< ZigBee PRO
	};

	/// single result of search for active networks
	struct FoundNetwork
	{
		uint64_t epid;								///< extended PAN ID
		uint16_t pid;								///< PAN ID
		uint8_t channel;							///< channel
		bool joiningPermitted;						///< true if network is allowing additional nodes to join
		ZigbeeStackProfile zigbeeStackProfile;		///< ZigBee Stack Profile
	};

	/// callback function for received Etrx2Event object
	typedef bool (*EventCallback)(std::unique_ptr<const Etrx2Event>);

	/**
	 * \brief Etrx2 constructor - just sets internal variables.
	 *
	 * \param [in] rx_stream is a pointer to FILE object used for receiving from ETRX2 module
	 * \param [in] tx_stream is a pointer to FILE object used for transmitting to ETRX2 module
	 */

	constexpr Etrx2(FILE * const rx_stream, FILE * const tx_stream) :
			eventCallback_(nullptr),
			mutex_(),
			requestQueue_(),
			request_(),
			droppedLines_(),
			processedCommands_(),
			receivedLines_(),
			rxStream_(rx_stream),
			txStream_(tx_stream)
	{};

	int connect(uint8_t * const channel, uint16_t * const pid, uint64_t * const epid);

	int disconnect();

	int establishNetwork(uint8_t * const channel, uint16_t * const pid, uint64_t * const epid);

	int getModuleInfo(char * const device_name, const size_t device_name_size, char * const firmware_revision,
			const size_t firmware_revision_size, uint64_t * const eui64);

	/**
	 * \brief Gets module statistics.
	 *
	 * \param [out] processed_commands is a reference to variable which will hold the number of processed commands
	 * \param [out] received_lines is a reference to variable which will hold the number of received lines
	 * \param [out] dropped_lines is a reference to variable which will hold the number of dropped lines
	 */

	void getModuleStats(uint32_t &processed_commands, uint32_t &received_lines, uint32_t &dropped_lines) const
	{
		processed_commands = processedCommands_;
		received_lines = receivedLines_;
		dropped_lines = droppedLines_;
	}

	int getNetworkInfo(uint64_t * const epid, uint16_t * const pid, NetworkFunction * const network_function,
			uint8_t * const channel, uint8_t * const power);

	int initialize();

	/// \brief registers event callback
	void registerEventCallback(const EventCallback event_callback) { eventCallback_ = event_callback; };

	int scanEnergy(std::vector<Etrx2::ChannelEnergy> &channel_energies);

	int searchNetworks(std::vector<Etrx2::FoundNetwork> &found_networks);

	int sRegisterAccess(const uint8_t s_register, const char * const write_data, const char * const password,
			char * const read_data, const size_t read_data_size);

	int transmitBroadcast(const uint8_t hops, const char * const data);

	int transmitUnicast(const uint64_t address, const char * const data, uint8_t * const sequence_number,
			bool * const acknowledged);

private:

	int handle_(std::unique_ptr<Etrx2Request> request);

	void rxTask_();

	/// callback function for received Etrx2Event object
	EventCallback eventCallback_;

	/// mutex used for serializing access to request handling
	xSemaphoreHandle mutex_;

	/// queue used to signal end of request handling and pass return value
	xQueueHandle requestQueue_;

	/// currently processed Etrx2Request, that's a pointer so that etrx2_parser.hpp does not need to be included
	std::unique_ptr<Etrx2Request> *request_;

	/// number of received lines that were dropped/ignored
	uint32_t droppedLines_;

	/// number of successfully processed commands
	uint32_t processedCommands_;

	/// number of received non-empty lines
	uint32_t receivedLines_;

	/// pointer to FILE object used for receiving from ETRX2 module
	FILE * const rxStream_;

	/// pointer to FILE object used for transmitting to ETRX2 module
	FILE * const txStream_;

	/**
	 * \brief Trampoline for rxTask_()
	 *
	 * \param [in] that is a pointer to Etrx2 object
	 */

	static void rxTrampoline_(void *that) { static_cast<Etrx2 *>(that)->rxTask_(); };

	static void trimTrailingCharacters(char *string);
};

#endif	// ETRX2_HPP_
