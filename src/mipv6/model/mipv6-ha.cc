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
#include "ns3/ipv6-routing-protocol.h"
#include "ns3/ipv6-l3-protocol.h"
#include "ns3/ipv6-interface.h"
#include "ns3/icmpv6-l4-protocol.h"
#include "ns3/ipv6-extension.h"
#include "ns3/ipv6-extension-header.h"
#include "ns3/callback.h"
#include "ns3/ipv6-static-routing-helper.h"
#include "ns3/ipv6-static-routing.h"
#include "mipv6-header.h"
#include "mipv6-mobility.h"
#include "mipv6-demux.h"
#include "mipv6-l4-protocol.h"
#include "mipv6-tun-l4-protocol.h"
#include "mipv6-ha.h"

using namespace std;

NS_LOG_COMPONENT_DEFINE ("mipv6HA");

namespace ns3 {

NS_OBJECT_ENSURE_REGISTERED (mipv6HA);

mipv6HA::mipv6HA ()
  : m_bCache (0)
{
}

mipv6HA::~mipv6HA ()
{
  m_bCache = 0;
}

void mipv6HA::NotifyNewAggregate ()
{
  if (GetNode () == 0)
    {
      Ptr<Node> node = this->GetObject<Node> ();
      m_bCache = CreateObject<BCache> ();

      SetNode (node);
      m_bCache->SetNode (node);
      Ptr<Icmpv6L4Protocol> icmpv6l4 = node->GetObject<Icmpv6L4Protocol> ();
      Ptr<Ipv6L3Protocol> ipv6 = node->GetObject<Ipv6L3Protocol> ();
      icmpv6l4->SetDADCallback (MakeCallback (&mipv6HA::DADFailureIndication, this));
      icmpv6l4->SetNSCallback (MakeCallback (&mipv6HA::IsAddress, this));
      icmpv6l4->SetHandleNSCallback (MakeCallback (&mipv6HA::HandleNS, this));
      ipv6->SetNSCallback2 (MakeCallback (&mipv6HA::IsAddress2, this));
    }

  mipv6Agent::NotifyNewAggregate ();
}

void mipv6HA::DADFailureIndication (Ipv6Address addr)
{
  BCache::Entry *bce = m_bCache->Lookup (addr);
  if (bce)
    {
      bce->SetState (BCache::Entry::INVALID);
    }
}

bool mipv6HA::IsAddress (Ipv6Address addr)
{
  BCache::Entry *bce = m_bCache->Lookup (addr);
  if (bce)
    {
      return true;
    }
  return false;
}

bool mipv6HA::IsAddress2 (Ipv6Address addr)
{
  return m_bCache->LookupSHoa (addr);
}

Ptr<Packet> mipv6HA::BuildBA (Ipv6MobilityBindingUpdateHeader bu,Ipv6Address hoa, uint8_t status)
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
  ba.SetLifetime ((uint16_t)MIPv6L4Protocol::MAX_BINDING_LIFETIME);
  p->AddHeader (type2extn);
  p->AddHeader (ba);

  return p;
}


uint8_t mipv6HA::HandleBU (Ptr<Packet> packet, const Ipv6Address &src, const Ipv6Address &dst, Ptr<Ipv6Interface> interface)
{
  NS_LOG_FUNCTION (this << packet << src << dst << interface);
  uint32_t r = interface->GetNAddresses ();
  for (uint32_t s = 0; s < r; s++)
    {
      Ipv6Address addr = (interface->GetAddress (s)).GetAddress ();
      NS_LOG_FUNCTION ("IIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIII" << addr);
    }

  Ptr<Packet> p = packet->Copy ();

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

  Ptr<MIPv6Demux> ipv6MobilityDemux = GetNode ()->GetObject<MIPv6Demux> ();
  NS_ASSERT (ipv6MobilityDemux);

  Ptr<MIPv6Mobility> ipv6Mobility = ipv6MobilityDemux->GetMobility (bu.GetMhType ());
  NS_ASSERT (ipv6Mobility);


  uint8_t errStatus = 0;
  BCache::Entry *bce = 0;
  BCache::Entry *bce2 = 0;

  //bool delayedRegister = false;

  bce2 = new BCache::Entry (m_bCache);
  bce2->SetCoa (src);
  bce2->SetHoa (homeaddr);
  bce2->SetState (BCache::Entry::PREFERRED);
  bce2->SetHA (dst);
  bce2->SetSolicitedHoA (Ipv6Address::MakeSolicitedAddress (homeaddr));
  bce2->SetLastBindingUpdateSequence (bu.GetSequence ());
  bce2->SetLastBindingUpdateTime (Time (bu.GetLifetime ()));
  errStatus = MIPv6Header::BA_STATUS_BINDING_UPDATE_ACCEPTED;
  bce2->MarkReachable ();


  Ptr<Packet> ba;
  ba = BuildBA (bu, homeaddr, errStatus);

  bce = m_bCache->Lookup (homeaddr);

  if (src.IsEqual (homeaddr))
    {
      if (bce)
        {
          ClearTunnelAndRouting (bce);
          m_bCache->Remove (bce);
        }
      free (bce2);
      if (bu.GetFlagA ())
        {
          SendMessage (ba, src, 64);
        }
      return 0;
    }


  if (bce)
    {
      ClearTunnelAndRouting (bce);
      m_bCache->Remove (bce);




      m_bCache->Add (bce2);

      if (bu.GetFlagA ())
        {
          SendMessage (ba, src, 64);
          SetupTunnelAndRouting (bce2);
        }

      return 0;


    }

  else
    {
      if (bu.GetFlagA ())
        {
          m_bCache->Add (bce2);
          Simulator::Schedule (Seconds (0.), &mipv6HA::DoDADForOffLinkAddress, this, homeaddr, interface);
          Simulator::Schedule (Seconds (1.), &mipv6HA::FunctionDadTimeoutForOffLinkAddress, this, interface, ba, homeaddr);
        }
      return 0;
    }



}

