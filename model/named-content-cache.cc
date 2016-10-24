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
