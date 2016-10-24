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

#include "ns3/type-id.h"
#include "ns3/type-name.h"
#include "named-content-cache-entry.h"


namespace ns3{


NamedContentCacheEntry::NamedContentCacheEntry()
                      : m_data(),
                        m_ttl(0), 
                        m_timestamp(0), 
                        m_frequency(1), 
                        m_sourceip(0), 
                        m_packetsize(0),
                        m_id(0)
{
}

NamedContentCacheEntry::~NamedContentCacheEntry()
{
}

 void NamedContentCacheEntry::SetData (std::string data)
{
  m_data = data;
}

 std::string NamedContentCacheEntry::GetData ()
{
  return(m_data);
}
 
 void NamedContentCacheEntry::SetTTL(uint8_t ttl)
{
  m_ttl = ttl;
}

 uint8_t NamedContentCacheEntry::GetTTL()
{
  return(m_ttl);
}
 
 void NamedContentCacheEntry::SetTimestamp(uint64_t timestamp)
{
  m_timestamp = timestamp;
}

 uint64_t NamedContentCacheEntry::GetTimestamp()
{
  return(m_timestamp);
}

 void NamedContentCacheEntry::SetFrequency(uint32_t frequency)
{
  m_frequency = frequency;
}

 uint32_t NamedContentCacheEntry::GetFrequency()
{
  return(m_frequency);
}

 void NamedContentCacheEntry::SetSourceIP(uint32_t sourceip)
{
  m_sourceip = sourceip;
}

 uint32_t NamedContentCacheEntry::GetSourceIP()
{
  return(m_sourceip);
}

 void NamedContentCacheEntry::SetPacketSize(uint32_t size)
{
  m_packetsize = size;
}

 uint32_t NamedContentCacheEntry::GetPacketSize()
{
  return(m_packetsize);
}

 void NamedContentCacheEntry::SetPacketID(uint32_t uid)
{
  m_id = uid;
}

 uint32_t NamedContentCacheEntry::GetPacketID()
{
  return(m_id);
}

}
