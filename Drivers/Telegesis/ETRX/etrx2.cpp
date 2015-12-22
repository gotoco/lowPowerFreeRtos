/**
 * \file etrx2.cpp
 * \brief Etrx2 class implementation
 *
 * hardware: Telegesis ETRX2 module
 *
 * \author Freddie Chopin, http://www.freddiechopin.info http://www.distortec.com
 * \author: Mazeryt Freager, http://www.gotoc.co
 */

#include "etrx2.hpp"
#include "etrx2_parser.hpp"
#include "etrx2_event.hpp"

#include "config.h"

#include "task.h"

#include <cerrno>
#include <cstring>
#include <cassert>

/*---------------------------------------------------------------------------------------------------------------------+
| public methods
+---------------------------------------------------------------------------------------------------------------------*/

/**
 * \brief Connects local node to any network.
 *
 * \param [out] channel is a pointer to variable which will hold the channel, nullptr if not used
 * \param [out] pid is a pointer to variable which will hold the PID number, nullptr if not used
 * \param [out] epid is a pointer to variable which will hold the EPID number, nullptr if not used
 *
 * \return 0 on success, negated value on handling error or error code returned by ETRX2 module (positive value)
 */

int Etrx2::connect(uint8_t * const channel, uint16_t * const pid, uint64_t * const epid)
{
	std::unique_ptr<Etrx2Request> request = Etrx2Request::createAt_jnRequest(requestQueue_, channel, pid, epid);
	return handle_(std::move(request));
}

/**
 * \brief Disconnects local node from network.
 *
 * \return 0 on success, negated value on handling error or error code returned by ETRX2 module (positive value)
 */

int Etrx2::disconnect()
{
	std::unique_ptr<Etrx2Request> request = Etrx2Request::createAt_dasslRequest_(requestQueue_);
	return handle_(std::move(request));
}

/**
 * \brief Establishes network.
 *
 * \param [out] channel is a pointer to variable which will hold the channel, nullptr if not used
 * \param [out] pid is a pointer to variable which will hold the PID number, nullptr if not used
 * \param [out] epid is a pointer to variable which will hold the EPID number, nullptr if not used
 *
 * \return 0 on success, negated value on handling error or error code returned by ETRX2 module (positive value)
 */

int Etrx2::establishNetwork(uint8_t * const channel, uint16_t * const pid, uint64_t * const epid)
{
	std::unique_ptr<Etrx2Request> request = Etrx2Request::createAt_enRequest(requestQueue_, channel, pid, epid);
	return handle_(std::move(request));
}

/**
 * \brief Reads ETRX2 module info
 *
 * \param [out] device_name is an array which will hold the device name, nullptr if not used
 * \param [in] device_name_size is the size of device_name array, recommended size is 16
 * \param [out] firmware_revision is an array which will hold the firmware revision, nullptr if not used
 * \param [in] firmware_revision_size is the size of firmware_revision array, recommended size is 8
 * \param [out] eui64 is a pointer to variable which will hold the EUI64, nullptr if not used
 *
 * \return 0 on success, negated value on handling error or error code returned by ETRX2 module (positive value)
 */

int Etrx2::getModuleInfo(char * const device_name, const size_t device_name_size, char * const firmware_revision,
		const size_t firmware_revision_size, uint64_t * const eui64)
{
	std::unique_ptr<Etrx2Request> request = Etrx2Request::createAtiRequest(requestQueue_, device_name, device_name_size,
			firmware_revision, firmware_revision_size, eui64);
	return handle_(std::move(request));
}

/**
 * \brief Reads network info
 *
 * \param [out] epid is a pointer to variable which will hold the EPID number, nullptr if not used
 * \param [out] pid is a pointer to variable which will hold the PID number, nullptr if not used
 * \param [out] network_function is a pointer to variable which will hold network function, nullptr if not used
 * \param [out] channel is a pointer to variable which will hold the channel, nullptr if not used
 * \param [out] power is a pointer to variable which will hold the power, nullptr if not used
 *
 * \return 0 on success, negated value on handling error or error code returned by ETRX2 module (positive value)
 */

