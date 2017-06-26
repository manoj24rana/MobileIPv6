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
#include "ns3/assert.h"
#include "ns3/packet.h"
#include "ns3/node.h"
#include "mipv6-agent.h"
#include "ns3/internet-module.h"
#include "mipv6-tun-l4-protocol.h"


using namespace std;

NS_LOG_COMPONENT_DEFINE ("Ipv6TunnelL4Protocol");

namespace ns3 {

NS_OBJECT_ENSURE_REGISTERED (Ipv6TunnelL4Protocol);

const uint8_t Ipv6TunnelL4Protocol::PROT_NUMBER = 41; /* IPV6-in-IPv6 */

TypeId Ipv6TunnelL4Protocol::GetTypeId ()
{
  static TypeId tid = TypeId ("ns3::Ipv6TunnelL4Protocol")
    .SetParent<IpL4Protocol> ()
    .AddConstructor<Ipv6TunnelL4Protocol> ()
  ;
  return tid;
}

Ipv6TunnelL4Protocol::Ipv6TunnelL4Protocol ()
  : m_node (0), counter (202)
{
  //myfile.open ("example.txt");
  // double d;
  // for(d=2.0;d<=14.0; d=d+0.01){
  // Simulator::Schedule(Seconds (d), &Ipv6TunnelL4Protocol::filewriter, this, counter-202, sctime-2.0);
  // }
  SetHomeAddress ("::");
  NS_LOG_FUNCTION_NOARGS ();
}
/*
void Ipv6TunnelL4Protocol::filewriter(uint64_t u, Time t){
myfile << u <<"\t" << t << "\n";
}
*/
Ipv6TunnelL4Protocol::~Ipv6TunnelL4Protocol ()
{
  NS_LOG_FUNCTION_NOARGS ();
  //myfile.close();
}

void Ipv6TunnelL4Protocol::DoDispose ()
{
  NS_LOG_FUNCTION_NOARGS ();

  m_node = 0;

  for ( TunnelMapI i = m_tunnelMap.begin (); i != m_tunnelMap.end (); i++ )
    {
      i->second = 0;
    }

  m_tunnelMap.clear ();
  //myfile.close();
  IpL4Protocol::DoDispose ();
}

void Ipv6TunnelL4Protocol::NotifyNewAggregate ()
{
  NS_LOG_FUNCTION_NOARGS ();

  if (m_node == 0)
    {
      Ptr<Node> node = this->GetObject<Node> ();
      if (node != 0)
        {
          Ptr<Ipv6L3Protocol> ipv6 = this->GetObject<Ipv6L3Protocol> ();
          if (ipv6 != 0)
            {
              this->SetNode (node);
              ipv6->Insert (this);
            }
        }
    }
  IpL4Protocol::NotifyNewAggregate ();
}

void Ipv6TunnelL4Protocol::SetNode (Ptr<Node> node)
{
  NS_LOG_FUNCTION (this << node);
  m_node = node;
}

Ptr<Node> Ipv6TunnelL4Protocol::GetNode (void)
{
  NS_LOG_FUNCTION_NOARGS ();
  return m_node;
}

int Ipv6TunnelL4Protocol::GetProtocolNumber () const
{
  NS_LOG_FUNCTION_NOARGS ();
  return PROT_NUMBER;
}

void Ipv6TunnelL4Protocol::SendMessage (Ptr<Packet> packet, Ipv6Address src, Ipv6Address dst, uint8_t ttl)
{
  NS_LOG_FUNCTION (this << packet << src << dst << (uint32_t)ttl);
  Ptr<Ipv6L3Protocol> ipv6 = m_node->GetObject<Ipv6L3Protocol> ();
  SocketIpTtlTag tag;
  NS_ASSERT (ipv6 != 0);

  tag.SetTtl (ttl);
  packet->AddPacketTag (tag);
  ipv6->Send (packet, src, dst, PROT_NUMBER, 0);
}

enum IpL4Protocol::RxStatus Ipv6TunnelL4Protocol::Receive (Ptr<Packet> p, Ipv6Header const &header, Ptr<Ipv6Interface> incomingInterface)
{

  NS_LOG_FUNCTION ("Bong!!!");

  Ptr<Ipv6L3Protocol> ipv6 = GetNode ()->GetObject<Ipv6L3Protocol>();
  NS_ASSERT (ipv6 != 0);
  Ipv6Address src = header.GetSourceAddress ();
  /**
   * Check whether the packet belongs to one of tunnels
   */
  Ptr<TunnelNetDevice> tdev = GetTunnelDevice (src);

