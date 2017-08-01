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

#ifndef MIPV6_HEADER_H
#define MIPV6_HEADER_H

#include "ns3/header.h"
#include "ns3/ipv6-address.h"
#include "mipv6-option-header.h"


namespace ns3 {

//class Identifier;

class Mipv6Header : public Header
{
public:


  /**
   * \enum MhType_e
   * \brief IPv6 mobility header type
   */
  enum MhType_e
  {
    HOME_TEST_INIT = 1,
    CARE_OF_TEST_INIT = 2,
    HOME_TEST = 3,
    CARE_OF_TEST = 4,
    IPV6_MOBILITY_BINDING_UPDATE = 5,
    IPV6_MOBILITY_BINDING_ACKNOWLEDGEMENT = 6,
    IPv6_BINDING_REFRESH_REQUEST = 7,
    IPv6_BINDING_ERROR = 8

  };
  /**
   * \enum BAStatus_e
   * \brief Binding acknowledgement status type
   */
  enum BAStatus_e
  {
    BA_STATUS_BINDING_UPDATE_ACCEPTED = 0,
    BA_STATUS_PREFIX_DISCOVERY_NECESSARY,
    BA_STATUS_REASON_UNSPECIFIED = 128,
    BA_STATUS_ADMINISTRATIVELY_PROHIBITED,
    BA_STATUS_INSUFFICIENT_RESOURCES = 130,


    BA_STATUS_HOME_REGISTRATION_NOT_SUPPORTED,
    BA_STATUS_NOT_HOME_SUBNET,
    BA_STATUS_NOT_HA_FOR_THIS_MN,
    BA_STATUS_DAD_FAILED,
    BA_STATUS_SEQUENCE_NUMBER_OUT_OF_WINDOW,
    BA_STATUS_EXPIRED_HOME_NONCE_INDEX,
    BA_STATUS_EXPIRED_CARE_OF_NONCE_INDEX,
    BA_STATUS_EXPIRED_NONCES,
    BA_STATUS_REGISTRATION_TYPE_CHANGE_DISALLOWED,
    BA_STATUS_INVALID_COA = 174

  };

  /**
   * \enum OptionType_e
   * \brief IPv6 mobility option type
   */
  enum OptionType_e
  {
    IPV6_MOBILITY_OPT_PAD1 = 0,
    IPV6_MOBILITY_OPT_PADN,
    IPV6_MOBILITY_OPT_BINDING_REFRESH_ADVICE,
    IPV6_MOBILITY_OPT_ALTERNATE_CARE_OF_ADDRESS,
    IPV6_MOBILITY_OPT_NONCE_INDICES,
    IPV6_MOBILITY_OPT_BINDING_AUTHORIZATION_DATA


  };
  /**
   * \brief Get the type identifier.
   * \return type identifier
   */
  static TypeId GetTypeId ();

  /**
   * \brief Get the instance type ID.
   * \return instance type ID
   */
  virtual TypeId GetInstanceTypeId () const;

  /**
   * \brief Constructor.
   */
  Mipv6Header ();

  /**
   * \brief Destructor.
   */
  virtual ~Mipv6Header ();

  /**
   * \brief Get the payload proto field.
   * \return payload proto (MUST BE 59)
   */
  uint8_t GetPayloadProto () const;

  /**
   * \brief Set the payload_proto.
   * \param payload_proto payload protocol
   */
  void SetPayloadProto (uint8_t payload_proto);

  /**
   * \brief Get the header len field.
   * \return the unit of 8 octets
   */
  uint8_t GetHeaderLen () const;

  /**
   * \brief Set the header_len.
   * \param header_len 8octets of header length
   */
  void SetHeaderLen (uint8_t header_len);

  /**
   * \brief Get the mh type field.
   * \return the mh type
   */
  uint8_t GetMhType () const;

  /**
   * \brief Set the mh type.
   * \param mh_type the mh type
   */
  void SetMhType (uint8_t mh_type);

  /**
   * \brief Get the reserved.
   * \return reserved
   */
  uint8_t GetReserved () const;

  /**
   * \brief Set the reserved.
   * \param reserved to set
   */
  void SetReserved (uint8_t reserved);

  /**
   * \brief Get the checksum.
   * \return checksum
   */
  uint16_t GetChecksum () const;

