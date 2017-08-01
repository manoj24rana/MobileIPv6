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

#include "ns3/uinteger.h"
#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/applications-module.h"
#include "ns3/csma-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/ipv6-static-routing.h"
#include "ns3/mipv6-module.h"
#include "ns3/radvd.h"
#include "ns3/radvd-interface.h"
#include "ns3/radvd-prefix.h"

#include "ns3/mobility-module.h"
#include "ns3/wifi-module.h"


/* *
   * Scenario: without handoff

                 Class: HandoffTestCase

   * Pass criteria: 1) Every received packets (before/after handoff) must be tunneled properly (as defined
                       in RFC 6275) and
                    2) All handoffs must be successful.

   * Use Cases:   Data transmission rate change: 1 Kbps, 10 Kbps, 50 Kbps, 100 kbps


   * Process:  1) (All received packets have passed through HA-MN/ MN-HA tunnel) && (All tunneled packets
                  use right tunnel identifiers i.e. correct source and destination IPv6 addresses) = Every
                  received packet have been tunneled properly.

                      Corresponding functions: TxPktAtTun (), RxPktAtTun (), TxPktAtClient (),
                                                and Test ()

               2) (A successful BU-BA transmission, at handoff) && (HA's Cache is updated with MN's new CoA)
                  (Future packets of that handoff tunneled properly) = A handoff is successful
  
                    Functions: RxBA (), Test () and all Rx/Tx functions
                    Variables: pktcounter and pktcounterathandoff


Note: We only consider received packets and received Binding packets because sent packets and BUs may be lost
      due to link failure or, handoff operation or, any other issues

* */



using namespace ns3;

class HandoffTestCase : public TestCase
{
public:
  HandoffTestCase ();
  virtual ~HandoffTestCase ();

  /**
   * \brief Initialize testing parameters.
   * \param: dir direction of transmission. 0 for MN to CN and 1 for CN to MN
   */
  void Initialize (uint8_t dir);

  /**
   * \brief Install MIPv6 agents.
   * \param: m mobile node
   * \param: h home agent
   * \param: c correspondent node
   */
  void InstallMIPv6 (Ptr<Node> m, Ptr<Node> h, Ptr<Node> c);

  /**
   * \brief trace packets received at tunnel end point (HA/MN)
   * \param: p original packet
   * \param: ih IPv6 inner header
   * \param: oh IPv6 outer header
   * \param: i interface which receives that packet
   */
  void RxPktAtTun (Ptr<Packet> p, Ipv6Header ih, Ipv6Header oh, Ptr<Ipv6Interface> i);

  /**
   * \brief trace packets transmitted at tunnel start point (HA/MN)
   * \param: p original packet
   * \param: ih IPv6 inner header
   * \param: oh IPv6 outer header
   */
  void TxPktAtTun (Ptr<Packet> p, Ipv6Header ih, Ipv6Header oh);

  /**
   * \brief trace packets transmitted from client (CN) at IPv6 layer
   * \param: p original packet
   * \param: ipv6 IPv6 object associated with the node which transmits
   * \param: interface interface which receives that packet
   */
  void TxPktAtClient (Ptr<const Packet> p, Ptr<Ipv6> ipv6, uint32_t interface);

  /**
   * \brief trace bu packets sent by MN
   * \param: packet bu packet
   * \param: src source address
   * \param: dst destination address
   */
  void TxBU (Ptr<Packet> packet, Ipv6Address src, Ipv6Address dst);

  /**
   * \brief trace ba packets received by MN
   * \param: packet ba packet
   * \param: src source address
   * \param: dst destination address
   * \param: interface interface which receives that packet
   */
  void RxBA (Ptr<Packet> packet, Ipv6Address src, Ipv6Address dst, Ptr<Ipv6Interface> interface);

  /**
   * \brief set trace callbacks for MN, HA and CN
   */
  void SetCallbacks ();

