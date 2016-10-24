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

#ifndef ROUTER_STACK_HELPER_H
#define ROUTER_STACK_HELPER_H

#include <string>
#include "ns3/node-container.h"
#include "ns3/net-device-container.h"
#include "ns3/packet.h"
#include "ns3/ptr.h"
#include "ns3/object-factory.h"
#include "ns3/ipv4-router-l3-protocol.h"
#include "ns3/internet-trace-helper.h"

namespace ns3 {

class Node;
class Ipv4RoutingHelper;

/**
 * \brief Aggregate IP/TCP/UDP and Caching functionality to only ICN Routers
 *        in the Network.
 *
 * This class is exactly same as the InternetStackHelper of ns3. The only
 * difference being that additional APIs that enable caching facility at ICN
 * Routers are also aggregated along with the regular internet stack. Also,
 * unlike InternetStackHelper, this class aggregates Ipv4RouterL3Protocol
 * instead of Ipv4L3Protocol for implementation of the Ipv4 Protocol. This
 * helper is used to aggregate the requisite stack only at ICN Router nodes in
 * a network implementing OICN Architecture, while other nodes use the regular
 * InternetStackHelper of ns3.
 */
class RouterStackHelper : public PcapHelperForIpv4

{
public:
	/**
	 * Create a new RouterStackHelper which uses a mix of static routing
	 * and global routing by default. The static routing protocol
	 * (ns3::Ipv4StaticRouting) and the global routing protocol are
	 * stored in an ns3::Ipv4ListRouting protocol with priorities 0, and -10
	 * by default. If you wish to use different priorites and different
	 * routing protocols, you need to use an adhoc ns3::Ipv4RoutingHelper,
	 * such as ns3::OlsrHelper
	 */
  RouterStackHelper(void);

  /**
   * Destroy the RouterStackHelper
   */
  virtual ~RouterStackHelper(void);

  /**
   * Return helper internal state to that of a newly constructed one
   */
  void Reset (void);

  /**
     * \param routing a new routing helper
     *
     * Set the routing helper to use during Install. The routing
     * helper is really an object factory which is used to create
     * an object of type ns3::Ipv4RoutingProtocol per node. This routing
     * object is then associated to a single ns3::Ipv4 object through its
     * ns3::Ipv4::SetRoutingProtocol.
     */
  void SetRoutingHelper (const Ipv4RoutingHelper &routing);

 
  /**
     * Aggregate implementations of the ns3::Ipv4, ns3::Udp, and ns3::Tcp classes
     * onto the provided node.  This method will assert if called on a node that
     * already has an Ipv4 object aggregated to it.
     *
     * \param nodeName The name of the node on which to install the stack.
     */
  void Install (std::string nodeName) const;

  /**
     * Aggregate implementations of the ns3::Ipv4, ns3::Udp, and ns3::Tcp classes onto
            * the provided node. Also aggregate classes that implement caching functionalities.
     * This method will assert if called on a node that already has an Ipv4 object aggregated
     * to it.
     *
     * \param node The node on which to install the stack.
     */
  void Install (Ptr<Node> node) const;

  /**
     * For each node in the input container, aggregate implementations of the
     * ns3::Ipv4, ns3::Udp, and, ns3::Tcp classes. The program will assert
     * if this method is called on a container with a node that already has
     * an Ipv4 object aggregated to it.
     *
     * \param c NodeContainer that holds the set of nodes on which to install the
     * new stacks.
     */
  void Install (NodeContainer c) const;

  /**
   * Aggregate IPv4, IPv6, UDP, and TCP stacks to all nodes in the simulation
   */
  void InstallAll (void) const;

  /**
     * \brief Enable/disable IPv4 stack install.
     * \param enable enable state
     */
  void SetIpv4StackInstall (bool enable);

  /**
   * \brief Enable/disable IPv4 ARP Jitter.
   * \param enable enable state
   */
  void SetIpv4ArpJitter (bool enable);

