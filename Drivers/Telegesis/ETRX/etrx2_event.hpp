/**
 * \file etrx2_event.hpp
 * \brief Etrx2Event class header
 * \author Freddie Chopin, http://www.freddiechopin.info http://www.distortec.com
 * \author: Mazeryt Freager, http://www.gotoc.co
 * \date 2013-05-20
 */

#ifndef ETRX2_EVENT_HPP_
#define ETRX2_EVENT_HPP_

#include <cstdint>

/// event coming from ETRX2 module
class Etrx2Event
{
private:

	using Dummy_ = int;	// this is here only to prevent Eclipse's parser errors...

public:

	/// type of event
	enum class Type
	{
		/// message received (broadcast, multicast or unicast)
		MESSAGE,
	};

	/**
	 * \brief Etrx2Event constructor - just sets internal variables.
	 *
	 * \param [in] type is the type of event
	 */

	constexpr Etrx2Event(const Type type) :
			type_(type)
	{};

	virtual ~Etrx2Event() {};

	/// \brief returns type of event
	Type getType() const { return type_; };

private:

	/// type of event
	const Type type_;
};

/// message event - broadcast, multicast or unicast
class Etrx2MessageEvent : public Etrx2Event
{
private:

	using Dummy_ = int;	// this is here only to prevent Eclipse's parser errors...

public:

	/// type of received message
	enum class Type
	{
		BROADCAST,		///< broadcast
		MULTICAST,		///< multicast
		UNICAST,		///< unicast
	};

	Etrx2MessageEvent(const Type type, const uint64_t eui64, const uint8_t length, const char * const data);

	virtual ~Etrx2MessageEvent() override;

	/**
	 * \brief Returns (via pointers) parameters embedded in this event
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

private:

	/// EUI64 of sender
	const uint64_t eui64_;

	/// data string, allocated dynamically
	const char * const data_;

	/// length of data_, not including terminating '\0'
	const uint8_t length_;

	/// type of *cast
	const Type type_;
};

#endif	// ETRX2_EVENT_HPP_
