/* -*- Mode:C++; c-file-style; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2016, CTO Networks Lab, Tata Consultancy Services (TCS)
 *
 * Author: Suvrat Agrawal <suv.agr@gmail.com>
 */

#ifndef DNSPLUSHEADER_H_
#define DNSPLUSHEADER_H_

#include <stdint.h>
#include <string>
#include "ns3/header.h"
#include "ns3/type-name.h"

namespace ns3{
/**
 * \brief It is similar to DNS header.
 * It used for name resolution of the content.
 **/

class DnsPlusHeader: public ns3::Header
{
public:
	DnsPlusHeader();
	~DnsPlusHeader();

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
	 * \brief Set QR bit.
	 * \param qrBit QR bit, 0 specify the Query and 1 a response.
	 */
	void SetQR(uint8_t qrBit);

	/**
	 * \brief Set Id.
	 * \param idBits Id of DNS request
	 */
	void SetId(uint16_t idBits);

	/**
	 * \brief Set opcode
	 * \param opcode OPCODE field
	 */
	void SetOPCODE(uint8_t opcode);
	//Opcode=2^14+2^12+2^11 for OICN, we have taken as 11 for the OICN

	/**
	 * \brief Set AA Authoritative Answer bit.
	 * \param aaBit Authoritative Answer bit.
	 */
	void SetAA(uint8_t aaBit);

	/**
	 * \brief Set Truncation bit
	 * \param tcBit Truncation bit
	 */
	void SetTC(uint8_t tcBit);

	/**
	 * \brief Set Recursion Desired bit
	 * \param rdBit Recursion Desired bit
	 */
	void SetRD(uint8_t rdBit);

	/**
	 * \brief Set Recursion Avaliable bit
	 * \param raBit Recursion Avaliable
	 */
	void SetRA(uint8_t raBit);
	/**
	 * \brief Set Z bit
	 * \param zBit Z bit
	 */
	void SetZ(uint8_t zBit);
	/**
	 * \brief Set Response Code
	 * \param rcodeBits Response Code
	 */
	void SetRCODE(uint8_t rcodeBits);
	/**
	 * \brief Set QDCount Bit
	 * \param qdcountBits QDCount Bit
	 */
	void SetQDCount(uint16_t qdcountBits);
	/**
	 * \brief Set ANCount Bit
	 * \param ancountBits ANCount Bit
	 */
	void SetANCount(uint16_t ancountBits);
	/**
	 * \brief Set NSCount bit
	 * \param nscountBits NSCount bit
	 */
	void SetNSCount(uint16_t nscountBits);

	/**
	 * \brief Set ARCount Bit
	 * \param arcountBits ARCount Bit
	 */
	void SetARCount(uint16_t arcountBits);


	//getter

	/**
	 * \brief Get Id of DNS request
	 * \return Id of DNS request
	 */
	uint16_t GetId();
	/**
	 * \brief Get QR bit
	 * \return QR bit
	 */
	uint8_t GetQR();

	/**
	 * \brief Get OPCODE bits
	 * \return OPCODE bits
	 */
	uint8_t GetOPCODE();

	/**
	 * \brief Get AA bit
	 * \return AA bit
	 */
	uint8_t GetAA();

	/**
	 * \brief Get TC bit
	 * \return TC bit
	 */
	uint8_t GetTC();

	/**
	 * \brief Get RD bit
	  * \return RD bit
	 */
	uint8_t GetRD();

	/**
	 * \brief Get RA bit
	 *  \return RA bit
	 */
	uint8_t GetRA();

	/**
	 * \brief Get Z bit
	  * \return Z bit
	 */
	uint8_t GetZ();

	/**
	 * \brief Get RCODE
	 * \return RCODE
	 */
	uint8_t GetRCODE();

	/**
	 * \brief Get QDcount
	 * \return QDCount
	 */
	uint16_t GetQDCount();

	/**
	 * \brief Get ANCount
	  * \return ANCount
	  */
	uint16_t GetANCount();

	/**
	 *\brief Get NScount
	 * \return NSCount bit
	 */
	uint16_t GetNSCount();

	/**
	 * \brief Get ARCount
	 * \return ARCount bit
	 */
	uint16_t GetARCount();


	private:
	uint32_t m_flag;//!< flag
	uint16_t m_Id;//!< Id bits of header
	uint8_t m_QR;//!< QR bit of header
	uint8_t m_OPCODE;//!< OPCODE bits of header
	uint8_t m_AA;//!< AA bit of header
	uint8_t m_TC;//!< TC bit of header
	uint8_t m_RD;//!< RD bit of header
	uint8_t m_RA;//!< RA bit of header
	uint8_t m_Z;//!< Z bits of header
	uint8_t m_RCODE;//!< RCODE bits of header
	uint16_t m_QDCount;//!< QDCount bits of header
	uint16_t m_ANCount;//!< ANCount bits of header
	uint16_t m_NSCount;//!< NSCount bits of header
	uint16_t m_ARCount;//!< ARCount bits of header

};
}
#endif /* DNSHEADER_H_ */
