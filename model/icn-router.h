/* -*- Mode:C++; c-file-style; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2016, CTO Networks Lab, Tata Consultancy Services (TCS)
 *
 * Author: Suvrat Agrawal <suv.agr@gmail.com>
 */

#ifndef ICN_ROUTER_H
#define ICN_ROUTER_H

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
 *\brief ICN Router has the capability to cache the contents.
 *
 * */

class IcnRouter : public Application
{
public:
	/**
	 * \brief Get the type ID.
	 * \return the object TypeId
	 */
	static TypeId GetTypeId (void);
	IcnRouter();
	virtual ~IcnRouter (void);
	/**
	 * \brief Read the recieved packet.
	 *  If packet is content query packet from ICN Manager,
	 *  it send the requested content to client address as mentioned in the packet.
	 * \param packet packet received
	 * \param senderAddress ipv4 address of sender
	 * \param port port of the sender
	 */
	void ReadPacket(Ptr<Packet> packet, Ipv4Address senderAddress, uint16_t senderPort);

	/**
	 * \brief It send the ACK packet to ICN Manager stating that it has received the content query packet.
	 * \param icnManagerIpv4Address ICN Manager Ipv4 address
	 * \param icnManagerPort ICN Manager port
	 * \param dnsPlusQuestionHeader DnsPlus Question header section
	 * \param dnsPlusHeader DnsPlus header section
	 * \param oicnHeader OICN header
	 **/
	void SendAckToIcnManager(Ipv4Address icnManagerIpv4Address, uint16_t icnManagerPort, DnsPlusQuestionHeader dnsPlusQuestionHeader, DnsPlusHeader dnsPlusHeader,OicnHeader oicnHeader);

	/**
	 * \brief It send the cached content to client as directed by the ICN Manager.
	 * \param clientAddress client ipv4 address
	 * \param name name of the requested content
	 **/
	void SendToClient (uint32_t clientAddress, std::string name);

protected:

	virtual void DoDispose (void);

private:

	/**
	 * \brief Handle a packet reception.
	 * This function is called by lower layers.
	 * \param socket the socket the packet was received to.
	 */
	void HandleReadIcnManger (Ptr<Socket> socket);
	virtual void StartApplication (void);
	virtual void StopApplication (void);

	Ptr<Socket> m_socket;//!< socket
	Ptr<Socket> m_socketIcnManager;//!< socket connected to ICN Manager
	uint16_t m_port;//!< ICN Router port it us defined as 89
	uint16_t m_clientPort;//!< ICN Client port defined as 26
};

}
#endif
