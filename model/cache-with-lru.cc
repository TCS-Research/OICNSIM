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
#include "ns3/ptr.h"
#include "ns3/log.h"
#include "ns3/packet.h"
#include "ns3/simulator.h"
#include "named-content-cache-entry.h"
#include "cache-with-lru.h"
#include <stdint.h>
#include <string>
#include <utility>
#include <cstring>
#include <iostream>
#include <boost/unordered_map.hpp>
#include <boost/bimap.hpp>
#include <boost/bimap/bimap.hpp>
#include <boost/bimap/set_of.hpp>
#include <boost/bimap/detail/bimap_core.hpp>
#include <boost/bimap/unordered_set_of.hpp>

NS_LOG_COMPONENT_DEFINE ("CacheWithLRU");

namespace ns3{

NS_OBJECT_ENSURE_REGISTERED (CacheWithLRU);

TypeId CacheWithLRU::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::CacheWithLRU")
    .SetParent<NamedContentCache> ()
    .AddConstructor<CacheWithLRU> ();

  return tid;

}

CacheWithLRU::CacheWithLRU()
 : cache_size (),
   policyindex (),
   cache()
{
}

CacheWithLRU::~CacheWithLRU()
{
}

NamedContentCacheEntry CacheWithLRU::CreateEntry(std::string Name, Ptr<Packet> packet, Ipv4Header ipheader)
{
	 NamedContentCacheEntry entry;
     uint8_t data[packet->GetSize()];
     packet->CopyData (data, packet->GetSize());
     std::string data1((char*)data);
     entry.SetData(data1);
     entry.SetTimestamp(Simulator::Now().GetMilliSeconds());
     return(entry);
}
  
uint32_t CacheWithLRU::CreateIndex(NamedContentCacheEntry entry)
{
     uint32_t index = (float)entry.GetTimestamp();
     return (index);
}
  
bool CacheWithLRU::IsEvictable(uint32_t index)
{
	return true;
}

std::string CacheWithLRU::EvictEntry()
{
	left_iterator it = policyindex.left.begin();
	 NS_LOG_DEBUG (" All Entries in cache ");
	    for(left_iterator iter = policyindex.left.begin(); iter != policyindex.left.end(); iter++ )
	    {
	    	 NS_LOG_DEBUG ("   "<<iter->first << "-->" << iter->second );
	    }
    std::string name = it->second;
    policyindex.left.erase(it);
    cache.erase(name);
    return(name);
}

void CacheWithLRU::EvictEntry(std::string Name)
{
	cache.erase(Name);
	policyindex.right.erase(Name);
}
  
void CacheWithLRU::UpdateIndex(std::string Name)
{
     policyindex.right.erase(Name);
     Cache::iterator it = cache.find(Name);
     NamedContentCacheEntry temp = it->second;
     cache.erase(it);
     temp.SetTimestamp(Simulator::Now().GetMilliSeconds());
     InsertToCache(Name, temp);
     InsertToPolicyIndex(CreateIndex(temp), Name);
}

void CacheWithLRU::SetFreshnessTime(uint64_t time)
{
}

uint64_t CacheWithLRU::GetFreshnessTime()
{
	return ((uint64_t)0);
}

void CacheWithLRU::SetCacheSize(uint32_t size)
{
	cache_size = size;
}

uint32_t CacheWithLRU::GetCacheSize()
{
	return(cache_size);
}

 void CacheWithLRU::InsertToCache(std::string Name, NamedContentCacheEntry entry)
{
  std::pair<std::string, NamedContentCacheEntry> Entry (Name, entry);
  cache.insert(Entry);
}

 void CacheWithLRU::InsertToPolicyIndex(uint32_t index, std::string Name)
{
	 policyindex.insert({index, Name});
}

bool CacheWithLRU::IsFull()
{
	 if (cache.size()<cache_size) return false;
	 else return true;
}

bool CacheWithLRU::IsUnique (std::string Name)
{
	for(Cache::iterator iter = cache.begin(); iter != cache.end(); iter++)
	{
		if (iter->first == Name) return false;
	}
	return true;
}

Ptr<Packet> CacheWithLRU::Find(std::string Name)
{
  Cache::iterator hit = cache.find(Name);
  NamedContentCacheEntry entryhit = hit->second;
  std::string data = entryhit.GetData();
  char buffer[data.size()+1];
  memset (buffer, 0, data.size()+1);
  strcpy(buffer,data.c_str());
  Ptr<Packet> packet = Create<Packet> ((uint8_t*)buffer, data.size());
  UpdateIndex(Name);
  return(packet);
}
}
