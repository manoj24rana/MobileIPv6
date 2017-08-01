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
#include "ns3/node.h"
#include "blist.h"
#include "mipv6-l4-protocol.h"
#include "mipv6-mn.h"

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("BList");
NS_OBJECT_ENSURE_REGISTERED (BList);

TypeId BList::GetTypeId ()
{
  static TypeId tid = TypeId ("ns3::BList")
    .SetParent<Object> ()
  ;
  return tid;
}


BList::~BList ()
{
  NS_LOG_FUNCTION_NOARGS ();
  Flush ();
}

void BList::DoDispose ()
{
  //Flush ();
  Object::DoDispose ();
}


void BList::Flush ()
{
  delete this;
}


Ptr<Node> BList::GetNode () const
{
  NS_LOG_FUNCTION (this);

  return m_node;
}

void BList::SetNode (Ptr<Node> node)
{
  NS_LOG_FUNCTION ( this << node );

  m_node = node;
}

BList::BList (std::list<Ipv6Address> haalist)
  : m_hstate (UNREACHABLE),
  m_tunnelIfIndex (-1),
  m_hpktbu (0),
  m_HaaList (haalist),
  m_hretransTimer (Timer::CANCEL_ON_DESTROY),
  m_hreachableTimer (Timer::CANCEL_ON_DESTROY),
  m_hrefreshTimer (Timer::CANCEL_ON_DESTROY),
  m_cnstate (UNREACHABLE),
  m_cnpktbu (0),
  m_cnretransTimer (Timer::CANCEL_ON_DESTROY),
  m_cnreachableTimer (Timer::CANCEL_ON_DESTROY),
  m_cnrefreshTimer (Timer::CANCEL_ON_DESTROY),
  m_hotiretransTimer (Timer::CANCEL_ON_DESTROY),
  m_cotiretransTimer (Timer::CANCEL_ON_DESTROY),
  m_HomeAddressRegisteredFlag (false)

{
  NS_LOG_FUNCTION_NOARGS ();
}

void BList::SetHomeAddressRegistered (bool flag)
{
  m_HomeAddressRegisteredFlag = flag;
}

bool BList::IsHomeAddressRegistered ()
{
  return m_HomeAddressRegisteredFlag;
}

void BList::FunctionHomeRefreshTimeout ()
{
  NS_LOG_FUNCTION (this);
  Ptr<Mipv6Mn> mn = GetNode ()->GetObject<Mipv6Mn> ();

  if (mn == 0)
    {
      NS_LOG_WARN ("No MN for Binding Update List");

      return;
    }

  SetHomeLastBindingUpdateTime (MicroSeconds (Simulator::Now ().GetMicroSeconds ()));
  SetHomeLastBindingUpdateSequence (mn->GetHomeBUSequence ());

  Ptr<Packet> p = mn->BuildHomeBU ();

  SetHomeBUPacket (p);

  ResetHomeRetryCount ();

  mn->SendMessage (p->Copy (), GetHA (), 64);

  MarkHomeRefreshing ();

  StartHomeRetransTimer ();
}

void BList::FunctionHomeReachableTimeout ()
{
  NS_LOG_FUNCTION (this);

  Ptr<Mipv6Mn> mn = GetNode ()->GetObject<Mipv6Mn>();

  NS_LOG_LOGIC ("Reachable Timeout");

  if ( mn == 0)
    {
      NS_LOG_WARN ("No MN for Binding Update List");

      return;
    }

  if (IsHomeReachable ())
    {
      MarkHomeUnreachable ();
    }
  else if (IsHomeRefreshing ())
    {
      MarkHomeUpdating ();
    }


  //delete routing && tunnel
  if (m_tunnelIfIndex >= 0)
    {
      mn->ClearTunnelAndRouting ();
    }
}

void BList::FunctionHomeRetransTimeout ()
{
  NS_LOG_FUNCTION (this);
  Ptr<Mipv6Mn> mn = GetNode ()->GetObject<Mipv6Mn>();

  if ( mn == 0)
    {
      NS_LOG_WARN ("No MN for Binding Update List");

      return;
    }

  IncreaseHomeRetryCount ();

  if ( GetHomeRetryCount () > Mipv6L4Protocol::MAX_BINDING_UPDATE_RETRY_COUNT )
    {
      NS_LOG_LOGIC ("Maximum retry count reached. Giving up..");

      return;
    }

  mn->SendMessage (GetHomeBUPacket ()->Copy (), GetHA (), 64);

  StartHomeRetransTimer ();
}

