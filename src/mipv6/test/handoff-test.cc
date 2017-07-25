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


class HandoffTestCase : public TestCase
{
public:
  HandoffTestCase ();
  virtual ~HandoffTestCase ();
  void CheckForHandoff ();
  void Initialize ();
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
HandoffTestCase::HandoffTestCase ()
  : TestCase ("Test two mipv6 tests for handoff")
{
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
HandoffTestCase::~HandoffTestCase ()
{
}



//
// This method is the pure virtual method from class TestCase that every
// TestCase must implement
//
void
HandoffTestCase::DoRun (void)
{
NodeContainer n, sta;
n.Create (4);
sta.Create (1);



InternetStackHelper internet;
internet.Install (n);
internet.Install (sta);


MobilityHelper mobility;
Ptr<ListPositionAllocator> positionAlloc;
positionAlloc = CreateObject<ListPositionAllocator> ();
positionAlloc->Add (Vector (120.0, 20.0, 0.0));  //1
positionAlloc->Add (Vector (120.0, 40.0, 0.0)); //2
positionAlloc->Add (Vector (95.0, 60.0, 0.0));  //3
positionAlloc->Add (Vector (145.0, 60.0, 0.0));  //4


mobility.SetPositionAllocator (positionAlloc);
mobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
mobility.Install (n);

positionAlloc = CreateObject<ListPositionAllocator> ();
positionAlloc->Add (Vector (85.0, 80.0, 0.0)); //STA
mobility.SetPositionAllocator (positionAlloc);
mobility.SetMobilityModel ("ns3::ConstantVelocityMobilityModel");  
mobility.Install(sta);

Ptr<ConstantVelocityMobilityModel> cvm = sta.Get(0)->GetObject<ConstantVelocityMobilityModel>();
cvm->SetVelocity(Vector (5, 0, 0)); //move left to right

Simulator::Schedule (Time (Seconds (14.0)), &ConstantVelocityMobilityModel::SetVelocity, cvm, Vector (-5, 0, 0));
Simulator::Schedule (Time (Seconds (28.0)), &ConstantVelocityMobilityModel::SetVelocity, cvm, Vector (5, 0, 0));
Simulator::Schedule (Time (Seconds (42.0)), &ConstantVelocityMobilityModel::SetVelocity, cvm, Vector (-5, 0, 0));
Simulator::Schedule (Time (Seconds (56.0)), &ConstantVelocityMobilityModel::SetVelocity, cvm, Vector (5, 0, 0));
Simulator::Schedule (Time (Seconds (70.0)), &ConstantVelocityMobilityModel::SetVelocity, cvm, Vector (-5, 0, 0));
Simulator::Schedule (Time (Seconds (84.0)), &ConstantVelocityMobilityModel::SetVelocity, cvm, Vector (5, 0, 0));



  // link four nodes
  PointToPointHelper pointToPoint;
  pointToPoint.SetDeviceAttribute ("DataRate", StringValue ("5Mbps"));
  pointToPoint.SetChannelAttribute ("Delay", StringValue ("2ms"));


  NetDeviceContainer d1, d2, d3, d4, d5, d6;

  d1 = pointToPoint.Install (n.Get (0), n.Get (1));
  d2 = pointToPoint.Install (n.Get (1), n.Get (2));
  d3 = pointToPoint.Install (n.Get (1), n.Get (3));


  Ipv6AddressHelper ipv61,ipv62,ipv63,ipv64,ipv65,ipv66;
  Ipv6InterfaceContainer iifc,iifc1,iifc2,iifc3,iifc4,iifc5,iifc6;


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
  iifc = ipv63.Assign (d3);
  iifc3.Add(iifc);
  iifc3.SetForwarding (0, true);
  iifc3.SetDefaultRouteInAllNodes (0);
  iifc3.SetForwarding (1, true);
  iifc3.SetDefaultRouteInAllNodes (1);

  Ssid ssid = Ssid("ns-3-ssid");
  YansWifiPhyHelper wifiPhy = YansWifiPhyHelper::Default ();
  wifiPhy.SetPcapDataLinkType (YansWifiPhyHelper::DLT_IEEE802_11_RADIO);

  WifiHelper wifi;
  NqosWifiMacHelper wifiMac = NqosWifiMacHelper::Default ();
  YansWifiChannelHelper wifiChannel = YansWifiChannelHelper::Default ();
  wifiPhy.SetChannel (wifiChannel.Create ());
   
  wifiMac.SetType ("ns3::ApWifiMac",
		           "Ssid", SsidValue (ssid),
		           "BeaconGeneration", BooleanValue (true),
		           "BeaconInterval", TimeValue (MilliSeconds(100)));
  d4 = wifi.Install (wifiPhy, wifiMac, n.Get(2));
  d5 = wifi.Install (wifiPhy, wifiMac, n.Get(3));


  wifiMac.SetType ("ns3::StaWifiMac",
	               "Ssid", SsidValue (ssid),
	               "ActiveProbing", BooleanValue (false));
  d6.Add( wifi.Install (wifiPhy, wifiMac, sta.Get(0)));


  ipv64.SetBase (Ipv6Address ("4444::"), Ipv6Prefix (64));
  iifc = ipv64.Assign (d4);
  iifc4.Add(iifc);
  iifc4.SetForwarding (0, true);
  iifc4.SetDefaultRouteInAllNodes (0);

  ipv65.SetBase (Ipv6Address ("5555::"), Ipv6Prefix (64));
  iifc = ipv65.Assign (d5);
  iifc5.Add(iifc);
  iifc5.SetForwarding (0, true);
  iifc5.SetDefaultRouteInAllNodes (0);


  iifc = ipv66.AssignWithoutAddress (d6);
  iifc6.Add(iifc);


  Ipv6Address prefix1 ("4444::");  //create the prefix 

  uint32_t indexRouter1 = iifc4.GetInterfaceIndex (0);  //AR interface (n-AR1) 

  Ptr<Radvd> radvd1=CreateObject<Radvd> ();
  Ptr<RadvdInterface> routerInterface1= Create<RadvdInterface> (indexRouter1, 1500, 50);
  Ptr<RadvdPrefix> routerPrefix1 = Create<RadvdPrefix> (prefix1, 64, 1.5, 2.0);

  routerInterface1->AddPrefix(routerPrefix1);

  radvd1->AddConfiguration(routerInterface1);

  n.Get(2)->AddApplication(radvd1);
  radvd1->SetStartTime(Seconds (1.0));
  radvd1->SetStopTime(Seconds (60.0));

  Ipv6Address prefix ("5555::");  //create the prefix 

  uint32_t indexRouter = iifc5.GetInterfaceIndex (0);  //AR interface (n-AR1) 

  Ptr<Radvd> radvd=CreateObject<Radvd> ();
  Ptr<RadvdInterface> routerInterface= Create<RadvdInterface> (indexRouter, 1500, 50);
  Ptr<RadvdPrefix> routerPrefix = Create<RadvdPrefix> (prefix, 64, 1.5, 2.0);

  routerInterface->AddPrefix(routerPrefix);

  radvd->AddConfiguration(routerInterface);

  n.Get(3)->AddApplication(radvd);
  radvd->SetStartTime(Seconds (1.0));
  radvd->SetStopTime(Seconds (60.0));



  Ipv6StaticRoutingHelper routingHelper;
  Ptr<Ipv6> ipv692 = n.Get(1)->GetObject<Ipv6> ();
  Ptr<Ipv6StaticRouting> rttop = routingHelper.GetStaticRouting(ipv692);
  rttop->AddNetworkRouteTo(Ipv6Address("4444::"),Ipv6Prefix(64),Ipv6Address("2222::200:ff:fe00:4"),2,0);
  rttop->AddNetworkRouteTo(Ipv6Address("5555::"),Ipv6Prefix(64),Ipv6Address("3333::200:ff:fe00:6"),3,0);

  Ptr<Ipv6> ipv693 = n.Get(2)->GetObject<Ipv6> ();
  rttop = routingHelper.GetStaticRouting(ipv693);
  rttop->AddNetworkRouteTo(Ipv6Address("1111::"),Ipv6Prefix(64),Ipv6Address("2222::200:ff:fe00:3"),1,0);
  rttop->AddNetworkRouteTo(Ipv6Address("3333::"),Ipv6Prefix(64),Ipv6Address("2222::200:ff:fe00:3"),1,0);

  Ptr<Ipv6> ipv694 = n.Get(3)->GetObject<Ipv6> ();
  rttop = routingHelper.GetStaticRouting(ipv694);
  rttop->AddNetworkRouteTo(Ipv6Address("1111::"),Ipv6Prefix(64),Ipv6Address("3333::200:ff:fe00:5"),1,0);
  rttop->AddNetworkRouteTo(Ipv6Address("2222::"),Ipv6Prefix(64),Ipv6Address("3333::200:ff:fe00:5"),1,0);


  InstallMIPv6 (sta.Get(0), n.Get(1), n.Get(0));

  Initialize();


//  InstallApplication (2., 5, 4, 15);

//  SetCallback();


  Simulator::Schedule (Time (Seconds (16.0)), &HandoffTestCase::CheckForHandoff, this);



  Simulator::Stop (Seconds (18.0));
  Simulator::Run ();
  Simulator::Destroy ();

}

void HandoffTestCase::Initialize ()
{
  d=0;
  Ptr<mipv6MN> mipmn = mn->GetObject<mipv6MN> (); 
  clientaddress = mipmn->GetHomeAddress ();
  serveraddress = Ipv6Address("1111::200:ff:fe00:1");
  pktcounter=0;
  pktcounterathandoff=0;
  list1.clear ();
  list2.clear ();
  list3.clear ();
  bulist.clear ();
  pkt_tun_recv=true; pkt_tun_sent=true; bu_seq=true; bu_found=true; mn_found_ha=true; coa_match_ha=true;
  SetCallback ();
  InstallApplication (2, 5, 4, 15);
}


void HandoffTestCase::CheckForHandoff ()
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
if(list2.size() != 0)
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


void HandoffTestCase::InstallMIPv6 (Ptr<Node> m, Ptr<Node> h, Ptr<Node> c)
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


void HandoffTestCase::InstallApplication (double interval, uint32_t maxpktcount, double starttime, double stoptime)
{
  uint16_t port = 4000;
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
std::cout<< "jio\n";
}


void HandoffTestCase::RxPktAtTun (Ptr<Packet> p, Ipv6Header ih, Ipv6Header oh, Ptr<Ipv6Interface> i)
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

void HandoffTestCase::TxPktAtTun (Ptr<Packet> p, Ipv6Header ih, Ipv6Header oh)
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

void HandoffTestCase::TxPktAtClient (Ptr<const Packet> p, Ptr<Ipv6> ipv6, uint32_t interface)
{
Ipv6Header h;
p->PeekHeader(h);
if (h.GetSourceAddress ().IsEqual(clientaddress) && h.GetDestinationAddress ().IsEqual(serveraddress))
  {
    list3.push_back (p->GetUid ());
  }
std::cout<< "pio\n";
}

void HandoffTestCase::RxPktAtServer (Ptr<const Packet> p, Ptr<Ipv6> ipv6, uint32_t interface)
{
Ipv6Header h;
p->PeekHeader(h);
if (h.GetSourceAddress ().IsEqual(clientaddress) && h.GetDestinationAddress ().IsEqual(serveraddress))
  {
    list1.push_back (p->GetUid ());
std::cout<< "qio\n";
  }
}


void HandoffTestCase::TxBU (Ptr<Packet> packet, Ipv6Address src, Ipv6Address dst)
{
bulist.push_back (packet);
std::cout<< "wio\n";
}

void HandoffTestCase::RxBA (Ptr<Packet> packet, Ipv6Address src, Ipv6Address dst, Ptr<Ipv6Interface> interface)
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


void HandoffTestCase::SetCallback ()
{
if (d == 0)
  {
    Ptr<Ipv6L3Protocol> ipl3 = cn->GetObject<Ipv6L3Protocol> ();
    ipl3->TraceConnectWithoutContext ("Rx", MakeCallback(&HandoffTestCase::RxPktAtServer, this));
    Ptr<Ipv6TunnelL4Protocol> iptun = ha->GetObject<Ipv6TunnelL4Protocol> ();
    iptun->TraceConnectWithoutContext ("RxHa", MakeCallback(&HandoffTestCase::RxPktAtTun, this));
    Ptr<Ipv6TunnelL4Protocol> iptun2 = mn->GetObject<Ipv6TunnelL4Protocol> ();
    iptun2->SetTxCallback (MakeCallback (&HandoffTestCase::TxPktAtTun, this)); 
  }
else
  {
    Ptr<Ipv6TunnelL4Protocol> iptun = mn->GetObject<Ipv6TunnelL4Protocol> ();
    iptun->TraceConnectWithoutContext ("RxMn", MakeCallback(&HandoffTestCase::RxPktAtTun, this));
    Ptr<Ipv6TunnelL4Protocol> iptun2 = ha->GetObject<Ipv6TunnelL4Protocol> ();
    iptun2->SetTxCallback (MakeCallback (&HandoffTestCase::TxPktAtTun, this));
    Ptr<Ipv6L3Protocol> ipl3 = cn->GetObject<Ipv6L3Protocol> ();
    ipl3->TraceConnectWithoutContext ("Tx", MakeCallback(&HandoffTestCase::TxPktAtClient, this));    
  }

Ptr<mipv6MN> mipmn = mn->GetObject<mipv6MN> ();
mipmn->TraceConnectWithoutContext ("TxBU", MakeCallback(&HandoffTestCase::TxBU, this));
mipmn->TraceConnectWithoutContext ("RxBA", MakeCallback(&HandoffTestCase::RxBA, this));
std::cout<< "bio\n";
}




class Mipv6TestSuite2 : public TestSuite
{
public:
  Mipv6TestSuite2 ();
};

Mipv6TestSuite2::Mipv6TestSuite2 ()
  : TestSuite ("mipv6-handoff-test", UNIT)
{
  // TestDuration for TestCase can be QUICK, EXTENSIVE or TAKES_FOREVER
  AddTestCase (new HandoffTestCase, TestCase::QUICK);
}

// Do not forget to allocate an instance of this TestSuite
static Mipv6TestSuite2 mipv6testsuite2;

