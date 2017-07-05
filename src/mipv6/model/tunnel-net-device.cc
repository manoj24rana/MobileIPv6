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

#include "ns3/log.h"
#include "ns3/mac48-address.h"
#include "ns3/error-model.h"
#include "ns3/channel.h"
#include "ns3/trace-source-accessor.h"
#include "ns3/uinteger.h"
#include "ns3/llc-snap-header.h"
#include "ns3/ipv6-l3-protocol.h"
#include "ns3/ipv6-routing-protocol.h"
#include "ns3/ipv6-route.h"
#include "ns3/ipv6-header.h"
#include "tunnel-net-device.h"

NS_LOG_COMPONENT_DEFINE ("TunnelNetDevice");

namespace ns3 {

NS_OBJECT_ENSURE_REGISTERED (TunnelNetDevice);

TypeId
TunnelNetDevice::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::TunnelNetDevice")
    .SetParent<NetDevice> ()
    .AddConstructor<TunnelNetDevice> ()
    .AddAttribute ("Mtu", "The MAC-level Maximum Transmission Unit",
                   UintegerValue (1500),
                   MakeUintegerAccessor (&TunnelNetDevice::SetMtu,
                                         &TunnelNetDevice::GetMtu),
                   MakeUintegerChecker<uint16_t> ())
    .AddTraceSource ("MacTx",
                     "Trace source indicating a packet has arrived for transmission by this device",
                     MakeTraceSourceAccessor (&TunnelNetDevice::m_macTxTrace),
                     "ns3::Packet::TracedCallback")
    .AddTraceSource ("MacPromiscRx",
                     "A packet has been received by this device, has been passed up from the physical layer "
                     "and is being forwarded up the local protocol stack.  This is a promiscuous trace,",
                     MakeTraceSourceAccessor (&TunnelNetDevice::m_macPromiscRxTrace),
                     "ns3::Packet::TracedCallback")
    .AddTraceSource ("MacRx",
                     "A packet has been received by this device, has been passed up from the physical layer "
                     "and is being forwarded up the local protocol stack.  This is a non-promiscuous trace,",
                     MakeTraceSourceAccessor (&TunnelNetDevice::m_macRxTrace),
                     "ns3::Packet::TracedCallback")
    //
    // Trace sources designed to simulate a packet sniffer facility (tcpdump).
    //
    .AddTraceSource ("Sniffer",
                     "Trace source simulating a non-promiscuous packet sniffer attached to the device",
                     MakeTraceSourceAccessor (&TunnelNetDevice::m_snifferTrace),
                     "ns3::Packet::TracedCallback")
    .AddTraceSource ("PromiscSniffer",
                     "Trace source simulating a promiscuous packet sniffer attached to the device",
                     MakeTraceSourceAccessor (&TunnelNetDevice::m_promiscSnifferTrace),
                     "ns3::Packet::TracedCallback")
  ;
  return tid;
}

TunnelNetDevice::TunnelNetDevice ()
  : m_localAddress ("::"),
  m_remoteAddress ("::"),
  m_refCount (1)
{
  NS_LOG_FUNCTION_NOARGS ();

  m_needsArp = false;
  m_supportsSendFrom = true;
  m_isPointToPoint = true;
}

void
TunnelNetDevice::SetNeedsArp (bool needsArp)
{
  NS_LOG_FUNCTION ( this << needsArp);
  m_needsArp = needsArp;
}

void
TunnelNetDevice::SetSupportsSendFrom (bool supportsSendFrom)
{
  NS_LOG_FUNCTION ( this << supportsSendFrom);
  m_supportsSendFrom = supportsSendFrom;
}

void
TunnelNetDevice::SetIsPointToPoint (bool isPointToPoint)
{
  NS_LOG_FUNCTION ( this << isPointToPoint);
  m_isPointToPoint = isPointToPoint;
}

bool
TunnelNetDevice::SetMtu (const uint16_t mtu)
{
  NS_LOG_FUNCTION ( this << mtu );
  m_mtu = mtu;
  return true;
}


TunnelNetDevice::~TunnelNetDevice ()
{
  NS_LOG_FUNCTION_NOARGS ();
}


void TunnelNetDevice::DoDispose ()
{
  NS_LOG_FUNCTION_NOARGS ();
  m_node = 0;
  NetDevice::DoDispose ();
}

Ipv6Address TunnelNetDevice::GetLocalAddress () const
{
  NS_LOG_FUNCTION_NOARGS ();

  return m_localAddress;
}

void TunnelNetDevice::SetLocalAddress (Ipv6Address laddr)
{
  NS_LOG_FUNCTION ( this << laddr );

  m_localAddress = laddr;
}

