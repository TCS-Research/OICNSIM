/* -*- Mode:C++; c-file-style; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2016, CTO Networks Lab, Tata Consultancy Services (TCS)
 *
 * Author: Suvrat Agrawal <suv.agr@gmail.com>
 */
#ifndef OICN_CLIENT_APPLICATION_H
#define OICN_CLIENT_APPLICATION_H

#include "ns3/application.h"
#include "ns3/ipv4-address.h"
#include <vector>
#include <stdint.h>
#include <unordered_map>
#include "ns3/event-id.h"
#include "ns3/type-name.h"
#include "ns3/type-id.h"
#include "ns3/socket.h"
#include "ns3/ptr.h"
#include "ns3/packet.h"
#include "ns3/node.h"


namespace ns3{

/**
 * \brief OICN client application requests for content name resolution to ICN Manager.
 *  ICN Manager in turn resolve the name request and direct source of content (ICN router or server) to send the content to client.
 * */
class OicnClient : public Application
{
public:

	/**
	 * \brief Get the type ID.
	 * \return the object TypeId
	 */
	static TypeId GetTypeId (void);
	OicnClient ();
	virtual ~OicnClient (void);

	/**
	 * \brief Set the content index.
	 * Here name are constructed based upon index.
	 * \param index value of the index
	 */
	void SetContentIndex (std::vector<int> index);

	/**
	 * \brief construct the sublayer header
	 * \param packet packet constructed by application layer
	 * \param name name of the content
	 * \return packet constructed after adding sublayer
	 */
	Ptr<Packet> ConstructSublayer (Ptr<Packet> packet, std::string name);

	/**
	 * \brief construct the DNS Plus header
	 * \param packet packet being constructed by application layer
	 * \return  packet constructed after adding DNS Plus header
	 */
	Ptr<Packet> ConstructDnsPlusHeader (Ptr<Packet> packet);

	/**
	 * \brief construct the DNS Plus Question Section
	 * \param packet packet being constructed by application layer
	 * \param name name of the content
	 * \return packet constructed after adding DNS Plus Question Section
	 */
	Ptr<Packet> ConstructDnsPlusQuestion (Ptr<Packet> packet, std::string name);

protected:

	virtual void DoDispose (void);

private:

	/**
	 * \brief Handle a packet reception from ICN Manager port (36).
	 * This function is called by lower layers.
	 * \param socket the socket the packet was received to.
	 */
	void HandleReadIcnManager (Ptr<Socket> socket);

	/**
	 * \brief Handle a packet reception from Source port (ICN Router or Server) (89).
	 * This function is called by lower layers.
	 * \param socket the socket the packet was received to.
	 */
	void HandleReadSource (Ptr<Socket> socket);
	virtual void StartApplication (void);
	virtual void StopApplication (void);

	/**
	 * \brief Schedule the next packet transmission
	 * \param dt time interval between packets.
	 */
	void ScheduleTransmit (Time dt);

	/**
	 * \brief Send a packet to ICN Manager
	 */
	void Send (void);

	std::vector<int> m_index;//!< index of the content name, name of content are formed based on index
	uint32_t m_i;//!< index of vector m_index
	uint32_t m_sent; //!< counter for sent packets
	uint32_t m_count;//!< the maximum number of packets the application will send
	//uint16_t m_portDynamic;//!<
	uint16_t m_portFixed;//!< client port is attached with port no 26, it will listen to this port from OICN Source(ICN router or server)
	Ptr<Socket> m_socketIcnManager;//!< socket associated with ICN Manager
	Ptr<Socket> m_socketSource;//!< socket associated with Source
	EventId m_sendEvent;//!< event to send the next packet
	Time m_interval;//!< packet inter-send time
	Address m_icnManagerAddress; //!< ICN Manager Address
	uint16_t m_icnManagerPort; //!< ICN Manager port
};
}
#endif