  /**
   * \brief Set the checksum.
   * \param checksum to set
   */
  void SetChecksum (uint16_t checksum);

  /**
   * \brief Print informations.
   * \param os output stream
   */
  virtual void Print (std::ostream& os) const;

  /**
   * \brief Get the serialized size.
   * \return serialized size
   */
  virtual uint32_t GetSerializedSize () const;

  /**
   * \brief Serialize the packet.
   * \param start start offset
   */
  virtual void Serialize (Buffer::Iterator start) const;

  /**
   * \brief Deserialize the packet.
   * \param start start offset
   * \return length of packet
   */
  virtual uint32_t Deserialize (Buffer::Iterator start);

private:
  /**
   * \brief The next header.
   */
  uint8_t m_payload_proto;

  /**
   * \brief The header length.
   */
  uint8_t m_header_len;

  /**
  * \brief The MH type.
  */
  uint8_t m_mh_type;

  /**
   * \brief Reserved.
   */
  uint8_t m_reserved;

  /**
  * \brief The checksum.
  */
  uint16_t m_checksum;

};

/**
 * \brief Option field for an IPv6MobilityHeader
 * Enables adding options to an IPv6MobilityHeader
 * Implementor's note: Make sure to add the result of
 * MobilityOptionField::GetSerializedSize () to your IPv6MobilityHeader::GetSerializedSize ()
 * return value. Call MobilityOptionField::Serialize and MobilityOptionField::Deserialize at the
 * end of your corresponding IPv6MobilityHeader methods.
 */
class Mipv6OptionField
{
public:
  /**
   * \brief Constructor.
   * \param optionsOffset option offset
   */
  Mipv6OptionField (uint32_t optionsOffset);

  /**
   * \brief Destructor.
   */
  ~Mipv6OptionField ();

  /**
   * \brief Get the serialized size of the packet.
   * \return size
   */
  uint32_t GetSerializedSize () const;

  /**
   * \brief Serialize all added options.
   * \param start Buffer iterator
   */
  void Serialize (Buffer::Iterator start) const;

  /**
   * \brief Deserialize the packet.
   * \param start Buffer iterator
   * \param length length
   * \return size of the packet
   */
  uint32_t Deserialize (Buffer::Iterator start, uint32_t length);

  /**
   * \brief Serialize the option, prepending pad1 or padn option as necessary
   * \param option the option header to serialize
   */
  void AddOption (Mipv6OptionHeader const& option);

  /**
   * \brief Get the offset where the options begin, measured from the start of
   * the extension header.
   * \return the offset from the start of the extension header
   */
  uint32_t GetOptionsOffset ();

  /**
   * \brief Get the buffer.
   * \return buffer
   */
  Buffer GetOptionBuffer ();

private:

  /**
   * \brief Calculate padding.
   * \param alignment alignment
   * \return calculated pad
   */
  uint32_t CalculatePad (Mipv6OptionHeader::Alignment alignment) const;

  /**
   * \brief Data payload.
   */
  Buffer m_optionData;

