/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2016 TCS (Tata Consultancy Services) Research
 *
 * Author: Anirudh Morabagal <anirudhmk.2010@gmail.com>
 *
 */
#include "ns3/type-id.h"
#include "ns3/type-name.h"
#include "named-content-cache.h"
#include "ns3/ptr.h"
#include "ns3/packet.h"
#include "named-content-cache-entry.h"
#include <stdint.h>
#include <string>
#include <unordered_map>
#include <boost/bimap.hpp>
#include <boost/bimap/bimap.hpp>
#include <boost/bimap/set_of.hpp>
#include <boost/bimap/detail/bimap_core.hpp>
#include <boost/bimap/unordered_set_of.hpp>
namespace ns3{

NS_OBJECT_ENSURE_REGISTERED (NamedContentCache);

TypeId NamedContentCache::GetTypeId (void)
{

  static TypeId tid = TypeId ("ns3::NamedContentCache")
    .SetParent<Object> ();


  return tid;

}

NamedContentCache::NamedContentCache()
{

}

NamedContentCache::~NamedContentCache()
{

}

} // end of namespace ns3
