/* -*- Mode:C++; c-file-style; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2016, CTO Networks Lab, Tata Consultancy Services (TCS)
 *
 * Author: Suvrat Agrawal <suv.agr@gmail.com>
 */
#include "dnsplus-answer-section.h"
#include "ns3/type-name.h"
#include "ns3/type-id.h"
#include "ns3/log.h"
#include <stdint.h>
#include <string>
#include <cstring>
#include <cmath>


NS_LOG_COMPONENT_DEFINE ("DnsPlusAnswerSection");


namespace ns3{

NS_OBJECT_ENSURE_REGISTERED (DnsPlusAnswerSection);

TypeId DnsPlusAnswerSection::GetTypeId (void)
{

	static TypeId tid = TypeId ("ns3::DnsPlusAnswerSection")
    		.SetParent<Header> ()
			.AddConstructor<DnsPlusAnswerSection> ();

	return tid;

}

TypeId
DnsPlusAnswerSection::GetInstanceTypeId (void) const
{
	return GetTypeId ();
}

void DnsPlusAnswerSection::Print (std::ostream &os) const
{

}


DnsPlusAnswerSection::DnsPlusAnswerSection()
:m_reservedFlag(2),
 m_name(),
 m_nameLength(0),
 m_Class(0),
 m_Type(0),
 m_ttl(255),
 m_rdLength(32)
{
}

DnsPlusAnswerSection::~DnsPlusAnswerSection() {
}

void DnsPlusAnswerSection::Serialize (Buffer::Iterator start) const
{
	Buffer::Iterator i = start;
	uint32_t temp;
	temp=(uint32_t) m_reservedFlag;
	temp = temp << 28;
	temp =temp | m_nameLength;
	i.WriteHtonU32(temp);

	char name[m_name.size()];
	memset(name,0,m_name.size());
	strcpy(name,m_name.c_str());
	i.Write ((uint8_t*)name, m_name.size());
	i.WriteHtonU16(m_Type);
	i.WriteHtonU16(m_Class);
	i.WriteHtonU16(m_ttl);
	i.WriteHtonU16(m_rdLength);
	i.WriteHtonU32(m_rdData);
}

uint32_t DnsPlusAnswerSection::Deserialize (Buffer::Iterator start)
{

	Buffer::Iterator i = start;
	uint32_t temp = i.ReadNtohU32();
	uint32_t temp2;

	m_reservedFlag  = (temp & 0xf0000000)>>28;
	m_nameLength = temp & 0x0fffffff;
	char name[m_nameLength+1];
	memset(name,0,m_nameLength);
	i.Read((uint8_t*)name, m_nameLength);
	name[m_nameLength]=0;
	m_name = name;

	m_Type=i.ReadNtohU16();
	m_Class=i.ReadNtohU16();
	m_ttl=i.ReadNtohU16();
	m_rdLength=i.ReadNtohU16();
	m_rdData =i.ReadNtohU32 ();

	return GetSerializedSize ();
}

uint32_t DnsPlusAnswerSection::GetSerializedSize (void) const
{
	return m_nameLength+16; //byte
}


void DnsPlusAnswerSection::SetName(std::string name)
{
	m_name = name;
	m_nameLength=name.size();
}
void DnsPlusAnswerSection::SetType(uint16_t type)
{
	m_Type=type;
}
void DnsPlusAnswerSection::SetClass(uint16_t Class)
{
	m_Class=Class;
}
void DnsPlusAnswerSection::SetReservedFlag(uint16_t reservedFlag)
{
	m_reservedFlag= reservedFlag;
}

void DnsPlusAnswerSection::SetTtl(uint16_t ttl)
{
	m_ttl = ttl;
}
void DnsPlusAnswerSection::SetRdLength(uint16_t rdLength)
{
	m_rdLength = rdLength;
}
void DnsPlusAnswerSection::SetRdData(Ipv4Address rdData)
{
	m_rdData = rdData.Get();
}
std::string DnsPlusAnswerSection::GetName()
{
	return m_name;
}
uint16_t DnsPlusAnswerSection::GetClass()
{
	return m_Class;
}

uint16_t DnsPlusAnswerSection:: GetType()
{
	return m_Type;
}

uint8_t DnsPlusAnswerSection:: GetReservedFlag()
{
	return m_reservedFlag;
}

uint16_t DnsPlusAnswerSection:: GetTtl()
{
	return m_ttl;
}
uint16_t DnsPlusAnswerSection::GetRdLength()
{
	return m_rdLength;
}
uint32_t DnsPlusAnswerSection::GetRdData()
{
	return m_rdData;
}

void DnsPlusAnswerSection::SetNameLength(uint32_t nameLength)
{
	m_nameLength =nameLength;

}

uint32_t DnsPlusAnswerSection::GetNameLength()
{
	return m_nameLength;
}

}

