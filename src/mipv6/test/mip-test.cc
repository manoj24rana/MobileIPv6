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


/* *
   * Scenario: without handoff

                 Class: NoHandoffTestCase

   * Pass criteria: 1) Every received packet must be tunneled properly (as defined in RFC 6275) and
                    2) If there is any handoff that must be successful.

   * Use Cases:   1) Data transmission direction: MN to CN, CN to MN 
                  2) Data transmission rate change: 1 Kbps, 10 Kbps, 100 Kbps, 1Mbps


   * Process:  1) (All received packets have passed through HA-MN/ MN-HA tunnel) && (All tunneled packets
                  use right tunnel identifiers i.e. correct source and destination IPv6 addresses) = Every
                  received packet have been tunneled properly.

                      Corresponding functions: TxPktAtTun (), RxPktAtTun (), TxPktAtClient (),
                                                RxPktAtServer  and Test ()

               2) (A successful BU-BA transmission, at handoff) && (HA's Cache is updated with MN's new CoA)
                  (Future packets of that handoff tunneled properly) = A handoff is successful
  
                    Functions: RxBA (), Test () and all Rx/Tx functions
                    Variables: pktcounter and pktcounterathandoff


Note: We only consider received packets and received Binding packets because sent packets and BUs may be lost
      due to link failure or, handoff operation or, any other issues

* */



using namespace ns3;

class NoHandoffTestCase : public TestCase
{
public:
  NoHandoffTestCase ();
  virtual ~NoHandoffTestCase ();

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
   * \brief trace packets received at server (CN) at IPv6 layer
   * \param: p original packet
   * \param: ipv6 IPv6 object associated with the node which transmits
   * \param: interface interface which receives that packet
   */
  void RxPktAtServer (Ptr<const Packet> p, Ptr<Ipv6> ipv6, uint32_t interface);

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
  Ipv6Address clientaddress, serveraddress;
  Ptr<Node> mn;
  Ptr<Node> ha;
  Ptr<Node> cn;
  int pktcounter;
  int pktcounterathandoff; // packet conter at the time of ba reception
  uint8_t d; // False if MN to CN and true if CN to MN
  std::list<uint64_t> list1;  // contains received packet's UID at server
  std::list<uint64_t> list2;  // contains received packet's UID at HA
  std::list<uint64_t> list3;  // contains received packet's UID at client
  std::list<uint64_t> list4;
  std::list<uint64_t> list5;
  std::list<uint64_t> list6;
  std::list<Ptr<Packet> > bulist;  // contains sent BUs of an MN
  bool pkt_tun_recv; //flag to check whether received encapsulated packet's header contains correct identifier
  bool pkt_tun_sent; //flag to check whether sent encapsulated packet's header contains correct identifier
  bool bu_seq;  //flag to check whether received ba's sequence number matches with the sent bu's sequence number
  bool bu_found;  // flag to check whether sent bu is not from the MN which receives corresponding ba
  bool mn_found_ha;  //flag to check whether MN's hoa is found at HA's cache
  bool coa_match_ha; //flag to check whether currently registered has hoa-coa binding at HA
};


NoHandoffTestCase::NoHandoffTestCase ()
  : TestCase ("Test two mipv6 tests for no handoff")
{
  d = 0;
  pkt_tun_recv = true;
  pkt_tun_sent = true;
  bu_seq = true;
  bu_found = true;
  mn_found_ha = true;
  coa_match_ha = true;
}

NoHandoffTestCase::~NoHandoffTestCase ()
{
}

