/**
 * \file etrx2_parser.cpp
 * \brief Etrx2 request parser
 *
 * hardware: Telegesis ETRX2 module
 *
 * \author Freddie Chopin, http://www.freddiechopin.info http://www.distortec.com
 * \author: Mazeryt Freager, http://www.gotoc.co
 */

#include "etrx2_parser.hpp"
#include "etrx2.hpp"
#include "etrx2_event.hpp"

#include <cstring>
#include <cstdlib>
#include <cerrno>

namespace
{

/*---------------------------------------------------------------------------------------------------------------------+
| private variables
+---------------------------------------------------------------------------------------------------------------------*/

/// definitions of the requests - indexes of entries MUST match the numeric value of Etrx2Command
constexpr Etrx2Request::Definition definitions_[] =
{
		// ATI - 5 stages: echo, 3 x response, prompt
		{"ATI", Etrx2Command::ATI, 5, 0},
		// ATS - max 3 stages: echo, max 1 x response, prompt
		{"ATS", Etrx2Command::ATS, 3, 0},
		// AT_BCAST - 2 stages: echo, OK prompt
		{"AT+BCAST", Etrx2Command::AT_BCAST, 2, 0},
		// AT_DASSL - 3 stages: echo, prompt, LeftPAN prompt
		{"AT+DASSL", Etrx2Command::AT_DASSL, 2, 1},
		// AT_EN - 3 stages: echo, JPAN prompt, prompt
		{"AT+EN", Etrx2Command::AT_EN, 3, 0},
		// AT_ESCAN - max 18 stages: echo, max 16 x response, prompt
		{"AT+ESCAN", Etrx2Command::AT_ESCAN, 18, 0},
		// AT_EN - 3 stages: echo, JPAN prompt, prompt
		{"AT+JN", Etrx2Command::AT_JN, 3, 0},
		// AT_N - 3 stages: echo, response, prompt
		{"AT+N", Etrx2Command::AT_N, 3, 0},
		// AT_PANSCAN - max 18 stages: echo, max 16 x response, prompt
		/// \todo add proper support for variable length response
		{"AT+PANSCAN", Etrx2Command::AT_PANSCAN, 18, 0},
		// AT_UCAST - 4 stages: echo, response, OK prompt, ACK or NACK prompt
		{"AT+UCAST", Etrx2Command::AT_UCAST, 3, 1},
};

/*---------------------------------------------------------------------------------------------------------------------+
| private functions
+---------------------------------------------------------------------------------------------------------------------*/

/**
 * \brief Get definition for given command
 *
 * \param [in] command is the Etrx2Command for which the definition should be returned
 *
 * \return reference to Etrx2Request::Definition struct with definition of the request
 */

constexpr const Etrx2Request::Definition & getDefinition_(const Etrx2Command command)
{
	return definitions_[static_cast<size_t>(command)];
}

/*---------------------------------------------------------------------------------------------------------------------+
| private types
+---------------------------------------------------------------------------------------------------------------------*/

/// echo of command issued to ETRX2 module
class EchoRxLine_ : public Etrx2RxLine
{
protected:

	/**
	 * \brief EchoRxLine_ constructor - just sets internal variables.
	 *
	 * \param [in] command is the command issued to ETRX2 module
	 */

	constexpr EchoRxLine_(const Etrx2Command command) :
			Etrx2RxLine(Type::ECHO),
			command_(command)
	{};

public:

	/// \brief returns command issued to ETRX2 module
	Etrx2Command getCommand() const { return command_; };

	static std::unique_ptr<const EchoRxLine_> factory(const char * const line);

private:

	/// command issued to ETRX2 module
	const Etrx2Command command_;
};

/// prompt from ETRX2 module
class PromptRxLine_ : public Etrx2RxLine
{
private:

	// this is here only to prevent Eclipse's parser errors...
	using Dummy_ = int;

public:

	/// types of received prompts
	enum class Type
	{
		ACK_NACK,				///< "ACK:" or "NACK:"
		BCAST_MCAST_UCAST,		///< "BCAST:", "MCAST:" or "UCAST:"
		JPAN,					///< "JPAN:"
		LEFTPAN,				///< "LeftPAN"
		OK_ERROR,				///< "OK" or "ERROR:"
	};

protected:

	/**
	 * \brief PromptRxLine_ constructor - just sets internal variables.
	 *
	 * \param [in] type is the type of received prompt
	 */

	constexpr PromptRxLine_(const Type type) :
			Etrx2RxLine(Etrx2RxLine::Type::PROMPT),
			type_(type)
	{};

public:

	/// \brief returns type of received prompt
	Type getType() const { return type_; };

	static std::unique_ptr<const PromptRxLine_> factory(const char * const line);

private:

	/// type of received prompt
	const Type type_;
};

/// ACK or NACK prompt from ETRX2 module
class AckNackPromptRxLine_ : public PromptRxLine_
{
protected:

	/**
	 * \brief AckNackPromptRxLine_ constructor - just sets internal variables.
	 *
	 * \param [in] acknowledged should be true if ACK, false if NACK
	 * \param [in] sequence_number is the sequence number
	 */

	constexpr AckNackPromptRxLine_(const bool acknowledged, const uint8_t sequence_number) :
			PromptRxLine_(PromptRxLine_::Type::ACK_NACK),
			acknowledged_(acknowledged),
			sequenceNumber_(sequence_number)
	{};

public:

	/**
	 * \brief Returns (via pointers) parameters embedded in this prompt
	 *
	 * \param [out] acknowledged is a pointer to variable which will hold the acknowledge status, nullptr if not used
	 * \param [out] sequence_number is a pointer to variable which will hold the sequence number, nullptr if not used
	 */

	void getParameters(bool * const acknowledged, uint8_t * const sequence_number) const
	{
		if (acknowledged != nullptr)
			*acknowledged = acknowledged_;
		if (sequence_number != nullptr)
			*sequence_number = sequenceNumber_;
	};

	static std::unique_ptr<const AckNackPromptRxLine_> factory(const char * const line);

private:

	/// true if ACK, false if NACK
	const bool acknowledged_;

	/// sequence number
	const uint8_t sequenceNumber_;
};

/// BCAST, MCAST or UCAST prompt from ETRX2 module
class BcastMcastUcastPromptRxLine_ : public PromptRxLine_
{
private:

	// this is here only to prevent Eclipse's parser errors...
	using Dummy_ = int;

public:

	/// type of received *cast
	enum class Type
	{
		BROADCAST,		///< broadcast
		MULTICAST,		///< multicast
		UNICAST,		///< unicast
	};

protected:

	/**
	 * \brief BcastMcastUcastPromptRxLine_ constructor - just sets internal variables.
	 *
	 * \param [in] type is the type of *cast, Type::{BROADCAST, MULTICAST, UNICAST}
	 * \param [in] eui64 is the EUI64 of sender
	 * \param [in] length is the length of data string, without terminating '\0'
	 * \param [in] data is the data string, alias pointing to part of RX line
	 */

