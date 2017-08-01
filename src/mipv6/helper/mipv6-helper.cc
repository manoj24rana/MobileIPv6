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

Mipv6HaHelper::Mipv6HaHelper ()
{
}

Mipv6HaHelper::~Mipv6HaHelper ()
{
}

void
Mipv6HaHelper::Install (Ptr<Node> node)
{
  m_node = node;
  Ptr<Mipv6L4Protocol> mipv6 = node->GetObject<Mipv6L4Protocol> ();

  if (mipv6 == 0)
    {
      mipv6 = CreateObject<Mipv6L4Protocol> ();
  
      node->AggregateObject (mipv6);
  
      mipv6 = node->GetObject<Mipv6L4Protocol> ();
  
      mipv6->RegisterMobility();

      mipv6->RegisterMobilityOptions();
    }

  Ptr<Ipv6TunnelL4Protocol> ip6tunnel = node->GetObject<Ipv6TunnelL4Protocol> ();
  
  if ( ip6tunnel == 0 )
    {
      ip6tunnel = CreateObject<Ipv6TunnelL4Protocol> ();
	  
      node->AggregateObject (ip6tunnel);
    }

  Ptr<Mipv6Mn> mn = node->GetObject<Mipv6Mn> ();
  if (mn)
    {

      NS_ASSERT_MSG( !mn, "MN stack is installed on HA, not allowed");
  
    }  
  Ptr<Mipv6Ha> ha = CreateObject<Mipv6Ha> ();
 	
  node->AggregateObject (ha);
}

std::list<Ipv6Address> Mipv6HaHelper::GetHomeAgentAddressList ()
{
  Ptr<Mipv6Ha> ha = m_node->GetObject<Mipv6Ha> ();
  NS_ASSERT (ha != 0);  // must install mipv6ha before
  return ha->HomeAgentAddressList ();
}

//MN Helper

Mipv6MnHelper::Mipv6MnHelper (std::list<Ipv6Address> haalist, bool rotopt)
 : m_Haalist (haalist),  //home agent list
 m_rotopt (rotopt)     //must be false in this implementation (set as false by default)
{
 m_rotopt = false;
}

Mipv6MnHelper::~Mipv6MnHelper ()
{
}

void
Mipv6MnHelper::Install (Ptr<Node> node) const
{
  Ptr<Mipv6L4Protocol> mipv6 = node->GetObject<Mipv6L4Protocol> ();

  if (mipv6 == 0)
    {
      mipv6 = CreateObject<Mipv6L4Protocol> ();
  
      node->AggregateObject (mipv6);
	  
      mipv6 = node->GetObject<Mipv6L4Protocol> ();
  
      mipv6->RegisterMobility ();

      mipv6->RegisterMobilityOptions ();
    }

  Ptr<Ipv6TunnelL4Protocol> ip6tunnel = node->GetObject<Ipv6TunnelL4Protocol> ();
  
  if ( ip6tunnel == 0 )
    {
      ip6tunnel = CreateObject<Ipv6TunnelL4Protocol> ();
	  
      node->AggregateObject (ip6tunnel);
    }

  Ptr<Mipv6Ha> ha = node->GetObject<Mipv6Ha> ();
  if (ha)
    {
 
      NS_ASSERT_MSG ( !ha, "HA stack is installed on MN, not allowed");

    }

  Ptr<Mipv6Mn> mn = CreateObject<Mipv6Mn> (m_Haalist);  // Pass Home Agent Address List as argument

  mn->SetRouteOptimizationReuiredField (m_rotopt);  //Set by default false as the current implementation does
                                                //not support route optimization, otherwise set to m_rotopt
  node->AggregateObject (mn);
}

//CN Helper

Mipv6CnHelper::Mipv6CnHelper ()
{
}

Mipv6CnHelper::~Mipv6CnHelper ()
{
}

void
Mipv6CnHelper::Install (Ptr<Node> node) const
{
  Ptr<Mipv6L4Protocol> mipv6 = node->GetObject<Mipv6L4Protocol> ();

  if (mipv6 == 0)
    {
      mipv6 = CreateObject<Mipv6L4Protocol> ();
  
      node->AggregateObject (mipv6);
  
      mipv6 = node->GetObject<Mipv6L4Protocol> ();
  
      mipv6->RegisterMobility ();

      mipv6->RegisterMobilityOptions ();
    }
	

  
  Ptr<Mipv6CN> cn = CreateObject<Mipv6CN> ();
 	
  node->AggregateObject (cn);
}


} // namespace ns3