void
NoHandoffTestCase::DoRun (void)
{
  NodeContainer n;
  n.Create (4);

  InternetStackHelper internet;
  internet.Install (n);

  // link three nodes
  PointToPointHelper pointToPoint;
  pointToPoint.SetDeviceAttribute ("DataRate", StringValue ("100Mbps"));
  pointToPoint.SetChannelAttribute ("Delay", StringValue ("1ns"));

  NetDeviceContainer d1, d2, d3, d4, d5;

  d1 = pointToPoint.Install (n.Get (0), n.Get (1));
  d2 = pointToPoint.Install (n.Get (1), n.Get (2));
  d3 = pointToPoint.Install (n.Get (2), n.Get (3));
  d4 = NetDeviceContainer (d3.Get(0));
  d5 = NetDeviceContainer (d3.Get(1));

  Ipv6AddressHelper ipv61, ipv62, ipv63;
  Ipv6InterfaceContainer iifc, iifc1, iifc2, iifc3, iifc4;

  ipv61.SetBase (Ipv6Address ("1111::"), Ipv6Prefix (64));
  iifc = ipv61.Assign (d1);
  iifc1.Add(iifc);
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
  iifc = ipv63.Assign (d4);
  iifc3.Add(iifc);
  iifc3.SetForwarding (0, true);
  iifc3.SetDefaultRouteInAllNodes (0);

  iifc = ipv63.AssignWithoutAddress (d5);
  iifc4.Add (iifc);

  //RADVD application at AR as it is mandatory for our implementation
  Ipv6Address prefix ("3333::");  //create the prefix 

  uint32_t indexRouter = iifc3.GetInterfaceIndex (0);  //AR interface (n-AR1) 

  Ptr<Radvd> radvd = CreateObject<Radvd> ();
  Ptr<RadvdInterface> routerInterface = Create<RadvdInterface> (indexRouter, 1500, 50);
  Ptr<RadvdPrefix> routerPrefix = Create<RadvdPrefix> (prefix, 64, 1.5, 2.0);

  routerInterface->AddPrefix (routerPrefix);

  radvd->AddConfiguration (routerInterface);

  n.Get (2)->AddApplication (radvd);
  radvd->SetStartTime(Seconds (1.0));
  radvd->SetStopTime(Seconds (200.0));


  Ipv6StaticRoutingHelper routingHelper;
  Ptr<Ipv6> ipv692 = n.Get (1)->GetObject<Ipv6> ();
  Ptr<Ipv6StaticRouting> rttop = routingHelper.GetStaticRouting (ipv692);
  rttop->AddNetworkRouteTo (Ipv6Address ("3333::"), Ipv6Prefix (64), Ipv6Address ("2222::200:ff:fe00:4"), 2, 0);

  Ptr<Ipv6> ipv693 = n.Get (2)->GetObject<Ipv6> ();
  rttop = routingHelper.GetStaticRouting (ipv693);
  rttop->AddNetworkRouteTo (Ipv6Address ("1111::"), Ipv6Prefix (64), Ipv6Address ("2222::200:ff:fe00:3"), 1, 0);


  //Install MIPv6
  InstallMIPv6 (n.Get(3), n.Get(1), n.Get(0));
  Ptr<Mipv6Mn> mipmn = mn->GetObject<Mipv6Mn> ();


  // Install Required Callbacks, applications and initialize parameters
  SetCallbacks();
  InstallApplications ();
  Initialize (0);   //initialize at MN->CN direction

  //Extracting Applications of MN and CN
  Ptr<Application> capp1 = mn->GetApplication (0);
  Ptr<Application> capp2 = cn->GetApplication (1);

  /*
   *  Now testing beguns for 8 cases changing the direction of transmission and
   *  changing transmission rate
  */



  // Running First Application i.e. MN->CN and check for 4 different data rates

  //Check the test for 1 Kbps speed
  Simulator::Schedule (Time (Seconds (12.0)), &NoHandoffTestCase::Test, this);

  //Check the test for 10 Kbps speed
  Simulator::Schedule (Time (Seconds (12.1)), &NoHandoffTestCase::Initialize, this, 0);
  Simulator::Schedule (Time (Seconds (12.2)), &UdpClient::SetAttribute, capp1, "Interval", TimeValue (Seconds (.1)));
  Simulator::Schedule (Time (Seconds (22.0)), &NoHandoffTestCase::Test, this);

  //Check the test for 100 Kbps speed
  Simulator::Schedule (Time (Seconds (22.1)), &NoHandoffTestCase::Initialize, this, 0);
  Simulator::Schedule (Time (Seconds (22.2)), &UdpClient::SetAttribute, capp1, "Interval", TimeValue (Seconds (.01)));
  Simulator::Schedule (Time (Seconds (32.0)), &NoHandoffTestCase::Test, this);

  //Check the test for 1 Mbps speed
  Simulator::Schedule (Time (Seconds (32.1)), &NoHandoffTestCase::Initialize, this, 0);
  Simulator::Schedule (Time (Seconds (32.2)), &UdpClient::SetAttribute, capp1, "Interval", TimeValue (Seconds (.001)));
  Simulator::Schedule (Time (Seconds (42.0)), &NoHandoffTestCase::Test, this);



  // Running Second Application i.e. CN->MN and check for 4 different data rates

  //Check the test for 1 Kbps speed
  Simulator::Schedule (Time (Seconds (56.0)), &NoHandoffTestCase::Initialize, this, 1);
  Simulator::Schedule (Time (Seconds (65.0)), &NoHandoffTestCase::Test, this);

  //Check the test for 10 Kbps speed
  Simulator::Schedule (Time (Seconds (65.1)), &NoHandoffTestCase::Initialize, this, 1);
  Simulator::Schedule (Time (Seconds (65.2)), &UdpClient::SetAttribute, capp2, "Interval", TimeValue (Seconds (.1)));
  Simulator::Schedule (Time (Seconds (75.0)), &NoHandoffTestCase::Test, this);

  //Check the test for 100 Kbps speed
  Simulator::Schedule (Time (Seconds (75.1)), &NoHandoffTestCase::Initialize, this, 1);
  Simulator::Schedule (Time (Seconds (75.2)), &UdpClient::SetAttribute, capp2, "Interval", TimeValue (Seconds (.01)));
  Simulator::Schedule (Time (Seconds (85.0)), &NoHandoffTestCase::Test, this);

  //Check the test for 1 Mbps speed
  Simulator::Schedule (Time (Seconds (85.1)), &NoHandoffTestCase::Initialize, this, 1);
  Simulator::Schedule (Time (Seconds (85.2)), &UdpClient::SetAttribute, capp2, "Interval", TimeValue (Seconds (.001)));
  Simulator::Schedule (Time (Seconds (95.0)), &NoHandoffTestCase::Test, this);

  Simulator::Stop (Seconds (100.0));
  Simulator::Run ();
  Simulator::Destroy ();

}

