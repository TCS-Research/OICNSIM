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

#ifndef NAMED_CONTENT_CACHE_H
#define NAMED_CONTENT_CACHE_H

#include "ns3/ptr.h"
#include "ns3/packet.h"
#include "named-content-cache-entry.h"
#include "ns3/type-name.h"
#include "ns3/object.h"
#include "ns3/type-id.h"
#include "ns3/ipv4-header.h"

#include <stdint.h>
#include <string>
#include <boost/unordered_map.hpp>
#include <boost/bimap.hpp>
#include <boost/bimap/bimap.hpp>
#include <boost/bimap/multiset_of.hpp>
#include <boost/bimap/detail/bimap_core.hpp>
#include <boost/bimap/unordered_set_of.hpp>

namespace ns3{

/**
 * \brief The base class for caching data at the ICN Router
 *
 * This class is a base class for all different types of ICN Router
 * cache, i.e. a FIFO based cache, an LRU based cache, a customized
 * cache etc. This class provides the two essential containers required
 * for a typical cache, a container to store the actual data, called
 * Cache, and a container to store the index of the corresponding data,
 * called PolicyIndex.
 *
 * A Cache entry is an object of NamedContentCacheEntry class, which is
 * basically a struct containing the data along with various parameters
 * corresponding to data, like timestamp, source IP etc. More about
 * NamedContentCacheEntry class can be found in its header file.
 *
 * Depending on caching policy used by a particular subclass, the index
 * of a content is calculated using the required parameters present in
 * the cache entry. The NamedContentCache class basically contains the
 * declarations for all required virtual functions needed for handling a
 * cache, i.e. insertion, eviction etc.
 *
 * This class is instantiated and aggregated to all ICN Routers in
 * the network, and is not required in any other components of OICN
 * Architecture.
 *
 * Note: The word Cache here refers to the boost::unordered_map that
 *       stores the content along with its corresponding name.
 *       The word PolicyIndex refers to the boost::bimap container
 *       used for indexing the stored content in Cache.
 */

class NamedContentCache : public Object
{
public:

   /**
	* \brief Get the type ID.
	* \return the object TypeId
	*/
   static TypeId GetTypeId (void);

   /**
    * \brief The main cache container which stores the data buffer and
    *        corresponding name is a boost::unordered_map. This is because
    *        we don't need our cache entry to be sorted unnecessarily.
    */
   typedef boost::unordered_map <std::string, NamedContentCacheEntry> Cache;

   /**
    * \brief The indexed container which is necessary for the purpose of
    *        eviction based on caching policy, is a boost::bimap. A bimap is
    *        required because we'll be searching for both index and name for
    *        the purpose of eviction and updation respectively.
    */
   typedef boost::bimap<boost::bimaps::multiset_of<uint32_t>, boost::bimaps::unordered_set_of<std::string>> PolicyIndex;

   /**
    * \brief Type Definition for the iterator of the left map of PolicyIndex
    */
   typedef PolicyIndex::left_map::iterator left_iterator;

   /**
    * \brief Type Definition for the iterator of the right map of PolicyIndex
    */
   typedef PolicyIndex::right_map::iterator right_iterator;

   /**
    * \brief Type Definition for the constant iterator of the left map of
    *        PolicyIndex
    */
   typedef PolicyIndex::left_map::const_iterator const_left_iterator;

   /**
    * \brief Type Definition for the constant iterator of the right map of
    *        PolicyIndex
    */
   typedef PolicyIndex::right_map::const_iterator const_right_iterator;

   NamedContentCache();
   virtual ~NamedContentCache();

   /**
    * \brief Set the cache size of the Cache
    * \param size the size to which the cache has to be set
    */
   virtual void SetCacheSize(uint32_t size)= 0;

   /**
    * \brief Get the cache size of the Cache
    * \return the cache size of the Cache
    */
   virtual uint32_t GetCacheSize(void)= 0;

