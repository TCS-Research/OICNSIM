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

#include "ns3/assert.h"
#include "ns3/log.h"
#include "ns3/object.h"
#include "ns3/names.h"
#include "ns3/ipv4.h"
#include "ns3/packet-socket-factory.h"
#include "ns3/config.h"
#include "ns3/simulator.h"
#include "ns3/string.h"
#include "ns3/net-device.h"
#include "ns3/callback.h"
#include "ns3/node.h"
#include "ns3/node-list.h"
#include "ns3/core-config.h"
#include "ns3/arp-l3-protocol.h"
#include "ns3/named-content-cache.h"
#include "router-stack-helper.h"
#include "ns3/sublayer-protocol.h"
#include "ns3/ipv4-global-routing.h"
#include "ns3/ipv4-list-routing-helper.h"
#include "ns3/ipv4-static-routing-helper.h"
#include "ns3/ipv4-global-routing-helper.h"
#include "ns3/global-router-interface.h"
#include <limits>
#include <map>

NS_LOG_COMPONENT_DEFINE ("RouterStackHelper");

namespace ns3 {

#define INTERFACE_CONTEXT

//
typedef std::pair<Ptr<Ipv4>, uint32_t> InterfacePairIpv4;  /*< Ipv4/interface pair */
typedef std::map<InterfacePairIpv4, Ptr<PcapFileWrapper> > InterfaceFileMapIpv4;  /*< Ipv4/interface and Pcap file wrapper container */
typedef std::map<InterfacePairIpv4, Ptr<OutputStreamWrapper> > InterfaceStreamMapIpv4;  /*< Ipv4/interface and output stream container */

static InterfaceFileMapIpv4 g_interfaceFileMapIpv4; /*< A mapping of Ipv4/interface pairs to pcap files */
static InterfaceStreamMapIpv4 g_interfaceStreamMapIpv4; /*< A mapping of Ipv4/interface pairs to ascii streams */


RouterStackHelper::RouterStackHelper ()
  : m_routing (0),
	cache_type(),
	m_non_cachable(true),
    m_ipv4Enabled (true),
    m_ipv4ArpJitterEnabled (true),
	freshness_time(0)

{
  Initialize ();
}

// private method called by both constructor and Reset ()
void
RouterStackHelper::Initialize ()
{
  Ipv4StaticRoutingHelper staticRouting;
  Ipv4GlobalRoutingHelper globalRouting;
  Ipv4ListRoutingHelper listRouting;
  listRouting.Add (staticRouting, 0);
  listRouting.Add (globalRouting, -10);
  SetRoutingHelper (listRouting);
}

RouterStackHelper::~RouterStackHelper ()
{
  delete m_routing;
}


void
RouterStackHelper::Reset (void)
{
  delete m_routing;
  m_routing = 0;
  m_ipv4Enabled = true;
  m_ipv4ArpJitterEnabled = true;
  Initialize ();
}

void 
RouterStackHelper::SetRoutingHelper (const Ipv4RoutingHelper &routing)
{
  delete m_routing;
  m_routing = routing.Copy ();
}

void
RouterStackHelper::SetIpv4StackInstall (bool enable)
{
  m_ipv4Enabled = enable;
}

void RouterStackHelper::SetIpv4ArpJitter (bool enable)
{
  m_ipv4ArpJitterEnabled = enable;
}

int64_t
RouterStackHelper::AssignStreams (NodeContainer c, int64_t stream)
{
  int64_t currentStream = stream;
  for (NodeContainer::Iterator i = c.Begin (); i != c.End (); ++i)
    {
      Ptr<Node> node = *i;
      Ptr<GlobalRouter> router = node->GetObject<GlobalRouter> ();
      if (router != 0)
        {
          Ptr<Ipv4GlobalRouting> gr = router->GetRoutingProtocol ();
          if (gr != 0)
            {
              currentStream += gr->AssignStreams (currentStream);
            }
        }
    
      Ptr<Ipv4> ipv4 = node->GetObject<Ipv4> ();
      if (ipv4 != 0)
        {
          Ptr<ArpL3Protocol> arpL3Protocol = ipv4->GetObject<ArpL3Protocol> ();
          if (arpL3Protocol != 0)
            {
              currentStream += arpL3Protocol->AssignStreams (currentStream);
            }
        }
    }
  return (currentStream - stream);
}

void 
RouterStackHelper::Install (NodeContainer c) const
{
  for (NodeContainer::Iterator i = c.Begin (); i != c.End (); ++i)
    {
      Install (*i);
    }
}

void 
RouterStackHelper::InstallAll (void) const
{
  Install (NodeContainer::GetGlobal ());
}

void
RouterStackHelper::CreateAndAggregateObjectFromTypeId (Ptr<Node> node, std::string typeId)
{
  ObjectFactory factory;
  factory.SetTypeId (typeId);
  Ptr<Object> protocol = factory.Create <Object> ();
  node->AggregateObject (protocol);
}

//This function is different from internet stack helper
void
RouterStackHelper::CreateAndAggregateObjectFromTypeId (Ptr<Node> node, std::string typeId, bool t)
{
  ObjectFactory factory;
  factory.SetTypeId (typeId);
  Ptr<Object> table = factory.Create <Object> ();
  node->AggregateObject (table);
}

void
RouterStackHelper::Install (Ptr<Node> node) const
{
  if (m_ipv4Enabled)
    {
      if (node->GetObject<Ipv4> () != 0)
        {
          NS_FATAL_ERROR ("RouterStackHelper::Install (): Aggregating " 
                          "an InternetStack to a node with an existing Ipv4 object");
          return;
        }

      CreateAndAggregateObjectFromTypeId (node, "ns3::ArpL3Protocol");
      CreateAndAggregateObjectFromTypeId (node, "ns3::SublayerProtocol");
      CreateAndAggregateObjectFromTypeId (node, "ns3::CacheManager", true);
      CreateAndAggregateObjectFromTypeId (node, cache_type, true);
      CreateAndAggregateObjectFromTypeId (node, "ns3::Ipv4RouterL3Protocol");
      CreateAndAggregateObjectFromTypeId (node, "ns3::Icmpv4L4Protocol");
      CreateAndAggregateObjectFromTypeId (node, "ns3::UdpL4Protocol");


      if (m_ipv4ArpJitterEnabled == false)
        {
          Ptr<ArpL3Protocol> arp = node->GetObject<ArpL3Protocol> ();
          NS_ASSERT (arp);
          arp->SetAttribute ("RequestJitter", StringValue ("ns3::ConstantRandomVariable[Constant=0.0]"));
        }
      // Set routing
      Ptr<Ipv4> ipv4 = node->GetObject<Ipv4> ();
      Ptr<Ipv4RoutingProtocol> ipv4Routing = m_routing->Create (node);
      ipv4->SetRoutingProtocol (ipv4Routing);

      Ptr<SublayerProtocol> sublayerprotocol = node->GetObject<SublayerProtocol>();
      sublayerprotocol->SetCachable(m_non_cachable);
      Ptr<NamedContentCache> cache = node->GetObject<NamedContentCache>();
      cache->SetCacheSize(cache_size);
      cache->SetFreshnessTime(freshness_time);
    }
}

void
RouterStackHelper::Install (std::string nodeName) const
{
  Ptr<Node> node = Names::Find<Node> (nodeName);
  Install (node);
}

void RouterStackHelper::SetFreshnessTime(uint64_t time)
{
	freshness_time = time;
}

uint64_t RouterStackHelper::GetFreshnessTime()
{
	return (freshness_time);
}

void RouterStackHelper::ControlCachability (bool non_cachable)
{
	m_non_cachable = non_cachable;
}

void
RouterStackHelper::SetCacheType (std::string typeId)
{
  cache_type = typeId;
}

std::string
RouterStackHelper::GetCacheType ()
{
  return (cache_type);
}

void RouterStackHelper::SetCacheSize(uint32_t size)
{
	cache_size = size;
}
/*
 * \brief Sync function for IPv4 packet - Pcap output
 * \param p smart pointer to the packet
 * \param ipv4 smart pointer to the node's IPv4 stack
 * \param interface incoming interface
 */
static void
Ipv4RouterL3ProtocolRxTxSink (Ptr<const Packet> p, Ptr<Ipv4> ipv4, uint32_t interface)
{
  NS_LOG_FUNCTION (p << ipv4 << interface);

  //
  // Since trace sources are independent of interface, if we hook a source
  // on a particular protocol we will get traces for all of its interfaces.
  // We need to filter this to only report interfaces for which the user 
  // has expressed interest.
  //
  InterfacePairIpv4 pair = std::make_pair (ipv4, interface);
  if (g_interfaceFileMapIpv4.find (pair) == g_interfaceFileMapIpv4.end ())
    {
      NS_LOG_INFO ("Ignoring packet to/from interface " << interface);
      return;
    }

  Ptr<PcapFileWrapper> file = g_interfaceFileMapIpv4[pair];
  file->Write (Simulator::Now (), p);
}

bool
RouterStackHelper::PcapHooked (Ptr<Ipv4> ipv4)
{
  for (  InterfaceFileMapIpv4::const_iterator i = g_interfaceFileMapIpv4.begin (); 
         i != g_interfaceFileMapIpv4.end (); 
         ++i)
    {
      if ((*i).first.first == ipv4)
        {
          return true;
        }
    }
  return false;
}

void 
RouterStackHelper::EnablePcapIpv4Internal (std::string prefix, Ptr<Ipv4> ipv4, uint32_t interface, bool explicitFilename)
{
  NS_LOG_FUNCTION (prefix << ipv4 << interface);

  if (!m_ipv4Enabled)
    {
      NS_LOG_INFO ("Call to enable Ipv4 pcap tracing but Ipv4 not enabled");
      return;
    }

  //
  // We have to create a file and a mapping from protocol/interface to file 
  // irrespective of how many times we want to trace a particular protocol.
  //
  PcapHelper pcapHelper;

  std::string filename;
  if (explicitFilename)
    {
      filename = prefix;
    }
  else
    {
      filename = pcapHelper.GetFilenameFromInterfacePair (prefix, ipv4, interface);
    }

  Ptr<PcapFileWrapper> file = pcapHelper.CreateFile (filename, std::ios::out, PcapHelper::DLT_RAW);

  //
  // However, we only hook the trace source once to avoid multiple trace sink
  // calls per event (connect is independent of interface).
  //
  if (!PcapHooked (ipv4))
    {
      //
      // Ptr<Ipv4> is aggregated to node and ipv4routerl3protocol is aggregated to
      // node so we can get to ipv4routerl3protocol through Ipv4.
      //
      Ptr<Ipv4RouterL3Protocol> ipv4routerl3protocol = ipv4->GetObject<Ipv4RouterL3Protocol> ();
      NS_ASSERT_MSG (ipv4routerl3protocol, "RouterStackHelper::EnablePcapIpv4Internal(): "
                     "m_ipv4Enabled and ipv4routerl3protocol inconsistent");

      bool result = ipv4routerl3protocol->TraceConnectWithoutContext ("Tx", MakeCallback (&Ipv4RouterL3ProtocolRxTxSink));
      NS_ASSERT_MSG (result == true, "RouterStackHelper::EnablePcapIpv4Internal():  "
                     "Unable to connect ipv4routerl3protocol \"Tx\"");

      result = ipv4routerl3protocol->TraceConnectWithoutContext ("Rx", MakeCallback (&Ipv4RouterL3ProtocolRxTxSink));
      NS_ASSERT_MSG (result == true, "RouterStackHelper::EnablePcapIpv4Internal():  "
                     "Unable to connect ipv4routerl3protocol \"Rx\"");
    }

  g_interfaceFileMapIpv4[std::make_pair (ipv4, interface)] = file;
}

} // namespace ns3