std::list<Ipv6Address> mipv6HA::HomeAgentAddressList ()
{
  NS_LOG_FUNCTION (this);
  std::list<Ipv6Address> HaaList;
  uint32_t ndevices = GetNode ()->GetNDevices ();
  Ptr<Ipv6L3Protocol> ipv6proto = GetNode ()->GetObject<Ipv6L3Protocol> ();
  Ipv6InterfaceAddress ipv6Addr;
  Ipv6Address addr;
  for (uint32_t i = 0; i < ndevices; i++)
    {
      Ptr<Ipv6Interface> in = ipv6proto->GetInterface (i);
      for (uint32_t j = 0; j < in->GetNAddresses (); j++)
        {
          ipv6Addr = in->GetAddress (j);
          addr = ipv6Addr.GetAddress ();
          if (!addr.IsLocalhost () && !addr.IsLinkLocal ())
            {
              HaaList.push_back (addr);
            }
        }
    }
  m_bCache->SetHomePrefixes (HaaList);
  return HaaList;
}

bool mipv6HA::SetupTunnelAndRouting (BCache::Entry *bce)
{
  NS_LOG_FUNCTION (this << bce);

  //create tunnel
  Ptr<Ipv6TunnelL4Protocol> th = GetNode ()->GetObject<Ipv6TunnelL4Protocol> ();
  NS_ASSERT (th);

  uint16_t tunnelIf = th->AddTunnel (bce->GetCoa ());

  bce->SetTunnelIfIndex (tunnelIf);

  //routing setup by static routing protocol
  Ipv6StaticRoutingHelper staticRoutingHelper;
  Ptr<Ipv6> ipv6 = GetNode ()->GetObject<Ipv6> ();

  Ptr<Ipv6StaticRouting> staticRouting = staticRoutingHelper.GetStaticRouting (ipv6);

  staticRouting->AddHostRouteTo (bce->GetHoa (), bce->GetTunnelIfIndex (),10);
  staticRouting->RemoveRoute ("fe80::", Ipv6Prefix (64), bce->GetTunnelIfIndex (), "fe80::");

  return true;
}


bool mipv6HA::ClearTunnelAndRouting (BCache::Entry *bce)
{
  NS_LOG_FUNCTION (this << bce);

  //routing setup by static routing protocol
  Ipv6StaticRoutingHelper staticRoutingHelper;
  Ptr<Ipv6> ipv6 = GetNode ()->GetObject<Ipv6> ();


  Ptr<Ipv6StaticRouting> staticRouting = staticRoutingHelper.GetStaticRouting (ipv6);

  staticRouting->RemoveRoute (bce->GetHoa (), Ipv6Prefix (64), bce->GetTunnelIfIndex (), bce->GetHoa ());

  //create tunnel
  Ptr<Ipv6TunnelL4Protocol> th = GetNode ()->GetObject<Ipv6TunnelL4Protocol> ();
  NS_ASSERT (th);

  th->RemoveTunnel (bce->GetCoa ());

  bce->SetTunnelIfIndex (-1);
  return true;
}

