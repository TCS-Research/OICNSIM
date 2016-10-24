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

#ifndef CACHE_WITH_LRU_H
#define CACHE_WITH_LRU_H

#include "ns3/ptr.h"
#include "ns3/packet.h"
#include "ns3/node.h"
#include "named-content-cache-entry.h"
#include "named-content-cache.h"
#include "ns3/type-name.h"
#include "ns3/object.h"
#include "ns3/type-id.h"
#include "ns3/ipv4-header.h"

#include <stdint.h>
#include <string>
#include <boost/unordered_map.hpp>
#include <boost/bimap.hpp>
#include <boost/bimap/bimap.hpp>
#include <boost/bimap/set_of.hpp>
#include <boost/bimap/detail/bimap_core.hpp>
#include <boost/bimap/unordered_set_of.hpp>

namespace ns3{

/**
 * \brief This class implements the Least Recently Used caching policy.
 *
 **/
class CacheWithLRU : public NamedContentCache
{
public:

   CacheWithLRU();
   virtual ~CacheWithLRU();
   static TypeId GetTypeId (void);
   virtual void SetCacheSize(uint32_t size);
   virtual uint32_t GetCacheSize(void);
   virtual void SetFreshnessTime(uint64_t time);
   virtual uint64_t GetFreshnessTime(void);
   virtual NamedContentCacheEntry CreateEntry(std::string Name, Ptr<Packet> packet, Ipv4Header ipheader);
   virtual uint32_t CreateIndex(NamedContentCacheEntry entry);
   virtual void InsertToCache(std::string Name, NamedContentCacheEntry entry);
   virtual void InsertToPolicyIndex(uint32_t index, std::string Name);
   virtual bool IsFull(void);
   virtual bool IsUnique (std::string Name);
   virtual bool IsEvictable(uint32_t index);
   virtual std::string EvictEntry(void);
   virtual void EvictEntry(std::string Name);
   virtual Ptr<Packet> Find(std::string Name);
   virtual void UpdateIndex(std::string Name);
  
  private:

   uint32_t cache_size;//!< size of the Cache
   Cache cache;//!< main cache container which stores the name and corresponding content
   PolicyIndex policyindex;//!< policy indexed container, containing index and content name
};

}
#endif
