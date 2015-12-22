/**
 * \file etrx2_parser.hpp
 * \brief Header for Etrx2 request parser
 * \author Freddie Chopin, http://www.freddiechopin.info http://www.distortec.com
 * \author: Mazeryt Freager, http://www.gotoc.co
 */

#ifndef ETRX2_PARSER_HPP_
#define ETRX2_PARSER_HPP_

#include "etrx2.hpp"

#include "FreeRTOS.h"
#include "queue.h"

#include <cstdint>

#include <memory>

class Etrx2Event;

/// lists handled ETRX2 commands
enum class Etrx2Command
{
	ATI,		///< "ATI"
	ATS,		///< "ATS"
	AT_BCAST,	///< "AT+BCAST"
	AT_DASSL,	///< "AT+DASSL"
	AT_EN,		///< "AT+EN"
	AT_ESCAN,	///< "AT+ESCAN"
	AT_JN,		///< "AT+JN"
	AT_N,		///< "AT+N"
	AT_PANSCAN,	///< "AT+PANSCAN"
	AT_UCAST,	///< "AT+UCAST"
};

/// single line that is received from ETRX2 module
class Etrx2RxLine
{
private:

	// this is here only to prevent Eclipse's parser errors...
	using Dummy_ = int;

public:

	/// types of Etrx2RxLine
	enum class Type
	{
		ECHO,			///< echo of command, "AT.*"
		PROMPT,			///< prompt
		RESPONSE,		///< response to previously issued command
	};

protected:

	/**
	 * \brief Etrx2RxLine constructor - just sets internal variables.
	 *
	 * \param [in] type is the type of this Etrx2RxLine object
	 */

	constexpr Etrx2RxLine(const Type type) :
			type_(type)
	{};

public:

	virtual ~Etrx2RxLine() {};

	virtual std::unique_ptr<const Etrx2Event> convertToEvent() const;

	/// \brief returns type of this Etrx2RxLine object
	Type getType() const { return type_; };

	static std::unique_ptr<const Etrx2RxLine> factory(const char * const line);

private:

	/// type of this Etrx2RxLine object
	const Type type_;
};

/// request for ETRX2 module, used for response too
class Etrx2Request
{
public:

	/// definition of the Etrx2Request
	struct Definition
	{
		/// string that will be sent to ETRX2 module for this command
		const char * const string;

		/// command issued to ETRX2 module
		const Etrx2Command command;

		/// number of main phases (echo, ..., OK or ERROR prompt) this request has
		const uint8_t mainPhases;

		/// additional phases that follow "OK" prompt
		const uint8_t additionalPhases;
	};

	/**
	 * \brief Etrx2Request constructor - just sets internal variables.
	 *
	 * \param [in] definition is a reference to Definition struct of this request
	 * \param [in] queue is a queue used to signal end of request handling and pass return value
	 */

	constexpr Etrx2Request(const Definition &definition, const xQueueHandle queue):
			definition_(definition),
			queue_(queue),
			ret_(),
			phase_()
	{};

	virtual ~Etrx2Request() {};

	bool feed(const Etrx2RxLine &rx_line);

	/// \brief returns true if this request is complete, false otherwise
	bool isComplete() const { return phase_ >= definition_.mainPhases + definition_.additionalPhases; };

	int sendCommand(FILE * const stream) const;

	static std::unique_ptr<Etrx2Request> createAtiRequest(const xQueueHandle queue, char * const device_name,
			const size_t device_name_size, char * const firmware_revision, const size_t firmware_revision_size,
			uint64_t * const eui64);

	static std::unique_ptr<Etrx2Request> createAtsRequest(const xQueueHandle queue, const uint8_t s_register,
			const char * const write_data, const char * const password, char * const read_data,
			const size_t read_data_size);

	static std::unique_ptr<Etrx2Request> createAt_bcastRequest(const xQueueHandle queue, const uint8_t hops,
			const char * const data);

	static std::unique_ptr<Etrx2Request> createAt_dasslRequest_(const xQueueHandle queue);

	static std::unique_ptr<Etrx2Request> createAt_enRequest(const xQueueHandle queue, uint8_t * const channel,
			uint16_t * const pid, uint64_t * const epid);

	static std::unique_ptr<Etrx2Request> createAt_escanRequest(const xQueueHandle queue,
			std::vector<Etrx2::ChannelEnergy> &channel_energies);

	static std::unique_ptr<Etrx2Request> createAt_jnRequest(const xQueueHandle queue, uint8_t * const channel,
			uint16_t * const pid, uint64_t * const epid);

	static std::unique_ptr<Etrx2Request> createAt_nRequest(const xQueueHandle queue, uint64_t * const epid,
			uint16_t * const pid, Etrx2::NetworkFunction * const network_function, uint8_t * const channel,
			uint8_t * const power);

	static std::unique_ptr<Etrx2Request> createAt_panscanRequest(const xQueueHandle queue,
			std::vector<Etrx2::FoundNetwork> &found_networks);

	static std::unique_ptr<Etrx2Request> createAt_ucastRequest(const xQueueHandle queue, const uint64_t address,
			const char * const data, uint8_t * const sequence_number, bool * const acknowledged);

	static int finalize(std::unique_ptr<Etrx2Request> request);

protected:

	/// \brief returns current phase of request handling
	uint8_t getPhase_() const { return phase_; };

	virtual bool feedInternal_(const char * const line);

	virtual bool feedInternal_(const Etrx2RxLine &rx_line);

	virtual int sendCommandInternal_(FILE * const stream) const;

private:

	/// reference to Definition struct of this request
	const Definition &definition_;

	/// queue used to signal end of request handling and pass return value
	const xQueueHandle queue_;

	/// return value of request handling
	int ret_;

	/// phase of request
	uint8_t phase_;
};

#endif	// ETRX2_PARSER_HPP_