  if (tdev == 0 && GetCacheAddressList ().size ())
    {
      std::list<Ipv6Address>::iterator iter = std::find (GetCacheAddressList ().begin (), GetCacheAddressList ().end (), src);
      if ( GetCacheAddressList ().end () != iter )
        {
          tdev = GetTunnelDevice (GetHA ());
        }
    }


/*  if(tdev == 0)
   {
     Ptr<mipv6Agent> agent = GetNode()->GetObject<mipv6Agent>();
     std::list<Ipv6Address> lista;
     lista = agent->GetAddressCache();
     if(lista.size() && agent->IsHomeMatch(src))
      {
        for(std::list<Ipv6Address>::iterator iter=lista.begin(); iter!=lista.end(); iter++)
         {
          tdev = GetTunnelDevice (iter.first());
          if (tdev)
           break;
         }
      }
   }  */
  if (tdev == 0 && src != "::")
    {
      NS_LOG_DEBUG ("The packet does not associate any tunnel device");
      return IpL4Protocol::RX_OK;
    }
  NS_LOG_FUNCTION (src << "Receive in ipv6tunnell4protocol ........................................");
  Ptr<Packet> packet = p->Copy ();

  Ipv6Header innerHeader;
  packet->RemoveHeader (innerHeader);

  Ipv6Address source = innerHeader.GetSourceAddress ();
  Ipv6Address destination = innerHeader.GetDestinationAddress ();
/*
if (destination==Ipv6Address("1001:db80::200:ff:fe00:b"))
{
//p->RemoveHeader(innerHeader);
return IpL4Protocol::RX_OK;
}
*/
  if (source.IsLinkLocal ()
      || destination.IsLinkLocal ()
      || destination.IsAllNodesMulticast ()
      || destination.IsAllRoutersMulticast ()
      || destination.IsAllHostsMulticast ()
      || destination.IsSolicitedMulticast ())
    {
      return IpL4Protocol::RX_OK;
    }

  //SocketIpTtlTag tag;
  NS_LOG_FUNCTION (source << destination << "TIMBER1 ........................................!!!!");
  //tag.SetTtl (innerHeader.GetHopLimit() - 1);
  NS_LOG_FUNCTION ("TIMBER1.1 ........................................!!!!");
//packet->AddPacketTag (tag);
  NS_LOG_FUNCTION ("TIMBER2 ........................................!!!!");
  //Prevent infinite loop
  Ptr<Ipv6Route> route;
  Socket::SocketErrno err;
  Ptr<NetDevice> oif (0); //specify non-zero if bound to a source address

  Ipv6StaticRoutingHelper routingHelper;

  Ptr<Ipv6StaticRouting> routing = routingHelper.GetStaticRouting (ipv6);

  NS_ASSERT (routing);

  route = routing->RouteOutput (packet, innerHeader, oif, err);
  NS_LOG_FUNCTION (source << destination << "TIMBER3 ........................................!!!!");

  if (destination.IsEqual (GetHomeAddress ()))

    {
      uint8_t nextHeader = innerHeader.GetNextHeader ();
      Ptr<IpL4Protocol> protocol = 0;
      protocol = ipv6->GetProtocol (nextHeader);
      if (protocol)
        {
          return protocol->Receive (packet,innerHeader,incomingInterface);
        }
//  Ptr<UdpL4Protocol> udpv6 = GetNode()->GetObject<UdpL4Protocol>();
//udpv6->Receive(packet,innerHeader,incomingInterface);
//NS_LOG_FUNCTION ("TIMBER4 .......................................................!!!!");
//NS_LOG_FUNCTION (counter);
//sctime=(Simulator::Now());
//myfile << counter << "\t" << sctime.GetMicroSeconds()/1000000.0 << "\n";
//counter++;
//NS_LOG_FUNCTION (sctime);
//return IpL4Protocol::RX_OK;

    }


