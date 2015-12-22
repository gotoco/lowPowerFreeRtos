/**
 * \file etrx2_cli.cpp
 * \brief Etrx2 command-line interface implementation
 *
 * hardware: Telegesis ETRX2 module
 *
 * \author: Mazeryt Freager, http://www.gotoc.co
 */

#include "etrx2_cli.hpp"

#include "etrx2.hpp"
#include "command.hpp"

#include <cerrno>
#include <cstring>
#include <cstdlib>

namespace
{

/*---------------------------------------------------------------------------------------------------------------------+
| private functions' declarations
+---------------------------------------------------------------------------------------------------------------------*/

int energyScanHandler_(const char **, uint32_t, FILE * const output_stream);
int etrx2InfoHandler_(const char **, uint32_t, FILE * const output_stream);
int etrx2SregisterHandler_(const char **arguments_array, uint32_t arguments_count, FILE *output_stream);
int etrx2StatsHandler_(const char **, uint32_t, FILE * const output_stream);
int networkConnectEstablishHandler_(const char **arguments_array, uint32_t, FILE *output_stream);
int networkDisconnectHandler_(const char **, uint32_t, FILE * const output_stream);
int networkInfoHandler_(const char **, uint32_t, FILE * const output_stream);
int networkSearchHandler_(const char **, uint32_t, FILE * const output_stream);
int transmitBroadcastHandler_(const char **arguments_array, uint32_t arguments_count, FILE *output_stream);
int transmitUnicastHandler_(const char **arguments_array, uint32_t arguments_count, FILE *output_stream);

/*---------------------------------------------------------------------------------------------------------------------+
| private variables
+---------------------------------------------------------------------------------------------------------------------*/

/// definition of "energy_scan" command
const CommandDefinition energyScanCommandDefinition_ =
{
		"energy_scan",			// command string
		0,						// maximum number of arguments
		energyScanHandler_,		// handler function
		"energy_scan: scan energy on all channels\n",	// string displayed by help function
};

/// definition of "etrx2_info" command
const CommandDefinition etrx2InfoCommandDefinition_ =
{
		"etrx2_info",			// command string
		0,						// maximum number of arguments
		etrx2InfoHandler_,		// handler function
		"etrx2_info: displays info about connected ETRX2 module\n",	// string displayed by help function
};

/// definition of "etrx2_sregister" command
const CommandDefinition etrx2SregisterCommandDefinition_ =
{
		"etrx2_sregister",		// command string
		3,						// maximum number of arguments
		etrx2SregisterHandler_,	// handler function
		"etrx2_sregister s_register [data] [password]: accesses S-Register\n"
		"\ts_register - decimal, hexadecimal (\"0x\" or \"0X\" prefix) or octal (\"0\" prefix),\n"
		"\tdata - data to be written,\n"
		"\tpassword - password for write access, if required\n",	// string displayed by help function
};

/// definition of "etrx2_stats" command
const CommandDefinition etrx2StatsCommandDefinition_ =
{
		"etrx2_stats",			// command string
		0,						// maximum number of arguments
		etrx2StatsHandler_,		// handler function
		"etrx2_stats: displays ETRX2 statistics\n",	// string displayed by help function
};

/// definition of "network_connect" command
const CommandDefinition networkConnectCommandDefinition_ =
{
		"network_connect",		// command string
		0,						// maximum number of arguments
		networkConnectEstablishHandler_,	// handler function
		"network_connect: connect local node to any network\n",	// string displayed by help function
};

/// definition of "network_disconnect" command
const CommandDefinition networkDisconnectCommandDefinition_ =
{
		"network_disconnect",	// command string
		0,						// maximum number of arguments
		networkDisconnectHandler_,	// handler function
		"network_disconnect: disconnect local node from network\n",	// string displayed by help function
};

/// definition of "network_establish" command
const CommandDefinition networkEstablishCommandDefinition_ =
{
		"network_establish",	// command string
		0,						// maximum number of arguments
		networkConnectEstablishHandler_,	// handler function
		"network_establish: establish network\n",	// string displayed by help function
};

/// definition of "network_info" command
const CommandDefinition networkInfoCommandDefinition_ =
{
		"network_info",			// command string
		0,						// maximum number of arguments
		networkInfoHandler_,	// handler function
		"network_info: displays info about current ZigBee network\n",	// string displayed by help function
};

/// definition of "network_search" command
const CommandDefinition networkSearchCommandDefinition_ =
{
		"network_search",		// command string
		0,						// maximum number of arguments
		networkSearchHandler_,	// handler function
		"network_search: searches for active networks\n",	// string displayed by help function
};

/// definition of "transmit_broadcast" command
const CommandDefinition transmitBroadcastCommandDefinition_ =
{
		"transmit_broadcast",	// command string
		2,						// maximum number of arguments
		transmitBroadcastHandler_,	// handler function
		"transmit_broadcast [hops] data: transmit broadcast\n"
		"\thops - decimal, hexadecimal (\"0x\" or \"0X\" prefix) or octal (\"0\" prefix),\n"
		"\tdefault = 0 (entire network)\n"
		"\tdata - no whitespace allowed\n",	// string displayed by help function
};

/// definition of "transmit_unicast" command
const CommandDefinition transmitUnicastCommandDefinition_ =
{
		"transmit_unicast",		// command string
		2,						// maximum number of arguments
		transmitUnicastHandler_,	// handler function
		"transmit_unicast address data: transmit unicast\n"
		"\taddress - decimal, hexadecimal (\"0x\" or \"0X\" prefix) or octal (\"0\" prefix)\n"
		"\tdata - no whitespace allowed\n",	// string displayed by help function
};

/// all commands for Etrx2 command-line interface
const CommandDefinition * const commandDefinitions_[] =
{
		&energyScanCommandDefinition_,
		&etrx2InfoCommandDefinition_,
		&etrx2SregisterCommandDefinition_,
		&etrx2StatsCommandDefinition_,
		&networkConnectCommandDefinition_,
		&networkDisconnectCommandDefinition_,
		&networkEstablishCommandDefinition_,
		&networkInfoCommandDefinition_,
		&networkSearchCommandDefinition_,
		&transmitBroadcastCommandDefinition_,
		&transmitUnicastCommandDefinition_,
};

/// instance of Etrx2 object, used by commands
Etrx2 *etrx2_;

}	// namespace