   /**
    * \brief Set the freshness time for the contents of Cache
    * \param time the freshness time in milliseconds
    */
   virtual void SetFreshnessTime(uint64_t time)= 0;

   /**
    * \brief Get the freshness time set for the contents of Cache
    * \return the freshness time in milliseconds
    */
   virtual uint64_t GetFreshnessTime(void)= 0;

   /**
    * \brief Create the Cache entry, which is an object of NamedContentCacheEntry
    *        class. The subclasses will define this function to create an entry
    *        that has the requisite parameters of the content, for the purpose of
    *        indexing content according to caching policy the subclass represents.
    * \param Name name of the content
    * \param packet packet received by ICN Router, with its IP, Transport and
    *        OICN headers removed
    * \param ipheader IP Header of the packet, from which the required parameters
               of the content (present in packet) can retrieved.
    * \return the newly created cache entry
    */
   virtual NamedContentCacheEntry CreateEntry(std::string Name, Ptr<Packet> packet, Ipv4Header ipheader)= 0;

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
   virtual uint32_t CreateIndex(NamedContentCacheEntry entry) = 0;

   /**
    * \brief Insert the Name of the content and its corresponding entry object to
    *        the Cache.
    * \param Name name of the content
    * \param entry the cache entry corresponding to the content
    */
   virtual void InsertToCache(std::string Name, NamedContentCacheEntry entry)= 0;

   /**
    * \brief Insert the index of the content and its corresponding name to
    *        the PolicyIndex.
    * \param index the unique integer index of the content
    * \param Name name of the content
    */
   virtual void InsertToPolicyIndex(uint32_t index, std::string Name)= 0;

   /**
    * \brief Check if the Cache is full, i.e. if the number of contents is equal
    *        the cache size.
    * \return true if the Cache is full
    */
   virtual bool IsFull(void)= 0;

   /**
    * \brief it check whether content is present in the cache or not
    * \param name name of the content
    * \return true if content is not present, false when content is present in the cache
    */
   virtual bool IsUnique (std::string name)= 0;

   /**
    * \brief Check if the content with the given index can be inserted with the
    *        eviction of some other content when the is full. The new content can be
    *        inserted if its index is lesser than (or greater than, when arranged in
    *        descending order) the first content, or if any content's freshness has
    *        expired. Some caching policies, like FIFO, keep on inserting new content,
    *        irrespective of its index. Subclasses representing such policies need not
    *        define and use this function.
    * \param index the unique integer index of the content
    * \return true if any existing content can be evicted so that the new content with
    *         the given index can be inserted.
    */
   virtual bool IsEvictable(uint32_t index) = 0;

   /**
    * \brief Evict the first content in PolicyIndex and also the corresponding entry
    *        in the Cache. This is the main eviction function of NamedContentCache.
    * \return the name of the evicted content
    */
   virtual std::string EvictEntry(void) = 0;

   /**
    * \brief Evict the content with corresponding name from both PolicyIndex and
    *        Cache. This has to be done when a packet having a content that already
    *        exists in the Cache has been received the ICN Router. This operation
    *        basically evicts the older entry of the same content so that the newer
    *        entry can be inserted. This function has to be defined by all subclasses
    *        because every content name in Cache and PolicyIndex has to be unique.
    * \param name name of the non-unique content
    */
   virtual void EvictEntry(std::string Name) = 0;

   /**
    * \brief Find the content with given name in the Cache
    * \return the content in the form of an Application Layer packet
    */
   virtual Ptr<Packet> Find(std::string Name)= 0;

   /**
    * \brief Update the index of the content with the given name. This has to be done
    *        when the content is retrieved from Cache to be sent to a querying
    *        OICN Client. The subclasses will define this function based on the
    *        requirements of the caching policies they represent.
    * \param the name of the content whose index has to be updated
    */
   virtual void UpdateIndex(std::string Name) = 0;

};
}
#endif
