/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2016 TCS (Tata Consultancy Services) Research
 *
 * Authors: Suvrat Agrawal <suv.agr@gmail.com>, Anirudh Morabagal <anirudhmk.2010@gmail.com>
 *
 */
#ifndef OICN_APPLICATION_HELPER_H
#define OICN_APPLICATION_HELPER_H

#include <stdint.h>
#include <vector>
#include "ns3/uinteger.h"
#include "ns3/application.h"
#include "ns3/application-container.h"
#include "ns3/node-container.h"
#include "ns3/object-factory.h"
#include "ns3/ipv4-address.h"
#include "ns3/node.h"

namespace ns3 {

/**
 * \brief Create a OICN Server application. ICN Manager direct OICN server to send requested content to OICN client.
 */
class OicnServerHelper
{
public:

	OicnServerHelper ();

	/**
	 * Create an OicnServer Application having content specified by indices on the specified node.
	 *
	 * \param node The node on which to create the Application. The node is
	 *             specified by a Ptr<Node>.
	 * \param index vector containing the indices of the content
	 * \returns an ApplicationContainer holding the Application created
	 */
	ApplicationContainer Install (Ptr<Node> node, std::vector<int> index) const;

	/**
	 * Create an OicnServer Application having content specified by indices on the specified Node.
	 *
	 *\param c The nodes on which to create the Applications.  The nodes
	 *          are specified by a NodeContainer.
	 *\param index vector containing the indices of the content
	 *\returns an ApplicationContainer holding the Application created
	 */
	ApplicationContainer Install (NodeContainer c, std::vector<int> index) const;


private:
	/**
	 * Install an OicnServer on the node
	 * \param node The node on which an OicnServer will be installed.
	 * \param index vector containing the indices of the content
	 * \returns an ApplicationContainer holding the Application created
	 */
	ApplicationContainer InstallPriv (Ptr<Node> node, std::vector<int> index) const;

	ObjectFactory m_factory; //!< Object factory.
};


/**
 * \brief Create an application which resolve the name request (check whether content present at ICN router or server)
 * and forward request to source of content (ICN router or server).
 */
class IcnManagerHelper
{
public:
	IcnManagerHelper();

	/**
	 * \brief Create a ICN Manager on the specified Node.
	 * \param node node on which to create the Application. The node is
	 *             specified by a Ptr<Node>.
	 * \param nameIndex it is the vector of all the name indices that present at corresponding server address specified in parameter serverAddress.
	 * \param serverAddress it is vector of server Ipv4 address containing the content corresponding to name indices specified in parameter nameIndex.
	 * \returns an ApplicationContainer holding the Application created
	 */
	ApplicationContainer
	Install (Ptr<Node> node, std::vector<std::vector<int>> nameIndex, std::vector<Ipv4Address> serverAddress ) const;
private :

	/**
	 * \brief Install ICN Manager on the node
	 * \param node node on which an ICNManager will be installed.
	 * \param nameIndex it is the vector of all the name indices that present at corresponding server address specified in parameter serverAddress.
	 * \param serverAddress it is vector of server Ipv4 address containing the content corresponding to name indices specified in parameter nameIndex.
	 * \returns an ApplicationContainer to the application installed.
	 */
	ApplicationContainer
	InstallPriv (Ptr<Node> node, std::vector<std::vector<int>> NameIndex, std::vector<Ipv4Address> ServerAddress  ) const;

	ObjectFactory m_factory; //!< Object factory.
};

/**
 * \brief Create a OICN Client application which send content name resolution request to ICN Manager. ICN Manager direct the content request to
 * ICN source (OICN server or ICN router) which reply to OICN client with requested content.
 */
class OicnClientHelpher
{
public:
	/**
	 * Create OicnClientHelpher configured with ICN Manager IP address and port number.
	 *
	 * \param ip the IPv4 address of the ICN Manager
	 * \param port the port number of the ICN Manager
	 */
	OicnClientHelpher (Ipv4Address ip, uint16_t port);

	/**
	 * Create OicnClientHelpher configured with ICN Manager IP address and port number.
	 *
	 * \param address the IP address of the ICN Manager
	 * \param port the port number of the ICN Manager
	 */
	OicnClientHelpher (Address address, uint16_t port);

	OicnClientHelpher ();

	/**
	 * Record an attribute to be set in each Application after it is is created.
	 *
	 * \param name the name of the attribute to set
	 * \param value the value of the attribute to set
	 */
	void SetAttribute (std::string name, const AttributeValue &value);

	/**
	 * Create an OicnClient Application querying the content specified by indices on the specified Node.
	 *
	 * \param node the node on which to create the Application. The node is
	 *             specified by a Ptr<Node>
	 * \param index vector containing the indices of the content to be queried
	 * \returns an ApplicationContainer holding the Application created
	 */
	ApplicationContainer Install (Ptr<Node> node, std::vector<int> index) const;

	/**
	 * Create an OicnClient Application querying the content specified by indices on the specified Node.
	 *
	 * \param node the node on which to create the Application. The node is
	 *             specified by a Ptr<Node>
	 * \param range range of the indices of the content to be queried
	 * \returns an ApplicationContainer holding the Application created
	 */
	ApplicationContainer Install (Ptr<Node> node, std::pair<int, int> range) const;

	/**
	 * Create an OicnClient Application querying the content specified by indices on the specified Node.
	 *
	 *\param c the nodes on which to create the Applications. The nodes
	 *          are specified by a NodeContainer
	 * \param index vector containing the indices of the content to be queried
	 * \returns an ApplicationContainer holding the Application created
	 */
	ApplicationContainer Install (NodeContainer c, std::vector<int> index) const;

