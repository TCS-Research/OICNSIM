/* -*- Mode:C++; c-file-style; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2016, CTO Networks Lab, Tata Consultancy Services (TCS)
 *
 * Author: Suvrat Agrawal <suv.agr@gmail.com>
 */
#ifndef CACHE_WITH_LFU_H
#define CACHE_WITH_LFU_H

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
 * \brief This class implements the Least Frequently Used caching policy.
 *
 **/
class CacheWithLFU : public NamedContentCache
{
public:

	CacheWithLFU();
   virtual ~CacheWithLFU();

   /**
	* \brief Get the type ID.
	* \return the object TypeId
	*/
   static TypeId GetTypeId (void);

   /**
    * \brief Set the cache size of the Cache
    * \param size the size to which the cache has to be set
    */
   virtual void SetCacheSize(uint32_t size);

   /**
    * \brief Get the cache size of the Cache
    * \return the cache size of the Cache
    */
   virtual uint32_t GetCacheSize(void);

   /**
    * \brief Set the freshness time for the contents of Cache
    * \param time the freshness time in milliseconds
    */
   virtual void SetFreshnessTime(uint64_t time);

   /**
    * \brief Get the freshness time set for the contents of Cache
    * \return the freshness time in milliseconds
    */
   virtual uint64_t GetFreshnessTime(void);

   /**
    * \brief Create the Cache entry, which is an object of NamedContentCacheEntry
    *        class. The subclasses will define this function to create an entry
    *        that has the requisite parameters of the content, for the purpose of
    *        indexing content according to caching policy the subclass represents.
    * \param Name name of the content
    * \param packet packet received by ICN Router, with its IP, Transport and
    *        OICN headers removed
    * \param ipheader ip header of the packet, from which the required parameters
               of the content (present in packet) can retrieved.
    * \return the newly created cache entry
    */
   virtual NamedContentCacheEntry CreateEntry(std::string Name, Ptr<Packet> packet, Ipv4Header ipheader);

   /**
    * \brief Create the index of the corresponding entry, by using the parameters
    *        of the content present in the entry class. The subclasses will define
    *        this function to create an index which, when put in order, will
    *        arrange the contents according to the caching policy the subclass
    *        represents. If some subclasses need decimal indexes, they have to
    *        multiply the obtained index with a factor to ensure unique integer
    *        indexes are obtained.
    * \param entry the cache entry corresponding to the content
    * \return the newly created integer index for the content
    */
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
   uint64_t freshness_time;//!< time after which the cache should be evicted
};
}
#endif
