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

#include "ns3/assert.h"
#include "ns3/log.h"
#include "ns3/object.h"
#include "ns3/names.h"
#include "ns3/ipv4.h"
#include "ns3/ipv6.h"
#include "ns3/packet-socket-factory.h"
#include "ns3/config.h"
#include "ns3/simulator.h"
#include "ns3/string.h"
#include "ns3/net-device.h"
#include "ns3/callback.h"
#include "ns3/node.h"
#include "ns3/core-config.h"
#include "ns3/arp-l3-protocol.h"
#include "ns3/mipv6-l4-protocol.h"
#include "ns3/mipv6-header.h"
#include "ns3/mipv6-mobility.h"
#include "ns3/mipv6-mn.h"
#include "ns3/mipv6-ha.h"
#include "ns3/mipv6-cn.h"
#include "ns3/identifier.h"
#include "ns3/mipv6-tun-l4-protocol.h"
//#include "ns3/ipv6-static-source-routing.h"

#include "ns3/ipv6-list-routing.h"

#include "mipv6-helper.h"
#include <limits>
#include <map>

NS_LOG_COMPONENT_DEFINE ("MIPv6Helper");

namespace ns3 {

mipv6HAHelper::mipv6HAHelper()
  
{
//NS_ASSERT_MSG("MIPv6Helper HA constructor");
}

mipv6HAHelper::~mipv6HAHelper()
{
}

void
mipv6HAHelper::Install (Ptr<Node> node) 
{

  m_node=node;
  Ptr<MIPv6L4Protocol> mipv6 = node->GetObject<MIPv6L4Protocol>();

  if(mipv6 == 0)
    {
      mipv6 = CreateObject<MIPv6L4Protocol>();
  
      node->AggregateObject(mipv6);
  
	  mipv6 = node->GetObject<MIPv6L4Protocol>();
  
      mipv6->RegisterMobility();
      
	}
	
  Ptr<Ipv6TunnelL4Protocol> ip6tunnel = node->GetObject<Ipv6TunnelL4Protocol>();
  
  if ( ip6tunnel == 0 )
    {
	  ip6tunnel = CreateObject<Ipv6TunnelL4Protocol>();
	  
	  node->AggregateObject (ip6tunnel);
	}

  
  Ptr<mipv6HA> ha = CreateObject<mipv6HA>();
 	
  node->AggregateObject(ha);
}

std::list<Ipv6Address> mipv6HAHelper::GetHomeAgentAddressList()
{
Ptr<mipv6HA> ha = m_node->GetObject<mipv6HA>();
return ha->HomeAgentAddressList();
}

mipv6MNHelper::mipv6MNHelper(std::list<Ipv6Address> haalist, bool rotopt)
:m_Haalist(haalist),
m_rotopt(rotopt)
{

}

mipv6MNHelper::~mipv6MNHelper()
{
}

void
mipv6MNHelper::Install (Ptr<Node> node) const
{
  Ptr<MIPv6L4Protocol> mipv6 = node->GetObject<MIPv6L4Protocol>();

  if(mipv6 == 0)
    {
      mipv6 = CreateObject<MIPv6L4Protocol>();
  
      node->AggregateObject(mipv6);
	  
	  mipv6 = node->GetObject<MIPv6L4Protocol>();
  
      mipv6->RegisterMobility();
      
	}

Ptr<Ipv6TunnelL4Protocol> ip6tunnel = node->GetObject<Ipv6TunnelL4Protocol>();
  
  if ( ip6tunnel == 0 )
    {
	  ip6tunnel = CreateObject<Ipv6TunnelL4Protocol>();
	  
	  node->AggregateObject (ip6tunnel);
	}


  Ptr<mipv6MN> mn = CreateObject<mipv6MN>(m_Haalist);
  mn->SetRouteOptimizationReuiredField(m_rotopt);
  
  //Attach static source routing
/*
  Ptr<Ipv6> ipv6 = node->GetObject<Ipv6>();
  
  NS_ASSERT_MSG(ipv6, "Install Internet-stack first before installing MIPv6-related agents");
  
  Ptr<Ipv6RoutingProtocol> routingProtocol = ipv6->GetRoutingProtocol();

  NS_ASSERT_MSG( routingProtocol, "MIPv6 needs Ipv6-routing protocol for operation");

  Ptr<Ipv6ListRouting> listRouting = DynamicCast<Ipv6ListRouting>(routingProtocol);
  
  NS_ASSERT_MSG( listRouting, "MIPv6 needs Ipv6-list-routing protocol for operation");
  
  Ptr<Ipv6StaticSourceRouting> sourceRouting = CreateObject<Ipv6StaticSourceRouting>();
  
  listRouting->AddRoutingProtocol(sourceRouting, 10); //higher priority than static routing
*/



//NS_ASSERT_MSG("MAG HELPER INSTALL METHOD");	
  node->AggregateObject(mn);
}
//CN
mipv6CNHelper::mipv6CNHelper()
  
{
//NS_ASSERT_MSG("MIPv6Helper HA constructor");
}

mipv6CNHelper::~mipv6CNHelper()
{
}

void
mipv6CNHelper::Install (Ptr<Node> node) const
{
  Ptr<MIPv6L4Protocol> mipv6 = node->GetObject<MIPv6L4Protocol>();

  if(mipv6 == 0)
    {
      mipv6 = CreateObject<MIPv6L4Protocol>();
  
      node->AggregateObject(mipv6);
  
	  mipv6 = node->GetObject<MIPv6L4Protocol>();
  
      mipv6->RegisterMobility();
      
	}
	

  
  Ptr<mipv6CN> cn = CreateObject<mipv6CN>();
 	
  node->AggregateObject(cn);
}


} // namespace ns3
