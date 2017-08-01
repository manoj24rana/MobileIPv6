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

#ifndef MIPV6_OPTION_HEADER_H
#define MIPV6_OPTION_HEADER_H

#include "ns3/header.h"
#include "ns3/ipv6-address.h"
#include "ns3/packet.h"

namespace ns3 {

/**
 * \class Ipv6MobilityOptionHeader
 * \brief Ipv6 Mobility option header.
 */
class Mipv6OptionHeader : public Header
{
public:
  /**
   * \struct Alignment
   * \brief represents the alignment requirements of an option header
   *
   * Represented ad facter*n+offset (eg. 8n+2) See RFC 2460.
   * No alignment is represented as 1n+0
   */
  struct Alignment
  {
    uint8_t factor; /**< Factor */
    uint8_t offset; /**< Offset */
  };

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
  Mipv6OptionHeader ();

  /**
   * \brief Destructor.
   */
  virtual ~Mipv6OptionHeader ();

  /**
   * \brief Get the type of the option.
   * \return type
   */
  uint8_t GetType () const;

  /**
   * \brief Set the type of the option.
   * \param type the type to set
   */
  void SetType (uint8_t type);

  /**
   * \brief Get the length of the option in 8 bytes unit.
   * \return length of the option
   */
  uint8_t GetLength () const;

  /**
   * \brief Set the length of the option.
   * \param len length value to set
   */
  void SetLength (uint8_t len);

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

  /**
   * \brief Get the Alignment requirement of this option header
   * \return the required alignment
   *
   * Subclasses should only implement this method, if special alignment is
   * required. Default is no alignment (1n+0)
   */
  virtual Alignment GetAlignment () const;

private:
  /**
   * \brief The type.
   */
  uint8_t m_type;

  /**
   * \brief The length.
   */
  uint8_t m_len;
};

/**
 * \class Ipv6MobilityOptionPad1Header
 * \brief Ipv6 Mobility option pad1 header.
 */
class Ipv6MobilityOptionPad1Header : public Mipv6OptionHeader
{
public:
  /**
   * \brief Get the type identifier.
   * \return type identifier
   */
  static TypeId GetTypeId ();
  /**
   * \brief Return the instance type identifier.
   * \return instance type ID
   */
  virtual TypeId GetInstanceTypeId () const;

  /**
   * \brief constructor.
   */
  Ipv6MobilityOptionPad1Header ();

  /**
   * \brief destructor.
   */
  virtual ~Ipv6MobilityOptionPad1Header ();
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

protected:
private:
};

/**
 * \class Ipv6MobilityOptionPadnHeader
 * \brief Ipv6 Mobility option padn header.
 */
class Ipv6MobilityOptionPadnHeader : public Mipv6OptionHeader
{
public:
  /**
   * \brief Get the type identifier.
   * \return type identifier
   */
  static TypeId GetTypeId ();
  /**
   * \brief Return the instance type identifier.
   * \return instance type ID
   */
  virtual TypeId GetInstanceTypeId () const;
  /**
   * \brief constructor.
   * \param length length
   */
  Ipv6MobilityOptionPadnHeader ( uint8_t length = 2 );
  /**
   * \brief destructor.
   */
  virtual ~Ipv6MobilityOptionPadnHeader ();
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

protected:
private:
};

/**
 * \class Ipv6MobilityOptionBindingRefreshAdviceHeader
 * \brief Ipv6Mobility Option Binding Refresh Advice Header.
 */
class Ipv6MobilityOptionBindingRefreshAdviceHeader : public Mipv6OptionHeader
{
public:
  /**
   * \brief Get the type identifier.
   * \return type identifier
   */
  static TypeId GetTypeId ();
  /**
   * \brief Return the instance type identifier.
   * \return instance type ID
   */
  virtual TypeId GetInstanceTypeId () const;
  /**
   * \brief constructor
   */
  Ipv6MobilityOptionBindingRefreshAdviceHeader ();
  /**
   * \brief destructor
   */
  virtual ~Ipv6MobilityOptionBindingRefreshAdviceHeader ();