bool BList::IsHomeUnreachable () const
{
  NS_LOG_FUNCTION (this);

  return m_hstate == UNREACHABLE;
}

bool BList::IsHomeUpdating () const
{
  NS_LOG_FUNCTION (this);

  return m_hstate == UPDATING;
}

bool BList::IsHomeRefreshing () const
{
  NS_LOG_FUNCTION (this);

  return m_hstate == REFRESHING;
}

bool BList::IsHomeReachable () const
{
  NS_LOG_FUNCTION (this);

  return m_hstate == REACHABLE;
}

void BList::MarkHomeUnreachable ()
{
  NS_LOG_FUNCTION (this);

  m_hstate = UNREACHABLE;
}

void BList::MarkHomeUpdating ()
{
  NS_LOG_FUNCTION (this);

  m_hstate = UPDATING;
}

void BList::MarkHomeRefreshing ()
{
  NS_LOG_FUNCTION (this);

  m_hstate = REFRESHING;
}

void BList::MarkHomeReachable ()
{
  NS_LOG_FUNCTION (this);

  m_hstate = REACHABLE;
}

void BList::StartHomeReachableTimer ()
{
  NS_LOG_FUNCTION (this);
  NS_ASSERT ( !m_hreachableTime.IsZero () );

  m_hreachableTimer.SetFunction (&BList::FunctionHomeReachableTimeout, this);
  m_hreachableTimer.SetDelay ( Seconds (m_hreachableTime.GetSeconds ()));
  m_hreachableTimer.Schedule ();
}

void BList::StopHomeReachableTimer ()
{
  NS_LOG_FUNCTION (this);
  m_hreachableTimer.Cancel ();
}

void BList::StartHomeRetransTimer ()
{
  NS_LOG_FUNCTION (this);
  m_hretransTimer.SetFunction (&BList::FunctionHomeRetransTimeout, this);

  if (GetHomeRetryCount () == 0)
    {
      if (IsHomeAddressRegistered ())
        {
          m_hretransTimer.SetDelay (Seconds (Mipv6L4Protocol::INITIAL_BINDING_ACK_TIMEOUT_FIRSTREG));
        }
      else
        {
          m_hretransTimer.SetDelay (Seconds (Mipv6L4Protocol::INITIAL_BINDING_ACK_TIMEOUT_FIRSTREG + 1.0));
        }
    }
  else
    {
      m_hretransTimer.SetDelay (Seconds (Mipv6L4Protocol::INITIAL_BINDING_ACK_TIMEOUT_REREG));
    }

  m_hretransTimer.Schedule ();
}

void BList::StopHomeRetransTimer ()
{
  NS_LOG_FUNCTION (this);

  m_hretransTimer.Cancel ();
}

void BList::StartHomeRefreshTimer ()
{
  NS_LOG_FUNCTION (this);

  NS_ASSERT ( !m_hreachableTime.IsZero () );

  m_hrefreshTimer.SetFunction (&BList::FunctionHomeRefreshTimeout, this);
  m_hrefreshTimer.SetDelay ( Seconds ( m_hreachableTime.GetSeconds () * 0.9 ) );
  m_hrefreshTimer.Schedule ();
}

void BList::StopHomeRefreshTimer ()
{
  NS_LOG_FUNCTION (this);
  m_hrefreshTimer.Cancel ();
}

Time BList::GetHomeReachableTime () const
{
  NS_LOG_FUNCTION (this);

  return m_hreachableTime;
}

void BList::SetHomeReachableTime (Time tm)
{
  NS_LOG_FUNCTION (this << tm );

  m_hreachableTime = tm;
}

uint8_t BList::GetHomeRetryCount () const
{
  NS_LOG_FUNCTION (this);
  return m_hretryCount;
}

void BList::ResetHomeRetryCount ()
{
  m_hretryCount = 0;
}

void BList::IncreaseHomeRetryCount ()
{
  NS_LOG_FUNCTION (this);
  m_hretryCount++;
}

Time BList::GetHomeInitialLifeTime () const
{
  NS_LOG_FUNCTION (this);
  return m_hinitiallifetime;
}