	constexpr BcastMcastUcastPromptRxLine_(const Type type, const uint64_t eui64, const uint8_t length,
			const char * const data) :
			PromptRxLine_(PromptRxLine_::Type::BCAST_MCAST_UCAST),
			eui64_(eui64),
			data_(data),
			length_(length),
			type_(type)
	{};

public:

	virtual std::unique_ptr<const Etrx2Event> convertToEvent() const override;

	/**
	 * \brief Returns (via pointers) parameters embedded in this prompt
	 *
	 * \param [out] type is a pointer to variable which will hold the type, nullptr if not used
	 * \param [out] eui64 is a pointer to variable which will hold EUI64, nullptr if not used
	 * \param [out] length is a pointer to variable which will hold length, nullptr if not used
	 * \param [out] data is a pointer to variable which will hold data, nullptr if not used
	 */

	void getParameters(Type * const type, uint64_t * const eui64, uint8_t * const length, const char ** const data)
			const
	{
		if (type != nullptr)
			*type = type_;
		if (eui64 != nullptr)
			*eui64 = eui64_;
		if (length != nullptr)
			*length = length_;
		if (data != nullptr)
			*data = data_;
	};

	static std::unique_ptr<const BcastMcastUcastPromptRxLine_> factory(const char * const line);

private:

	/// EUI64 of sender
	const uint64_t eui64_;

	/// data string, this is just an alias pointing to part of RX line!
	const char * const data_;

	/// length of data, not including terminating '\0'
	const uint8_t length_;

	/// type of *cast
	const Type type_;
};

/// JPAN prompt from ETRX2 module
class JpanPromptRxLine_ : public PromptRxLine_
{
protected:

	/**
	 * \brief JpanPromptRxLine_ constructor - just sets internal variables.
	 *
	 * \param [in] channel is the IEEE 802.15.4 radio channel (11-26)
	 * \param [in] pid is the PAN ID
	 * \param [in] epid is the extended PAN ID
	 */

	constexpr JpanPromptRxLine_(const uint8_t channel, const uint16_t pid, const uint64_t epid) :
			PromptRxLine_(PromptRxLine_::Type::JPAN),
			epid_(epid),
			pid_(pid),
			channel_(channel)
	{};

public:

	/**
	 * \brief Returns (via pointers) parameters of established/joined PAN
	 *
	 * \param [out] channel is a pointer to variable which will hold the channel, nullptr if not used
	 * \param [out] pid is a pointer to variable which will hold PID, nullptr if not used
	 * \param [out] epid is a pointer to variable which will hold EPID, nullptr if not used
	 */

	void getParameters(uint8_t * const channel, uint16_t * const pid, uint64_t * const epid) const
	{
		if (channel != nullptr)
			*channel = channel_;
		if (pid != nullptr)
			*pid = pid_;
		if (epid != nullptr)
			*epid = epid_;
	}

	static std::unique_ptr<const JpanPromptRxLine_> factory(const char * const line);

private:

	/// extended PAN ID
	const uint64_t epid_;

	/// PAN ID
	const uint16_t pid_;

	/// IEEE 802.15.4 radio channel (11-26)
	const uint8_t channel_;
};

/// OK or error prompt from ETRX2 module
class OkErrorPromptRxLine_ : public PromptRxLine_
{
protected:

	/**
	 * \brief OkErrorPromptRxLine_ constructor - just sets internal variables.
	 *
	 * \param [in] error_code is the error code received from ETRX2 module
	 */

	constexpr OkErrorPromptRxLine_(const uint8_t error_code) :
			PromptRxLine_(PromptRxLine_::Type::OK_ERROR),
			errorCode_(error_code)
	{};

public:

	/// \brief returns error code received from ETRX2 module
	uint8_t getErrorCode() const { return errorCode_; };

	static std::unique_ptr<const OkErrorPromptRxLine_> factory(const char * const line);

private:

	/// error code received from ETRX2 module
	const uint8_t errorCode_;
};

/// response that is not a prompt and not an echo - this must be consumed by current Etrx2Request
class ResponseRxLine_ : public Etrx2RxLine
{
protected:

	/**
	 * \brief ResponseRxLine_ constructor - just sets internal variables.
	 *
	 * \param [in] line is the received line (only pointer is stored)
	 */

	constexpr ResponseRxLine_(const char * const line) :
			Etrx2RxLine(Etrx2RxLine::Type::RESPONSE),
			line_(line)
	{};

public:

	/// \brief returns pointer to received line
	const char * getLine() const { return line_; };

	static std::unique_ptr<const ResponseRxLine_> factory(const char * const line);

private:

	/// pointer to received line
	const char * const line_;
};

/// "ATI" command
class AtiRequest_ : public Etrx2Request
{
public:

	/**
	 * \brief AtiRequest_ constructor - just sets internal variables.
	 *
	 * \param [in] queue is a queue used to signal end of request handling and pass return value
	 * \param [out] device_name is an array which will hold the device name, nullptr if not used
	 * \param [in] device_name_size is the size of device_name array, recommended size is 16
	 * \param [out] firmware_revision is an array which will hold the firmware revision, nullptr if not used
	 * \param [in] firmware_revision_size is the size of firmware_revision array, recommended size is 8
	 * \param [out] eui64 is a pointer to uint64_t variable which will hold the EUI64, nullptr if not used
	 */

	constexpr AtiRequest_(const xQueueHandle queue, char * const device_name, const size_t device_name_size,
			char * const firmware_revision, const size_t firmware_revision_size, uint64_t * const eui64) :
			Etrx2Request(getDefinition_(Etrx2Command::ATI), queue),
			deviceName_(device_name),
			deviceNameSize_(device_name_size),
			eui64_(eui64),
			firmwareRevision_(firmware_revision),
			firmwareRevisionSize_(firmware_revision_size)
	{};

	virtual ~AtiRequest_() override {};

protected:

	virtual bool feedInternal_(const char * const line) override;

private:

	/// name of ETRX2 module
	char * const deviceName_;

	/// size of deviceName_ array
	const size_t deviceNameSize_;

	/// own 64-bit address,
	uint64_t * const eui64_;

	/// firmware revision of ETRX2 module
	char * const firmwareRevision_;

	/// size of firmwareRevision_ array
	const size_t firmwareRevisionSize_;
};

/// "ATS" request
class AtsRequest_ : public Etrx2Request
{
public:

	/**
	 * \brief AtsRequest_ constructor - just sets internal variables.
	 *
	 * \param [in] queue is a queue used to signal end of request handling and pass return value
	 * \param [in] s_register is the S-Register to be accessed
	 * \param [in] write_data is the data buffer for write operation, nullptr for read operation
	 * \param [in] password is a string with password for write operation, nullptr if not required
	 * \param [out] read_data is the data buffer for read operation, nullptr for write operation
	 * \param [in] read_data_size is the size of read_data array, ignored for read operation
	 */

	constexpr AtsRequest_(const xQueueHandle queue, const uint8_t s_register, const char * const write_data,
			const char * const password, char * const read_data, const size_t read_data_size) :
			Etrx2Request(getDefinition_(Etrx2Command::ATS), queue),
			password_(password),
			readData_(read_data),
			readDataSize_(read_data_size),
			writeData_(write_data),
			sRegister_(s_register)
	{};

