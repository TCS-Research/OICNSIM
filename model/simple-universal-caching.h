/* -*- Mode:C++; c-file-style; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2016, CTO Networks Lab, Tata Consultancy Services (TCS)
 *
 * Author: Anirudh Morabagal <anirudhmk.2010@gmail.com>
 */
#ifndef SIMPLE_UNIVERSAL_CACHING_H
#define SIMPLE_UNIVERSAL_CACHING_H

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
 * \brief This class implements the Universal caching policy.
 *
 *	For more details of universal caching please refer :
 *	B. Panigrahi, S. Shailendra, H. K. Rath, and A. Simha, “Universal Caching Model and Markov-based Cache Analysis for Information Centric Networks,” Springer Photonic Network Communication Journal (PNET-2015)
 *
 **/
class SimpleUniversalCaching : public NamedContentCache
{
public:

   SimpleUniversalCaching();
   virtual ~SimpleUniversalCaching();
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
   Cache cache;//!< the main cache container which stores the name and corresponding content
   PolicyIndex policyindex;//!< policy indexed container, containing index and content name
   uint64_t freshness_time;//!< time after which the cache should be evicted
};
//}
}
#endif
