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

#include <iostream>
#include <unordered_map>
#include "ns3/type-name.h"
#include "ns3/simulator.h"
#include "ns3/node.h"
#include "ns3/node-list.h"
#include "ns3/ptr.h"
#include "oicn-client-application.h"
#include "icn-manager.h"
#include "ns3/log.h"
#include "cache-manager.h"
#include "oicn-server-application.h"
#include <c++/4.8/unordered_map>
#include <stdint.h>
#include <cstring>
#include <boost/unordered_map.hpp>
#include <string>
#include <utility>
#include <vector>
#include <string>
#include <cstring>
#include "content.h"
#include <stdint.h>
#include <utility>
#include "ns3/ipv4-address.h"
#include "ns3/ipv4.h"

NS_LOG_COMPONENT_DEFINE ("ICN-Manager");

namespace ns3{

NS_OBJECT_ENSURE_REGISTERED (IcnManager);

TypeId
IcnManager::GetTypeId (void)
{
	
  static TypeId tid = TypeId ("ns3::IcnManager")
    .SetParent<Application> ()
    .AddConstructor<IcnManager> ();
  return tid;
}


IcnManager::IcnManager()
: m_port(36),
  m_sourcePort(89),//oicn server works on well known port 89
  m_clientPort(26),
  m_count(0)
  { }


CachedRouterTable IcnManager::cachedRouterTable;
NameServerTable IcnManager::nameServerTable;

void IcnManager::NameRegisteration(std::vector<std::vector<int>> nameIndex, std::vector<Ipv4Address> serverAddress)
{
	for (uint32_t i=0; i<nameIndex.size(); i++)
	{
		std::vector<int> temp1=nameIndex[i];
		for (uint32_t j=0; j<temp1.size(); j++)
		{

			std::string name = Content::GetContent(temp1[j]);
			std::pair<std::string,Ipv4Address> entry (name, serverAddress[i]);
			nameServerTable.insert(entry);
		}
	}
}


void IcnManager::SetCachedRouterEntry (Ipv4Address routerIpv4Address, std::string name)
{
	std::pair<std::string, Ipv4Address> entry (name, routerIpv4Address);
	cachedRouterTable.insert(entry);
}

void IcnManager::EvictCachedRouterEntry (Ipv4Address routerIpv4Address, std::string name)
{

	std::pair<crIterator, crIterator> iterpair = cachedRouterTable.equal_range(name);
	for (crIterator it = iterpair.first; it != iterpair.second; it++)
	{
		if (it->second == routerIpv4Address)
		{
			cachedRouterTable.erase(it);
			break;
		}
	}

}


void
IcnManager::StartApplication (void)
{
  if (m_socketClient == 0)
    {
      TypeId tid = TypeId::LookupByName ("ns3::UdpSocketFactory");
      m_socketClient = Socket::CreateSocket (GetNode (), tid);
      InetSocketAddress local = InetSocketAddress (Ipv4Address::GetAny (), m_port);
      m_socketClient->Bind (local);
      //NS_LOG_DEBUG("ip which node is bound"<<InetSocketAddress::ConvertFrom (local).GetIpv4 ()<<"Get Node"<<GetNode ()->GetId()<<" Port "<<m_port);
    }
  m_socketClient->SetRecvCallback (MakeCallback (&IcnManager::HandleReadClient, this));

  if (m_socketSource == 0)
      {
        TypeId tid = TypeId::LookupByName ("ns3::UdpSocketFactory");
        m_socketSource = Socket::CreateSocket (GetNode (), tid);
        m_socketSource->Bind ();
      }
    m_socketSource->SetRecvCallback (MakeCallback (&IcnManager::HandleReadSource, this));

}

void
IcnManager::DoDispose (void)
{
  Application::DoDispose ();
}

void
IcnManager::StopApplication ()
{
    if (m_socketClient != 0)
    {
      m_socketClient->Close ();
      m_socketClient->SetRecvCallback (MakeNullCallback<void, Ptr<Socket> > ());
    }
  if (m_socketSource != 0)
     {
       m_socketSource->Close ();
       m_socketSource->SetRecvCallback (MakeNullCallback<void, Ptr<Socket> > ());
     }
}

void
IcnManager::HandleReadClient (Ptr<Socket> socket)
{

	Ptr<Packet> packet;
	Address from;
	while ((packet = socket->RecvFrom (from)))
	{
		if (InetSocketAddress::IsMatchingType (from))
		{
		        NS_LOG_INFO ("At time " << Simulator::Now ().GetSeconds () << "s ICN-Manager received request from client with ip address " <<
								InetSocketAddress::ConvertFrom (from).GetIpv4 ()<<" and port "<< InetSocketAddress::ConvertFrom (from).GetPort ());
			Ipv4Address clientIpv4Address= InetSocketAddress::ConvertFrom (from).GetIpv4 ();
			uint16_t clientPort=InetSocketAddress::ConvertFrom (from).GetPort ();
			uint32_t clientAddress= clientIpv4Address.Get();
			ReadPacket(packet,clientIpv4Address,clientPort);
			m_count++;
		}
	}
}

void
IcnManager::HandleReadSource (Ptr<Socket> socket)
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
			if( qrFlag  )
			{
				//NS_LOG_DEBUG(" QR value is 1, it is response");
				uint8_t opCode=dnsPlusHeader.GetOPCODE();
				if(opCode==14 ||opCode==15 )
				{
					if(opCode==14)
						NS_LOG_INFO("ICN Manager got ACK from server");
					else
						NS_LOG_INFO("ICN Manager got ACK from Cached Source (Router)");
					uint16_t qdCount=dnsPlusHeader.GetQDCount();
					while (qdCount--)
					{
						DnsPlusQuestionHeader dnsPlusQuestionHeader;
						packet->RemoveHeader(dnsPlusQuestionHeader);
						uint16_t qType=dnsPlusQuestionHeader.GetQclass();
						if(qType==1)
						{
							//NS_LOG_INFO("Qtype is 1, it is host address ");
							uint16_t qClass=dnsPlusQuestionHeader.GetQclass();
							if(qClass==1)
							{
								//NS_LOG_INFO("QClass is 1, it is Internet Class ");
								std::string name =dnsPlusQuestionHeader.GetName();
								//NS_LOG_INFO(" Name "<<name);
								if(name.size()>0)
								{
									NS_LOG_DEBUG("ICN Manager Got ACK of name "<<name);
								}
							}
							else
								NS_LOG_INFO("Wrong Packet format");
						}
						else
							NS_LOG_INFO("Wrong Packet format");
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




void IcnManager::ReadPacket(Ptr<Packet> packet, Ipv4Address clientIpv4Address, uint16_t clientPort)
{
	OicnHeader oicnHeader;
	packet->RemoveHeader(oicnHeader);
	DnsPlusHeader dnsPlusHeader;
	packet->RemoveHeader(dnsPlusHeader);
	uint8_t qrFlag=dnsPlusHeader.GetQR();
	if( !qrFlag  )
	{
		//NS_LOG_DEBUG(" QR value is 0, it is query");
		uint8_t opCode=dnsPlusHeader.GetOPCODE();
		if(opCode==11)
		{
			//NS_LOG_INFO("It is a ICN Query");
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
							bool flag=false;
							Ipv4Address const *sourceIpAddress = NameResolution (name,flag);
							if(sourceIpAddress != nullptr)
							{
								SendAckToClient(clientIpv4Address,clientPort,dnsPlusQuestionHeader,dnsPlusHeader,oicnHeader,flag);
								//NS_LOG_INFO("Ip Address "<<*sourceIpAddress);
								SendToSource(*sourceIpAddress, clientIpv4Address,dnsPlusQuestionHeader,dnsPlusHeader,oicnHeader);
							}
							else
							{
								NS_LOG_INFO(" Content Not Found at ICN-Manager ");
								SendAckToClient(clientIpv4Address,clientPort,dnsPlusQuestionHeader,dnsPlusHeader,oicnHeader,flag);
							}
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
		//NS_LOG_DEBUG(" QR value "<< (uint16_t)qrFlag);
		NS_LOG_INFO("Wrong packet format");
		return;
	}
}


Ipv4Address const * IcnManager::NameResolution (std::string name, bool &flag)
{

	crIterator crIt=cachedRouterTable.find(name);
	//NS_LOG_INFO (" Router Table is ");
        //print_map(cachedRouterTable);
	if (crIt != cachedRouterTable.end())
	{

		//NS_LOG_INFO("Taking from router");
		Ipv4Address const * routerAddress = &(crIt->second);
		//NS_LOG_INFO("Router Ip Address "<<*routerAddress);
		NS_LOG_INFO("ICN Manager has found the content name in the Router");
		flag=true;
		return ((routerAddress));
	}
	else
	{
		nsiterator nsi = nameServerTable.find(name);
		//if there more than 1 server for same name then taking only one server i.e. only 1 ipaddress
		if(nsi!= nameServerTable.end())
		{
			NS_LOG_INFO("ICN Manager has found the content name at Server ");
			flag=true;
			return (&(nsi->second));
		}
		else
		{
			NS_LOG_INFO("ICN Manager is not able to find the content name");
			flag=false;
			return nullptr;
		}
		//Send upper hierarchical
	}
}

void IcnManager::SendAckToClient(Ipv4Address  clientIpv4Address, uint16_t clientPort, DnsPlusQuestionHeader dnsPlusQuestionHeader, DnsPlusHeader dnsPlusHeader,OicnHeader oicnHeader, bool &flag)
{
	Ptr<Packet> packet = Create<Packet>();
	packet->AddHeader(dnsPlusQuestionHeader);
	DnsPlusHeader dnsPlusHeader1;
	dnsPlusHeader1.SetQR(0);
	dnsPlusHeader1.SetOPCODE(12);
	if(flag)
		dnsPlusHeader1.SetRCODE(6);
	else
		dnsPlusHeader1.SetRCODE(7);
	dnsPlusHeader1.SetTC(dnsPlusHeader.GetTC());
	dnsPlusHeader1.SetAA(dnsPlusHeader.GetAA());
	dnsPlusHeader1.SetTC(dnsPlusHeader.GetTC());
	dnsPlusHeader1.SetRD(dnsPlusHeader.GetRD());
	dnsPlusHeader1.SetRA(dnsPlusHeader.GetRA());
	dnsPlusHeader1.SetZ(dnsPlusHeader.GetZ());
	dnsPlusHeader1.SetRCODE(dnsPlusHeader.GetRCODE());
	dnsPlusHeader1.SetQDCount(dnsPlusHeader.GetQDCount());
	dnsPlusHeader1.SetANCount(dnsPlusHeader.GetANCount());
	dnsPlusHeader1.SetNSCount(dnsPlusHeader.GetNSCount());
	dnsPlusHeader1.SetARCount(dnsPlusHeader.GetARCount());

	packet->AddHeader(dnsPlusHeader1);
	packet->AddHeader(oicnHeader);

	m_socketClient->Connect (InetSocketAddress (clientIpv4Address, clientPort));
	m_socketClient->Send(packet);

}

void IcnManager::SendToSource(Ipv4Address sourceIpv4Address, Ipv4Address clientIpv4Address,DnsPlusQuestionHeader dnsPlusQuestionHeader,DnsPlusHeader dnsPlusHeader,OicnHeader oicnHeader)
{
	Ptr<Packet> packet = Create<Packet>();

	DnsPlusAnswerSection dnsPlusAnswerSection;

	dnsPlusAnswerSection.SetNameLength(dnsPlusQuestionHeader.GetNameLength());
	dnsPlusAnswerSection.SetName(dnsPlusQuestionHeader.GetName());
	dnsPlusAnswerSection.SetType(dnsPlusQuestionHeader.GetQtype());
	dnsPlusAnswerSection.SetClass(dnsPlusQuestionHeader.GetQclass());
	dnsPlusAnswerSection.SetRdData(clientIpv4Address);

	packet->AddHeader(dnsPlusAnswerSection);
	packet->AddHeader(dnsPlusQuestionHeader);
	DnsPlusHeader dnsPlusHeader1;
	dnsPlusHeader1.SetQR(1);
	dnsPlusHeader1.SetOPCODE(13);
	dnsPlusHeader1.SetTC(dnsPlusHeader.GetTC());
	dnsPlusHeader1.SetAA(dnsPlusHeader.GetAA());
	dnsPlusHeader1.SetTC(dnsPlusHeader.GetTC());
	dnsPlusHeader1.SetRD(dnsPlusHeader.GetRD());
	dnsPlusHeader1.SetRA(dnsPlusHeader.GetRA());
	dnsPlusHeader1.SetZ(dnsPlusHeader.GetZ());
	dnsPlusHeader1.SetRCODE(0);//No error condition
	dnsPlusHeader1.SetQDCount(dnsPlusHeader.GetQDCount());
	dnsPlusHeader1.SetANCount(1);
	dnsPlusHeader1.SetNSCount(dnsPlusHeader.GetNSCount());
	dnsPlusHeader1.SetARCount(dnsPlusHeader.GetARCount());
	packet->AddHeader(dnsPlusHeader1);
	packet->AddHeader(oicnHeader);

	NS_LOG_DEBUG("ICN Manager sending content request to Source with address "<<sourceIpv4Address<<" and port "<<m_sourcePort);
	m_socketSource->Connect(InetSocketAddress (sourceIpv4Address, m_sourcePort));
	m_socketSource->Send(packet);
}

}
