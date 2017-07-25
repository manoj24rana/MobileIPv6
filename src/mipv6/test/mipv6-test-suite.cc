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
#include <fstream>
#include "ns3/log.h"
#include "ns3/abort.h"
#include "ns3/config.h"
#include "ns3/string.h"
#include "ns3/uinteger.h"

#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/applications-module.h"
#include "ns3/mobility-module.h"
#include "ns3/wifi-module.h"
#include "ns3/csma-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/bridge-module.h"
#include "ns3/ipv6-static-routing.h"
#include "ns3/ipv6-routing-table-entry.h"
#include "ns3/mipv6-module.h"
#include "ns3/internet-trace-helper.h"
#include "ns3/trace-helper.h"
#include "ns3/internet-apps-module.h"
#include "ns3/radvd.h"
#include "ns3/radvd-interface.h"
#include "ns3/radvd-prefix.h"
#include <iostream>
#include <iomanip>
#include <fstream>
#include <vector>
#include <string>
#include "ns3/test.h"
#include "ns3/simulator.h"
#include "ns3/ptr.h"
#include "ns3/object.h"

/* *
   * Scenarios: 1) without handoff and 2) with handoff of an MN

                 Classes: NoHandoffTestCase and HandoffTestCase

   * Pass criteria: 1) Every received packet (whether after or, before handoff it is received)
                       must be tunneled properly (as defined in RFC 6275) and
                    2) All handoffs must be successful.

   * Use Cases:   1) Data transmission direction: MN to CN, CN to MN 
                  2) Data transmission rate change: 10 Kbps, 100 Kbps
                           Both scenario use both of these use cases

   * Status (expected):      1) Only first criteria must be fulfilled in first scenario and
                             2) both criteria must be fulfilled in second scenario 

   * Process:  1) (All received packets have passed through HA-MN tunnel) && (All tunneled packets
                  use right tunnel identifiers) = Every received packet have tunneled properly.

                      Corresponding functions: TxPktAtTun (), RxPktAtTun () and CheckForNoHandoff ()

               2) (A successful BU-BA transmission, at handoff) && (HA's Cache is updated with MN's new CoA)
                  (Packets are successfully transmitted using this new tunnel) = A handoff is successful
  
                    Functions: RxBA () and CheckForHandoff ()
                    Variables: pkt_total_counter and pkt_handoff_counter.


Note: We only consider received packets and received Binding packets because sent packets and BUs may be lost
      due to link failure or, handoff operation or, any other issues

* */

using namespace ns3;

// Functions and variables used for all scenario classes


class NoHandoffTestCase : public TestCase
{
public:
  NoHandoffTestCase ();
  virtual ~NoHandoffTestCase ();
  void CheckForNoHandoff ();
  void Initialize (uint8_t dir, double interval, uint32_t maxpktcount, double start, double stop);
  void InstallMIPv6 (Ptr<Node> m, Ptr<Node> h, Ptr<Node> c);
  void InstallApplication (double interval, uint32_t maxpktcount, double starttime, double stoptime);
  void RxPktAtTun (Ptr<Packet> p, Ipv6Header ih, Ipv6Header oh, Ptr<Ipv6Interface> i);
  void TxPktAtTun (Ptr<Packet> p, Ipv6Header ih, Ipv6Header oh);
  void TxPktAtClient (Ptr<const Packet> p, Ptr<Ipv6> ipv6, uint32_t interface);
  void RxPktAtServer (Ptr<const Packet> p, Ptr<Ipv6> ipv6, uint32_t interface);
  void TxBU (Ptr<Packet> packet, Ipv6Address src, Ipv6Address dst);
  void RxBA (Ptr<Packet> packet, Ipv6Address src, Ipv6Address dst, Ptr<Ipv6Interface> interface);
  void SetCallback ();
  void MakeCallbackNull();

private:
  virtual void DoRun (void);
  Ipv6Address clientaddress, serveraddress;
  uint16_t appport;
  Ptr<Node> mn;
  Ptr<Node> ha;
  Ptr<Node> cn;
  int pktcounter;
  int pktcounterathandoff;
  uint8_t d; // False if MN to CN and true if CN to MN
  std::list<uint64_t> list1;  // contains received packet's UID at server
  std::list<uint64_t> list2;  // contains received packet's UID at HA
  std::list<uint64_t> list3;  // contains received packet's UID at client
  std::list<Ptr<Packet> > bulist;  // contains sent BUs of an MN
  bool pkt_tun_recv, pkt_tun_sent, bu_seq, bu_found, mn_found_ha, coa_match_ha;
};


// Add some help text to this case to describe what it is intended to test
NoHandoffTestCase::NoHandoffTestCase ()
  : TestCase ("Test two mipv6 tests for no handoff")
{
appport=4000;
d=0;
pkt_tun_recv=true;
pkt_tun_sent=true;
bu_seq=true;
bu_found=true;
mn_found_ha=true;
coa_match_ha=true;
}

// This destructor does nothing but we include it as a reminder that
// the test case should clean up after itself
NoHandoffTestCase::~NoHandoffTestCase ()
{
}



//
// This method is the pure virtual method from class TestCase that every
// TestCase must implement
//
void
NoHandoffTestCase::DoRun (void)
{
NodeContainer n;
n.Create (4);

InternetStackHelper internet;
internet.Install (n);





  // link three nodes
  PointToPointHelper pointToPoint;
  pointToPoint.SetDeviceAttribute ("DataRate", StringValue ("5Mbps"));
  pointToPoint.SetChannelAttribute ("Delay", StringValue ("2ms"));


  NetDeviceContainer d1, d2, d3, d4, d5;

  d1 = pointToPoint.Install (n.Get (0), n.Get (1));
  d2 = pointToPoint.Install (n.Get (1), n.Get (2));
  d3 = pointToPoint.Install (n.Get (2), n.Get (3));
  d4 = NetDeviceContainer (d3.Get(0));
  d5 = NetDeviceContainer (d3.Get(1));


  Ipv6AddressHelper ipv61,ipv62,ipv63;
  Ipv6InterfaceContainer iifc,iifc1,iifc2,iifc3,iifc4;


  ipv61.SetBase (Ipv6Address ("1111::"), Ipv6Prefix (64));
  iifc = ipv61.Assign (d1);
  iifc1.Add(iifc);
  iifc1.SetForwarding (0, true);
  iifc1.SetDefaultRouteInAllNodes (0);
  iifc1.SetForwarding (1, true);
  iifc1.SetDefaultRouteInAllNodes (1);

  ipv62.SetBase (Ipv6Address ("2222::"), Ipv6Prefix (64));
  iifc = ipv62.Assign (d2);
  iifc2.Add(iifc);
  iifc2.SetForwarding (0, true);
  iifc2.SetDefaultRouteInAllNodes (0);
  iifc2.SetForwarding (1, true);
  iifc2.SetDefaultRouteInAllNodes (1);

  ipv63.SetBase (Ipv6Address ("3333::"), Ipv6Prefix (64));
  iifc = ipv63.Assign (d4);
  iifc3.Add(iifc);
  iifc3.SetForwarding (0, true);
  iifc3.SetDefaultRouteInAllNodes (0);

  iifc = ipv63.AssignWithoutAddress (d5);
  iifc4.Add(iifc);


  Ipv6Address prefix ("3333::");  //create the prefix 

  uint32_t indexRouter = iifc3.GetInterfaceIndex (0);  //AR interface (n-AR1) 

  Ptr<Radvd> radvd=CreateObject<Radvd> ();
  Ptr<RadvdInterface> routerInterface= Create<RadvdInterface> (indexRouter, 1500, 50);
  Ptr<RadvdPrefix> routerPrefix = Create<RadvdPrefix> (prefix, 64, 1.5, 2.0);

  routerInterface->AddPrefix(routerPrefix);

  radvd->AddConfiguration(routerInterface);

  n.Get(2)->AddApplication(radvd);
  radvd->SetStartTime(Seconds (1.0));
  radvd->SetStopTime(Seconds (200.0));



  Ipv6StaticRoutingHelper routingHelper;
  Ptr<Ipv6> ipv692 = n.Get(1)->GetObject<Ipv6> ();
  Ptr<Ipv6StaticRouting> rttop = routingHelper.GetStaticRouting(ipv692);
  rttop->AddNetworkRouteTo(Ipv6Address("3333::"),Ipv6Prefix(64),Ipv6Address("2222::200:ff:fe00:4"),2,0);

  Ptr<Ipv6> ipv693 = n.Get(2)->GetObject<Ipv6> ();
  rttop = routingHelper.GetStaticRouting(ipv693);
  rttop->AddNetworkRouteTo(Ipv6Address("1111::"),Ipv6Prefix(64),Ipv6Address("2222::200:ff:fe00:3"),1,0);


  InstallMIPv6 (n.Get(3), n.Get(1), n.Get(0));





  Initialize (0, 0.002, 5000, 4, 15);

  Simulator::Schedule (Time (Seconds (16.0)), &NoHandoffTestCase::CheckForNoHandoff, this);

//  Simulator::Schedule (Time (Seconds (16.5)), &NoHandoffTestCase::MakeCallbackNull, this);

//  Simulator::Schedule (Time (Seconds (17.0)), &NoHandoffTestCase::Initialize, this, 1, 0.02, 500, 4, 15);

//  Simulator::Schedule (Time (Seconds (35.0)), &NoHandoffTestCase::CheckForNoHandoff, this);

//  Simulator::Schedule (Time (Seconds (36.0)), &NoHandoffTestCase::Initialize, this, 1, 0.02, 500);

//  Simulator::Schedule (Time (Seconds (52.0)), &NoHandoffTestCase::CheckForNoHandoff, this);

//  Simulator::Schedule (Time (Seconds (53.0)), &NoHandoffTestCase::Initialize, this, 1, 0.02, 500, 54, 69);

//  Simulator::Schedule (Time (Seconds (70.0)), &NoHandoffTestCase::CheckForNoHandoff, this);



  Simulator::Stop (Seconds (18.0));
  Simulator::Run ();
  Simulator::Destroy ();

}

