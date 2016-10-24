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
#include "cache-with-fifo.h"
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

NS_LOG_COMPONENT_DEFINE ("CacheWithFifo");

namespace ns3{

NS_OBJECT_ENSURE_REGISTERED (CacheWithFifo);

TypeId CacheWithFifo::GetTypeId (void)
{
	static TypeId tid = TypeId ("ns3::CacheWithFifo")
    		.SetParent<NamedContentCache> ()
			.AddConstructor<CacheWithFifo> ();

	return tid;

}

CacheWithFifo::CacheWithFifo()
: cache_size (),
  policyindex (),
  cache()
{
}


CacheWithFifo::~CacheWithFifo()
{
}

NamedContentCacheEntry CacheWithFifo::CreateEntry(std::string Name, Ptr<Packet> packet, Ipv4Header ipheader)
{
	NamedContentCacheEntry entry;
	uint8_t data[packet->GetSize()];
	packet->CopyData (data, packet->GetSize());
	std::string data1((char*)data);
	entry.SetData(data1);
	entry.SetTimestamp(Simulator::Now().GetMilliSeconds());
	return(entry);
}

uint32_t CacheWithFifo::CreateIndex(NamedContentCacheEntry entry)
{
	uint32_t index = (float)entry.GetTimestamp();
	return (index);
}

bool CacheWithFifo::IsEvictable(uint32_t index)
{
	return true;
}

std::string CacheWithFifo::EvictEntry()
{
	left_iterator it = policyindex.left.begin();
	//NS_LOG_DEBUG (" Entry to be evicted "<<it->second<<" with index "<<it->first);
	std::string name = it->second;
	policyindex.left.erase(it);
	cache.erase(name);
	return(name);
}

void CacheWithFifo::EvictEntry(std::string Name)
{
	cache.erase(Name);
	policyindex.right.erase(Name);
}

void CacheWithFifo::UpdateIndex(std::string Name)
{
}

void CacheWithFifo::SetFreshnessTime(uint64_t time)
{
}

uint64_t CacheWithFifo::GetFreshnessTime()
{
	return ((uint64_t)0);
}

void CacheWithFifo::SetCacheSize(uint32_t size)
{
	cache_size = size;
}

uint32_t CacheWithFifo::GetCacheSize()
{
	return(cache_size);
}

void CacheWithFifo::InsertToCache(std::string Name, NamedContentCacheEntry entry)
{
	std::pair<std::string, NamedContentCacheEntry> Entry (Name, entry);
	cache.insert(Entry);
}

void CacheWithFifo::InsertToPolicyIndex(uint32_t index, std::string Name)
{
	policyindex.insert({index, Name});
}

bool CacheWithFifo::IsFull()
{
	if (cache.size()>=cache_size) return true;
	else return false;
}

bool CacheWithFifo::IsUnique (std::string Name)
{
	for(Cache::iterator iter = cache.begin(); iter != cache.end(); iter++)
	{
		if (iter->first == Name) return false;
	}
	return true;
}


Ptr<Packet> CacheWithFifo::Find(std::string Name)
{

	Cache::iterator hit = cache.find(Name);
	NamedContentCacheEntry entryhit = hit->second;
	std::string data = entryhit.GetData();
	char buffer[data.size()+1];
	memset (buffer, 0, data.size()+1);
	strcpy(buffer,data.c_str());
	Ptr<Packet> packet = Create<Packet> ((uint8_t*)buffer, data.size());
	return(packet);
}

}