  /**
   * \brief Offset.
   */
  uint32_t m_optionsOffset;
};


class Ipv6MobilityBindingUpdateHeader : public Mipv6Header,
                                        public Mipv6OptionField
{
public:
  /**
   * \brief Get the UID of this class.
   * \return UID
   */
  static TypeId GetTypeId ();

  /**
   * \brief Get the instance type ID.
   * \return instance type ID
   */
  virtual TypeId GetInstanceTypeId () const;

  /**
   * \brief Constructor.
   */
  Ipv6MobilityBindingUpdateHeader ();

  /**
   * \brief Destructor.
   */
  virtual ~Ipv6MobilityBindingUpdateHeader ();

  /**
   * \brief Get the Sequence field.
   * \return sequence value
   */
  uint16_t GetSequence () const;

  /**
   * \brief Set the sequence field.
   * \param sequence the sequence value
   */
  void SetSequence (uint16_t sequence);

  /**
   * \brief Get the A flag.
   * \return A flag
   */
  bool GetFlagA () const;

  /**
   * \brief Set the A flag.
   * \param a value
   */
  void SetFlagA (bool a);

  /**
   * \brief Get the H flag.
   * \return H flag
   */
  bool GetFlagH () const;

  /**
   * \brief Set the H flag.
   * \param h value
   */
  void SetFlagH (bool h);

  /**
   * \brief Get the L flag.
   * \return L flag
   */
  bool GetFlagL () const;

  /**
   * \brief Set the L flag.
   * \param l value
   */
  void SetFlagL (bool l);

  /**
   * \brief Get the K flag.
   * \return K flag
   */
  bool GetFlagK () const;

  /**
   * \brief Set the K flag.
   * \param k value
   */
  void SetFlagK (bool k);



  /**
   * \brief Get the Reserved2 value.
   * \return Reserved2 value
   */

  uint16_t GetReserved2 () const;

  /**
   * \brief Set the Reserved.
   * \param reserved2 reserved value
   */
  void SetReserved2 (uint16_t reserved2);

  /**
   * \brief Get the Lifetime value.
   * \return Lifetime
   */
  uint16_t GetLifetime () const;

  /**
   * \brief Set the Lifetime.
   * \param lifetime lifetime value
   */
  void SetLifetime (uint16_t lifetime);

  /**
   * \brief Print informations.
   * \param os output stream
   */
  virtual void Print (std::ostream& os) const;

  /**
   * \brief Get the serialized size.
   * \return serialized size
   */
  virtual uint32_t GetSerializedSize () const;

  /**
   * \brief Serialize the packet.
   * \param start start offset
   */
  virtual void Serialize (Buffer::Iterator start) const;

  /**
   * \brief Deserialize the packet.
   * \param start start offset
   * \return length of packet
   */
  virtual uint32_t Deserialize (Buffer::Iterator start);

private:
  /**
   * \brief The Sequence field
   */
  uint16_t m_sequence;

  /**
   * \brief The A flag.
   */
  bool m_flagA;

  /**
   * \brief The H flag.
   */
  bool m_flagH;

  /**
   * \brief The L flag.
   */
  bool m_flagL;

  /**
   * \brief The K flag.
   */
  bool m_flagK;

  /**
   * \brief The P flag.
   */
  /**
   * \brief The reserved value.
   */
  uint16_t m_reserved2;

  /**
   * \brief The Lifetime.
   */
  uint16_t m_lifetime;
};

/**
 * \class Ipv6MobilityBindingAckHeader
 * \brief Ipv6 Mobility Binding Acknowledgement header.
 */
class Ipv6MobilityBindingAckHeader : public Mipv6Header,
                                     public Mipv6OptionField
{
public:
  /**
   * \brief Get the UID of this class.
   * \return UID
   */
  static TypeId GetTypeId ();

  /**
   * \brief Get the instance type ID.
   * \return instance type ID
   */
  virtual TypeId GetInstanceTypeId () const;

  /**
   * \brief Constructor.
   */
  Ipv6MobilityBindingAckHeader ();

  /**
   * \brief Destructor.
   */
  virtual ~Ipv6MobilityBindingAckHeader ();

  /**
   * \brief Get the Status field.
   * \return status value
   */
  uint8_t GetStatus () const;

  /**
   * \brief Set the status field.
   * \param status the status value
   */
  void SetStatus (uint8_t status);

  /**
   * \brief Get the K flag.
   * \return K flag
   */
  bool GetFlagK () const;

  /**
   * \brief Set the K flag.
   * \param k value
   */
  void SetFlagK (bool k);

  /**
   * \brief Get the P flag.
   * \return P flag
   */

  uint8_t GetReserved2 () const;

  /**
   * \brief Set the reserved2 field.
   * \param reserved2 the reserved2 value
   */
  void SetReserved2 (uint8_t reserved2);

  /**
   * \brief Get the Sequence field.
   * \return sequence value
   */
  uint16_t GetSequence () const;

  /**
   * \brief Set the sequence field.
   * \param sequence the sequence value
   */
  void SetSequence (uint16_t sequence);

  /**
   * \brief Get the Lifetime value.
   * \return Lifetime
   */
  uint16_t GetLifetime () const;

  /**
   * \brief Set the Lifetime.
   * \param lifetime lifetime value
   */
  void SetLifetime (uint16_t lifetime);

  /**
   * \brief Print informations.
   * \param os output stream
   */
  virtual void Print (std::ostream& os) const;

  /**
   * \brief Get the serialized size.
   * \return serialized size
   */
  virtual uint32_t GetSerializedSize () const;

  /**
   * \brief Serialize the packet.
   * \param start start offset
   */
  virtual void Serialize (Buffer::Iterator start) const;

  /**
   * \brief Deserialize the packet.
   * \param start start offset
   * \return length of packet
   */
  virtual uint32_t Deserialize (Buffer::Iterator start);

private:
  /**
   * \brief The status value.
   */
  uint8_t m_status;

  /**
   * \brief The K flag.
   */
  bool m_flagK;

  /**
   * \brief The R flag.
   */
  //bool m_flagR;

  /**
   * \brief The reserved value.
   */
  uint8_t m_reserved2;

  /**
   * \brief The Sequence field
   */
  uint16_t m_sequence;

  /**
   * \brief The Lifetime.
   */
  uint16_t m_lifetime;
};


class Ipv6HoTIHeader : public Mipv6Header
{
public:
  /**
   * \brief Get the UID of this class.
   * \return UID

   */
  static TypeId GetTypeId ();