  /**
  * Assign a fixed random variable stream number to the random variables
  * used by this model.  Return the number of streams (possibly zero) that
  * have been assigned.  The Install() method should have previously been
  * called by the user.
  *
  * \param stream first stream index to use
  * \param c NodeContainer of the set of nodes for which the internet models
  *          should be modified to use a fixed stream
  * \return the number of stream indices assigned by this helper
  */
  int64_t AssignStreams (NodeContainer c, int64_t stream);

  /**
    * \brief Get the TypeId of the subclass of NamedContentCache, that
    *        implements the required caching policy.
    * \return the TypeId of the subclass
    */
  void SetCacheType (std::string typeId);
  /**
    * \brief Get the TypeId of the subclass of NamedContentCache, that
    *        implements the required caching policy.
    * \return the TypeId of the subclass
    */
  std::string GetCacheType (void);

  /**
    * \brief Set the size of the cache of the ICN Router
    * \param size cache size to be set
    */
  void SetCacheSize(uint32_t size);

  /**
   * \brief Function to set content is cachable or not at ICN Router
   * \param non_cachable parameter to set cacheability of the content, if true then non-cacheable otherwise cacheable
   */
  void ControlCachability (bool non_cachable);

  /**
    * \brief Set the freshness time for the contents of the cache of the
    *        ICN Router
    * \param time freshness time to be set
    */
  void SetFreshnessTime(uint64_t time);

  /**
    * \brief Get the freshness time set for the contents of the cache of
    *        the ICN Router
    * \return the freshness time
    */
  uint64_t GetFreshnessTime(void);

private:
  /**
   * @brief Enable pcap output the indicated Ipv4 and interface pair.
   * @internal
   *
   * @param prefix Filename prefix to use for pcap files.
   * @param ipv4 Ptr to the Ipv4 interface on which you want to enable tracing.
   * @param interface Interface ID on the Ipv4 on which you want to enable tracing.
   * @param explicitFilename Treat the prefix as an explicit filename if true
   */
 virtual void EnablePcapIpv4Internal (std::string prefix,
                                       Ptr<Ipv4> ipv4, 
                                       uint32_t interface,
                                       bool explicitFilename);


  /**
   * \brief Initialize the helper to its default values
   */
  void Initialize (void);


  /**
   * \internal
   * \brief IPv4 routing helper.
   */
  const Ipv4RoutingHelper *m_routing;

  /**
   * \internal
   *
   * \brief create an object from its TypeId and aggregates it to the node
   * \param node the node
   * \param typeId the object TypeId
   */
  static void CreateAndAggregateObjectFromTypeId (Ptr<Node> node, std::string typeId);

  /**
     * \internal
     *
     * \brief create an object, that does not implement a protocol but essential for
     *        caching eg: CacheManager, NamedContentCache from its TypeId and aggregates
     *        it to the node.
     * \param node the node
     * \param typeId the object TypeId
     * \param t a parameter used to distinguish this function from the other
     *        CreateAndAggregateObjectFromTypeId function in this class.
     */

  //This was newly added for router stack helper
  static void CreateAndAggregateObjectFromTypeId (Ptr<Node> node, std::string typeId, bool t);


  /**
   * \internal
   */
  static void Cleanup (void);

  /**
   * \internal
   *
   * \brief checks if there is an hook to a Pcap wrapper
   * \param ipv4 pointer to the IPv4 object
   * \returns true if a hook is found
   */
  bool PcapHooked (Ptr<Ipv4> ipv4);


  /**
   * \internal
   *
   * \brief IPv4 install state (enabled/disabled) ?
   */
  bool m_ipv4Enabled;

  /**
   *\internal
   *
   * \brief IPv4 ARP Jitter state (enabled/disabled) ?
   */
  bool m_ipv4ArpJitterEnabled;

  /**
   *
   *
   * \brief TypeId of the sublclass of NamedContentCache
   */
  std::string cache_type;


  bool m_non_cachable;

  /**
   *
   *
   * \brief Cache size of the cache of ICN Router
   */
  uint32_t cache_size;

  /**
   *
   *
   * \brief freshness time set for the contents of the cache of ICN Router
   */
  uint64_t freshness_time;

};
} // namespace ns3
#endif