/*---------------------------------------------------------------------------------------------------------------------+
| public functions
+---------------------------------------------------------------------------------------------------------------------*/

/**
 * \brief Initializes Etrx2 command-line interface.
 *
 * Saves object instance and registers commands.
 *
 * \param [in] etrx2 is a reference to Etrx2 object on which the commands will operate
 *
 * \return 0 on success, negated errno code otherwise (errno not set)
 */

int etrx2CliInitialize(Etrx2 &etrx2)
{
	etrx2_ = &etrx2;

	for (const CommandDefinition * const command_definition : commandDefinitions_)
	{
		const int ret = commandRegister(*command_definition);
		if (ret != 0)	// registration failure?
			return ret;
	}

	return 0;
}

namespace
{

/*---------------------------------------------------------------------------------------------------------------------+
| private functions
+---------------------------------------------------------------------------------------------------------------------*/

/**
 * \brief Handler of "energy_scan" command.
 *
 * Scans energy on all channels.
 *
 * \param [out] output_stream is the stream used for output
 *
 * \return 0 on success, negated errno code otherwise (errno not set) or error code returned by ETRX2 module (positive
 * value)
 */

int energyScanHandler_(const char **, uint32_t, FILE * const output_stream)
{
	std::vector<Etrx2::ChannelEnergy> channel_energies;
	channel_energies.reserve(16);

	int ret = etrx2_->scanEnergy(channel_energies);

	if (ret == 0)
	{
		ret = fputs("Channel    Energy\n-----------------\n", output_stream);
		ret = ret >= 0 ? 0 : -EIO;
	}

	for (auto i = channel_energies.cbegin(); i != channel_energies.cend() && ret == 0; ++i)
	{
		ret = fiprintf(output_stream, "%-10hhu %hhu\n", i->channel, i->energy);
		ret = ret >= 0 ? 0 : -EIO;
	}

	return ret;
}

/**
 * \brief Handler of "etrx2_info" command.
 *
 * Displays info about connected ETRX2 module (taken from "ATI" command").
 *
 * \param [out] output_stream is the stream used for output
 *
 * \return 0 on success, negated errno code otherwise (errno not set) or error code returned by ETRX2 module (positive
 * value)
 */

int etrx2InfoHandler_(const char **, uint32_t, FILE * const output_stream)
{
	char device_name[16], firmware_revision[8];
	uint64_t eui64;

	int ret = etrx2_->getModuleInfo(device_name, sizeof(device_name), firmware_revision, sizeof(firmware_revision),
			&eui64);

	if (ret == 0)
	{
		ret = fiprintf(output_stream, "Device name - %s\nFirmware revision - %s\nEUI64 = %#llx\n",
				device_name, firmware_revision, eui64);
		ret = ret >= 0 ? 0 : -EIO;
	}

	return ret;
}

/**
 * \brief Handler of "etrx2_sregister" command.
 *
 * Accesses S-Register
 *
 * \param [in] arguments_array is the array with arguments, first elements is the command string
 * \param [in] arguments_count is the number of arguments in arguments_array
 * \param [out] output_stream is the stream used for output
 *
 * \return 0 on success, negated errno code otherwise (errno not set)
 */

int etrx2SregisterHandler_(const char **arguments_array, uint32_t arguments_count, FILE *output_stream)
{
	if (arguments_count < 2)	// at least S-Register must be given
		return -EINVAL;

	const uint8_t s_register = strtoul(arguments_array[1], nullptr, 0);
	const bool write = arguments_count > 2;
	const char * const write_data = write ? arguments_array[2] : nullptr;
	const char * const password = arguments_count == 4 ? arguments_array[3] : nullptr;
	char buffer[64];	// the longest string that can be read is 60 characters
	char * const read_data = !write ? buffer : nullptr;

	int ret = etrx2_->sRegisterAccess(s_register, write_data, password, read_data, sizeof(buffer));
	if (ret != 0)
		return ret;

	if (write)
		ret = fputs("Done\n", output_stream);
	else
		ret = fiprintf(output_stream, "Value = %s\n", buffer);

	return ret >= 0 ? 0 : -EIO;
}

/**
 * \brief Handler of "etrx2_stats" command.
 *
 * Displays ETRX2 statistics.
 *
 * \param [out] output_stream is the stream used for output
 *
 * \return 0 on success, negated errno code otherwise (errno not set)
 */

int etrx2StatsHandler_(const char **, uint32_t, FILE * const output_stream)
{
	uint32_t processed_commands, received_lines, dropped_lines;

	etrx2_->getModuleStats(processed_commands, received_lines, dropped_lines);

	const int ret = fiprintf(output_stream, "Processed commands = %lu\nReceived lines = %lu\n"
			"Dropped lines = %lu\n",
			processed_commands, received_lines, dropped_lines);

	return ret >= 0 ? 0 : -EIO;
}

/**
 * \brief Handler of "network_connect" and "network_establish" commands.
 *
 * Connects local node to any network or establishes network.
 *
 * \param [in] arguments_array is the array with arguments, first elements is the command string
 * \param [out] output_stream is the stream used for output
 *
 * \return 0 on success, negated errno code otherwise (errno not set) or error code returned by ETRX2 module (positive
 * value)
 */

int networkConnectEstablishHandler_(const char **arguments_array, uint32_t, FILE *output_stream)
{
	uint8_t channel;
	uint16_t pid;
	uint64_t epid;

	// check whether "network_connect" or "network_establish" was called
	const bool new_network = strcmp(arguments_array[0], networkEstablishCommandDefinition_.command) == 0;

	int ret = new_network ? etrx2_->establishNetwork(&channel, &pid, &epid) : etrx2_->connect(&channel, &pid, &epid);

	if (ret == 0)
	{
		ret = fiprintf(output_stream, "%s!\nEPID = %#llx\nPID = %#hx\nChannel = %hhu\n",
				new_network ? "Established" : "Connected", epid, pid, channel);
		ret = ret >= 0 ? 0 : -EIO;
	}

	return ret;
}

/**
 * \brief Handler of "network_disconnect" command.
 *
 * Disconnects local node from network.
 *
 * \param [out] output_stream is the stream used for output
 *
 * \return 0 on success, negated errno code otherwise (errno not set) or error code returned by ETRX2 module (positive
 * value)
 */

int networkDisconnectHandler_(const char **, uint32_t, FILE * const output_stream)
{
	int ret = etrx2_->disconnect();

	if (ret == 0)
	{
		ret = fputs("Disconnected!\n", output_stream);
		ret = ret >= 0 ? 0 : -EIO;
	}

	return ret;
}

/**
 * \brief Handler of "network_info" command.
 *
 * Displays info about network (taken from "AT+N" command").
 *
 * \param [out] output_stream is the stream used for output
 *
 * \return 0 on success, negated errno code otherwise (errno not set) or error code returned by ETRX2 module (positive
 * value)
 */

int networkInfoHandler_(const char **, uint32_t, FILE * const output_stream)
{
	uint8_t channel, power;
	Etrx2::NetworkFunction network_function;
	uint64_t epid;
	uint16_t pid;

	int ret = etrx2_->getNetworkInfo(&epid, &pid, &network_function, &channel, &power);

	if (ret == 0)
	{
		if (network_function == Etrx2::NetworkFunction::NONE)
			ret = fputs("Not connected\n", output_stream);
		else
		{
			const char *network_function_string;

			if (network_function == Etrx2::NetworkFunction::COO)
				network_function_string = "Coordinator";
			else if (network_function == Etrx2::NetworkFunction::FFD)
				network_function_string = "Router";
			else if (network_function == Etrx2::NetworkFunction::MED)
				network_function_string = "Mobile Sleepy End Device";
			else if (network_function == Etrx2::NetworkFunction::SED)
				network_function_string = "Sleepy End Device";
			else /* if (network_function == Etrx2::NetworkFunction::ZED) */
				network_function_string = "End Device";

			ret = fiprintf(output_stream, "Network function - %s\nEPID = %#llx\nPID = %#hx\nChannel = %hhu\n"
					"Power = %hhudBm\n", network_function_string, epid, pid, channel, power);
		}

		ret = ret >= 0 ? 0 : -EIO;
	}

	return ret;
}

/**
 * \brief Handler of "network_search" command.
 *
 * Searches for active networks.
 *
 * \param [out] output_stream is the stream used for output
 *
 * \return 0 on success, negated errno code otherwise (errno not set) or error code returned by ETRX2 module (positive
 * value)
 */

int networkSearchHandler_(const char **, uint32_t, FILE * const output_stream)
{
	static const char *zigbee_profile_strings[] = {"Custom", "ZigBee", "ZigBee PRO"};

	std::vector<Etrx2::FoundNetwork> found_networks;
	found_networks.reserve(16);

	int ret = etrx2_->searchNetworks(found_networks);

	if (ret == 0)
	{
		if (found_networks.size() != 0)
		{
			ret = fputs("EPID                  PID       Ch.    J.     Profile\n"
					"--------------------------------------------------------\n", output_stream);
			ret = ret >= 0 ? 0 : -EIO;

			for (auto i = found_networks.cbegin(); i != found_networks.cend() && ret == 0; ++i)
			{
				ret = fiprintf(output_stream, "%#-21llx %#-9hx %-6hhu %-6s %s\n", i->epid, i->pid, i->channel,
						i->joiningPermitted ? "Yes" : "No",
						zigbee_profile_strings[static_cast<uint32_t>(i->zigbeeStackProfile)]);
				ret = ret >= 0 ? 0 : -EIO;
			}
		}
		else
		{
			ret = fputs("No networks...\n", output_stream);
			ret = ret >= 0 ? 0 : -EIO;
		}
	}

	return ret;
}

/**
 * \brief Handler of "transmit_broadcast" command.
 *
 * Transmits broadcast.
 *
 * \param [in] arguments_array is the array with arguments, first elements is the command string
 * \param [in] arguments_count is the number of arguments in arguments_array
 * \param [out] output_stream is the stream used for output
 *
 * \return 0 on success, negated errno code otherwise (errno not set) or error code returned by ETRX2 module (positive
 * value)
 */

int transmitBroadcastHandler_(const char **arguments_array, uint32_t arguments_count, FILE *output_stream)
{
	if (arguments_count < 2)	// at least data must be given
		return -EINVAL;

	uint8_t hops;
	const char *data;

	if (arguments_count == 2)	// hops not given?
	{
		hops = 0;
		data = arguments_array[1];
	}
	else	// hops given?
	{
		hops = strtoul(arguments_array[1], nullptr, 0);
		data = arguments_array[2];
	}

	int ret = etrx2_->transmitBroadcast(hops, data);

	if (ret == 0)
	{
		ret = fputs("Broadcast sent\n", output_stream);
		ret = ret >= 0 ? 0 : -EIO;
	}

	return ret;
}

/**
 * \brief Handler of "transmit_unicast" command.
 *
 * Transmits unicast.
 *
 * \param [in] arguments_array is the array with arguments, first elements is the command string
 * \param [in] arguments_count is the number of arguments in arguments_array
 * \param [out] output_stream is the stream used for output
 *
 * \return 0 on success, negated errno code otherwise (errno not set) or error code returned by ETRX2 module (positive
 * value)
 */

int transmitUnicastHandler_(const char **arguments_array, uint32_t arguments_count, FILE *output_stream)
{
	int ret = -EINVAL;

	if (arguments_count == 3)	// both address and data must be given
	{
		const uint64_t address = strtoull(arguments_array[1], nullptr, 0);
		uint8_t sequence_number;
		bool acknowledged;

		ret = etrx2_->transmitUnicast(address, arguments_array[2], &sequence_number, &acknowledged);

		if (ret == 0)
		{
			ret = fiprintf(output_stream, "Sequence number = %hhu\nAcknowledge%sreceived\n",
					sequence_number, acknowledged ? " " : " not ");
			ret = ret >= 0 ? 0 : -EIO;
		}
	}

	return ret;
}

}	// namespace
