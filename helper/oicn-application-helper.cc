/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2016 TCS (Tata Consultancy Services) Research
 *
 * Authors: Suvrat Agrawal <suv.agr@tcs.com>, Anirudh Morabagal <anirudhmk.2010@gmail.com>
 *
 */
#include "ns3/icn-router.h"
#include "ns3/type-id.h"
#include "oicn-application-helper.h"
#include "ns3/log.h"
#include "ns3/node.h"
#include "ns3/uinteger.h"
#include "ns3/oicn-client-application.h"
#include "ns3/oicn-zipf-client.h"
#include "ns3/oicn-server-application.h"
#include "ns3/icn-manager.h"
#include "ns3/content.h"
#include <vector>

NS_LOG_COMPONENT_DEFINE ("OICNAppHelper");

namespace ns3 {


OicnServerHelper::OicnServerHelper ()
{
  m_factory.SetTypeId (OicnServer::GetTypeId ());
}

ApplicationContainer
OicnServerHelper::Install (Ptr<Node> node, std::vector<int> index) const
{
  return ApplicationContainer (InstallPriv (node, index));
}


ApplicationContainer
OicnServerHelper::Install (NodeContainer c, std::vector<int> index) const
{
  ApplicationContainer apps;
  for (NodeContainer::Iterator i = c.Begin (); i != c.End (); ++i)
    {
      apps.Add (InstallPriv (*i, index));
    }

  return apps;
}


ApplicationContainer
OicnServerHelper::InstallPriv (Ptr<Node> node, std::vector<int> index) const
{
  ApplicationContainer apps;
  Ptr<OicnServer> server = m_factory.Create<OicnServer> ();
  server->SetContent(index);
  node->AddApplication (server);

  apps.Add(server);
  return apps;
}

/**
 * IcnManagerHelper
 * */

IcnManagerHelper::IcnManagerHelper()
{
  m_factory.SetTypeId (IcnManager::GetTypeId ());
}

ApplicationContainer IcnManagerHelper::Install (Ptr<Node> node, std::vector<std::vector<int>> nameIndex, std::vector<Ipv4Address> serverAddress ) const
{
	return ApplicationContainer (InstallPriv (node, nameIndex, serverAddress));
}

ApplicationContainer IcnManagerHelper:: InstallPriv (Ptr<Node> node, std::vector<std::vector<int>> nameIndex, std::vector<Ipv4Address> serverAddress ) const
{
	  ApplicationContainer apps;
	  Ptr<IcnManager> IcnManagerPtr = m_factory.Create<IcnManager> ();
	  IcnManagerPtr->NameRegisteration( nameIndex, serverAddress);
	  node->AddApplication (IcnManagerPtr);
	  apps.Add(IcnManagerPtr);
	  return apps;
}

/**
 * OicnClientHelpher
 *
 * */


OicnClientHelpher::OicnClientHelpher ()
{
	  m_factory.SetTypeId (OicnClient::GetTypeId ());
}

OicnClientHelpher::OicnClientHelpher (Ipv4Address address, uint16_t port)
{
  m_factory.SetTypeId (OicnClient::GetTypeId ());
  SetAttribute ("IcnManagerAddress", AddressValue (Address(address)));
  SetAttribute ("IcnManagerPort", UintegerValue (port));
}

OicnClientHelpher::OicnClientHelpher (Address address, uint16_t port)
{
  m_factory.SetTypeId (OicnClient::GetTypeId ());
  SetAttribute ("IcnManagerAddress", AddressValue (address));
  SetAttribute ("IcnManagerPort", UintegerValue (port));
}


void
OicnClientHelpher::SetAttribute (
  std::string name,
  const AttributeValue &value)
{
  m_factory.Set (name, value);
}

ApplicationContainer
OicnClientHelpher::Install (Ptr<Node> node, std::vector<int> index) const
{
  return ApplicationContainer (InstallPriv (node, index));
}

ApplicationContainer
OicnClientHelpher::Install (Ptr<Node> node, std::pair<int, int> range) const
{
  return ApplicationContainer (InstallPriv (node, range));
}

ApplicationContainer
OicnClientHelpher::Install (NodeContainer c, std::vector<int> index) const
{
  ApplicationContainer apps;
  for (NodeContainer::Iterator i = c.Begin (); i != c.End (); ++i)
    {
      apps.Add(InstallPriv (*i, index));
    }

  return apps;
}

ApplicationContainer
OicnClientHelpher::Install (NodeContainer c, std::pair<int, int> range) const
{
  ApplicationContainer apps;
  for (NodeContainer::Iterator i = c.Begin (); i != c.End (); ++i)
    {
      apps.Add(InstallPriv (*i, range));
    }

  return apps;
}

ApplicationContainer OicnClientHelpher::InstallPriv (Ptr<Node> node, std::vector<int> index) const
{
  ApplicationContainer apps;
  Ptr<OicnClient> client = m_factory.Create<OicnClient> ();
  client->SetContentIndex(index);
  node->AddApplication (client);
  apps.Add(client);

  return apps;
}

ApplicationContainer OicnClientHelpher::InstallPriv (Ptr<Node> node, std::pair<int, int> range) const
{
  ApplicationContainer apps;
  Ptr<OICNZipfClient> client = Create<OICNZipfClient> ();
  client->SetZipf(range);
  node->AddApplication (client);
  apps.Add(client);
  return apps;
}



/*
 *OicnZipfClientHelpe
 *
 */
OicnZipfClientHelper::OicnZipfClientHelper(Address address, uint16_t port)
{
  m_factory.SetTypeId (OICNZipfClient::GetTypeId ());
  SetAttribute ("IcnManagerAddress", AddressValue (address));
  SetAttribute ("IcnManagerPort", UintegerValue (port));
}

OicnZipfClientHelper::OicnZipfClientHelper(Ipv4Address address, uint16_t port)
{
  m_factory.SetTypeId (OICNZipfClient::GetTypeId ());
  SetAttribute ("IcnManagerAddress", AddressValue (address));
  SetAttribute ("IcnManagerPort", UintegerValue (port));
}


void
OicnZipfClientHelper::SetAttribute (
  std::string name,
  const AttributeValue &value)
{
  m_factory.Set (name, value);
}

ApplicationContainer
OicnZipfClientHelper::Install (Ptr<Node> node, std::pair<int, int> range) const
{
	return ApplicationContainer (InstallPriv (node, range));
}


ApplicationContainer
OicnZipfClientHelper::Install (NodeContainer c, std::pair<int, int> range) const
{

  ApplicationContainer apps;
  for (NodeContainer::Iterator i = c.Begin (); i != c.End (); ++i)
    {
      apps.Add (InstallPriv (*i,range));
    }

  return apps;
}


ApplicationContainer
OicnZipfClientHelper::InstallPriv (Ptr<Node> node,  std::pair<int, int> range) const
{
  ApplicationContainer apps;
  Ptr<OICNZipfClient> zipfClient = m_factory.Create<OICNZipfClient> ();
  zipfClient->SetZipf(range);
  node->AddApplication (zipfClient);
  apps.Add(zipfClient);
  return apps;
}
/*
*ICN Router
*/
IcnRouterHelpher::IcnRouterHelpher()
{
  m_factory.SetTypeId (IcnRouter::GetTypeId ());
}


void
IcnRouterHelpher::SetAttribute (
  std::string name,
  const AttributeValue &value)
{
    m_factory.Set (name, value);
}

ApplicationContainer
IcnRouterHelpher::Install (Ptr<Node> node) const
{
	return ApplicationContainer (InstallPriv (node));
}


ApplicationContainer
IcnRouterHelpher::Install (NodeContainer c) const
{
  ApplicationContainer apps;
  for (NodeContainer::Iterator i = c.Begin (); i != c.End (); ++i)
    {
      apps.Add (InstallPriv (*i));
    }
  return apps;
}


ApplicationContainer
IcnRouterHelpher::InstallPriv (Ptr<Node> node) const
{
  ApplicationContainer apps;
  Ptr<IcnRouter> icnRouter = m_factory.Create<IcnRouter> ();
  node->AddApplication (icnRouter);
  apps.Add(icnRouter);
  return apps;
}

} // namespace ns3

