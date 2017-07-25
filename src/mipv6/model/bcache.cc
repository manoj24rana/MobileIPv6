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

#include "ns3/log.h"
#include "ns3/uinteger.h"
#include "ns3/node.h"
#include "ns3/ipv6-address.h"
#include "bcache.h"
#include "ns3/ptr.h"


namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("BCache");
NS_OBJECT_ENSURE_REGISTERED (BCache);

TypeId BCache::GetTypeId ()
{
  static TypeId tid = TypeId ("ns3::BCache")
    .SetParent<Object> ();
  return tid;
}

BCache::BCache ()
{
  NS_LOG_FUNCTION_NOARGS ();
}

BCache::~BCache ()
{
  NS_LOG_FUNCTION_NOARGS ();
  Flush ();
}

void BCache::DoDispose ()
{
  NS_LOG_FUNCTION_NOARGS ();
  Flush ();
  Object::DoDispose ();
}

BCache::Entry *BCache::Lookup (Ipv6Address mnhoa)
{
  NS_LOG_FUNCTION (this << mnhoa );

  if ( m_bCache.find (mnhoa) != m_bCache.end ())
    {
      BCache::Entry* entry = m_bCache[mnhoa];

      return entry;
    }
  return 0;
}

bool BCache::LookupSHoa (Ipv6Address shoa)
{
  for (BCacheI i = m_bCache.begin (); i != m_bCache.end (); i++)
    {
      if (((*i).second->GetSolicitedHoA ()).IsEqual (shoa))
        {
          return true;
        }
    }
  return false;
}

void BCache::Add (BCache::Entry *bce)
{
  NS_LOG_FUNCTION (this << bce );


  if ( m_bCache.find (bce->GetHoa ()) != m_bCache.end ())
    {
      BCache::Entry* entry2 = m_bCache[bce->GetHoa ()];

      bce->SetNext (entry2);
    }

  m_bCache[bce->GetHoa ()] = bce;

}



void BCache::Remove (BCache::Entry* entry)
{
  NS_LOG_FUNCTION_NOARGS ();

  for (BCacheI i = m_bCache.begin (); i != m_bCache.end (); i++)
    {
      if ((*i).second == entry)
        {
          m_bCache.erase (i);
          delete entry;
          return;
        }
    }
}


void BCache::SetHomePrefixes (std::list<Ipv6Address> halist)
{
  m_HaaList = halist;
  std::list<Ipv6Address> hlist;
  hlist = m_HaaList;
  uint8_t buf1[16],buf2[16];
  while (hlist.size ())
    {
      hlist.front ().GetBytes (buf1);
      for (uint8_t i = 0; i < 16; i++)
        {
          if (i < 8)
            {
              buf2[i] = buf1[i];
            }
          else
            {
              buf2[i] = 0;
            }
        }
      Ipv6Address addr (buf2);
      m_HomePrefixList.push_back (addr);
      hlist.pop_front ();
    }
}



void BCache::Flush ()
{
  NS_LOG_FUNCTION_NOARGS ();

  for (BCacheI i = m_bCache.begin (); i != m_bCache.end (); i++)
    {
      delete (*i).second; /* delete the pointer BindingCache::Entry */
    }

  m_bCache.erase (m_bCache.begin (), m_bCache.end ());
}


Ptr<Node> BCache::GetNode () const
{
  NS_LOG_FUNCTION_NOARGS ();

  return m_node;
}

void BCache::SetNode (Ptr<Node> node)
{
  NS_LOG_FUNCTION ( this << node );

  m_node = node;
}


BCache::Entry::Entry (Ptr<BCache> bcache)
  : m_bCache (bcache),
  m_state (UNREACHABLE),
  m_tunnelIfIndex (-1),
  m_next (0),
  m_homeinitcookie (0x0),
  m_careofinitcookie (0x0),
  m_homekeygentoken (0xFFFFFFFFFFFFFFFF),
  m_careofkeygentoken (0xFFFFFFFFFFFFFFFF),
  m_homenonceindex (0xFF),
  m_careofnonceindex (0xFF)
{
  NS_LOG_FUNCTION_NOARGS ();
}

BCache::Entry *BCache::Entry::Copy ()
{
  NS_LOG_FUNCTION_NOARGS ();

  Entry *bce = new Entry (this->m_bCache);

  bce->SetCoa (this->GetCoa ());
  bce->SetHA (this->GetHA ());
  bce->SetHoa (this->GetHoa ());
  bce->SetTunnelIfIndex (this->GetTunnelIfIndex ());
  bce->SetLastBindingUpdateTime (this->GetLastBindingUpdateTime ());
  bce->SetLastBindingUpdateSequence (this->GetLastBindingUpdateSequence ());

  bce->SetNext (0);
  return bce;
}


bool BCache::Entry::IsUnreachable () const
{
  NS_LOG_FUNCTION_NOARGS ();

  return m_state == UNREACHABLE;
}

void BCache::Entry::MarkReachable ()
{
  NS_LOG_FUNCTION_NOARGS ();

  m_state = REACHABLE;
}



bool BCache::Entry::Match (Ipv6Address mnhoa) const
{
  NS_LOG_FUNCTION ( this << mnhoa );
  NS_ASSERT ( mnhoa == GetHoa () );

  if ( GetHoa () != mnhoa )
    {
      return false;
    }

  return true;
}