	virtual ~AtsRequest_() override {};

protected:

	virtual bool feedInternal_(const char * const line) override;

	virtual int sendCommandInternal_(FILE * const stream) const override;

private:

	/// password for write operation, nullptr if not used
	const char * const password_;

	/// data buffer for read operation, nullptr for write operation
	char * const readData_;

	/// size of readData_ array, ignored for read operation
	const size_t readDataSize_;

	/// data buffer for write operation, nullptr for read operation
	const char * const writeData_;

	/// S-Register to be accessed
	const uint8_t sRegister_;
};

/// "AT+BCAST" command
class At_bcastRequest_ : public Etrx2Request
{
public:

	/**
	 * \brief At_bcastRequest_ constructor - just sets internal variables.
	 *
	 * \param [in] queue is a queue used to signal end of request handling and pass return value
	 * \param [in] hops is the number of hops the broadcast can make, 0 or 30 for entire network
	 * \param [in] data is a pointer to string that will be sent, there's a limit on length!
	 */

	constexpr At_bcastRequest_(const xQueueHandle queue, const uint8_t hops, const char * const data) :
			Etrx2Request(getDefinition_(Etrx2Command::AT_BCAST), queue),
			data_(data),
			hops_(hops)
	{};

	virtual ~At_bcastRequest_() override {};

protected:

	virtual int sendCommandInternal_(FILE * const stream) const override;

private:

	/// pointer to string that will be sent, there's a limit on length!
	const char * const data_;

	/// number of hops the broadcast can make, 0 or 30 for entire network
	const uint8_t hops_;
};

/// "AT+DASSL" command
class At_dasslRequest_ : public Etrx2Request
{
public:

	/**
	 * \brief At_dasslRequest_ constructor - just sets internal variables.
	 *
	 * \param [in] queue is a queue used to signal end of request handling and pass return value
	 */

	constexpr At_dasslRequest_(const xQueueHandle queue) :
			Etrx2Request(getDefinition_(Etrx2Command::AT_DASSL), queue)
	{};

	virtual ~At_dasslRequest_() override {};

protected:

	virtual bool feedInternal_(const Etrx2RxLine &rx_line) override;
};

/// "AT+EN" or "AT+JN" command
class At_enAt_jnRequest_ : public Etrx2Request
{
public:

	/**
	 * \brief At_enAt_jnRequest_ constructor - just sets internal variables.
	 *
	 * \param [in] command is the command issued to ETRX2 module, should be Etrx2Command::AT_EN or Etrx2Command::AT_JN
	 * \param [in] queue is a queue used to signal end of request handling and pass return value
	 * \param [out] channel is a pointer to variable which will hold the channel, nullptr if not used
	 * \param [out] pid is a pointer to variable which will hold the PID number, nullptr if not used
	 * \param [out] epid is a pointer to variable which will hold the EPID number, nullptr if not used
	 */

	constexpr At_enAt_jnRequest_(const Etrx2Command command, const xQueueHandle queue,
			uint8_t * const channel, uint16_t * const pid, uint64_t * const epid) :
			Etrx2Request(getDefinition_(command), queue),
			channel_(channel),
			epid_(epid),
			pid_(pid)
	{};

	virtual ~At_enAt_jnRequest_() override {};

protected:

	virtual bool feedInternal_(const Etrx2RxLine &rx_line) override;

private:

	/// IEEE 802.15.4 radio channel (11-26)
	uint8_t * const channel_;

	/// extended PAN ID
	uint64_t * const epid_;

	/// PAN ID
	uint16_t * const pid_;
};

/// "AT+ESCAN" command
class At_escanRequest_ : public Etrx2Request
{
public:

	/**
	 * \brief At_escanRequest_ constructor - just sets internal variables.
	 *
	 * \param [in] queue is a queue used to signal end of request handling and pass return value
	 * \param [out] channel_energies is a reference to vector which will hold the energies on channels, it should have
	 * capacity for 16 elements (reserve(16))
	 */

	constexpr At_escanRequest_(const xQueueHandle queue, std::vector<Etrx2::ChannelEnergy> &channel_energies) :
			Etrx2Request(getDefinition_(Etrx2Command::AT_ESCAN), queue),
			channelEnergies_(channel_energies)
	{};

	virtual ~At_escanRequest_() override {};

protected:

	virtual bool feedInternal_(const char * const line) override;

private:

	/// energies on channels
	std::vector<Etrx2::ChannelEnergy> &channelEnergies_;
};

/// "AT+N" command
class At_nRequest_ : public Etrx2Request
{
public:

	/**
	 * \brief At_nRequest_ constructor - just sets internal variables.
	 *
	 * \param [in] queue is a queue used to signal end of request handling and pass return value
	 * \param [out] epid is a pointer to variable which will hold the EPID number, nullptr if not used
	 * \param [out] pid is a pointer to variable which will hold the PID number, nullptr if not used
	 * \param [out] network_function is a pointer to variable which will hold network function, nullptr if not used
	 * \param [out] channel is a pointer to variable which will hold the channel, nullptr if not used
	 * \param [out] power is a pointer to variable which will hold the power, nullptr if not used
	 */

	constexpr At_nRequest_(const xQueueHandle queue, uint64_t * const epid, uint16_t * const pid,
			Etrx2::NetworkFunction * const network_function, uint8_t * const channel, uint8_t * const power) :
			Etrx2Request(getDefinition_(Etrx2Command::AT_N), queue),
			channel_(channel),
			networkFunction_(network_function),
			epid_(epid),
			pid_(pid),
			power_(power)
	{};

	virtual ~At_nRequest_() override {};

protected:

	virtual bool feedInternal_(const char * const line) override;

private:

	/// IEEE 802.15.4 radio channel (11-26)
	uint8_t * const channel_;

	/// network function
	Etrx2::NetworkFunction * const networkFunction_;

	/// extended PAN ID
	uint64_t * const epid_;

	/// PAN ID
	uint16_t * const pid_;

	/// node's output power in dBm
	uint8_t * const power_;
};

/// "AT+PANSCAN" command
class At_panscanRequest_ : public Etrx2Request
{
public:

	/**
	 * \brief At_panscanRequest_ constructor - just sets internal variables.
	 *
	 * \param [in] queue is a queue used to signal end of request handling and pass return value
	 * \param [out] found_networks is a reference to vector which will hold the found networks, it should have capacity
	 * for 16 elements (reserve(16))
	 */

	constexpr At_panscanRequest_(const xQueueHandle queue, std::vector<Etrx2::FoundNetwork> &found_networks) :
			Etrx2Request(getDefinition_(Etrx2Command::AT_PANSCAN), queue),
			foundNetworks_(found_networks)
	{};

	virtual ~At_panscanRequest_() override {};

protected:

	virtual bool feedInternal_(const char * const line) override;

private:

	/// found networks
	std::vector<Etrx2::FoundNetwork> &foundNetworks_;
};

/// "AT+UCAST" command
class At_ucastRequest_ : public Etrx2Request
{
public:

