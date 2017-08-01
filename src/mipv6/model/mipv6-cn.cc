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
#include "ns3/assert.h"
#include "ns3/packet.h"
#include "ns3/node.h"
#include "ns3/ipv6-interface.h"
#include "ns3/ipv6-extension-header.h"
#include "mipv6-header.h"
#include "mipv6-mobility.h"
#include "mipv6-demux.h"
#include "mipv6-l4-protocol.h"
#include "mipv6-cn.h"
#include "ns3/object.h"
#include "ns3/pointer.h"

using namespace std;

NS_LOG_COMPONENT_DEFINE ("Mipv6CN");

namespace ns3 {

NS_OBJECT_ENSURE_REGISTERED (Mipv6CN);

TypeId
Mipv6CN::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::Mipv6CN")
    .SetParent<Mipv6Agent> ()
    .AddConstructor<Mipv6CN> ()
    .AddAttribute ("BCache", "The binding cache associated with this agent.",
                   PointerValue (),
                   MakePointerAccessor (&Mipv6CN::m_bCache),
                   MakePointerChecker<BCache> ())
    .AddTraceSource ("RxBU",
                     "Receive BU packet from MN",
                     MakeTraceSourceAccessor (&Mipv6CN::m_rxbuTrace),
                     "ns3::Mipv6CN::RxBuTracedCallback")


    ;
  return tid;
}


Mipv6CN::Mipv6CN ()
  : m_bCache (0)
{
}

Mipv6CN::~Mipv6CN ()
{
  m_bCache = 0;
}

void Mipv6CN::NotifyNewAggregate ()
{
  if (GetNode () == 0)
    {
      Ptr<Node> node = this->GetObject<Node> ();
      m_bCache = CreateObject<BCache> ();


      SetNode (node);
      m_bCache->SetNode (node);



    }

  Mipv6Agent::NotifyNewAggregate ();
}


Ptr<Packet> Mipv6CN::BuildBA (Ipv6MobilityBindingUpdateHeader bu, Ipv6Address hoa, uint8_t status)
{
  NS_LOG_FUNCTION (this << status << "BUILD BACK");

  Ptr<Packet> p = Create<Packet> ();

  Ipv6MobilityBindingAckHeader ba;
  Ipv6ExtensionType2RoutingHeader type2extn;
  type2extn.SetReserved (0);
  type2extn.SetHomeAddress (hoa);

  ba.SetSequence (bu.GetSequence ());
  ba.SetFlagK (true);
  ba.SetStatus (status);
  ba.SetLifetime ((uint16_t)Mipv6L4Protocol::MAX_BINDING_LIFETIME);
  p->AddHeader (type2extn);
  p->AddHeader (ba);

  return p;
}

Ptr<Packet> Mipv6CN::BuildHoT (Ipv6HoTIHeader hoti, Ipv6Address hoa)
{

  Ptr<Packet> p = Create<Packet> ();
  BCache::Entry *bce;
  bce = m_bCache->Lookup (hoa);

  Ipv6HoTHeader hot;

  hot.SetReserved (0);
  hot.SetHomeInitCookie (hoti.GetHomeInitCookie ());
  hot.SetHomeNonceIndex (bce->GetHomeNonceIndex ());
  hot.SetHomeKeygenToken (bce->GetHomeKeygenToken ());
  Ipv6ExtensionType2RoutingHeader type2extn;
  type2extn.SetReserved (0);
  type2extn.SetHomeAddress (hoa);



  p->AddHeader (type2extn);
  p->AddHeader (hot);

  return p;
}

Ptr<Packet> Mipv6CN::BuildCoT (Ipv6CoTIHeader coti, Ipv6Address hoa)
{

  Ptr<Packet> p = Create<Packet> ();
  BCache::Entry *bce;
  bce = m_bCache->Lookup (hoa);

  Ipv6CoTHeader cot;



  cot.SetReserved (0);
  cot.SetCareOfInitCookie (coti.GetCareOfInitCookie ());
  cot.SetCareOfNonceIndex (bce->GetCareOfNonceIndex ());
  cot.SetCareOfKeygenToken (bce->GetCareOfKeygenToken ());
  Ipv6ExtensionType2RoutingHeader type2extn;
  type2extn.SetReserved (0);
  type2extn.SetHomeAddress (hoa);



  p->AddHeader (type2extn);


  p->AddHeader (cot);

  return p;
}


