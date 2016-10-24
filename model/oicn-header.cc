/* -*- Mode:C++; c-file-style; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2016, TCS Research & Innovation
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * Authors: Anirudh Morabagal <anirudhmk.2010@gmail.com>
 *          Suvrat Agrawal <suvrat.a@tcs.com>
 *
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

NS_OBJECT_ENSURE_REGISTERED (OicnHeader);

TypeId OicnHeader::GetTypeId (void)
{
	static TypeId tid = TypeId ("ns3::OICNHeader")
    		.SetParent<Header> ()
			.AddConstructor<OicnHeader> ();

	return tid;
}

OicnHeader::OicnHeader ()
: m_name(),
  header_length(0),
  first4bytes(0)
{
}

OicnHeader::~OicnHeader ()
{
}

TypeId
OicnHeader::GetInstanceTypeId (void) const
{
	return GetTypeId ();
}

void OicnHeader::SetName(std::string name)
{
	m_name = name;
	header_length = name.size() + 4; //4 bytes
}

std::string OicnHeader::GetName()
{
	return(m_name);
}

void OicnHeader::SetFirst4Bytes(uint32_t HeaderLength, bool reply, bool cachable)
{
	if (reply)
	{
		if (cachable) first4bytes = cachable_start + HeaderLength;
		else first4bytes = non_cachable_start + HeaderLength;
	}
	else first4bytes = request_start + HeaderLength;
}

uint32_t OicnHeader::GetFirst4Bytes()
{
	return(first4bytes);
}

void OicnHeader::SetRequest()
{
	SetFirst4Bytes(header_length, false, false);
}

void OicnHeader::SetCachable()
{
	SetFirst4Bytes(header_length, true, true);
}

void OicnHeader::SetNonCachable()
{
	SetFirst4Bytes(header_length, true, false);
}

void OicnHeader::Serialize (Buffer::Iterator start) const
{
	Buffer::Iterator i = start;
	i.WriteHtonU32(first4bytes);
	char name[m_name.size()];
	memset(name,0,m_name.size());
	strcpy(name,m_name.c_str());
	i.Write ((uint8_t*)name, m_name.size());
}

uint32_t OicnHeader::Deserialize (Buffer::Iterator start)
{
	Buffer::Iterator i = start;
	first4bytes = i.ReadNtohU32();
	if (first4bytes > cachable_start) header_length = first4bytes - cachable_start;
	else if(first4bytes > non_cachable_start) header_length = first4bytes - non_cachable_start;
	else if(first4bytes > request_start) header_length = first4bytes - request_start;
	char name[header_length-3];
	memset(name,0,header_length-3);
	i.Read((uint8_t*)name, (header_length-4));
	m_name = name;
	return GetSerializedSize ();
}

uint32_t OicnHeader::GetSerializedSize (void) const
{
	return(header_length);
}

void OicnHeader::Print (std::ostream &os) const
{
}
}
