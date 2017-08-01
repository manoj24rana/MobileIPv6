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

#ifndef IPV6_MOBILITY_H
#define IPV6_MOBILITY_H

#include "ns3/object.h"
#include "ns3/node.h"
#include "ns3/ptr.h"
#include "ns3/ipv6-address.h"
#include "ns3/ipv6-interface.h"

namespace ns3 {

class Mipv6OptionBundle;
/**
 * \class Ipv6Mobility
 * \brief Ipv6 Mobility base
 *
 * If you want to implement a new Ipv6 Mobility, all you have to do is
 * implement a subclass of this class and add it to an Ipv6MobilityDemux.
 */
class Mipv6Mobility : public Object
{
public:
  /**
   * \brief Get the type identificator.
   * \return type identificator
   */
  static TypeId GetTypeId (void);

  /**
   * \brief Destructor.
   */
  virtual ~Mipv6Mobility ();

  /**
   * \brief Set the node.
   * \param node the node to set
   */
  void SetNode (Ptr<Node> node);


  /**
   * \brief Set the node.
   * \returns the node
   */
  Ptr<Node> GetNode () const;

  /**
   * \brief Get the option number.
   * \return option number
   */
  virtual uint8_t GetMobilityNumber () const = 0;

  /**
   * \brief Process method, Called from Ipv6MobilityL4Protocol::Receive.
   * \param p the packet
   * \param src the source address
   * \param dst the destination address
   * \param interface IPv6 interface
   * \return the processed size
   */
  virtual uint8_t Process (Ptr<Packet> p, Ipv6Address src, Ipv6Address dst, Ptr<Ipv6Interface> interface) = 0;


  /**
   * \brief Process method, Called from Ipv6MobilityL4Protocol::Receive.
   * \param packet the packet
   * \param offset the offset of the extension to process
   * \param length length
   * \param bundle bundle
   * \return the processed size
   */
  virtual uint8_t ProcessOptions (Ptr<Packet> packet, uint8_t offset, uint8_t length, Mipv6OptionBundle &bundle);

private:
  /**
   * \brief The node.
   */
  Ptr<Node> m_node;
};

/**
 * \class Ipv6MobilityBindingUpdate
 * \brief Ipv6 Mobility Binding Update. If you want to implement a new Ipv6 Mobility,
    all you have to do is implement a subclass of this class and add it to an Ipv6MobilityDemux.
 */
class Ipv6MobilityBindingUpdate : public Mipv6Mobility
{
public:
  /**
   * \brief mobility no.
   */
  static const uint8_t MOB_NUMBER = 5;

  /**
   * \brief Get the type identificator.
   * \return type identificator
   */
  static TypeId GetTypeId (void);

  /**
   * \brief Destructor.
   */
  virtual ~Ipv6MobilityBindingUpdate ();

  /**
   * \brief Get the option number.
   * \return option number
   */
  virtual uint8_t GetMobilityNumber () const;

  /**
   * \brief Process method
   *
   * Called from Ipv6MobilityL4Protocol::Receive.
   * \param p the packet
   * \param src the source address
   * \param dst the destination address
   * \param interface the interface
   * \return the processed size
   */
  virtual uint8_t Process (Ptr<Packet> p, Ipv6Address src, Ipv6Address dst, Ptr<Ipv6Interface> interface);

private:
};

/**
 * \class Ipv6MobilityBindingAck
 * \brief Ipv6 Mobility Binding Acknowledgement
 *
 * If you want to implement a new Ipv6 Mobility, all you have to do is
 * implement a subclass of this class and add it to an Ipv6MobilityDemux.
 */
class Ipv6MobilityBindingAck : public Mipv6Mobility
{
public:
  /**
   * \brief mobility no.
   */
  static const uint8_t MOB_NUMBER = 6;

  /**
   * \brief Get the type identificator.
   * \return type identificator
   */
  static TypeId GetTypeId (void);

  /**
   * \brief Destructor.
   */
  virtual ~Ipv6MobilityBindingAck ();

  /**
   * \brief Get the option number.
   * \return option number
   */
  virtual uint8_t GetMobilityNumber () const;

  /**
   * \brief Process method
   *
   * Called from Ipv6MobilityL4Protocol::Receive.
   * \param p the packet
   * \param src the source address
   * \param dst the destination address
   * \param interface the interface
   * \return the processed size
   */
  virtual uint8_t Process (Ptr<Packet> p, Ipv6Address src, Ipv6Address dst, Ptr<Ipv6Interface> interface);

private:
};

class Ipv6MobilityHoTI : public Mipv6Mobility
{
public:
  /**
   * \brief mobility no.
   */
  static const uint8_t MOB_NUMBER = 1;

  /**
   * \brief Get the type identificator.
   * \return type identificator
   */
  static TypeId GetTypeId (void);

  /**
   * \brief Destructor.
   */
  virtual ~Ipv6MobilityHoTI ();

  /**
   * \brief Get the option number.
   * \return option number
   */
  virtual uint8_t GetMobilityNumber () const;