uint8_t Mipv6CN::HandleBU (Ptr<Packet> packet, const Ipv6Address &src, const Ipv6Address &dst, Ptr<Ipv6Interface> interface)
{
  NS_LOG_FUNCTION (this << packet << src << dst << interface);

  Ptr<Packet> p = packet->Copy ();
  m_rxbuTrace (p, src, dst, interface);

  Ipv6MobilityBindingUpdateHeader bu;
  p->RemoveHeader (bu);

  Ipv6ExtensionDestinationHeader dest;
  p->RemoveHeader (dest);

  Buffer buf;

  Buffer::Iterator start;
  buf = dest.GetOptionBuffer ();
  start = buf.Begin ();
  Ipv6HomeAddressOptionHeader homopt;
  homopt.Deserialize (start);
  Ipv6Address homeaddr;
  homeaddr = homopt.GetHomeAddress ();

  Ptr<Mipv6Demux> ipv6MobilityDemux = GetNode ()->GetObject<Mipv6Demux> ();
  NS_ASSERT (ipv6MobilityDemux);

  Ptr<Mipv6Mobility> ipv6Mobility = ipv6MobilityDemux->GetMobility (bu.GetMhType ());
  NS_ASSERT (ipv6Mobility);


  uint8_t errStatus = 0;
  BCache::Entry *bce = 0;
  BCache::Entry *bce2 = 0;


  bce = m_bCache->Lookup (homeaddr);
  if (bce)
    {
      bce->SetCoa (src);
      bce->SetHoa (homeaddr);
      bce->SetHA (dst);
      bce->SetLastBindingUpdateSequence (bu.GetSequence ());
      bce->SetLastBindingUpdateTime (Time (bu.GetLifetime ()));
      errStatus = Mipv6Header::BA_STATUS_BINDING_UPDATE_ACCEPTED;
      bce->MarkReachable ();
    }

  else
    {
      bce2->SetCoa (src);
      bce2->SetHoa (homeaddr);
      bce2->SetHA (dst);
      bce2->SetLastBindingUpdateSequence (bu.GetSequence ());
      bce2->SetLastBindingUpdateTime (Time (bu.GetLifetime ()));
      errStatus = Mipv6Header::BA_STATUS_BINDING_UPDATE_ACCEPTED;
      bce2->MarkReachable ();

      m_bCache->Add (bce2);
    }

  if (bu.GetFlagA ())
    {
      Ptr<Packet> ba;
      ba = BuildBA (bu, homeaddr, errStatus);

      SendMessage (ba, src, 64);
    }

  return 0;
}
uint8_t Mipv6CN::HandleHoTI (Ptr<Packet> packet, const Ipv6Address &src, const Ipv6Address &dst, Ptr<Ipv6Interface> interface)
{
  NS_LOG_FUNCTION (this << packet << src << dst << interface);

  Ptr<Packet> p = packet->Copy ();

  Ipv6HoTIHeader hoti;
  p->RemoveHeader (hoti);

  Ipv6ExtensionDestinationHeader dest;
  p->RemoveHeader (dest);

  Buffer buf;

  Buffer::Iterator start;
  buf = dest.GetOptionBuffer ();
  start = buf.Begin ();
  Ipv6HomeAddressOptionHeader homopt;
  homopt.Deserialize (start);
  Ipv6Address homeaddr;
  homeaddr = homopt.GetHomeAddress ();

  Ptr<Mipv6Demux> ipv6MobilityDemux = GetNode ()->GetObject<Mipv6Demux> ();
  NS_ASSERT (ipv6MobilityDemux);

  Ptr<Mipv6Mobility> ipv6Mobility = ipv6MobilityDemux->GetMobility (hoti.GetMhType ());
  NS_ASSERT (ipv6Mobility);



  BCache::Entry *bce = 0;


  bce = m_bCache->Lookup (homeaddr);


  if (bce)
    {
      bce->SetHomeInitCookie (hoti.GetHomeInitCookie ());
    }
  else
    {
      BCache::Entry *bce2 = 0;
      bce2->SetHoa (homeaddr);
      bce2->SetHomeInitCookie (hoti.GetHomeInitCookie ());
      m_bCache->Add (bce2);
    }


  Ptr<Packet> hot;
  hot = BuildHoT (hoti, homeaddr);

  SendMessage (hot, src, 64);

  return 0;
}

uint8_t Mipv6CN::HandleCoTI (Ptr<Packet> packet, const Ipv6Address &src, const Ipv6Address &dst, Ptr<Ipv6Interface> interface)
{
  NS_LOG_FUNCTION (this << packet << src << dst << interface);

  Ptr<Packet> p = packet->Copy ();

  Ipv6CoTIHeader coti;
  p->RemoveHeader (coti);

  Ipv6ExtensionDestinationHeader dest;
  p->RemoveHeader (dest);

  Buffer buf;

  Buffer::Iterator start;
  buf = dest.GetOptionBuffer ();
  start = buf.Begin ();
  Ipv6HomeAddressOptionHeader homopt;
  homopt.Deserialize (start);
  Ipv6Address homeaddr;
  homeaddr = homopt.GetHomeAddress ();

  Ptr<Mipv6Demux> ipv6MobilityDemux = GetNode ()->GetObject<Mipv6Demux> ();
  NS_ASSERT (ipv6MobilityDemux);

  Ptr<Mipv6Mobility> ipv6Mobility = ipv6MobilityDemux->GetMobility (coti.GetMhType ());
  NS_ASSERT (ipv6Mobility);



  BCache::Entry *bce = 0;


  bce = m_bCache->Lookup (homeaddr);


  if (bce)
    {
      bce->SetCareOfInitCookie (coti.GetCareOfInitCookie ());
    }
  else
    {
      BCache::Entry *bce2 = 0;
      bce2->SetHoa (homeaddr);
      bce2->SetCareOfInitCookie (coti.GetCareOfInitCookie ());
      m_bCache->Add (bce2);
    }


  Ptr<Packet> cot;
  cot = BuildCoT (coti, homeaddr);

  SendMessage (cot, src, 64);

  return 0;
}


} /* namespace ns3 */

