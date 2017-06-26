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
class MIPv6OptionHeader : public Header
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
    uint8_t factor;
    uint8_t offset;
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
  MIPv6OptionHeader ();

  /**
   * \brief Destructor.
   */
  virtual ~MIPv6OptionHeader ();

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

class Ipv6MobilityOptionPad1Header : public MIPv6OptionHeader
{
public:
  static TypeId GetTypeId ();
  virtual TypeId GetInstanceTypeId () const;

  Ipv6MobilityOptionPad1Header ();

  virtual ~Ipv6MobilityOptionPad1Header ();

  virtual void Print (std::ostream& os) const;
  virtual uint32_t GetSerializedSize () const;
  virtual void Serialize (Buffer::Iterator start) const;
  virtual uint32_t Deserialize (Buffer::Iterator start);

protected:
private:
};

class Ipv6MobilityOptionPadnHeader : public MIPv6OptionHeader
{
public:
  static TypeId GetTypeId ();
  virtual TypeId GetInstanceTypeId () const;

  Ipv6MobilityOptionPadnHeader ( uint8_t length = 2 );

  virtual ~Ipv6MobilityOptionPadnHeader ();

  virtual void Print (std::ostream& os) const;
  virtual uint32_t GetSerializedSize () const;
  virtual void Serialize (Buffer::Iterator start) const;
  virtual uint32_t Deserialize (Buffer::Iterator start);

protected:
private:
};

class Ipv6MobilityOptionBindingRefreshAdviceHeader : public MIPv6OptionHeader
{
public:
  static TypeId GetTypeId ();
  virtual TypeId GetInstanceTypeId () const;

  Ipv6MobilityOptionBindingRefreshAdviceHeader ();

  virtual ~Ipv6MobilityOptionBindingRefreshAdviceHeader ();


  uint16_t GetRefreshInterval () const;
  void SetRefreshInterval (uint16_t intvl);

  virtual void Print (std::ostream& os) const;
  virtual uint32_t GetSerializedSize () const;
  virtual void Serialize (Buffer::Iterator start) const;
  virtual uint32_t Deserialize (Buffer::Iterator start);
  virtual Alignment GetAlignment () const;
protected:
private:
  uint16_t m_refreshinterval;
};

class Ipv6MobilityOptionAlternateCareofAddressHeader : public MIPv6OptionHeader
{
public:
  static TypeId GetTypeId ();
  virtual TypeId GetInstanceTypeId () const;

  Ipv6MobilityOptionAlternateCareofAddressHeader ();

  virtual ~Ipv6MobilityOptionAlternateCareofAddressHeader ();



  Ipv6Address GetAlternateCareofAddress () const;
  void SetAlternateCareofAddress (Ipv6Address coa);

  virtual void Print (std::ostream& os) const;
  virtual uint32_t GetSerializedSize () const;
  virtual void Serialize (Buffer::Iterator start) const;
  virtual uint32_t Deserialize (Buffer::Iterator start);
  virtual Alignment GetAlignment () const;

protected:
private:
  Ipv6Address m_coa;
};

class Ipv6MobilityOptionNonceIndicesHeader : public MIPv6OptionHeader
{
public:
  static TypeId GetTypeId ();
  virtual TypeId GetInstanceTypeId () const;

  Ipv6MobilityOptionNonceIndicesHeader ();

  virtual ~Ipv6MobilityOptionNonceIndicesHeader ();

  uint16_t GetHomeNonceIndex () const;
  void SetHomeNonceIndex (uint16_t hi);

  uint16_t GetCareOfNonceIndex () const;
  void SetCareOfNonceIndex (uint16_t coi);

  virtual void Print (std::ostream& os) const;
  virtual uint32_t GetSerializedSize () const;
  virtual void Serialize (Buffer::Iterator start) const;
  virtual uint32_t Deserialize (Buffer::Iterator start);
  virtual Alignment GetAlignment () const;
protected:
private:
  uint16_t m_homenonceindex;
  uint16_t m_careofnonceindex;

};

class Ipv6MobilityOptionBindingAuthorizationDataHeader : public MIPv6OptionHeader
{
public:
  static TypeId GetTypeId ();
  virtual TypeId GetInstanceTypeId () const;

  Ipv6MobilityOptionBindingAuthorizationDataHeader ();

  virtual ~Ipv6MobilityOptionBindingAuthorizationDataHeader ();
  //Taking 64 bit authenticator instead of 96 bits
  uint64_t GetAuthenticator () const;
  void SetAuthenticator (uint64_t auth);

  virtual void Print (std::ostream& os) const;
  virtual uint32_t GetSerializedSize () const;
  virtual void Serialize (Buffer::Iterator start) const;
  virtual uint32_t Deserialize (Buffer::Iterator start);
  virtual Alignment GetAlignment () const;
protected:
private:
  uint64_t m_auth;
};


} /* namespace ns3 */

#endif /* IPV6_MOBILITY_OPTION_HEADER_H */