  /**
   * \brief Process method
   *
   * Called from Ipv6MobilityL4Protocol::Receive.
   * \param p the packet
   * \param src the source address
   * \param dst the destination address
   * \param interface the interface
   * \return the processed size
   */
  virtual uint8_t Process (Ptr<Packet> p, Ipv6Address src, Ipv6Address dst, Ptr<Ipv6Interface> interface);

private:
};

class Ipv6MobilityCoTI : public Mipv6Mobility
{
public:
  /**
   * \brief mobility no.
   */
  static const uint8_t MOB_NUMBER = 2;

  /**
   * \brief Get the type identificator.
   * \return type identificator
   */
  static TypeId GetTypeId (void);

  /**
   * \brief Destructor.
   */
  virtual ~Ipv6MobilityCoTI ();

  /**
   * \brief Get the option number.
   * \return option number
   */
  virtual uint8_t GetMobilityNumber () const;

  /**
   * \brief Process method
   *
   * Called from Ipv6MobilityL4Protocol::Receive.
   * \param p the packet
   * \param src the source address
   * \param dst the destination address
   * \param interface the interface
   * \return the processed size
   */
  virtual uint8_t Process (Ptr<Packet> p, Ipv6Address src, Ipv6Address dst, Ptr<Ipv6Interface> interface);

private:
};

class Ipv6MobilityHoT : public Mipv6Mobility
{
public:
  /**
   * \brief mobility no.
   */
  static const uint8_t MOB_NUMBER = 3;

  /**
   * \brief Get the type identificator.
   * \return type identificator
   */
  static TypeId GetTypeId (void);

  /**
   * \brief Destructor.
   */
  virtual ~Ipv6MobilityHoT ();

  /**
   * \brief Get the option number.
   * \return option number
   */
  virtual uint8_t GetMobilityNumber () const;

  /**
   * \brief Process method
   * Called from Ipv6MobilityL4Protocol::Receive.
   * \param p the packet
   * \param src the source address
   * \param dst the destination address
   * \param interface the interface
   * \return the processed size
   */
  virtual uint8_t Process (Ptr<Packet> p, Ipv6Address src, Ipv6Address dst, Ptr<Ipv6Interface> interface);

private:
};

class Ipv6MobilityCoT : public Mipv6Mobility
{
public:
  /**
   * \brief mobility no.
   */
  static const uint8_t MOB_NUMBER = 4;

  /**
   * \brief Get the type identificator.
   * \return type identificator
   */
  static TypeId GetTypeId (void);

  /**
   * \brief Destructor.
   */
  virtual ~Ipv6MobilityCoT ();

  /**
   * \brief Get the option number.
   * \return option number
   */
  virtual uint8_t GetMobilityNumber () const;

  /**
   * \brief Process method
   *
   * Called from Ipv6MobilityL4Protocol::Receive.
   * \param p the packet
   * \param src the source address
   * \param dst the destination address
   * \param interface the interface
   * \return the processed size
   */
  virtual uint8_t Process (Ptr<Packet> p, Ipv6Address src, Ipv6Address dst, Ptr<Ipv6Interface> interface);

private:
};


class Ipv6BindingRefreshRequest : public Mipv6Mobility
{
public:
  /**
   * \brief mobility no.
   */
  static const uint8_t MOB_NUMBER = 5;

  /**
   * \brief Get the type identificator.
   * \return type identificator
   */
  static TypeId GetTypeId (void);

  /**
   * \brief Destructor.

   */
  virtual ~Ipv6BindingRefreshRequest ();

  /**

   * \brief Get the option number.
   * \return option number
   */
  virtual uint8_t GetMobilityNumber () const;

  /**
   * \brief Process method
   *
   * Called from Ipv6MobilityL4Protocol::Receive.

   * \param p the packet
   * \param src the source address
   * \param dst the destination address
   * \param interface the interface
   * \return the processed size
   */
  virtual uint8_t Process (Ptr<Packet> p, Ipv6Address src, Ipv6Address dst, Ptr<Ipv6Interface> interface);

private:
};

class Ipv6BindingError : public Mipv6Mobility
{
public:
  /**
   * \brief mobility no.
   */
  static const uint8_t MOB_NUMBER = 6;

  /**
   * \brief Get the type identificator.
   * \return type identificator
   */
  static TypeId GetTypeId (void);

  /**
   * \brief Destructor.
   */
  virtual ~Ipv6BindingError ();

  /**
   * \brief Get the option number.
   * \return option number
   */
  virtual uint8_t GetMobilityNumber () const;

  /**
   * \brief Process method
   *
   * Called from Ipv6MobilityL4Protocol::Receive.
   * \param p the packet
   * \param src the source address
   * \param dst the destination address
   * \param interface the interface
   * \return the processed size
   */
  virtual uint8_t Process (Ptr<Packet> p, Ipv6Address src, Ipv6Address dst, Ptr<Ipv6Interface> interface);

private:
};

} /* namespace ns3 */

#endif /* IPV6_MOBILITY_H */
