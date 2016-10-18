/* -*- Mode:C++; c-file-style; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2016, CTO Networks Lab, Tata Consultancy Services (TCS)
 *
 * Author: Anirudh Morabagal <anirudhmk.2010@gmail.com>
 */

#ifndef CACHE_MANAGER_H
#define CACHE_MANAGER_H

#include "ns3/ptr.h"
#include "ns3/packet.h"
#include "ns3/ipv4-header.h"
#include "ns3/type-name.h"
#include "ns3/type-id.h"
#include "ns3/object.h"
#include "ns3/node.h"

#include <stdint.h>

namespace ns3{
/**
 * \brief Manages the Named Content Cache of an OICN Router
 *
 * This class takes care of the insertion and eviction process of the
 * Cache and PolicyIndex through the functions of Named Content Cache
 * class. It also acts as an Interface between ICN Manager and
 * OICN Router when ICN Manager has to request a particular OICN Router
 * to retrieve a queried content from its cache. The Cache Manager then
 * find the queried content through its name from the cache, creates a
 * packet and sends it down the stack to SublayerProtocol class.
 *
 * This class is instantiated and aggregated to all OICN Routers in
 * the network, and is not required in any other components of OICN
 * Architecture.
 *
 * Note: The word Cache here refers to the boost::unordered_map that
 *       stores the content along with its corresponding name.
 *       The word PolicyIndex refers to the boost::bimap container
 *       used for indexing the stored content in Cache.
 */

class CacheManager : public Object
{
public:

/**
 * \brief Get the type ID.
 * \return the object TypeId
 */
 static TypeId GetTypeId (void);
 CacheManager ();
 virtual ~CacheManager();

 /**
  * \brief Set node associated with this stack.
  * \param node node to set
  */
 void SetNode (Ptr<Node> node);

 /**
  * \brief Get node associated with this stack.
  * \return node of the stack
  */
 Ptr<Node> GetNode ();

 /**
  * \brief Creates a cache entry from the data of the packet and stores
  *        it in the Cache. It also creates an index for the content and
  *        stores it in the PolicyIndex. All these are achieved by calling
  *        the appropriate functions in Named Content Cache class
  * \param packet packet received by OICN Router
  * \param name name of the content present in the packet
  * \param ipheader ip header of the packet
  * \return true if the content is cached
  */
 bool SetEntry (Ptr<Packet> packet, std::string name, Ipv4Header ipheader);

private:
 

 Ptr<Node> m_node;
};
}
#endif