Ipv6Address TunnelNetDevice::GetRemoteAddress () const
{
  NS_LOG_FUNCTION_NOARGS ();

  return m_remoteAddress;
}

void TunnelNetDevice::SetRemoteAddress (Ipv6Address raddr)
{
  NS_LOG_FUNCTION ( this << raddr );

  m_remoteAddress = raddr;
}

void TunnelNetDevice::IncreaseRefCount ()
{
  NS_LOG_FUNCTION_NOARGS ();
  m_refCount++;
}

void TunnelNetDevice::DecreaseRefCount ()
{
  NS_LOG_FUNCTION_NOARGS ();
  m_refCount--;
}

uint32_t TunnelNetDevice::GetRefCount () const
{
  NS_LOG_FUNCTION_NOARGS ();
  return m_refCount;
}

bool
TunnelNetDevice::Receive (Ptr<Packet> packet, uint16_t protocol,
                          const Address &source, const Address &destination,
                          PacketType packetType)
{
  NS_LOG_FUNCTION ( this << packet << protocol << source << destination << packetType << "Receive ECHO!!!!!!!!!!!!!!!!!!");
  //
  // For all kinds of packetType we receive, we hit the promiscuous sniffer
  // hook and pass a copy up to the promiscuous callback.  Pass a copy to
  // make sure that nobody messes with our packet.
  //
  m_promiscSnifferTrace (packet);
  if (!m_promiscRxCallback.IsNull ())
    {
      m_macPromiscRxTrace (packet);
      m_promiscRxCallback (this, packet, protocol, source, destination, packetType);
    }

  //
  // If this packet is not destined for some other host, it must be for us
  // as either a broadcast, multicast or unicast.  We need to hit the mac
  // packet received trace hook and forward the packet up the stack.
  //
  if (packetType != PACKET_OTHERHOST)
    {
      m_snifferTrace (packet);
      m_macRxTrace (packet);
      return m_rxCallback (this, packet, protocol, source);
    }
  return true;
}


void
TunnelNetDevice::SetIfIndex (const uint32_t index)
{
  NS_LOG_FUNCTION ( this << index);
  m_index = index;
}

uint32_t
TunnelNetDevice::GetIfIndex (void) const
{
  NS_LOG_FUNCTION_NOARGS ();
  return m_index;
}

Ptr<Channel>
TunnelNetDevice::GetChannel (void) const
{
  NS_LOG_FUNCTION_NOARGS ();
  return Ptr<Channel> ();
}

Address
TunnelNetDevice::GetAddress (void) const
{
  NS_LOG_FUNCTION_NOARGS ();
  return m_myAddress;
}

void
TunnelNetDevice::SetAddress (Address addr)
{
  NS_LOG_FUNCTION ( this << addr);
  m_myAddress = addr;
}

uint16_t
TunnelNetDevice::GetMtu (void) const
{
  NS_LOG_FUNCTION_NOARGS ();
  return m_mtu;
}

bool
TunnelNetDevice::IsLinkUp (void) const
{
  NS_LOG_FUNCTION_NOARGS ();
  return true;
}

void
TunnelNetDevice::AddLinkChangeCallback (Callback<void> callback)
{
  NS_LOG_FUNCTION_NOARGS ();
}

bool
TunnelNetDevice::IsBroadcast (void) const
{
  NS_LOG_FUNCTION_NOARGS ();
  return true;
}

Address
TunnelNetDevice::GetBroadcast (void) const
{
  NS_LOG_FUNCTION_NOARGS ();
  return Mac48Address ("ff:ff:ff:ff:ff:ff");
}

bool
TunnelNetDevice::IsMulticast (void) const
{
  NS_LOG_FUNCTION_NOARGS ();
  return false;
}

Address TunnelNetDevice::GetMulticast (Ipv4Address multicastGroup) const
{
  NS_LOG_FUNCTION_NOARGS ();
  return Mac48Address ("ff:ff:ff:ff:ff:ff");
}

Address TunnelNetDevice::GetMulticast (Ipv6Address addr) const
{
  NS_LOG_FUNCTION_NOARGS ();
  return Mac48Address ("ff:ff:ff:ff:ff:ff");
}


bool
TunnelNetDevice::IsPointToPoint (void) const
{
  NS_LOG_FUNCTION_NOARGS ();
  return m_isPointToPoint;
}

