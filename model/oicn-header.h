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

#ifndef OICN_HEADER_H
#define OICN_HEADER_H

#include <stdint.h>
#include <string>
#include "ns3/header.h"
#include "ns3/type-name.h"

namespace ns3{

/**
 * \brief Class for Creating and Appending OICN Sublayer to a Packet
 *
 *                   |--1bit--|---3 bits---|----------28 bits----------|variable length|
 * ...Transport Layer|flag bit|reserve bits|OICN Sublayer Header Length|OICN Chunk Name|Application Layer....
 *
 * The OICN Sublayer is embedded between the Transport Layer and Application Layer. The header consists of a 
 * flag bit followed by 3 Reserved Bits, Header Length and ICN Chunk Name. The flag bit just indicates whether 
 * the OICN Packet is request packet or data packet. The ICN Chunk Name is the name of the Packet data being 
 * queried, when the OICN Packet is a request, or the name of the data/content itself, when the OICN Packet is 
 * a data packet. The header length is 28 bits long, which indicates that the chunk name can a have a maximum 
 * of (2^20)-32 ASCII Characters. This indicates that unlike other typical headers, the OICN header has variable 
 * header length.
 *
 * Among the 3 Reserve bits, one bit is currently used to indicate whether the packet data can be cached in the 
 * present ICN Router or not. The ICN Router or OICN Server can change this cachable bit to ensure the packet 
 * data is not cached further down the network. Recently, we found that this feature actually creates an obstacle 
 * for the caching capability of the network. So we will gradually remove this feature in the near future.
 */


class OicnHeader : public Header
{
public:
/**
  * \brief Constructor
  *
  * Creates a null header
  */
  OicnHeader ();
  ~OicnHeader ();
  /**
  * \brief Get the type ID.
  * \return the object TypeId
  */
  static TypeId GetTypeId (void);
  virtual TypeId GetInstanceTypeId (void) const;
  virtual uint32_t GetSerializedSize (void) const;
  virtual void Serialize (Buffer::Iterator start) const;
  virtual uint32_t Deserialize (Buffer::Iterator start);
  virtual void Print (std::ostream &os) const;
  /**
  * \brief Set the Chunk Name of the data contained in the OICN Packet, or the data 
  *        being queried.
  * \param name The Chunk Name in std::string format
  */
  void SetName(std::string name);
   /**
  * \brief Get the Chunk Name of the data contained in the OICN Packet, or the data 
  *        being queried.
  * \return The Chunk Name in std::string format
  */
  std::string GetName(void);
   /**
  * \brief Set the flag bit to request, indicating a request OICN Packet
  */
  void SetRequest(void);
   /**
  * \brief Set the reserve bits to cachable, indicating the data in the OICN Packet can
  *        be cached in the incoming nodes of ICN Routers.
  */
  void SetCachable(void);
   /**
  * \brief Set the reserve bits to non-cachable, indicating the data in the OICN Packet 
  *        cannot be cached in the incoming nodes of ICN Routers.
  */
  void SetNonCachable(void);
   /**
  * \brief Get the first fours bytes of the OICN Header. The first four bytes includes the 
  *        flag bit, reserve bits and header length.
  * \return The first four bytes of the OICN Header in unsigned integer format
  */
  uint32_t GetFirst4Bytes (void);

  private:
/**
  * \brief Set the first fours bytes of the OICN Header. The first four bytes includes the 
  *        flag bit, reserve bits and header length. Since the first four bytes will be in
  *        unsigned integer form, some adjustments have to be made to ensure the correct 
  *        first four bytes after it is serialized. Integer constants like cachable_start,
  *        non_cachable_start and request_start, depending on whether the OICN Packet is a
  *        request or a data packet, have to added to the integer header length. This results
  *        in the correct conversion from integer to bits during serialization. This has to 
  *        be done because the chunk name in the OICN Header is variable, resulting in variable
  *        header length. In other typical headers, the fixed header length is just serialized.
  *        This function is private because other classes should set the first four bytes only 
  *        through the SetRequest, SetCachable and SetNonCachable functions. Additional functions
  *        can be added in the future to set the first four bytes differently from what is 
  *        already being provided.
  * \param HeaderLength the header length of the OICN Header
  * \param reply the flag bit indicating if the OICN Packet is request or reply, zero if request
  *        and one if reply
  * \param cachable indicates whether the data in the packet can be cached in incoming ICN Router
  *        nodes or not.
  */
  void SetFirst4Bytes(uint32_t HeaderLength, bool reply, bool cachable);

  std::string m_name;//!< Chunk Name of the data present in the OICN Packet or being queried
  uint32_t header_length;//!< Header Length of the OICN Header
  uint32_t first4bytes;//!< The first four bytes of the OICN Header in unsigned integer format

  static const uint32_t cachable_start = 4026531840;    //11110000000000000000000000000000
  static const uint32_t non_cachable_start = 2952790016;//10110000000000000000000000000000
  static const uint32_t request_start = 805306368;     //00110000000000000000000000000000

};
}
#endif
