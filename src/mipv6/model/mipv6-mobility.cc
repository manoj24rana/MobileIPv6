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
#include "ns3/uinteger.h"
#include "ns3/simulator.h"
#include "mipv6-mobility.h"
#include "mipv6-option.h"
#include "mipv6-demux.h"
#include "mipv6-option-demux.h"
#include "mipv6-header.h"
#include "mipv6-agent.h"

NS_LOG_COMPONENT_DEFINE ("Mipv6Mobility");

namespace ns3 {

NS_OBJECT_ENSURE_REGISTERED (Mipv6Mobility);

TypeId Mipv6Mobility::GetTypeId ()
{
  static TypeId tid = TypeId ("ns3::Mipv6Mobility")
    .SetParent<Object>()
    .AddAttribute ("MobilityNumber", "The IPv6 mobility number.",
                   UintegerValue (0),
                   MakeUintegerAccessor (&Mipv6Mobility::GetMobilityNumber),
                   MakeUintegerChecker<uint8_t> ())
  ;
  return tid;
}

Mipv6Mobility::~Mipv6Mobility ()
{
  NS_LOG_FUNCTION_NOARGS ();
}

void Mipv6Mobility::SetNode (Ptr<Node> node)
{
  NS_LOG_FUNCTION (this << node);
  m_node = node;
}

Ptr<Node> Mipv6Mobility::GetNode () const
{
  NS_LOG_FUNCTION (this);

  return m_node;
}

uint8_t Mipv6Mobility::ProcessOptions (Ptr<Packet> packet, uint8_t offset, uint8_t length, Mipv6OptionBundle &bundle)
{
  NS_LOG_FUNCTION (this << packet << length);
  Ptr<Packet> p = packet->Copy ();
  p->RemoveAtStart (offset);

  Ptr<Mipv6OptionDemux> ipv6MobilityOptionDemux = GetNode ()->GetObject<Mipv6OptionDemux>();
  NS_ASSERT (ipv6MobilityOptionDemux != 0);

  Ptr<Mipv6Option> ipv6MobilityOption = 0;

  uint8_t processedSize = 0;
  uint32_t size = p->GetSize ();
  uint8_t *data = new uint8_t[size];
  p->CopyData (data, size);

  uint8_t optType;
  uint8_t optLen;

  while ( processedSize < length )
    {
      optType = *(data + processedSize);

      ipv6MobilityOption = ipv6MobilityOptionDemux->GetOption ( optType );

      if ( ipv6MobilityOption == 0 )
        {
          if ( optType == 0 )
            {
              optLen = 1;
            }
          else
            {
              optLen = *(data + processedSize + 1) + 2;
            }

          NS_LOG_LOGIC ("No matched Ipv6MobilityOption for type=" << (uint32_t)optType );
        }
      else
        {
          optLen = ipv6MobilityOption->Process (packet, offset + processedSize, bundle);
        }

      processedSize += optLen;
      p->RemoveAtStart (optLen);
    }

  delete [] data;

  return processedSize;
}



NS_OBJECT_ENSURE_REGISTERED (Ipv6MobilityBindingUpdate);

TypeId Ipv6MobilityBindingUpdate::GetTypeId ()
{
  static TypeId tid = TypeId ("ns3::Ipv6MobilityBindingUpdate")
    .SetParent<Mipv6Mobility>()
    .AddConstructor<Ipv6MobilityBindingUpdate>()
  ;
  return tid;
}

Ipv6MobilityBindingUpdate::~Ipv6MobilityBindingUpdate ()
{
  NS_LOG_FUNCTION_NOARGS ();
}

uint8_t Ipv6MobilityBindingUpdate::GetMobilityNumber () const
{
  return MOB_NUMBER;
}

uint8_t Ipv6MobilityBindingUpdate::Process (Ptr<Packet> p, Ipv6Address src, Ipv6Address dst, Ptr<Ipv6Interface> interface)
{
  NS_LOG_FUNCTION (this << p << src << dst << interface);

  Ptr<Packet> packet = p->Copy ();

  Ipv6MobilityBindingUpdateHeader buh;
  Mipv6OptionBundle bundle;
  /*  Mobile Ipv6 process routine */
  packet->RemoveHeader (buh);

  Ptr<Mipv6Agent> mip6 = GetNode ()->GetObject<Mipv6Agent>();

  if ( mip6 )
    {

      Simulator::ScheduleNow ( &Mipv6Agent::Receive, mip6, p, src, dst, interface);
      NS_LOG_FUNCTION ( this << packet << src << dst << interface << "PROCESS BU" );
      return 0;
    }


  Ptr<Mipv6Demux> ipv6MobilityDemux = GetNode ()->GetObject<Mipv6Demux>();
  NS_ASSERT ( ipv6MobilityDemux );

  Ptr<Mipv6Mobility> ipv6Mobility = ipv6MobilityDemux->GetMobility (buh.GetMhType ());
  NS_ASSERT ( ipv6Mobility );

  uint8_t length = ((buh.GetHeaderLen () + 1 ) << 3) - buh.GetOptionsOffset ();

  ipv6Mobility->ProcessOptions ( packet, buh.GetOptionsOffset (), length, bundle);


  NS_LOG_LOGIC (" No Handler for Binding Update");

  return 0;
}

NS_OBJECT_ENSURE_REGISTERED (Ipv6MobilityBindingAck);

TypeId Ipv6MobilityBindingAck::GetTypeId ()
{
  static TypeId tid = TypeId ("ns3::Ipv6MobilityBindingAck")
    .SetParent<Mipv6Mobility>()
    .AddConstructor<Ipv6MobilityBindingAck>()
  ;
  return tid;
}

Ipv6MobilityBindingAck::~Ipv6MobilityBindingAck ()
{
  NS_LOG_FUNCTION_NOARGS ();
}

uint8_t Ipv6MobilityBindingAck::GetMobilityNumber () const
{
  return MOB_NUMBER;
}

uint8_t Ipv6MobilityBindingAck::Process (Ptr<Packet> p, Ipv6Address src, Ipv6Address dst, Ptr<Ipv6Interface> interface)
{
  NS_LOG_FUNCTION (this << p << src << dst << interface);

  Ptr<Packet> packet = p->Copy ();

  Ipv6MobilityBindingAckHeader bah;
  Mipv6OptionBundle bundle;
  /* Mobile Ipv6 process routine */
  packet->PeekHeader (bah);

  Ptr<Mipv6Agent> mip6 = GetNode ()->GetObject<Mipv6Agent>();

  if ( mip6 )
    {
      NS_LOG_FUNCTION ( this << packet << src << dst << interface << "PROCESS BACK" );
      Simulator::ScheduleNow ( &Mipv6Agent::Receive, mip6, p, src, dst, interface);

      return 0;
    }


  Ptr<Mipv6Demux> ipv6MobilityDemux = GetNode ()->GetObject<Mipv6Demux>();
  NS_ASSERT ( ipv6MobilityDemux );

  Ptr<Mipv6Mobility> ipv6Mobility = ipv6MobilityDemux->GetMobility (bah.GetMhType ());
  NS_ASSERT ( ipv6Mobility );

  uint8_t length = ((bah.GetHeaderLen () + 1 ) << 3) - bah.GetOptionsOffset ();
  ipv6Mobility->ProcessOptions ( packet, bah.GetOptionsOffset (), length, bundle);

  NS_LOG_LOGIC (" No Handler for Binding Ack");

  return 0;
}

NS_OBJECT_ENSURE_REGISTERED (Ipv6MobilityHoTI);

TypeId Ipv6MobilityHoTI::GetTypeId ()
{
  static TypeId tid = TypeId ("ns3::Ipv6MobilityHoTI")
    .SetParent<Mipv6Mobility>()
    .AddConstructor<Ipv6MobilityHoTI>()
  ;
  return tid;
}

Ipv6MobilityHoTI::~Ipv6MobilityHoTI ()
{
  NS_LOG_FUNCTION_NOARGS ();
}

uint8_t Ipv6MobilityHoTI::GetMobilityNumber () const
{
  return MOB_NUMBER;
}

uint8_t Ipv6MobilityHoTI::Process (Ptr<Packet> p, Ipv6Address src, Ipv6Address dst, Ptr<Ipv6Interface> interface)
{
  NS_LOG_FUNCTION (this << p << src << dst << interface);

  Ptr<Packet> packet = p->Copy ();


  Ptr<Mipv6Agent> mip6 = GetNode ()->GetObject<Mipv6Agent>();

  if ( mip6 )
    {
      NS_LOG_FUNCTION ( this << packet << src << dst << interface << "PROCESS HoTI" );
      Simulator::ScheduleNow ( &Mipv6Agent::Receive, mip6, p, src, dst, interface);

      return 0;
    }



  NS_LOG_LOGIC (" No Handler for HoTI");

  return 0;
}

NS_OBJECT_ENSURE_REGISTERED (Ipv6MobilityCoTI);

TypeId Ipv6MobilityCoTI::GetTypeId ()
{
  static TypeId tid = TypeId ("ns3::Ipv6MobilityCoTI")
    .SetParent<Mipv6Mobility>()
    .AddConstructor<Ipv6MobilityCoTI>()
  ;
  return tid;
}

Ipv6MobilityCoTI::~Ipv6MobilityCoTI ()
{
  NS_LOG_FUNCTION_NOARGS ();
}

uint8_t Ipv6MobilityCoTI::GetMobilityNumber () const
{
  return MOB_NUMBER;
}

uint8_t Ipv6MobilityCoTI::Process (Ptr<Packet> p, Ipv6Address src, Ipv6Address dst, Ptr<Ipv6Interface> interface)
{
  NS_LOG_FUNCTION (this << p << src << dst << interface);

  Ptr<Packet> packet = p->Copy ();


  Ptr<Mipv6Agent> mip6 = GetNode ()->GetObject<Mipv6Agent>();

  if ( mip6 )
    {
      NS_LOG_FUNCTION ( this << packet << src << dst << interface << "PROCESS CoTI" );
      Simulator::ScheduleNow ( &Mipv6Agent::Receive, mip6, p, src, dst, interface);

      return 0;
    }



  NS_LOG_LOGIC (" No Handler for CoTI");

  return 0;
}

NS_OBJECT_ENSURE_REGISTERED (Ipv6MobilityHoT);

TypeId Ipv6MobilityHoT::GetTypeId ()
{
  static TypeId tid = TypeId ("ns3::Ipv6MobilityHoT")
    .SetParent<Mipv6Mobility>()
    .AddConstructor<Ipv6MobilityHoT>()
  ;
  return tid;
}

Ipv6MobilityHoT::~Ipv6MobilityHoT ()
{
  NS_LOG_FUNCTION_NOARGS ();
}

uint8_t Ipv6MobilityHoT::GetMobilityNumber () const
{
  return MOB_NUMBER;
}

uint8_t Ipv6MobilityHoT::Process (Ptr<Packet> p, Ipv6Address src, Ipv6Address dst, Ptr<Ipv6Interface> interface)
{
  NS_LOG_FUNCTION (this << p << src << dst << interface);

  Ptr<Packet> packet = p->Copy ();


  Ptr<Mipv6Agent> mip6 = GetNode ()->GetObject<Mipv6Agent>();

  if ( mip6 )
    {
      NS_LOG_FUNCTION ( this << packet << src << dst << interface << "PROCESS HoT" );
      Simulator::ScheduleNow ( &Mipv6Agent::Receive, mip6, p, src, dst, interface);

      return 0;
    }



  NS_LOG_LOGIC (" No Handler for HoT");

  return 0;
}

NS_OBJECT_ENSURE_REGISTERED (Ipv6MobilityCoT);

TypeId Ipv6MobilityCoT::GetTypeId ()
{
  static TypeId tid = TypeId ("ns3::Ipv6MobilityCoT")
    .SetParent<Mipv6Mobility>()
    .AddConstructor<Ipv6MobilityCoT>()
  ;
  return tid;
}

Ipv6MobilityCoT::~Ipv6MobilityCoT ()
{
  NS_LOG_FUNCTION_NOARGS ();
}

uint8_t Ipv6MobilityCoT::GetMobilityNumber () const
{
  return MOB_NUMBER;
}

uint8_t Ipv6MobilityCoT::Process (Ptr<Packet> p, Ipv6Address src, Ipv6Address dst, Ptr<Ipv6Interface> interface)
{
  NS_LOG_FUNCTION (this << p << src << dst << interface);

  Ptr<Packet> packet = p->Copy ();


  Ptr<Mipv6Agent> mip6 = GetNode ()->GetObject<Mipv6Agent>();

  if ( mip6 )
    {
      NS_LOG_FUNCTION ( this << packet << src << dst << interface << "PROCESS CoTI" );
      Simulator::ScheduleNow ( &Mipv6Agent::Receive, mip6, p, src, dst, interface);

      return 0;
    }



  NS_LOG_LOGIC (" No Handler for CoT");

  return 0;
}

NS_OBJECT_ENSURE_REGISTERED (Ipv6BindingRefreshRequest);

TypeId Ipv6BindingRefreshRequest::GetTypeId ()
{
  static TypeId tid = TypeId ("ns3::Ipv6BindingRefreshRequest")
    .SetParent<Mipv6Mobility>()
    .AddConstructor<Ipv6BindingRefreshRequest>()
  ;
  return tid;
}

Ipv6BindingRefreshRequest::~Ipv6BindingRefreshRequest ()
{
  NS_LOG_FUNCTION_NOARGS ();
}

uint8_t Ipv6BindingRefreshRequest::GetMobilityNumber () const
{
  return MOB_NUMBER;
}

uint8_t Ipv6BindingRefreshRequest::Process (Ptr<Packet> p, Ipv6Address src, Ipv6Address dst, Ptr<Ipv6Interface> interface)
{
  NS_LOG_FUNCTION (this << p << src << dst << interface);

  Ptr<Packet> packet = p->Copy ();


  Ptr<Mipv6Agent> mip6 = GetNode ()->GetObject<Mipv6Agent>();

  if ( mip6 )
    {
      NS_LOG_FUNCTION ( this << packet << src << dst << interface << "PROCESS BRR" );
      Simulator::ScheduleNow ( &Mipv6Agent::Receive, mip6, p, src, dst, interface);

      return 0;
    }



  NS_LOG_LOGIC (" No Handler for BRR");

  return 0;
}

NS_OBJECT_ENSURE_REGISTERED (Ipv6BindingError);

TypeId Ipv6BindingError::GetTypeId ()
{
  static TypeId tid = TypeId ("ns3::Ipv6BindingError")
    .SetParent<Mipv6Mobility>()
    .AddConstructor<Ipv6BindingError>()
  ;
  return tid;
}

Ipv6BindingError::~Ipv6BindingError ()
{
  NS_LOG_FUNCTION_NOARGS ();
}

uint8_t Ipv6BindingError::GetMobilityNumber () const
{
  return MOB_NUMBER;
}

uint8_t Ipv6BindingError::Process (Ptr<Packet> p, Ipv6Address src, Ipv6Address dst, Ptr<Ipv6Interface> interface)
{
  NS_LOG_FUNCTION (this << p << src << dst << interface);

  Ptr<Packet> packet = p->Copy ();


  Ptr<Mipv6Agent> mip6 = GetNode ()->GetObject<Mipv6Agent>();

  if ( mip6 )
    {
      NS_LOG_FUNCTION ( this << packet << src << dst << interface << "PROCESS BE" );
      Simulator::ScheduleNow ( &Mipv6Agent::Receive, mip6, p, src, dst, interface);

      return 0;
    }



  NS_LOG_LOGIC (" No Handler for Binding Error");

  return 0;
}


} /* namespace ns3 */