void NoHandoffTestCase::Initialize (uint8_t dir, double interval, uint32_t maxpktcount,  double start, double stop)
{
d=dir;
Ptr<mipv6MN> mipmn = mn->GetObject<mipv6MN> ();


if (d==0)
{
  clientaddress=mipmn->GetHomeAddress ();
  serveraddress=Ipv6Address ("1111::200:ff:fe00:1");
}
else
{
  clientaddress=Ipv6Address ("1111::200:ff:fe00:1");
  serveraddress=mipmn->GetHomeAddress ();
}
pktcounter=0;
pktcounterathandoff=0;
list1.clear ();
list2.clear ();
list3.clear ();
bulist.clear ();
pkt_tun_recv=true; pkt_tun_sent=true; bu_seq=true; bu_found=true; mn_found_ha=true; coa_match_ha=true;
SetCallback ();
InstallApplication (interval, maxpktcount, start, stop);
}


void NoHandoffTestCase::CheckForNoHandoff ()
{
bool f = true;
std::list<uint64_t>::iterator it, it1, it2;
for (it=list1.begin(); it!=list1.end(); ++it)
  {
    it1 = std::find (list2.begin (), list2.end (), *it);
    it2 = std::find (list3.begin (), list3.end (), *it);
    if(it1 != list2.end () && it2 != list3.end())
      {
        //std::cout<< "\nInside Check:" << *it1 << *it2 << *it << "\n";
        list2.erase (it1);
        list3.erase (it2);
      }
    else
      {
        f = false;
        break;
      } 
  }
list1.clear();
if(list1.size () != 0 || list2.size() != 0)
  f=false;

NS_TEST_ASSERT_MSG_EQ (f, true, "Failure Happens in MIPv6 test...");
NS_TEST_ASSERT_MSG_GT (pktcounter, pktcounterathandoff, "Handoff is not successful");
NS_TEST_ASSERT_MSG_EQ (pkt_tun_recv, true, "Failure Happens in Tunneled packet...");
NS_TEST_ASSERT_MSG_EQ (pkt_tun_sent, true, "Failure Happens in Tunneled packet...");
NS_TEST_ASSERT_MSG_EQ (bu_seq, true, "Failure Happens in Tunneled packet...");
NS_TEST_ASSERT_MSG_EQ (bu_found, true, "Failure Happens in Tunneled packet...");
NS_TEST_ASSERT_MSG_EQ (mn_found_ha, true, "Failure Happens in Tunneled packet...");
NS_TEST_ASSERT_MSG_EQ (coa_match_ha, true, "Failure Happens in Tunneled packet...");
}


void NoHandoffTestCase::InstallMIPv6 (Ptr<Node> m, Ptr<Node> h, Ptr<Node> c)
{
  mipv6HAHelper hh;
  hh.Install (h);
  mipv6MNHelper mh (hh.GetHomeAgentAddressList(),false); 
  mh.Install (m);

  mn=m;
  ha=h;
  cn=c;
std::cout<< "uio\n";
}


void NoHandoffTestCase::InstallApplication (double interval, uint32_t maxpktcount, double starttime, double stoptime)
{
  uint16_t port = appport;
  UdpServerHelper server (port);
  ApplicationContainer apps;
  if (d)
    {
      apps = server.Install (mn);
    }
  else
    {
      apps = server.Install (cn);      
    }
  apps.Start (Seconds (starttime));
  apps.Stop (Seconds (stoptime));



  uint32_t MaxPacketSize = 1024;
  Time interPacketInterval = Seconds (interval);
  uint32_t maxPacketCount = maxpktcount;
  UdpClientHelper client (serveraddress, port);

  client.SetAttribute ("MaxPackets", UintegerValue (maxPacketCount));
  client.SetAttribute ("Interval", TimeValue (interPacketInterval));
  client.SetAttribute ("PacketSize", UintegerValue (MaxPacketSize));
  if (d)
    {
      apps = client.Install (cn);
    }
  else
    {
      apps = client.Install (mn);
    }
  apps.Start (Seconds (starttime));
  apps.Stop (Seconds (stoptime-1.0));
 
appport++;
std::cout<< "jio\n";
}


void NoHandoffTestCase::RxPktAtTun (Ptr<Packet> p, Ipv6Header ih, Ipv6Header oh, Ptr<Ipv6Interface> i)
{
  pktcounter++;
  if(!d)
    {
      if (ih.GetSourceAddress ().IsEqual(clientaddress) && ih.GetDestinationAddress ().IsEqual(serveraddress))
        {
          list2.push_back (p->GetUid ());
          Ptr<mipv6MN> mipmn = mn->GetObject<mipv6MN>();
          bool b=(oh.GetSourceAddress ().IsEqual(mipmn->GetCoA()) && mipmn->IsHomeMatch(oh.GetDestinationAddress ()));
          pkt_tun_recv &=b;
std::cout<< oh.GetSourceAddress ()<< oh.GetDestinationAddress () << "mio\n";
          
        }
    }
  else
    {
      if (ih.GetSourceAddress ().IsEqual(clientaddress) && ih.GetDestinationAddress ().IsEqual(serveraddress))
        {
          list1.push_back (p->GetUid ());
          Ptr<mipv6MN> mipmn = mn->GetObject<mipv6MN>();
          bool b=(oh.GetDestinationAddress ().IsEqual(mipmn->GetCoA()) && mipmn->IsHomeMatch(oh.GetSourceAddress ()));
          pkt_tun_recv &=b;
        }

    }
}

