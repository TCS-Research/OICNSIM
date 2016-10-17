/* -*- Mode:C++; c-file-style; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2015, CTO Networks Lab, Tata Consultancy Services (TCS)
 *
 * Author: Anirudh Morabagal <anirudhmk.2010@gmail.com>
 */

#include "oicn-header.h"
#include "ns3/type-name.h"
#include "ns3/type-id.h"
#include "ns3/log.h"
#include <stdint.h>
#include <string>
#include <cstring>

NS_LOG_COMPONENT_DEFINE ("OICNHeader");

namespace ns3{

NS_OBJECT_ENSURE_REGISTERED (OICNHeader);

TypeId OICNHeader::GetTypeId (void)
{
	
  static TypeId tid = TypeId ("ns3::OICNHeader")
    .SetParent<Header> ()
    .AddConstructor<OICNHeader> ();

  return tid;

}



OICNHeader::OICNHeader ()
  : m_name(),
    header_length(0),
    first4bytes(0)
{

}

OICNHeader::~OICNHeader ()
{

}

TypeId
OICNHeader::GetInstanceTypeId (void) const
{
  return GetTypeId ();
}

void OICNHeader::SetName(std::string name)
{
  m_name = name;
  header_length = name.size() + 4;
}

 std::string OICNHeader::GetName()
{
  return(m_name);
}

void OICNHeader::SetFirst4Bytes(uint32_t HeaderLength, bool reply, bool cachable)
{
  if (reply)
	{
	  if (cachable) first4bytes = cachable_start + HeaderLength;
	  else first4bytes = non_cachable_start + HeaderLength;
	}
 else first4bytes = request_start + HeaderLength;
  NS_LOG_DEBUG (" first4bytes is "<< first4bytes<<" and header length is "<<HeaderLength);
}

uint32_t OICNHeader::GetFirst4Bytes()
{
	return(first4bytes);
}

void OICNHeader::SetRequest()
{
	SetFirst4Bytes(header_length, false, false);
}

void OICNHeader::SetCachable()
{
	SetFirst4Bytes(header_length, true, true);
}

void OICNHeader::SetNonCachable()
{
	SetFirst4Bytes(header_length, true, false);
}

void OICNHeader::Serialize (Buffer::Iterator start) const
{
  NS_LOG_INFO (" Check ");
  Buffer::Iterator i = start;
  NS_LOG_DEBUG (" first4bytes in serialize is "<<first4bytes);
  i.WriteHtonU32(first4bytes);
  char name[m_name.size()];
  memset(name,0,m_name.size()); // initializing the char array to zero
  strcpy(name,m_name.c_str()); // coverting chunk name from std::string format to the char format, convenient for serialization
  NS_LOG_DEBUG ("Length of Name is  "<<m_name.size()<<" Name is : "<<name);//
  //i.Write ((uint8_t*)name, header_length); //we used this in comm magazine result
  i.Write ((uint8_t*)name, m_name.size());
}

uint32_t OICNHeader::Deserialize (Buffer::Iterator start)
{
  Buffer::Iterator i = start;
  first4bytes = i.ReadNtohU32();
  // Deserializing headers of variable header length is not straight forward. 
  // We have to check whether header is request, cachable or non cachable and then get the header length.
  NS_LOG_DEBUG (" first4bytes obtained from deserialization is "<<first4bytes);
  if (first4bytes > cachable_start) header_length = first4bytes - cachable_start;
  else if(first4bytes > non_cachable_start) header_length = first4bytes - non_cachable_start;
  else if(first4bytes > request_start) header_length = first4bytes - request_start;
  NS_LOG_DEBUG (" headerlength obtained from deserialization is "<<header_length);
  //size_t name_size= static_cast<size_t>(header_length-4);
  // With the header length, deserializing the chunk name is easy.
  char name[header_length-3]; //= {0};//size_t(header_length-4)];//={0};//[header_length-4];
  memset(name,0,header_length-3);
  NS_LOG_DEBUG (" after memset is "<<name);
  i.Read((uint8_t*)name, (header_length-4));// header_length-4);
  m_name = name;
  NS_LOG_DEBUG (" name and m_name obtained from deserialization is, size "<<name<<" and "<<m_name <<" "<<m_name.size());
  return GetSerializedSize ();
}

 uint32_t OICNHeader::GetSerializedSize (void) const
{
  return(header_length);
}

 void OICNHeader::Print (std::ostream &os) const
 {

 }
}// end of namespace ns3
