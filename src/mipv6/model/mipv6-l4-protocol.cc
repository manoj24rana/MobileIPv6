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
#include "ns3/ipv6-route.h"
#include "ns3/ipv6-l3-protocol.h"
#include "ns3/ipv6-interface.h"
#include "ns3/object-base.h"
#include "ns3/header.h"
#include "ns3/chunk.h"
#include "ns3/type-id.h"
#include "mipv6-mobility.h"
#include "mipv6-option.h"
#include "mipv6-header.h"
#include "mipv6-demux.h"
#include "mipv6-option-demux.h"
#include "mipv6-l4-protocol.h"
#include "ns3/ip-l4-protocol.h"

using namespace std;

NS_LOG_COMPONENT_DEFINE ("Mipv6L4Protocol");

namespace ns3 {

NS_OBJECT_ENSURE_REGISTERED (Mipv6L4Protocol);

const uint8_t Mipv6L4Protocol::PROT_NUMBER = 135;

const double Mipv6L4Protocol::MAX_BINDING_LIFETIME = (int)0xffff << 2;

const double Mipv6L4Protocol::INITIAL_BINDING_ACK_TIMEOUT_FIRSTREG = 1.5;

const double Mipv6L4Protocol::INITIAL_BINDING_ACK_TIMEOUT_REREG = 1.0;

const uint8_t Mipv6L4Protocol::MAX_BINDING_UPDATE_RETRY_COUNT = 3;

const uint8_t Mipv6L4Protocol::MAX_HOTI_RETRY_COUNT = 3;

const uint8_t Mipv6L4Protocol::MAX_COTI_RETRY_COUNT = 3;

const uint32_t Mipv6L4Protocol::MIN_DELAY_BEFORE_BCE_DELETE = 10000;

const uint32_t Mipv6L4Protocol::MIN_DELAY_BEFORE_NEW_BCE_ASSIGN = 1500;

const uint32_t Mipv6L4Protocol::TIMESTAMP_VALIDITY_WINDOW = 300;

TypeId Mipv6L4Protocol::GetTypeId ()
{
  static TypeId tid = TypeId ("ns3::Mipv6L4Protocol")
    .SetParent<IpL4Protocol> ()
    .AddConstructor<Mipv6L4Protocol> ();
  return tid;
}

Mipv6L4Protocol::Mipv6L4Protocol ()
  : m_node (0)
{
  NS_LOG_FUNCTION_NOARGS ();
}

Mipv6L4Protocol::~Mipv6L4Protocol ()
{
  NS_LOG_FUNCTION_NOARGS ();
}

void Mipv6L4Protocol::DoDispose ()
{
  m_node = 0;
  IpL4Protocol::DoDispose ();
}

void Mipv6L4Protocol::NotifyNewAggregate ()
{
  NS_LOG_FUNCTION (this);
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

void Mipv6L4Protocol::SetNode (Ptr<Node> node)
{
  NS_LOG_FUNCTION (this << node);
  m_node = node;
}

Ptr<Node> Mipv6L4Protocol::GetNode (void)
{
  NS_LOG_FUNCTION (this);
  return m_node;
}

int Mipv6L4Protocol::GetProtocolNumber () const
{
  NS_LOG_FUNCTION (this);
  return PROT_NUMBER;
}

int Mipv6L4Protocol::GetVersion () const
{
  NS_LOG_FUNCTION (this);
  return 1;
}

void Mipv6L4Protocol::SendMessage (Ptr<Packet> packet, Ipv6Address src, Ipv6Address dst, uint8_t ttl)
{
  NS_LOG_FUNCTION (this << packet << src << dst << (uint32_t)ttl);
  Ptr<Ipv6L3Protocol> ipv6 = m_node->GetObject<Ipv6L3Protocol> ();
  SocketIpTtlTag tag;
  NS_ASSERT (ipv6 != 0);

  tag.SetTtl (ttl);
  packet->AddPacketTag (tag);
  ipv6->Send (packet, src, dst, PROT_NUMBER, 0);
}

enum IpL4Protocol::RxStatus Mipv6L4Protocol::Receive (Ptr<Packet> packet, Ipv6Address const &src, Ipv6Address const &dst, Ptr<Ipv6Interface> interface)
{
  NS_LOG_FUNCTION (this << packet << src << dst << interface << "VANCH");
  Ptr<Packet> p = packet->Copy ();
  Ptr<Mipv6Demux> ipv6MobilityDemux = GetObject<Mipv6Demux>();
  Ptr<Mipv6Mobility> ipv6Mobility = 0;
  Mipv6Header mh;

  p->PeekHeader (mh);

  ipv6Mobility = ipv6MobilityDemux->GetMobility ( mh.GetMhType () );

  if (ipv6Mobility)
    {
      ipv6Mobility->Process (p, src, dst, interface);
    }
  else
    {
      NS_LOG_FUNCTION ( "Mobility Packet with Unknown MhType (" << (uint32_t)mh.GetMhType () << ")" );
    }

  return IpL4Protocol::RX_OK;
}

enum IpL4Protocol::RxStatus Mipv6L4Protocol::Receive (Ptr<Packet> p, Ipv6Header const &header, Ptr<Ipv6Interface> incomingInterface)
{

  NS_LOG_FUNCTION (this << p << header << incomingInterface << "VAMCH");
  Ptr<Packet> packet = p->Copy ();
  Ptr<Mipv6Demux> ipv6MobilityDemux = GetObject<Mipv6Demux>();
  Ptr<Mipv6Mobility> ipv6Mobility = 0;
  Mipv6Header mh;

  packet->PeekHeader (mh);
  ipv6Mobility = ipv6MobilityDemux->GetMobility ( mh.GetMhType () );
  if (ipv6Mobility)
    {
      Ipv6Address src = header.GetSourceAddress ();
      Ipv6Address dst = header.GetDestinationAddress ();

      ipv6Mobility->Process (packet, src, dst, incomingInterface);
    }
  else
    {
      NS_LOG_FUNCTION ( "Mobility Packet with Unknown MhType (" << (uint32_t)mh.GetMhType () << ")" );
    }

  return IpL4Protocol::RX_OK;

}

enum IpL4Protocol::RxStatus Mipv6L4Protocol::Receive (Ptr<Packet> p, Ipv4Header const &header, Ptr<Ipv4Interface> incomingInterface)
{


  return IpL4Protocol::RX_OK;

}


void Mipv6L4Protocol::RegisterMobility ()
{
  Ptr<Mipv6Demux> ipv6MobilityDemux = CreateObject<Mipv6Demux>();
  ipv6MobilityDemux->SetNode ( m_node );

  m_node->AggregateObject ( ipv6MobilityDemux );

  Ptr<Ipv6MobilityBindingUpdate> bu = CreateObject<Ipv6MobilityBindingUpdate>();
  bu->SetNode (m_node);
  ipv6MobilityDemux->Insert (bu);

  Ptr<Ipv6MobilityBindingAck> ba = CreateObject<Ipv6MobilityBindingAck>();
  ba->SetNode (m_node);
  ipv6MobilityDemux->Insert (ba);

  Ptr<Ipv6MobilityHoTI> hoti = CreateObject<Ipv6MobilityHoTI>();
  hoti->SetNode (m_node);
  ipv6MobilityDemux->Insert (hoti);

  Ptr<Ipv6MobilityCoTI> coti = CreateObject<Ipv6MobilityCoTI>();
  coti->SetNode (m_node);
  ipv6MobilityDemux->Insert (coti);

  Ptr<Ipv6MobilityHoT> hot = CreateObject<Ipv6MobilityHoT>();
  hot->SetNode (m_node);
  ipv6MobilityDemux->Insert (hot);

  Ptr<Ipv6MobilityCoT> cot = CreateObject<Ipv6MobilityCoT>();
  cot->SetNode (m_node);
  ipv6MobilityDemux->Insert (cot);

}
void Mipv6L4Protocol::SetDownTarget (IpL4Protocol::DownTargetCallback cb)
{
}
void Mipv6L4Protocol::SetDownTarget6 (IpL4Protocol::DownTargetCallback6 cb)
{
}
IpL4Protocol::DownTargetCallback Mipv6L4Protocol::GetDownTarget (void) const
{
  IpL4Protocol::DownTargetCallback t;
  return t;
}
IpL4Protocol::DownTargetCallback6 Mipv6L4Protocol::GetDownTarget6 (void) const
{
  IpL4Protocol::DownTargetCallback6 y;
  return y;
}

void Mipv6L4Protocol::RegisterMobilityOptions ()
{
  Ptr<Mipv6OptionDemux> ipv6MobilityOptionDemux = CreateObject<Mipv6OptionDemux>();
  ipv6MobilityOptionDemux->SetNode ( m_node );

  m_node->AggregateObject ( ipv6MobilityOptionDemux );

  Ptr<Ipv6MobilityOptionPad1> pad1 = CreateObject<Ipv6MobilityOptionPad1>();
  pad1->SetNode (m_node);
  ipv6MobilityOptionDemux->Insert (pad1);

  Ptr<Ipv6MobilityOptionPadn> padn = CreateObject<Ipv6MobilityOptionPadn>();
  padn->SetNode (m_node);
  ipv6MobilityOptionDemux->Insert (padn);

  //for MIPv6
  Ptr<Ipv6MobilityOptionBindingRefreshAdvice> adv = CreateObject<Ipv6MobilityOptionBindingRefreshAdvice>();
  adv->SetNode (m_node);
  ipv6MobilityOptionDemux->Insert (adv);

  Ptr<Ipv6MobilityOptionAlternateCareofAddress> acoa = CreateObject<Ipv6MobilityOptionAlternateCareofAddress>();
  acoa->SetNode (m_node);
  ipv6MobilityOptionDemux->Insert (acoa);

  Ptr<Ipv6MobilityOptionNonceIndices> ni = CreateObject<Ipv6MobilityOptionNonceIndices>();
  ni->SetNode (m_node);
  ipv6MobilityOptionDemux->Insert (ni);

  Ptr<Ipv6MobilityOptionBindingAuthorizationData> auth = CreateObject<Ipv6MobilityOptionBindingAuthorizationData>();
  auth->SetNode (m_node);
  ipv6MobilityOptionDemux->Insert (auth);

}

} /* namespace ns3 */