void BList::SetHomeInitialLifeTime (Time tm)
{
  NS_LOG_FUNCTION ( this << tm );

  m_hinitiallifetime = tm;
}

Time BList::GetHomeRemainingLifeTime () const
{
  NS_LOG_FUNCTION (this);
  return m_hreachableTimer.GetDelayLeft ();
}


Time BList::GetHomeLastBindingUpdateTime () const
{
  NS_LOG_FUNCTION (this);
  return m_hbulastsent;
}

void BList::SetHomeLastBindingUpdateTime (Time tm)
{
  NS_LOG_FUNCTION ( this << tm );
  m_hbulastsent = tm;
}


uint16_t BList::GetHomeLastBindingUpdateSequence () const
{
  NS_LOG_FUNCTION (this);

  return m_hlastBindingUpdateSequence;
}

void BList::SetHomeLastBindingUpdateSequence (uint16_t seq)
{
  NS_LOG_FUNCTION ( this << seq);

  m_hlastBindingUpdateSequence = seq;
}

Ptr<Packet> BList::GetHomeBUPacket () const
{
  NS_LOG_FUNCTION (this);

  return m_hpktbu;
}

void BList::SetHomeBUPacket (Ptr<Packet> pkt)
{
  NS_LOG_FUNCTION ( this << pkt );

  m_hpktbu = pkt;
}

int16_t BList::GetTunnelIfIndex () const
{
  NS_LOG_FUNCTION (this);

  return m_tunnelIfIndex;
}

void BList::SetTunnelIfIndex (int16_t tunnelif)
{
  NS_LOG_FUNCTION ( this << tunnelif );

  m_tunnelIfIndex = tunnelif;
}
void BList::SetHoa (Ipv6Address hoa)
{
  m_hoa = hoa;
}
Ipv6Address BList::GetHoa (void) const
{
  return m_hoa;
}
void BList::SetCoa (Ipv6Address addr)
{
  m_coa = addr;
}
Ipv6Address BList::GetCoa (void) const
{
  NS_LOG_FUNCTION (this);
  return m_coa;
}

Ipv6Address BList::GetHA () const
{
  return m_ha;
}

void BList::SetHA (Ipv6Address ha)
{
  m_ha = ha;
}

std::list<Ipv6Address> BList::GetHomeAgentList () const
{
  return m_HaaList;
}

void BList::SetHomeAgentList (std::list<Ipv6Address> haalist)
{
  m_HaaList = haalist;
}

bool BList::GetHomeBUFlag () const
{
  return m_hflag;
}

void BList::SetHomeBUFlag (bool f)
{
  m_hflag = f;
}

void BList::FunctionCNRefreshTimeout ()
{
  NS_LOG_FUNCTION (this);
  Ptr<Mipv6Mn> mn = GetNode ()->GetObject<Mipv6Mn> ();

  if (mn == 0)
    {
      NS_LOG_WARN ("No MN for Binding Update List");

      return;
    }

  SetCNLastBindingUpdateTime (MicroSeconds (Simulator::Now ().GetMicroSeconds ()));
  SetCNLastBindingUpdateSequence (mn->GetCNBUSequence ());

  Ptr<Packet> p = mn->BuildCNBU ();

  SetCNBUPacket (p);

  ResetCNRetryCount ();

  mn->SendMessage (p->Copy (), GetCN (), 64);

  MarkCNRefreshing ();

  StartCNRetransTimer ();
}

void BList::FunctionCNReachableTimeout ()
{
  NS_LOG_FUNCTION (this);
  Ptr<Mipv6Mn> mn = GetNode ()->GetObject<Mipv6Mn>();

  NS_LOG_LOGIC ("Reachable Timeout");

  if ( mn == 0)
    {
      NS_LOG_WARN ("No MN for Binding Update List");

      return;
    }

  if (IsCNReachable ())
    {
      MarkCNUnreachable ();
    }
  else if (IsCNRefreshing ())
    {
      MarkCNUpdating ();
    }

}