  /**
   * \brief install two udp client-server applications in two directions
   */
  void InstallApplications ();

  /**
   * \brief test whether all data packets transmitted before the call time of this function
   *  are passed or not and test the whether handoff is successful or, not 
   */
  void Test ();

private:
  virtual void DoRun (void);
  Ipv6Address clientaddress, serveraddress; // dynamically set while running code
  Ptr<Node> mn;
  Ptr<Node> ha;
  Ptr<Node> cn;
  uint64_t pktcounter;
  uint64_t pktcounterathandoff; // packet conter at the time of ba reception
  uint8_t d; // False if MN to CN and true if CN to MN
  std::list<uint64_t> list1; // contains received packet's UID at server
  std::list<uint64_t> list2; // contains received packet's UID at HA
  std::list<uint64_t> list3; // contains received packet's UID at client
  std::list<Ptr<Packet> > bulist;  // contains sent BUs of an MN
  bool pkt_tun_recv; //flag to check whether received encapsulated packet's header contains correct identifier
  bool pkt_tun_sent; //flag to check whether sent encapsulated packet's header contains correct identifier
  bool bu_seq;  //flag to check whether received ba's sequence number matches with the sent bu's sequence number
  bool bu_found;  // flag to check whether sent bu is not from the MN which receives corresponding ba
  bool mn_found_ha;  //flag to check whether MN's hoa is found at HA's cache
  bool coa_match_ha; //flag to check whether currently registered has hoa-coa binding at HA
};


HandoffTestCase::HandoffTestCase ()
  : TestCase ("Test two mipv6 tests for no handoff")
{
  d = 1;
  pkt_tun_recv = true;
  pkt_tun_sent = true;
  bu_seq = true;
  bu_found = true;
  mn_found_ha = true;
  coa_match_ha = true;
}

HandoffTestCase::~HandoffTestCase ()
{
}

