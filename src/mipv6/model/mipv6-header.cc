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

namespace ns3 {

NS_OBJECT_ENSURE_REGISTERED (Mipv6Header);

NS_LOG_COMPONENT_DEFINE ("Mipv6Header");

TypeId Mipv6Header::GetTypeId ()
{
  static TypeId tid = TypeId ("ns3::Mipv6Header")
    .SetParent<Header> ()
    .AddConstructor<Mipv6Header> ()
  ;
  return tid;
}

TypeId Mipv6Header::GetInstanceTypeId () const
{
  return GetTypeId ();
}

Mipv6Header::Mipv6Header ()
  : m_payload_proto (59),
  m_header_len (0),
  m_mh_type (0),
  m_reserved (0),
  m_checksum (0)
{
}

Mipv6Header::~Mipv6Header ()
{
}

uint8_t Mipv6Header::GetPayloadProto () const
{
  return m_payload_proto;
}

void Mipv6Header::SetPayloadProto (uint8_t payload_proto)
{
  m_payload_proto = payload_proto;
}

uint8_t Mipv6Header::GetHeaderLen () const
{
  return m_header_len;
}

void Mipv6Header::SetHeaderLen (uint8_t header_len)
{
  m_header_len = header_len;
}

uint8_t Mipv6Header::GetMhType () const
{
  return m_mh_type;
}

void Mipv6Header::SetMhType (uint8_t mh_type)
{
  m_mh_type = mh_type;
}

uint8_t Mipv6Header::GetReserved () const
{
  return m_reserved;
}

void Mipv6Header::SetReserved (uint8_t reserved)
{
  m_reserved = reserved;
}

uint16_t Mipv6Header::GetChecksum () const
{
  return m_checksum;
}

void Mipv6Header::SetChecksum (uint16_t checksum)
{
  m_checksum = checksum;
}

void Mipv6Header::Print (std::ostream& os) const
{
  os << "( payload_proto = " << (uint32_t)GetPayloadProto () << " header_len = " << (uint32_t)GetHeaderLen () << " mh_type = " << (uint32_t)GetMhType () << " checksum = " << (uint32_t)GetChecksum () << ")";
}

uint32_t Mipv6Header::GetSerializedSize () const
{
  return 6;
}

uint32_t Mipv6Header::Deserialize (Buffer::Iterator start)
{
  Buffer::Iterator i = start;

  m_payload_proto = i.ReadU8 ();
  m_header_len = i.ReadU8 ();
  m_mh_type = i.ReadU8 ();
  m_reserved = i.ReadU8 ();
  m_checksum = i.ReadNtohU16 ();

  return GetSerializedSize ();
}

void Mipv6Header::Serialize (Buffer::Iterator start) const
{
  Buffer::Iterator i = start;

  i.WriteU8 (m_payload_proto);
  i.WriteU8 (m_header_len);
  i.WriteU8 (m_mh_type);
  i.WriteU8 (m_reserved);
  i.WriteU16 (0);
}


Mipv6OptionField::Mipv6OptionField (uint32_t optionsOffset)
  : m_optionData (0),
  m_optionsOffset (optionsOffset)
{
}

Mipv6OptionField::~Mipv6OptionField ()
{
}

uint32_t Mipv6OptionField::GetSerializedSize () const
{
  return m_optionData.GetSize () + CalculatePad ((Mipv6OptionHeader::Alignment) {8,0});
}

void Mipv6OptionField::Serialize (Buffer::Iterator start) const
{
  start.Write (m_optionData.Begin (), m_optionData.End ());
  uint32_t fill = CalculatePad ((Mipv6OptionHeader::Alignment) {8,0});

  NS_LOG_LOGIC ("fill with " << fill << " bytes padding");
  switch (fill)
    {
    case 0:
      return;
    case 1:
      Ipv6MobilityOptionPad1Header ().Serialize (start);
      return;
    default:
      Ipv6MobilityOptionPadnHeader (fill).Serialize (start);
      return;
    }
}

uint32_t Mipv6OptionField::Deserialize (Buffer::Iterator start, uint32_t length)
{
  uint8_t buf[length];
  start.Read (buf, length);
  m_optionData = Buffer ();
  m_optionData.AddAtEnd (length);
  m_optionData.Begin ().Write (buf, length);
  return length;
}

void Mipv6OptionField::AddOption (Mipv6OptionHeader const& option)
{
  NS_LOG_FUNCTION (this << option);

  uint32_t pad = CalculatePad (option.GetAlignment ());

  NS_LOG_LOGIC ("need " << pad << " bytes padding");
  switch (pad)
    {
    case 0:
      break;       //no padding needed
    case 1:
      AddOption (Ipv6MobilityOptionPad1Header ());
      break;
    default:
      AddOption (Ipv6MobilityOptionPadnHeader (pad));
      break;
    }

  m_optionData.AddAtEnd (option.GetSerializedSize ());
  Buffer::Iterator it = m_optionData.End ();
  it.Prev (option.GetSerializedSize ());
  option.Serialize (it);
}

uint32_t Mipv6OptionField::CalculatePad (Mipv6OptionHeader::Alignment alignment) const
{
  return (alignment.offset - (m_optionData.GetSize () + m_optionsOffset)) % alignment.factor;
}

uint32_t Mipv6OptionField::GetOptionsOffset ()
{
  return m_optionsOffset;
}

Buffer Mipv6OptionField::GetOptionBuffer ()
{
  return m_optionData;
}


NS_OBJECT_ENSURE_REGISTERED (Ipv6MobilityBindingUpdateHeader);

TypeId Ipv6MobilityBindingUpdateHeader::GetTypeId ()
{
  static TypeId tid = TypeId ("ns3::Ipv6MobilityBindingUpdateHeader")
    .SetParent<Mipv6Header> ()
    .AddConstructor<Ipv6MobilityBindingUpdateHeader> ()
  ;
  return tid;
}

TypeId Ipv6MobilityBindingUpdateHeader::GetInstanceTypeId () const
{
  return GetTypeId ();
}

Ipv6MobilityBindingUpdateHeader::Ipv6MobilityBindingUpdateHeader ()
  : Mipv6OptionField (12)
{
  SetHeaderLen (0);
  SetMhType (IPV6_MOBILITY_BINDING_UPDATE);
  SetReserved (0);
  SetChecksum (0);

  SetSequence (0);
  SetFlagA (0);
  SetFlagH (0);
  SetFlagL (0);
  SetFlagK (0);
  SetReserved2 (0);
  SetLifetime (0);
}

Ipv6MobilityBindingUpdateHeader::~Ipv6MobilityBindingUpdateHeader ()
{
}

uint16_t Ipv6MobilityBindingUpdateHeader::GetSequence () const
{
  return m_sequence;
}

void Ipv6MobilityBindingUpdateHeader::SetSequence (uint16_t sequence)
{
  m_sequence = sequence;
}

bool Ipv6MobilityBindingUpdateHeader::GetFlagA () const
{
  return m_flagA;
}

void Ipv6MobilityBindingUpdateHeader::SetFlagA (bool a)
{
  m_flagA = a;
}

bool Ipv6MobilityBindingUpdateHeader::GetFlagH () const
{
  return m_flagH;
}

void Ipv6MobilityBindingUpdateHeader::SetFlagH (bool h)
{
  m_flagH = h;
}

bool Ipv6MobilityBindingUpdateHeader::GetFlagL () const
{
  return m_flagL;
}

void Ipv6MobilityBindingUpdateHeader::SetFlagL (bool l)
{
  m_flagL = l;
}

bool Ipv6MobilityBindingUpdateHeader::GetFlagK () const
{
  return m_flagK;
}

void Ipv6MobilityBindingUpdateHeader::SetFlagK (bool k)
{
  m_flagK = k;
}


uint16_t Ipv6MobilityBindingUpdateHeader::GetReserved2 () const
{
  return m_reserved2;
}

void Ipv6MobilityBindingUpdateHeader::SetReserved2 (uint16_t reserved2)
{
  m_reserved2 = reserved2;
}

uint16_t Ipv6MobilityBindingUpdateHeader::GetLifetime () const
{
  return m_lifetime;
}

void Ipv6MobilityBindingUpdateHeader::SetLifetime (uint16_t lifetime)
{
  m_lifetime = lifetime;
}

void Ipv6MobilityBindingUpdateHeader::Print (std::ostream& os) const
{
  os << "( payload_proto = " << (uint32_t)GetPayloadProto () << " header_len = " << (uint32_t)GetHeaderLen () << " mh_type = " << (uint32_t)GetMhType () << " checksum = " << (uint32_t)GetChecksum ();
  os << " sequence = " << (uint32_t)GetSequence () << ")";
}

uint32_t Ipv6MobilityBindingUpdateHeader::GetSerializedSize () const
{
  return 12;
}

void Ipv6MobilityBindingUpdateHeader::Serialize (Buffer::Iterator start) const
{
  Buffer::Iterator i = start;
  uint32_t reserved2 = m_reserved2;

  i.WriteU8 (GetPayloadProto ());

  i.WriteU8 ((uint8_t) (( GetSerializedSize () >> 3) - 1));
  i.WriteU8 (GetMhType ());
  i.WriteU8 (GetReserved ());

  i.WriteU16 (0);

  i.WriteHtonU16 (m_sequence);

  if (m_flagA)
    {
      reserved2 |= (uint16_t)(1 << 15);
    }

  if (m_flagH)
    {
      reserved2 |= (uint16_t)(1 << 14);
    }

  if (m_flagL)
    {
      reserved2 |= (uint16_t)(1 << 13);
    }

  if (m_flagK)
    {
      reserved2 |= (uint16_t)(1 << 12);
    }

  i.WriteHtonU16 (reserved2);
  i.WriteHtonU16 (m_lifetime);

}

uint32_t Ipv6MobilityBindingUpdateHeader::Deserialize (Buffer::Iterator start)
{
  Buffer::Iterator i = start;

  SetPayloadProto (i.ReadU8 ());
  SetHeaderLen (i.ReadU8 ());
  SetMhType (i.ReadU8 ());
  SetReserved (i.ReadU8 ());

  SetChecksum (i.ReadU16 ());

  m_sequence = i.ReadNtohU16 ();

  m_reserved2 = i.ReadNtohU16 ();

  m_flagA = false;
  m_flagH = false;
  m_flagL = false;
  m_flagK = false;

  if (m_reserved2 & (1 << 15))
    {
      m_flagA = true;
    }

  if (m_reserved2 & (1 << 14))
    {
      m_flagH = true;
    }

  if (m_reserved2 & (1 << 13))
    {
      m_flagL = true;
    }

  if (m_reserved2 & (1 << 12))
    {
      m_flagK = true;
    }

  m_lifetime = i.ReadNtohU16 ();

  return GetSerializedSize ();
}

NS_OBJECT_ENSURE_REGISTERED (Ipv6MobilityBindingAckHeader);

TypeId Ipv6MobilityBindingAckHeader::GetTypeId ()
{
  static TypeId tid = TypeId ("ns3::Ipv6MobilityBindingAckHeader")
    .SetParent<Mipv6Header> ()
    .AddConstructor<Ipv6MobilityBindingAckHeader> ()
  ;
  return tid;
}

TypeId Ipv6MobilityBindingAckHeader::GetInstanceTypeId () const
{
  return GetTypeId ();
}

Ipv6MobilityBindingAckHeader::Ipv6MobilityBindingAckHeader ()
  : Mipv6OptionField (12)
{
  SetHeaderLen (0);
  SetMhType (IPV6_MOBILITY_BINDING_ACKNOWLEDGEMENT);
  SetReserved (0);
  SetChecksum (0);

  SetStatus (0);
  SetFlagK (0);
  SetReserved2 (0);
  SetSequence (0);
  SetLifetime (0);
}

Ipv6MobilityBindingAckHeader::~Ipv6MobilityBindingAckHeader ()
{
}

uint8_t Ipv6MobilityBindingAckHeader::GetStatus () const
{
  return m_status;
}

void Ipv6MobilityBindingAckHeader::SetStatus (uint8_t status)
{
  m_status = status;
}

bool Ipv6MobilityBindingAckHeader::GetFlagK () const
{
  return m_flagK;
}

void Ipv6MobilityBindingAckHeader::SetFlagK (bool k)
{
  m_flagK = k;
}

uint8_t Ipv6MobilityBindingAckHeader::GetReserved2 () const
{
  return m_reserved2;
}

void Ipv6MobilityBindingAckHeader::SetReserved2 (uint8_t reserved2)
{
  m_reserved2 = reserved2;
}

uint16_t Ipv6MobilityBindingAckHeader::GetSequence () const
{
  return m_sequence;
}

void Ipv6MobilityBindingAckHeader::SetSequence (uint16_t sequence)
{
  m_sequence = sequence;
}

uint16_t Ipv6MobilityBindingAckHeader::GetLifetime () const
{
  return m_lifetime;
}

void Ipv6MobilityBindingAckHeader::SetLifetime (uint16_t lifetime)
{
  m_lifetime = lifetime;
}

void Ipv6MobilityBindingAckHeader::Print (std::ostream& os) const
{
  os << "( payload_proto = " << (uint32_t)GetPayloadProto () << " header_len = " << (uint32_t)GetHeaderLen () << " mh_type = " << (uint32_t)GetMhType () << " checksum = " << (uint32_t)GetChecksum ();
  os << " status = " << (uint32_t)GetStatus () << " sequence = " << (uint32_t)GetSequence () << ")";
}

uint32_t Ipv6MobilityBindingAckHeader::GetSerializedSize () const
{
  return 12;
}

void Ipv6MobilityBindingAckHeader::Serialize (Buffer::Iterator start) const
{
  Buffer::Iterator i = start;
  uint32_t reserved2 = m_reserved2;

  i.WriteU8 (GetPayloadProto ());

  i.WriteU8 ( (uint8_t) (( GetSerializedSize () >> 3) - 1) );
  i.WriteU8 (GetMhType ());
  i.WriteU8 (GetReserved ());
  i.WriteU16 (0);

  i.WriteU8 (m_status);

  if (m_flagK)
    {
      reserved2 |= (uint8_t)(1 << 7);
    }

  i.WriteU8 (reserved2);
  i.WriteHtonU16 (m_sequence);
  i.WriteHtonU16 (m_lifetime);


}

uint32_t Ipv6MobilityBindingAckHeader::Deserialize (Buffer::Iterator start)
{
  Buffer::Iterator i = start;

  SetPayloadProto (i.ReadU8 ());
  SetHeaderLen (i.ReadU8 ());
  SetMhType (i.ReadU8 ());
  SetReserved (i.ReadU8 ());

  SetChecksum (i.ReadU16 ());

  m_status = i.ReadU8 ();

  m_reserved2 = i.ReadU8 ();


  if (m_reserved2 & (1 << 7))
    {
      m_flagK = true;
    }

  m_sequence = i.ReadNtohU16 ();
  m_lifetime = i.ReadNtohU16 ();
  return GetSerializedSize ();
}




NS_OBJECT_ENSURE_REGISTERED (Ipv6HoTIHeader);

TypeId Ipv6HoTIHeader::GetTypeId ()
{
  static TypeId tid = TypeId ("ns3::Ipv6HoTIHeader")
    .SetParent<Mipv6Header> ()
    .AddConstructor<Ipv6HoTIHeader> ()
  ;
  return tid;
}
TypeId Ipv6HoTIHeader::GetInstanceTypeId () const
{
  return GetTypeId ();
}
Ipv6HoTIHeader::Ipv6HoTIHeader ()
{

  SetHeaderLen (0);
  SetMhType (HOME_TEST_INIT);
  SetReserved (0);
  SetChecksum (0);

  SetReserved2 (0);
  SetHomeInitCookie (0);
}
Ipv6HoTIHeader::~Ipv6HoTIHeader ()
{
}
uint16_t Ipv6HoTIHeader::GetReserved2 () const
{
  return m_reserved2;
}

void Ipv6HoTIHeader::SetReserved2 (uint16_t reserved2)
{
  m_reserved2 = reserved2;
}

uint64_t Ipv6HoTIHeader::GetHomeInitCookie () const
{
  return m_homeinitcookie;
}

void Ipv6HoTIHeader::SetHomeInitCookie (uint64_t hicookie)
{
  m_homeinitcookie = hicookie;
}

uint32_t Ipv6HoTIHeader::GetSerializedSize () const
{
  return 16;
}

void Ipv6HoTIHeader::Print (std::ostream& os) const
{
  os << "( payload_proto = " << (uint32_t)GetPayloadProto () << " header_len = " << (uint32_t)GetHeaderLen () << " mh_type = " << (uint32_t)GetMhType () << " checksum = " << (uint32_t)GetChecksum ();
  os << " reserved2 = " << (uint16_t)GetReserved2 () << ")" << "home_init_cookie = " << (uint64_t)GetHomeInitCookie ();
}

void Ipv6HoTIHeader::Serialize (Buffer::Iterator start) const
{
  Buffer::Iterator i = start;
  i.WriteU8 (GetPayloadProto ());

  i.WriteU8 ((uint8_t) (( GetSerializedSize () >> 3) - 1));
  i.WriteU8 (GetMhType ());
  i.WriteU8 (GetReserved ());
  i.WriteU16 (0);


  i.WriteU16 (0);
  i.WriteU64 (GetHomeInitCookie ());
}
uint32_t Ipv6HoTIHeader::Deserialize (Buffer::Iterator start)
{
  Buffer::Iterator i = start;

  SetPayloadProto (i.ReadU8 ());
  SetHeaderLen (i.ReadU8 ());
  SetMhType (i.ReadU8 ());
  SetReserved (i.ReadU8 ());
  SetChecksum (i.ReadU16 ());


  SetReserved2 (i.ReadU16 ());
  SetHomeInitCookie (i.ReadU64 ());
  return GetSerializedSize ();

}


NS_OBJECT_ENSURE_REGISTERED (Ipv6CoTIHeader);

TypeId Ipv6CoTIHeader::GetTypeId ()
{
  static TypeId tid = TypeId ("ns3::Ipv6CoTIHeader")
    .SetParent<Mipv6Header> ()
    .AddConstructor<Ipv6CoTIHeader> ()
  ;
  return tid;
}
TypeId Ipv6CoTIHeader::GetInstanceTypeId () const
{
  return GetTypeId ();
}
Ipv6CoTIHeader::Ipv6CoTIHeader ()
{

  SetHeaderLen (0);
  SetMhType (CARE_OF_TEST_INIT);
  SetReserved (0);
  SetChecksum (0);

  SetReserved2 (0);
}
Ipv6CoTIHeader::~Ipv6CoTIHeader ()
{
}
uint16_t Ipv6CoTIHeader::GetReserved2 () const
{
  return m_reserved2;
}

void Ipv6CoTIHeader::SetReserved2 (uint16_t reserved2)
{
  m_reserved2 = reserved2;
}

uint64_t Ipv6CoTIHeader::GetCareOfInitCookie () const
{
  return m_careofinitcookie;
}

void Ipv6CoTIHeader::SetCareOfInitCookie (uint64_t coaicookie)
{
  m_careofinitcookie = coaicookie;
}

uint32_t Ipv6CoTIHeader::GetSerializedSize () const
{
  return 16;
}

void Ipv6CoTIHeader::Print (std::ostream& os) const
{
  os << "( payload_proto = " << (uint32_t)GetPayloadProto () << " header_len = " << (uint32_t)GetHeaderLen () << " mh_type = " << (uint32_t)GetMhType () << " checksum = " << (uint32_t)GetChecksum ();
  os << " reserved2 = " << (uint16_t)GetReserved2 () << " care_of_init_cookie = " << (uint64_t)GetCareOfInitCookie () << ")";
}

void Ipv6CoTIHeader::Serialize (Buffer::Iterator start) const
{
  Buffer::Iterator i = start;
  i.WriteU8 (GetPayloadProto ());

  i.WriteU8 ((uint8_t) (( GetSerializedSize () >> 3) - 1));
  i.WriteU8 (GetMhType ());
  i.WriteU8 (GetReserved ());
  i.WriteU16 (0);


  i.WriteU16 (0);
  i.WriteU64 (0);
}
uint32_t Ipv6CoTIHeader::Deserialize (Buffer::Iterator start)
{
  Buffer::Iterator i = start;

  SetPayloadProto (i.ReadU8 ());
  SetHeaderLen (i.ReadU8 ());
  SetMhType (i.ReadU8 ());
  SetReserved (i.ReadU8 ());
  SetChecksum (i.ReadU16 ());


  SetReserved2 (i.ReadU16 ());
  SetCareOfInitCookie (i.ReadU64 ());

  return GetSerializedSize ();

}

NS_OBJECT_ENSURE_REGISTERED (Ipv6HoTHeader);

TypeId Ipv6HoTHeader::GetTypeId ()
{
  static TypeId tid = TypeId ("ns3::Ipv6HoTHeader")
    .SetParent<Mipv6Header> ()
    .AddConstructor<Ipv6HoTHeader> ()
  ;
  return tid;
}
TypeId Ipv6HoTHeader::GetInstanceTypeId () const
{
  return GetTypeId ();
}
Ipv6HoTHeader::Ipv6HoTHeader ()
{

  SetHeaderLen (0);
  SetMhType (HOME_TEST);
  SetReserved (0);
  SetChecksum (0);

  SetHomeNonceIndex (0);
  SetHomeInitCookie (0);
  SetHomeKeygenToken (0);
}
Ipv6HoTHeader::~Ipv6HoTHeader ()
{
}
uint16_t Ipv6HoTHeader::GetHomeNonceIndex () const
{
  return m_homenonceindex;
}

void Ipv6HoTHeader::SetHomeNonceIndex (uint16_t homenonceind)
{
  m_homenonceindex = homenonceind;
}

uint64_t Ipv6HoTHeader::GetHomeInitCookie () const
{
  return m_homeinitcookie;
}

void Ipv6HoTHeader::SetHomeInitCookie (uint64_t homeinitcookie)
{
  m_homeinitcookie = homeinitcookie;
}

uint64_t Ipv6HoTHeader::GetHomeKeygenToken () const
{
  return m_homekeygentoken;
}

void Ipv6HoTHeader::SetHomeKeygenToken (uint64_t homekeygentoken)
{
  m_homekeygentoken = homekeygentoken;
}

uint32_t Ipv6HoTHeader::GetSerializedSize () const
{
  return 24;
}

void Ipv6HoTHeader::Print (std::ostream& os) const
{
  os << "( payload_proto = " << (uint32_t)GetPayloadProto () << " header_len = " << (uint32_t)GetHeaderLen () << " mh_type = " << (uint32_t)GetMhType () << " checksum = " << (uint32_t)GetChecksum ();
  os << " homenonceindex = " << (uint16_t)GetHomeNonceIndex () << " homeinitcookie = " << (uint64_t)GetHomeInitCookie () << "HomeKeygenToken=" << (uint64_t) GetHomeKeygenToken ();
}

void Ipv6HoTHeader::Serialize (Buffer::Iterator start) const
{
  Buffer::Iterator i = start;
  i.WriteU8 (GetPayloadProto ());

  i.WriteU8 ((uint8_t) (( GetSerializedSize () >> 3) - 1));
  i.WriteU8 (GetMhType ());
  i.WriteU8 (GetReserved ());
  i.WriteU16 (0);


  i.WriteU16 (GetHomeNonceIndex ());
  i.WriteU64 (GetHomeInitCookie ());
  i.WriteU64 (GetHomeKeygenToken ());
}
uint32_t Ipv6HoTHeader::Deserialize (Buffer::Iterator start)
{
  Buffer::Iterator i = start;

  SetPayloadProto (i.ReadU8 ());
  SetHeaderLen (i.ReadU8 ());
  SetMhType (i.ReadU8 ());
  SetReserved (i.ReadU8 ());
  SetChecksum (i.ReadU16 ());


  SetHomeNonceIndex (i.ReadU16 ());
  SetHomeInitCookie (i.ReadU64 ());
  SetHomeKeygenToken (i.ReadU64 ());
  return GetSerializedSize ();

}

NS_OBJECT_ENSURE_REGISTERED (Ipv6CoTHeader);

TypeId Ipv6CoTHeader::GetTypeId ()
{
  static TypeId tid = TypeId ("ns3::Ipv6CoTHeader")
    .SetParent<Mipv6Header> ()
    .AddConstructor<Ipv6CoTHeader> ()
  ;
  return tid;
}
TypeId Ipv6CoTHeader::GetInstanceTypeId () const
{
  return GetTypeId ();
}
Ipv6CoTHeader::Ipv6CoTHeader ()
{

  SetHeaderLen (0);
  SetMhType (CARE_OF_TEST);
  SetReserved (0);
  SetChecksum (0);

  SetCareOfNonceIndex (0);
  SetCareOfInitCookie (0);
  SetCareOfKeygenToken (0);

}
Ipv6CoTHeader::~Ipv6CoTHeader ()
{
}
uint16_t Ipv6CoTHeader::GetCareOfNonceIndex () const
{
  return m_careofnonceindex;
}

void Ipv6CoTHeader::SetCareOfNonceIndex (uint16_t careofnonceind)
{
  m_careofnonceindex = careofnonceind;
}

uint64_t Ipv6CoTHeader::GetCareOfInitCookie () const
{
  return m_careofinitcookie;
}

void Ipv6CoTHeader::SetCareOfInitCookie (uint64_t careofinitcookie)
{
  m_careofinitcookie = careofinitcookie;
}

uint64_t Ipv6CoTHeader::GetCareOfKeygenToken () const
{
  return m_careofkeygenoken;
}

void Ipv6CoTHeader::SetCareOfKeygenToken (uint64_t careofkeygentoken)
{
  m_careofkeygenoken = careofkeygentoken;
}

uint32_t Ipv6CoTHeader::GetSerializedSize () const
{
  return 24;
}

void Ipv6CoTHeader::Print (std::ostream& os) const
{
  os << "( payload_proto = " << (uint32_t)GetPayloadProto () << " header_len = " << (uint32_t)GetHeaderLen () << " mh_type = " << (uint32_t)GetMhType () << " checksum = " << (uint32_t)GetChecksum ();
  os << " care_of_nonceindex = " << (uint16_t)GetCareOfNonceIndex () << "care_of_initcookie" << (uint64_t) GetCareOfInitCookie () << "care_of_keygenoken" << (uint64_t) GetCareOfKeygenToken () << ")";
}

void Ipv6CoTHeader::Serialize (Buffer::Iterator start) const
{
  Buffer::Iterator i = start;
  i.WriteU8 (GetPayloadProto ());

  i.WriteU8 ((uint8_t) (( GetSerializedSize () >> 3) - 1));
  i.WriteU8 (GetMhType ());
  i.WriteU8 (GetReserved ());
  i.WriteU16 (0);


  i.WriteU16 (GetCareOfNonceIndex ());
  i.WriteU64 (GetCareOfInitCookie ());
  i.WriteU64 (GetCareOfKeygenToken ());
}

uint32_t Ipv6CoTHeader::Deserialize (Buffer::Iterator start)
{
  Buffer::Iterator i = start;

  SetPayloadProto (i.ReadU8 ());
  SetHeaderLen (i.ReadU8 ());
  SetMhType (i.ReadU8 ());
  SetReserved (i.ReadU8 ());
  SetChecksum (i.ReadU16 ());


  SetCareOfNonceIndex (i.ReadU16 ());
  SetCareOfInitCookie (i.ReadU64 ());
  SetCareOfKeygenToken (i.ReadU64 ());
  return GetSerializedSize ();

}

NS_OBJECT_ENSURE_REGISTERED (Ipv6BindingErrorHeader);

TypeId Ipv6BindingErrorHeader::GetTypeId ()
{
  static TypeId tid = TypeId ("ns3::Ipv6BindingErrorHeader")
    .SetParent<Mipv6Header> ()
    .AddConstructor<Ipv6BindingErrorHeader> ()
  ;
  return tid;
}
TypeId Ipv6BindingErrorHeader::GetInstanceTypeId () const
{
  return GetTypeId ();
}
Ipv6BindingErrorHeader::Ipv6BindingErrorHeader ()
{
  SetHeaderLen (0);
  SetMhType (IPv6_BINDING_ERROR);
  SetReserved (0);
  SetChecksum (0);

  SetStatus (0);
  SetReserved2 (0);
  SetHomeAddress ("0::0");

}
Ipv6BindingErrorHeader::~Ipv6BindingErrorHeader ()
{
}
uint8_t Ipv6BindingErrorHeader::GetReserved2 () const
{
  return m_reserved2;
}

void Ipv6BindingErrorHeader::SetReserved2 (uint8_t reserved2)
{
  m_reserved2 = reserved2;
}

void Ipv6BindingErrorHeader::SetStatus (uint8_t stat)
{
  m_status = stat;
}

uint8_t Ipv6BindingErrorHeader::GetStatus () const
{
  return m_status;
}

Ipv6Address Ipv6BindingErrorHeader::GetHomeAddress () const
{
  return m_hoa;
}

void Ipv6BindingErrorHeader::SetHomeAddress (Ipv6Address hoa)
{
  m_hoa = hoa;
}

uint32_t Ipv6BindingErrorHeader::GetSerializedSize () const
{
  return 25;
}

void Ipv6BindingErrorHeader::Print (std::ostream& os) const
{
  os << "( payload_proto = " << (uint32_t)GetPayloadProto () << " header_len = " << (uint32_t)GetHeaderLen () << " mh_type = " << (uint32_t)GetMhType () << " checksum = " << (uint32_t)GetChecksum ();
  os << " status = " << (uint8_t)GetStatus () << "Home_Address" << (Ipv6Address) GetHomeAddress () << ")";
}

void Ipv6BindingErrorHeader::Serialize (Buffer::Iterator start) const
{
  Buffer::Iterator i = start;
  i.WriteU8 (GetPayloadProto ());

  i.WriteU8 ((uint8_t) (( GetSerializedSize () >> 3) - 1));
  i.WriteU8 (GetMhType ());
  i.WriteU8 (GetReserved ());
  i.WriteU16 (0);

  i.WriteU8 (GetStatus ());

  i.WriteU8 (GetReserved2 ());

  uint8_t buf[16];
  Ipv6Address addr;
  addr = GetHomeAddress ();
  addr.Serialize (buf);
  i.Write (buf,16);
}

uint32_t Ipv6BindingErrorHeader::Deserialize (Buffer::Iterator start)
{
  Buffer::Iterator i = start;

  SetPayloadProto (i.ReadU8 ());
  SetHeaderLen (i.ReadU8 ());
  SetMhType (i.ReadU8 ());
  SetReserved (i.ReadU8 ());
  SetChecksum (i.ReadU16 ());


  SetStatus (i.ReadU8 ());
  SetReserved2 (i.ReadU8 ());
  uint8_t buf[16];
  i.Read (buf,16);
  SetHomeAddress (Ipv6Address::Deserialize (buf));
  return GetSerializedSize ();

}

NS_OBJECT_ENSURE_REGISTERED (Ipv6BindingRefreshRequestHeader);

TypeId Ipv6BindingRefreshRequestHeader::GetTypeId ()
{
  static TypeId tid = TypeId ("ns3::Ipv6BindingRefreshRequestHeader")
    .SetParent<Mipv6Header> ()
    .AddConstructor<Ipv6BindingRefreshRequestHeader> ()
  ;
  return tid;
}
TypeId Ipv6BindingRefreshRequestHeader::GetInstanceTypeId () const
{
  return GetTypeId ();
}
Ipv6BindingRefreshRequestHeader::Ipv6BindingRefreshRequestHeader ()
{
  SetHeaderLen (0);
  SetMhType (IPv6_BINDING_REFRESH_REQUEST);
  SetReserved (0);
  SetChecksum (0);


  SetReserved2 (0);

}
Ipv6BindingRefreshRequestHeader::~Ipv6BindingRefreshRequestHeader ()
{
}
uint16_t Ipv6BindingRefreshRequestHeader::GetReserved2 () const
{
  return m_reserved2;
}

void Ipv6BindingRefreshRequestHeader::SetReserved2 (uint16_t reserved2)
{
  m_reserved2 = reserved2;
}


uint32_t Ipv6BindingRefreshRequestHeader::GetSerializedSize () const
{
  return 8;
}

void Ipv6BindingRefreshRequestHeader::Print (std::ostream& os) const
{
  os << "( payload_proto = " << (uint32_t)GetPayloadProto () << " header_len = " << (uint32_t)GetHeaderLen () << " mh_type = " << (uint32_t)GetMhType () << " checksum = " << (uint32_t)GetChecksum ();

}

void Ipv6BindingRefreshRequestHeader::Serialize (Buffer::Iterator start) const
{
  Buffer::Iterator i = start;
  i.WriteU8 (GetPayloadProto ());

  i.WriteU8 ((uint8_t) (( GetSerializedSize () >> 3) - 1));
  i.WriteU8 (GetMhType ());
  i.WriteU8 (GetReserved ());
  i.WriteU16 (0);


  i.WriteU16 (GetReserved2 ());
}

uint32_t Ipv6BindingRefreshRequestHeader::Deserialize (Buffer::Iterator start)
{
  Buffer::Iterator i = start;

  SetPayloadProto (i.ReadU8 ());
  SetHeaderLen (i.ReadU8 ());
  SetMhType (i.ReadU8 ());
  SetReserved (i.ReadU8 ());
  SetChecksum (i.ReadU16 ());

  SetReserved2 (i.ReadU8 ());
  return GetSerializedSize ();

}




} /* namespace ns3 */