void NoHandoffTestCase::TxPktAtTun (Ptr<Packet> p, Ipv6Header ih, Ipv6Header oh)
{
std::cout<< "lio\n";
  if(!d)
    {
      if (ih.GetSourceAddress ().IsEqual(clientaddress) && ih.GetDestinationAddress ().IsEqual(serveraddress))
        {
          list3.push_back (p->GetUid ());
          Ptr<mipv6MN> mipmn = mn->GetObject<mipv6MN>();
          bool b=oh.GetSourceAddress ().IsEqual(mipmn->GetCoA()) && mipmn->IsHomeMatch(oh.GetDestinationAddress ());
          pkt_tun_sent &=b;
        }
    }
  else
    {
      if (ih.GetSourceAddress ().IsEqual(clientaddress) && ih.GetDestinationAddress ().IsEqual(serveraddress))
        {
          list2.push_back (p->GetUid ());
          Ptr<mipv6MN> mipmn = mn->GetObject<mipv6MN>();
          bool b=oh.GetDestinationAddress ().IsEqual(mipmn->GetCoA()) && mipmn->IsHomeMatch(oh.GetSourceAddress ());
          pkt_tun_sent &=b;
        }

    }
}

void NoHandoffTestCase::TxPktAtClient (Ptr<const Packet> p, Ptr<Ipv6> ipv6, uint32_t interface)
{
Ipv6Header h;
p->PeekHeader(h);
if (h.GetSourceAddress ().IsEqual(clientaddress) && h.GetDestinationAddress ().IsEqual(serveraddress))
  {
    list3.push_back (p->GetUid ());
  }
std::cout<< "pio\n";
}

void NoHandoffTestCase::RxPktAtServer (Ptr<const Packet> p, Ptr<Ipv6> ipv6, uint32_t interface)
{
Ipv6Header h;
p->PeekHeader(h);
if (h.GetSourceAddress ().IsEqual(clientaddress) && h.GetDestinationAddress ().IsEqual(serveraddress))
  {
    list1.push_back (p->GetUid ());
std::cout<< "qio\n";
  }
}


void NoHandoffTestCase::TxBU (Ptr<Packet> packet, Ipv6Address src, Ipv6Address dst)
{
bulist.push_back (packet);
std::cout<< "wio\n";
}

void NoHandoffTestCase::RxBA (Ptr<Packet> packet, Ipv6Address src, Ipv6Address dst, Ptr<Ipv6Interface> interface)
{
pktcounterathandoff = pktcounter;

Ipv6MobilityBindingUpdateHeader bu;
Ipv6MobilityBindingAckHeader ba;
packet->RemoveHeader (ba);
std::list<Ptr<Packet> >::iterator it;

for (it=bulist.begin(); it!=bulist.end(); ++it)
  {
    (*it)->PeekHeader (bu);

    if(ba.GetSequence () == bu.GetSequence ())
      {
        break;
      }
  }
bu_seq &= (bu.GetSequence ()==ba.GetSequence ());
bool b=false;
if (it != bulist.end())
b=true;
bu_found &=b;

//Testing whether correct HoA-CoA mapping exist in HA Cache

Ipv6ExtensionType2RoutingHeader type2; //Fetching Type2 extension header which contains home address
packet->RemoveHeader (type2);
Ptr<mipv6HA> mipha=ha->GetObject<mipv6HA> ();
PointerValue ptr;
mipha->GetAttribute ("BCache", ptr);  //Fetching Cache of HA
Ptr<BCache> bcache = ptr.Get<BCache> ();
BCache::Entry *entry = bcache->Lookup(type2.GetHomeAddress ());
b=false;
if (entry)
b=true;
mn_found_ha &=b;


Ptr<mipv6MN> mipmn=mn->GetObject<mipv6MN> ();
Ipv6Address t1=mipmn->GetCoA ();
Ipv6Address t2=entry->GetCoa ();
b=false;
b=t1.IsEqual(t2);
coa_match_ha &=b;
std::cout<< "nio\n";
}






void NoHandoffTestCase::SetCallback ()
{
if (d == 0)
  {
    Ptr<Ipv6L3Protocol> ipl3 = cn->GetObject<Ipv6L3Protocol> ();
    ipl3->TraceConnectWithoutContext ("Rx", MakeCallback(&NoHandoffTestCase::RxPktAtServer, this));
    Ptr<Ipv6TunnelL4Protocol> iptun = ha->GetObject<Ipv6TunnelL4Protocol> ();
    iptun->TraceConnectWithoutContext ("RxHa", MakeCallback(&NoHandoffTestCase::RxPktAtTun, this));
    Ptr<Ipv6TunnelL4Protocol> iptun2 = mn->GetObject<Ipv6TunnelL4Protocol> ();
    iptun2->SetTxCallback (MakeCallback (&NoHandoffTestCase::TxPktAtTun, this)); 
  }
else
  {
    Ptr<Ipv6TunnelL4Protocol> iptun = mn->GetObject<Ipv6TunnelL4Protocol> ();
    iptun->TraceConnectWithoutContext ("RxMn", MakeCallback(&NoHandoffTestCase::RxPktAtTun, this));
    Ptr<Ipv6TunnelL4Protocol> iptun2 = ha->GetObject<Ipv6TunnelL4Protocol> ();
    iptun2->SetTxCallback (MakeCallback (&NoHandoffTestCase::TxPktAtTun, this));
    Ptr<Ipv6L3Protocol> ipl3 = cn->GetObject<Ipv6L3Protocol> ();
    ipl3->TraceConnectWithoutContext ("Tx", MakeCallback(&NoHandoffTestCase::TxPktAtClient, this));    
  }

Ptr<mipv6MN> mipmn = mn->GetObject<mipv6MN> ();
mipmn->TraceConnectWithoutContext ("TxBU", MakeCallback(&NoHandoffTestCase::TxBU, this));
mipmn->TraceConnectWithoutContext ("RxBA", MakeCallback(&NoHandoffTestCase::RxBA, this));
std::cout<< "bio\n";
}

void NoHandoffTestCase::MakeCallbackNull()
{
if (d == 0)
  {
    Ptr<Ipv6L3Protocol> ipl3 = cn->GetObject<Ipv6L3Protocol> ();
    ipl3->TraceConnectWithoutContext ("Rx", MakeNullCallback<Ptr<const Packet>, Ptr<Ipv6>, uint32_t> ());
    Ptr<Ipv6TunnelL4Protocol> iptun = ha->GetObject<Ipv6TunnelL4Protocol> ();
    iptun->TraceConnectWithoutContext ("RxHa", MakeNullCallback<Ptr<Packet>, Ipv6Header, Ipv6Header, Ptr<Ipv6Interface> > ());
    Ptr<Ipv6TunnelL4Protocol> iptun2 = mn->GetObject<Ipv6TunnelL4Protocol> ();
    iptun2->SetTxCallback (MakeNullCallback<void, Ptr<Packet>, Ipv6Header, Ipv6Header> ()); 
  }
else
  {
    Ptr<Ipv6TunnelL4Protocol> iptun = mn->GetObject<Ipv6TunnelL4Protocol> ();
    iptun->TraceConnectWithoutContext ("RxMn", MakeNullCallback<Ptr<Packet>, Ipv6Header, Ipv6Header, Ptr<Ipv6Interface> > ());
    Ptr<Ipv6TunnelL4Protocol> iptun2 = ha->GetObject<Ipv6TunnelL4Protocol> ();
    iptun2->SetTxCallback (MakeNullCallback<void, Ptr<Packet>, Ipv6Header, Ipv6Header> ());
    Ptr<Ipv6L3Protocol> ipl3 = cn->GetObject<Ipv6L3Protocol> ();
    ipl3->TraceConnectWithoutContext ("Tx", MakeNullCallback<Ptr<const Packet>, Ptr<Ipv6>, uint32_t>());    
  }
}


//ooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooooo
class NoHandoffTestCase2 : public TestCase
{
public:
  NoHandoffTestCase2 ();
  virtual ~NoHandoffTestCase2 ();
  void CheckForNoHandoff ();
  void Initialize (uint8_t dir, double interval, uint32_t maxpktcount, double start, double stop);
  void InstallMIPv6 (Ptr<Node> m, Ptr<Node> h, Ptr<Node> c);
  void InstallApplication (double interval, uint32_t maxpktcount, double starttime, double stoptime);
  void RxPktAtTun (Ptr<Packet> p, Ipv6Header ih, Ipv6Header oh, Ptr<Ipv6Interface> i);
  void TxPktAtTun (Ptr<Packet> p, Ipv6Header ih, Ipv6Header oh);
  void TxPktAtClient (Ptr<const Packet> p, Ptr<Ipv6> ipv6, uint32_t interface);
  void RxPktAtServer (Ptr<const Packet> p, Ptr<Ipv6> ipv6, uint32_t interface);
  void TxBU (Ptr<Packet> packet, Ipv6Address src, Ipv6Address dst);
  void RxBA (Ptr<Packet> packet, Ipv6Address src, Ipv6Address dst, Ptr<Ipv6Interface> interface);
  void SetCallback ();
  void MakeCallbackNull();

private:
  virtual void DoRun (void);
  Ipv6Address clientaddress, serveraddress;
  uint16_t appport;
  Ptr<Node> mn;
  Ptr<Node> ha;
  Ptr<Node> cn;
  int pktcounter;
  int pktcounterathandoff;
  uint8_t d; // False if MN to CN and true if CN to MN
  std::list<uint64_t> list1;  // contains received packet's UID at server
  std::list<uint64_t> list2;  // contains received packet's UID at HA
  std::list<uint64_t> list3;  // contains received packet's UID at client
  std::list<Ptr<Packet> > bulist;  // contains sent BUs of an MN
  bool pkt_tun_recv, pkt_tun_sent, bu_seq, bu_found, mn_found_ha, coa_match_ha;
};


// Add some help text to this case to describe what it is intended to test
NoHandoffTestCase2::NoHandoffTestCase2 ()
  : TestCase ("Test two mipv6 tests for no handoff")
{
appport=4000;
d=0;
pkt_tun_recv=true;
pkt_tun_sent=true;
bu_seq=true;
bu_found=true;
mn_found_ha=true;
coa_match_ha=true;
}

// This destructor does nothing but we include it as a reminder that
// the test case should clean up after itself
NoHandoffTestCase2::~NoHandoffTestCase2 ()
{
}



//
// This method is the pure virtual method from class TestCase that every
// TestCase must implement
//
void
NoHandoffTestCase2::DoRun (void)
{
NodeContainer n;
n.Create (4);

InternetStackHelper internet;
internet.Install (n);





  // link three nodes
  PointToPointHelper pointToPoint;
  pointToPoint.SetDeviceAttribute ("DataRate", StringValue ("5Mbps"));
  pointToPoint.SetChannelAttribute ("Delay", StringValue ("2ms"));


  NetDeviceContainer d1, d2, d3, d4, d5;

  d1 = pointToPoint.Install (n.Get (0), n.Get (1));
  d2 = pointToPoint.Install (n.Get (1), n.Get (2));
  d3 = pointToPoint.Install (n.Get (2), n.Get (3));
  d4 = NetDeviceContainer (d3.Get(0));
  d5 = NetDeviceContainer (d3.Get(1));


  Ipv6AddressHelper ipv61,ipv62,ipv63;
  Ipv6InterfaceContainer iifc,iifc1,iifc2,iifc3,iifc4;


  ipv61.SetBase (Ipv6Address ("1111::"), Ipv6Prefix (64));
  iifc = ipv61.Assign (d1);
  iifc1.Add(iifc);
  iifc1.SetForwarding (0, true);
  iifc1.SetDefaultRouteInAllNodes (0);
  iifc1.SetForwarding (1, true);
  iifc1.SetDefaultRouteInAllNodes (1);

  ipv62.SetBase (Ipv6Address ("2222::"), Ipv6Prefix (64));
  iifc = ipv62.Assign (d2);
  iifc2.Add(iifc);
  iifc2.SetForwarding (0, true);
  iifc2.SetDefaultRouteInAllNodes (0);
  iifc2.SetForwarding (1, true);
  iifc2.SetDefaultRouteInAllNodes (1);

  ipv63.SetBase (Ipv6Address ("3333::"), Ipv6Prefix (64));
  iifc = ipv63.Assign (d4);
  iifc3.Add(iifc);
  iifc3.SetForwarding (0, true);
  iifc3.SetDefaultRouteInAllNodes (0);

  iifc = ipv63.AssignWithoutAddress (d5);
  iifc4.Add(iifc);


  Ipv6Address prefix ("3333::");  //create the prefix 

  uint32_t indexRouter = iifc3.GetInterfaceIndex (0);  //AR interface (n-AR1) 

  Ptr<Radvd> radvd=CreateObject<Radvd> ();
  Ptr<RadvdInterface> routerInterface= Create<RadvdInterface> (indexRouter, 1500, 50);
  Ptr<RadvdPrefix> routerPrefix = Create<RadvdPrefix> (prefix, 64, 1.5, 2.0);

  routerInterface->AddPrefix(routerPrefix);

  radvd->AddConfiguration(routerInterface);

  n.Get(2)->AddApplication(radvd);
  radvd->SetStartTime(Seconds (1.0));
  radvd->SetStopTime(Seconds (200.0));



  Ipv6StaticRoutingHelper routingHelper;
  Ptr<Ipv6> ipv692 = n.Get(1)->GetObject<Ipv6> ();
  Ptr<Ipv6StaticRouting> rttop = routingHelper.GetStaticRouting(ipv692);
  rttop->AddNetworkRouteTo(Ipv6Address("3333::"),Ipv6Prefix(64),Ipv6Address("2222::200:ff:fe00:4"),2,0);

  Ptr<Ipv6> ipv693 = n.Get(2)->GetObject<Ipv6> ();
  rttop = routingHelper.GetStaticRouting(ipv693);
  rttop->AddNetworkRouteTo(Ipv6Address("1111::"),Ipv6Prefix(64),Ipv6Address("2222::200:ff:fe00:3"),1,0);


  InstallMIPv6 (n.Get(3), n.Get(1), n.Get(0));





  Initialize (1, 0.02, 500, 5, 16);

  Simulator::Schedule (Time (Seconds (17.0)), &NoHandoffTestCase2::CheckForNoHandoff, this);




  Simulator::Stop (Seconds (18.0));
  Simulator::Run ();
  Simulator::Destroy ();

}

void NoHandoffTestCase2::Initialize (uint8_t dir, double interval, uint32_t maxpktcount,  double start, double stop)
{
d=dir;
Ptr<mipv6MN> mipmn = mn->GetObject<mipv6MN> ();


if (d==0)
{
  clientaddress=mipmn->GetHomeAddress ();
  serveraddress=Ipv6Address ("1111::200:ff:fe00:1");
}
else
{
  clientaddress=Ipv6Address ("1111::200:ff:fe00:1");
  serveraddress=mipmn->GetHomeAddress ();
}
pktcounter=0;
pktcounterathandoff=0;
list1.clear ();
list2.clear ();
list3.clear ();
bulist.clear ();
pkt_tun_recv=true; pkt_tun_sent=true; bu_seq=true; bu_found=true; mn_found_ha=true; coa_match_ha=true;
SetCallback ();
InstallApplication (interval, maxpktcount, start, stop);
}


void NoHandoffTestCase2::CheckForNoHandoff ()
{
bool f = true;
std::list<uint64_t>::iterator it, it1, it2;
for (it=list1.begin(); it!=list1.end(); ++it)
  {
    it1 = std::find (list2.begin (), list2.end (), *it);
    it2 = std::find (list3.begin (), list3.end (), *it);
    if(it1 != list2.end () && it2 != list3.end())
      {
        //std::cout<< "\nInside Check:" << *it1 << *it2 << *it << "\n";
        list2.erase (it1);
        list3.erase (it2);
      }
    else
      {
        f = false;
        break;
      } 
  }
list1.clear();
if(list1.size () != 0 || list2.size() != 0)
  f=false;

NS_TEST_ASSERT_MSG_EQ (f, true, "Failure Happens in MIPv6 test...");
NS_TEST_ASSERT_MSG_GT (pktcounter, pktcounterathandoff, "Handoff is not successful");
NS_TEST_ASSERT_MSG_EQ (pkt_tun_recv, true, "Failure Happens in Tunneled packet...");
NS_TEST_ASSERT_MSG_EQ (pkt_tun_sent, true, "Failure Happens in Tunneled packet...");
NS_TEST_ASSERT_MSG_EQ (bu_seq, true, "Failure Happens in Tunneled packet...");
NS_TEST_ASSERT_MSG_EQ (bu_found, true, "Failure Happens in Tunneled packet...");
NS_TEST_ASSERT_MSG_EQ (mn_found_ha, true, "Failure Happens in Tunneled packet...");
NS_TEST_ASSERT_MSG_EQ (coa_match_ha, true, "Failure Happens in Tunneled packet...");
}