void BList::FunctionCNRetransTimeout ()
{
  NS_LOG_FUNCTION (this);
  Ptr<Mipv6Mn> mn = GetNode ()->GetObject<Mipv6Mn>();

  if ( mn == 0)
    {
      NS_LOG_WARN ("No MN for Binding Update List");

      return;
    }

  IncreaseCNRetryCount ();

  if ( GetCNRetryCount () > Mipv6L4Protocol::MAX_BINDING_UPDATE_RETRY_COUNT )
    {
      NS_LOG_LOGIC ("Maximum retry count reached. Giving up..");

      return;
    }

  mn->SendMessage (GetCNBUPacket ()->Copy (), GetCN (), 64);

  StartCNRetransTimer ();
}

void BList::FunctionHoTIRetransTimeout ()
{
  NS_LOG_FUNCTION (this);
  Ptr<Mipv6Mn> mn = GetNode ()->GetObject<Mipv6Mn>();

  if ( mn == 0)
    {
      NS_LOG_WARN ("No MN for Binding Update List");

      return;
    }

  IncreaseHoTIRetryCount ();

  if ( GetHoTIRetryCount () > Mipv6L4Protocol::MAX_HOTI_RETRY_COUNT )
    {
      NS_LOG_LOGIC ("Maximum retry count reached. Giving up..");

      return;
    }

  mn->SendMessage (GetHoTIPacket ()->Copy (), GetCN (), 64);

  StartHoTIRetransTimer ();
}

void BList::FunctionCoTIRetransTimeout ()
{
  NS_LOG_FUNCTION (this);
  Ptr<Mipv6Mn> mn = GetNode ()->GetObject<Mipv6Mn>();

  if ( mn == 0)
    {
      NS_LOG_WARN ("No MN for Binding Update List");

      return;
    }

  IncreaseCoTIRetryCount ();

  if ( GetCoTIRetryCount () > Mipv6L4Protocol::MAX_COTI_RETRY_COUNT )
    {
      NS_LOG_LOGIC ("Maximum retry count reached. Giving up..");

      return;
    }

  mn->SendMessage (GetCoTIPacket ()->Copy (), GetCN (), 64);

  StartCoTIRetransTimer ();
}

bool BList::IsCNUnreachable () const
{
  NS_LOG_FUNCTION (this);

  return m_cnstate == UNREACHABLE;
}

bool BList::IsCNUpdating () const
{
  NS_LOG_FUNCTION (this);

  return m_cnstate == UPDATING;
}

bool BList::IsCNRefreshing () const
{
  NS_LOG_FUNCTION (this);

  return m_cnstate == REFRESHING;
}

bool BList::IsCNReachable () const
{
  NS_LOG_FUNCTION (this);

  return m_cnstate == REACHABLE;
}

void BList::MarkCNUnreachable ()
{
  NS_LOG_FUNCTION (this);

  m_cnstate = UNREACHABLE;
}

void BList::MarkCNUpdating ()
{
  NS_LOG_FUNCTION (this);

  m_cnstate = UPDATING;
}

void BList::MarkCNRefreshing ()
{
  NS_LOG_FUNCTION (this);

  m_cnstate = REFRESHING;
}

void BList::MarkCNReachable ()
{
  NS_LOG_FUNCTION (this);

  m_cnstate = REACHABLE;
}

void BList::StartCNReachableTimer ()
{
  NS_LOG_FUNCTION (this);
  NS_ASSERT ( !m_cnreachableTime.IsZero () );

  m_cnreachableTimer.SetFunction (&BList::FunctionCNReachableTimeout, this);
  m_cnreachableTimer.SetDelay ( Seconds (m_cnreachableTime.GetSeconds ()));
  m_cnreachableTimer.Schedule ();
}

void BList::StopCNReachableTimer ()
{
  NS_LOG_FUNCTION (this);
  m_cnreachableTimer.Cancel ();
}

void BList::StartCNRetransTimer ()
{
  NS_LOG_FUNCTION (this);
  m_cnretransTimer.SetFunction (&BList::FunctionCNRetransTimeout, this);

  if (GetCNRetryCount () == 0)
    {
      m_cnretransTimer.SetDelay (Seconds (Mipv6L4Protocol::INITIAL_BINDING_ACK_TIMEOUT_FIRSTREG));
    }
  else
    {
      m_cnretransTimer.SetDelay (Seconds (Mipv6L4Protocol::INITIAL_BINDING_ACK_TIMEOUT_REREG));
    }

  m_cnretransTimer.Schedule ();
}