void mipv6HA::DoDADForOffLinkAddress (Ipv6Address target, Ptr<Ipv6Interface> interface)
{
  NS_LOG_FUNCTION (this << target << interface);
  Ipv6Address addr;
  Ptr<Ipv6L3Protocol> ipv6 = GetNode ()->GetObject<Ipv6L3Protocol> ();
  Ptr<Icmpv6L4Protocol> icmp = GetNode ()->GetObject<Icmpv6L4Protocol> ();

  NS_ASSERT (ipv6);


  /** \todo disable multicast loopback to prevent NS probing to be received by the sender */

  NdiscCache::Ipv6PayloadHeaderPair p = icmp->ForgeNS ("::",Ipv6Address::MakeSolicitedAddress (target), target, interface->GetDevice ()->GetAddress ());

  /* update last packet UID */
  interface->SetNsDadUid (target, p.first->GetUid ());
  Simulator::Schedule (Time (0), &Ipv6Interface::Send, interface, p.first, p.second, Ipv6Address::MakeSolicitedAddress (target));
}

void mipv6HA::FunctionDadTimeoutForOffLinkAddress (Ptr<Ipv6Interface> interface, Ptr<Packet> ba, Ipv6Address homeaddr)
{
  if (m_bCache->Lookup (homeaddr)->GetState () != BCache::Entry::INVALID)
    {
      SendMessage (ba, m_bCache->Lookup (homeaddr)->GetCoa (), 64);
    }
  SetupTunnelAndRouting (m_bCache->Lookup (homeaddr));
}

void mipv6HA::HandleNS (Ptr<Packet> packet, Ptr<Ipv6Interface> interface, Ipv6Address src, Ipv6Address target)
{
  Ipv6InterfaceAddress ifaddr (target);

  if (packet->GetUid () == ifaddr.GetNsDadUid ())
    {
      /* don't process our own DAD probe */
      NS_LOG_LOGIC ("Hey we receive our DAD probe!");
      return;
    }

  Icmpv6OptionLinkLayerAddress lla (1);
  Address hardwareAddress;
  NdiscCache::Entry* entry = 0;
  Ptr<Icmpv6L4Protocol> icmp = GetNode ()->GetObject<Icmpv6L4Protocol> ();
  Ptr<NdiscCache> cache = icmp->GetCache (interface->GetDevice ());
  uint8_t flags = 0;

  /* XXX search all options following the NS header */

  if (src != Ipv6Address::GetAny ())
    {
      uint8_t type;
      packet->CopyData (&type, sizeof(type));

      if (type != Icmpv6Header::ICMPV6_OPT_LINK_LAYER_SOURCE)
        {
          return;
        }

      /* Get LLA */
      packet->RemoveHeader (lla);

      entry = cache->Lookup (src);

      if (!entry)
        {
          entry = cache->Add (src);
          entry->SetRouter (false);
          entry->MarkStale (lla.GetAddress ());
        }
      else if (entry->GetMacAddress () != lla.GetAddress ())
        {
          entry->MarkStale (lla.GetAddress ());
        }

      flags = 3; /* S + O flags */
    }
  else
    {
      /* it means someone do a DAD */
      flags = 1; /* O flag */
    }

  /* send a NA to src */
  Ptr<Ipv6L3Protocol> ipv6 = GetNode ()->GetObject<Ipv6L3Protocol> ();

  if (ipv6->IsForwarding (ipv6->GetInterfaceForDevice (interface->GetDevice ())))
    {
      flags += 4; /* R flag */
    }

  hardwareAddress = interface->GetDevice ()->GetAddress ();
  BCache::Entry *bce = m_bCache->Lookup (target);






  Ptr<Packet> p = Create<Packet> ();
  Ipv6Header ipHeader;
  Icmpv6NA na;
  Icmpv6OptionLinkLayerAddress llOption (0, hardwareAddress);  /* we give our mac address in response */


  /* forge the entire NA packet from IPv6 header to ICMPv6 link-layer option, so that the packet does not pass by Icmpv6L4Protocol::Lookup again */

  p->AddHeader (llOption);
  na.SetIpv6Target (target);

  if ((flags & 1))
    {
      na.SetFlagO (true);
    }
  if ((flags & 2) && bce->GetHA () != Ipv6Address::GetAny ())
    {
      na.SetFlagS (true);
    }
  if ((flags & 4))
    {
      na.SetFlagR (true);
    }

  na.CalculatePseudoHeaderChecksum (bce->GetHA (), src, p->GetSize () + na.GetSerializedSize (), Icmpv6L4Protocol::PROT_NUMBER);
  p->AddHeader (na);

  ipHeader.SetSourceAddress (bce->GetHA ());
  ipHeader.SetDestinationAddress (src);
  ipHeader.SetNextHeader (Icmpv6L4Protocol::PROT_NUMBER);
  ipHeader.SetPayloadLength (p->GetSize ());
  ipHeader.SetHopLimit (255);










  NdiscCache::Ipv6PayloadHeaderPair pi (p, ipHeader);
  interface->Send (pi.first, pi.second, src.IsAny () ? Ipv6Address::GetAllNodesMulticast () : src);

  /* not a NS for us discard it */
}

} /* namespace ns3 */

