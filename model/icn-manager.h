/* -*- Mode:C++; c-file-style; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2016, CTO Networks Lab, Tata Consultancy Services (TCS)
 *
 * Author: Suvrat Agrawal <suv.agr@gmail.com>
 */

#ifndef ICN_MANAGER_H
#define ICN_MANAGER_H
#include "ns3/type-name.h"
#include "ns3/application.h"
#include <c++/4.8/unordered_map>
#include <stdint.h>
#include <iostream>
#include <string>
#include <vector>
#include <utility>
#include <boost/unordered_map.hpp>
#include "oicn-server-application.h"
#include "ns3/node.h"
#include "ns3/ptr.h"
#include "ns3/type-id.h"
#include "oicn-header.h"
#include "dnsplus-question-header.h"
#include "dnsplus-answer-section.h"
#include "ns3/ipv4-address.h"
#include <stdint.h>
#include "dnsplus-header.h"


namespace ns3{


template< class MapType >
void print_map(const MapType & m)
{
	typedef typename MapType::const_iterator const_iterator;
	for( const_iterator iter = m.begin(), iend = m.end(); iter != iend; ++iter )
	{
		std::cout <<"   "<<iter->first << "-->" << iter->second << std::endl;
	}
}


/**
 * \brief Container to store mapping of name and corresponding ip address of the server.
 *It is used at ICN Manager to find ipv4 address of server at which the requested name is stored.
 **/

typedef boost::unordered_multimap<std::string, Ipv4Address> NameServerTable;

/**
 * \brief container to store mapping of name and corresponding ip address of the cached router.
 *Content is cached at the router. It stores the name of the content and ip address of router where
 *the content was cached.
 *
 * */
typedef boost::unordered_multimap<std::string, Ipv4Address> CachedRouterTable;

/**
 * \brief Type Definition for the constant iterater of the CachedRouterTable
 */
typedef CachedRouterTable::const_iterator crIterator;

/**
 * \brief Type Definition for the constant iterater of the CachedRouterTable
 */
typedef NameServerTable::const_iterator nsiterator;


/**
 *
 * \brief It resolves the name requested by the client and direct the source (server or cached source(router))
 *  to send the requested content back to client
 *
 * */


class IcnManager : public Application
{
public:
	IcnManager();
	/**
	 * \brief Get the type ID.
	 * \return the object TypeId
	 */
	static TypeId GetTypeId (void);

	/**
	 * \brief Enter the name of the content and ipv4 address of corresponding router into CachedRouterTable
	 * \param routerIpv4Address ipv4 address of router at which content is store
	 * \param name name of the content
	 */
	static void SetCachedRouterEntry (Ipv4Address routerIpv4Address, std::string name);

	/**
	 * \brief Evict the name of the content and ipv4 address of corresponding router from CachedRouterTable
	 * \param routerIpv4Address ipv4 address of router at which content is store
	 * \param name name of the content
	 */
	static void EvictCachedRouterEntry (Ipv4Address routerIpv4Address, std::string name);

	/**
	 * \brief Read the packet received.
	 *  If packet received is content query packet from OICN Client, it call NameResolution function to know the source of the content.
	 *  Further if name resolution is successful, it sends ACK to client, otherwise it sends NACK to client
	 * \param packet packet received
	 * \param clientAddress ipv4 address of the client
	 * \param clientPort port of the client
	 */
	void ReadPacket(Ptr<Packet> packet, Ipv4Address clientAddress, uint16_t clientPort);

	/**
	 * \brief Search the name in CachedRouterTable or nameServerTable
	 * \param name content name to be searched
	 * \param flag true value represent content found and false value represent content not found
	 * \returns ipv4 address of the source of the content
	 *
	 */
	Ipv4Address const * NameResolution (std::string name, bool &flag);

	/**
	 * \brief Initialise nameServerTable with name and corresponding server ipv4 address.
	 * \param nameIndex it is index of content that is stored at particular server
	 * \param serverAddress address of the server containing the content (here content is identified by name. Name are defined on the basis of indices of nameIndex)
	 */
	void NameRegisteration(std::vector<std::vector<int>> nameIndex, std::vector<Ipv4Address> serverAddress);

	/**
	 * \brief Send ACK/NACK to client depending upon ICN Manager has able to resolve name or not
	 * \param clientIpv4Address client ipv4 address to which ACK has to be sent
	 * \param clientPort client port to which ACK has to be sent
	 * \param dnsPlusQuestionHeader dnsPlus Question header section of the packet
	 * \param dnsPlusHeader dnsPlus header section of the packet
	 * \param oicnHeader oicn header section of the packet
	 * \param flag if true shows ACK has to be sent otherwise NACK
	 */
	void SendAckToClient(Ipv4Address  clientIpv4Address, uint16_t clientPort, DnsPlusQuestionHeader dnsPlusQuestionHeader, DnsPlusHeader dnsPlusHeader,OicnHeader oicnHeader,bool &flag);

	/**
	 * \brief Send Reply packet (with dnsPlus Answer section) to source of content.
	 *  dnsPlus Answer section contains client ipv4 address.
	 *  It directs the source of content to send the content at mentioned client address.
	 * \param sourceIpAddress ipv4 address of source to which packet has to be sent
	 * \param clientIpv4Address ipv4 address of source to which packet has to be sent
	 * \param dnsPlusQuestionHeader dnsPlus Question header section of the packet
	 * \param dnsPlusHeader dnsPlus header section of the packet
	 * \param oicnHeader oicn header section of the packet
	 */
	void SendToSource(Ipv4Address sourceIpAddress, Ipv4Address clientIpv4Address,DnsPlusQuestionHeader dnsPlusQuestionHeader,DnsPlusHeader dnsPlusHeader,OicnHeader oicnHeader);

protected:

	virtual void DoDispose (void);

private:

	/**
	 * \brief Handle a packet reception from client port (26).
	 * This function is called by lower layers.
	 * \param socket the socket the packet was received to
	 */
	void HandleReadClient (Ptr<Socket> socket);

	/**
	 * \brief Handle a packet reception from source port (both server source and cached source(icn router) port 89).
	 * This function is called by lower layers.
	 * \param socket the socket the packet was received to
	 */

	void HandleReadSource (Ptr<Socket> socket);

	uint32_t m_count; //!< maximum number of packets the application will send
	Time m_interval; //!< packet inter-send time
	uint32_t m_size; //!< size of the sent packet

	virtual void StartApplication (void);
	virtual void StopApplication (void);
	Ptr<Socket> m_socketClient;//!<socket connected to client port 26
	Ptr<Socket> m_socketSource;//!<socket connected to source port 89
	uint16_t m_port; //!<port at which client will listen to the request, here port defined as 36
	uint16_t m_sourcePort;//!<port at which source (server or cached router) will listen to the request, here source port defined as 89
	uint16_t m_clientPort;//!<port at which client will listen to the request, here client port defined as 26

	/**
	 * \brief Container to store mapping of name and corresponding ip address of the server.
	 */
	static NameServerTable nameServerTable;
	/**
	 * \brief Container to store mapping of name and corresponding ip address of the cached router.
	 */
	static CachedRouterTable cachedRouterTable;
};


}
#endif