void
HandoffTestCase::DoRun (void)
{
/*
                                     -----
                                    | CN |
                                     -----
                                   p2p | 1111::/64
                                       |
                                     -----
                                    | HA |
                                     -----
                                      /\                                         
                        2222::/64   /    \ 3333::/64
                                  / p2p    \ p2p
                                /            \
                            -----          -----
                           | AR1 |        | AR2 |
                            -----          -----
                             ^ 4444::/64      | 5555::/64
                        wifi ^
                             ^ 
                           ------     5 m/s
                           | MN | ===============>
                           ------ <===============       
                                     -5 m/s
*/



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
  mobility.Install (sta);

  Ptr<ConstantVelocityMobilityModel> cvm = sta.Get (0)->GetObject<ConstantVelocityMobilityModel> ();
  cvm->SetVelocity(Vector (5, 0, 0)); //move left to right

  Simulator::Schedule (Time (Seconds (14.0)), &ConstantVelocityMobilityModel::SetVelocity, cvm, Vector (-5, 0, 0)); //move right to left
  Simulator::Schedule (Time (Seconds (28.0)), &ConstantVelocityMobilityModel::SetVelocity, cvm, Vector (5, 0, 0));
  Simulator::Schedule (Time (Seconds (42.0)), &ConstantVelocityMobilityModel::SetVelocity, cvm, Vector (-5, 0, 0));
  Simulator::Schedule (Time (Seconds (56.0)), &ConstantVelocityMobilityModel::SetVelocity, cvm, Vector (5, 0, 0));
  Simulator::Schedule (Time (Seconds (70.0)), &ConstantVelocityMobilityModel::SetVelocity, cvm, Vector (-5, 0, 0));
  Simulator::Schedule (Time (Seconds (84.0)), &ConstantVelocityMobilityModel::SetVelocity, cvm, Vector (5, 0, 0));



  // link four nodes
  PointToPointHelper pointToPoint;
  pointToPoint.SetDeviceAttribute ("DataRate", StringValue ("100Mbps"));
  pointToPoint.SetChannelAttribute ("Delay", StringValue ("2ms"));


  NetDeviceContainer d1, d2, d3, d4, d5, d6;

  d1 = pointToPoint.Install (n.Get (0), n.Get (1));
  d2 = pointToPoint.Install (n.Get (1), n.Get (2));
  d3 = pointToPoint.Install (n.Get (1), n.Get (3));


  Ipv6AddressHelper ipv61, ipv62, ipv63, ipv64, ipv65, ipv66;
  Ipv6InterfaceContainer iifc, iifc1, iifc2, iifc3, iifc4, iifc5, iifc6;


  ipv61.SetBase (Ipv6Address ("1111::"), Ipv6Prefix (64));
  iifc = ipv61.Assign (d1);
  iifc1.Add (iifc);
  iifc1.SetForwarding (0, true);
  iifc1.SetDefaultRouteInAllNodes (0);
  iifc1.SetForwarding (1, true);
  iifc1.SetDefaultRouteInAllNodes (1);

  ipv62.SetBase (Ipv6Address ("2222::"), Ipv6Prefix (64));
  iifc = ipv62.Assign (d2);
  iifc2.Add (iifc);
  iifc2.SetForwarding (0, true);
  iifc2.SetDefaultRouteInAllNodes (0);
  iifc2.SetForwarding (1, true);
  iifc2.SetDefaultRouteInAllNodes (1);

  ipv63.SetBase (Ipv6Address ("3333::"), Ipv6Prefix (64));
  iifc = ipv63.Assign (d3);
  iifc3.Add (iifc);
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
		           "BeaconInterval", TimeValue (MilliSeconds (100)));
  d4 = wifi.Install (wifiPhy, wifiMac, n.Get (2));
  d5 = wifi.Install (wifiPhy, wifiMac, n.Get (3));


  wifiMac.SetType ("ns3::StaWifiMac",
	               "Ssid", SsidValue (ssid),
	               "ActiveProbing", BooleanValue (false));
  d6.Add (wifi.Install (wifiPhy, wifiMac, sta.Get (0)));


  ipv64.SetBase (Ipv6Address ("4444::"), Ipv6Prefix (64));
  iifc = ipv64.Assign (d4);
  iifc4.Add (iifc);
  iifc4.SetForwarding (0, true);
  iifc4.SetDefaultRouteInAllNodes (0);

  ipv65.SetBase (Ipv6Address ("5555::"), Ipv6Prefix (64));
  iifc = ipv65.Assign (d5);
  iifc5.Add (iifc);
  iifc5.SetForwarding (0, true);
  iifc5.SetDefaultRouteInAllNodes (0);


  iifc = ipv66.AssignWithoutAddress (d6);
  iifc6.Add (iifc);


  Ipv6Address prefix1 ("4444::");  //create the prefix 

  uint32_t indexRouter1 = iifc4.GetInterfaceIndex (0);  //AR interface (n-AR1) 

  Ptr<Radvd> radvd1 = CreateObject<Radvd> ();
  Ptr<RadvdInterface> routerInterface1 = Create<RadvdInterface> (indexRouter1, 1500, 50);
  Ptr<RadvdPrefix> routerPrefix1 = Create<RadvdPrefix> (prefix1, 64, 1.5, 2.0);

  routerInterface1->AddPrefix (routerPrefix1);

  radvd1->AddConfiguration (routerInterface1);

  n.Get (2)->AddApplication (radvd1);
  radvd1->SetStartTime (Seconds (1.0));
  radvd1->SetStopTime (Seconds (200.0));

  Ipv6Address prefix ("5555::");  //create the prefix 

  uint32_t indexRouter = iifc5.GetInterfaceIndex (0);  //AR interface (n-AR1) 

  Ptr<Radvd> radvd = CreateObject<Radvd> ();
  Ptr<RadvdInterface> routerInterface = Create<RadvdInterface> (indexRouter, 1500, 50);
  Ptr<RadvdPrefix> routerPrefix = Create<RadvdPrefix> (prefix, 64, 1.5, 2.0);

  routerInterface->AddPrefix (routerPrefix);

  radvd->AddConfiguration (routerInterface);

  n.Get (3)->AddApplication (radvd);
  radvd->SetStartTime(Seconds (1.0));
  radvd->SetStopTime(Seconds (200.0));



  Ipv6StaticRoutingHelper routingHelper;
  Ptr<Ipv6> ipv692 = n.Get (1)->GetObject<Ipv6> ();
  Ptr<Ipv6StaticRouting> rttop = routingHelper.GetStaticRouting (ipv692);
  rttop->AddNetworkRouteTo (Ipv6Address ("4444::"), Ipv6Prefix (64), Ipv6Address ("2222::200:ff:fe00:4"), 2, 0);
  rttop->AddNetworkRouteTo (Ipv6Address ("5555::"), Ipv6Prefix (64), Ipv6Address ("3333::200:ff:fe00:6"), 3, 0);

  Ptr<Ipv6> ipv693 = n.Get (2)->GetObject<Ipv6> ();
  rttop = routingHelper.GetStaticRouting (ipv693);
  rttop->AddNetworkRouteTo (Ipv6Address ("1111::"), Ipv6Prefix (64), Ipv6Address ("2222::200:ff:fe00:3"), 1, 0);
  rttop->AddNetworkRouteTo (Ipv6Address("3333::"), Ipv6Prefix(64), Ipv6Address("2222::200:ff:fe00:3"), 1, 0);

  Ptr<Ipv6> ipv694 = n.Get (3)->GetObject<Ipv6> ();
  rttop = routingHelper.GetStaticRouting (ipv694);
  rttop->AddNetworkRouteTo(Ipv6Address ("1111::"), Ipv6Prefix (64), Ipv6Address ("3333::200:ff:fe00:5"), 1, 0);
  rttop->AddNetworkRouteTo(Ipv6Address ("2222::"), Ipv6Prefix (64), Ipv6Address ("3333::200:ff:fe00:5"), 1, 0);


  //Install MIPv6
  InstallMIPv6 (sta.Get (0), n.Get (1), n.Get (0));
  Ptr<Mipv6Mn> mipmn = mn->GetObject<Mipv6Mn> ();


  // Install Required Callbacks, applications and initialize parameters
  SetCallbacks();
  InstallApplications ();
  Initialize (1);   //initialize at MN->CN direction

  //Extracting Applications of MN and CN
  Ptr<Application> capp2 = cn->GetApplication (0);

  /*
   *  Now testing beguns for 4 cases changing transmission rate
  */



  // Running Application i.e. MN->CN and check for 4 different data rates


  //Check the test for 1 Kbps speed
  Simulator::Schedule (Time (Seconds (5.1)), &HandoffTestCase::Initialize, this, 1);
  Simulator::Schedule (Time (Seconds (14.0)), &HandoffTestCase::Test, this);

  //Check the test for 10 Kbps speed
  Simulator::Schedule (Time (Seconds (14.1)), &HandoffTestCase::Initialize, this, 1);
  Simulator::Schedule (Time (Seconds (14.2)), &UdpClient::SetAttribute, capp2, "Interval", TimeValue (Seconds (.1)));
  Simulator::Schedule (Time (Seconds (28.0)), &HandoffTestCase::Test, this);

  //Check the test for 50 Kbps speed
  Simulator::Schedule (Time (Seconds (28.1)), &HandoffTestCase::Initialize, this, 1);
  Simulator::Schedule (Time (Seconds (28.2)), &UdpClient::SetAttribute, capp2, "Interval", TimeValue (Seconds (.02)));
  Simulator::Schedule (Time (Seconds (56.0)), &HandoffTestCase::Test, this);

  //Check the test for 100 kbps speed
  Simulator::Schedule (Time (Seconds (56.1)), &HandoffTestCase::Initialize, this, 1);
  Simulator::Schedule (Time (Seconds (56.2)), &UdpClient::SetAttribute, capp2, "Interval", TimeValue (Seconds (.01)));
  Simulator::Schedule (Time (Seconds (95.0)), &HandoffTestCase::Test, this);

  Simulator::Stop (Seconds (100.0));
  Simulator::Run ();
  Simulator::Destroy ();

}