  ipv6->Send (packet, source, destination, innerHeader.GetNextHeader (), route);
  NS_LOG_FUNCTION ("TIMBER5 .......................................................!!!!");
  return IpL4Protocol::RX_OK;
}



uint16_t Ipv6TunnelL4Protocol::AddTunnel (Ipv6Address remote, Ipv6Address local)
{
  NS_LOG_FUNCTION (this << remote << local);

  //Search existing tunnel device
  TunnelMapI it = m_tunnelMap.find (remote);
  Ptr<TunnelNetDevice> dev = m_node->GetObject<TunnelNetDevice> ();
  //NS_LOG_FUNCTION ("BARA1");
  if (it == m_tunnelMap.end ())
    {
      //    NS_LOG_FUNCTION ("BARA2");
      if (dev == 0)
        {
          // NS_LOG_FUNCTION ("BARA3" << dev);
          dev = CreateObject<TunnelNetDevice> ();

          dev->SetAddress (Mac48Address::Allocate ());
          m_node->AddDevice (dev);
          m_tunnelMap.insert (std::pair<Ipv6Address, Ptr<TunnelNetDevice> > (remote, dev));
        }
      dev->SetRemoteAddress (remote);
      dev->SetLocalAddress (local);
//NS_LOG_FUNCTION ("BARA4");
    }

  else
    {
      dev = it->second;
    }

  dev->IncreaseRefCount ();
  NS_LOG_FUNCTION ("Yukjjbscma ..........................." << dev->GetRefCount ());
  Ptr<Ipv6> ipv6 = m_node->GetObject<Ipv6> ();
  int32_t ifIndex = -1;
  //NS_LOG_FUNCTION ("BARA5");
  ifIndex = ipv6->GetInterfaceForDevice (dev);

  if (ifIndex == -1)
    {
      ifIndex = ipv6->AddInterface (dev);
      //   NS_LOG_FUNCTION ("BARA6");
      NS_ASSERT_MSG (ifIndex >= 0, "Cannot add an IPv6 interface");

      ipv6->SetMetric (ifIndex, 1);
      ipv6->SetUp (ifIndex);
    }
  //NS_LOG_FUNCTION ("BARA7");
  return ifIndex;
}

void Ipv6TunnelL4Protocol::RemoveTunnel (Ipv6Address remote)
{
  NS_LOG_FUNCTION ( "Remove tunnel............................." << remote);

  Ptr<TunnelNetDevice> dev = GetTunnelDevice (remote);

  if (dev)
    {

      NS_LOG_FUNCTION ("Line 1 ...........................");
      dev->DecreaseRefCount ();

      if (dev->GetRefCount () == 0)
        {
          TunnelMapI it = m_tunnelMap.find (remote);
          NS_LOG_FUNCTION ("Line 2 ..........................." << "...." << dev->GetRefCount ());
          it->second = 0;

          m_tunnelMap.erase (it);
        }
      NS_LOG_FUNCTION ("Line 3 ..........................." << dev->GetRefCount ());
    }
}

uint16_t  Ipv6TunnelL4Protocol::ModifyTunnel (Ipv6Address remote, Ipv6Address newRemote, Ipv6Address local)
{
  NS_LOG_FUNCTION ( this << remote << newRemote << local );

  Ptr<TunnelNetDevice> dev = GetTunnelDevice (remote);
  NS_ASSERT (dev != 0);
  NS_ASSERT (dev->GetRefCount () > 0);

  RemoveTunnel (remote);
  return AddTunnel (newRemote, local);
}

Ptr<TunnelNetDevice> Ipv6TunnelL4Protocol::GetTunnelDevice (Ipv6Address remote)
{
  NS_LOG_FUNCTION ( this << remote );

  TunnelMapI it = m_tunnelMap.find (remote);

  if (it != m_tunnelMap.end ())
    {
      return it->second;
    }

  return 0;
}

void Ipv6TunnelL4Protocol::SetDownTarget (IpL4Protocol::DownTargetCallback cb)
{
}
void Ipv6TunnelL4Protocol::SetDownTarget6 (IpL4Protocol::DownTargetCallback6 cb)
{
}
IpL4Protocol::DownTargetCallback Ipv6TunnelL4Protocol::GetDownTarget (void) const
{
  IpL4Protocol::DownTargetCallback t;
  return t;
}
IpL4Protocol::DownTargetCallback6 Ipv6TunnelL4Protocol::GetDownTarget6 (void) const
{
  IpL4Protocol::DownTargetCallback6 y;
  return y;
}
enum IpL4Protocol::RxStatus Ipv6TunnelL4Protocol::Receive (Ptr<Packet> p, Ipv4Header const &header, Ptr<Ipv4Interface> incomingInterface)
{


  return IpL4Protocol::RX_OK;

}

void Ipv6TunnelL4Protocol::SetHomeAddress (Ipv6Address hoa)
{
  m_hoa = hoa;
}

Ipv6Address Ipv6TunnelL4Protocol::GetHomeAddress ()
{
  return m_hoa;
}

void Ipv6TunnelL4Protocol::SetCacheAddressList (std::list<Ipv6Address> list)
{
  m_Cachelist = list;
}

std::list<Ipv6Address> Ipv6TunnelL4Protocol::GetCacheAddressList ()
{
  return m_Cachelist;
}

void Ipv6TunnelL4Protocol::SetHA (Ipv6Address ha)
{
  m_ha = ha;
}

Ipv6Address Ipv6TunnelL4Protocol::GetHA ()
{
  return m_ha;
}

} /* namespace ns3 */