bool
TunnelNetDevice::Send (Ptr<Packet> packet, const Address& dest, uint16_t protocolNumber)
{

  NS_LOG_FUNCTION ( this << packet << dest << protocolNumber );
  Ipv6Header iph;
  packet->PeekHeader (iph);

  Ipv6Address a,b;
  a = iph.GetSourceAddress ();
  b = iph.GetDestinationAddress ();
  NS_LOG_FUNCTION ("Source and Destination Address:" << a << b);

  if (a.IsLinkLocal ()
      || b.IsLinkLocal ()
      || b.IsAllNodesMulticast ()
      || b.IsAllRoutersMulticast ()
      || b.IsSolicitedMulticast ())
    {

      return true;

    }
  Ptr<Ipv6L3Protocol> ipv6 = GetNode ()->GetObject<Ipv6L3Protocol>();
  NS_ASSERT (ipv6 != 0 && ipv6->GetRoutingProtocol () != 0);
  NS_ASSERT ( !m_remoteAddress.IsAny () );

  Ipv6Address src = m_localAddress;
  Ipv6Address dst = m_remoteAddress;
  SocketIpTtlTag tag;
  uint8_t ttl = 64;
  m_macTxTrace (packet);
  if ( m_localAddress.IsAny () )
    {
      Ipv6Header header;

      Socket::SocketErrno err;
      Ptr<Ipv6Route> route;
      Ptr<NetDevice> oif (0);     //specify non-zero if bound to a source address
      header.SetDestinationAddress (dst);
      route = ipv6->GetRoutingProtocol ()->RouteOutput (packet, header, oif, err);

      if (route == 0)
        {
          NS_LOG_LOGIC ("No route for tunnel remote address");

          return false;
        }

      src = route->GetSource ();
      NS_LOG_FUNCTION ("Source Address=" << src << "Destination Address=" << dst);
      tag.SetTtl (ttl);
      ipv6->Send (packet, src, dst, 41 /* IPv6-in-IPv6 */, route);
    }
  else
    {
      tag.SetTtl (ttl);
      packet->AddPacketTag (tag);

      ipv6->Send (packet, src, dst, 41 /* IPv6-in-IPv6 */, 0);
    }

  return true;
}

bool
TunnelNetDevice::SendFrom (Ptr<Packet> packet, const Address& source, const Address& dest, uint16_t protocolNumber)
{

  NS_LOG_FUNCTION ( this << packet << dest << protocolNumber );

  NS_ASSERT (m_supportsSendFrom);

  Ptr<Ipv6L3Protocol> ipv6 = GetNode ()->GetObject<Ipv6L3Protocol>();
  NS_ASSERT (ipv6 != 0 && ipv6->GetRoutingProtocol () != 0);
  NS_ASSERT ( !m_remoteAddress.IsAny () );

  Ipv6Address src = m_localAddress;
  Ipv6Address dst = m_remoteAddress;
  SocketIpTtlTag tag;
  uint8_t ttl = 64;

  m_macTxTrace (packet);

  Mac48Address dest2 = Mac48Address::ConvertFrom (dest);
  if (dest2.IsBroadcast () || dest2.IsGroup ())
    {
      NS_LOG_LOGIC ("try to send broadcast target.. skipped");
      return true;
    }

  if ( m_localAddress.IsAny () )
    {

      Ipv6Header header;
      Socket::SocketErrno err;
      Ptr<Ipv6Route> route;
      Ptr<NetDevice> oif (0);     //specify non-zero if bound to a source address

      header.SetDestinationAddress (dst);
      route = ipv6->GetRoutingProtocol ()->RouteOutput (packet, header, oif, err);

      if (route == 0)
        {
          NS_LOG_LOGIC ("No route for tunnel remote address");

          return false;
        }

      src = route->GetSource ();

      tag.SetTtl (ttl);
      packet->AddPacketTag (tag);

      ipv6->Send (packet, src, dst, 41 /* IPv6-in-IPv6 */, route);
    }
  else
    {
      tag.SetTtl (ttl);
      packet->AddPacketTag (tag);

      ipv6->Send (packet, src, dst, 41 /* IPv6-in-IPv6 */, 0);
    }

  return true;
}

Ptr<Node>
TunnelNetDevice::GetNode (void) const
{
  return m_node;
}

void
TunnelNetDevice::SetNode (Ptr<Node> node)
{
  m_node = node;
}

bool
TunnelNetDevice::NeedsArp (void) const
{
  return m_needsArp;
}

void
TunnelNetDevice::SetReceiveCallback (NetDevice::ReceiveCallback cb)
{
  NS_LOG_FUNCTION_NOARGS ();
  m_rxCallback = cb;
}

void
TunnelNetDevice::SetPromiscReceiveCallback (NetDevice::PromiscReceiveCallback cb)
{
  NS_LOG_FUNCTION_NOARGS ();
  m_promiscRxCallback = cb;
}

bool
TunnelNetDevice::SupportsSendFrom () const
{
  NS_LOG_FUNCTION_NOARGS ();
  return m_supportsSendFrom;
}

bool TunnelNetDevice::IsBridge (void) const
{
  NS_LOG_FUNCTION_NOARGS ();
  return false;
}


} // namespace ns3