void HandoffTestCase::Initialize (uint8_t dir)
{
  d = dir;
  Ptr<Mipv6Mn> mipmn = mn->GetObject<Mipv6Mn> ();

  clientaddress = Ipv6Address ("1111::200:ff:fe00:1");
  serveraddress = mipmn->GetHomeAddress ();
  
  pktcounter = 0;
  pktcounterathandoff = 0;

  list1.clear ();
}

void HandoffTestCase::InstallApplications ()
{
  //Second Application CN->MN

  Ptr<Mipv6Mn> mipmn = mn->GetObject<Mipv6Mn> ();
  uint16_t port = 5000;
  UdpServerHelper server2 (port);
  
  ApplicationContainer serverapps = server2.Install (mn);      
    
  serverapps.Start (Seconds (5));
  serverapps.Stop (Seconds (95));



  uint32_t MaxPacketSize = 1024;
  Time interPacketInterval = Seconds (1.0);
  uint32_t maxPacketCount = 50000;
  UdpClientHelper client2 (mipmn->GetHomeAddress (), port);

  client2.SetAttribute ("MaxPackets", UintegerValue (maxPacketCount));
  client2.SetAttribute ("Interval", TimeValue (interPacketInterval));
  client2.SetAttribute ("PacketSize", UintegerValue (MaxPacketSize));

  ApplicationContainer clientapps = client2.Install (cn);
    
  clientapps.Start (Seconds (5));
  clientapps.Stop (Seconds (95));
}


