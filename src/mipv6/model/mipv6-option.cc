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
#include "ns3/address-utils.h"
#include "ns3/log.h"
#include "ns3/uinteger.h"
#include "ns3/identifier.h"
#include "mipv6-option.h"
#include "mipv6-option-header.h"

NS_LOG_COMPONENT_DEFINE ("MIPv6Option");

namespace ns3
{

NS_OBJECT_ENSURE_REGISTERED (MIPv6Option);

TypeId MIPv6Option::GetTypeId ()
{
  static TypeId tid = TypeId ("ns3::MIPv6Option")
    .SetParent<Object>()
	.AddAttribute ("MobilityOptionNumber", "The IPv6 mobility option number.",
	               UintegerValue (0),
				   MakeUintegerAccessor (&MIPv6Option::GetMobilityOptionNumber),
				   MakeUintegerChecker<uint8_t> ())
	;
  return tid;
}

MIPv6Option::~MIPv6Option()
{
  NS_LOG_FUNCTION_NOARGS ();
}

void MIPv6Option::SetNode (Ptr<Node> node)
{
  NS_LOG_FUNCTION (this << node);
  m_node = node;
}

MIPv6OptionBundle::MIPv6OptionBundle()
 : m_hoa("0::0"),
   m_coa("0::0"),
   m_hi(0),
   m_coi(0),
   m_auth(0),
   m_interval(0)
{
  NS_LOG_FUNCTION_NOARGS();
}


Ipv6Address MIPv6OptionBundle::GetHomeAddress() const
{
  NS_LOG_FUNCTION_NOARGS();
  
  return m_hoa;
}

void MIPv6OptionBundle::SetHomeAddress(Ipv6Address hoa)
{
  NS_LOG_FUNCTION ( this << hoa );

  m_hoa = hoa;
}

Ipv6Address MIPv6OptionBundle::GetCareofAddress() const
{
  NS_LOG_FUNCTION_NOARGS();
  
  return m_coa;
}

void MIPv6OptionBundle::SetCareofAddress(Ipv6Address coa)
{
  NS_LOG_FUNCTION ( this << m_coa );
  
  m_coa = coa;
}

uint16_t MIPv6OptionBundle::GetHomeNonceIndex() const
{
  NS_LOG_FUNCTION_NOARGS();
  
  return m_hi;
}

void MIPv6OptionBundle::SetHomeNonceIndex(uint16_t hi)
{
  NS_LOG_FUNCTION_NOARGS();
  
  m_hi = hi;
}

uint16_t MIPv6OptionBundle::GetCareofNonceIndex() const
{

  return m_coi;
}

void MIPv6OptionBundle::SetCareofNonceIndex(uint16_t coi)
{
  
  m_coi=coi;
}

uint64_t MIPv6OptionBundle::GetAuthenticator() const
{
  return m_auth;
}

void MIPv6OptionBundle::SetAuthenticator(uint64_t auth)
{
  
  m_auth=auth;
}

uint16_t MIPv6OptionBundle::GetRefreshInterval() const
{
  return m_interval;
}

void MIPv6OptionBundle::SetRefreshInterval(uint16_t intvl)
{
 m_interval=intvl;
}


NS_OBJECT_ENSURE_REGISTERED (Ipv6MobilityOptionPad1);

TypeId Ipv6MobilityOptionPad1::GetTypeId ()
{
  static TypeId tid = TypeId ("ns3::Ipv6MobilityOptionPad1")
    .SetParent<MIPv6Option>()
	;
  return tid;
}

Ipv6MobilityOptionPad1::~Ipv6MobilityOptionPad1()
{
  NS_LOG_FUNCTION_NOARGS ();
}

uint8_t Ipv6MobilityOptionPad1::GetMobilityOptionNumber () const
{
  NS_LOG_FUNCTION_NOARGS ();
  
  return OPT_NUMBER;
}

uint8_t Ipv6MobilityOptionPad1::Process (Ptr<Packet> packet, uint8_t offset, MIPv6OptionBundle& bundle)
{
  NS_LOG_FUNCTION ( this << packet );
  
  Ptr<Packet> p = packet->Copy();
  
  p->RemoveAtStart(offset);
  
  Ipv6MobilityOptionPad1Header pad1;
  
  p->RemoveHeader(pad1);
  
  return pad1.GetSerializedSize();
}

NS_OBJECT_ENSURE_REGISTERED (Ipv6MobilityOptionPadn);

TypeId Ipv6MobilityOptionPadn::GetTypeId ()
{
  static TypeId tid = TypeId ("ns3::Ipv6MobilityOptionPadn")
    .SetParent<MIPv6Option>()
	;
  return tid;
}

Ipv6MobilityOptionPadn::~Ipv6MobilityOptionPadn()
{
  NS_LOG_FUNCTION_NOARGS ();
}

uint8_t Ipv6MobilityOptionPadn::GetMobilityOptionNumber () const
{
  NS_LOG_FUNCTION_NOARGS ();
  
  return OPT_NUMBER;
}

uint8_t Ipv6MobilityOptionPadn::Process (Ptr<Packet> packet, uint8_t offset, MIPv6OptionBundle& bundle)
{
  NS_LOG_FUNCTION ( this << packet );
  
  Ptr<Packet> p = packet->Copy();
  
  p->RemoveAtStart(offset);

  Ipv6MobilityOptionPadnHeader padn;
  
  p->RemoveHeader(padn);
  
  return padn.GetSerializedSize();
}

NS_OBJECT_ENSURE_REGISTERED (Ipv6MobilityOptionBindingRefreshAdvice);

TypeId Ipv6MobilityOptionBindingRefreshAdvice::GetTypeId ()
{
  static TypeId tid = TypeId ("ns3::Ipv6MobilityOptionBindingRefreshAdvice")
    .SetParent<MIPv6Option>()
	;
  return tid;
}

Ipv6MobilityOptionBindingRefreshAdvice::~Ipv6MobilityOptionBindingRefreshAdvice()
{
  NS_LOG_FUNCTION_NOARGS ();
}

uint8_t Ipv6MobilityOptionBindingRefreshAdvice::GetMobilityOptionNumber () const
{
  NS_LOG_FUNCTION_NOARGS ();
  
  return OPT_NUMBER;
}

uint8_t Ipv6MobilityOptionBindingRefreshAdvice::Process (Ptr<Packet> packet, uint8_t offset, MIPv6OptionBundle& bundle)
{
  NS_LOG_FUNCTION ( this << packet << (uint32_t)offset);
  
  Ptr<Packet> p = packet->Copy();
  
  p->RemoveAtStart(offset);
  
  Ipv6MobilityOptionBindingRefreshAdviceHeader rad;
  
  p->RemoveHeader(rad);
  
  if ( rad.GetType () == 2 ) //Network Address Identifier
    {
	  bundle.SetRefreshInterval(rad.GetRefreshInterval());
	}
  
  return rad.GetSerializedSize();
}

NS_OBJECT_ENSURE_REGISTERED (Ipv6MobilityOptionAlternateCareofAddress);

TypeId Ipv6MobilityOptionAlternateCareofAddress::GetTypeId ()
{
  static TypeId tid = TypeId ("ns3::Ipv6MobilityOptionAlternateCareofAddress")
    .SetParent<MIPv6Option>()
	;
  return tid;
}

Ipv6MobilityOptionAlternateCareofAddress::~Ipv6MobilityOptionAlternateCareofAddress()
{
  NS_LOG_FUNCTION_NOARGS ();
}

uint8_t Ipv6MobilityOptionAlternateCareofAddress::GetMobilityOptionNumber () const
{
  NS_LOG_FUNCTION_NOARGS ();
  
  return OPT_NUMBER;
}

uint8_t Ipv6MobilityOptionAlternateCareofAddress::Process (Ptr<Packet> packet, uint8_t offset, MIPv6OptionBundle& bundle)
{
  NS_LOG_FUNCTION ( this << packet );
  
  Ptr<Packet> p = packet->Copy();
  
  p->RemoveAtStart(offset);
  
  Ipv6MobilityOptionAlternateCareofAddressHeader aca;
  
  p->RemoveHeader(aca);
  
  bundle.SetCareofAddress(aca.GetAlternateCareofAddress());
 
  return aca.GetSerializedSize();
}

NS_OBJECT_ENSURE_REGISTERED (Ipv6MobilityOptionNonceIndices);

TypeId Ipv6MobilityOptionNonceIndices::GetTypeId ()
{
  static TypeId tid = TypeId ("ns3::Ipv6MobilityOptionNonceIndices")
    .SetParent<MIPv6Option>()
	;
  return tid;
}

Ipv6MobilityOptionNonceIndices::~Ipv6MobilityOptionNonceIndices()
{
  NS_LOG_FUNCTION_NOARGS ();
}

uint8_t Ipv6MobilityOptionNonceIndices::GetMobilityOptionNumber () const
{
  NS_LOG_FUNCTION_NOARGS ();
  
  return OPT_NUMBER;
}

uint8_t Ipv6MobilityOptionNonceIndices::Process (Ptr<Packet> packet, uint8_t offset, MIPv6OptionBundle& bundle)
{
  NS_LOG_FUNCTION ( this << packet );
  
  Ipv6MobilityOptionNonceIndicesHeader nih;
  
  Ptr<Packet> p = packet->Copy();
  
  p->RemoveAtStart(offset);
  
  p->RemoveHeader(nih);
  
  bundle.SetHomeNonceIndex(nih.GetHomeNonceIndex());
  bundle.SetCareofNonceIndex(nih.GetCareOfNonceIndex());
 
  return nih.GetSerializedSize();
}

NS_OBJECT_ENSURE_REGISTERED (Ipv6MobilityOptionBindingAuthorizationData);

TypeId Ipv6MobilityOptionBindingAuthorizationData::GetTypeId ()
{
  static TypeId tid = TypeId ("ns3::Ipv6MobilityOptionBindingAuthorizationData")
    .SetParent<MIPv6Option>()
	;
  return tid;
}

Ipv6MobilityOptionBindingAuthorizationData::~Ipv6MobilityOptionBindingAuthorizationData()
{
  NS_LOG_FUNCTION_NOARGS ();
}

uint8_t Ipv6MobilityOptionBindingAuthorizationData::GetMobilityOptionNumber () const
{
  NS_LOG_FUNCTION_NOARGS ();
  
  return OPT_NUMBER;
}

uint8_t Ipv6MobilityOptionBindingAuthorizationData::Process (Ptr<Packet> packet, uint8_t offset, MIPv6OptionBundle& bundle)
{
  NS_LOG_FUNCTION ( this << packet );
  
  Ipv6MobilityOptionBindingAuthorizationDataHeader authh;
  
  Ptr<Packet> p = packet->Copy();
  
  p->RemoveAtStart(offset);
  
  p->RemoveHeader(authh);
  
  bundle.SetAuthenticator(authh.GetAuthenticator());
 
  return authh.GetSerializedSize();
}


} /* namespace ns3 */