void BList::StopCNRetransTimer ()
{
  NS_LOG_FUNCTION (this);

  m_cnretransTimer.Cancel ();
}

void BList::StartHoTIRetransTimer ()
{
  NS_LOG_FUNCTION (this);
  m_hotiretransTimer.SetFunction (&BList::FunctionHoTIRetransTimeout, this);

  if (GetHoTIRetryCount () == 0)
    {
      m_hotiretransTimer.SetDelay (Seconds (Mipv6L4Protocol::INITIAL_BINDING_ACK_TIMEOUT_FIRSTREG));
    }
  else
    {
      m_hotiretransTimer.SetDelay (Seconds (Mipv6L4Protocol::INITIAL_BINDING_ACK_TIMEOUT_REREG));
    }

  m_hotiretransTimer.Schedule ();
}

void BList::StopHoTIRetransTimer ()
{
  NS_LOG_FUNCTION (this);

  m_hotiretransTimer.Cancel ();
}

void BList::StartCoTIRetransTimer ()
{
  NS_LOG_FUNCTION (this);
  m_cotiretransTimer.SetFunction (&BList::FunctionCoTIRetransTimeout, this);

  if (GetCoTIRetryCount () == 0)
    {
      m_cotiretransTimer.SetDelay (Seconds (Mipv6L4Protocol::INITIAL_BINDING_ACK_TIMEOUT_FIRSTREG));
    }
  else
    {
      m_cotiretransTimer.SetDelay (Seconds (Mipv6L4Protocol::INITIAL_BINDING_ACK_TIMEOUT_REREG));
    }

  m_cotiretransTimer.Schedule ();
}

void BList::StopCoTIRetransTimer ()
{
  NS_LOG_FUNCTION (this);

  m_cotiretransTimer.Cancel ();
}


void BList::StartCNRefreshTimer ()
{
  NS_LOG_FUNCTION (this);

  NS_ASSERT ( !m_cnreachableTime.IsZero () );

  m_cnrefreshTimer.SetFunction (&BList::FunctionCNRefreshTimeout, this);
  m_cnrefreshTimer.SetDelay ( Seconds ( m_cnreachableTime.GetSeconds () * 0.9 ) );
  m_cnrefreshTimer.Schedule ();
}

void BList::StopCNRefreshTimer ()
{
  NS_LOG_FUNCTION (this);
  m_cnrefreshTimer.Cancel ();
}

Time BList::GetCNReachableTime () const
{
  NS_LOG_FUNCTION (this);

  return m_cnreachableTime;
}

void BList::SetCNReachableTime (Time tm)
{
  NS_LOG_FUNCTION (this << tm );

  m_cnreachableTime = tm;
}

uint8_t BList::GetCNRetryCount () const
{
  NS_LOG_FUNCTION (this);

  return m_cnretryCount;
}

void BList::ResetCNRetryCount ()
{
  NS_LOG_FUNCTION (this);

  m_cnretryCount = 0;
}

void BList::IncreaseCNRetryCount ()
{
  NS_LOG_FUNCTION (this);

  m_cnretryCount++;
}

uint8_t BList::GetHoTIRetryCount () const
{
  NS_LOG_FUNCTION (this);

  return m_hotiretryCount;
}

void BList::ResetHoTIRetryCount ()
{
  NS_LOG_FUNCTION (this);

  m_hotiretryCount = 0;
}

void BList::IncreaseHoTIRetryCount ()
{
  NS_LOG_FUNCTION (this);

  m_hotiretryCount++;
}

uint8_t BList::GetCoTIRetryCount () const
{
  NS_LOG_FUNCTION (this);

  return m_cotiretryCount;
}

void BList::ResetCoTIRetryCount ()
{
  NS_LOG_FUNCTION (this);

  m_cotiretryCount = 0;
}

void BList::IncreaseCoTIRetryCount ()
{
  NS_LOG_FUNCTION (this);

  m_cotiretryCount++;
}



Time BList::GetCNInitialLifeTime () const
{
  NS_LOG_FUNCTION (this);

  return m_cninitiallifetime;
}

void BList::SetCNInitialLifeTime (Time tm)
{
  NS_LOG_FUNCTION ( this << tm );

  m_cninitiallifetime = tm;
}