void NoHandoffTestCase2::InstallMIPv6 (Ptr<Node> m, Ptr<Node> h, Ptr<Node> c)
{
  mipv6HAHelper hh;
  hh.Install (h);
  mipv6MNHelper mh (hh.GetHomeAgentAddressList(),false); 
  mh.Install (m);

  mn=m;
  ha=h;
  cn=c;
std::cout<< "uio\n";
}


void NoHandoffTestCase2::InstallApplication (double interval, uint32_t maxpktcount, double starttime, double stoptime)
{
  uint16_t port = appport;
  UdpServerHelper server (port);
  ApplicationContainer apps;
  if (d)
    {
      apps = server.Install (mn);
    }
  else
    {
      apps = server.Install (cn);      
    }
  apps.Start (Seconds (starttime));
  apps.Stop (Seconds (stoptime));



  uint32_t MaxPacketSize = 1024;
  Time interPacketInterval = Seconds (interval);
  uint32_t maxPacketCount = maxpktcount;
  UdpClientHelper client (serveraddress, port);

  client.SetAttribute ("MaxPackets", UintegerValue (maxPacketCount));
  client.SetAttribute ("Interval", TimeValue (interPacketInterval));
  client.SetAttribute ("PacketSize", UintegerValue (MaxPacketSize));
  if (d)
    {
      apps = client.Install (cn);
    }
  else
    {
      apps = client.Install (mn);
    }
  apps.Start (Seconds (starttime));
  apps.Stop (Seconds (stoptime-1.0));
 
appport++;
std::cout<< "jio\n";
}


void NoHandoffTestCase2::RxPktAtTun (Ptr<Packet> p, Ipv6Header ih, Ipv6Header oh, Ptr<Ipv6Interface> i)
{
  pktcounter++;
  if(!d)
    {
      if (ih.GetSourceAddress ().IsEqual(clientaddress) && ih.GetDestinationAddress ().IsEqual(serveraddress))
        {
          list2.push_back (p->GetUid ());
          Ptr<mipv6MN> mipmn = mn->GetObject<mipv6MN>();
          bool b=(oh.GetSourceAddress ().IsEqual(mipmn->GetCoA()) && mipmn->IsHomeMatch(oh.GetDestinationAddress ()));
          pkt_tun_recv &=b;
std::cout<< oh.GetSourceAddress ()<< oh.GetDestinationAddress () << "mio\n";
          
        }
    }
  else
    {
      if (ih.GetSourceAddress ().IsEqual(clientaddress) && ih.GetDestinationAddress ().IsEqual(serveraddress))
        {
          list1.push_back (p->GetUid ());
          Ptr<mipv6MN> mipmn = mn->GetObject<mipv6MN>();
          bool b=(oh.GetDestinationAddress ().IsEqual(mipmn->GetCoA()) && mipmn->IsHomeMatch(oh.GetSourceAddress ()));
          pkt_tun_recv &=b;
        }

    }
}

void NoHandoffTestCase2::TxPktAtTun (Ptr<Packet> p, Ipv6Header ih, Ipv6Header oh)
{
std::cout<< "lio\n";
  if(!d)
    {
      if (ih.GetSourceAddress ().IsEqual(clientaddress) && ih.GetDestinationAddress ().IsEqual(serveraddress))
        {
          list3.push_back (p->GetUid ());
          Ptr<mipv6MN> mipmn = mn->GetObject<mipv6MN>();
          bool b=oh.GetSourceAddress ().IsEqual(mipmn->GetCoA()) && mipmn->IsHomeMatch(oh.GetDestinationAddress ());
          pkt_tun_sent &=b;
        }
    }
  else
    {
      if (ih.GetSourceAddress ().IsEqual(clientaddress) && ih.GetDestinationAddress ().IsEqual(serveraddress))
        {
          list2.push_back (p->GetUid ());
          Ptr<mipv6MN> mipmn = mn->GetObject<mipv6MN>();
          bool b=oh.GetDestinationAddress ().IsEqual(mipmn->GetCoA()) && mipmn->IsHomeMatch(oh.GetSourceAddress ());
          pkt_tun_sent &=b;
        }

    }
}

void NoHandoffTestCase2::TxPktAtClient (Ptr<const Packet> p, Ptr<Ipv6> ipv6, uint32_t interface)
{
Ipv6Header h;
p->PeekHeader(h);
if (h.GetSourceAddress ().IsEqual(clientaddress) && h.GetDestinationAddress ().IsEqual(serveraddress))
  {
    list3.push_back (p->GetUid ());
  }
std::cout<< "pio\n";
}

void NoHandoffTestCase2::RxPktAtServer (Ptr<const Packet> p, Ptr<Ipv6> ipv6, uint32_t interface)
{
Ipv6Header h;
p->PeekHeader(h);
if (h.GetSourceAddress ().IsEqual(clientaddress) && h.GetDestinationAddress ().IsEqual(serveraddress))
  {
    list1.push_back (p->GetUid ());
std::cout<< "qio\n";
  }
}


void NoHandoffTestCase2::TxBU (Ptr<Packet> packet, Ipv6Address src, Ipv6Address dst)
{
bulist.push_back (packet);
std::cout<< "wio\n";
}

void NoHandoffTestCase2::RxBA (Ptr<Packet> packet, Ipv6Address src, Ipv6Address dst, Ptr<Ipv6Interface> interface)
{
pktcounterathandoff = pktcounter;

Ipv6MobilityBindingUpdateHeader bu;
Ipv6MobilityBindingAckHeader ba;
packet->RemoveHeader (ba);
std::list<Ptr<Packet> >::iterator it;

for (it=bulist.begin(); it!=bulist.end(); ++it)
  {
    (*it)->PeekHeader (bu);

    if(ba.GetSequence () == bu.GetSequence ())
      {
        break;
      }
  }
bu_seq &= (bu.GetSequence ()==ba.GetSequence ());
bool b=false;
if (it != bulist.end())
b=true;
bu_found &=b;

//Testing whether correct HoA-CoA mapping exist in HA Cache

Ipv6ExtensionType2RoutingHeader type2; //Fetching Type2 extension header which contains home address
packet->RemoveHeader (type2);
Ptr<mipv6HA> mipha=ha->GetObject<mipv6HA> ();
PointerValue ptr;
mipha->GetAttribute ("BCache", ptr);  //Fetching Cache of HA
Ptr<BCache> bcache = ptr.Get<BCache> ();
BCache::Entry *entry = bcache->Lookup(type2.GetHomeAddress ());
b=false;
if (entry)
b=true;
mn_found_ha &=b;


Ptr<mipv6MN> mipmn=mn->GetObject<mipv6MN> ();
Ipv6Address t1=mipmn->GetCoA ();
Ipv6Address t2=entry->GetCoa ();
b=false;
b=t1.IsEqual(t2);
coa_match_ha &=b;
std::cout<< "nio\n";
}