	/**
	 * \brief At_ucastRequest_ constructor - just sets internal variables.
	 *
	 * \param [in] queue is a queue used to signal end of request handling and pass return value
	 * \param [in] address is the EUI64 address of destination
	 * \param [in] data is a pointer to string that will be sent, there's a limit on length!
	 * \param [out] sequence_number is a pointer to variable which will hold the sequence number, nullptr if not used
	 * \param [out] acknowledged is a pointer to variable which will hold the acknowledge status, nullptr if not used
	 */

	constexpr At_ucastRequest_(const xQueueHandle queue, const uint64_t address, const char * const data,
			uint8_t * const sequence_number, bool * const acknowledged) :
			Etrx2Request(getDefinition_(Etrx2Command::AT_UCAST), queue),
			address_(address),
			acknowledged_(acknowledged),
			data_(data),
			seqenceNumber_(sequence_number != nullptr ? *sequence_number : seqenceNumberStorage_),
			seqenceNumberStorage_()
	{};

	virtual ~At_ucastRequest_() override {};

protected:

	virtual bool feedInternal_(const char * const line) override;

	virtual bool feedInternal_(const Etrx2RxLine &rx_line) override;

	virtual int sendCommandInternal_(FILE * const stream) const override;

private:

	/// EUI64 address of destination
	const uint64_t address_;

	/// true if unicast was acknowledged, false otherwise
	bool * const acknowledged_;

	/// pointer to string that will be sent, there's a limit on length!
	const char * const data_;

	/// sequence number
	uint8_t &seqenceNumber_;

	/// sequenceNumber_ reference is required for proper operation and uses this storage if nullptr is used as parameter
	uint8_t seqenceNumberStorage_;
};

}	// namespace

/*---------------------------------------------------------------------------------------------------------------------+
| public methods of Etrx2RxLine
+---------------------------------------------------------------------------------------------------------------------*/

/**
 * \brief Tries to convert Etrx2RxLine to Etrx2Event.
 *
 * Default implementation - no conversion.
 *
 * \return unique_ptr to Etrx2Event polymorphic object, nullptr if conversion is not possible
 */

std::unique_ptr<const Etrx2Event> Etrx2RxLine::convertToEvent() const
{
	return nullptr;
}

/**
 * \brief Factory method for Etrx2RxLine
 *
 * \param [in] line is the received line
 *
 * \return unique_ptr to processed Etrx2RxLine polymorphic object - always valid
 */

std::unique_ptr<const Etrx2RxLine> Etrx2RxLine::factory(const char * const line)
{
	std::unique_ptr<const Etrx2RxLine> rx_line = EchoRxLine_::factory(line);	// first try checking for command echo
	if (rx_line == nullptr)
	{
		rx_line = PromptRxLine_::factory(line);			// check for prompt
		if (rx_line == nullptr)
			rx_line = ResponseRxLine_::factory(line);	// everything else is consumed by ResponseRxLine_
	}

	return rx_line;
}

/*---------------------------------------------------------------------------------------------------------------------+
| public methods of Etrx2Request
+---------------------------------------------------------------------------------------------------------------------*/

/**
 * \brief Feeds Etrx2RxLine to Etrx2Request object.
 *
 * It handles the command echo and some prompt types, if not consumed here the rx_line is passed to feedInternal_().
 *
 * \param [in] rx_line is a reference to Etrx2RxLine object received from ETRX2 module
 *
 * \return true if rx_line was consumed, false otherwise
 */

bool Etrx2Request::feed(const Etrx2RxLine &rx_line)
{
	bool consumed = false;

	if (rx_line.getType() == Etrx2RxLine::Type::ECHO)	// command echo may "reset" the request to first phase...
	{
		const EchoRxLine_ &echo_rx_line = static_cast<const EchoRxLine_ &>(rx_line);
		if (echo_rx_line.getCommand() == definition_.command)	// ... but only if the commands match
		{
			consumed = true;
			phase_ = 1;
		}
	}
	else if (rx_line.getType() == Etrx2RxLine::Type::PROMPT)	// some prompts are valid for all requests...
	{
		const PromptRxLine_ &prompt_rx_line = static_cast<const PromptRxLine_ &>(rx_line);
		if (prompt_rx_line.getType() == PromptRxLine_::Type::OK_ERROR)	// ... but only if it's "OK" or "ERROR:" prompt
		{
			consumed = true;
			const OkErrorPromptRxLine_ &ok_error_prompt_rx_line =
					static_cast<const OkErrorPromptRxLine_ &>(prompt_rx_line);
			ret_ = ok_error_prompt_rx_line.getErrorCode();
			// OK (no errors) may "finish" the request if it has no additional phases, error will "finish" the request
			phase_ = ret_ == 0 ? definition_.mainPhases : definition_.mainPhases + definition_.additionalPhases;
		}
	}

	if (!consumed)	// rx_line not consumed yet - push it "down" to internal functions
	{
		consumed = feedInternal_(rx_line);

		if (consumed)	// "increment" of phase only for feedInternal_()!
			phase_++;
	}

	return consumed;
};

/**
 * \brief Writes the command's string (including the final '\r') to output stream.
 *
 * The stream is NOT flushed.
 *
 * \param [in] stream is the output stream
 *
 * \return 0 on success, negated errno code otherwise (errno not set)
 */

int Etrx2Request::sendCommand(FILE * const stream) const
{
	int ret = fputs(definition_.string, stream);
	ret = ret >= 0 ? 0 : -EIO;

	if (ret == 0)
		ret = sendCommandInternal_(stream);

	if (ret == 0)
	{
		ret = fputc('\r', stream);
		ret = ret >= 0 ? 0 : -EIO;
	}

	return ret;
}

/**
 * \brief Creates "ATI" request.
 *
 * \param [in] queue is a queue used to signal end of request handling and pass return value
 * \param [out] device_name is an array which will hold the device name, nullptr if not used
 * \param [in] device_name_size is the size of device_name array, recommended size is 16
 * \param [out] firmware_revision is an array which will hold the firmware revision, nullptr if not used
 * \param [in] firmware_revision_size is the size of firmware_revision array, recommended size is 8
 * \param [out] eui64 is a pointer to variable which will hold the EUI64, nullptr if not used
 *
 * \return unique_ptr to created AtiRequest_ object
 */

std::unique_ptr<Etrx2Request> Etrx2Request::createAtiRequest(const xQueueHandle queue, char * const device_name,
		const size_t device_name_size, char * const firmware_revision, const size_t firmware_revision_size,
		uint64_t * const eui64)
{
	std::unique_ptr<Etrx2Request> request(new AtiRequest_(queue, device_name, device_name_size, firmware_revision,
			firmware_revision_size, eui64));
	return request;
}

/**
 * \brief Creates "ATS" request.
 *
 * \param [in] queue is a queue used to signal end of request handling and pass return value
 * \param [in] s_register is the S-Register to be accessed
 * \param [in] write_data is the data buffer for write operation, nullptr for read operation
 * \param [in] password is a string with password for write operation, nullptr if not required
 * \param [out] read_data is the data buffer for read operation, nullptr for write operation
 * \param [in] read_data_size is the size of read_data array, ignored for read operation
 *
 * \return unique_ptr to created AtsRequest_ object
 */

