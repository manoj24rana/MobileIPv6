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
#include "ns3/node.h"
#include "ns3/packet.h"
#include "ns3/net-device.h"
#include "ns3/ipv6-l3-protocol.h"
#include "ns3/ipv6-routing-protocol.h"
#include "ns3/ipv6-route.h"
#include "ns3/ipv6-interface.h"
#include "mipv6-header.h"
#include "mipv6-agent.h"
#include "mipv6-l4-protocol.h"

using namespace std;

NS_LOG_COMPONENT_DEFINE ("mipv6Agent");

namespace ns3
{

NS_OBJECT_ENSURE_REGISTERED (mipv6Agent);

TypeId mipv6Agent::GetTypeId ()
{
  static TypeId tid = TypeId ("ns3::mipv6Agent")
    .SetParent<Object> ()
    .AddConstructor<mipv6Agent> ()
    ;
  return tid;
}

mipv6Agent::mipv6Agent ()
  : count(0), m_node (0)
{
  NS_LOG_FUNCTION_NOARGS ();
}

mipv6Agent::~mipv6Agent ()
{
  NS_LOG_FUNCTION_NOARGS ();
}

void mipv6Agent::DoDispose ()
{
  NS_LOG_FUNCTION_NOARGS ();

  m_node = 0;
  Object::DoDispose ();
}

void mipv6Agent::SetNode (Ptr<Node> node)
{
  NS_LOG_FUNCTION (this << node);
  m_node = node;
}

Ptr<Node> mipv6Agent::GetNode (void)
{
  NS_LOG_FUNCTION_NOARGS();
  return m_node;
}
uint8_t mipv6Agent::Receive (Ptr<Packet> packet, const Ipv6Address &src, const Ipv6Address &dst, Ptr<Ipv6Interface> interface)
{
  //NS_LOG_FUNCTION ( this << packet << src << dst << interface );
  
  Ptr<Packet> p = packet->Copy ();
  
  MIPv6Header mh;
  
  p->PeekHeader (mh);
  
  uint8_t mhType = mh.GetMhType ();
  //NS_LOG_FUNCTION ( this << packet << src << dst << interface << "kela99" );
  if (mhType == MIPv6Header::IPV6_MOBILITY_BINDING_UPDATE)
    {
	NS_LOG_FUNCTION (this << packet << src<<"BU" << "recieve BU");
        HandleBU (packet, src, dst, interface);     
	}
  else if (mhType == MIPv6Header::IPV6_MOBILITY_BINDING_ACKNOWLEDGEMENT)
    {
        NS_LOG_FUNCTION (this << packet << src<<"receive BACK");	
        HandleBA (packet, src, dst, interface);     
	}


else if (mhType == MIPv6Header::HOME_TEST_INIT)
    {
	NS_LOG_FUNCTION (this << packet << src<<"HoTI" << "recieve HoTI");
       HandleHoTI (packet, src, dst, interface);
	}
else if (mhType == MIPv6Header::HOME_TEST)
    {
	NS_LOG_FUNCTION (this << packet << src<<"HoT" << "recieve HoT");
       HandleHoT (packet, src, dst, interface);
	}
else if (mhType == MIPv6Header::CARE_OF_TEST_INIT)
    {
	NS_LOG_FUNCTION (this << packet << src<<"CoTI" << "recieve CoTI");
       HandleCoTI (packet, src, dst, interface);
	}
else if (mhType == MIPv6Header::CARE_OF_TEST)
    {
	NS_LOG_FUNCTION (this << packet << src<<"CoT" << "recieve CoT");
       HandleCoT (packet, src, dst, interface);
	}
  else
    {
	  NS_LOG_ERROR ("Unknown MHType (" << (uint32_t)mhType << ")");
	}
	
  return 0;
}

void mipv6Agent::SendMessage (Ptr<Packet> packet, Ipv6Address dst, uint32_t ttl)
{
  NS_LOG_FUNCTION (this << packet << dst << (uint32_t)ttl<< "send");
  
  Ptr<Ipv6L3Protocol> ipv6 = m_node->GetObject<Ipv6L3Protocol> ();
  
  NS_ASSERT (ipv6 != 0 && ipv6->GetRoutingProtocol () != 0);
  
  Ipv6Header header;
  SocketIpTtlTag tag;
  Socket::SocketErrno err;
  Ptr<Ipv6Route> route;
  Ptr<NetDevice> oif (0); //specify non-zero if bound to a source address

  header.SetDestinationAddress (dst);
  route = ipv6->GetRoutingProtocol ()->RouteOutput (packet, header, oif, err);

  if (route != 0)
    {
      tag.SetTtl (ttl);
      packet->AddPacketTag (tag);
      Ipv6Address src = route->GetSource ();
NS_LOG_FUNCTION ("Lura1" << src << "    " << dst);
/*
if (src==Ipv6Address("fe80::200:ff:fe00:2") || src==Ipv6Address("fe80::200:ff:fe00:4"))
src=Ipv6Address("3001:db80::200:ff:fe00:5");

else if(src==Ipv6Address("::") && (count==0||count==1)){
src=Ipv6Address("8888:56ac::200:ff:fe00:9");
packet->RemovePacketTag(tag);
count++;
}

else if(src==Ipv6Address("::") && count==2){
src=Ipv6Address("9999:db80::200:ff:fe00:9");
packet->RemovePacketTag(tag);
}


*/

      ipv6->Send (packet, src, dst, 135, route);
NS_LOG_LOGIC ("route found and send hmipv6 message");
    }
  else
    {
      NS_LOG_LOGIC ("no route.. drop mipv6 message");
    }
}

uint8_t mipv6Agent::HandleBU (Ptr<Packet> packet, const Ipv6Address &src, const Ipv6Address &dst, Ptr<Ipv6Interface> interface)
{
  NS_LOG_FUNCTION ( this << src << dst );
  
  NS_LOG_WARN ("No handler for BU message");

  return 0;
}

uint8_t mipv6Agent::HandleBA (Ptr<Packet> packet, const Ipv6Address &src, const Ipv6Address &dst, Ptr<Ipv6Interface> interface)
{
  NS_LOG_FUNCTION ( this << src << dst );
  
  NS_LOG_WARN ("No handler for BA message");
  
  return 0;
}



uint8_t mipv6Agent::HandleHoTI (Ptr<Packet> packet, const Ipv6Address &src, const Ipv6Address &dst, Ptr<Ipv6Interface> interface)
{
  NS_LOG_FUNCTION ( this << src << dst );
  
  NS_LOG_WARN ("No handler for HoTI message");

  return 0;
}

uint8_t mipv6Agent::HandleCoTI (Ptr<Packet> packet, const Ipv6Address &src, const Ipv6Address &dst, Ptr<Ipv6Interface> interface)
{
  NS_LOG_FUNCTION ( this << src << dst );
  
  NS_LOG_WARN ("No handler for CoTI message");
  
  return 0;
}
uint8_t mipv6Agent::HandleHoT (Ptr<Packet> packet, const Ipv6Address &src, const Ipv6Address &dst, Ptr<Ipv6Interface> interface)
{
  NS_LOG_FUNCTION ( this << src << dst );
  
  NS_LOG_WARN ("No handler for HoT message");

  return 0;
}

uint8_t mipv6Agent::HandleCoT (Ptr<Packet> packet, const Ipv6Address &src, const Ipv6Address &dst, Ptr<Ipv6Interface> interface)
{
  NS_LOG_FUNCTION ( this << src << dst );
  
  NS_LOG_WARN ("No handler for CoT message");
  
  return 0;
}


} /* namespace ns3 */

