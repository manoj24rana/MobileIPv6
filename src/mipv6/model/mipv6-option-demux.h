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

#ifndef MIPV6_OPTION_DEMUX_H
#define MIPV6_OPTION_DEMUX_H

#include <list>

#include "ns3/header.h"
#include "ns3/object.h"
#include "ns3/ptr.h"

namespace ns3 {

class Mipv6Option;
class Node;

/**
 * \class Mipv6OptionDemux
 * \brief Ipv6 Mobility Option Demux.
 */
class Mipv6OptionDemux : public Object
{
public:
  /**
   * \brief The interface ID.
   * \return type ID
   */
  static TypeId GetTypeId (void);

  /**
   * \brief Constructor.
   */
  Mipv6OptionDemux ();

  /**
   * \brief Destructor.
   */
  virtual ~Mipv6OptionDemux ();

  /**
   * \brief Set the node.
   * \param node the node to set
   */
  void SetNode (Ptr<Node> node);

  /**
   * \brief Insert a new IPv6 Mobility Option.
   * \param option the option to insert
   */
  void Insert (Ptr<Mipv6Option> option);

  /**
   * \brief Get the option corresponding to optionNumber.
   * \param optionNumber the option number of the option to retrieve
   * \return a matching IPv6 Mobility option
   */
  Ptr<Mipv6Option> GetOption (int optionNumber);

  /**
   * \brief Remove an option from this demux.
   * \param option pointer on the option to remove
   */
  void Remove (Ptr<Mipv6Option> option);

protected:
  /**
   * \brief Dispose this object.
   */
  virtual void DoDispose ();

private:
  /**
   * \brief List of IPv6 Options supported.
   */
  typedef std::list<Ptr<Mipv6Option> > Ipv6MobilityOptionList_t;

  /**
   * \brief List of IPv6 Options supported.
   */
  Ipv6MobilityOptionList_t m_options;

  /**
   * \brief The node.
   */
  Ptr<Node> m_node;
};

} /* namespace ns3 */

#endif /* IPV6_MOBILITY_OPTION_DEMUX_H */
