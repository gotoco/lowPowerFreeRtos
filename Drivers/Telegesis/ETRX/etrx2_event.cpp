/**
 * \file etrx2_event.cpp
 * \brief Etrx2Event class implementation
 *
 * hardware: Telegesis ETRX2 module
 *
 * \author Freddie Chopin, http://www.freddiechopin.info http://www.distortec.com
 * \author: Mazeryt Freager, http://www.gotoc.co
 */

#include "etrx2_event.hpp"

#include <cstring>

/*---------------------------------------------------------------------------------------------------------------------+
| public functions of Etrx2MessageEvent
+---------------------------------------------------------------------------------------------------------------------*/

/**
 * \brief Etrx2MessageEvent constructor - allocates memory and sets internal variables.
 *
 * \param [in] type is the type of *cast, Type::{BROADCAST, MULTICAST, UNICAST}
 * \param [in] eui64 is the EUI64 of sender
 * \param [in] length is the length of data string, without terminating '\0'
 * \param [in] data is the data string, alias pointing to part of RX line
 */

Etrx2MessageEvent::Etrx2MessageEvent(const Type type, const uint64_t eui64, const uint8_t length,
		const char * const data) :
		Etrx2Event(Etrx2Event::Type::MESSAGE),
		eui64_(eui64),
		data_(strcpy(new char[length + 1], data)),
		length_(length),
		type_(type)
{};

/// \brief Etrx2MessageEvent destructor - de-allocates memory.

Etrx2MessageEvent::~Etrx2MessageEvent()
{
	delete[] data_;
}