void HandoffTestCase::Test ()
{
  bool f = true;
  std::list<uint64_t>::iterator it, it1, it2;

      for (it = list1.begin(); it != list1.end(); ++it)
        {
          it1 = std::find (list2.begin (), list2.end (), *it);
          it2 = std::find (list3.begin (), list3.end (), *it);
          if(it1 == list2.end () || it2 == list3.end())
            {
              f = false;
              break;
            } 
        }

    

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
  Mipv6HaHelper hh;
  hh.Install (h);
  Mipv6MnHelper mh (hh.GetHomeAgentAddressList(), false); 
  mh.Install (m);

  mn = m;
  ha = h;
  cn = c;
}



void HandoffTestCase::RxPktAtTun (Ptr<Packet> p, Ipv6Header ih, Ipv6Header oh, Ptr<Ipv6Interface> i)
{
  pktcounter++;
  if (ih.GetSourceAddress ().IsEqual(clientaddress) && ih.GetDestinationAddress ().IsEqual(serveraddress))
    {
      list1.push_back (p->GetUid ());
      Ptr<Mipv6Mn> mipmn = mn->GetObject<Mipv6Mn>();
      pkt_tun_recv &= (oh.GetDestinationAddress ().IsEqual(mipmn->GetCoA()) && mipmn->IsHomeMatch(oh.GetSourceAddress ()));
    }
}

void HandoffTestCase::TxPktAtTun (Ptr<Packet> p, Ipv6Header ih, Ipv6Header oh)
{
  if (ih.GetSourceAddress ().IsEqual(clientaddress) && ih.GetDestinationAddress ().IsEqual(serveraddress))
    {
      list2.push_back (p->GetUid ());
      Ptr<Mipv6Mn> mipmn = mn->GetObject<Mipv6Mn>();
      pkt_tun_sent &= mipmn->IsHomeMatch(oh.GetSourceAddress ());  //Check the source address only, because destination is MN's CoA, which will
    }                                                              //not match while HA tries to send, MN assigns different CoA due to handoff
}

