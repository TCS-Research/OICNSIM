/* -*- Mode:C++; c-file-style; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2016, CTO Networks Lab, Tata Consultancy Services (TCS)
 *
 * Author: Suvrat Agrawal <suv.agr@gmail.com>
 */
#include "icn-router.h"
#include "content.h"
#include "icn-manager.h"
#include <stdint.h>
#include <string>
#include <iostream>
#include <utility>
#include <boost/unordered_map.hpp>
#include <new>
#include <vector>
#include <c++/4.8/unordered_map>
#include "ns3/ipv4-address.h"
#include "ns3/callback.h"
#include "ns3/uinteger.h"
#include "ns3/simulator.h"
#include "ns3/ptr.h"
#include "ns3/node.h"
#include "ns3/node-list.h"
#include "ns3/packet.h"
#include "ns3/type-id.h"
#include "icn-manager.h"
#include "ns3/log.h"
#include "oicn-header.h"
#include "ns3/ipv4.h"
#include "ns3/ipv4-interface-address.h"
#include "ns3/node.h"
#include "ns3/socket.h"
#include "ns3/type-name.h"
#include "cache-manager.h"
#include "named-content-cache.h"
#include "sublayer-protocol.h"
NS_LOG_COMPONENT_DEFINE ("IcnRouter");

namespace ns3{

NS_OBJECT_ENSURE_REGISTERED (IcnRouter);

TypeId
IcnRouter::GetTypeId (void)
{

  static TypeId tid = TypeId ("ns3::IcnRouter")
    .SetParent<Application> ()
    .AddConstructor<IcnRouter> ()
	 ;
  return tid;
}

IcnRouter::IcnRouter ()
  : m_port(89),
	m_clientPort(26)
{
}

IcnRouter::~IcnRouter ()
{
}

void
IcnRouter::DoDispose (void)
{
  Application::DoDispose ();
}

void
IcnRouter::StartApplication (void)
{
	if (m_socket == 0)
    {
      TypeId tid = TypeId::LookupByName ("ns3::UdpSocketFactory");
      m_socket = Socket::CreateSocket (GetNode (), tid);
      InetSocketAddress local = InetSocketAddress (Ipv4Address::GetAny(), m_port);
      m_socket->Bind (local);
    }
	  m_socket->SetRecvCallback (MakeCallback (&IcnRouter::HandleReadIcnManger, this));
}

void
IcnRouter::StopApplication ()
{
	if (m_socket != 0)
    {
	  m_socket->Close();
    }
}

void
IcnRouter::HandleReadIcnManger (Ptr<Socket> socket)
{
	Ptr<Packet> packet;
	Address from;
	while ((packet = socket->RecvFrom (from)))
	{
		if (InetSocketAddress::IsMatchingType (from))
		{
			Ipv4Address senderIpv4Address= InetSocketAddress::ConvertFrom (from).GetIpv4 ();
			uint16_t senderPort=InetSocketAddress::ConvertFrom (from).GetPort ();
			ReadPacket(packet,senderIpv4Address,senderPort);
		}
	}
}


void IcnRouter::ReadPacket(Ptr<Packet> packet, Ipv4Address senderIpv4Address, uint16_t senderPort)
{
	OICNHeader oicnHeader;
	packet->RemoveHeader(oicnHeader);
	DnsPlusHeader dnsPlusHeader;
	packet->RemoveHeader(dnsPlusHeader);
	uint8_t qrFlag=dnsPlusHeader.GetQR();
	if( qrFlag  )
	{
		//NS_LOG_INFO(" QR value is 1, it is response from ICN Manager");
		uint8_t opCode=dnsPlusHeader.GetOPCODE();
		if(opCode==13)
		{
			//NS_LOG_INFO("It is a ICN type Response");
			uint16_t qdCount=dnsPlusHeader.GetQDCount();
			uint16_t anCount=dnsPlusHeader.GetANCount();
			while (anCount--)
			{
				DnsPlusQuestionHeader dnsPlusQuestionHeader;
				packet->RemoveHeader(dnsPlusQuestionHeader);
				DnsPlusAnswerSection dnsPlusAnswerSection;
				packet->RemoveHeader(dnsPlusAnswerSection);
				uint16_t qType=dnsPlusAnswerSection.GetClass();
				if(qType==1)
				{
					//NS_LOG_INFO("Qtype is 1, it is a host address ");
					uint16_t qClass=dnsPlusAnswerSection.GetClass();
					if(qClass==1)
					{
						//NS_LOG_INFO("QClass is 1, it is Internet Class ");
						std::string name =dnsPlusAnswerSection.GetName();
						uint32_t clientAddress = dnsPlusAnswerSection.GetRdData();
						if(name.size()>0)
						{
							NS_LOG_INFO(" ICN ROuter Recieved content request for the name "<<name);
							SendAckToIcnManager(senderIpv4Address,senderPort,dnsPlusQuestionHeader,dnsPlusHeader,oicnHeader);
							SendToClient(clientAddress,name);
						}
					}
					else
						NS_LOG_INFO(" Wrong packet format ");
				}
				else
					NS_LOG_INFO(" Wrong packet format ");
			}
		}
	}
	else
	{
		//NS_LOG_DEBUG(" QR value "<< (uint16_t)qrFlag);
		NS_LOG_INFO("Wrong packet format");
		return;
	}
}


void IcnRouter::SendAckToIcnManager(Ipv4Address icnManagerIpv4Address, uint16_t icnManagerPort, DnsPlusQuestionHeader dnsPlusQuestionHeader, DnsPlusHeader dnsPlusHeader,OICNHeader oicnHeader)
{

	Ptr<Packet> packet = Create<Packet>();
	packet->AddHeader(dnsPlusQuestionHeader);
	DnsPlusHeader dnsPlusHeader1;
	dnsPlusHeader1.SetQR(1);
	dnsPlusHeader1.SetOPCODE(15);
	dnsPlusHeader1.SetTC(dnsPlusHeader.GetTC());
	dnsPlusHeader1.SetAA(dnsPlusHeader.GetAA());
	dnsPlusHeader1.SetTC(dnsPlusHeader.GetTC());
	dnsPlusHeader1.SetRD(dnsPlusHeader.GetRD());
	dnsPlusHeader1.SetRA(dnsPlusHeader.GetRA());
	dnsPlusHeader1.SetZ(dnsPlusHeader.GetZ());
	dnsPlusHeader1.SetRCODE(dnsPlusHeader.GetRCODE());
	dnsPlusHeader1.SetQDCount(dnsPlusHeader.GetQDCount());
	dnsPlusHeader1.SetANCount(0);
	dnsPlusHeader1.SetNSCount(0);
	dnsPlusHeader1.SetARCount(0);

	packet->AddHeader(dnsPlusHeader1);
	packet->AddHeader(oicnHeader);
	NS_LOG_INFO(" Sending ACK to ICN Manager ");
	m_socket->Connect (InetSocketAddress (icnManagerIpv4Address, icnManagerPort));
	m_socket->Send(packet);

}

void IcnRouter::SendToClient (uint32_t clientAddress, std::string name)
{
	  Ptr<NamedContentCache> namedcontentcache = m_node->GetObject<NamedContentCache>();
	  Ptr<Packet> packet = namedcontentcache->Find(name)->Copy ();

	  Ptr<SublayerProtocol> sublayerprotocol = m_node->GetObject<SublayerProtocol> ();
	  sublayerprotocol->ConstructPacket(packet, name, clientAddress, m_clientPort);
}

}
