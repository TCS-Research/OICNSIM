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

#ifndef OICN_SERVER_APPLICATION_H
#define OICN_SERVER_APPLICATION_H

#include "ns3/application.h"
#include "ns3/ipv4-address.h"
#include <vector>
#include <stdint.h>
#include <boost/unordered_map.hpp>
#include <string>
#include <iostream>

#include "dnsplus-header.h"
#include "ns3/type-name.h"
#include "ns3/socket.h"
#include "ns3/node.h"
#include "ns3/ptr.h"
#include "ns3/packet.h"
#include "ns3/type-id.h"
#include "oicn-header.h"
#include "dnsplus-question-header.h"


namespace ns3{
/**
 *\brief OICN Server Application, it accept request of content from ICN Manager and send to the client.
 *
 *  */

class OicnServer : public Application
{
public:
	
	/**
	 * \brief Get the type ID.
	 * \return the object TypeId
	 */
	static TypeId GetTypeId (void);
	OicnServer ();
	virtual ~OicnServer (void);

	/**
	 * \brief During initialisation of server, it sets the content that server will contain.
	 * Content list will remain same during the simulation
	 * \param index index of content, which the server will contain
	 */
	void SetContent (std::vector<int> index);

	/**
	 * \brief It retrieves the content corresponding to the name, passed as the parameter to the function.
	 * \param name name of the content
	 * \return packet containing the content, corresponding to the name mentioned in the parameter
	 */
	Ptr<Packet> GetContent (std::string name);

	/**
	 * \brief Construct the sublayer header.
	 * \param packet packet constructed by application layer
	 * \param name name of the content
	 * \return packet constructed after adding sublayer
	 */
	Ptr<Packet> ConstructSublayer (Ptr<Packet> packet, std::string name);

	/**
	 * \brief Send the content to client.
	 * \param clientAddress address of the client
	 * \param name name of the content
	 **/
	void SendToClient (uint32_t clientAddress, std::string name);

	/**
	 * \brief Read the packet received at ICN Manager port i.e. 89.
	 * \param packet packet received at ICN Manager port
	 * \param senderAddress Ipv4 address of the sender
	 * \param senderPort port of the sender
	 **/
	void ReadPacket(Ptr<Packet> packet, Ipv4Address senderAddress, uint16_t senderPort);

	/**
	 * \brief After receiving the content request from the ICN Manager, it send back the ACK of same to ICN Manager.
	 * \param icnManagerIpv4Address ICN Manager Ipv4 address to which ACK has to be sent
	 * \param icnManagerPort ICN Manager port at which ACK has to be sent
	 * \param dnsPlusQuestionHeader dns Plus Question header section of the packet
	 * \param dnsPlusHeader dns Plus header section of the packet
	 * \param oicnHeader oicn header section of the packet
	 **/
	void SendAckToIcnManager(Ipv4Address icnManagerIpv4Address, uint16_t icnManagerPort, DnsPlusQuestionHeader dnsPlusQuestionHeader, DnsPlusHeader dnsPlusHeader,OicnHeader oicnHeader);

protected:
	
	virtual void DoDispose (void);

private:

	/**
	 * \brief Handle a packet reception from ICN Manager port (36).
	 * This function is called by lower layers.
	 * \param socket the socket the packet was received to
	 */
	void HandleReadIcnManger (Ptr<Socket> socket);
	virtual void StartApplication (void);
	virtual void StopApplication (void);

	boost::unordered_map <std::string, std::string> server_content; //!< It contain name and corresponding content
	Ptr<Socket> m_socket;//!< socket
	Ptr<Socket> m_socketIcnManager;//!< socket connected to ICN Manager port 36
	uint16_t m_port;//!< port at which server listens it defined as 89
	uint16_t m_clientPort;//!< port at which client will listen to the request, here defined as 26
};
}
#endif