int Etrx2::getNetworkInfo(uint64_t * const epid, uint16_t * const pid, NetworkFunction * const network_function,
		uint8_t * const channel, uint8_t * const power)
{
	std::unique_ptr<Etrx2Request> request = Etrx2Request::createAt_nRequest(requestQueue_, epid, pid, network_function,
			channel, power);
	return handle_(std::move(request));
}

/**
 * \brief Initializes Etrx2 object.
 *
 * Creates internal rx task, creates mutex and request queue.
 *
 * \return 0 on success, negated errno code otherwise (errno not set)
 */

int Etrx2::initialize()
{
	portBASE_TYPE ret2 = xTaskCreate(rxTrampoline_, reinterpret_cast<const signed char *>("ETRX2 rx"),
			ETRX2_RX_TASK_STACK_SIZE, this, ETRX2_RX_TASK_PRIORITY, nullptr);
	int ret = ret2 == pdPASS ? 0 : -ENOMEM;

	if (ret == 0)
	{
		mutex_ = xSemaphoreCreateMutex();
		ret = mutex_ != nullptr ? 0 : -ENOMEM;
	}

	if (ret == 0)
	{
		requestQueue_ = xQueueCreate(1, sizeof(int));
		ret = requestQueue_ != nullptr ? 0 : -ENOMEM;
	}

	return ret;
}

/**
 * \brief Scans energy on all channels
 *
 * \param [out] channel_energies is a reference to vector which will hold the energies on channels, it should have
 * capacity for 16 elements (reserve(16))
 *
 * \return 0 on success, negated value on handling error or error code returned by ETRX2 module (positive value)
 */

int Etrx2::scanEnergy(std::vector<Etrx2::ChannelEnergy> &channel_energies)
{
	std::unique_ptr<Etrx2Request> request = Etrx2Request::createAt_escanRequest(requestQueue_, channel_energies);
	return handle_(std::move(request));
}

/**
 * \brief Searches for active networks.
 *
 * \param [out] found_networks is a reference to vector which will hold the found networks, it should have capacity for
 * 16 elements (reserve(16))
 *
 * \return 0 on success, negated value on handling error or error code returned by ETRX2 module (positive value)
 */

int Etrx2::searchNetworks(std::vector<Etrx2::FoundNetwork> &found_networks)
{
	std::unique_ptr<Etrx2Request> request = Etrx2Request::createAt_panscanRequest(requestQueue_, found_networks);
	return handle_(std::move(request));
}

/**
 * \brief Accesses S-Register.
 *
 * \param [in] s_register is the S-Register to be accessed
 * \param [in] write_data is the data buffer for write operation, nullptr for read operation
 * \param [in] password is a string with password for write operation, nullptr if not required
 * \param [out] read_data is the data buffer for read operation, nullptr for write operation
 * \param [in] read_data_size is the size of read_data array, ignored for read operation
 *
 * \return 0 on success, negated value on handling error or error code returned by ETRX2 module (positive value)
 */

int Etrx2::sRegisterAccess(const uint8_t s_register, const char * const write_data,
		const char * const password, char * const read_data, const size_t read_data_size)
{
	std::unique_ptr<Etrx2Request> request = Etrx2Request::createAtsRequest(requestQueue_, s_register, write_data,
			password, read_data, read_data_size);
	return handle_(std::move(request));
}

/**
 * \brief Transmits broadcast.
 *
 * \param [in] hops is the number of hops the broadcast can make, 0 or 30 for entire network
 * \param [in] data is a pointer to string that will be sent, there's a limit on length!
 *
 * \return 0 on success, negated value on handling error or error code returned by ETRX2 module (positive value)
 */

int Etrx2::transmitBroadcast(const uint8_t hops, const char * const data)
{
	std::unique_ptr<Etrx2Request> request = Etrx2Request::createAt_bcastRequest(requestQueue_, hops, data);
	return handle_(std::move(request));
}