void HandoffTestCase::TxPktAtClient (Ptr<const Packet> p, Ptr<Ipv6> ipv6, uint32_t interface)
{
  Ipv6Header h;
  p->PeekHeader (h);
  if (h.GetSourceAddress ().IsEqual (clientaddress) && h.GetDestinationAddress ().IsEqual (serveraddress))
    {
      list3.push_back (p->GetUid ());
    }
}

void HandoffTestCase::TxBU (Ptr<Packet> packet, Ipv6Address src, Ipv6Address dst)
{
  bulist.push_back (packet);
}

void HandoffTestCase::RxBA (Ptr<Packet> packet, Ipv6Address src, Ipv6Address dst, Ptr<Ipv6Interface> interface)
{
  pktcounterathandoff = pktcounter;

  Ipv6MobilityBindingUpdateHeader bu;
  Ipv6MobilityBindingAckHeader ba;
  packet->RemoveHeader (ba);
  std::list<Ptr<Packet> >::iterator it;

  for (it = bulist.begin (); it != bulist.end (); ++it)
    {
      (*it)->PeekHeader (bu);

      if(ba.GetSequence () == bu.GetSequence ())
        {
          break;
        }
    }
  bu_seq &= (bu.GetSequence () == ba.GetSequence ());
  bu_found &= (it != bulist.end ());

//Testing whether correct HoA-CoA mapping exist in HA Cache

  Ipv6ExtensionType2RoutingHeader type2; //Fetching Type2 extension header which contains home address
  packet->RemoveHeader (type2);
  Ptr<Mipv6Ha> mipha = ha->GetObject<Mipv6Ha> ();
  PointerValue ptr;
  mipha->GetAttribute ("BCache", ptr);  //Fetching Cache of HA
  Ptr<BCache> bcache = ptr.Get<BCache> ();
  BCache::Entry *entry = bcache->Lookup (type2.GetHomeAddress ());

  mn_found_ha &= (entry != 0);

  Ptr<Mipv6Mn> mipmn = mn->GetObject<Mipv6Mn> ();
  coa_match_ha &= mipmn->GetCoA ().IsEqual(entry->GetCoa ());
}


void HandoffTestCase::SetCallbacks ()
{
  Ptr<Ipv6L3Protocol> ipl3 = cn->GetObject<Ipv6L3Protocol> ();
  ipl3->TraceConnectWithoutContext ("Tx", MakeCallback(&HandoffTestCase::TxPktAtClient, this));



  Ptr<Ipv6TunnelL4Protocol> iptun = ha->GetObject<Ipv6TunnelL4Protocol> ();
  iptun->SetTxCallback (MakeCallback (&HandoffTestCase::TxPktAtTun, this));


  Ptr<Ipv6TunnelL4Protocol> iptun2 = mn->GetObject<Ipv6TunnelL4Protocol> ();
  iptun2->TraceConnectWithoutContext ("RxMn", MakeCallback (&HandoffTestCase::RxPktAtTun, this));

  Ptr<Mipv6Mn> mipmn = mn->GetObject<Mipv6Mn> ();
  mipmn->TraceConnectWithoutContext ("TxBU", MakeCallback (&HandoffTestCase::TxBU, this));
  mipmn->TraceConnectWithoutContext ("RxBA", MakeCallback (&HandoffTestCase::RxBA, this));
}

/**
 * \brief test suite 2
 */
class Mipv6TestSuite2 : public TestSuite
{
  public:
    Mipv6TestSuite2 ();
};

Mipv6TestSuite2::Mipv6TestSuite2 ()
  : TestSuite ("mipv6-test2", UNIT)
{
  AddTestCase (new HandoffTestCase, TestCase::QUICK);
}

static Mipv6TestSuite2 mipv6testsuite;
