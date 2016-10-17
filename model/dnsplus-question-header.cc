/* -*- Mode:C++; c-file-style; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2016, CTO Networks Lab, Tata Consultancy Services (TCS)
 *
 * Author: Suvrat Agrawal <suv.agr@gmail.com>
 */

#include "dnsplus-question-header.h"
#include "ns3/type-name.h"
#include "ns3/type-id.h"
#include "ns3/log.h"
#include <stdint.h>
#include <string>
#include <cstring>
#include <cmath>

NS_LOG_COMPONENT_DEFINE ("DnsPlusQuestionHeader");


namespace ns3{

NS_OBJECT_ENSURE_REGISTERED (DnsPlusQuestionHeader);

TypeId DnsPlusQuestionHeader::GetTypeId (void)
{

  static TypeId tid = TypeId ("ns3::DnsPlusQuestionHeader")
    .SetParent<Header> ()
    .AddConstructor<DnsPlusQuestionHeader> ();

  return tid;

}

TypeId
DnsPlusQuestionHeader::GetInstanceTypeId (void) const
{
  return GetTypeId ();
}

void DnsPlusQuestionHeader::Print (std::ostream &os) const
 {

 }


DnsPlusQuestionHeader::DnsPlusQuestionHeader()
:m_reservedFlag(2),
m_name(),
m_nameLength(0),
m_qClass(0),
m_qType(0)
{
}

DnsPlusQuestionHeader::~DnsPlusQuestionHeader() {
}

void DnsPlusQuestionHeader::Serialize (Buffer::Iterator start) const
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
   i.WriteHtonU16(m_qType);
   i.WriteHtonU16(m_qClass);
}

uint32_t DnsPlusQuestionHeader::Deserialize (Buffer::Iterator start)
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
  m_qType=i.ReadNtohU16();
  m_qClass=i.ReadNtohU16();

  return GetSerializedSize ();
}

 uint32_t DnsPlusQuestionHeader::GetSerializedSize (void) const
{
  return m_nameLength+8; //byte
}


void DnsPlusQuestionHeader::SetName(std::string name)
{
	m_name = name;
	m_nameLength=name.size();
}
void DnsPlusQuestionHeader::SetQtype(uint16_t qType)
{
	m_qType=qType;
}
void DnsPlusQuestionHeader::SetQclass(uint16_t qClass)
{
	m_qClass=qClass;
}
void DnsPlusQuestionHeader::SetReservedFlag(uint16_t reservedFlag)
{
	m_reservedFlag= reservedFlag;
}
void DnsPlusQuestionHeader::SetNameLength( uint32_t nameLength)
{
	m_nameLength = nameLength;
}
std::string DnsPlusQuestionHeader::GetName()
{
	return m_name;
}
uint16_t DnsPlusQuestionHeader::GetQclass()
{
	return m_qClass;
}

uint16_t DnsPlusQuestionHeader:: GetQtype()
{
	return m_qType;
}

uint8_t DnsPlusQuestionHeader:: GetReservedFlag()
{
	return m_reservedFlag;
}

uint32_t DnsPlusQuestionHeader::GetNameLength()
{
	return m_nameLength;
}

}