void NoHandoffTestCase2::SetCallback ()
{
if (d == 0)
  {
    Ptr<Ipv6L3Protocol> ipl3 = cn->GetObject<Ipv6L3Protocol> ();
    ipl3->TraceConnectWithoutContext ("Rx", MakeCallback(&NoHandoffTestCase2::RxPktAtServer, this));
    Ptr<Ipv6TunnelL4Protocol> iptun = ha->GetObject<Ipv6TunnelL4Protocol> ();
    iptun->TraceConnectWithoutContext ("RxHa", MakeCallback(&NoHandoffTestCase2::RxPktAtTun, this));
    Ptr<Ipv6TunnelL4Protocol> iptun2 = mn->GetObject<Ipv6TunnelL4Protocol> ();
    iptun2->SetTxCallback (MakeCallback (&NoHandoffTestCase2::TxPktAtTun, this)); 
  }
else
  {
    Ptr<Ipv6TunnelL4Protocol> iptun = mn->GetObject<Ipv6TunnelL4Protocol> ();
    iptun->TraceConnectWithoutContext ("RxMn", MakeCallback(&NoHandoffTestCase2::RxPktAtTun, this));
    Ptr<Ipv6TunnelL4Protocol> iptun2 = ha->GetObject<Ipv6TunnelL4Protocol> ();
    iptun2->SetTxCallback (MakeCallback (&NoHandoffTestCase2::TxPktAtTun, this));
    Ptr<Ipv6L3Protocol> ipl3 = cn->GetObject<Ipv6L3Protocol> ();
    ipl3->TraceConnectWithoutContext ("Tx", MakeCallback(&NoHandoffTestCase2::TxPktAtClient, this));    
  }

Ptr<mipv6MN> mipmn = mn->GetObject<mipv6MN> ();
mipmn->TraceConnectWithoutContext ("TxBU", MakeCallback(&NoHandoffTestCase2::TxBU, this));
mipmn->TraceConnectWithoutContext ("RxBA", MakeCallback(&NoHandoffTestCase2::RxBA, this));
std::cout<< "bio\n";
}

void NoHandoffTestCase2::MakeCallbackNull()
{
if (d == 0)
  {
    Ptr<Ipv6L3Protocol> ipl3 = cn->GetObject<Ipv6L3Protocol> ();
    ipl3->TraceConnectWithoutContext ("Rx", MakeNullCallback<Ptr<const Packet>, Ptr<Ipv6>, uint32_t> ());
    Ptr<Ipv6TunnelL4Protocol> iptun = ha->GetObject<Ipv6TunnelL4Protocol> ();
    iptun->TraceConnectWithoutContext ("RxHa", MakeNullCallback<Ptr<Packet>, Ipv6Header, Ipv6Header, Ptr<Ipv6Interface> > ());
    Ptr<Ipv6TunnelL4Protocol> iptun2 = mn->GetObject<Ipv6TunnelL4Protocol> ();
    iptun2->SetTxCallback (MakeNullCallback<void, Ptr<Packet>, Ipv6Header, Ipv6Header> ()); 
  }
else
  {
    Ptr<Ipv6TunnelL4Protocol> iptun = mn->GetObject<Ipv6TunnelL4Protocol> ();
    iptun->TraceConnectWithoutContext ("RxMn", MakeNullCallback<Ptr<Packet>, Ipv6Header, Ipv6Header, Ptr<Ipv6Interface> > ());
    Ptr<Ipv6TunnelL4Protocol> iptun2 = ha->GetObject<Ipv6TunnelL4Protocol> ();
    iptun2->SetTxCallback (MakeNullCallback<void, Ptr<Packet>, Ipv6Header, Ipv6Header> ());
    Ptr<Ipv6L3Protocol> ipl3 = cn->GetObject<Ipv6L3Protocol> ();
    ipl3->TraceConnectWithoutContext ("Tx", MakeNullCallback<Ptr<const Packet>, Ptr<Ipv6>, uint32_t>());    
  }
}

//sssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssssss

class NoHandoffTestCase3 : public TestCase
{
public:
  NoHandoffTestCase3 ();
  virtual ~NoHandoffTestCase3 ();
  void CheckForNoHandoff ();
  void Initialize (uint8_t dir, double interval, uint32_t maxpktcount, double start, double stop);
  void InstallMIPv6 (Ptr<Node> m, Ptr<Node> h, Ptr<Node> c);
  void InstallApplication (double interval, uint32_t maxpktcount, double starttime, double stoptime);
  void RxPktAtTun (Ptr<Packet> p, Ipv6Header ih, Ipv6Header oh, Ptr<Ipv6Interface> i);
  void TxPktAtTun (Ptr<Packet> p, Ipv6Header ih, Ipv6Header oh);
  void TxPktAtClient (Ptr<const Packet> p, Ptr<Ipv6> ipv6, uint32_t interface);
  void RxPktAtServer (Ptr<const Packet> p, Ptr<Ipv6> ipv6, uint32_t interface);
  void TxBU (Ptr<Packet> packet, Ipv6Address src, Ipv6Address dst);
  void RxBA (Ptr<Packet> packet, Ipv6Address src, Ipv6Address dst, Ptr<Ipv6Interface> interface);
  void SetCallback ();
  void MakeCallbackNull();

private:
  virtual void DoRun (void);
  Ipv6Address clientaddress, serveraddress;
  uint16_t appport;
  Ptr<Node> mn;
  Ptr<Node> ha;
  Ptr<Node> cn;
  int pktcounter;
  int pktcounterathandoff;
  uint8_t d; // False if MN to CN and true if CN to MN
  std::list<uint64_t> list1;  // contains received packet's UID at server
  std::list<uint64_t> list2;  // contains received packet's UID at HA
  std::list<uint64_t> list3;  // contains received packet's UID at client
  std::list<Ptr<Packet> > bulist;  // contains sent BUs of an MN
  bool pkt_tun_recv, pkt_tun_sent, bu_seq, bu_found, mn_found_ha, coa_match_ha;
};


// Add some help text to this case to describe what it is intended to test
NoHandoffTestCase3::NoHandoffTestCase3 ()
  : TestCase ("Test two mipv6 tests for no handoff")
{
appport=4000;
d=0;
pkt_tun_recv=true;
pkt_tun_sent=true;
bu_seq=true;
bu_found=true;
mn_found_ha=true;
coa_match_ha=true;
}

// This destructor does nothing but we include it as a reminder that
// the test case should clean up after itself
NoHandoffTestCase3::~NoHandoffTestCase3 ()
{
}



//
// This method is the pure virtual method from class TestCase that every
// TestCase must implement
//
void
NoHandoffTestCase3::DoRun (void)
{
NodeContainer n;
n.Create (4);

InternetStackHelper internet;
internet.Install (n);





  // link three nodes
  PointToPointHelper pointToPoint;
  pointToPoint.SetDeviceAttribute ("DataRate", StringValue ("5Mbps"));
  pointToPoint.SetChannelAttribute ("Delay", StringValue ("2ms"));


  NetDeviceContainer d1, d2, d3, d4, d5;

  d1 = pointToPoint.Install (n.Get (0), n.Get (1));
  d2 = pointToPoint.Install (n.Get (1), n.Get (2));
  d3 = pointToPoint.Install (n.Get (2), n.Get (3));
  d4 = NetDeviceContainer (d3.Get(0));
  d5 = NetDeviceContainer (d3.Get(1));


  Ipv6AddressHelper ipv61,ipv62,ipv63;
  Ipv6InterfaceContainer iifc,iifc1,iifc2,iifc3,iifc4;


  ipv61.SetBase (Ipv6Address ("1111::"), Ipv6Prefix (64));
  iifc = ipv61.Assign (d1);
  iifc1.Add(iifc);
  iifc1.SetForwarding (0, true);
  iifc1.SetDefaultRouteInAllNodes (0);
  iifc1.SetForwarding (1, true);
  iifc1.SetDefaultRouteInAllNodes (1);

  ipv62.SetBase (Ipv6Address ("2222::"), Ipv6Prefix (64));
  iifc = ipv62.Assign (d2);
  iifc2.Add(iifc);
  iifc2.SetForwarding (0, true);
  iifc2.SetDefaultRouteInAllNodes (0);
  iifc2.SetForwarding (1, true);
  iifc2.SetDefaultRouteInAllNodes (1);

  ipv63.SetBase (Ipv6Address ("3333::"), Ipv6Prefix (64));
  iifc = ipv63.Assign (d4);
  iifc3.Add(iifc);
  iifc3.SetForwarding (0, true);
  iifc3.SetDefaultRouteInAllNodes (0);

  iifc = ipv63.AssignWithoutAddress (d5);
  iifc4.Add(iifc);


  Ipv6Address prefix ("3333::");  //create the prefix 

  uint32_t indexRouter = iifc3.GetInterfaceIndex (0);  //AR interface (n-AR1) 

  Ptr<Radvd> radvd=CreateObject<Radvd> ();
  Ptr<RadvdInterface> routerInterface= Create<RadvdInterface> (indexRouter, 1500, 50);
  Ptr<RadvdPrefix> routerPrefix = Create<RadvdPrefix> (prefix, 64, 1.5, 2.0);

  routerInterface->AddPrefix(routerPrefix);

  radvd->AddConfiguration(routerInterface);

  n.Get(2)->AddApplication(radvd);
  radvd->SetStartTime(Seconds (1.0));
  radvd->SetStopTime(Seconds (200.0));



  Ipv6StaticRoutingHelper routingHelper;
  Ptr<Ipv6> ipv692 = n.Get(1)->GetObject<Ipv6> ();
  Ptr<Ipv6StaticRouting> rttop = routingHelper.GetStaticRouting(ipv692);
  rttop->AddNetworkRouteTo(Ipv6Address("3333::"),Ipv6Prefix(64),Ipv6Address("2222::200:ff:fe00:4"),2,0);

  Ptr<Ipv6> ipv693 = n.Get(2)->GetObject<Ipv6> ();
  rttop = routingHelper.GetStaticRouting(ipv693);
  rttop->AddNetworkRouteTo(Ipv6Address("1111::"),Ipv6Prefix(64),Ipv6Address("2222::200:ff:fe00:3"),1,0);


  InstallMIPv6 (n.Get(3), n.Get(1), n.Get(0));





  Initialize (1, 0.002, 5000, 4, 15);

  Simulator::Schedule (Time (Seconds (16.0)), &NoHandoffTestCase3::CheckForNoHandoff, this);




  Simulator::Stop (Seconds (18.0));
  Simulator::Run ();
  Simulator::Destroy ();

}

