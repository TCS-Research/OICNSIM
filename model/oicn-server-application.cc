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
 * Authors: Suvrat Agrawal <suvrat.a@tcs.com>
 *          Samar Shailendra <s.samar@tcs.com>          
 *
 */

#include "oicn-server-application.h"
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

NS_LOG_COMPONENT_DEFINE ("OicnServer");

namespace ns3{

NS_OBJECT_ENSURE_REGISTERED (OicnServer);

TypeId
OicnServer::GetTypeId (void)
{

  static TypeId tid = TypeId ("ns3::OicnServer")
    .SetParent<Application> ()
    .AddConstructor<OicnServer> ()
  ;
  return tid;
}

OicnServer::OicnServer ()
  : m_port(89),
	m_clientPort(26)

{
	server_content.reserve(100);
	std::pair<std::string,std::string> initializer ("oicn://server_content/initializer ", " Dummy Data ");
	server_content.insert(initializer);
}

OicnServer::~OicnServer ()
{
}

void
OicnServer::DoDispose (void)
{
  Application::DoDispose ();
}

void
OicnServer::StartApplication (void)
{
	if (m_socket == 0)
    {
      TypeId tid = TypeId::LookupByName ("ns3::UdpSocketFactory");
      m_socket = Socket::CreateSocket (GetNode (), tid);
      InetSocketAddress local = InetSocketAddress (Ipv4Address::GetAny(), m_port);
      m_socket->Bind (local);
    }
	  m_socket->SetRecvCallback (MakeCallback (&OicnServer::HandleReadIcnManger, this));
}

void
OicnServer::StopApplication ()
{
	if (m_socket != 0)
    {
	  m_socket->Close();
    }
}

void OicnServer::SetContent (std::vector<int> index)
{
	for (uint32_t i=0; i<index.size(); i++)
    {
		std::string Data = " Dummy Data";
		for (int j = 0; j < 100; j++)
		{
			Data.append(" Dummy Data");
		}
		std::string name = Content::GetContent(index[i]);
		std::pair<std::string, std::string> entry (name, Data);
		server_content.insert(entry);
	 }
}

Ptr<Packet> OicnServer::GetContent (std::string Name)
{
	boost::unordered_map <std::string, std::string>::const_iterator it = server_content.find(Name);
	boost::unordered_map <std::string, std::string>::const_iterator iter;
	std::string data = it->second;
	char buffer[data.size()+1];
	memset (buffer, 0, data.size()+1);
	strcpy(buffer,data.c_str());
	Ptr<Packet> packet = Create<Packet> ((uint8_t*)buffer, data.size());
	return(packet);
}

void OicnServer::SendToClient (uint32_t ClientAddress, std::string Name)
{
	Ipv4Address m_clientaddress;
	m_clientaddress.Set(ClientAddress);
	Ptr<Packet> packet = ConstructSublayer (GetContent(Name), Name)->Copy();
	m_socket->Connect(InetSocketAddress(m_clientaddress, m_clientPort));
	m_socket->Send(packet);
}

Ptr<Packet> OicnServer::ConstructSublayer (Ptr<Packet> packet, std::string Name)
{
	OicnHeader oicnheader;
	oicnheader.SetName (Name);
	oicnheader.SetCachable();
	packet->AddHeader(oicnheader);
	return (packet);
}

void
OicnServer::HandleReadIcnManger (Ptr<Socket> socket)
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

void OicnServer::ReadPacket(Ptr<Packet> packet, Ipv4Address senderIpv4Address, uint16_t senderPort)
{
	OicnHeader oicnHeader;
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
			//NS_LOG_INFO("It is a ICN Response");
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
						//NS_LOG_INFO(" Name "<<name);
						if(name.size()>0)
						{
							NS_LOG_INFO("OICN Server got request of the content name "<<name);
							SendAckToIcnManager(senderIpv4Address,senderPort,dnsPlusQuestionHeader,dnsPlusHeader,oicnHeader);
							SendToClient (clientAddress,name);
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
		NS_LOG_INFO(" Wrong packet format ");
		return;
	}
}


void OicnServer::SendAckToIcnManager(Ipv4Address icnManagerIpv4Address, uint16_t icnManagerPort, DnsPlusQuestionHeader dnsPlusQuestionHeader, DnsPlusHeader dnsPlusHeader,OicnHeader oicnHeader)
{

	Ptr<Packet> packet = Create<Packet>();
	packet->AddHeader(dnsPlusQuestionHeader);
	DnsPlusHeader dnsPlusHeader1;
	dnsPlusHeader1.SetQR(1);
	dnsPlusHeader1.SetOPCODE(14);
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


}