/**
 * \brief Transmits unicast.
 *
 * \param [in] address is the EUI64 address of destination
 * \param [in] data is a pointer to string that will be sent, there's a limit on length!
 * \param [out] sequence_number is a pointer to variable which will hold the sequence number, nullptr if not used
 * \param [out] acknowledged is a pointer to variable which will hold the acknowledge status, nullptr if not used
 *
 * \return 0 on success, negated value on handling error or error code returned by ETRX2 module (positive value)
 */

int Etrx2::transmitUnicast(const uint64_t address, const char * const data, uint8_t * const sequence_number,
		bool * const acknowledged)
{
	std::unique_ptr<Etrx2Request> request = Etrx2Request::createAt_ucastRequest(requestQueue_, address, data,
			sequence_number, acknowledged);
	return handle_(std::move(request));
}

/*---------------------------------------------------------------------------------------------------------------------+
| private methods
+---------------------------------------------------------------------------------------------------------------------*/

/**
 * \brief Handles single Etrx2Request
 *
 * Access is serialized with a mutex.
 *
 * \param [in, out] request is an unique_ptr to Etrx2Request (valid or not), that will be handled
 *
 * \return 0 on success, negated value on handling error or error code returned by ETRX2 module (positive value)
 */

int Etrx2::handle_(std::unique_ptr<Etrx2Request> request)
{
	int ret = -EINVAL;

	if (request != nullptr)
	{
		portBASE_TYPE ret2 = xSemaphoreTake(mutex_, portMAX_DELAY);
		assert(ret2 == pdTRUE);

		request_ = &request;

		ret = request->sendCommand(txStream_);

		if (ret == 0)
			ret = fflush(txStream_);

		if (ret == 0)
		{
			ret2 = xQueueReceive(requestQueue_, &ret, portMAX_DELAY);	// wait for response
			assert(ret2 == pdTRUE);

			processedCommands_++;

			ret2 = xSemaphoreGive(mutex_);
			assert(ret2 == pdTRUE);
		}
	}

	return ret;
}

/**
 * \brief Rx task of Etrx2 object.
 *
 * This task only deals with receiving from ETRX2 module.
 */

void Etrx2::rxTask_()
{
	char buffer[ETRX2_BUFFER_SIZE];
	std::unique_ptr<Etrx2Request> request;

	while (1)
	{
		char * const line = fgets(buffer, sizeof(buffer), rxStream_);

		if (request_ != nullptr && *request_ != nullptr)	// is there some new request to handle?
		{
			request = std::move(*request_);	// this may delete previous request, possibly incomplete one
			request_ = nullptr;
		}

		if (line != nullptr)	// something received?
		{
			trimTrailingCharacters(line);	// trim trailing characters that are useless

			if (strlen(line) != 0)	// string not empty?
			{
				const std::unique_ptr<const Etrx2RxLine> rx_line = Etrx2RxLine::factory(line);
				assert(rx_line != nullptr);
				receivedLines_++;

				bool consumed = false;

				if (request != nullptr)	// there is some request already started?
				{
					consumed = request->feed(*rx_line);	// feed data into it

					if (request->isComplete())
					{
						// signal the caller that the request is done and delete it
						const int ret = Etrx2Request::finalize(std::move(request));
						assert(ret == 0);
					}
				}

				if (!consumed && eventCallback_ != nullptr)
				{
					std::unique_ptr<const Etrx2Event> event = rx_line->convertToEvent();
					if (event != nullptr)	// received line was successfully converted to event?
						consumed = eventCallback_(std::move(event));
				}

				if (!consumed)
					droppedLines_++;
			}
		}
	}
}

/*---------------------------------------------------------------------------------------------------------------------+
| private static methods
+---------------------------------------------------------------------------------------------------------------------*/

/**
 * \brief Trims trailing characters.
 *
 * '\r', '\n', ' ' and 0xFF are removed from the end of the string.
 *
 * \param [in] string is a zero-terminated string that will be processed
 */

void Etrx2::trimTrailingCharacters(char *string)
{
	for (int8_t i = strlen(string) - 1;
			i >= 0 && (string[i] == '\r' || string[i] == '\n' || string[i] == ' ' || string[i] == 0xFF); i--)
		string[i] = '\0';
}
