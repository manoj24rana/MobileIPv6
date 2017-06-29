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
#include "ns3/node.h"
#include "ns3/mipv6-l4-protocol.h"
#include "ns3/mipv6-header.h"
#include "ns3/mipv6-mobility.h"
#include "ns3/mipv6-mn.h"
#include "ns3/mipv6-ha.h"
#include "ns3/mipv6-cn.h"
#include "ns3/mipv6-tun-l4-protocol.h"
#include "mipv6-helper.h"

NS_LOG_COMPONENT_DEFINE ("MIPv6Helper");

namespace ns3 {

//HA Helper

mipv6HAHelper::mipv6HAHelper()
{
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

      mipv6->RegisterMobilityOptions();
    }
	
  Ptr<Ipv6TunnelL4Protocol> ip6tunnel = node->GetObject<Ipv6TunnelL4Protocol>();
  
  if ( ip6tunnel == 0 )
    {
	  ip6tunnel = CreateObject<Ipv6TunnelL4Protocol>();
	  
	  node->AggregateObject (ip6tunnel);
    }

  Ptr<mipv6MN> mn = node->GetObject<mipv6MN>();
  if (mn)
    NS_ASSERT_MSG( !mn, "MN stack is installed on HA, not allowed");
    
  Ptr<mipv6HA> ha = CreateObject<mipv6HA>();
 	
  node->AggregateObject(ha);
}

std::list<Ipv6Address> mipv6HAHelper::GetHomeAgentAddressList()
{
  Ptr<mipv6HA> ha = m_node->GetObject<mipv6HA>();
  NS_ASSERT (ha != 0);  // must install mipv6ha before
  return ha->HomeAgentAddressList();
}

//MN Helper

mipv6MNHelper::mipv6MNHelper(std::list<Ipv6Address> haalist, bool rotopt)
:m_Haalist(haalist),  //home agent list
m_rotopt(rotopt)     //must be false in this implementation (set as false by default)
{
m_rotopt=false;
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

      mipv6->RegisterMobilityOptions();
    }

  Ptr<Ipv6TunnelL4Protocol> ip6tunnel = node->GetObject<Ipv6TunnelL4Protocol>();
  
  if ( ip6tunnel == 0 )
    {
      ip6tunnel = CreateObject<Ipv6TunnelL4Protocol>();
	  
      node->AggregateObject (ip6tunnel);
    }

  Ptr<mipv6HA> ha = node->GetObject<mipv6HA>();
  if (ha)
    NS_ASSERT_MSG( !ha, "HA stack is installed on MN, not allowed");


  Ptr<mipv6MN> mn = CreateObject<mipv6MN>(m_Haalist);  // Pass Home Agent Address List as argument

  mn->SetRouteOptimizationReuiredField(m_rotopt);  //Set by default false as the current implementation does
                                                //not support route optimization, otherwise set to m_rotopt
  node->AggregateObject(mn);
}

//CN Helper

mipv6CNHelper::mipv6CNHelper()
{
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

      mipv6->RegisterMobilityOptions();
    }
	

  
  Ptr<mipv6CN> cn = CreateObject<mipv6CN>();
 	
  node->AggregateObject(cn);
}


} // namespace ns3