  /**
   * \brief get refresh interval.
   * \return refresh interval
   */
  uint16_t GetRefreshInterval () const;
  /**
   * \brief set refresh interval.
   * \param intvl refresh interval
   */
  void SetRefreshInterval (uint16_t intvl);
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
  /**
   * \brief Get the Alignment requirement of this option header
   * \return the required alignment
   */
  virtual Alignment GetAlignment () const;
protected:
private:
  /**
   * \brief refresh interval
   */
  uint16_t m_refreshinterval;
};

/**
 * \class Ipv6MobilityOptionAlternateCareofAddressHeader
 * \brief Ipv6 Mobility Option Alternate CareofAddress Header.
 */
class Ipv6MobilityOptionAlternateCareofAddressHeader : public Mipv6OptionHeader
{
public:
  /**
   * \brief Get the type identifier.
   * \return type identifier
   */
  static TypeId GetTypeId ();
  /**
   * \brief Return the instance type identifier.
   * \return instance type ID
   */
  virtual TypeId GetInstanceTypeId () const;
  /**
   * \brief constructor
   */
  Ipv6MobilityOptionAlternateCareofAddressHeader ();
  /**
   * \brief destructor
   */
  virtual ~Ipv6MobilityOptionAlternateCareofAddressHeader ();


  /**
   * \brief get alternate CoA.
   * \return alternate CoA
   */
  Ipv6Address GetAlternateCareofAddress () const;
  /**
   * \brief set alternate CoA.
   * \param coa alternate CoA
   */
  void SetAlternateCareofAddress (Ipv6Address coa);
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
  /**
   * \brief Get the Alignment requirement of this option header
   * \return the required alignment
   */
  virtual Alignment GetAlignment () const;

protected:
private:
  /**
   * \brief the alternate CoA
   */
  Ipv6Address m_coa;
};
/**
 * \class Ipv6MobilityOptionNonceIndicesHeader
 * \brief Ipv6 Mobility Option Nonce Indices Header.
 */
class Ipv6MobilityOptionNonceIndicesHeader : public Mipv6OptionHeader
{
public:
  /**
   * \brief Get the type identifier.
   * \return type identifier
   */
  static TypeId GetTypeId ();
  /**
   * \brief Return the instance type identifier.
   * \return instance type ID
   */
  virtual TypeId GetInstanceTypeId () const;
  /**
   * \brief constructor
   */
  Ipv6MobilityOptionNonceIndicesHeader ();
  /**
   * \brief destructor
   */
  virtual ~Ipv6MobilityOptionNonceIndicesHeader ();
  /**
   * \brief get home nonce index
   * \return home nonce index
   */
  uint16_t GetHomeNonceIndex () const;
  /**
   * \brief set home nonce index
   * \param hi home nonce index
   */
  void SetHomeNonceIndex (uint16_t hi);
  /**
   * \brief get care-of nonce index
   * \return care-of nonce index
   */
  uint16_t GetCareOfNonceIndex () const;
  /**
   * \brief set care-of nonce index
   * \param coi care-of nonce index
   */
  void SetCareOfNonceIndex (uint16_t coi);
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
  /**
   * \brief Get the Alignment requirement of this option header
   * \return the required alignment
   */
  virtual Alignment GetAlignment () const;
protected:
private:
  /**
   * \brief home nonce index
   */
  uint16_t m_homenonceindex;
  /**
   * \brief care-of nonce index
   */
  uint16_t m_careofnonceindex;

};
/**
 * \class Ipv6MobilityOptionBindingAuthorizationDataHeader
 * \brief Ipv6 Mobility Option Binding Authorization Data Header.
 */
class Ipv6MobilityOptionBindingAuthorizationDataHeader : public Mipv6OptionHeader
{
public:
  /**
   * \brief Get the type identifier.
   * \return type identifier
   */
  static TypeId GetTypeId ();
  /**
   * \brief Return the instance type identifier.
   * \return instance type ID
   */
  virtual TypeId GetInstanceTypeId () const;
  /**
   * \brief constructor
   */
  Ipv6MobilityOptionBindingAuthorizationDataHeader ();
  /**
   * \brief destructor
   */
  virtual ~Ipv6MobilityOptionBindingAuthorizationDataHeader ();
  //Taking 64 bit authenticator instead of 96 bits
  /**
   * \brief get authenticator
   * \return authenticator
   */
  uint64_t GetAuthenticator () const;
  /**
   * \brief set authenticator
   * \param auth authenticator
   */
  void SetAuthenticator (uint64_t auth);
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
  /**
   * \brief Get the Alignment requirement of this option header
   * \return the required alignment
   */
  virtual Alignment GetAlignment () const;
protected:
private:
  /**
   * \brief the authenticator
   */
  uint64_t m_auth;
};


} /* namespace ns3 */

#endif /* IPV6_MOBILITY_OPTION_HEADER_H */
