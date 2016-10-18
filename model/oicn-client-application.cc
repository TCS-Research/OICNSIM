/* -*- Mode:C++; c-file-style; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2016, CTO Networks Lab, Tata Consultancy Services (TCS)
 *
 * Author: Suvrat Agrawal <suv.agr@gmail.com>
 */
#include "oicn-client-application.h"
#include "sublayer-protocol.h"
#include <stdint.h>
#include <string>
#include <vector>
#include <unordered_map>
#include "ns3/ipv4-address.h"
#include "ns3/callback.h"
#include "ns3/uinteger.h"
#include "ns3/event-id.h"
#include "ns3/inet-socket-address.h"
#include "ns3/address.h"
#include "ns3/node.h"
#include "ns3/node-list.h"
#include "ns3/ptr.h"
#include "ns3/packet.h"
#include "ns3/type-id.h"
#include "icn-manager.h"
#include "oicn-header.h"
#include "ns3/simulator.h"
#include "ns3/log.h"
#include "content.h"
#include "dnsplus-header.h"
#include "ns3/ipv4.h"
#include "ns3/ipv4-l3-protocol.h"
#include "ns3/ipv4-interface-address.h"
#include "ns3/node.h"
#include "ns3/socket.h"
#include "ns3/type-name.h"
#include "dnsplus-question-header.h"

NS_LOG_COMPONENT_DEFINE ("OicnClient");

namespace ns3{

NS_OBJECT_ENSURE_REGISTERED (OicnClient);

TypeId
OicnClient::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::OicnClient")
    .SetParent<Application> ()
    .AddConstructor<OicnClient> ()
	.AddAttribute ("IcnManagerAddress",
			"The destination Address of the outbound packets",
			AddressValue (),
			MakeAddressAccessor (&OicnClient::m_icnManagerAddress),
			MakeAddressChecker ())
	.AddAttribute ("IcnManagerPort",
					"The destination port of the outbound packets",
					UintegerValue (0),
					MakeUintegerAccessor (&OicnClient::m_icnManagerPort),
					MakeUintegerChecker<uint16_t> ())
	.AddAttribute ("MaxPackets",
					"The maximum number of packets the application will send",
					UintegerValue (100),
					MakeUintegerAccessor (&OicnClient::m_count),
					MakeUintegerChecker<uint32_t> ())
	.AddAttribute ("Interval",
					"The time to wait between packets",
					TimeValue (Seconds (1.0)),
					MakeTimeAccessor (&OicnClient::m_interval),
					MakeTimeChecker ());
  return tid;
}

OicnClient::OicnClient ()
{
	m_portFixed = 26;
    m_sent = 0;
	m_i = 0;
	m_index = {0};
	m_sendEvent = EventId();
}

OicnClient::~OicnClient ()
{
 
}

void OicnClient::DoDispose (void)
{
	Application::DoDispose ();
}

void OicnClient::StartApplication (void)
{
	if (m_socketIcnManager == 0)
	{
		TypeId tid = TypeId::LookupByName ("ns3::UdpSocketFactory");
		m_socketIcnManager = Socket::CreateSocket (GetNode (), tid);
		if (Ipv4Address::IsMatchingType(m_icnManagerAddress) == true)
		{
			m_socketIcnManager->Bind();//it can be any port
			m_socketIcnManager->Connect (InetSocketAddress (Ipv4Address::ConvertFrom(m_icnManagerAddress), m_icnManagerPort));
		}
	}
	m_socketIcnManager->SetRecvCallback (MakeCallback (&OicnClient::HandleReadIcnManager, this));
	m_sendEvent = Simulator::Schedule (Seconds (0.0), &OicnClient::Send, this);

	/*This is for the listening from the server, client port is fixed to 26 here*/
	if (m_socketSource == 0)
		{
			TypeId tid = TypeId::LookupByName ("ns3::UdpSocketFactory");
			m_socketSource = Socket::CreateSocket (GetNode (), tid);
			InetSocketAddress local = InetSocketAddress (Ipv4Address::GetAny (), m_portFixed);
			m_socketSource->Bind (local);
		}
	    m_socketSource->SetRecvCallback (MakeCallback (&OicnClient::HandleReadSource, this));
}

void OicnClient::SetContentIndex (std::vector<int> index)
{
	m_index = index;
}

void
OicnClient::ScheduleTransmit (Time dt)
{
  m_sendEvent = Simulator::Schedule (dt, &OicnClient::Send, this);
}

void
OicnClient::Send (void)
{
  std::string name = Content::GetContent (m_index[m_i]);
  m_i++;
  Ptr<Ipv4> ipv4 = GetNode()->GetObject<Ipv4> ();
  uint32_t host_address = ipv4->GetAddress(1, 0).GetLocal().Get();

  Ptr<Packet> packet = Create<Packet>();

  packet = ConstructDnsPlusQuestion(packet,name)->Copy();
  packet = ConstructDnsPlusHeader (packet)->Copy();
  packet = ConstructSublayer (packet,name)->Copy();

  m_socketIcnManager->Send (packet);

  ++m_sent;

  if (Ipv4Address::IsMatchingType (m_icnManagerAddress))
    {
	  NS_LOG_DEBUG (" The Client on node "<<GetNode()->GetId()<<" has queried for content with name "<<name<<" at Time "<<Simulator::Now ().GetSeconds ()<<"s");
    }

  if (m_sent < m_count)
    {
      ScheduleTransmit (m_interval);
    }
}


