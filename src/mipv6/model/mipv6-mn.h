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

namespace ns3 {

class mipv6MN : public mipv6Agent
{
public:
  mipv6MN (std::list<Ipv6Address> haalist);

  virtual ~mipv6MN ();

  uint16_t GetHomeBUSequence ();
  uint16_t GetCNBUSequence ();

  Ptr<Packet> BuildHomeBU ();
  Ptr<Packet> BuildCNBU ();
  Ptr<Packet> BuildHoTI ();
  Ptr<Packet> BuildCoTI ();
  bool SetupTunnelAndRouting ();
  void ClearTunnelAndRouting ();
  void StartRRTest ();
  void SetRouteOptimizationReuiredField (bool roflag);
  bool IsRouteOptimizationRequired ();
  bool IsHomeMatch (Ipv6Address addr);
  void SetDefaultRouterAddress (Ipv6Address addr, uint32_t index);
  bool CheckAddresses (Ipv6Address ha, Ipv6Address hoa);





protected:
  virtual void NotifyNewAggregate ();

  virtual void HandleNewAttachment (Ipv6Address ipr);
  virtual uint8_t HandleBA (Ptr<Packet> packet, const Ipv6Address &src, const Ipv6Address &dst, Ptr<Ipv6Interface> interface);
  virtual uint8_t HandleHoT (Ptr<Packet> packet, const Ipv6Address &src, const Ipv6Address &dst, Ptr<Ipv6Interface> interface);
  virtual uint8_t HandleCoT (Ptr<Packet> packet, const Ipv6Address &src, const Ipv6Address &dst, Ptr<Ipv6Interface> interface);
//  virtual std::list<Ipv6Address> GetAddressCache();
//  virtual void SetAddressCache(std::list<Ipv6Address> acac);

private:
  Ptr<BList> m_buinf;
  uint16_t m_hsequence;
  uint16_t m_cnsequence;
  std::list<Ipv6Address> m_Haalist;
  bool m_roflag;
  Ipv6Address m_defaultrouteraddress;
  Ipv6Address m_OldPrefixToUse;
  uint32_t m_OldinterfaceIndex;
  uint32_t m_IfIndex;


};

} /* namespace ns3 */

#endif /* MIPV6_MN_H */

