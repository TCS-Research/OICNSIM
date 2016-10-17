/* -*- Mode:C++; c-file-style; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2016, CTO Networks Lab, Tata Consultancy Services (TCS)
 *
 * Author: Suvrat Agrawal <suv.agr@gmail.com>
 */

#ifndef DNSPLUS_QUESTION_HEADER_H_
#define DNSPLUS_QUESTION_HEADER_H_

#include <stdint.h>
#include <string>
#include "ns3/header.h"
#include "ns3/type-name.h"
#include <stdint.h>
#include <string>
#include <cstring>


namespace ns3 {

/**
 * \brief It is similar to DNS Question Section header.
 * it contains the name of the content to be queried.
 **/

class DnsPlusQuestionHeader: public Header {
public:
	DnsPlusQuestionHeader();
	virtual ~DnsPlusQuestionHeader();
	/**
	 * \brief Get the type ID.
	 * \return the object TypeId
	 */
	static TypeId GetTypeId (void);

	virtual TypeId GetInstanceTypeId (void) const;
	virtual uint32_t GetSerializedSize (void) const;
	virtual void Serialize (Buffer::Iterator start) const;
	virtual uint32_t Deserialize (Buffer::Iterator start);
	virtual void Print (std::ostream &os) const;
	//Setters
	/**
	 * \brif Set the length of the name
	 * \param nameLength length of the name
	 */

	void SetNameLength( uint32_t nameLength);
	/**
	 * \brif Set the name of the content
	 * \param name name of the content
	 */

	void SetName(std::string name);

	/**
	 * \brief Set the type of data
	 * \param qType type of data
	 */

	void SetQtype(uint16_t qType);
	/**
	 * \brief Set the class of data
	 * \param qClass class of data
	 */

	void SetQclass(uint16_t qClass);

	/**
	 * \brief Set the Reserved flag, these are not currently used, reserved for future use
	 * \param reservedFlag reserved flag bit
	 */

	void SetReservedFlag(uint16_t reservedFlag);

//Getters

	/**
	 * \brief Get the name of content.
	 * \return Name of content
	 */
	std::string  GetName();

	/**
	 * \brief Get the class of the query.
	 * \return class of the query
	 */
	uint16_t GetQclass();

	/**
	 * \brief Get the type of the query.
	 * \return type of the query
	 */
	uint16_t GetQtype();

	/**
	 * \brief Get the reserved flag.
	 * \return Reserved flag
	 */
	uint8_t GetReservedFlag();

	/**
	 * \brief Get the lenght of the name.
	 * \return Length of the name
	 */
	uint32_t GetNameLength();

private :

	uint8_t m_reservedFlag;//!< reserved flag
	std::string m_name;//!< name of the content
	uint16_t m_qClass;//!< class of the query
	uint16_t m_qType;//!< type of the query
	uint32_t m_nameLength;//!< length of the name

};


}
#endif /* DNSPLUS_QUESTION_HEADER_H_ */
