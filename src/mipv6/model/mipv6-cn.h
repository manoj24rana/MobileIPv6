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

class mipv6CN : public mipv6Agent
{
public:
  mipv6CN ();

  virtual ~mipv6CN ();
  //Ipv6Address tempadds;
  //uint16_t temptunnelIf;
protected:
  virtual void NotifyNewAggregate ();

  Ptr<Packet> BuildBA (Ipv6MobilityBindingUpdateHeader bu, Ipv6Address hoa, uint8_t status);
  Ptr<Packet> BuildHoT (Ipv6HoTIHeader hoti, Ipv6Address hoa);
  Ptr<Packet> BuildCoT (Ipv6CoTIHeader coti, Ipv6Address hoa);
  virtual uint8_t HandleBU (Ptr<Packet> packet, const Ipv6Address &src, const Ipv6Address &dst, Ptr<Ipv6Interface> interface);
  virtual uint8_t HandleHoTI (Ptr<Packet> packet, const Ipv6Address &src, const Ipv6Address &dst, Ptr<Ipv6Interface> interface);
  virtual uint8_t HandleCoTI (Ptr<Packet> packet, const Ipv6Address &src, const Ipv6Address &dst, Ptr<Ipv6Interface> interface);

private:
  Ptr<BCache> m_bCache;

};

} /* namespace ns3 */

#endif /* MIPV6_CN_H */