	/**
	 * Create an OicnClient Application querying the content specified by indices on the specified Node.
	 *
	 *\param c the nodes on which to create the Applications. The nodes
	 *          are specified by a NodeContainer.
	 * \param range range of the indices of the content to be queried
	 * \returns an ApplicationContainer holding the Application created
	 */
	ApplicationContainer Install (NodeContainer c, std::pair<int, int> range) const;

private:
	/**
	 * Install an OICN Client on the node configured with all the
	 * attributes set with SetAttribute.
	 *
	 * \param node the node on which an OICNClient will be installed
	 * \param index vector containing the indices of the content
	 * \returns an ApplicationContainer holding the Application created
	 */
	ApplicationContainer InstallPriv (Ptr<Node> node, std::vector<int> index) const;

	/**
	 * Install an OICN Client on the node configured with all the
	 * attributes set with SetAttribute.
	 *
	 * \param node the node on which an OICNClient will be installed
	 * \param range range of the indices of the content to be queried
	 * \returns an ApplicationContainer holding the Application created
	 */
	ApplicationContainer InstallPriv (Ptr<Node> node, std::pair<int, int> range) const;

	ObjectFactory m_factory; //!< Object factory.
};


/**
 * \brief Create OICN Zipf Client, it generates request for the contents following Zipf-Mandelbrot Distribution.
 * Here is the explanation of Zipf-Mandelbrot Distribution:
 * http://en.wikipedia.org/wiki/Zipf%E2%80%93Mandelbrot_law.
 * Client application send name resolution to ICN Manager.
 * ICN Manager do the name resolution and direct source (ICN router or server) to send the content to the OICN Zipf Client.
 * */


class OicnZipfClientHelper
{
public:
	/**
	 * Create OicnZipfClientHelpher configured with ICN Manager IP address and port number
	 *
	 * \param ip the IP address of the ICN Manager
	 * \param port the port number of the ICN Manager
	 */
	OicnZipfClientHelper (Address ip, uint16_t port);

	/**
	 * Create OicnZipfClientHelpher configured with ICN Manager IP address and port number
	 *
	 * \param ip the IPv4 address of the ICN Manager
	 * \param port the port number of the ICN Manager
	 */
	OicnZipfClientHelper (Ipv4Address ip, uint16_t port);


	/**
	 * Record an attribute to be set in each Application after it is is created.
	 *
	 * \param name the name of the attribute to set
	 * \param value the value of the attribute to set
	 */
	void SetAttribute (std::string name, const AttributeValue &value);


	/**
	 * Create an OicnZipfClient Application querying the content specified by indices on the specified Node.
	 *
	 * \param node the node on which to create the Application. The node is
	 *             specified by a Ptr<Node>.
	 * \param range range of the indices of the content to be queried
	 * \returns an ApplicationContainer holding the Application created
	 */
	ApplicationContainer Install (Ptr<Node> node,std::pair<int, int> range) const;

	/**
	 * Create an OicnZipfClient Application querying the content specified by indices on the specified Node.
	 *
	 * \param nodeName the name of the node on which to create the application
	 * \param range range of the indices of the content to be queried
	 * \returns an ApplicationContainer holding the Application created
	 */
	ApplicationContainer Install (std::string nodeName,std::pair<int, int> range) const;

	/**
	 * Create an OicnZipfClient Application querying the content specified by indices on the specified Node.
	 *
	 *\param c the nodes on which to create the Applications. The nodes
	 *          are specified by a NodeContainer.
	 * \param range range of the indices of the content to be queried
	 * \returns an ApplicationContainer holding the Application created
	 */
	ApplicationContainer Install (NodeContainer c,std::pair<int, int> range) const;

private:

	/**
	 * Install an OICN Zipf Client on the node configured with all the
	 * attributes set with SetAttribute.
	 *
	 * \param node the node on which an OICNClient will be installed.
	 * \param range range of the indices of the content to be queried
	 * \returns an ApplicationContainer holding the Application created
	 */
	ApplicationContainer InstallPriv (Ptr<Node> node,std::pair<int, int> range) const;
	ObjectFactory m_factory; //!< Object factory.
};

/**
 * \brief ICN Router has the capability to cache the contents.
 *
 */

class IcnRouterHelpher
{
public:

	IcnRouterHelpher ();

	/**
	 * Record an attribute to be set in each Application after it is is created.
	 *
	 * \param name the name of the attribute to set
	 * \param value the value of the attribute to set
	 */
	void SetAttribute (std::string name, const AttributeValue &value);

	/**
	 * Create an ICN Router Application
	 *
	 * \param node the node on which to create the Application. The node is
	 *             specified by a Ptr<Node>.
	 * \return an ApplicationContainer holding the Application created
	 */
	ApplicationContainer Install (Ptr<Node> node) const;

	/**
	 * Create an ICN Router Application
	 * \param nodeName the name of the node on which to create the application
	 * \returns an ApplicationContainer holding the Application created
	 */
	ApplicationContainer Install (std::string nodeName) const;

	/**
	 * Create an ICN Router Application
	 *\param c the nodes on which to create the Applications. The nodes
	 *          are specified by a NodeContainer.
	 * \returns an ApplicationContainer holding the Application created
	 */
	ApplicationContainer Install (NodeContainer c) const;

private:

	/**
	 * Install an ICN Router Application on the node configured with all the
	 * attributes set with SetAttribute.
	 * \param node the node on which an OICNClient will be installed.
	 * \returns an ApplicationContainer holding the Application created
	 */
	ApplicationContainer InstallPriv (Ptr<Node> node) const;
	ObjectFactory m_factory; //!< Object factory.
};

}

#endif 