std::unique_ptr<Etrx2Request> Etrx2Request::createAtsRequest(const xQueueHandle queue, const uint8_t s_register,
		const char * const write_data, const char * const password, char * const read_data, const size_t read_data_size)
{
	std::unique_ptr<Etrx2Request> request(new AtsRequest_(queue, s_register, write_data, password, read_data,
			read_data_size));
	return request;
}

/**
 * \brief Creates "AT+BCAST" request.
 *
 * \param [in] queue is a queue used to signal end of request handling and pass return value
 * \param [in] hops is the number of hops the broadcast can make, 0 or 30 for entire network
 * \param [in] data is a pointer to string that will be sent, there's a limit on length!
 *
 * \return unique_ptr to created At_bcastRequest_ object
 */

std::unique_ptr<Etrx2Request> Etrx2Request::createAt_bcastRequest(const xQueueHandle queue, const uint8_t hops,
		const char * const data)
{
	std::unique_ptr<Etrx2Request> request(new At_bcastRequest_(queue, hops, data));
	return request;
}

/**
 * \brief Creates "AT+DASSL" request.
 *
 * \param [in] queue is a queue used to signal end of request handling and pass return value
 *
 * \return unique_ptr to created At_dasslRequest_ object
 */

std::unique_ptr<Etrx2Request> Etrx2Request::createAt_dasslRequest_(const xQueueHandle queue)
{
	std::unique_ptr<Etrx2Request> request(new At_dasslRequest_(queue));
	return request;
}

/**
 * \brief Creates "AT+EN" request.
 *
 * \param [in] queue is a queue used to signal end of request handling and pass return value
 * \param [out] channel is a pointer to variable which will hold the channel, nullptr if not used
 * \param [out] pid is a pointer to variable which will hold the PID number, nullptr if not used
 * \param [out] epid is a pointer to variable which will hold the EPID number, nullptr if not used
 *
 * \return unique_ptr to created At_enRequest_ object
 */

std::unique_ptr<Etrx2Request> Etrx2Request::createAt_enRequest(const xQueueHandle queue, uint8_t * const channel,
		uint16_t * const pid, uint64_t * const epid)
{
	std::unique_ptr<Etrx2Request> request(new At_enAt_jnRequest_(Etrx2Command::AT_EN, queue, channel, pid, epid));
	return request;
}

/**
 * \brief Creates "AT+ESCAN" request.
 *
 * \param [in] queue is a queue used to signal end of request handling and pass return value
 * \param [out] channel_energies is a reference to vector which will hold the energies on channels, it should have
 * capacity for 16 elements (reserve(16))
 *
 * \return unique_ptr to created At_escanRequest_ object
 */

std::unique_ptr<Etrx2Request> Etrx2Request::createAt_escanRequest(const xQueueHandle queue,
		std::vector<Etrx2::ChannelEnergy> &channel_energies)
{
	std::unique_ptr<Etrx2Request> request(new At_escanRequest_(queue, channel_energies));
	return request;
}

/**
 * \brief Creates "AT+JN" request.
 *
 * \param [in] queue is a queue used to signal end of request handling and pass return value
 * \param [out] channel is a pointer to variable which will hold the channel, nullptr if not used
 * \param [out] pid is a pointer to variable which will hold the PID number, nullptr if not used
 * \param [out] epid is a pointer to variable which will hold the EPID number, nullptr if not used
 *
 * \return unique_ptr to created At_jnRequest_ object
 */

std::unique_ptr<Etrx2Request> Etrx2Request::createAt_jnRequest(const xQueueHandle queue, uint8_t * const channel,
		uint16_t * const pid, uint64_t * const epid)
{
	std::unique_ptr<Etrx2Request> request(new At_enAt_jnRequest_(Etrx2Command::AT_JN, queue, channel, pid,
			epid));
	return request;
}

/**
 * \brief Creates "AT+N" request.
 *
 * \param [in] queue is a queue used to signal end of request handling and pass return value
 * \param [out] epid is a pointer to variable which will hold the EPID number, nullptr if not used
 * \param [out] pid is a pointer to variable which will hold the PID number, nullptr if not used
 * \param [out] network_function is a pointer to variable which will hold network function, nullptr if not used
 * \param [out] channel is a pointer to variable which will hold the channel, nullptr if not used
 * \param [out] power is a pointer to variable which will hold the power, nullptr if not used
 *
 * \return unique_ptr to created At_nRequest_ object
 */

std::unique_ptr<Etrx2Request> Etrx2Request::createAt_nRequest(const xQueueHandle queue, uint64_t * const epid,
		uint16_t * const pid, Etrx2::NetworkFunction * const network_function, uint8_t * const channel,
		uint8_t * const power)
{
	std::unique_ptr<Etrx2Request> request(new At_nRequest_(queue, epid, pid, network_function, channel, power));
	return request;
}

/**
 * \brief Creates "AT+PANSCAN" request.
 *
 * \param [in] queue is a queue used to signal end of request handling and pass return value
 * \param [out] found_networks is a reference to vector which will hold the found networks, it should have capacity for
 * 16 elements (reserve(16))
 *
 * \return unique_ptr to created At_panscanRequest_ object
 */

std::unique_ptr<Etrx2Request> Etrx2Request::createAt_panscanRequest(const xQueueHandle queue,
		std::vector<Etrx2::FoundNetwork> &found_networks)
{
	std::unique_ptr<Etrx2Request> request(new At_panscanRequest_(queue, found_networks));
	return request;
}

/**
 * \brief Creates "AT+UCAST" request.
 *
 * \param [in] queue is a queue used to signal end of request handling and pass return value
 * \param [in] address is the EUI64 address of destination
 * \param [in] data is a pointer to string that will be sent, there's a limit on length!
 * \param [out] sequence_number is a pointer to variable which will hold the sequence number, nullptr if not used
 * \param [out] acknowledged is a pointer to variable which will hold the acknowledge status, nullptr if not used
 *
 * \return unique_ptr to created At_nRequest_ object
 */

std::unique_ptr<Etrx2Request> Etrx2Request::createAt_ucastRequest(const xQueueHandle queue, const uint64_t address,
		const char * const data, uint8_t * const sequence_number, bool * const acknowledged)
{
	std::unique_ptr<Etrx2Request> request(new At_ucastRequest_(queue, address, data, sequence_number, acknowledged));
	return request;
}

/**
 * \brief Finalizes handling of request.
 *
 * Signals the caller that the request handling is done (with a queue) and deletes the request.
 *
 * \param [in] request is an unique_ptr to Etrx2Request object that will be finalized
 *
 * \return 0 on success, negative value on failure
 */

int Etrx2Request::finalize(std::unique_ptr<Etrx2Request> request)
{
	const portBASE_TYPE ret = xQueueSend(request->queue_, &request->ret_, 0);
	request.reset();
	return ret == pdTRUE ? 0 : -1;
}

/*---------------------------------------------------------------------------------------------------------------------+
| protected methods of Etrx2Request
+---------------------------------------------------------------------------------------------------------------------*/

/**
 * \brief Feeds string to Etrx2Request object.
 *
 * Default implementation - does nothing.
 *
 * \param [in] line is a string received from ETRX2 module
 *
 * \return true if line was consumed, false otherwise
 */