  /**
   * \brief Get the instance type ID.
   * \return instance type ID
   */
  virtual TypeId GetInstanceTypeId () const;

  /**
   * \brief Constructor.
   */
  Ipv6HoTIHeader ();

  /**
   * \brief Destructor.
   */
  virtual ~Ipv6HoTIHeader ();
  /**
   * \brief Get the Reserved2 field.
   * \return reserved2 value
   */
  uint16_t GetReserved2 () const;

  /**
   * \brief Set the Reserved2 field.
   * \param reserved2 reserved value
   */
  void SetReserved2 (uint16_t reserved2);
  /**
   * \brief Get the home init cookie.
   * \return home init cookie
   */
  uint64_t GetHomeInitCookie () const;
  /**
   * \brief Set the home init cookie.
   * \param hicookie home init cookie
   */
  void SetHomeInitCookie (uint64_t hicookie);

  /**
   * \brief Print informations.
   * \param os output stream
   */
  virtual void Print (std::ostream& os) const;

  /**
   * \brief Get the serialized size.
   * \return serialized size
   */
  virtual uint32_t GetSerializedSize () const;

  /**
   * \brief Serialize the packet.

   * \param start start offset
   */
  virtual void Serialize (Buffer::Iterator start) const;

  /**
   * \brief Deserialize the packet.
   * \param start start offset
   * \return length of packet
   */
  virtual uint32_t Deserialize (Buffer::Iterator start);

private:
  /**
   * \brief The reserved value.
   */
  uint16_t m_reserved2;
  /**
   * \brief home init cookie.
   */
  uint64_t m_homeinitcookie;

};

class Ipv6CoTIHeader : public Mipv6Header
{
public:
  /**
   * \brief Get the UID of this class.
   * \return UID
   */
  static TypeId GetTypeId ();

  /**
   * \brief Get the instance type ID.
   * \return instance type ID
   */
  virtual TypeId GetInstanceTypeId () const;

  /**
   * \brief Constructor.
   */
  Ipv6CoTIHeader ();

  /**
   * \brief Destructor.
   */
  virtual ~Ipv6CoTIHeader ();
  /**
   * \brief get reserved 2 field.
   */
  uint16_t GetReserved2 () const;

  /**
   * \brief Set the Reserved 2.
   * \param reserved2 reserved value
   */
  void SetReserved2 (uint16_t reserved2);
  /**
   * \brief get care-of init cookie.
   * \return care-of init cookie
   */
  uint64_t GetCareOfInitCookie () const;
  /**
   * \brief set care-of init cookie.
   * \param coaicookie care-of init cookie
   */
  void SetCareOfInitCookie (uint64_t coaicookie);
  /**
   * \brief Print informations.
   * \param os output stream
   */
  virtual void Print (std::ostream& os) const;

  /**
   * \brief Get the serialized size.
   * \return serialized size
   */
  virtual uint32_t GetSerializedSize () const;

  /**
   * \brief Serialize the packet.
   * \param start start offset
   */
  virtual void Serialize (Buffer::Iterator start) const;

