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

#ifndef MIPV6_MN_H
#define MIPV6_MN_H

#include "mipv6-agent.h"
#include "blist.h"
#include "ns3/traced-callback.h"

namespace ns3 {

class Mipv6Mn : public Mipv6Agent
{
public:
  /**
   * \brief Get the type identifier.
   * \return type identifier
   */
  static TypeId GetTypeId (void);
  /**
   * \brief constructor.
   * \param haalist list of home agent addresses
   */
  Mipv6Mn (std::list<Ipv6Address> haalist);

  virtual ~Mipv6Mn ();

  /**
   * \brief get home BU seq. no.
   * \return seq no.
   */
  uint16_t GetHomeBUSequence ();

  /**
   * \brief get correspondent BU seq. no.
   * \return seq no.
   */
  uint16_t GetCNBUSequence ();

  /**
   * \brief build Home BU
   * \return home BU packet
   */
  Ptr<Packet> BuildHomeBU ();

  /**
   * \brief build CN BU
   * \return CN BU packet
   */
  Ptr<Packet> BuildCNBU ();

  /**
   * \brief build HoTI
   * \return HoTI packet
   */
  Ptr<Packet> BuildHoTI ();

  /**
   * \brief build CoTI
   * \return CoTI packet
   */
  Ptr<Packet> BuildCoTI ();

  /**
   * \brief setup tunnel to transmit packet to CN
   * \return status
   */
  bool SetupTunnelAndRouting ();

  /**
   * \brief clear tunnel
   */
  void ClearTunnelAndRouting ();

  /**
   * \brief start RR test timer
   * \return HoTI packet
   */
  void StartRRTest ();

  /**
   * \brief set route optimization field
   * \param roflag route optimization flag
   */
  void SetRouteOptimizationReuiredField (bool roflag);

  /**
   * \brief Check whether route optimization field is set or, not.
   * \return status
   */
  bool IsRouteOptimizationRequired ();

  /**
   * \brief check an address whether matched with any of its home agent address
   * \param addr an address
   * \return if address becomes successful
   */
  bool IsHomeMatch (Ipv6Address addr);

  /**
   * \brief set the address of connected AR as its default router
   * \param addr address
   * \param index the interface index of its connected default router
   */
  void SetDefaultRouterAddress (Ipv6Address addr, uint32_t index);

  /**
   * \brief check for match
   * \param ha the home agent address
   * \param hoa the home address
   * \return whether these two addresses match
   */
  bool CheckAddresses (Ipv6Address ha, Ipv6Address hoa);

  /**
   * \return HoA
   */
  Ipv6Address GetHomeAddress ();

  /**
   * \return CoA
   */
  Ipv6Address GetCoA ();

  /**
   * TracedCallback signature for BA reception event.
   *
   * \param [in] packet The ba packet.
   * \param [in] src The source address
   * \param [in] dst The destination address
   * \param [in] interface the interface in which the bu received
   */
  typedef void (* RxBaTracedCallback)
    (Ptr<Packet> packet, Ipv6Address src, Ipv6Address dst, Ptr<Ipv6Interface> interface);


  /**
   * TracedCallback signature for BU sent event.
   *
   * \param [in] packet The bu packet.
   * \param [in] src The source address
   * \param [in] dst The destination address
   */
  typedef void (* TxBuTracedCallback)
    (Ptr<Packet> packet, Ipv6Address src, Ipv6Address dst);




protected:
  virtual void NotifyNewAggregate ();

  /**
   * \brief handle attachment with a network, called from ICMPv6L4Protocol
   * \param ipr the CoA currently configured at ICMPv6 layer
   */
  virtual void HandleNewAttachment (Ipv6Address ipr);

  /**
   * \brief Handle recieved BA from HA/CN.
   * \param packet BA packet
   * \param src address of HA/CN
   * \param dst CoA of MN
   * \param interface IPv6 interface which recieves the BA
   * \return status
   */
  virtual uint8_t HandleBA (Ptr<Packet> packet, const Ipv6Address &src, const Ipv6Address &dst, Ptr<Ipv6Interface> interface);

  /**
   * \brief Handle recieved HoT from CN.
   * \param packet HoT packet
   * \param src address of CN
   * \param dst CoA of MN
   * \param interface IPv6 interface which recieves the HoT
   * \return status
   */
  virtual uint8_t HandleHoT (Ptr<Packet> packet, const Ipv6Address &src, const Ipv6Address &dst, Ptr<Ipv6Interface> interface);

  /**
   * \brief Handle recieved CoT from CN.
   * \param packet CoT packet
   * \param src address of CN
   * \param dst CoA of MN
   * \param interface IPv6 interface which recieves the CoT
   * \return status
   */
  virtual uint8_t HandleCoT (Ptr<Packet> packet, const Ipv6Address &src, const Ipv6Address &dst, Ptr<Ipv6Interface> interface);

private:

  /**
   * \brief Binding information list of the MN.
   */
  Ptr<BList> m_buinf;

  /**
   * \brief home binding update sequence no.
   */
  uint16_t m_hsequence;

  /**
   * \brief correspondent binding update sequence no.
   */
  uint16_t m_cnsequence;

  /**
   * \brief home agent address list.
   */
  std::list<Ipv6Address> m_Haalist;

  /**
   * \brief route optimization flag.
   */
  bool m_roflag;

  /**
   * \brief default router (i.e. connected AR) address .
   */
  Ipv6Address m_defaultrouteraddress;

  /**
   * \brief prefix of the previous default route before handoff.
   */
  Ipv6Address m_OldPrefixToUse;

  /**
   * \brief interface index of the previous default route before handoff.
   */
  uint32_t m_OldinterfaceIndex;

  /**
   * \brief current interface index of the MN.
   */
  uint32_t m_IfIndex;

  /**
   * \brief Callback to trace RX (reception) ba packets.
   */ 
  TracedCallback<Ptr<Packet>, Ipv6Address, Ipv6Address, Ptr<Ipv6Interface> > m_rxbaTrace;

  /**
   * \brief Callback to trace TX (transmission) bu packets.
   */ 
  TracedCallback<Ptr<Packet>, Ipv6Address, Ipv6Address> m_txbuTrace;

};

} /* namespace ns3 */

#endif /* MIPV6_MN_H */

