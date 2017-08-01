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

#ifndef TUNNEL_NET_DEVICE_H
#define TUNNEL_NET_DEVICE_H

#include "ns3/address.h"
#include "ns3/node.h"
#include "ns3/net-device.h"
#include "ns3/callback.h"
#include "ns3/packet.h"
#include "ns3/ptr.h"
#include "ns3/traced-callback.h"

namespace ns3 {


/**
 * \class TunnelNetDevice
 * \brief A tunnel device, similar to Linux TUN/TAP interfaces.
 *
 */
class TunnelNetDevice : public NetDevice
{
public:

  /**
   * \brief get typeid
   * \return typeid
   */
  static TypeId GetTypeId (void);
  TunnelNetDevice ();

  virtual ~TunnelNetDevice ();

  /**
   * \brief Configure whether the virtual device needs ARP
   *
   * \param needsArp the the 'needs arp' value that will be returned
   * by the NeedsArp() method.  The method IsBroadcast() will also
   * return this value.
   */
  void SetNeedsArp (bool needsArp);

  /**
   * \brief Configure whether the virtual device is point-to-point
   *
   * \param isPointToPoint the value that should be returned by the
   * IsPointToPoint method for this instance.
   */
  void SetIsPointToPoint (bool isPointToPoint);

  /**
   * \brief Configure whether the virtual device supports SendFrom
   */
  void SetSupportsSendFrom (bool supportsSendFrom);

  /**
   * \brief Configure the reported MTU for the virtual device.
   * \param mtu MTU value to set
   * \return whether the MTU value was within legal bounds
   */
  bool SetMtu (const uint16_t mtu);

  /**
   * \brief get local address.
   * \returns local address
   */
  Ipv6Address GetLocalAddress () const;

  /**
   * \brief set local address.
   * \param laddr local address
   */
  void SetLocalAddress (Ipv6Address laddr);

  /**
   * \brief get remote address.
   * \returns remote address
   */
  Ipv6Address GetRemoteAddress () const;

  /**
   * \brief set remote address.
   * \param raddr remote address
   */
  void SetRemoteAddress (Ipv6Address raddr);

  /**
   * \brief increase ref count.
  */
  void IncreaseRefCount ();

  /**
   * \brief decrease ref count.
  */
  void DecreaseRefCount ();

  /**
   * \brief get the ref count.
  */
  uint32_t GetRefCount () const;

  /**
   * \param packet packet sent from below up to Network Device
   * \param protocol Protocol type
   * \param source the address of the sender of this packet.
   * \param destination the address of the receiver of this packet.
   * \param packetType type of packet received (broadcast/multicast/unicast/otherhost)
   * \returns true if the packet was forwarded successfully, false otherwise.
   *
   * Forward a "virtually received" packet up
   * the node's protocol stack.
   */
  bool Receive (Ptr<Packet> packet, uint16_t protocol,
                const Address &source, const Address &destination,
                PacketType packetType);


  // inherited from NetDevice base class.
  virtual void SetIfIndex (const uint32_t index);
  virtual uint32_t GetIfIndex (void) const;
  virtual Ptr<Channel> GetChannel (void) const;
  virtual void SetAddress (Address address);
  virtual Address GetAddress (void) const;
  virtual uint16_t GetMtu (void) const;
  virtual bool IsLinkUp (void) const;
  virtual void AddLinkChangeCallback (Callback<void> callback);
  virtual bool IsBroadcast (void) const;
  virtual Address GetBroadcast (void) const;
  virtual bool IsMulticast (void) const;
  virtual Address GetMulticast (Ipv4Address multicastGroup) const;
  virtual Address GetMulticast (Ipv6Address addr) const;
  virtual bool IsPointToPoint (void) const;
  virtual bool Send (Ptr<Packet> packet, const Address& dest, uint16_t protocolNumber);
  virtual bool SendFrom (Ptr<Packet> packet, const Address& source, const Address& dest, uint16_t protocolNumber);
  virtual Ptr<Node> GetNode (void) const;
  virtual void SetNode (Ptr<Node> node);
  virtual bool NeedsArp (void) const;
  virtual void SetReceiveCallback (NetDevice::ReceiveCallback cb);
  virtual void SetPromiscReceiveCallback (NetDevice::PromiscReceiveCallback cb);
  virtual bool SupportsSendFrom () const;
  virtual bool IsBridge (void) const;

  /**
   * TracedCallback signature for arrived packet going to be transmitted by this device.
   * \param [in] packet The arrived packet.
   * \param [in] ih The inner header
   * \param [in] oh The outer header
   */
  typedef void (* TracedCallback2)
    (Ptr<Packet> packet, Ipv6Header ih, Ipv6Header oh);


protected:
  virtual void DoDispose (void);

private:

  /**
   * \brief mac address.
   */
  Address m_myAddress;
  /**
   * \brief Callback to trace received non-promiscuous packets which will be forwarded up the local protocol stack.
   */
  TracedCallback<Ptr<const Packet> > m_macRxTrace;
  /**
   * \brief Callback to trace received packets arrived for transmission.
   */
  TracedCallback<Ptr<const Packet> > m_macTxTrace;
  /**
   * \brief Callback to trace received promiscuous packets which will be forwarded up the local protocol stack.
   */
  TracedCallback<Ptr<const Packet> > m_macPromiscRxTrace;
  /**
   * \brief Callback to trace a non-promiscuous packet sniffer attached to the device.
   */
  TracedCallback<Ptr<const Packet> > m_snifferTrace;
  /**
   * \brief Callback to trace a promiscuous packet sniffer attached to the device.
   */
  TracedCallback<Ptr<const Packet> > m_promiscSnifferTrace;
  /**
   * \brief Callback to trace a packet arrived for transmission by this device.
   */
  TracedCallback<Ptr<Packet>, Ipv6Header, Ipv6Header> m_macTxTrace2;

  /**
   * \brief node which contains this device.
   */
  Ptr<Node> m_node;
  /**
   * \brief received packet callback variable.
   */
  ReceiveCallback m_rxCallback;
  /**
   * \brief received promiscuous packet callback variable.
   */
  PromiscReceiveCallback m_promiscRxCallback;
  /**
   * \brief currently not used.
   */
  std::string m_name;

  /**
   * \brief interface index.
  */
  uint32_t m_index;
  /**
   * \brief MTU.
  */
  uint16_t m_mtu;
  /**
   * \brief flag whether ARP required.
  */
  bool m_needsArp;
  /**
   * \brief flag whether it supports send from.
  */
  bool m_supportsSendFrom;
  /**
   * \brief point-to-point flag.
  */
  bool m_isPointToPoint;
  /**
   * \brief local address.
  */
  Ipv6Address m_localAddress;
  /**
   * \brief remote address.
  */
  Ipv6Address m_remoteAddress;
  /**
   * \brief ref count.
  */
  uint32_t m_refCount;
};

}  // namespace ns3

#endif /* TUNNEL_NET_DEVICE_H */