  /**
   * \brief Deserialize the packet.
   * \param start start offset
   * \return length of packet
   */
  virtual uint32_t Deserialize (Buffer::Iterator start);

private:
  /**
   * \brief The reserved value.
   */
  uint16_t m_reserved2;
  /**
   * \brief care-of init cookie.
   */
  uint64_t m_careofinitcookie;

};

class Ipv6HoTHeader : public Mipv6Header
{
public:
  /**
   * \brief Get the UID of this class.
   * \return UID
   */
  static TypeId GetTypeId ();

  /**
   * \brief Get the instance type ID.
   * \return instance type ID
   */
  virtual TypeId GetInstanceTypeId () const;

  /**
   * \brief Constructor.
   */
  Ipv6HoTHeader ();

  /**
   * \brief Destructor.
   */
  virtual ~Ipv6HoTHeader ();
  /**
   * \brief Get home nonce index.
   * \return home nonce index
   */
  uint16_t GetHomeNonceIndex () const;

  /**
   * \brief Set the nonce index.
   * \param homenonceind nonce id
   */
  void SetHomeNonceIndex (uint16_t homenonceind);
  /**
   * \brief Get home init cookie.
   * \return home init cookie
   */
  uint64_t GetHomeInitCookie () const;
  /**
   * \brief Set the home init cookie.
   * \param homeinitcookie home init cookie
   */
  void SetHomeInitCookie (uint64_t homeinitcookie);

  /**
   * \brief Get the home keygen token.
   * \return the home keygen token
   */
  uint64_t GetHomeKeygenToken () const;

  /**
   * \brief Set the home keygen token.
   * \param homekeygentoken the home keygen token
   */
  void SetHomeKeygenToken (uint64_t homekeygentoken);

  /**
   * \brief Print informations.
   * \param os output stream
   */
  virtual void Print (std::ostream& os) const;

  /**
   * \brief Get the serialized size.
   * \return serialized size
   */
  virtual uint32_t GetSerializedSize () const;

  /**
   * \brief Serialize the packet.
   * \param start start offset
   */
  virtual void Serialize (Buffer::Iterator start) const;

  /**
   * \brief Deserialize the packet.
   * \param start start offset
   * \return length of packet
   */
  virtual uint32_t Deserialize (Buffer::Iterator start);

private:
  /**
   * \brief The home nonce index.
   */
  uint16_t m_homenonceindex;
  /**
   * \brief The home init cookie.
   */
  uint16_t m_homeinitcookie;
  /**
   * \brief The home keygen token.
   */
  uint16_t m_homekeygentoken;

};

class Ipv6CoTHeader : public Mipv6Header
{
public:
  /**
   * \brief Get the UID of this class.
   * \return UID
   */
  static TypeId GetTypeId ();

  /**
   * \brief Get the instance type ID.
   * \return instance type ID
   */
  virtual TypeId GetInstanceTypeId () const;

  /**
   * \brief Constructor.
   */
  Ipv6CoTHeader ();

  /**
   * \brief Destructor.
   */
  virtual ~Ipv6CoTHeader ();
  /**
   * \brief Get the care-of nonce index.
   * \return the care-of nonce index
   */
  uint16_t GetCareOfNonceIndex () const;

  /**
   * \brief Set care of nonce index.
   * \param careofnonceind care of nonce index
   */
  void SetCareOfNonceIndex (uint16_t careofnonceind);

  /**
   * \brief Get  care-of init cookie.
   * \return care-of init cookie
   */
  uint64_t GetCareOfInitCookie () const;

  /**
   * \brief Set care of init cookie.
   * \param careofinitcookie care of init cookie
   */
  void SetCareOfInitCookie (uint64_t careofinitcookie);

  /**
   * \brief Get care of keygen token.
   * \return care of keygen token
   */
  uint64_t GetCareOfKeygenToken () const;
  /**
   * \brief Set care of keygen token.
   * \param careofkeygentoken care of keygen token
   */
  void SetCareOfKeygenToken (uint64_t careofkeygentoken);

  /**
   * \brief Print informations.
   * \param os output stream
   */
  virtual void Print (std::ostream& os) const;

  /**
   * \brief Get the serialized size.
   * \return serialized size
   */
  virtual uint32_t GetSerializedSize () const;

