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

#ifndef IPV6_TUNNEL_L4_PROTOCOL_H
#define IPV6_TUNNEL_L4_PROTOCOL_H

#include "ns3/ipv6-address.h"
#include "ns3/ip-l4-protocol.h"
#include "ns3/tunnel-net-device.h"

namespace ns3 {

class Node;
class Packet;

/**
 * \class Ipv6TunnelL4Protocol
 * \brief An implementation of the Ipv6 Tunnel protocol.
 */
class Ipv6TunnelL4Protocol : public IpL4Protocol
{
public:
  /**
   * \brief Interface ID
   */
  static TypeId GetTypeId ();

  /**
   * \brief IPv6 Mobility protocol number (135).
   */
  static const uint8_t PROT_NUMBER;

  /**
   * \brief Get MIPv6 protocol number.
   * \return protocol number
   */
  static uint16_t GetStaticProtocolNumber ();

  /**
   * \brief Constructor.
   */
  Ipv6TunnelL4Protocol ();

  /**
   * \brief Destructor.
   */
  virtual ~Ipv6TunnelL4Protocol ();

  /**
   * \brief Set the node.
   * \param node the node to set
   */
  void SetNode (Ptr<Node> node);

  /**
   * \brief Get the node.
   * \return node
   */
  Ptr<Node> GetNode (void);

  /**
 * \brief This method is called by AddAgregate and completes the aggregation
 * by setting the node in the Ipv6 Mobility stack.
 */
  virtual void NotifyNewAggregate ();

  /**
   * \brief Get the protocol number.
   * \return protocol number
   */
  virtual int GetProtocolNumber () const;

  /**
   * \brief Send a packet via IPv6 Mobility, note that packet already contains IPv6 Mobility header.
   * \param packet the packet to send which contains IPv6 Mobility header
   * \param src source address
   * \param dst destination address
   * \param ttl next hop limit
   */
  void SendMessage (Ptr<Packet> packet, Ipv6Address src, Ipv6Address dst, uint8_t ttl);

  /**
   * \brief Receive method.
   * \param p the packet
   * \param src source address
   * \param dst destination address
   * \param interface the interface from which the packet is coming
   */
  virtual enum IpL4Protocol::RxStatus Receive (Ptr<Packet> p,Ipv6Header const &header,Ptr<Ipv6Interface> incomingInterface);
  virtual void SetDownTarget (IpL4Protocol::DownTargetCallback cb);
  virtual void SetDownTarget6 (IpL4Protocol::DownTargetCallback6 cb);
  virtual IpL4Protocol::DownTargetCallback GetDownTarget (void) const;
  virtual IpL4Protocol::DownTargetCallback6 GetDownTarget6 (void) const;
  virtual enum IpL4Protocol::RxStatus Receive (Ptr<Packet> p,
                                               Ipv4Header const &header,
                                               Ptr<Ipv4Interface> incomingInterface);

  uint16_t AddTunnel (Ipv6Address remote, Ipv6Address local = Ipv6Address::GetZero ());
  void RemoveTunnel (Ipv6Address remote);
  uint16_t ModifyTunnel (Ipv6Address remote, Ipv6Address newRemote, Ipv6Address local = Ipv6Address::GetZero ());
  Ptr<TunnelNetDevice> GetTunnelDevice (Ipv6Address remote);



  // myfile.open ("example.txt");
  //myfile.close();
  //void filewriter(uint64_t u, Time t);
  void SetHomeAddress (Ipv6Address hoa);
  Ipv6Address GetHomeAddress ();

  void SetCacheAddressList (std::list<Ipv6Address> list);
  std::list<Ipv6Address> GetCacheAddressList ();

  void SetHA (Ipv6Address ha);
  Ipv6Address GetHA ();

  /**
   * TracedCallback signature for BU reception event.
   *
   * \param [in] packet The data packet originally sent.
   * \param [in] header IPv6 inner header
   * \param [in] interface the IPv6 interface in which the data packet received
   */
  typedef void (* RxTracedCallback)
    (Ptr<Packet> packet, Ipv6Header header, Ptr<Ipv6Interface> interface);

protected:
  /**
   * \brief Dispose this object.
   */
  virtual void DoDispose ();

private:
  typedef std::map<Ipv6Address, Ptr<TunnelNetDevice> > TunnelMap;
  typedef std::map<Ipv6Address, Ptr<TunnelNetDevice> >::iterator TunnelMapI;

  /**
   * \brief The node.
   */
  Ptr<Node> m_node;
  uint64_t counter;
  Time sctime;
  //std::ofstream myfile;
  TunnelMap m_tunnelMap;
  Ipv6Address m_hoa;
  std::list<Ipv6Address> m_Cachelist;
  Ipv6Address m_ha;
  /**
   * \brief Callback to trace RX (reception) data packets at HA.
   */ 
  TracedCallback<Ptr<Packet>, Ipv6Header, Ptr<Ipv6Interface> > m_rxHaPktTrace;
  /**
   * \brief Callback to trace RX (reception) data packets at MN.
   */ 
  TracedCallback<Ptr<Packet>, Ipv6Header, Ptr<Ipv6Interface> > m_rxMnPktTrace;

};

} /* namespace ns3 */

#endif /* IPV6_TUNNEL_L4_PROTOCOL_H */