void NoHandoffTestCase::Initialize (uint8_t dir)
{
  d = dir;
  Ptr<Mipv6Mn> mipmn = mn->GetObject<Mipv6Mn> ();
  if (d == 0)
  {
    clientaddress = mipmn->GetHomeAddress ();
    serveraddress = Ipv6Address ("1111::200:ff:fe00:1");
  }
  else
  {
    clientaddress = Ipv6Address ("1111::200:ff:fe00:1");
    serveraddress = mipmn->GetHomeAddress ();
  }
  pktcounter = 0;
  pktcounterathandoff = 0;
  list1.clear ();
  list4.clear ();
}

void NoHandoffTestCase::InstallApplications ()
{
  //First Application, MN->CN

  uint16_t port = 4000;
  UdpServerHelper server (port);
  ApplicationContainer serverapps;
  
  serverapps = server.Install (cn);      
    
  serverapps.Start (Seconds (4));
  serverapps.Stop (Seconds (50));



  uint32_t MaxPacketSize = 1024;
  Time interPacketInterval = Seconds (1.0);
  uint32_t maxPacketCount = 50000;
  UdpClientHelper client (Ipv6Address ("1111::200:ff:fe00:1"), port);

  client.SetAttribute ("MaxPackets", UintegerValue (maxPacketCount));
  client.SetAttribute ("Interval", TimeValue (interPacketInterval));
  client.SetAttribute ("PacketSize", UintegerValue (MaxPacketSize));

  ApplicationContainer clientapps;  
  clientapps = client.Install (mn);
    
  clientapps.Start (Seconds (4));
  clientapps.Stop (Seconds (50));

  //Second Application CN->MN

  Ptr<Mipv6Mn> mipmn = mn->GetObject<Mipv6Mn> ();
  port = 5000;
  UdpServerHelper server2 (port);
  
  serverapps = server2.Install (mn);      
    
  serverapps.Start (Seconds (55));
  serverapps.Stop (Seconds (95));



  MaxPacketSize = 1024;
  interPacketInterval = Seconds (1.0);
  maxPacketCount = 50000;
  UdpClientHelper client2 (mipmn->GetHomeAddress (), port);

  client2.SetAttribute ("MaxPackets", UintegerValue (maxPacketCount));
  client2.SetAttribute ("Interval", TimeValue (interPacketInterval));
  client2.SetAttribute ("PacketSize", UintegerValue (MaxPacketSize));

  clientapps = client2.Install (cn);
    
  clientapps.Start (Seconds (55));
  clientapps.Stop (Seconds (95));
}