Time BList::GetCNRemainingLifeTime () const
{
  NS_LOG_FUNCTION (this);
  return m_cnreachableTimer.GetDelayLeft ();
}

Time BList::GetCNLastBindingUpdateTime () const
{
  NS_LOG_FUNCTION (this);
  return m_cnbulastsent;
}

void BList::SetCNLastBindingUpdateTime (Time tm)
{
  NS_LOG_FUNCTION ( this << tm );
  m_cnbulastsent = tm;
}


uint16_t BList::GetCNLastBindingUpdateSequence () const
{
  NS_LOG_FUNCTION (this);

  return m_cnlastBindingUpdateSequence;
}

void BList::SetCNLastBindingUpdateSequence (uint16_t seq)
{
  NS_LOG_FUNCTION ( this << seq);

  m_cnlastBindingUpdateSequence = seq;
}

Ptr<Packet> BList::GetCNBUPacket () const
{
  NS_LOG_FUNCTION (this);

  return m_cnpktbu;
}

void BList::SetCNBUPacket (Ptr<Packet> pkt)
{
  NS_LOG_FUNCTION ( this << pkt );

  m_cnpktbu = pkt;
}


void BList::SetCN (Ipv6Address addr)
{
  NS_LOG_FUNCTION (this << addr);
  m_cn = addr;
}
Ipv6Address BList::GetCN (void) const
{
  NS_LOG_FUNCTION (this);
  return m_cn;
}

bool BList::GetCNBUFlag () const
{
  NS_LOG_FUNCTION (this);
  return m_cnflag;
}

void BList::SetCNBUFlag (bool f)
{
  NS_LOG_FUNCTION (this << f);
  m_cnflag = f;
}

uint64_t BList::GetHomeInitCookie () const
{
  NS_LOG_FUNCTION (this);
  return m_homeinitcookie;
}

void BList::SetHomeInitCookie (uint64_t hcookie)
{
  NS_LOG_FUNCTION (this << hcookie);
  m_homeinitcookie = hcookie;
}

uint64_t BList::GetCareOfInitCookie () const
{
  NS_LOG_FUNCTION (this);
  return m_careofinitcookie;
}

void BList::SetCareOfInitCookie (uint64_t ccookie)
{
  NS_LOG_FUNCTION (this << ccookie);
  m_careofinitcookie = ccookie;
}

uint64_t BList::GetHomeKeygenToken () const
{
  NS_LOG_FUNCTION (this);
  return m_homekeygentoken;
}

void BList::SetHomeKeygenToken (uint64_t htoken)
{
  NS_LOG_FUNCTION (this << htoken);
  m_homekeygentoken = htoken;
}

uint64_t BList::GetCareOfKeygenToken () const
{
  NS_LOG_FUNCTION (this);
  return m_careofkeygentoken;
}

void BList::SetCareOfKeygenToken (uint64_t ctoken)
{
  NS_LOG_FUNCTION (this << ctoken);
  m_careofkeygentoken = ctoken;
}

uint16_t BList::GetHomeNonceIndex () const
{
  NS_LOG_FUNCTION (this);
  return m_homenonceindex;
}

void BList::SetHomeNonceIndex (uint16_t hnonce)
{
  NS_LOG_FUNCTION (this << hnonce);
  m_homenonceindex = hnonce;
}

uint16_t BList::GetCareOfNonceIndex () const
{
  NS_LOG_FUNCTION (this);
  return m_careofnonceindex;
}

void BList::SetCareOfNonceIndex (uint16_t cnonce)
{
  NS_LOG_FUNCTION (this << cnonce);
  m_careofnonceindex = cnonce;
}

Ptr<Packet> BList::GetHoTIPacket () const
{
  NS_LOG_FUNCTION (this);
  return m_pkthoti;
}

void BList::SetHoTIPacket (Ptr<Packet> pkt)
{
  NS_LOG_FUNCTION (this << pkt);
  m_pkthoti = pkt;
}

Ptr<Packet> BList::GetCoTIPacket () const
{
  NS_LOG_FUNCTION (this);
  return m_pktcoti;
}

void BList::SetCoTIPacket (Ptr<Packet> pkt)
{
  NS_LOG_FUNCTION (this << pkt);
  m_pktcoti = pkt;
}

} /* namespace ns3 */
