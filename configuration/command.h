/**
 * \file command.h
 *
 * \brief Header for command.cpp
 */

#ifndef COMMAND_H_
#define COMMAND_H_

#include <stdint.h>
#include <stddef.h>

#include "error.h"

/*---------------------------------------------------------------------------------------------------------------------+
| global variables' types
+---------------------------------------------------------------------------------------------------------------------*/

/// typedef of calback functions
typedef enum Error (*CommandCallback)(const char **arguments_array, uint32_t arguments_count, char *output_buffer,
		size_t output_buffer_length);

/// entry to the commands list
struct CommandDefinition
{
	const char * const command;				///< command string
	uint32_t arguments_count_max;			///< maximum number of arguments
	const CommandCallback callback;		///< callback function
	const char * const help_string;			///< string displayed by help function
};

/// struct for map of key-value pairs formed of arguments
struct CommandArgumentPair
{
	const char *key;						///< key - without preceeding "--"
	const char *value;						///< value associated with key
};

/*---------------------------------------------------------------------------------------------------------------------+
| global functions' prototypes
+---------------------------------------------------------------------------------------------------------------------*/

uint32_t commandArgumentsToPairs(const char **arguments_array, uint32_t arguments_count,
		struct CommandArgumentPair *pairs, size_t pairs_length);
enum Error commandProcessInput(char *input, char *output_buffer, size_t output_buffer_length);
void commandRegister(const struct CommandDefinition *definition);

#endif /* COMMAND_H_ */