bool Etrx2Request::feedInternal_(const char *)
{
	return false;
};

/**
 * \brief Feeds Etrx2RxLine to Etrx2Request object.
 *
 * Default implementation - tries to convert Etrx2RxLine to ResponseRxLine_ and passes string to
 * feedInternal_(const char *)
 *
 * \param [in] rx_line is a reference to Etrx2RxLine object received from ETRX2 module
 *
 * \return true if line was consumed, false otherwise
 */

bool Etrx2Request::feedInternal_(const Etrx2RxLine &rx_line)
{
	bool consumed = false;

	if (rx_line.getType() == Etrx2RxLine::Type::RESPONSE)
	{
		const ResponseRxLine_ &request_rx_line = static_cast<const ResponseRxLine_ &>(rx_line);
		const char * const line = request_rx_line.getLine();
		consumed = feedInternal_(line);
	}

	return consumed;
}

/**
 * \brief Internal function to send command.
 *
 * Control enters here after the command identification was sent ("AT..."), it should send every additional text up to
 * (but without) '\r'.
 *
 * Default implementation - does nothing.
 *
 * \param [in] stream is the output stream
 *
 * \return 0 on success, negated errno code otherwise (errno not set)
 */

int Etrx2Request::sendCommandInternal_(FILE *) const
{
	return 0;
}