Ptr<Packet> OicnClient::ConstructDnsPlusHeader (Ptr<Packet> packet)
{
	DnsPlusHeader dnsPlusHeader;
    dnsPlusHeader.SetQR(0);
    dnsPlusHeader.SetOPCODE(11);//OICN Query
    dnsPlusHeader.SetQDCount(1);
    dnsPlusHeader.SetTC(0);
    dnsPlusHeader.SetRD(0);
    packet->AddHeader(dnsPlusHeader);
    return (packet);
}

Ptr<Packet> OicnClient::ConstructDnsPlusQuestion (Ptr<Packet> packet, std::string name)
{
	DnsPlusQuestionHeader dnsPlusQuestionHeader;
	dnsPlusQuestionHeader.SetName(name);
	dnsPlusQuestionHeader.SetQtype(1);//hostaddress
	dnsPlusQuestionHeader.SetQclass(1);//OICN Query
    packet->AddHeader(dnsPlusQuestionHeader);
    return (packet);
}


Ptr<Packet> OicnClient::ConstructSublayer (Ptr<Packet> packet, std::string name)
{
	OicnHeader oicnheader;
	oicnheader.SetName (name);
	oicnheader.SetRequest();
	oicnheader.SetNonCachable();
	packet->AddHeader(oicnheader);
	return (packet);
}

void OicnClient::HandleReadIcnManager (Ptr<Socket> socket)
{
	Ptr<Packet> packet;
	Address from;
	while ((packet = socket->RecvFrom (from)))
	{

		if (InetSocketAddress::IsMatchingType (from))
		{
			OicnHeader oicnheader;
			packet->RemoveHeader(oicnheader);

			DnsPlusHeader dnsPlusHeader;
			packet->RemoveHeader(dnsPlusHeader);

			uint8_t qrFlag=dnsPlusHeader.GetQR();
			if( !qrFlag  )
			{
				//NS_LOG_DEBUG(" QR value is 0, it is query");
				uint8_t opCode=dnsPlusHeader.GetOPCODE();
				if(opCode==12)
				{
					//NS_LOG_INFO("It is a ICN ACK type packet");
					uint8_t rCode=dnsPlusHeader.GetRCODE();
					uint16_t qdCount=dnsPlusHeader.GetQDCount();
					while (qdCount--)
					{
						DnsPlusQuestionHeader dnsPlusQuestionHeader;
						packet->RemoveHeader(dnsPlusQuestionHeader);
						uint16_t qType=dnsPlusQuestionHeader.GetQclass();
						if(qType==1)
						{
							//NS_LOG_INFO("Qtype is 1, it is looking host address ");
							uint16_t qClass=dnsPlusQuestionHeader.GetQclass();
							if(qClass==1)
							{
								//NS_LOG_INFO("QClass is 1, it is Internet Class ");
								std::string name =dnsPlusQuestionHeader.GetName();
								//NS_LOG_INFO(" Name "<<name);
								if(name.size()>0)
								{
									if(rCode==6)
										NS_LOG_DEBUG("Got ACK from IcnManager of name "<<name);
									if(rCode==7)
										NS_LOG_DEBUG("Got NACK from IcnManager of name "<<name);
								}
							}
							else
								NS_LOG_INFO(" Wrong Packet format ");
						}
						else
							NS_LOG_INFO(" Wrong Packet format ");
					}
				}
			}
			else
			{
				//NS_LOG_DEBUG(" QR value "<< (uint16_t)qrFlag);
				NS_LOG_INFO("Wrong Packet format");
				return;
			}
		}
	}
}


void OicnClient::HandleReadSource (Ptr<Socket> socket)
{
	Ptr<Packet> packet;
	Address from;
	while ((packet = socket->RecvFrom (from)))
	{

		if (InetSocketAddress::IsMatchingType (from))
		{
			OicnHeader oicnheader;
			packet->RemoveHeader(oicnheader);
			uint32_t First4Bytes = oicnheader.GetFirst4Bytes()-oicnheader.GetSerializedSize();
			if (First4Bytes == 4026531840)//It is a Cachable Reply Packet
			{
				std::string name = oicnheader.GetName();
				NS_LOG_DEBUG("Content received  at client corresponding to name : "<<name);
			}
			else
			{
				NS_LOG_INFO ("It is not a Cachable Reply Packet ");
			}
		}
	}
}


void OicnClient::StopApplication ()
{

	if (m_socketIcnManager != 0)
    {
      m_socketIcnManager->Close ();
      m_socketIcnManager->SetRecvCallback (MakeNullCallback<void, Ptr<Socket> > ());
      m_socketIcnManager = 0;
    }

	if (m_socketSource != 0)
	    {
	      m_socketSource->Close ();
	      m_socketSource->SetRecvCallback (MakeNullCallback<void, Ptr<Socket> > ());
	      m_socketSource = 0;
	    }

  Simulator::Cancel (m_sendEvent);
}

}
