/* -*- Mode:C++; c-file-style; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2016, CTO Networks Lab, Tata Consultancy Services (TCS)
 *
 * Author: Anirudh Morabagal <anirudhmk.2010@gmail.com>
 */

#ifndef SUBLAYER_PROTOCOL_H
#define SUBLAYER_PROTOCOL_H

#include "ns3/ptr.h"
#include "ns3/packet.h"
#include "ns3/udp-header.h"
#include "oicn-header.h"
#include "ns3/type-name.h"
#include "ns3/type-id.h"
#include "ns3/ipv4-header.h"
#include "ns3/socket.h"
#include "ns3/object.h"
#include "ns3/node.h"
#include <stdint.h>
#include <string>

namespace ns3{
/**
 * \brief Implements the OICN Sublayer.
 *
 * This class implements the functions that ensures overlay of
 * OICN Architecture with Current Internet Architecture. It has APIs
 * that help in distinguishing OICN Packets from regular Packets. It
 * also has APIs that construct an OICN Packet and append the
 * OICN Sublayer to the Packet, whenever data is retrieved from the
 * in the OICN Router.
 *
 * This class is instantiated and aggregated to all OICN Routers in
 * the network, and is not required in any other components of OICN
 * Architecture.
 *
 * Note: The word Cache here refers to the Named Content Cache of the
 *       ICN Router.
 */

class SublayerProtocol : public Object
{
public:

	SublayerProtocol();
	virtual ~SublayerProtocol();

	/**
	 * \brief Get the type ID.
	 * \return the object TypeId
	 */
	static TypeId GetTypeId (void);

	/**
	 * \brief Set node associated with this stack.
	 * \param node node to set
	 */
	void SetNode(Ptr<Node> n);

	/**
	 * \brief Get node associated with this stack.
	 * \return node of the stack
	 */
	Ptr<Node> GetNode(void);

	/**
	 * \brief Check for the existence of OICN Sublayer in the packet, i.e.
	 *        check whether the parameter packet is OICN Packet or not. This
	 *        API is called by the Ipv4RouterL3Protocol class whenever it
	 *        receives a packet.
	 * \param packet packet received by OICN Router
	 * \param ipheader IP Header of the packet
	 * \return the packet after processing
	 */
	Ptr<Packet> OICNSublayerCheck (Ptr<Packet> packet, Ipv4Header ipheader);

	/**
	 * \brief Check whether OICN Sublayer exists in the IP packet
	 * \param packet packet received by OICN Router
	 * \param ipheader IP Header of the packet
	 * \return true if OICN Sublayer exists
	 */
	bool HasSublayer(Ptr<Packet> packet, Ipv4Header ipheader);

	/**
	 * \brief Check whether the Packet is UDP or not
	 * \param ipheader IP Header of the packet
	 * \return true if the Transport Layer used is UDP
	 */
	bool isUdp (Ipv4Header ipheader);

	/**
	 * \brief Cache the Application Layer (data) of the Packet after it has been recognized
	 *        as an OICN Packet. If the Cache is not full, the data is automatically cached.
	 *        If the Cache is full, the index (proirity) of the data will determine if
	 *        some other Cache entry has to be evicted or not.
	 * \param packet1 packet received by OICN Router with its Transport Header removed
	 * \param name name of the corresponding data (or content) of the packet, stored in
	 *        OICN Header of the packet.
	 * \param ipheader ip header of the packet
	 * \return true if the data has been cached
	 */
	bool CheckCache (Ptr<Packet> packet1, std::string name, Ipv4Header ipheader);


	/**
	 * \brief Construct an OICN Packet for the data retrieved from Cache. This API is
	 *        required when ICN Manager requests for data cached in an OICN Router to
	 *        be sent to an OICN Client querying for that content.
	 * \param packet packet consisting of only the Transport Layer Data. This packet
	 *        is created after the queried content is searched and found in the Cache
	 *        by the Cache Manager.
	 * \param Name name of the data retrieved from the Cache.
	 * \param DestinationAddress IP address of the OICN Client querying for that content
	 * \param PortNumber Port Number of the OICN Client querying for that content
	 */
	void ConstructPacket (Ptr<Packet> packet, std::string Name, uint32_t DestinationAddress, uint32_t PortNumber);

	/**
	 * \brief Construct the OICN Sublayer for the OICN Packet created in ConstructPacket API,
	 *        by appending the OICN Header.
	 * \param packet OICN Packet to which OICN Header has to be appended.
	 * \param Name name of the content in the OICN Packet.
	 */
	void ConstructSublayer (Ptr<Packet> packet, std::string Name);

	/**
	 * \brief Store the UDP header of the received packet as a Private Variable
	 *        'udp_header' in the SublayerProtocol Object.
	 * \param header UDP header removed from the received packet
	 */
	void SetUdpHeader (UdpHeader header);

	/**
	 * \brief Store the OICN header of the received packet as a Private Variable
	 *        'oicn_header' in the SublayerProtocol Object.
	 * \param header OICN header removed from the received packet
	 */
	void SetOICNHeader (OicnHeader header);

	/**
	 * \brief Set the cachability of the content
	 * \param non_cachable indicating whether content is cachable or not
	 */
	void SetCachable(bool non_cachable);

	/**
	 * \brief Get the UDP header stored in udp_header Private Variable
	 * \return UdpHeader UDP header stored in udp_header Private Variable
	 */
	UdpHeader GetUdpHeader ();

	/**
	 * \brief Get the OICN header stored in oicn_header Private Variable
	 * \return OICNHeader OICN header stored in oicn_header Private Variable
	 */
	OicnHeader GetOICNHeader ();


	
private:
	
	/**
	 * \brief node pointer of current stack
	 */
	Ptr<Node> m_node;
	/**
	 * \brief UDP header of the received packet
	 */
	UdpHeader udp_header;
	/**
	 * \brief OICN header of the received packet
	 */
	OicnHeader oicn_header;

	/**
	 * \brief socket pointer for the process of sending data queried by OICN Client
	 */
	Ptr<Socket> m_socket;

	/**
	 * \brief indicates whether content is cachable or not
	 */
	bool m_set_non_cachable;
};

}
#endif
