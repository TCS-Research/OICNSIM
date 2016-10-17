/* -*- Mode:C++; c-file-style; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2016, CTO Networks Lab, Tata Consultancy Services (TCS)
 *
 * Authors: Suvrat Agrawal <suv.agr@gmail.com>, Anirudh Morabagal <anirudhmk.2010@gmail.com>
 */
#ifndef OICN_ZIPF_CLIENT_H
#define OICN_ZIPF_CLIENT_H

#include "ns3/application.h"
#include "ns3/ipv4-address.h"
#include <vector>
#include <stdint.h>
#include "ns3/random-variable-stream.h"
#include "ns3/event-id.h"
#include "ns3/type-name.h"
#include "ns3/socket.h"
#include "ns3/ptr.h"
#include "ns3/packet.h"
#include "ns3/type-id.h"
#include "ns3/node.h"

namespace ns3{
/**
 * \brief OICN Zipf Client application, it generates request for the contents following Zipf-Mandelbrot Distribution.
 * Here is the explanation of Zipf-Mandelbrot Distribution:
 * http://en.wikipedia.org/wiki/Zipf%E2%80%93Mandelbrot_law.
 * Client application send name resolution to ICN Manager.
 * ICN Manager do the name resolution and direct source (ICN router or server) to send the content to the OICN Zipf Client.
 * */

class OICNZipfClient : public Application
{
public:
	/**
	 * \brief Get the type ID.
	 * \return the object TypeId
	 */
	static TypeId GetTypeId (void);
	OICNZipfClient (void);
	virtual ~OICNZipfClient (void);
	/**
	 * \brief Set the range of the index of content
	 * \param range range of the index of content
	 */
	void SetZipf (std::pair<int, int> range);

	/**
	 * \brief Construct the sublayer header
	 * \param packet packet constructed by application layer
	 * \param name name of the content
	 * \return packet constructed after adding sublayer
	 */
	Ptr<Packet> ConstructSublayer (Ptr<Packet> packet, std::string Name);

	/**
	 * \brief Construct the DNS Plus header
	 * \param packet packet being constructed by application layer
	 * \return packet constructed after adding DNS Plus header
	 */
	Ptr<Packet> ConstructDnsPlusHeader (Ptr<Packet> packet);

	/**
	 * \brief Construct the DNS Plus Question Section
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
	 * \brief Generate the indices of content to be requested following Zipf-Mandelbrot Distribution
	 * \return index of the content next to be request
	 */
	int GetNextContentIndex ();

	/**
	 * \brief Schedule the next packet transmission
	 * \param dt time interval between packets.
	 */
	void ScheduleTransmit (Time dt);
	/**
	 * \brief Send a packet to ICN Manager
	 */
	void Send ();

	std::pair<uint32_t, uint32_t> m_range;//!< range of the indices of the contents
	uint32_t m_sent;//!< counter for sent packets
	//uint32_t m_recieveAck;//!< counter for recieved ACK
	uint32_t m_count;//!< the maximum number of packets the application will send
	double m_q;                 //!< q in (k+q)^s
	double m_s;                 //!< s in (k+q)^s
	std::vector<double> m_Pcum; //!< cumulative probability
	uint32_t m_N;//!< number of the contents
	uint16_t m_portFixed;//!< client port is attached with port no 26, it will listen to this port from OICN Source(ICN router or server)
    Ptr<Socket> m_socketIcnManager;//!< socket associated with ICN Manager
    Ptr<Socket> m_socketSource;//!< socket associated with Source
    EventId m_sendEvent;//!< event to send the next packet
    Time m_interval;//!< packet inter-send time
    Address m_icnManagerAddress;  //!< ICN Manager Address
    uint16_t m_icnManagerPort; //!< ICN Manager port
};
}
#endif
