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
#include "ns3/object-vector.h"
#include "mipv6-option.h"
#include "mipv6-option-demux.h"

namespace ns3 {

NS_OBJECT_ENSURE_REGISTERED (Mipv6OptionDemux);

TypeId Mipv6OptionDemux::GetTypeId ()
{
  static TypeId tid = TypeId ("ns3::Mipv6OptionDemux")
    .SetParent<Object> ()
    .AddAttribute ("MobilityOptions", "The set of IPv6 Mobility options registered with this demux.",
                   ObjectVectorValue (),
                   MakeObjectVectorAccessor (&Mipv6OptionDemux::m_options),
                   MakeObjectVectorChecker<Mipv6Option> ())
  ;
  return tid;
}

Mipv6OptionDemux::Mipv6OptionDemux ()
{
}

Mipv6OptionDemux::~Mipv6OptionDemux ()
{
}

void Mipv6OptionDemux::DoDispose ()
{
  for (Ipv6MobilityOptionList_t::iterator it = m_options.begin (); it != m_options.end (); it++)
    {
      (*it)->Dispose ();
      *it = 0;
    }
  m_options.clear ();
  m_node = 0;
  Object::DoDispose ();
}

void Mipv6OptionDemux::SetNode (Ptr<Node> node)
{
  m_node = node;
}

void Mipv6OptionDemux::Insert (Ptr<Mipv6Option> option)
{
  m_options.push_back (option);
}

Ptr<Mipv6Option> Mipv6OptionDemux::GetOption (int optionNumber)
{
  for (Ipv6MobilityOptionList_t::iterator i = m_options.begin (); i != m_options.end (); ++i)
    {
      if ((*i)->GetMobilityOptionNumber () == optionNumber)
        {
          return *i;
        }
    }
  return 0;
}

void Mipv6OptionDemux::Remove (Ptr<Mipv6Option> option)
{
  m_options.remove (option);
}

} /* namespace ns3 */