Ipv6Address BCache::Entry::GetSolicitedHoA () const
{
  NS_LOG_FUNCTION_NOARGS ();
  return m_shoa;
}

void BCache::Entry::SetSolicitedHoA (Ipv6Address shoa)
{
  NS_LOG_FUNCTION ( this << shoa );
  m_shoa = shoa;
}



Ipv6Address BCache::Entry::GetCoa () const
{
  NS_LOG_FUNCTION_NOARGS ();

  return m_coa;
}

Ipv6Address BCache::Entry::GetHoa () const
{
  NS_LOG_FUNCTION_NOARGS ();

  return m_hoa;
}

Ipv6Address BCache::Entry::GetHA () const
{
  NS_LOG_FUNCTION_NOARGS ();
  return m_haa;
}

void BCache::Entry::SetCoa (Ipv6Address coa)
{
  NS_LOG_FUNCTION ( this << coa );
  m_oldCoa = m_coa;
  m_coa = coa;
}

void BCache::Entry::SetHoa (Ipv6Address hoa)
{
  NS_LOG_FUNCTION ( this << hoa );

  m_hoa = hoa;
}

void BCache::Entry::SetHA (Ipv6Address haa)
{
  NS_LOG_FUNCTION ( this << haa );

  m_haa = haa;
}

int16_t BCache::Entry::GetTunnelIfIndex () const
{
  NS_LOG_FUNCTION_NOARGS ();

  return m_tunnelIfIndex;
}

void BCache::Entry::SetTunnelIfIndex (int16_t tunnelif)
{
  NS_LOG_FUNCTION ( this << tunnelif );

  m_tunnelIfIndex = tunnelif;
}

Time BCache::Entry::GetLastBindingUpdateTime () const
{
  NS_LOG_FUNCTION_NOARGS ();

  return m_lastBindingUpdateTime;
}

void BCache::Entry::SetLastBindingUpdateTime (Time tm)
{
  NS_LOG_FUNCTION ( this << tm );

  m_lastBindingUpdateTime = tm;
}

uint16_t BCache::Entry::GetLastBindingUpdateSequence () const
{
  NS_LOG_FUNCTION_NOARGS ();

  return m_lastBindingUpdateSequence;
}

void BCache::Entry::SetLastBindingUpdateSequence (uint16_t seq)
{
  NS_LOG_FUNCTION ( this << seq);

  m_lastBindingUpdateSequence = seq;
}

BCache::Entry *BCache::Entry::GetNext () const
{
  NS_LOG_FUNCTION_NOARGS ();

  return m_next;
}

void BCache::Entry::SetNext (BCache::Entry *entry)
{
  NS_LOG_FUNCTION ( this << entry );

  m_next = entry;
}

Ipv6Address BCache::Entry::GetOldCoa () const
{
  NS_LOG_FUNCTION_NOARGS ();

  return m_oldCoa;
}

uint64_t BCache::Entry::GetHomeInitCookie () const
{
  NS_LOG_FUNCTION_NOARGS ();

  return m_homeinitcookie;
}

void BCache::Entry::SetHomeInitCookie (uint64_t hcookie)
{
  NS_LOG_FUNCTION_NOARGS ();

  m_homeinitcookie = hcookie;
}

uint64_t BCache::Entry::GetCareOfInitCookie () const
{
  NS_LOG_FUNCTION_NOARGS ();

  return m_careofinitcookie;
}

void BCache::Entry::SetCareOfInitCookie (uint64_t ccookie)
{
  m_careofinitcookie = ccookie;
}

uint64_t BCache::Entry::GetHomeKeygenToken () const
{
  NS_LOG_FUNCTION_NOARGS ();

  return m_homekeygentoken;
}

void BCache::Entry::SetHomeKeygenToken (uint64_t htoken)
{
  NS_LOG_FUNCTION_NOARGS ();

  m_homekeygentoken = htoken;
}

uint64_t BCache::Entry::GetCareOfKeygenToken () const
{
  NS_LOG_FUNCTION_NOARGS ();

  return m_careofkeygentoken;
}

void BCache::Entry::SetCareOfKeygenToken (uint64_t ctoken)
{
  NS_LOG_FUNCTION_NOARGS ();

  m_careofkeygentoken = ctoken;
}

uint16_t BCache::Entry::GetHomeNonceIndex () const
{
  NS_LOG_FUNCTION_NOARGS ();

  return m_homenonceindex;
}

void BCache::Entry::SetHomeNonceIndex (uint16_t hnonce)
{
  NS_LOG_FUNCTION_NOARGS ();

  m_homenonceindex = hnonce;
}

uint16_t BCache::Entry::GetCareOfNonceIndex () const
{
  NS_LOG_FUNCTION_NOARGS ();

  return m_careofnonceindex;
}

void BCache::Entry::SetCareOfNonceIndex (uint16_t cnonce)
{
  NS_LOG_FUNCTION_NOARGS ();

  m_careofnonceindex = cnonce;
}

void BCache::Entry::SetState (BCache::Entry::State_e state)
{
  NS_LOG_FUNCTION_NOARGS ();

  m_addrstate = state;
}

BCache::Entry::State_e BCache::Entry::GetState ()
{
  NS_LOG_FUNCTION_NOARGS ();

  return m_addrstate;
}

}

/* namespace ns3 */
