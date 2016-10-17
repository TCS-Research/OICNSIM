/* -*- Mode:C++; c-file-style; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2015, CTO Networks Lab, Tata Consultancy Services (TCS)
 *
 * Author: Anirudh Morabagal <anirudhmk.2010@gmail.com>
 */

#ifndef NAMED_CONTENT_CACHE_ENTRY_H
#define NAMED_CONTENT_CACHE_ENTRY_H

#include "ns3/type-name.h"
#include "ns3/type-id.h"
#include <stdint.h>
#include <string>

namespace ns3{

/**
 * \brief Entry class for NamedContentCache container of ICN Router
 *
 * This class represents the entry of the cache of an ICN Router. It
 * is basically a struct like class that stores all the available
 * parameters associated with the data packet, like timestamp, source
 * IP, packet size, frequency, number of hops undertaken to reach the
 * ICN Router (in the form of TTL) etc. These parameters will be
 * required to assign a priority for the data packet based on the
 * caching policy used. This class has an empty constructor, so that
 * the user can assign and use values to only to the required parameters.
 * Since this is a structure like class with functions to assign values
 * to the members, a type ID is not required.
 */


class NamedContentCacheEntry

{
  private:
 /**
  * \brief The packet data stored a an std::string
  */
  std::string m_data;
  /**
   * \brief The TTL value obtained from IP header, stored as unsigned char
   */
  uint8_t m_ttl;
  /**
   * \brief The timestamp, generally obtained from Simulator class
   */
  uint64_t m_timestamp;
  /**
   * \brief The frequency of the data being retrieved from the cache to fulfill
   *        a query of the client
   */
  uint32_t m_frequency;
  /**
    * \brief The IP address of the server (or another ICN Router), from which the
    *        packet was received at the ICN Router. Can be retrieved from IP header.
    */
  uint32_t m_sourceip;
  /**
   * \brief The size of the data packet
   */
  uint32_t m_packetsize;
  /**
    * \brief The packet ID of the received data packet, only relevant inside ns3
    */
  uint32_t m_id;
  
  public:
   
  NamedContentCacheEntry();
  ~NamedContentCacheEntry();
  /**
    * \brief Set the value of m_data member of this class
    * \param the packet data in the form of std::string
    */
  void SetData (std::string data);

  /**
    * \brief Get the value of m_data member of this class
    * \return the packet data in the form of std::string
    */
  std::string GetData ();

  /**
    * \brief Set the value of m_ttl member of this class
    * \param the TTL of the packet when it was received at the current ICN Router
    */
  void SetTTL(uint8_t ttl);

  /**
   * \brief Get the value of m_ttl member of this class
   * \return the TTL of the packet data
   */
  uint8_t GetTTL();

  /**
    * \brief Set the value of m_timestamp member of this class
    * \param the time at which the packet was received at the current ICN Router
    */
  void SetTimestamp(uint64_t timestamp);

  /**
    * \brief Get the value of m_timestamp member of this class
    * \return the timestamp of the packet
    */
  uint64_t GetTimestamp();

  /**
   * \brief Set the value of m_frequency member of this class
   * \param the number of times the packet data has been retrieved from the cache.
   *        Always assigned the value 1 when the entry object for a particular packet
   *        is created, i.e. constructor initializes this value to 1.
   */
  void SetFrequency(uint32_t frequency);

  /**
   * \brief Get the value of m_frequency member of this class
   * \return the frequency of retrieval of packet data from the cache
   */
  uint32_t GetFrequency();

  /**
   * \brief Set the value of m_sourceip member of this class
   * \param the Source IP address in integer form
   */
  void SetSourceIP(uint32_t sourceip);

  /**
   * \brief Get the value of m_sourceip member of the class
   * \return the source IP of the packet in integer form
   */
  uint32_t GetSourceIP();

  /**
    * \brief Set the value of m_packetsize member of this class
    * \param the size of the packet data
    */
    void SetPacketSize(uint32_t size);

   /**
    * \brief Get the value of m_packetsize member of the class
    * \return the size of the packet data
    */
    uint32_t GetPacketSize();

   /**
    * \brief Set the value of m_id member of this class
    * \param the integer ID of the packet, set by ns3 while it was created
    */
    void SetPacketID(uint32_t uid);

   /**
    * \brief Get the value of m_id member of the class
    * \return the integer ID of the packet
    */
    uint32_t GetPacketID();

};
}
#endif
