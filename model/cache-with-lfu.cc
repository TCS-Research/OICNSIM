/* -*- Mode:C++; c-file-style; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2016, CTO Networks Lab, Tata Consultancy Services (TCS)
 *
 * Author: Suvrat Agrawal <suv.agr@gmail.com>
 */
#include "ns3/type-id.h"
#include "ns3/type-name.h"
#include "ns3/ptr.h"
#include "ns3/log.h"
#include "ns3/packet.h"
#include "ns3/simulator.h"
#include "named-content-cache-entry.h"
#include "cache-with-lfu.h"
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

NS_LOG_COMPONENT_DEFINE ("CacheWithLFU");

namespace ns3{

NS_OBJECT_ENSURE_REGISTERED (CacheWithLFU);

TypeId CacheWithLFU::GetTypeId (void)
{
	NS_LOG_INFO (" Check ");
	static TypeId tid = TypeId ("ns3::CacheWithLFU")
    		.SetParent<NamedContentCache> ()
			.AddConstructor<CacheWithLFU> ();

	return tid;

}

CacheWithLFU::CacheWithLFU()
: cache_size (),
  policyindex (),
  cache(),
  freshness_time(0)
{
}


CacheWithLFU::~CacheWithLFU()
{

}

NamedContentCacheEntry CacheWithLFU::CreateEntry(std::string Name, Ptr<Packet> packet, Ipv4Header ipheader)
{
	NamedContentCacheEntry entry;
	uint8_t data[packet->GetSize()];
	packet->CopyData (data, packet->GetSize());
	std::string data1((char*)data);
	entry.SetData(data1);
	entry.SetTimestamp(Simulator::Now().GetMilliSeconds());
	entry.SetTTL(ipheader.GetTtl());
	entry.SetFrequency((uint32_t)1);
	return(entry);
}

uint32_t CacheWithLFU::CreateIndex(NamedContentCacheEntry entry)
{
	uint32_t index = entry.GetFrequency();
	return (index);
}

bool CacheWithLFU::IsEvictable(uint32_t index)
{
	left_iterator it = policyindex.left.begin();
	if (index >= it->first) return true;
	else
	{
		std::pair<left_iterator, left_iterator> iterpair = policyindex.left.equal_range (it->first);
		for (left_iterator i = iterpair.first; i != iterpair.second; i++)
		{
			Cache::iterator pos = cache.find (i->second);
			if ((Simulator::Now().GetMilliSeconds())-((pos->second).GetTimestamp()) >= freshness_time) return true;
		}
		return false;
	}
}

std::string CacheWithLFU::EvictEntry()
{
	std::string temp;
	left_iterator it = policyindex.left.begin();
	std::pair<left_iterator, left_iterator> iterpair = policyindex.left.equal_range (it->first);

	NS_LOG_DEBUG(" Policy Index Table Before Eviction");
	for(left_iterator iter = policyindex.left.begin(); iter != policyindex.left.end(); iter++ )
	{
		NS_LOG_DEBUG(" "<<iter->first << "-->" << iter->second << std::endl);
	}

	for (left_iterator i = iterpair.first; i != iterpair.second; i++)
	{
		Cache::iterator pos = cache.find (i->second);
		if ((Simulator::Now().GetMilliSeconds())-((pos->second).GetTimestamp()) >= freshness_time)
		{
			temp = pos->first;
			cache.erase (pos);
			policyindex.left.erase (i);
			break;
		}
	}
	NS_LOG_DEBUG(" Policy Index Table After Eviction");

	for(left_iterator iter = policyindex.left.begin(); iter != policyindex.left.end(); iter++ )
	{
		NS_LOG_DEBUG(" "<<iter->first << "-->" << iter->second << std::endl);
	}

	return(temp);
}

void CacheWithLFU::EvictEntry(std::string Name)
{
	cache.erase(Name);
	policyindex.right.erase(Name);
}

void CacheWithLFU::UpdateIndex(std::string Name)
{

	policyindex.right.erase(Name);
	Cache::iterator it = cache.find(Name);
	NamedContentCacheEntry temp = it->second;
	cache.erase(it);
	uint32_t f = temp.GetFrequency();
	f++;
	temp.SetFrequency(f);
	uint32_t index = CreateIndex(temp);
	InsertToCache(Name, temp);
	InsertToPolicyIndex(index, Name);
}

void CacheWithLFU::SetFreshnessTime(uint64_t time)
{
	freshness_time = time;
}

uint64_t CacheWithLFU::GetFreshnessTime()
{
	return (freshness_time);
}

void CacheWithLFU::SetCacheSize(uint32_t size)
{
	cache_size = size;
}

uint32_t CacheWithLFU::GetCacheSize()
{
	return(cache_size);
}

void CacheWithLFU::InsertToCache(std::string Name, NamedContentCacheEntry entry)
{
	std::pair<std::string, NamedContentCacheEntry> Entry (Name, entry);
	cache.insert(Entry);
}

void CacheWithLFU::InsertToPolicyIndex(uint32_t index, std::string Name)
{
	policyindex.insert({index, Name});
	for(left_iterator iter = policyindex.left.begin(); iter != policyindex.left.end(); iter++ )
	{
		NS_LOG_DEBUG(" "<<iter->first << "-->" << iter->second << std::endl);
	}
}

bool CacheWithLFU::IsFull()
{
	if (cache.size()>=cache_size) return true;
	else return false;
}

bool CacheWithLFU::IsUnique (std::string Name)
{
	for(Cache::iterator iter = cache.begin(); iter != cache.end(); iter++)
	{
		if (iter->first == Name) return false;
	}
	return true;
}

Ptr<Packet> CacheWithLFU::Find(std::string Name)
{
	Cache::iterator hit = cache.find(Name);
	NamedContentCacheEntry entryhit = hit->second;
	std::string data = entryhit.GetData();
	char buffer[data.size()+1];
	memset (buffer, 0, data.size()+1);
	strcpy(buffer,data.c_str());
	Ptr<Packet> packet = Create<Packet> ((uint8_t*)buffer, data.size());
	UpdateIndex (Name);
	return(packet);
}

}