namespace
{

/*---------------------------------------------------------------------------------------------------------------------+
| public methods of EchoRxLine_
+---------------------------------------------------------------------------------------------------------------------*/

/**
 * \brief Factory method for EchoRxLine_
 *
 * \param [in] line is the received line
 *
 * \return unique_ptr to processed EchoRxLine_ object, nullptr if object could not be created
 */

std::unique_ptr<const EchoRxLine_> EchoRxLine_::factory(const char * const line)
{
	std::unique_ptr<const EchoRxLine_> echo_rx_line;

	if (strncmp("AT", line, strlen("AT")) == 0)	// all echo lines start with "AT"
		// check whole definitions_ array until a match is found
		for (uint32_t i = 0; i < sizeof(definitions_) / sizeof(*definitions_) && echo_rx_line == nullptr; i++)
			if (strncmp(line, definitions_[i].string, strlen(definitions_[i].string)) == 0)
				echo_rx_line.reset(new EchoRxLine_(definitions_[i].command));

	return echo_rx_line;
}

/*---------------------------------------------------------------------------------------------------------------------+
| public methods of PromptRxLine_
+---------------------------------------------------------------------------------------------------------------------*/

/**
 * \brief Factory method for PromptRxLine_
 *
 * \param [in] line is the received line
 *
 * \return unique_ptr to processed PromptRxLine_ object, nullptr if object could not be created
 */

std::unique_ptr<const PromptRxLine_> PromptRxLine_::factory(const char * const line)
{
	std::unique_ptr<const PromptRxLine_> prompt_rx_line = OkErrorPromptRxLine_::factory(line);
	if (prompt_rx_line == nullptr)
		prompt_rx_line = AckNackPromptRxLine_::factory(line);
	if (prompt_rx_line == nullptr)
		prompt_rx_line = BcastMcastUcastPromptRxLine_::factory(line);
	if (prompt_rx_line == nullptr)
		prompt_rx_line = JpanPromptRxLine_::factory(line);
	if (prompt_rx_line == nullptr && strncmp("LeftPAN", line, strlen("LeftPAN")) == 0)
		prompt_rx_line.reset(new PromptRxLine_(Type::LEFTPAN));

	return prompt_rx_line;
}

/*---------------------------------------------------------------------------------------------------------------------+
| public methods of AckNackPromptRxLine_
+---------------------------------------------------------------------------------------------------------------------*/

/**
 * \brief Factory method for AckNackPromptRxLine_
 *
 * \param [in] line is the received line
 *
 * \return unique_ptr to processed AckNackPromptRxLine_ object, nullptr if object could not be created
 */

std::unique_ptr<const AckNackPromptRxLine_> AckNackPromptRxLine_::factory(const char * const line)
{
	std::unique_ptr<const AckNackPromptRxLine_> ack_nack_prompt_rx_line;

	const bool ack = strncmp("ACK:", line, strlen("ACK:")) == 0;
	const bool nack = strncmp("NACK:", line, strlen("NACK:")) == 0;

	if (ack || nack)
	{
		const uint8_t sequence_number = strtoul(line + (ack ? 4 : 5), nullptr, 16);
		ack_nack_prompt_rx_line.reset(new AckNackPromptRxLine_(ack, sequence_number));
	}

	return ack_nack_prompt_rx_line;
}

/*---------------------------------------------------------------------------------------------------------------------+
| public methods of BcastMcastUcastPromptRxLine_
+---------------------------------------------------------------------------------------------------------------------*/

/**
 * \brief Converts BcastMcastUcastPromptRxLine_ to Etrx2Event.
 *
 * \return unique_ptr to Etrx2Event polymorphic object, nullptr if conversion is not possible
 */

std::unique_ptr<const Etrx2Event> BcastMcastUcastPromptRxLine_::convertToEvent() const
{
	Etrx2MessageEvent::Type type;

	if (type_ == Type::BROADCAST)
		type = Etrx2MessageEvent::Type::BROADCAST;
	else if (type_ == Type::MULTICAST)
		type = Etrx2MessageEvent::Type::MULTICAST;
	else // if (type_ == Type::UNICAST)
		type = Etrx2MessageEvent::Type::UNICAST;

	std::unique_ptr<const Etrx2Event> event(new Etrx2MessageEvent(type, eui64_, length_, data_));

	return event;
}

/**
 * \brief Factory method for BcastMcastUcastPromptRxLine_
 *
 * \param [in] line is the received line
 *
 * \return unique_ptr to processed BcastMcastUcastPromptRxLine_ object, nullptr if object could not be created
 */

std::unique_ptr<const BcastMcastUcastPromptRxLine_> BcastMcastUcastPromptRxLine_::factory(const char * const line)
{
	std::unique_ptr<const BcastMcastUcastPromptRxLine_> bcast_mcast_ucast_prompt_rx_line;

	const bool bcast = strncmp("BCAST:", line, strlen("BCAST:")) == 0;
	const bool mcast = strncmp("MCAST:", line, strlen("MCAST:")) == 0;
	const bool ucast = strncmp("UCAST:", line, strlen("UCAST:")) == 0;

	if (bcast || mcast || ucast)
	{
		uint64_t eui64;
		uint8_t length;
		const int ret = siscanf(line + 6, "%llx,%hhx=", &eui64, &length);
		if (ret == 2)
		{
			const Type type = bcast ? Type::BROADCAST : mcast ? Type::MULTICAST : Type::UNICAST;
			bcast_mcast_ucast_prompt_rx_line.reset(new BcastMcastUcastPromptRxLine_(type, eui64, length, line + 26));
		}
	}

	return bcast_mcast_ucast_prompt_rx_line;
}

/*---------------------------------------------------------------------------------------------------------------------+
| public methods of JpanPromptRxLine_
+---------------------------------------------------------------------------------------------------------------------*/

/**
 * \brief Factory method for JpanPromptRxLine_
 *
 * \param [in] line is the received line
 *
 * \return unique_ptr to processed JpanPromptRxLine_ object, nullptr if object could not be created
 */

std::unique_ptr<const JpanPromptRxLine_> JpanPromptRxLine_::factory(const char * const line)
{
	std::unique_ptr<const JpanPromptRxLine_> jpan_prompt_rx_line;

	if (strncmp("JPAN:", line, strlen("JPAN:")) == 0)
	{
		uint8_t channel;
		uint16_t pid;
		uint64_t epid;
		const int ret = siscanf(line + 5, "%hhu,%hx,%llx", &channel, &pid, &epid);
		if (ret == 3)
			jpan_prompt_rx_line.reset(new JpanPromptRxLine_(channel, pid, epid));
	}

	return jpan_prompt_rx_line;
}

/*---------------------------------------------------------------------------------------------------------------------+
| public methods of OkErrorPromptRxLine_
+---------------------------------------------------------------------------------------------------------------------*/

/**
 * \brief Factory method for OkErrorPromptRxLine_
 *
 * \param [in] line is the received line
 *
 * \return unique_ptr to processed OkErrorPromptRxLine_ object, nullptr if object could not be created
 */

std::unique_ptr<const OkErrorPromptRxLine_> OkErrorPromptRxLine_::factory(const char * const line)
{
	std::unique_ptr<const OkErrorPromptRxLine_> ok_error_prompt_rx_line;

	const bool ok = strncmp("OK", line, strlen("OK")) == 0;
	const bool error = strncmp("ERROR:", line, strlen("ERROR:")) == 0;

	if (ok || error)
	{
		const uint8_t error_code = error ? strtoul(line + 6, nullptr, 16) : 0;	// OK -> error code == 0
		ok_error_prompt_rx_line.reset(new OkErrorPromptRxLine_(error_code));
	}

	return ok_error_prompt_rx_line;
}

/*---------------------------------------------------------------------------------------------------------------------+
| public methods of ResponseRxLine_
+---------------------------------------------------------------------------------------------------------------------*/

/**
 * \brief Factory method for ResponseRxLine_
 *
 * \param [in] line is the received line
 *
 * \return unique_ptr to processed ResponseRxLine_ object, always valid
 */

std::unique_ptr<const ResponseRxLine_> ResponseRxLine_::factory(const char * const line)
{
	return std::unique_ptr<const ResponseRxLine_>(new ResponseRxLine_(line));
}

/*---------------------------------------------------------------------------------------------------------------------+
| protected methods of AtiRequest_
+---------------------------------------------------------------------------------------------------------------------*/

/**
 * \brief Feeds Etrx2RxLine to AtiRequest_ object.
 *
 * \param [in] line is a string received from ETRX2 module
 *
 * \return true if line was consumed, false otherwise
 */

bool AtiRequest_::feedInternal_(const char * const line)
{
	bool consumed = true;

	if (getPhase_() == 1 && strncasecmp("Telegesis ", line, strlen("Telegesis ")) == 0)	// device name
	{
		if (deviceName_ != nullptr)
		{
			strncpy(deviceName_, line + strlen("Telegesis "), deviceNameSize_ - 1);
			deviceName_[deviceNameSize_ - 1] = '\0';
		}
	}
	else if (getPhase_() == 2 && strncmp("R", line, strlen("R")) == 0)	// firmware revision
	{
		if (firmwareRevision_ != nullptr)
		{
			strncpy(firmwareRevision_, line, firmwareRevisionSize_ - 1);
			firmwareRevision_[firmwareRevisionSize_ - 1] = '\0';
		}
	}
	else if (getPhase_() == 3)	// own 64-bit adddress
	{
		if (eui64_ != nullptr)
			*eui64_ = strtoull(line, nullptr, 16);
	}
	else
		consumed = false;

	return consumed;
}

/*---------------------------------------------------------------------------------------------------------------------+
| protected methods of AtsRequest_
+---------------------------------------------------------------------------------------------------------------------*/

/**
 * \brief Feeds Etrx2RxLine to AtsRequest_ object.
 *
 * \param [in] line is a string received from ETRX2 module
 *
 * \return true if line was consumed, false otherwise
 */

bool AtsRequest_::feedInternal_(const char * const line)
{
	if (writeData_ != nullptr)	// write operation does not consume anything
		return false;

	// read operation consumes everything
	strncpy(readData_, line, readDataSize_ - 1);
	readData_[readDataSize_ - 1] = '\0';
	return true;
}

/**
 * \brief Internal function to send command.
 *
 * \param [in] stream is the output stream
 *
 * \return 0 on success, negated errno code otherwise (errno not set)
 */

int AtsRequest_::sendCommandInternal_(FILE * const stream) const
{
	int ret;

	if (writeData_ != nullptr)	// write operation?
	{
		ret = fiprintf(stream, "%02hhx=%s", sRegister_, writeData_);
		if (ret >= 0 && password_ != nullptr)	// password given?
			ret = fiprintf(stream, ",%s", password_);
	}
	else	// read operation
		ret = fiprintf(stream, "%02hhx?", sRegister_);

	return ret >= 0 ? 0 : -EIO;
}

/*---------------------------------------------------------------------------------------------------------------------+
| protected methods of At_bcastRequest_
+---------------------------------------------------------------------------------------------------------------------*/

/**
 * \brief Internal function to send command.
 *
 * \param [in] stream is the output stream
 *
 * \return 0 on success, negated errno code otherwise (errno not set)
 */

int At_bcastRequest_::sendCommandInternal_(FILE * const stream) const
{
	const int ret = fiprintf(stream, ":%02hhx,%s", hops_, data_);
	return ret >= 0 ? 0 : -EIO;
}

/*---------------------------------------------------------------------------------------------------------------------+
| protected methods of At_dasslRequest_
+---------------------------------------------------------------------------------------------------------------------*/

/**
 * \brief Feeds Etrx2RxLine to At_dasslRequest_ object.
 *
 * \param [in] rx_line is a reference to Etrx2RxLine object received from ETRX2 module
 *
 * \return true if line was consumed, false otherwise
 */

bool At_dasslRequest_::feedInternal_(const Etrx2RxLine &rx_line)
{
	bool consumed = false;

	if (rx_line.getType() == Etrx2RxLine::Type::PROMPT)
	{
		const PromptRxLine_ &prompt_rx_line = static_cast<const PromptRxLine_ &>(rx_line);
		if (prompt_rx_line.getType() == PromptRxLine_::Type::LEFTPAN)
			consumed = true;
	}

	return consumed;
}

/*---------------------------------------------------------------------------------------------------------------------+
| protected methods of At_enAt_jnRequest_
+---------------------------------------------------------------------------------------------------------------------*/

/**
 * \brief Feeds Etrx2RxLine to At_enAt_jnRequest_ object.
 *
 * \param [in] rx_line is a reference to Etrx2RxLine object received from ETRX2 module
 *
 * \return true if line was consumed, false otherwise
 */

bool At_enAt_jnRequest_::feedInternal_(const Etrx2RxLine &rx_line)
{
	bool consumed = false;

	if (getPhase_() == 1 && rx_line.getType() == Etrx2RxLine::Type::PROMPT)
	{
		const PromptRxLine_ &prompt_rx_line = static_cast<const PromptRxLine_ &>(rx_line);
		if (prompt_rx_line.getType() == PromptRxLine_::Type::JPAN)
		{
			const JpanPromptRxLine_ &jpan_prompt_rx_line = static_cast<const JpanPromptRxLine_ &>(prompt_rx_line);
			jpan_prompt_rx_line.getParameters(channel_, pid_, epid_);
			consumed = true;
		}
	}

	return consumed;
}

/*---------------------------------------------------------------------------------------------------------------------+
| protected methods of At_escanRequest_
+---------------------------------------------------------------------------------------------------------------------*/

/**
 * \brief Feeds Etrx2RxLine to At_escanRequest_ object.
 *
 * \param [in] line is a string received from ETRX2 module
 *
 * \return true if line was consumed, false otherwise
 */

bool At_escanRequest_::feedInternal_(const char * const line)
{
	bool consumed = false;

	Etrx2::ChannelEnergy channel_energy;
	const int ret = siscanf(line, "%hhu:%hhx", &channel_energy.channel, &channel_energy.energy);
	if (ret == 2)
	{
		channelEnergies_.push_back(channel_energy);
		consumed = true;
	}

	return consumed;
}

/*---------------------------------------------------------------------------------------------------------------------+
| protected methods of At_nRequest_
+---------------------------------------------------------------------------------------------------------------------*/

/**
 * \brief Feeds Etrx2RxLine to At_nRequest_ object.
 *
 * \param [in] line is a string received from ETRX2 module
 *
 * \return true if line was consumed, false otherwise
 */

bool At_nRequest_::feedInternal_(const char * const line)
{
	bool consumed = false;

	if (strncmp("+N=", line, strlen("+N=")) == 0)
	{
		if(strncmp("NoPAN", line + 3, strlen("NoPAN")) == 0)	// node is not part of any PAN
		{
			if (networkFunction_ != nullptr)
				*networkFunction_ = Etrx2::NetworkFunction::NONE;
			consumed = true;
		}
		else	// node is part of PAN
		{
			Etrx2::NetworkFunction network_function = Etrx2::NetworkFunction::NONE;

			if(strncmp("COO", line + 3, strlen("COO")) == 0)
				network_function = Etrx2::NetworkFunction::COO;
			else if(strncmp("FFD", line + 3, strlen("FFD")) == 0)
				network_function = Etrx2::NetworkFunction::FFD;
			else if(strncmp("MED", line + 3, strlen("MED")) == 0)
				network_function = Etrx2::NetworkFunction::MED;
			else if(strncmp("SED", line + 3, strlen("SED")) == 0)
				network_function = Etrx2::NetworkFunction::SED;
			else if(strncmp("ZED", line + 3, strlen("ZED")) == 0)
				network_function = Etrx2::NetworkFunction::ZED;

			if (network_function != Etrx2::NetworkFunction::NONE)
			{
				uint8_t channel, power;
				uint16_t pid;
				uint64_t epid;
				const int ret = siscanf(line + 7, "%hhu,%hhu,%hx,%llx", &channel, &power, &pid, &epid);
				if (ret == 4)
				{
					if (networkFunction_ != nullptr)
						*networkFunction_ = network_function;
					if (channel_ != nullptr)
						*channel_ = channel;
					if (power_ != nullptr)
						*power_ = power;
					if (pid_ != nullptr)
						*pid_ = pid;
					if (epid_ != nullptr)
						*epid_ = epid;

					consumed = true;
				}
			}
		}
	}

	return consumed;
}

/*---------------------------------------------------------------------------------------------------------------------+
| protected methods of At_panscanRequest_
+---------------------------------------------------------------------------------------------------------------------*/

/**
 * \brief Feeds Etrx2RxLine to At_panscanRequest_ object.
 *
 * \param [in] line is a string received from ETRX2 module
 *
 * \return true if line was consumed, false otherwise
 */

bool At_panscanRequest_::feedInternal_(const char * const line)
{
	bool consumed = false;

	Etrx2::FoundNetwork found_network;
	uint8_t profile_number;
	uint8_t joining_permitted_number;
	const int ret = siscanf(line, "+PANSCAN:%hhd,%hx,%llx,%hhd,%hhd", &found_network.channel, &found_network.pid,
			&found_network.epid, &profile_number, &joining_permitted_number);
	if (ret == 5 && profile_number <= 2 && joining_permitted_number <= 1)
	{
		found_network.zigbeeStackProfile = static_cast<Etrx2::ZigbeeStackProfile>(profile_number);
		found_network.joiningPermitted = joining_permitted_number;
		foundNetworks_.push_back(found_network);
		consumed = true;
	}

	return consumed;
}

/*---------------------------------------------------------------------------------------------------------------------+
| protected methods of At_ucastRequest_
+---------------------------------------------------------------------------------------------------------------------*/

/**
 * \brief Feeds Etrx2RxLine to At_ucastRequest_ object.
 *
 * \param [in] line is a string received from ETRX2 module
 *
 * \return true if line was consumed, false otherwise
 */

bool At_ucastRequest_::feedInternal_(const char * const line)
{
	bool consumed = false;

	if (strncmp("SEQ:", line, strlen("SEQ:")) == 0)
	{
		seqenceNumber_ = strtoul(line + 4, nullptr, 16);
		consumed = true;
	}

	return consumed;
}

/**
 * \brief Feeds Etrx2RxLine to At_ucastRequest_ object.
 *
 * \param [in] rx_line is a reference to Etrx2RxLine object received from ETRX2 module
 *
 * \return true if line was consumed, false otherwise
 */

bool At_ucastRequest_::feedInternal_(const Etrx2RxLine &rx_line)
{
	bool consumed = false;

	if (getPhase_() == 1)
		consumed = Etrx2Request::feedInternal_(rx_line);
	else if (getPhase_() == 3 && rx_line.getType() == Etrx2RxLine::Type::PROMPT)
	{
		const PromptRxLine_ &prompt_rx_line = static_cast<const PromptRxLine_ &>(rx_line);
		if (prompt_rx_line.getType() == PromptRxLine_::Type::ACK_NACK)
		{
			const AckNackPromptRxLine_ &ack_nack_prompt_rx_line =
					static_cast<const AckNackPromptRxLine_ &>(prompt_rx_line);
			uint8_t sequence_number;
			ack_nack_prompt_rx_line.getParameters(acknowledged_, &sequence_number);
			if (sequence_number == seqenceNumber_)
				consumed = true;
		}
	}

	return consumed;
}

/**
 * \brief Internal function to send command.
 *
 * \param [in] stream is the output stream
 *
 * \return 0 on success, negated errno code otherwise (errno not set)
 */

int At_ucastRequest_::sendCommandInternal_(FILE * const stream) const
{
	const int ret = fiprintf(stream, ":%016llx=%s", address_, data_);
	return ret >= 0 ? 0 : -EIO;
}

}	// namespace
