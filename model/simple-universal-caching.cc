/* -*- Mode:C++; c-file-style; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2016, CTO Networks Lab, Tata Consultancy Services (TCS)
 *
 * Author: Anirudh Morabagal <anirudhmk.2010@gmail.com>
 */
#include "ns3/type-id.h"
#include "ns3/type-name.h"
#include "ns3/ptr.h"
#include "ns3/log.h"
#include "ns3/packet.h"
#include "ns3/simulator.h"
#include "named-content-cache-entry.h"
#include "simple-universal-caching.h"
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

NS_LOG_COMPONENT_DEFINE ("SimpleUniversalCaching");

namespace ns3{

NS_OBJECT_ENSURE_REGISTERED (SimpleUniversalCaching);

TypeId SimpleUniversalCaching::GetTypeId (void)
{
	static TypeId tid = TypeId ("ns3::SimpleUniversalCaching")
    				.SetParent<NamedContentCache> ()
					.AddConstructor<SimpleUniversalCaching> ();

	return tid;
}

SimpleUniversalCaching::SimpleUniversalCaching()
: cache_size (),
  policyindex (),
  cache(),
  freshness_time(0)
{
}


SimpleUniversalCaching::~SimpleUniversalCaching()
{
}

NamedContentCacheEntry SimpleUniversalCaching::CreateEntry(std::string Name, Ptr<Packet> packet, Ipv4Header ipheader)
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

uint32_t SimpleUniversalCaching::CreateIndex(NamedContentCacheEntry entry)
{
	uint32_t freq_max = entry.GetFrequency();
	if (cache.size() == 0)
	{
		uint32_t index = 10000 + (((64-entry.GetTTL())*20000)/64);
		return (index);
	}
	else
	{
		for(Cache::iterator iter = cache.begin(); iter != cache.end(); iter++ )
		{
			if (freq_max <= (iter->second).GetFrequency()) freq_max = (iter->second).GetFrequency();
		}
		uint32_t index = ((entry.GetFrequency()*10000)/freq_max) + (((64-entry.GetTTL())*20000)/64);
		return (index);
	}
}

bool SimpleUniversalCaching::IsEvictable(uint32_t index)
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

std::string SimpleUniversalCaching::EvictEntry()
{

	std::string temp;
	left_iterator it = policyindex.left.begin();
	std::pair<left_iterator, left_iterator> iterpair = policyindex.left.equal_range (it->first);
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
	return(temp);
}

void SimpleUniversalCaching::EvictEntry(std::string Name)
{
	cache.erase(Name);
	policyindex.right.erase(Name);
}

void SimpleUniversalCaching::UpdateIndex(std::string Name)
{
	policyindex.right.erase(Name);
	Cache::iterator it = cache.find(Name);
	NamedContentCacheEntry temp = it->second;
	cache.erase(it);
	temp.SetTimestamp(Simulator::Now().GetMilliSeconds());
	uint32_t f = temp.GetFrequency();
	f++;
	temp.SetFrequency(f);
	uint32_t index = CreateIndex(temp);
	InsertToCache(Name, temp);
	InsertToPolicyIndex(index, Name);
}

void SimpleUniversalCaching::SetFreshnessTime(uint64_t time)
{
	freshness_time = time;
}

uint64_t SimpleUniversalCaching::GetFreshnessTime()
{
	return (freshness_time);
}

void SimpleUniversalCaching::SetCacheSize(uint32_t size)
{
	cache_size = size;
}

uint32_t SimpleUniversalCaching::GetCacheSize()
{
	return(cache_size);
}

void SimpleUniversalCaching::InsertToCache(std::string Name, NamedContentCacheEntry entry)
{
	std::pair<std::string, NamedContentCacheEntry> Entry (Name, entry);
	cache.insert(Entry);
}

void SimpleUniversalCaching::InsertToPolicyIndex(uint32_t index, std::string Name)
{
	policyindex.insert({index, Name});
}

bool SimpleUniversalCaching::IsFull()
{
	if (cache.size()>=cache_size) return true;
	else return false;
}

bool SimpleUniversalCaching::IsUnique (std::string Name)
{
	for(Cache::iterator iter = cache.begin(); iter != cache.end(); iter++)
	{
		if (iter->first == Name) return false;
	}
	return true;
}

Ptr<Packet> SimpleUniversalCaching::Find(std::string Name)
{
	Cache::iterator hit = cache.find(Name);
	NamedContentCacheEntry entryhit = hit->second;
	std::string data = entryhit.GetData();
	char buffer[data.size()+1];
	memset (buffer, 0, data.size()+1);
	strcpy(buffer,data.c_str()); //This is was missing added later
	Ptr<Packet> packet = Create<Packet> ((uint8_t*)buffer, data.size());
	UpdateIndex (Name);
	return(packet);
}
}