void NoHandoffTestCase3::Initialize (uint8_t dir, double interval, uint32_t maxpktcount,  double start, double stop)
{
d=dir;
Ptr<mipv6MN> mipmn = mn->GetObject<mipv6MN> ();


if (d==0)
{
  clientaddress=mipmn->GetHomeAddress ();
  serveraddress=Ipv6Address ("1111::200:ff:fe00:1");
}
else
{
  clientaddress=Ipv6Address ("1111::200:ff:fe00:1");
  serveraddress=mipmn->GetHomeAddress ();
}
pktcounter=0;
pktcounterathandoff=0;
list1.clear ();
list2.clear ();
list3.clear ();
bulist.clear ();
pkt_tun_recv=true; pkt_tun_sent=true; bu_seq=true; bu_found=true; mn_found_ha=true; coa_match_ha=true;
SetCallback ();
InstallApplication (interval, maxpktcount, start, stop);
}


void NoHandoffTestCase3::CheckForNoHandoff ()
{
bool f = true;
std::list<uint64_t>::iterator it, it1, it2;
for (it=list1.begin(); it!=list1.end(); ++it)
  {
    it1 = std::find (list2.begin (), list2.end (), *it);
    it2 = std::find (list3.begin (), list3.end (), *it);
    if(it1 != list2.end () && it2 != list3.end())
      {
        //std::cout<< "\nInside Check:" << *it1 << *it2 << *it << "\n";
        list2.erase (it1);
        list3.erase (it2);
      }
    else
      {
        f = false;
        break;
      } 
  }
list1.clear();
if(list1.size () != 0 || list2.size() != 0)
  f=false;

NS_TEST_ASSERT_MSG_EQ (f, true, "Failure Happens in MIPv6 test...");
NS_TEST_ASSERT_MSG_GT (pktcounter, pktcounterathandoff, "Handoff is not successful");
NS_TEST_ASSERT_MSG_EQ (pkt_tun_recv, true, "Failure Happens in Tunneled packet...");
NS_TEST_ASSERT_MSG_EQ (pkt_tun_sent, true, "Failure Happens in Tunneled packet...");
NS_TEST_ASSERT_MSG_EQ (bu_seq, true, "Failure Happens in Tunneled packet...");
NS_TEST_ASSERT_MSG_EQ (bu_found, true, "Failure Happens in Tunneled packet...");
NS_TEST_ASSERT_MSG_EQ (mn_found_ha, true, "Failure Happens in Tunneled packet...");
NS_TEST_ASSERT_MSG_EQ (coa_match_ha, true, "Failure Happens in Tunneled packet...");
}


void NoHandoffTestCase3::InstallMIPv6 (Ptr<Node> m, Ptr<Node> h, Ptr<Node> c)
{
  mipv6HAHelper hh;
  hh.Install (h);
  mipv6MNHelper mh (hh.GetHomeAgentAddressList(),false); 
  mh.Install (m);

  mn=m;
  ha=h;
  cn=c;
std::cout<< "uio\n";
}


void NoHandoffTestCase3::InstallApplication (double interval, uint32_t maxpktcount, double starttime, double stoptime)
{
  uint16_t port = appport;
  UdpServerHelper server (port);
  ApplicationContainer apps;
  if (d)
    {
      apps = server.Install (mn);
    }
  else
    {
      apps = server.Install (cn);      
    }
  apps.Start (Seconds (starttime));
  apps.Stop (Seconds (stoptime));



  uint32_t MaxPacketSize = 1024;
  Time interPacketInterval = Seconds (interval);
  uint32_t maxPacketCount = maxpktcount;
  UdpClientHelper client (serveraddress, port);

  client.SetAttribute ("MaxPackets", UintegerValue (maxPacketCount));
  client.SetAttribute ("Interval", TimeValue (interPacketInterval));
  client.SetAttribute ("PacketSize", UintegerValue (MaxPacketSize));
  if (d)
    {
      apps = client.Install (cn);
    }
  else
    {
      apps = client.Install (mn);
    }
  apps.Start (Seconds (starttime));
  apps.Stop (Seconds (stoptime-1.0));
 
appport++;
std::cout<< "jio\n";
}


void NoHandoffTestCase3::RxPktAtTun (Ptr<Packet> p, Ipv6Header ih, Ipv6Header oh, Ptr<Ipv6Interface> i)
{
  pktcounter++;
  if(!d)
    {
      if (ih.GetSourceAddress ().IsEqual(clientaddress) && ih.GetDestinationAddress ().IsEqual(serveraddress))
        {
          list2.push_back (p->GetUid ());
          Ptr<mipv6MN> mipmn = mn->GetObject<mipv6MN>();
          bool b=(oh.GetSourceAddress ().IsEqual(mipmn->GetCoA()) && mipmn->IsHomeMatch(oh.GetDestinationAddress ()));
          pkt_tun_recv &=b;
std::cout<< oh.GetSourceAddress ()<< oh.GetDestinationAddress () << "mio\n";
          
        }
    }
  else
    {
      if (ih.GetSourceAddress ().IsEqual(clientaddress) && ih.GetDestinationAddress ().IsEqual(serveraddress))
        {
          list1.push_back (p->GetUid ());
          Ptr<mipv6MN> mipmn = mn->GetObject<mipv6MN>();
          bool b=(oh.GetDestinationAddress ().IsEqual(mipmn->GetCoA()) && mipmn->IsHomeMatch(oh.GetSourceAddress ()));
          pkt_tun_recv &=b;
        }

    }
}

