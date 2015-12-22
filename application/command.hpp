/**
 * \file command.hpp
 * \brief Header for command.cpp
 * \author Freddie Chopin, http://www.freddiechopin.info http://www.distortec.com
 * \author: Mazeryt Freager, http://www.gotoc.co
 */

#ifndef COMMAND_HPP_
#define COMMAND_HPP_

#include <cstdint>
#include <cstdio>

/*---------------------------------------------------------------------------------------------------------------------+
| global variables' types
+---------------------------------------------------------------------------------------------------------------------*/

/// entry to the commands list
struct CommandDefinition
{
	/// typedef of handler function
	typedef int (&Handler)(const char **arguments_array, uint32_t arguments_count, FILE *output_stream);

	/// command string
	const char *command;

	/// maximum number of arguments
	uint32_t argumentsCountMax;

	/// handler function
	Handler handler;

	/// string displayed by help function
	const char *helpString;
};

/// struct for map of key-value pairs formed of arguments
struct CommandArgumentPair
{
	/// key - without preceeding "--"
	const char *key;

	/// value associated with key
	const char *value;
};

/*---------------------------------------------------------------------------------------------------------------------+
| global functions' prototypes
+---------------------------------------------------------------------------------------------------------------------*/

uint32_t commandArgumentsToPairs(const char * const * const arguments_array, const uint32_t arguments_count,
		CommandArgumentPair * const pairs, const size_t pairs_length);
int commandProcessInput(char * const input, FILE * const output_stream);
int commandRegister(const CommandDefinition &definition);

#endif	// COMMAND_HPP_
