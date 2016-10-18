/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2016 TCS (Tata Consultancy Services) Research
 *
 *
 * Author: Suvrat Agrawal <suv.agr@gmail.com>
 * \brief This is example script of working of O-ICN  Architecture
 */
//
// Topology
//		    		   ICN Manager (Node-5)
// 			     	  .	    	      .
//				     .	     	       .
//			    	.  	      	    	.
//		           .   	       		     .
//		          .    	        	      .
//     		ICN Router1 ++++++++++++ Legacy Router ++++++++++ ICN Router2 +++++++ Server
//			  (Node-1)	               (Node-2)	               (Node-3)	          (Node-4)
//			  +      +
//	         +	      +
//	        +  	       +
//	       +  	        +
//	    Client1        Client2
//	   (Node-0)	       (Node-6)

#include <iostream>
#include <fstream>
#include <string>
#include <cassert>
#include <vector>

#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/applications-module.h"
#include "ns3/o-icn-module.h"
#include "ns3/flow-monitor-helper.h"
#include "ns3/ipv4-global-routing-helper.h"
#include <iostream>
#include <unordered_map>
using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("OicnExampleLRU");

int
main (int argc, char *argv[])
{

  // Enable logging
  LogComponentEnable ("ICN-Manager", LOG_LEVEL_INFO);
  LogComponentEnable("IcnRouter",LOG_LEVEL_INFO);
  LogComponentEnable ("OicnServer", LOG_LEVEL_INFO);
  LogComponentEnable ("OICNZipfClient", LOG_LEVEL_INFO);
  LogComponentEnable ("OicnClient", LOG_LEVEL_INFO);
  LogComponentEnable ("OicnExampleLRU", LOG_LEVEL_INFO);

  NodeContainer c;
  c.Create (7);
  NodeContainer n0n1 = NodeContainer (c.Get (0), c.Get (1));
  NodeContainer n1n2 = NodeContainer (c.Get (1), c.Get (2));
  NodeContainer n2n3 = NodeContainer (c.Get (2), c.Get (3));
  NodeContainer n3n4 = NodeContainer (c.Get (3), c.Get (4));
  NodeContainer n2n5 = NodeContainer (c.Get (2), c.Get (5));
  NodeContainer n1n6 = NodeContainer (c.Get (1), c.Get (6));

  NodeContainer normal;
  normal.Add(c.Get(0));
  normal.Add(c.Get(2));
  normal.Add(c.Get(4));
  normal.Add(c.Get(5));
  normal.Add(c.Get(6));

  InternetStackHelper internet;
  internet.Install (normal);

  NodeContainer icn_routers;
  icn_routers.Add(c.Get(1));
  icn_routers.Add(c.Get(3));

  //it install Internet stack and caching functionality to the ICN Router nodes
  RouterStackHelper router;
  router.SetCacheType("ns3::CacheWithLRU");
  router.ControlCachability(false);
  router.SetCacheSize(3);
  router.Install (icn_routers);

  PointToPointHelper p2p;
  p2p.SetDeviceAttribute ("DataRate", StringValue ("5120kbps"));
  p2p.SetChannelAttribute ("Delay", StringValue ("2ms"));

  NetDeviceContainer d0d1 = p2p.Install (n0n1);
  NetDeviceContainer d1d2 = p2p.Install (n1n2);
  NetDeviceContainer d2d3 = p2p.Install (n2n3);
  NetDeviceContainer d3d4 = p2p.Install (n3n4);
  NetDeviceContainer d2d5 = p2p.Install (n2n5);
  NetDeviceContainer d1d6 = p2p.Install (n1n6);

  Ipv4Address serverAddress;

  Ipv4AddressHelper ipv4;
  ipv4.SetBase ("10.1.1.0", "255.255.255.0");
  Ipv4InterfaceContainer i0i1 = ipv4.Assign (d0d1);

  ipv4.SetBase ("10.1.2.0", "255.255.255.0");
  Ipv4InterfaceContainer i1i2 = ipv4.Assign (d1d2);

  ipv4.SetBase ("10.1.3.0", "255.255.255.0");
  Ipv4InterfaceContainer i2i3 = ipv4.Assign (d2d3);

  ipv4.SetBase ("10.1.4.0", "255.255.255.0");
  Ipv4InterfaceContainer i3i4 = ipv4.Assign (d3d4);

  ipv4.SetBase ("10.1.5.0", "255.255.255.0");
  Ipv4InterfaceContainer i2i5 = ipv4.Assign (d2d5);

  ipv4.SetBase ("10.1.6.0", "255.255.255.0");
  Ipv4InterfaceContainer i1i6 = ipv4.Assign (d1d6);

  serverAddress= i3i4.GetAddress (1);
  Ipv4Address icnManagerAddress = i2i5.GetAddress (1);

  Ipv4GlobalRoutingHelper::PopulateRoutingTables ();

  //Indices of the contents that the server will contain
  std::vector<int> sindex (10000);
  for (int i=0; i<10000; i++)
  {
	  sindex[i]=i;
  }

   std::vector<int> index1 (5000);

  // The OICN Servers being installed on nodes 4
  OicnServerHelper server;
  ApplicationContainer app1 = server.Install (c.Get (4), sindex);
  app1.Start (Seconds (0.0));
  app1.Stop (Seconds (3200.0));

  std::vector<std::vector<int>> nameIndexes{sindex};
  std::vector<Ipv4Address>serverAddressContainer{serverAddress};

  IcnManagerHelper icnManager ;
  //Installing IcnManager Application, ICN Manager contain the name indices and corresponding server address
  ApplicationContainer appIcnManager = icnManager.Install (c.Get (5),nameIndexes,serverAddressContainer);
  appIcnManager.Start (Seconds (0.0));
  appIcnManager.Stop (Seconds (3200.0));

//ICN Manager listen on port no 36
  uint16_t icnManagerPort = 36;



  //Maximum number of packet that client will request
  uint32_t maxPacketCount = 5;
  //Packet inter-send time
  Time interPacketInterval = MilliSeconds (120);

  //it is range of name indices that client will ask for
  std::pair<int, int> range = std::make_pair(1, 5);
  OicnZipfClientHelper zipfclient(icnManagerAddress, icnManagerPort);

  zipfclient.SetAttribute ("MaxPackets", UintegerValue (maxPacketCount));
  zipfclient.SetAttribute ("Interval", TimeValue (interPacketInterval));

  ApplicationContainer zipfapps, zipfappsall;
  Ptr<UniformRandomVariable> j = CreateObject<UniformRandomVariable> ();

  //list of all clients nodes, here is only one client node i.e node 0
  std::vector<uint32_t> client_nodes = {0};

  //installing OICN Zipf Client
  for (uint32_t i = 0; i < client_nodes.size(); i++)
  {
  	zipfapps = zipfclient.Install(c.Get(client_nodes[i]),range);
  	zipfapps.Start( Seconds (j->GetValue(0.0, 20.0)));
  	zipfapps.Stop( Seconds ( 1600.0 + j->GetValue(0.0, 50.0)));
    zipfappsall.Add(zipfapps);
  }

  //indices of content
  for (int i=0; i<5; i++)
   {
 	  index1[i]=i;
   }

  //Installing the non zipf OICN client
   OicnClientHelpher oicnClient (icnManagerAddress, icnManagerPort);
   oicnClient.SetAttribute ("MaxPackets", UintegerValue (maxPacketCount));
   oicnClient.SetAttribute ("Interval", TimeValue (interPacketInterval));
   ApplicationContainer appOicnClient = oicnClient.Install (c.Get (6),index1);
   appOicnClient.Start (Seconds (0.0));
   appOicnClient.Stop (Seconds (1660.0));


 //Installing ICN Router application
  IcnRouterHelpher icnRouter;
  ApplicationContainer appicnRouter = icnRouter.Install (icn_routers);
  appicnRouter.Start (Seconds (0.0));
  appicnRouter.Stop (Seconds (3200.0));

  Simulator::Stop (Seconds (3200.0));

  Simulator::Run ();
  Simulator::Destroy ();

  return 0;
}
