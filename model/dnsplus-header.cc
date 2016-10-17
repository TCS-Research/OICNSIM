/* -*- Mode:C++; c-file-style; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2016, CTO Networks Lab, Tata Consultancy Services (TCS)
 *
 * Author: Suvrat Agrawal <suv.agr@gmail.com>
 */

#include "ns3/type-name.h"
#include "ns3/type-id.h"
#include "ns3/log.h"
#include <stdint.h>
#include <string>
#include <cstring>
#include <cmath>
#include <iostream>

#include "dnsplus-header.h"
NS_LOG_COMPONENT_DEFINE ("DnsPlusHeader");

namespace ns3{

NS_OBJECT_ENSURE_REGISTERED (DnsPlusHeader);

TypeId DnsPlusHeader::GetTypeId (void)
{

	static TypeId tid = TypeId ("ns3::DnsPlusHeader")
    		.SetParent<Header> ()
			.AddConstructor<DnsPlusHeader> ();

	return tid;

}

TypeId
DnsPlusHeader::GetInstanceTypeId (void) const
{
	return GetTypeId ();
}

DnsPlusHeader::DnsPlusHeader()
:m_flag(0),
 m_Id(0),
 m_QR(0),
 m_OPCODE(0),
 m_AA(0),
 m_TC(0),
 m_RD(0),
 m_RA(0),
 m_Z(0),
 m_RCODE(0),
 m_QDCount(0),
 m_ANCount(0),
 m_NSCount(0),
 m_ARCount(0)
{
	// TODO Auto-generated constructor stub

}

DnsPlusHeader::~DnsPlusHeader() {
	// TODO Auto-generated destructor stub
}
void DnsPlusHeader::Serialize (Buffer::Iterator start) const
{
	NS_LOG_INFO (" Check ");
	Buffer::Iterator i = start;
	i.WriteHtonU16(m_Id);
	NS_LOG_DEBUG (" Flag is "<< m_flag);
	i.WriteHtonU16(m_flag);
	i.WriteHtonU16(m_QDCount);
	i.WriteHtonU16(m_ANCount);
	i.WriteHtonU16(m_NSCount);
	i.WriteHtonU16(m_ARCount);

	/*  Buffer::Iterator i = start;
  NS_LOG_DEBUG (" first2bytes in serialize is "<<first4bytes);
  i.WriteHtonU16(Id)
  i.WriteHtonU16(second2bytes);
  i.WriteHtonU16(QDCount);
  ;*/
}

uint32_t DnsPlusHeader::Deserialize (Buffer::Iterator start)
{
	Buffer::Iterator i = start;
	m_Id = i.ReadNtohU16();
	m_flag=i.ReadNtohU16();
	NS_LOG_DEBUG(" Flag value "<< m_flag);

	m_QR=((m_flag & 0x8000)>>15);
	m_OPCODE=(m_flag & 0x7800)>>11;
	m_AA =(m_flag & 0x0400)>>10;
	m_TC=(m_flag & 0x0200)>>9;
	m_RD=(m_flag & 0x0100)>>8;
	m_RA=(m_flag & 0x0080)>>7;
	m_Z=(m_flag & 0x0070)>>4;
	m_RCODE=(m_flag & 0x000F);

	NS_LOG_DEBUG(" m_QR "<< (uint16_t )m_QR);
	NS_LOG_DEBUG(" m_OPCODE "<< (uint16_t )m_OPCODE);
	NS_LOG_DEBUG(" m_AA "<< (uint16_t )m_AA);
	NS_LOG_DEBUG(" m_TC "<< (uint16_t )m_TC);
	NS_LOG_DEBUG(" m_RD "<< (uint16_t )m_RD);
	NS_LOG_DEBUG(" m_RA "<< (uint16_t )m_RA);
	NS_LOG_DEBUG(" m_Z "<< (uint16_t )m_Z);
	NS_LOG_DEBUG(" m_RCODE "<< (uint16_t )m_RCODE);

	m_QDCount=i.ReadNtohU16();
	m_ANCount=i.ReadNtohU16();
	m_NSCount=i.ReadNtohU16();
	m_ARCount=i.ReadNtohU16();
	return GetSerializedSize ();
}

uint32_t DnsPlusHeader::GetSerializedSize (void) const
{
	return 12; //byte
}

void DnsPlusHeader::Print (std::ostream &os) const
{

}

void DnsPlusHeader::SetId(uint16_t idBits)
{
	m_Id=idBits;
}
void DnsPlusHeader::SetQR(uint8_t qrbit	)
{
	if(qrbit == 1)
		m_QR = 1;
	else if(qrbit == 0)
		m_QR = 0;
	else
		NS_LOG_DEBUG("Wrong value of QR");

	m_flag=m_flag+qrbit*pow(2,15);
	NS_LOG_DEBUG(" Flag value "<< m_flag);
}

void DnsPlusHeader::SetOPCODE(uint8_t opcodeBits)
{
	m_OPCODE=opcodeBits;
	m_flag=m_flag+opcodeBits*pow(2,11);
}
void DnsPlusHeader::SetAA(uint8_t aaBit)
{
	m_AA=aaBit;
	m_flag=m_flag+aaBit*pow(2,10);
}
void DnsPlusHeader::SetTC(uint8_t tcBit)
{
	m_TC=tcBit;
	m_flag=m_flag+tcBit*pow(2,9);
}
void DnsPlusHeader::SetRD(uint8_t rdBit)
{
	m_RD=rdBit;
	m_flag=m_flag+rdBit*pow(2,8);
}
void DnsPlusHeader::SetRA(uint8_t raBit)
{
	m_RA=raBit;
	m_flag=m_flag+raBit*pow(2,7);
}

void DnsPlusHeader::SetZ(uint8_t zBit)
{
	m_Z=zBit;
	m_flag=m_flag+zBit*pow(2,4);
}
void DnsPlusHeader::SetRCODE(uint8_t rcodeBits)
{
	m_RCODE=rcodeBits;
	m_flag=m_flag+rcodeBits;
}

void DnsPlusHeader::SetQDCount(uint16_t qdcountBits)
{
	m_QDCount=qdcountBits;
}
void DnsPlusHeader::SetANCount(uint16_t ancountBits)
{
	m_ANCount=ancountBits;
}
void DnsPlusHeader::SetNSCount(uint16_t nscountBits)
{
	m_NSCount=nscountBits;
}
void DnsPlusHeader::SetARCount(uint16_t arcountBits)
{
	m_ARCount=arcountBits;
}

uint16_t DnsPlusHeader::GetId()
{
	return m_Id;
}

uint8_t DnsPlusHeader::GetQR()
{
	return m_QR;
}

uint8_t DnsPlusHeader::GetOPCODE()
{
	return m_OPCODE;
}

uint8_t DnsPlusHeader:: GetAA()
{
	return m_AA;
}

uint8_t DnsPlusHeader:: GetTC()
{
	return m_TC;
}

uint8_t DnsPlusHeader:: GetRD()
{
	return m_RD;
}

uint8_t DnsPlusHeader::GetRA()
{
	return m_RA;
}

uint8_t DnsPlusHeader::GetZ()
{
	return m_Z;
}

uint8_t DnsPlusHeader::GetRCODE()
{
	return m_RCODE;
}

uint16_t DnsPlusHeader::GetQDCount()
{
	return m_QDCount;
}

uint16_t DnsPlusHeader::GetANCount()
{
	return m_ANCount;
}

uint16_t DnsPlusHeader::GetNSCount()
{
	return m_NSCount;
}

uint16_t DnsPlusHeader::GetARCount()
{
	return m_ARCount;
}


}

