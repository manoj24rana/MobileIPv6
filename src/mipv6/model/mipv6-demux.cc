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
#include "mipv6-demux.h"
#include "mipv6-mobility.h"

namespace ns3 {

NS_OBJECT_ENSURE_REGISTERED (Mipv6Demux);

TypeId Mipv6Demux::GetTypeId ()
{
  static TypeId tid = TypeId ("ns3::Mipv6Demux")
    .SetParent<Object> ()
    .AddConstructor<Mipv6Demux> ()
    .AddAttribute ("Mobilities", "The set of IPv6 Mobilities registered with this demux.",
                   ObjectVectorValue (),
                   MakeObjectVectorAccessor (&Mipv6Demux::m_mobilities),
                   MakeObjectVectorChecker<Mipv6Mobility> ())
  ;
  return tid;
}

Mipv6Demux::Mipv6Demux ()
{
}

Mipv6Demux::~Mipv6Demux ()
{
}

void Mipv6Demux::DoDispose ()
{
  for (Ipv6MobilityList_t::iterator it = m_mobilities.begin (); it != m_mobilities.end (); it++)
    {
      (*it)->Dispose ();
      *it = 0;
    }
  m_mobilities.clear ();
  m_node = 0;
  Object::DoDispose ();
}

void Mipv6Demux::SetNode (Ptr<Node> node)
{
  m_node = node;
}

void Mipv6Demux::Insert (Ptr<Mipv6Mobility> mobility)
{
  m_mobilities.push_back (mobility);
}

Ptr<Mipv6Mobility> Mipv6Demux::GetMobility (int mobilityNumber)
{
  for (Ipv6MobilityList_t::iterator i = m_mobilities.begin (); i != m_mobilities.end (); ++i)
    {
      if ((*i)->GetMobilityNumber () == mobilityNumber)
        {
          return *i;
        }
    }
  return 0;
}

void Mipv6Demux::Remove (Ptr<Mipv6Mobility> mobility)
{
  m_mobilities.remove (mobility);
}

} /* namespace ns3 */