void NoHandoffTestCase3::TxPktAtTun (Ptr<Packet> p, Ipv6Header ih, Ipv6Header oh)
{
std::cout<< "lio\n";
  if(!d)
    {
      if (ih.GetSourceAddress ().IsEqual(clientaddress) && ih.GetDestinationAddress ().IsEqual(serveraddress))
        {
          list3.push_back (p->GetUid ());
          Ptr<mipv6MN> mipmn = mn->GetObject<mipv6MN>();
          bool b=oh.GetSourceAddress ().IsEqual(mipmn->GetCoA()) && mipmn->IsHomeMatch(oh.GetDestinationAddress ());
          pkt_tun_sent &=b;
        }
    }
  else
    {
      if (ih.GetSourceAddress ().IsEqual(clientaddress) && ih.GetDestinationAddress ().IsEqual(serveraddress))
        {
          list2.push_back (p->GetUid ());
          Ptr<mipv6MN> mipmn = mn->GetObject<mipv6MN>();
          bool b=oh.GetDestinationAddress ().IsEqual(mipmn->GetCoA()) && mipmn->IsHomeMatch(oh.GetSourceAddress ());
          pkt_tun_sent &=b;
        }

    }
}

void NoHandoffTestCase3::TxPktAtClient (Ptr<const Packet> p, Ptr<Ipv6> ipv6, uint32_t interface)
{
Ipv6Header h;
p->PeekHeader(h);
if (h.GetSourceAddress ().IsEqual(clientaddress) && h.GetDestinationAddress ().IsEqual(serveraddress))
  {
    list3.push_back (p->GetUid ());
  }
std::cout<< "pio\n";
}

void NoHandoffTestCase3::RxPktAtServer (Ptr<const Packet> p, Ptr<Ipv6> ipv6, uint32_t interface)
{
Ipv6Header h;
p->PeekHeader(h);
if (h.GetSourceAddress ().IsEqual(clientaddress) && h.GetDestinationAddress ().IsEqual(serveraddress))
  {
    list1.push_back (p->GetUid ());
std::cout<< "qio\n";
  }
}


void NoHandoffTestCase3::TxBU (Ptr<Packet> packet, Ipv6Address src, Ipv6Address dst)
{
bulist.push_back (packet);
std::cout<< "wio\n";
}

void NoHandoffTestCase3::RxBA (Ptr<Packet> packet, Ipv6Address src, Ipv6Address dst, Ptr<Ipv6Interface> interface)
{
pktcounterathandoff = pktcounter;

Ipv6MobilityBindingUpdateHeader bu;
Ipv6MobilityBindingAckHeader ba;
packet->RemoveHeader (ba);
std::list<Ptr<Packet> >::iterator it;

for (it=bulist.begin(); it!=bulist.end(); ++it)
  {
    (*it)->PeekHeader (bu);

    if(ba.GetSequence () == bu.GetSequence ())
      {
        break;
      }
  }
bu_seq &= (bu.GetSequence ()==ba.GetSequence ());
bool b=false;
if (it != bulist.end())
b=true;
bu_found &=b;

//Testing whether correct HoA-CoA mapping exist in HA Cache

Ipv6ExtensionType2RoutingHeader type2; //Fetching Type2 extension header which contains home address
packet->RemoveHeader (type2);
Ptr<mipv6HA> mipha=ha->GetObject<mipv6HA> ();
PointerValue ptr;
mipha->GetAttribute ("BCache", ptr);  //Fetching Cache of HA
Ptr<BCache> bcache = ptr.Get<BCache> ();
BCache::Entry *entry = bcache->Lookup(type2.GetHomeAddress ());
b=false;
if (entry)
b=true;
mn_found_ha &=b;


Ptr<mipv6MN> mipmn=mn->GetObject<mipv6MN> ();
Ipv6Address t1=mipmn->GetCoA ();
Ipv6Address t2=entry->GetCoa ();
b=false;
b=t1.IsEqual(t2);
coa_match_ha &=b;
std::cout<< "nio\n";
}






void NoHandoffTestCase3::SetCallback ()
{
if (d == 0)
  {
    Ptr<Ipv6L3Protocol> ipl3 = cn->GetObject<Ipv6L3Protocol> ();
    ipl3->TraceConnectWithoutContext ("Rx", MakeCallback(&NoHandoffTestCase3::RxPktAtServer, this));
    Ptr<Ipv6TunnelL4Protocol> iptun = ha->GetObject<Ipv6TunnelL4Protocol> ();
    iptun->TraceConnectWithoutContext ("RxHa", MakeCallback(&NoHandoffTestCase3::RxPktAtTun, this));
    Ptr<Ipv6TunnelL4Protocol> iptun2 = mn->GetObject<Ipv6TunnelL4Protocol> ();
    iptun2->SetTxCallback (MakeCallback (&NoHandoffTestCase3::TxPktAtTun, this)); 
  }
else
  {
    Ptr<Ipv6TunnelL4Protocol> iptun = mn->GetObject<Ipv6TunnelL4Protocol> ();
    iptun->TraceConnectWithoutContext ("RxMn", MakeCallback(&NoHandoffTestCase3::RxPktAtTun, this));
    Ptr<Ipv6TunnelL4Protocol> iptun2 = ha->GetObject<Ipv6TunnelL4Protocol> ();
    iptun2->SetTxCallback (MakeCallback (&NoHandoffTestCase3::TxPktAtTun, this));
    Ptr<Ipv6L3Protocol> ipl3 = cn->GetObject<Ipv6L3Protocol> ();
    ipl3->TraceConnectWithoutContext ("Tx", MakeCallback(&NoHandoffTestCase3::TxPktAtClient, this));    
  }

Ptr<mipv6MN> mipmn = mn->GetObject<mipv6MN> ();
mipmn->TraceConnectWithoutContext ("TxBU", MakeCallback(&NoHandoffTestCase3::TxBU, this));
mipmn->TraceConnectWithoutContext ("RxBA", MakeCallback(&NoHandoffTestCase3::RxBA, this));
std::cout<< "bio\n";
}

void NoHandoffTestCase3::MakeCallbackNull()
{
if (d == 0)
  {
    Ptr<Ipv6L3Protocol> ipl3 = cn->GetObject<Ipv6L3Protocol> ();
    ipl3->TraceConnectWithoutContext ("Rx", MakeNullCallback<Ptr<const Packet>, Ptr<Ipv6>, uint32_t> ());
    Ptr<Ipv6TunnelL4Protocol> iptun = ha->GetObject<Ipv6TunnelL4Protocol> ();
    iptun->TraceConnectWithoutContext ("RxHa", MakeNullCallback<Ptr<Packet>, Ipv6Header, Ipv6Header, Ptr<Ipv6Interface> > ());
    Ptr<Ipv6TunnelL4Protocol> iptun2 = mn->GetObject<Ipv6TunnelL4Protocol> ();
    iptun2->SetTxCallback (MakeNullCallback<void, Ptr<Packet>, Ipv6Header, Ipv6Header> ()); 
  }
else
  {
    Ptr<Ipv6TunnelL4Protocol> iptun = mn->GetObject<Ipv6TunnelL4Protocol> ();
    iptun->TraceConnectWithoutContext ("RxMn", MakeNullCallback<Ptr<Packet>, Ipv6Header, Ipv6Header, Ptr<Ipv6Interface> > ());
    Ptr<Ipv6TunnelL4Protocol> iptun2 = ha->GetObject<Ipv6TunnelL4Protocol> ();
    iptun2->SetTxCallback (MakeNullCallback<void, Ptr<Packet>, Ipv6Header, Ipv6Header> ());
    Ptr<Ipv6L3Protocol> ipl3 = cn->GetObject<Ipv6L3Protocol> ();
    ipl3->TraceConnectWithoutContext ("Tx", MakeNullCallback<Ptr<const Packet>, Ptr<Ipv6>, uint32_t>());    
  }
}
//ttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttttt

class Mipv6TestSuite : public TestSuite
{
public:
  Mipv6TestSuite ();
};

Mipv6TestSuite::Mipv6TestSuite ()
  : TestSuite ("mipv6-test", UNIT)
{
  // TestDuration for TestCase can be QUICK, EXTENSIVE or TAKES_FOREVER
  AddTestCase (new NoHandoffTestCase, TestCase::QUICK);
  AddTestCase (new NoHandoffTestCase2, TestCase::QUICK);
  AddTestCase (new NoHandoffTestCase3, TestCase::QUICK);
}

// Do not forget to allocate an instance of this TestSuite
static Mipv6TestSuite mipv6testsuite;

