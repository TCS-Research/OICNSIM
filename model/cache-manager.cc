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
#include "cache-manager.h"
#include "icn-manager.h"
#include "named-content-cache.h"
#include "ns3/ptr.h"
#include "ns3/log.h"
#include "ns3/packet.h"
#include "sublayer-protocol.h"
#include "ns3/ipv4-header.h"
#include "ns3/ipv4-address.h"
#include "ns3/ipv4.h"

NS_LOG_COMPONENT_DEFINE ("CacheManager");

namespace ns3{

NS_OBJECT_ENSURE_REGISTERED (CacheManager);

TypeId CacheManager::GetTypeId (void)
{
	
  static TypeId tid = TypeId ("ns3::CacheManager")
    .SetParent<Object> ()
    .AddConstructor<CacheManager> ();

  return tid;

}

CacheManager::CacheManager ()
{

}

CacheManager::~CacheManager()
{

}


 void CacheManager::SetNode (Ptr<Node> node)
{
	m_node = node;
}

 Ptr<Node> CacheManager::GetNode ()
{
	return (m_node);
}

bool CacheManager::SetEntry (Ptr<Packet> packet1, std::string Name, Ipv4Header ipheader)
{
	// A cache entry is created and its corresponding index is also calculated based on custom policies
	Ptr<NamedContentCache> namedcontentcache = m_node->GetObject<NamedContentCache>();
    NamedContentCacheEntry entry = namedcontentcache->CreateEntry(Name, packet1, ipheader);
    uint64_t index = namedcontentcache->CreateIndex(entry);
    // Check if the cache is full before inserting, and insert after eviction.
    Ptr<Ipv4> ipv4 = m_node->GetObject<Ipv4> ();
    Ipv4Address routerAddress = ipv4->GetAddress(1, 0).GetLocal();
    //NS_LOG_DEBUG("ipv4 address of router is "<<routerAddress);


    if (namedcontentcache->IsUnique(Name))
    {
    	NS_LOG_DEBUG("Name is unique");
    	if (namedcontentcache->IsFull())
    	{
    		NS_LOG_DEBUG ("Cache is full ");
    	    /*This check is required because some policies may not allow blind eviction. */
    	    if (namedcontentcache->IsEvictable(index))
    	    {
    	    	IcnManager::EvictCachedRouterEntry (routerAddress, namedcontentcache->EvictEntry());
                namedcontentcache->InsertToCache(Name, entry);
                namedcontentcache->InsertToPolicyIndex(index, Name);
               	IcnManager::SetCachedRouterEntry(routerAddress, Name);
                return true;
    	    }
    	    /* The entry was not cached because its index was not higher than the lowest entry already
    	       existing in the cache. */
    	    else
    	    {
    	    	NS_LOG_DEBUG("Not cached, index is not higher");
    	    	return false;
    	    }
    	}
    	// Insert if cache is not full.
        else
        {
        	NS_LOG_DEBUG("There is place for name to be inserted into the cache");
        	namedcontentcache->InsertToCache(Name, entry);
            namedcontentcache->InsertToPolicyIndex(index, Name);
            IcnManager::SetCachedRouterEntry(routerAddress, Name);
            return true;
        }
    }
    else
    {
    		NS_LOG_DEBUG("Name is not unique");
     		namedcontentcache->EvictEntry(Name);
		    namedcontentcache->InsertToCache(Name, entry);
		    namedcontentcache->InsertToPolicyIndex(index, Name);
		    return true;
    }
}

} // end of namespace ns3
