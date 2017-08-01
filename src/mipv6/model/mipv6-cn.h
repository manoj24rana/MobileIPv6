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

#ifndef MIPV6_CN_H
#define MIPV6_CN_H

#include "mipv6-agent.h"
#include "bcache.h"
#include "mipv6-header.h"

namespace ns3 {
class Packet;

class Mipv6CN : public Mipv6Agent
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
  Mipv6CN ();
  /**
   * \brief Destructor.
   */
  virtual ~Mipv6CN ();

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
   * \brief build BA.
   * \param bu the received bu header
   * \param hoa home address
   * \param status staus of BU reception
   * \return the built BA 
   */
  Ptr<Packet> BuildBA (Ipv6MobilityBindingUpdateHeader bu, Ipv6Address hoa, uint8_t status);

  /**
   * \brief build HoT.
   * \param hoti the received hoti header
   * \param hoa home address
   * \return the built HoT 
   */
  Ptr<Packet> BuildHoT (Ipv6HoTIHeader hoti, Ipv6Address hoa);

  /**
   * \brief build CoT.
   * \param coti the received coti header
   * \param hoa home address
   * \return the built CoT 
   */
  Ptr<Packet> BuildCoT (Ipv6CoTIHeader coti, Ipv6Address hoa);

  /**
   * \brief Handle BU.
   * \param packet the received BU packet
   * \param src source address
   * \param dst destination address
   * \param interface where it is received
   * \return BU handling status
   */
  virtual uint8_t HandleBU (Ptr<Packet> packet, const Ipv6Address &src, const Ipv6Address &dst, Ptr<Ipv6Interface> interface);

  /**
   * \brief Handle HoTI.
   * \param packet the received HoTI packet
   * \param src source address
   * \param dst destination address
   * \param interface where it is received
   * \return status 
   */
  virtual uint8_t HandleHoTI (Ptr<Packet> packet, const Ipv6Address &src, const Ipv6Address &dst, Ptr<Ipv6Interface> interface);

  /**
   * \brief Handle CoTI.
   * \param packet the received CoTI packet
   * \param src source address
   * \param dst destination address
   * \param interface where it is received 
   * \return status 
   */
  virtual uint8_t HandleCoTI (Ptr<Packet> packet, const Ipv6Address &src, const Ipv6Address &dst, Ptr<Ipv6Interface> interface);

private:

  /**
   * \brief the binding cache associated with this CN 
   */
  Ptr<BCache> m_bCache;

  /**
   * \brief Callback to trace RX (reception) bu packets.
   */ 
  TracedCallback<Ptr<Packet>, Ipv6Address, Ipv6Address, Ptr<Ipv6Interface> > m_rxbuTrace;

};

} /* namespace ns3 */

#endif /* MIPV6_CN_H */

