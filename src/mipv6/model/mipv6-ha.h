/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2017 Jadavpur University, India
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
 * Author: Manoj Kumar Rana <manoj24.rana@gmail.com>
 */

#ifndef MIPV6_HA_H
#define MIPV6_HA_H

#include "mipv6-agent.h"
#include "bcache.h"
#include "mipv6-header.h"

namespace ns3 {
class Packet;

class Mipv6Ha : public Mipv6Agent
{
public:
  /**
   * \brief The interface ID.
   * \return type ID
   */
  static TypeId GetTypeId (void);
  /**
   * \brief Constructor.
   */
  Mipv6Ha ();
  /**
   * \brief Destructor.
   */
  virtual ~Mipv6Ha ();

  /**
   * \brief all the home link  addresses are returned.
   * \returns home address list 
   */
  std::list<Ipv6Address> HomeAgentAddressList ();

  /**
   * \brief perform DAD on behalf of MN for its HoA in home network.
   * \param target target address
   * \param interface interface in which HoA is defined 
   */
  void DoDADForOffLinkAddress (Ipv6Address target, Ptr<Ipv6Interface> interface);

  /**
   * \brief performing binding process after DAD completion.
   * \param interface interface at which BU is received
   * \param ba formed BA
   * \param homeaddr home address of the MN
   */
  void FunctionDadTimeoutForOffLinkAddress (Ptr<Ipv6Interface> interface, Ptr<Packet> ba, Ipv6Address homeaddr);

  /**
   * \brief Indication from the ICMPv6L4Protocol, if any corresponding NA is received and DAD fails.
   * \param addr the target address
   */
  void DADFailureIndication (Ipv6Address addr);

  /**
   * \brief lookup for home address of MN
   * \param addr home address
   * \returns status
   */
  bool IsAddress (Ipv6Address addr);

  /**
   * \brief lookup for solicited home address of MN
   * \param addr home address
   * \returns status
   */
  bool IsAddress2 (Ipv6Address addr);

  /**
   * \brief handle NS during DAD
   * \param packet the NS packet
   * \param interface where received
   * \param src the source address
   * \param target the target address
   */
  void HandleNS (Ptr<Packet> packet, Ptr<Ipv6Interface> interface, Ipv6Address src, Ipv6Address target);

  /**
   * TracedCallback signature for BU reception event.
   *
   * \param [in] packet The bu packet.
   * \param [in] src The source address
   * \param [in] dst The destination address
   * \param [in] interface the interface in which the bu received
   */
  typedef void (* RxBuTracedCallback)
    (Ptr<Packet> packet, Ipv6Address src, Ipv6Address dst, Ptr<Ipv6Interface> interface);

protected:
  virtual void NotifyNewAggregate ();

  /**
   * \brief build BA in response of BU
   * \param bu the BU packet
   * \param hoa the home address
   * \param status the staus of BU reception
   * \return a ba packet
   */
  Ptr<Packet> BuildBA (Ipv6MobilityBindingUpdateHeader bu, Ipv6Address hoa, uint8_t status);

  /**
   * \brief handle BU
   * \param packet the BU packet
   * \param src the source address
   * \param dst the destination address
   * \param interface the interface at which BU is received
   * \return status
   */
  virtual uint8_t HandleBU (Ptr<Packet> packet, const Ipv6Address &src, const Ipv6Address &dst, Ptr<Ipv6Interface> interface);

  /**
   * \brief setup tunnel for a bcache entry
   * \param bce BCache entry
   * \return status whether tunnel is set up or not
   */
  bool SetupTunnelAndRouting (BCache::Entry *bce);

  /**
   * \brief clear tunnel for a bcache entry
   * \param bce BCache entry
   * \return status whether tunnel is cleared or not
   */
  bool ClearTunnelAndRouting (BCache::Entry *bce);


private:
  /**
   * \brief the binding cache associated with this HA.
   */
  Ptr<BCache> m_bCache;

  /**
   * \brief Callback to trace RX (reception) bu packets.
   */ 
  TracedCallback<Ptr<Packet>, Ipv6Address, Ipv6Address, Ptr<Ipv6Interface> > m_rxbuTrace;
};

} /* namespace ns3 */

#endif /* MIPV6_HA_H */

