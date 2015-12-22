/**
 * \file command.cpp
 * \brief Command interpreter
 *
 * prefix: command
 *
 * \author Freddie Chopin, http://www.freddiechopin.info http://www.distortec.com
 * \author: Mazeryt Freager, http://www.gotoc.co
 */

#include "command.hpp"

#include <new>

#include <cstring>
#include <cerrno>

namespace
{

/*---------------------------------------------------------------------------------------------------------------------+
| local variables' types
+---------------------------------------------------------------------------------------------------------------------*/

/// list element holding the command definition
struct ListEntry
{
	/// command definition
	const CommandDefinition *definition;

	/// ptr to next item on the list
	ListEntry *next;
};

/*---------------------------------------------------------------------------------------------------------------------+
| local functions' prototypes
+---------------------------------------------------------------------------------------------------------------------*/

int helpHandler_(const char **arguments_array, uint32_t arguments_count, FILE *output_stream);

/*---------------------------------------------------------------------------------------------------------------------+
| local variables
+---------------------------------------------------------------------------------------------------------------------*/

/// definition of "help" command
const CommandDefinition helpCommandDefinition_ =
{
		"help",				// command string
		0,					// maximum number of arguments
		helpHandler_,		// handler function
		"help: lists all available commands\n",	// string displayed by help function
};

/// head of list holding all registered commands, always starts with "help" handler
ListEntry listHead_ =
{
		&helpCommandDefinition_,
		nullptr,
};

}	// namespace

/*---------------------------------------------------------------------------------------------------------------------+
| global functions
+---------------------------------------------------------------------------------------------------------------------*/

/**
 * \brief Converts arguments to map of key-value pairs.
 *
 * Key is identified by double hyphen. Values are optional.
 *
 * \param [in] arguments_array is an array with arguments
 * \param [in] arguments_count is the number or arguments on the arguments_array
 * \param [out] pairs is the output array with pairs
 * \param [in] pairs_length is the size of externally provided pairs array
 *
 * \return number of found pairs
 */

uint32_t commandArgumentsToPairs(const char * const * const arguments_array, const uint32_t arguments_count,
		CommandArgumentPair * const pairs, const size_t pairs_length)
{
	pairs[0].key = nullptr;

	uint32_t i = 0, j = 0;
	uint32_t found = 0;

	while (i < arguments_count && j < pairs_length)
	{
		const char * const argument = arguments_array[i++];

		// if the argument is in form --something - it's a key
		if (argument[0] == '-' && argument[1] == '-' && argument[2] != '\0')
		{
			// if current key in pairs array is valid and another key was found - increment the index
			if (pairs[j].key != nullptr)
				j++;

			pairs[j].key = &argument[2];
			pairs[j].value = nullptr;			// initially value is unknown

			found++;
		}
		else								// otherwise it's a value
			pairs[j].value = argument;
	}

	return found;
}

/**
 * \brief Processes command input string.
 *
 * \param [in] input is input string
 * \param [out] output_stream is the stream used for output
 *
 * \return 0 on success, negated errno code otherwise (errno not set)
 */

int commandProcessInput(char * const input, FILE * const output_stream)
{
	ListEntry *ptr = &listHead_;

	int error = -EINVAL;	// initially assume that command was not found

	char *lasts;
	const char *argument = strtok_r(input, " \t\n", &lasts);	// get the command itself (first word)
	if (argument == nullptr)
		return error;

	while (ptr != nullptr)
	{
		if (strcmp(ptr->definition->command, argument) == 0)	// was the input matched to command?
		{
			// always make space for command name - first entry
			const uint32_t arguments_count_max = ptr->definition->argumentsCountMax + 1;

			const char ** const arguments_array = new (std::nothrow) const char*[arguments_count_max];

			if (arguments_array == nullptr)
				return -ENOMEM;

			uint32_t arguments_count = 0;
			arguments_array[arguments_count++] = argument;	// first store the command (first word)

			while (arguments_count < arguments_count_max)
			{
				argument = strtok_r(nullptr, " \t\n", &lasts);

				if (argument == nullptr)	// valid argument?
					break;					// no - no more arguments available, so break...

				arguments_array[arguments_count++] = argument;
			}

			error = ptr->definition->handler(arguments_array, arguments_count, output_stream);	// execute handler

			delete[] arguments_array;

			break;
		}

		ptr = ptr->next;					// advance list
	}

	return error;
}

/**
 * \brief Registers new command to command interpreter.
 *
 * This function should be called BEFORE the scheduler is started, as it's not thread-safe.
 *
 * \param [in] definition is a reference to command definition struct, it should be in flash or available through entire
 * program, as only the pointer is copied
 *
 * \return 0 on success, negated errno code otherwise (errno not set)
 */

int commandRegister(const CommandDefinition &definition)
{
	ListEntry * const new_entry = new ListEntry;

	if (new_entry == nullptr)
		return -ENOMEM;

	new_entry->definition = &definition;
	new_entry->next = nullptr;

	ListEntry *ptr = &listHead_;

	while (ptr->next != nullptr)
		ptr = ptr->next;

	ptr->next = new_entry;

	return 0;
}

/*---------------------------------------------------------------------------------------------------------------------+
| local functions
+---------------------------------------------------------------------------------------------------------------------*/

namespace
{

/**
 * \brief Handler of 'help' command.
 *
 * Displays all available commands' help strings.
 *
 * \param [out] output_stream is the stream used for output
 *
 * \return 0 on success, negated errno code otherwise (errno not set)
 */

int helpHandler_(const char **, uint32_t, FILE *output_stream)
{
	const ListEntry *ptr = &listHead_;

	while (ptr != nullptr)
	{
		if (fputs(ptr->definition->helpString, output_stream) == EOF)
			return -EIO;

		ptr = ptr->next;					// advance list
	}

	return 0;
}

}	// namespace
