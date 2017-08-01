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
#include "mipv6-header.h"
#include "mipv6-option-header.h"

NS_LOG_COMPONENT_DEFINE ("Mipv6OptionHeader");

namespace ns3 {

NS_OBJECT_ENSURE_REGISTERED (Mipv6OptionHeader);

TypeId Mipv6OptionHeader::GetTypeId ()
{
  static TypeId tid = TypeId ("ns3::Mipv6OptionHeader")
    .SetParent<Header> ()
    .AddConstructor<Mipv6OptionHeader> ()
  ;
  return tid;
}

TypeId Mipv6OptionHeader::GetInstanceTypeId () const
{
  return GetTypeId ();
}


Mipv6OptionHeader::Mipv6OptionHeader ()
{
  m_type = 0;
  m_len = 0;
}

Mipv6OptionHeader::~Mipv6OptionHeader ()
{
}

uint8_t Mipv6OptionHeader::GetType () const
{
  NS_LOG_FUNCTION (this);
  return m_type;
}

void Mipv6OptionHeader::SetType (uint8_t type)
{
  NS_LOG_FUNCTION (this << type);
  m_type = type;
}

uint8_t Mipv6OptionHeader::GetLength () const
{
  NS_LOG_FUNCTION (this);
  return m_len;
}

void Mipv6OptionHeader::SetLength (uint8_t len)
{
  NS_LOG_FUNCTION (this << len);
  m_len = len;
}

void Mipv6OptionHeader::Print (std::ostream& os) const
{
  os << "( type = " << (uint32_t)GetType () << " length = " << (uint32_t)GetLength () << ")";
}

uint32_t Mipv6OptionHeader::GetSerializedSize () const
{
  return m_len;
}

uint32_t Mipv6OptionHeader::Deserialize (Buffer::Iterator start)
{
  return GetSerializedSize ();
}

void Mipv6OptionHeader::Serialize (Buffer::Iterator start) const
{
}

Mipv6OptionHeader::Alignment Mipv6OptionHeader::GetAlignment () const
{
  return (Alignment){
           1,0
  };
}

NS_OBJECT_ENSURE_REGISTERED (Ipv6MobilityOptionPad1Header);

TypeId Ipv6MobilityOptionPad1Header::GetTypeId ()
{
  static TypeId tid = TypeId ("ns3::Ipv6MobilityOptionPad1Header")
    .SetParent<Mipv6OptionHeader> ()
    .AddConstructor<Ipv6MobilityOptionPad1Header> ()
  ;
  return tid;
}

TypeId Ipv6MobilityOptionPad1Header::GetInstanceTypeId () const
{
  return GetTypeId ();
}

Ipv6MobilityOptionPad1Header::Ipv6MobilityOptionPad1Header ()
{
  SetType (Mipv6Header::IPV6_MOBILITY_OPT_PAD1);
  SetLength (0);
}

Ipv6MobilityOptionPad1Header::~Ipv6MobilityOptionPad1Header ()
{
}

void Ipv6MobilityOptionPad1Header::Print (std::ostream& os) const
{
  os << "( type=" << GetType () << ")";
}

uint32_t Ipv6MobilityOptionPad1Header::GetSerializedSize () const
{
  return 1;
}

void Ipv6MobilityOptionPad1Header::Serialize (Buffer::Iterator start) const
{
  Buffer::Iterator i = start;

  i.WriteU8 (GetType ());
}

uint32_t Ipv6MobilityOptionPad1Header::Deserialize (Buffer::Iterator start)
{
  Buffer::Iterator i = start;

  SetType (i.ReadU8 ());

  return GetSerializedSize ();
}

NS_OBJECT_ENSURE_REGISTERED (Ipv6MobilityOptionPadnHeader);

TypeId Ipv6MobilityOptionPadnHeader::GetTypeId ()
{
  static TypeId tid = TypeId ("ns3::Ipv6MobilityOptionPadnHeader")
    .SetParent<Mipv6OptionHeader> ()
    .AddConstructor<Ipv6MobilityOptionPadnHeader> ()
  ;
  return tid;
}

TypeId Ipv6MobilityOptionPadnHeader::GetInstanceTypeId () const
{
  return GetTypeId ();
}

Ipv6MobilityOptionPadnHeader::Ipv6MobilityOptionPadnHeader (uint8_t length)
{
  SetType (Mipv6Header::IPV6_MOBILITY_OPT_PADN);
  SetLength (length - 2);
}

Ipv6MobilityOptionPadnHeader::~Ipv6MobilityOptionPadnHeader ()
{
}

void Ipv6MobilityOptionPadnHeader::Print (std::ostream& os) const
{
  os << "( type=" << GetType () << ", length(excluding TL)=" << GetLength () << ")";
}

uint32_t Ipv6MobilityOptionPadnHeader::GetSerializedSize () const
{
  return GetLength () + 2;
}

void Ipv6MobilityOptionPadnHeader::Serialize (Buffer::Iterator start) const
{
  Buffer::Iterator i = start;
  uint8_t len = GetLength ();

  i.WriteU8 (GetType ());
  i.WriteU8 (len);

  while (len-- > 0)
    {
      i.WriteU8 (0);
    }
}

uint32_t Ipv6MobilityOptionPadnHeader::Deserialize (Buffer::Iterator start)
{
  Buffer::Iterator i = start;
  uint8_t len;

  SetType (i.ReadU8 ());
  SetLength ((len = i.ReadU8 ()));

  while (len-- > 0)
    {
      i.ReadU8 ();
    }

  return GetSerializedSize ();
}

NS_OBJECT_ENSURE_REGISTERED (Ipv6MobilityOptionBindingRefreshAdviceHeader);

TypeId Ipv6MobilityOptionBindingRefreshAdviceHeader::GetTypeId ()
{
  static TypeId tid = TypeId ("ns3::Ipv6MobilityOptionBindingRefreshAdviceHeader")
    .SetParent<Mipv6OptionHeader> ()
    .AddConstructor<Ipv6MobilityOptionBindingRefreshAdviceHeader> ()
  ;
  return tid;
}

TypeId Ipv6MobilityOptionBindingRefreshAdviceHeader::GetInstanceTypeId () const
{
  return GetTypeId ();
}

Ipv6MobilityOptionBindingRefreshAdviceHeader::Ipv6MobilityOptionBindingRefreshAdviceHeader ()
{
  SetType (Mipv6Header::IPV6_MOBILITY_OPT_BINDING_REFRESH_ADVICE);
  SetLength (2);
  SetRefreshInterval (10);
}

Ipv6MobilityOptionBindingRefreshAdviceHeader::~Ipv6MobilityOptionBindingRefreshAdviceHeader ()
{
}

uint16_t Ipv6MobilityOptionBindingRefreshAdviceHeader::GetRefreshInterval () const
{
  return m_refreshinterval;
}

void Ipv6MobilityOptionBindingRefreshAdviceHeader::SetRefreshInterval (uint16_t intvl)
{
  m_refreshinterval = intvl;
}

void Ipv6MobilityOptionBindingRefreshAdviceHeader::Print (std::ostream& os) const
{
  os << "( type=" << GetType () << ", length(excluding TL)=" << GetLength () << ", RefreshInterval=" << GetRefreshInterval () << ")";
}

uint32_t Ipv6MobilityOptionBindingRefreshAdviceHeader::GetSerializedSize () const
{
  return GetLength () + 2;
}

void Ipv6MobilityOptionBindingRefreshAdviceHeader::Serialize (Buffer::Iterator start) const
{

  Buffer::Iterator i = start;

  i.WriteU8 (GetType ());
  i.WriteU8 (GetLength ());

  i.WriteU16 (GetRefreshInterval ());

}

uint32_t Ipv6MobilityOptionBindingRefreshAdviceHeader::Deserialize (Buffer::Iterator start)
{

  Buffer::Iterator i = start;


  SetType (i.ReadU8 ());
  SetLength (i.ReadU8 ());

  SetRefreshInterval (i.ReadU16 ());

  return GetSerializedSize ();
}

Mipv6OptionHeader::Alignment Ipv6MobilityOptionBindingRefreshAdviceHeader::GetAlignment () const
{
  return (Alignment){
           2,0
  };                       //2n
}


NS_OBJECT_ENSURE_REGISTERED (Ipv6MobilityOptionAlternateCareofAddressHeader);

TypeId Ipv6MobilityOptionAlternateCareofAddressHeader::GetTypeId ()
{
  static TypeId tid = TypeId ("ns3::Ipv6MobilityOptionAlternateCareofAddressHeader")
    .SetParent<Mipv6OptionHeader> ()
    .AddConstructor<Ipv6MobilityOptionAlternateCareofAddressHeader> ()
  ;
  return tid;
}

TypeId Ipv6MobilityOptionAlternateCareofAddressHeader::GetInstanceTypeId () const
{
  return GetTypeId ();
}

Ipv6MobilityOptionAlternateCareofAddressHeader::Ipv6MobilityOptionAlternateCareofAddressHeader ()
{
  SetType (Mipv6Header::IPV6_MOBILITY_OPT_ALTERNATE_CARE_OF_ADDRESS);
  SetLength (16);

  m_coa.Set ("::");

}



Ipv6MobilityOptionAlternateCareofAddressHeader::~Ipv6MobilityOptionAlternateCareofAddressHeader ()
{
}


Ipv6Address Ipv6MobilityOptionAlternateCareofAddressHeader::GetAlternateCareofAddress () const
{
  return m_coa;
}

void Ipv6MobilityOptionAlternateCareofAddressHeader::SetAlternateCareofAddress (Ipv6Address coa)
{
  m_coa = coa;
}

void Ipv6MobilityOptionAlternateCareofAddressHeader::Print (std::ostream& os) const
{
  os << "( type=" << GetType () << ", length(excluding TL)=" << GetLength () <<  ", coa=" << GetAlternateCareofAddress () << ")";
}

uint32_t Ipv6MobilityOptionAlternateCareofAddressHeader::GetSerializedSize () const
{
  return GetLength () + 2;
}

void Ipv6MobilityOptionAlternateCareofAddressHeader::Serialize (Buffer::Iterator start) const
{
  uint8_t buff_prefix[16];
  Buffer::Iterator i = start;

  i.WriteU8 (GetType ());
  i.WriteU8 (GetLength ());


  m_coa.Serialize (buff_prefix);
  i.Write (buff_prefix, 16);
}

uint32_t Ipv6MobilityOptionAlternateCareofAddressHeader::Deserialize (Buffer::Iterator start)
{
  uint8_t buff[16];
  Buffer::Iterator i = start;

  SetType (i.ReadU8 ());
  SetLength (i.ReadU8 ());


  i.Read (buff, 16);
  m_coa.Set (buff);

  return GetSerializedSize ();
}

Mipv6OptionHeader::Alignment Ipv6MobilityOptionAlternateCareofAddressHeader::GetAlignment () const
{
  return (Alignment){
           8,6
  };                       //8n+6
}

NS_OBJECT_ENSURE_REGISTERED (Ipv6MobilityOptionNonceIndicesHeader);

TypeId Ipv6MobilityOptionNonceIndicesHeader::GetTypeId ()
{
  static TypeId tid = TypeId ("ns3::Ipv6MobilityOptionNonceIndicesHeader")
    .SetParent<Mipv6OptionHeader> ()
    .AddConstructor<Ipv6MobilityOptionNonceIndicesHeader> ()
  ;
  return tid;
}

TypeId Ipv6MobilityOptionNonceIndicesHeader::GetInstanceTypeId () const
{
  return GetTypeId ();
}

Ipv6MobilityOptionNonceIndicesHeader::Ipv6MobilityOptionNonceIndicesHeader ()
{
  SetType (Mipv6Header::IPV6_MOBILITY_OPT_NONCE_INDICES);
  SetLength (4);

  m_homenonceindex = 0;
  m_careofnonceindex = 0;
}

Ipv6MobilityOptionNonceIndicesHeader::~Ipv6MobilityOptionNonceIndicesHeader ()
{
}

uint16_t Ipv6MobilityOptionNonceIndicesHeader::GetHomeNonceIndex () const
{
  return m_homenonceindex;
}

void Ipv6MobilityOptionNonceIndicesHeader::SetHomeNonceIndex (uint16_t hi)
{
  m_homenonceindex = hi;
}

uint16_t Ipv6MobilityOptionNonceIndicesHeader::GetCareOfNonceIndex () const
{
  return m_careofnonceindex;
}

void Ipv6MobilityOptionNonceIndicesHeader::SetCareOfNonceIndex (uint16_t coi)
{
  m_careofnonceindex = coi;
}

void Ipv6MobilityOptionNonceIndicesHeader::Print (std::ostream& os) const
{
  os << "( type=" << (uint32_t)GetType () << ", length(excluding TL)=" << (uint32_t)GetLength () << ", home_index=" << (uint16_t)GetHomeNonceIndex () << ", care_of_index=" << (uint16_t)GetCareOfNonceIndex () << ")";
}

uint32_t Ipv6MobilityOptionNonceIndicesHeader::GetSerializedSize () const
{
  return GetLength () + 2;
}

void Ipv6MobilityOptionNonceIndicesHeader::Serialize (Buffer::Iterator start) const
{
  Buffer::Iterator i = start;

  i.WriteU8 (GetType ());
  i.WriteU8 (GetLength ());

  i.WriteU16 (m_homenonceindex);
  i.WriteU16 (m_careofnonceindex);
}

uint32_t Ipv6MobilityOptionNonceIndicesHeader::Deserialize (Buffer::Iterator start)
{
  Buffer::Iterator i = start;

  SetType (i.ReadU8 ());
  SetLength (i.ReadU8 ());

  m_homenonceindex = i.ReadU16 ();
  m_careofnonceindex = i.ReadU16 ();

  return GetSerializedSize ();
}


Mipv6OptionHeader::Alignment Ipv6MobilityOptionNonceIndicesHeader::GetAlignment () const
{
  return (Alignment){
           2,0
  };                       //2n
}

NS_OBJECT_ENSURE_REGISTERED (Ipv6MobilityOptionBindingAuthorizationDataHeader);

TypeId Ipv6MobilityOptionBindingAuthorizationDataHeader::GetTypeId ()
{
  static TypeId tid = TypeId ("ns3::Ipv6MobilityOptionBindingAuthorizationDataHeader")
    .SetParent<Mipv6OptionHeader> ()
    .AddConstructor<Ipv6MobilityOptionBindingAuthorizationDataHeader> ()
  ;
  return tid;
}

TypeId Ipv6MobilityOptionBindingAuthorizationDataHeader::GetInstanceTypeId () const
{
  return GetTypeId ();
}

Ipv6MobilityOptionBindingAuthorizationDataHeader::Ipv6MobilityOptionBindingAuthorizationDataHeader ()
{
  SetType (Mipv6Header::IPV6_MOBILITY_OPT_BINDING_AUTHORIZATION_DATA);
  SetLength (8);

  m_auth = 0;
}


Ipv6MobilityOptionBindingAuthorizationDataHeader::~Ipv6MobilityOptionBindingAuthorizationDataHeader ()
{
}

uint64_t Ipv6MobilityOptionBindingAuthorizationDataHeader::GetAuthenticator () const
{
  return m_auth;
}

void Ipv6MobilityOptionBindingAuthorizationDataHeader::SetAuthenticator (uint64_t auth)
{
  m_auth = auth;
}


void Ipv6MobilityOptionBindingAuthorizationDataHeader::Print (std::ostream& os) const
{
  os << "( type=" << (uint32_t)GetType () << ", length(excluding TL)=" << (uint32_t)GetLength () << ", Authenticator=" << (uint64_t)GetAuthenticator () << ")";
}

uint32_t Ipv6MobilityOptionBindingAuthorizationDataHeader::GetSerializedSize () const
{
  return GetLength () + 2;
}

void Ipv6MobilityOptionBindingAuthorizationDataHeader::Serialize (Buffer::Iterator start) const
{
  Buffer::Iterator i = start;

  i.WriteU8 (GetType ());
  i.WriteU8 (GetLength ());

  i.WriteU64 (m_auth);
}

uint32_t Ipv6MobilityOptionBindingAuthorizationDataHeader::Deserialize (Buffer::Iterator start)
{
  Buffer::Iterator i = start;

  SetType (i.ReadU8 ());
  SetLength (i.ReadU8 ());

  m_auth = i.ReadU64 ();

  return GetSerializedSize ();
}


Mipv6OptionHeader::Alignment Ipv6MobilityOptionBindingAuthorizationDataHeader::GetAlignment () const
{
  return (Alignment){
           8,2
  };                       //8n+2
}


} /* namespace ns3 */