void NoHandoffTestCase::Test ()
{
  bool f = true;
  std::list<uint64_t>::iterator it, it1, it2;
  if (d == 0)
    {
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
    }
  else
    {
      for (it = list4.begin(); it != list4.end(); ++it)
        {
          it1 = std::find (list5.begin (), list5.end (), *it);
          it2 = std::find (list6.begin (), list6.end (), *it);
          if(it1 == list5.end () || it2 == list6.end())
            {
              f = false;
              break;
            } 
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

void NoHandoffTestCase::InstallMIPv6 (Ptr<Node> m, Ptr<Node> h, Ptr<Node> c)
{
  Mipv6HaHelper hh;
  hh.Install (h);
  Mipv6MnHelper mh (hh.GetHomeAgentAddressList(), false); 
  mh.Install (m);

  mn = m;
  ha = h;
  cn = c;
}



void NoHandoffTestCase::RxPktAtTun (Ptr<Packet> p, Ipv6Header ih, Ipv6Header oh, Ptr<Ipv6Interface> i)
{
  pktcounter++;
  if(d == 0)
    {
      if (ih.GetSourceAddress ().IsEqual (clientaddress) && ih.GetDestinationAddress ().IsEqual (serveraddress))
        {
          list2.push_back (p->GetUid ());
          Ptr<Mipv6Mn> mipmn = mn->GetObject<Mipv6Mn> ();          
          pkt_tun_recv &= (oh.GetSourceAddress ().IsEqual (mipmn->GetCoA ()) && mipmn->IsHomeMatch (oh.GetDestinationAddress ()));
        }
    }
  else
    {
      if (ih.GetSourceAddress ().IsEqual(clientaddress) && ih.GetDestinationAddress ().IsEqual (serveraddress))
        {
          list4.push_back (p->GetUid ());
          Ptr<Mipv6Mn> mipmn = mn->GetObject<Mipv6Mn> ();
          pkt_tun_recv &= (oh.GetDestinationAddress ().IsEqual (mipmn->GetCoA ()) && mipmn->IsHomeMatch (oh.GetSourceAddress ()));
        }

    }
}

void NoHandoffTestCase::TxPktAtTun (Ptr<Packet> p, Ipv6Header ih, Ipv6Header oh)
{
  if(d == 0)
    {
      if (ih.GetSourceAddress ().IsEqual (clientaddress) && ih.GetDestinationAddress ().IsEqual (serveraddress))
        {
          list3.push_back (p->GetUid ());
          Ptr<Mipv6Mn> mipmn = mn->GetObject<Mipv6Mn> ();
          pkt_tun_sent &= oh.GetSourceAddress ().IsEqual (mipmn->GetCoA ()) && mipmn->IsHomeMatch (oh.GetDestinationAddress ());
        }
    }
  else
    {
      if (ih.GetSourceAddress ().IsEqual(clientaddress) && ih.GetDestinationAddress ().IsEqual(serveraddress))
        {
          list5.push_back (p->GetUid ());
          Ptr<Mipv6Mn> mipmn = mn->GetObject<Mipv6Mn> ();
          pkt_tun_sent &= oh.GetDestinationAddress ().IsEqual (mipmn->GetCoA ()) && mipmn->IsHomeMatch (oh.GetSourceAddress ());
        }

    }
}

void NoHandoffTestCase::TxPktAtClient (Ptr<const Packet> p, Ptr<Ipv6> ipv6, uint32_t interface)
{
  Ipv6Header h;
  p->PeekHeader (h);
  if (h.GetSourceAddress ().IsEqual (clientaddress) && h.GetDestinationAddress ().IsEqual (serveraddress))
    {
      list6.push_back (p->GetUid ());
    }
}

void NoHandoffTestCase::RxPktAtServer (Ptr<const Packet> p, Ptr<Ipv6> ipv6, uint32_t interface)
{
  Ipv6Header h;
  p->PeekHeader (h);
  if (h.GetSourceAddress ().IsEqual (clientaddress) && h.GetDestinationAddress ().IsEqual (serveraddress))
    {
      list1.push_back (p->GetUid ());
    }
}


void NoHandoffTestCase::TxBU (Ptr<Packet> packet, Ipv6Address src, Ipv6Address dst)
{
  bulist.push_back (packet);
}

void NoHandoffTestCase::RxBA (Ptr<Packet> packet, Ipv6Address src, Ipv6Address dst, Ptr<Ipv6Interface> interface)
{
  pktcounterathandoff = pktcounter;

  Ipv6MobilityBindingUpdateHeader bu;
  Ipv6MobilityBindingAckHeader ba;
  packet->RemoveHeader (ba);
  std::list<Ptr<Packet> >::iterator it;

  for (it = bulist.begin (); it!=bulist.end (); ++it)
    {
      (*it)->PeekHeader (bu);

      if(ba.GetSequence () == bu.GetSequence ())
        {
          break;
        }
    }
  bu_seq &= (bu.GetSequence () == ba.GetSequence ());
  bu_found &= (it != bulist.end());

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
  coa_match_ha &= mipmn->GetCoA ().IsEqual (entry->GetCoa ());
}






void NoHandoffTestCase::SetCallbacks ()
{
  Ptr<Ipv6L3Protocol> ipl3 = cn->GetObject<Ipv6L3Protocol> ();
  ipl3->TraceConnectWithoutContext ("Rx", MakeCallback(&NoHandoffTestCase::RxPktAtServer, this));
  ipl3->TraceConnectWithoutContext ("Tx", MakeCallback(&NoHandoffTestCase::TxPktAtClient, this));



  Ptr<Ipv6TunnelL4Protocol> iptun = ha->GetObject<Ipv6TunnelL4Protocol> ();
  iptun->TraceConnectWithoutContext ("RxHa", MakeCallback(&NoHandoffTestCase::RxPktAtTun, this));
  iptun->SetTxCallback (MakeCallback (&NoHandoffTestCase::TxPktAtTun, this));


  Ptr<Ipv6TunnelL4Protocol> iptun2 = mn->GetObject<Ipv6TunnelL4Protocol> ();
  iptun2->SetTxCallback (MakeCallback (&NoHandoffTestCase::TxPktAtTun, this)); 
  iptun2->TraceConnectWithoutContext ("RxMn", MakeCallback(&NoHandoffTestCase::RxPktAtTun, this));

  Ptr<Mipv6Mn> mipmn = mn->GetObject<Mipv6Mn> ();
  mipmn->TraceConnectWithoutContext ("TxBU", MakeCallback(&NoHandoffTestCase::TxBU, this));
  mipmn->TraceConnectWithoutContext ("RxBA", MakeCallback(&NoHandoffTestCase::RxBA, this));
}

/**
 * \brief test suite 1
 */
class Mipv6TestSuite : public TestSuite
{
  public:
    Mipv6TestSuite ();
};

Mipv6TestSuite::Mipv6TestSuite ()
  : TestSuite ("mipv6-test1", UNIT)
{
  AddTestCase (new NoHandoffTestCase, TestCase::QUICK);
}

static Mipv6TestSuite mipv6testsuite;