  /**
   * \brief Serialize the packet.
   * \param start start offset
   */
  virtual void Serialize (Buffer::Iterator start) const;

  /**
   * \brief Deserialize the packet.
   * \param start start offset
   * \return length of packet
   */
  virtual uint32_t Deserialize (Buffer::Iterator start);

private:
  /**
   * \brief The care-of nonce index.
   */
  uint16_t m_careofnonceindex;
  /**
   * \brief The care-of init cookie.
   */
  uint16_t m_careofinitcookie;
  /**
   * \brief The care-of keygen token.
   */
  uint16_t m_careofkeygenoken;

};

class Ipv6BindingRefreshRequestHeader : public Mipv6Header
{
public:
  /**

   * \brief Get the UID of this class.
   * \return UID
   */
  static TypeId GetTypeId ();

  /**
   * \brief Get the instance type ID.
   * \return instance type ID
   */
  virtual TypeId GetInstanceTypeId () const;

  /**
   * \brief Constructor.
   */
  Ipv6BindingRefreshRequestHeader ();

  /**
   * \brief Destructor.
   */
  virtual ~Ipv6BindingRefreshRequestHeader ();
  /**
   * \brief get reserved 2 field value.
   * \return reserved2 field value
   */
  uint16_t GetReserved2 () const;

  /**
   * \brief Set the Reserved2 field value.
   * \param reserved2 reserved2 value
   */
  void SetReserved2 (uint16_t reserved2);

  /**
   * \brief Print informations.
   * \param os output stream
   */
  virtual void Print (std::ostream& os) const;

  /**
   * \brief Get the serialized size.
   * \return serialized size
   */
  virtual uint32_t GetSerializedSize () const;

  /**
   * \brief Serialize the packet.
   * \param start start offset
   */
  virtual void Serialize (Buffer::Iterator start) const;

  /**
   * \brief Deserialize the packet.
   * \param start start offset
   * \return length of packet
   */
  virtual uint32_t Deserialize (Buffer::Iterator start);

private:
  /**
   * \brief The reserved value.
   */
  uint16_t m_reserved2;

};

class Ipv6BindingErrorHeader : public Mipv6Header
{
public:
  /**
   * \brief Get the UID of this class.
   * \return UID
   */
  static TypeId GetTypeId ();

  /**
   * \brief Get the instance type ID.
   * \return instance type ID
   */
  virtual TypeId GetInstanceTypeId () const;

  /**
   * \brief Constructor.
   */
  Ipv6BindingErrorHeader ();

  /**
   * \brief Destructor.
   */
  virtual ~Ipv6BindingErrorHeader ();

  /**
   * \brief Set the status.
   * \param stat status value
   */
  void SetStatus (uint8_t stat);
  /**
   * \brief Get the status value.
   * \return the status value
   */
  uint8_t GetStatus () const;

  /**
   * \brief Get the reserved2.
   * \return reserved 2
   */
  uint8_t GetReserved2 () const;

  /**
   * \brief Set the Reserved2 field value.
   * \param reserved2 reserved2 value
   */
  void SetReserved2 (uint8_t reserved2);
  /**
   * \brief Get the home address.
   * \return home address
   */
  Ipv6Address GetHomeAddress () const;
  /**
   * \brief Set the home address.
   * \param hoa home address
   */
  void SetHomeAddress (Ipv6Address hoa);

  /**
   * \brief Print informations.
   * \param os output stream
   */
  virtual void Print (std::ostream& os) const;

  /**
   * \brief Get the serialized size.
   * \return serialized size
   */
  virtual uint32_t GetSerializedSize () const;

  /**
   * \brief Serialize the packet.
   * \param start start offset
   */
  virtual void Serialize (Buffer::Iterator start) const;

  /**
   * \brief Deserialize the packet.
   * \param start start offset
   * \return length of packet
   */
  virtual uint32_t Deserialize (Buffer::Iterator start);

private:
  /**
   * \brief The status.
   */
  uint8_t m_status;
  /**
   * \brief The reserved value.
   */
  uint16_t m_reserved2;
  /**
   * \brief The home address.
   */
  Ipv6Address m_hoa;

};





} /* namespace ns3 */

#endif /* IPV6_MOBILITY_HEADER_H */
