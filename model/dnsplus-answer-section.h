/* -*- Mode:C++; c-file-style; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2016, CTO Networks Lab, Tata Consultancy Services (TCS)
 *
 * Author: Suvrat Agrawal <suv.agr@gmail.com>
 */

#ifndef DNSPLUS_ANSWER_SECTION_H_
#define DNSPLUS_ANSWER_SECTION_H_

#include <stdint.h>
#include <string>
#include "ns3/header.h"
#include "ns3/type-name.h"
#include <stdint.h>
#include <string>
#include <cstring>
#include "ns3/ipv4-address.h"


namespace ns3 {

/**
 * \brief It is similar to DNS Answer Section header.
 * ICN Manager send packet with this header to the source of the requested content.
 * This header contains the ip address of the client to whom content has to be sent
 **/
class DnsPlusAnswerSection: public Header {
public:
	DnsPlusAnswerSection();
	virtual ~DnsPlusAnswerSection();

	/**
	 * \brief Get the type ID.
	 * \return the object TypeId.
	 */
	static TypeId GetTypeId (void);
	virtual TypeId GetInstanceTypeId (void) const;
	virtual uint32_t GetSerializedSize (void) const;
	virtual void Serialize (Buffer::Iterator start) const;
	virtual uint32_t Deserialize (Buffer::Iterator start);
	virtual void Print (std::ostream &os) const;

	/**
	 * \brief Set the name of the content in answer section.
	 * \param name it is name of the content.
	 */
	void SetName(std::string name);

	/**
	 * \brief Set the length of the name of the content in answer section.
	 * \param nameLength length of the name of content.
	 */
	void SetNameLength(uint32_t nameLength);

	/**
	 * \brief Set the type of data in RD DATA field of answer section.
	 * \param type type of data in RD DATA field of answer section.
	 */
	void SetType(uint16_t Type);

	/**
	 * \brief Set the class of data in RD DATA field of answer section.
	 * \param class class of data in RD DATA field of answer section.
	 */
	void SetClass(uint16_t Class);

	/**
	 * \brief Set the Reserved flag, these are not currently used, reserved for future use.
	 * \param reservedFlag reserved flag bit
	 */
	void SetReservedFlag(uint16_t reservedFlag);

	/**
	 * \brief Set the ttl field of answer section.
	 * \param ttl ttl field of answer section.
	 */
	void SetTtl(uint16_t ttl);

	/**
	 * \brief Set length of RD DATA field of answer section.
	 * \param rdLength length of RD DATA field of answer section.
	 */
	void SetRdLength(uint16_t rdLength);

	/**
	 * \brief Set the RD DATA field of answer section.
	 * \param rdData value of data in RD DATA field of answer section.
	 */
	void SetRdData(Ipv4Address rdData);

	//getters

	/**
	 * \brief Get name of the content.
	 * \return name of the content.
	 */
	std::string  GetName();

	/**
	 * \brief Get the class of the answer section.
	 * \return class of answer section.
	 */
	uint16_t GetClass();

	/**
	 * \brief Get the type of answer section.
	 * \return type of answer section.
	 */
	uint16_t GetType();

	/**
	 * \brief Get the reserved flag.
	 * \return reservedFlag of answer section.
	 */
	uint8_t GetReservedFlag();

	/**
	 * \brief Get the ttl of answer section.
	 * \return ttl of answer section.
	 */
	uint16_t GetTtl();

	/**
	 * \brief Get the length of RD Data of answer section.
	 * \return length of RD Data of answer section.
	 */
	uint16_t GetRdLength();

	/**
	 * \brief Get the RD Data of answer section.
	 * \return RD Data of answer section.
	 */
	uint32_t GetRdData();

	/**
	 * \brief Get the Length of the name of the content.
	 * \return length of the name of the content.
	 */
	uint32_t GetNameLength();

private :

	uint8_t m_reservedFlag; //!< reserved flag bits
	std::string m_name; //!< name of the content
	uint16_t m_Class;//!< class of data in RD DATA field
	uint16_t m_Type;//!< type of data in RD DATA field
	uint32_t m_nameLength;//!< length of the name of the content
	uint16_t m_ttl;//!< time to live of answer section
	uint16_t m_rdLength;//!< length of RD Data
	uint32_t m_rdData;//!< RD Data of answer section
};


}
#endif /* DNSPLUS_ANSWER_SECTION_H_ */
