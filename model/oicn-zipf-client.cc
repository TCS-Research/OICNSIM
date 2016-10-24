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

#include "oicn-zipf-client.h"
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
#include "ns3/ipv4.h"
#include "ns3/ipv4-l3-protocol.h"
#include "ns3/ipv4-interface-address.h"
#include "ns3/node.h"
#include "ns3/socket.h"
#include "ns3/type-name.h"
#include "dnsplus-question-header.h"

namespace ns3{

NS_OBJECT_ENSURE_REGISTERED (OICNZipfClient);

NS_LOG_COMPONENT_DEFINE ("OICNZipfClient");

TypeId
OICNZipfClient::GetTypeId (void)
{

	static TypeId tid = TypeId ("ns3::OICNZipfClient")
    		.SetParent<Application> ()
			.AddConstructor<OICNZipfClient> ()
			.AddAttribute ("IcnManagerAddress",
					"The destination Address of the outbound packets",
					AddressValue (),
					MakeAddressAccessor (&OICNZipfClient::m_icnManagerAddress),
					MakeAddressChecker ())
					.AddAttribute ("IcnManagerPort",
							"The destination port of the outbound packets",
							UintegerValue (0),
							MakeUintegerAccessor (&OICNZipfClient::m_icnManagerPort),
							MakeUintegerChecker<uint16_t> ())
							.AddAttribute ("MaxPackets",
									"The maximum number of packets the application will send",
									UintegerValue (100),
									MakeUintegerAccessor (&OICNZipfClient::m_count),
									MakeUintegerChecker<uint32_t> ())
									.AddAttribute ("Interval",
											"The time to wait between packets",
											TimeValue (Seconds (1.0)),
											MakeTimeAccessor (&OICNZipfClient::m_interval),
											MakeTimeChecker ());
	return tid;
}

OICNZipfClient::OICNZipfClient ()
:m_count (0),
 m_sent(0)
{

	m_range = {0, 0};
	m_N = 0;
	m_q = 0.7;
	m_s = 0.7;
	m_sendEvent = EventId();
	m_portFixed = 26;
}

OICNZipfClient::~OICNZipfClient ()
{
}

void OICNZipfClient::DoDispose (void)
{
	Application::DoDispose ();
}

void OICNZipfClient::StartApplication (void)
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
	m_socketIcnManager->SetRecvCallback (MakeCallback (&OICNZipfClient::HandleReadIcnManager, this));
	m_sendEvent = Simulator::Schedule (Seconds (0.0), &OICNZipfClient::Send, this);

	/*This is for the listening from the server, client port is fixed to 26 here*/
	if (m_socketSource == 0)
	{
		TypeId tid = TypeId::LookupByName ("ns3::UdpSocketFactory");
		m_socketSource = Socket::CreateSocket (GetNode (), tid);
		InetSocketAddress local = InetSocketAddress (Ipv4Address::GetAny (), m_portFixed);
		m_socketSource->Bind (local);
	}
	m_socketSource->SetRecvCallback (MakeCallback (&OICNZipfClient::HandleReadSource, this));
}

void OICNZipfClient::SetZipf (std::pair<int, int> range)
{
	m_range = range;
	m_N = range.second - range.first;

	m_Pcum = std::vector<double>(m_N + 1);

	m_Pcum[0] = 0.0;
	for (uint32_t i = 1; i <= m_N; i++)
	{
		m_Pcum[i] = m_Pcum[i - 1] + 1.0 / std::pow(i + m_q, m_s);
	}

	for (uint32_t i = 1; i <= m_N; i++)
	{
		m_Pcum[i] = m_Pcum[i] / m_Pcum[m_N];
	}

}

int OICNZipfClient::GetNextContentIndex ()
{
	uint32_t content_index = m_range.first;
	double p_sum = 0;
	Ptr<UniformRandomVariable> rng = CreateObject<UniformRandomVariable> ();
	double p_random = rng->GetValue(0.0, 1.0);
	while (p_random == 0)
	{
		p_random = rng->GetValue(0.0, 1.0);
	}
	int j = 0;
	for (uint32_t i = m_range.first; i <= m_range.second; i++)
	{
		p_sum = m_Pcum[j]; // m_Pcum[i] = m_Pcum[i-1] + p[i], p[0] = 0;   e.g.: p_cum[1] = p[1],
		j++;			   // p_cum[2] = p[1] + p[2]
		if (p_random <= p_sum)
		{
			content_index = i;
			break;
		} // if
	}   // for
	return content_index;
}


void
OICNZipfClient::Send (void)
{
	NS_LOG_FUNCTION (this);
	std::string name = Content::GetContent (GetNextContentIndex ());
	NS_LOG_INFO("The Zipf Client on node "<< GetNode()->GetId()<<" has queried for content with name "<<name);
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
		NS_LOG_DEBUG (" Node, Packet Number Sent and Time is : "<<GetNode()->GetId()<<" "<<m_sent<<" "<<Simulator::Now ().GetMicroSeconds ());
	}

	if (m_sent < m_count)
	{
		ScheduleTransmit (m_interval);
	}
}

void
OICNZipfClient::ScheduleTransmit (Time dt)
{
	m_sendEvent = Simulator::Schedule (dt, &OICNZipfClient::Send, this);
}

Ptr<Packet> OICNZipfClient::ConstructDnsPlusHeader (Ptr<Packet> packet)
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

Ptr<Packet> OICNZipfClient::ConstructDnsPlusQuestion (Ptr<Packet> packet, std::string name)
{

	DnsPlusQuestionHeader dnsPlusQuestionHeader;
	dnsPlusQuestionHeader.SetName(name);
	dnsPlusQuestionHeader.SetQtype(1);//hostaddress
	dnsPlusQuestionHeader.SetQclass(1);//OICN Query
	packet->AddHeader(dnsPlusQuestionHeader);
	return (packet);
}



Ptr<Packet> OICNZipfClient::ConstructSublayer (Ptr<Packet> packet, std::string name)
{

	OicnHeader oicnheader;
	oicnheader.SetName (name);
	oicnheader.SetRequest();
	oicnheader.SetNonCachable();
	packet->AddHeader(oicnheader);
	return (packet);
}


void OICNZipfClient::HandleReadIcnManager (Ptr<Socket> socket)
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
								if(name.size()>0)
								{
									if(rCode==6)
										NS_LOG_DEBUG("Got ACK from IcnManager of name "<<name);
									if(rCode==7)
										NS_LOG_DEBUG("Got NACK from IcnManager of name "<<name);
								}
							}
							else
								NS_LOG_INFO("Wrong packet format ");
						}
						else
							NS_LOG_INFO("Wrong packet format ");
					}
				}
			}
			else
			{
				NS_LOG_INFO("Wrong packet format ");
				return;
			}
		}
	}
}


void OICNZipfClient::HandleReadSource (Ptr<Socket> socket)
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
				NS_LOG_DEBUG("Name received  at client is "<<name);
				NS_LOG_DEBUG("Node, Packet Number Received and Time is : "<<GetNode()->GetId()<<" "<<m_sent<<" "<<Simulator::Now ().GetSeconds ()<<"s ");
			}
			else
			{
				NS_LOG_INFO ("It is not a Cachable Reply Packet ");
			}
		}
	}
}



void OICNZipfClient::StopApplication ()
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


