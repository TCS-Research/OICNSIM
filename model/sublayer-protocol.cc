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

#include "ns3/ipv4-address.h"
#include "ns3/ptr.h"
#include "ns3/node.h"
#include "ns3/packet.h"
#include "ns3/log.h"
#include "ns3/simulator.h"
#include "ns3/type-name.h"
#include "ns3/ipv4-header.h"
#include "ns3/udp-header.h"
#include "ns3/socket.h"
#include "ns3/net-device.h"
#include "ns3/inet-socket-address.h"
#include "ns3/type-id.h"
#include "ns3/node-list.h"
#include "sublayer-protocol.h"
#include "ns3/node.h"
#include "cache-manager.h"
#include "oicn-header.h"
#include <string>

NS_LOG_COMPONENT_DEFINE ("SublayerProtocol");

namespace ns3{

NS_OBJECT_ENSURE_REGISTERED (SublayerProtocol);

TypeId SublayerProtocol::GetTypeId (void)
{
	static TypeId tid = TypeId ("ns3::SublayerProtocol")
    				.SetParent<Object> ()
					.AddConstructor<SublayerProtocol> ();


	return tid;
}

SublayerProtocol::SublayerProtocol ()
: udp_header(),
  oicn_header(),
  m_set_non_cachable(false)
{
}

SublayerProtocol::~SublayerProtocol()
{

}

void SublayerProtocol::SetCachable(bool non_cachable)
{
	m_set_non_cachable = non_cachable;
}

Ptr<Packet> SublayerProtocol::OICNSublayerCheck (Ptr<Packet> packet, Ipv4Header ipheader)
{
	Ptr<Packet> packet1 = packet->Copy ();
	if (HasSublayer(packet1, ipheader))
	{
		NS_LOG_INFO ("Packet contain ICN sublayer");
		std::string Name = oicn_header.GetName();
		if (CheckCache(packet1,Name,ipheader))
		{
			if(m_set_non_cachable)
			{
				oicn_header.SetNonCachable();
			}
			else oicn_header.SetCachable();
			packet1->AddHeader (oicn_header);
			packet1->AddHeader (udp_header);
			return (packet1);
		}
		else return (packet);
	}
	else return (packet);
}

/* The Transport layer of the packet is removed (we'll process only udp packets first) and stored in
 * transportheader, private variable of SublayerProtocol, then checking the existence of OICN sublayer
 * is done.
 */
bool SublayerProtocol::HasSublayer (Ptr<Packet> packet1, Ipv4Header ipheader)    /* The packet buffer here has only transport
                                                        layer and a possible OICN Sublayer. */
{
	UdpHeader udpheader;
	if (isUdp(ipheader))
	{
		NS_LOG_INFO (" Has UDP Header ");
		packet1->RemoveHeader(udpheader);
		if (udpheader.GetSourcePort() == 89)
		{
			NS_LOG_INFO (" Is from OICN Server Port");
			OicnHeader oicnheader;
			packet1->RemoveHeader(oicnheader);
			uint32_t First4Bytes = oicnheader.GetFirst4Bytes()-oicnheader.GetSerializedSize();
			if (First4Bytes == 4026531840)
			{
				NS_LOG_INFO (" Is a Cachable Reply Packet");
				SetUdpHeader(udpheader);
				SetOICNHeader(oicnheader);
				return true;
			}
			else return false;
		}
		else return false;
	}
	else return false;
}
/* Currently we'll not deal with fragmented IP packets as we assume that our OICN Server will
    create Ip packets with minimum MTU to ensure no fragmentation. In reality, fragmentation does
    occur and only the first fragmented IP packet has the OICN Sublayer. Then our code should be
    able to re-assemble subsequent fragments and cache the whole data. */

bool SublayerProtocol::isUdp (Ipv4Header ipheader)
{
	if (int(ipheader.GetProtocol()) ==  17) return true;
	else return false;
}

bool SublayerProtocol::CheckCache (Ptr<Packet> packet1, std::string Name, Ipv4Header ipheader)
{
	Ptr<CacheManager> cachemanager = m_node->GetObject<CacheManager> ();
	if (cachemanager->SetEntry (packet1,Name,ipheader))
	{
		NS_LOG_INFO (" Packet Cached ");
		return true;
	}
	else return false;
}

void SublayerProtocol::ConstructPacket (Ptr<Packet> packet, std::string Name, uint32_t DestinationAddress, uint32_t PortNumber)
{
	// Adding OICN Sublayer to the packet buffer.
	ConstructSublayer(packet, Name);
	Ipv4Address m_destinationaddress;
	m_destinationaddress.Set(DestinationAddress);

	if (m_socket == 0)
	{
		TypeId tid = TypeId::LookupByName ("ns3::UdpSocketFactory");
		m_socket = Socket::CreateSocket (GetNode(), tid);
		InetSocketAddress local = InetSocketAddress (Ipv4Address::GetAny(), 89);//OICN server address
		m_socket->Bind (local);
	}
	m_socket->Connect (InetSocketAddress (m_destinationaddress, PortNumber));
	m_socket->Send(packet);
}

void SublayerProtocol::ConstructSublayer (Ptr<Packet> packet, std::string Name)
{

	OicnHeader oicnheader;
	oicnheader.SetName (Name);

	/* if else condition for constructing sublayer
	 * if the oicn header is set non-cachable while sending itself, the packet will not cache anywhere.
	 * So the commented if else statement is bad code.
	 */
	/*	if(m_set_non_cachable)
	{
		oicnheader.SetNonCachable();
	}
	else*/ oicnheader.SetCachable();
	packet->AddHeader(oicnheader);
}

void SublayerProtocol::SetNode(Ptr<Node> n)
{
	m_node = n;
}

Ptr<Node> SublayerProtocol::GetNode()
{
	return (m_node);
}

void SublayerProtocol::SetUdpHeader (UdpHeader header)
{
	udp_header = header;
}

void SublayerProtocol::SetOICNHeader (OicnHeader header)
{
	oicn_header = header;
}

UdpHeader SublayerProtocol::GetUdpHeader ()
{
	return(udp_header);
}

OicnHeader SublayerProtocol::GetOICNHeader ()
{
	return(oicn_header);
}
} // end of namespace ns3
