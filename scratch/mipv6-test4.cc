/* -*-  Mode: C++; c-file-style: "gnu"; indent-tabs-mode:nil; -*- */
/*
 *
 * Mobile IPv6 (RFC3775) Implementation in NS3 
 *
 * Jadavpur Univerity (JU)
 * School of Mobile Computing and Communucation (SMCC)
 *
 * Author: M. K. Rana <manoj24.rana@gmail.com>
 *
 */

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
#include "ns3/ipv6-static-source-routing.h"
#include "ns3/ipv6-list-routing-helper.h"
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

//




//


using namespace ns3;
NS_LOG_COMPONENT_DEFINE ("mip6Wifi");

void
ReceivePacket(Ptr<const Packet> p, const Address & addr)
{
	std::cout << Simulator::Now ().GetSeconds () << "\t" << p->GetSize() <<"\n";
}



int main (int argc, char *argv[])
{

NodeContainer sta;
NodeContainer ha;
NodeContainer cn;
NodeContainer ars;
NodeContainer mid;
NodeContainer backbone1;
NodeContainer backbone2;

CommandLine cmd;
cmd.Parse (argc, argv);

  ars.Create(2);
  ha.Create(1);
  sta.Create(2);
  cn.Create(1);
  mid.Create(1);



NetDeviceContainer staDevs;
NetDeviceContainer haDevs;
NetDeviceContainer cnDevs;
NetDeviceContainer ar1Devs;
NetDeviceContainer ar2Devs;
NetDeviceContainer midDevs;
NetDeviceContainer backbone1Devs;
NetDeviceContainer backbone2Devs;



Ipv6InterfaceContainer staIfs;
Ipv6InterfaceContainer haIfs;
Ipv6InterfaceContainer cnIfs;
Ipv6InterfaceContainer ar1Ifs;
Ipv6InterfaceContainer ar2Ifs;
Ipv6InterfaceContainer midIfs;
Ipv6InterfaceContainer backbone1Ifs;
Ipv6InterfaceContainer backbone2Ifs;



//SeedManager::SetSeed (123456);


InternetStackHelper internet;

internet.Install (ars);
internet.Install (mid);
internet.Install (ha);
internet.Install (cn);
internet.Install (sta);

/*
Ipv6ListRoutingHelper list;
Ipv6StaticSourceRoutingHelper sourcer;
Ipv6StaticRoutingHelper staticr;
list.Add (sourcer, 10);
list.Add (staticr, 0);
internet.SetRoutingHelper (list);

internet.Install (sta);
*/

backbone1.Add(mid.Get(0));
backbone1.Add(ars.Get(0));
backbone1.Add(ars.Get(1));

backbone2.Add(cn.Get(0));
backbone2.Add(mid.Get(0));
backbone2.Add(ha.Get(0));

CsmaHelper csma;
Ipv6AddressHelper ipv6;
Ipv6InterfaceContainer iifc;

csma.SetChannelAttribute ("DataRate", DataRateValue (DataRate(2000000)));
csma.SetChannelAttribute ("Delay", TimeValue (MilliSeconds(1)));
csma.SetDeviceAttribute ("Mtu", UintegerValue (1400));
backbone1Devs = csma.Install(backbone1);
backbone2Devs = csma.Install(backbone2);


ipv6.SetBase (Ipv6Address ("2001:db80::"), Ipv6Prefix (64));
iifc = ipv6.Assign (backbone1Devs);
backbone1Ifs.Add(iifc);
ipv6.SetBase (Ipv6Address ("5001:db80::"), Ipv6Prefix (64));
iifc = ipv6.Assign (backbone2Devs);
backbone2Ifs.Add(iifc);


backbone1Ifs.SetForwarding(0,true);
backbone1Ifs.SetForwarding(1,true);
backbone1Ifs.SetForwarding(2,true);
backbone2Ifs.SetForwarding(0,true);
backbone2Ifs.SetForwarding(1,true);
backbone2Ifs.SetForwarding(2,true);
backbone1Ifs.SetDefaultRouteInAllNodes (0);
backbone1Ifs.SetDefaultRouteInAllNodes (1);
backbone1Ifs.SetDefaultRouteInAllNodes (2);
backbone2Ifs.SetDefaultRouteInAllNodes (0);
backbone2Ifs.SetDefaultRouteInAllNodes (1);
backbone2Ifs.SetDefaultRouteInAllNodes (2);


MobilityHelper mobility;
Ptr<ListPositionAllocator> positionAlloc;
positionAlloc = CreateObject<ListPositionAllocator> ();
positionAlloc->Add (Vector (0.0, -20.0, 0.0));  //MID
positionAlloc->Add (Vector (-50.0, 20.0, 0.0)); //AR1
positionAlloc->Add (Vector (50.0, 20.0, 0.0));  //AR2

mobility.SetPositionAllocator (positionAlloc);
mobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
mobility.Install (backbone1);

positionAlloc = CreateObject<ListPositionAllocator> ();
positionAlloc->Add (Vector (25.0, -20.0, 0.0));
mobility.SetPositionAllocator (positionAlloc);
mobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
mobility.Install (ha);

positionAlloc = CreateObject<ListPositionAllocator> ();
positionAlloc->Add (Vector (-25.0, -20.0, 0.0));
mobility.SetPositionAllocator (positionAlloc);
mobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
mobility.Install (cn);


Ssid ssid = Ssid("ns-3-ssid");
YansWifiPhyHelper wifiPhy = YansWifiPhyHelper::Default ();
wifiPhy.SetPcapDataLinkType (YansWifiPhyHelper::DLT_IEEE802_11_RADIO);

WifiHelper wifi = WifiHelper::Default ();
NqosWifiMacHelper wifiMac = NqosWifiMacHelper::Default ();
YansWifiChannelHelper wifiChannel = YansWifiChannelHelper::Default ();
wifiPhy.SetChannel (wifiChannel.Create ());
   
wifiMac.SetType ("ns3::ApWifiMac",
		           "Ssid", SsidValue (ssid),
		           "BeaconGeneration", BooleanValue (true),
		           "BeaconInterval", TimeValue (MilliSeconds(100)));

ar1Devs = wifi.Install (wifiPhy, wifiMac, ars.Get(0));
ar2Devs = wifi.Install (wifiPhy, wifiMac, ars.Get(1));
Ipv6AddressHelper ipv62;

ipv62.SetBase (Ipv6Address ("8888:56ac::"), Ipv6Prefix (64));
iifc = ipv62.Assign (ar1Devs);
ar1Ifs.Add(iifc);
ar1Ifs.SetForwarding (0, true);
ar1Ifs.SetDefaultRouteInAllNodes (0);
  

Ipv6AddressHelper ipv63;
ipv63.SetBase (Ipv6Address ("9999:db80::"), Ipv6Prefix (64));
iifc = ipv63.Assign (ar2Devs);
ar2Ifs.Add(iifc);
ar2Ifs.SetForwarding (0, true);
ar2Ifs.SetDefaultRouteInAllNodes (0);


positionAlloc = CreateObject<ListPositionAllocator> ();
positionAlloc->Add (Vector (-50.0, 50.0, 0.0)); //STA1
positionAlloc->Add (Vector (50.0, 50.0, 0.0)); //STA2
mobility.SetPositionAllocator (positionAlloc);
mobility.SetMobilityModel ("ns3::ConstantVelocityMobilityModel");  
mobility.Install(sta);

Ptr<ConstantVelocityMobilityModel> cvm = sta.Get(0)->GetObject<ConstantVelocityMobilityModel>();
cvm->SetVelocity(Vector (5, 0, 0)); //move left to right

cvm = sta.Get(1)->GetObject<ConstantVelocityMobilityModel>();
cvm->SetVelocity(Vector (-5, 0, 0)); //move right to left

wifiMac.SetType ("ns3::StaWifiMac",
	               "Ssid", SsidValue (ssid),
	               "ActiveProbing", BooleanValue (false));
staDevs.Add( wifi.Install (wifiPhy, wifiMac, sta));
iifc = ipv6.AssignWithoutAddress (staDevs);
staIfs.Add(iifc);
//staIfs.SetRouter(0, true);





Ipv6Address prefix ("8888:56ac::");  //create the prefix 

uint32_t indexRouter = ar1Ifs.GetInterfaceIndex (0);  //AR interface (n-AR1) 

Ptr<Radvd> radvd=CreateObject<Radvd> ();
Ptr<RadvdInterface> routerInterface= Create<RadvdInterface> (indexRouter, 1500, 50);
Ptr<RadvdPrefix> routerPrefix = Create<RadvdPrefix> (prefix, 64, 1.5, 2.0);

routerInterface->AddPrefix(routerPrefix);

radvd->AddConfiguration(routerInterface);

ars.Get(0)->AddApplication(radvd);
radvd->SetStartTime(Seconds (1.0));
radvd->SetStopTime(Seconds (100.0));



Ipv6Address prefix2 ("9999:db80::");  //create the prefix 


uint32_t indexRouter2 = ar2Ifs.GetInterfaceIndex (0);  //AR interface (R-n1) 
Ptr<Radvd> radvd2=CreateObject<Radvd> ();

Ptr<RadvdInterface> routerInterface2 = Create<RadvdInterface> (indexRouter2, 1500, 50);
Ptr<RadvdPrefix> routerPrefix2 = Create<RadvdPrefix> (prefix2, 64, 1.5, 2.0);

routerInterface2->AddPrefix(routerPrefix2);

radvd2->AddConfiguration(routerInterface2);

ars.Get(1)->AddApplication(radvd2);
radvd2->SetStartTime(Seconds (4.1));
radvd2->SetStopTime(Seconds (100.0));


Ipv6StaticRoutingHelper routingHelper;
Ptr<Ipv6> ipv692 = mid.Get(0)->GetObject<Ipv6> ();
Ptr<Ipv6StaticRouting> rttop = routingHelper.GetStaticRouting(ipv692);
rttop->AddNetworkRouteTo(Ipv6Address("8888:56ac::"),Ipv6Prefix(64),Ipv6Address("2001:db80::200:ff:fe00:2"),1,0);
rttop->AddNetworkRouteTo(Ipv6Address("9999:db80::"),Ipv6Prefix(64),Ipv6Address("2001:db80::200:ff:fe00:3"),1,0);
ipv692 = ars.Get(0)->GetObject<Ipv6> ();
rttop = routingHelper.GetStaticRouting(ipv692);
rttop->AddNetworkRouteTo(Ipv6Address("5001:db80::"),Ipv6Prefix(64),Ipv6Address("2001:db80::200:ff:fe00:1"),1,0);
ipv692 = ars.Get(1)->GetObject<Ipv6> ();
rttop = routingHelper.GetStaticRouting(ipv692);
rttop->AddNetworkRouteTo(Ipv6Address("5001:db80::"),Ipv6Prefix(64),Ipv6Address("2001:db80::200:ff:fe00:1"),1,0);


mipv6HAHelper hahelper;
hahelper.Install(ha.Get(0));
mipv6MNHelper mnhelper1(hahelper.GetHomeAgentAddressList(),false); 
mnhelper1.Install(sta.Get(0));
mnhelper1.Install(sta.Get(1));

//LogComponentEnable("BList", LOG_LEVEL_ALL);
LogComponentEnable("mipv6MN", LOG_LEVEL_ALL);
LogComponentEnable("mipv6HA", LOG_LEVEL_ALL);
//LogComponentEnable("Ipv6L3Protocol", LOG_LEVEL_ALL);
//LogComponentEnable("Ipv6TunnelL4Protocol", LOG_LEVEL_ALL);


UdpEchoServerHelper echoServer1 (9);
UdpEchoServerHelper echoServer2 (10);

  ApplicationContainer serverApps1 = echoServer1.Install (sta.Get (0));
  ApplicationContainer serverApps2 = echoServer2.Install (sta.Get (1));
  serverApps1.Start (Seconds (4.0));
  serverApps1.Stop (Seconds (700.0));

  serverApps2.Start (Seconds (5.0));
  serverApps2.Stop (Seconds (700.0));

  UdpEchoClientHelper echoClient1 (Ipv6Address("5001:db80::200:ff:fe00:9"), 9);
  UdpEchoClientHelper echoClient2 (Ipv6Address("5001:db80::200:ff:fe00:a"), 10);

  echoClient1.SetAttribute ("MaxPackets", UintegerValue (100));
  echoClient1.SetAttribute ("Interval", TimeValue (Seconds (1.)));
  echoClient1.SetAttribute ("PacketSize", UintegerValue (1024));

  echoClient2.SetAttribute ("MaxPackets", UintegerValue (100));
  echoClient2.SetAttribute ("Interval", TimeValue (Seconds (1.)));
  echoClient2.SetAttribute ("PacketSize", UintegerValue (1024));

  ApplicationContainer clientApps1 = echoClient1.Install (cn.Get (0));
  ApplicationContainer clientApps2 = echoClient2.Install (cn.Get (0));
  clientApps1.Start (Seconds (4.1));
  clientApps1.Stop (Seconds (700.0));  

  clientApps2.Start (Seconds (6.5));
  clientApps2.Stop (Seconds (700.0));

  LogComponentEnable("UdpEchoClientApplication", LOG_LEVEL_ALL);
  LogComponentEnable("UdpEchoServerApplication", LOG_LEVEL_ALL);

internet.EnablePcapIpv6 ("prefix1", sta.Get(0));
internet.EnablePcapIpv6 ("prefix2", ha.Get(0));
internet.EnablePcapIpv6 ("prefix3", cn.Get(0));
internet.EnablePcapIpv6 ("prefix4", sta.Get(1));


Ptr<OutputStreamWrapper> routingStream = Create<OutputStreamWrapper> (&std::cout);
routingHelper.PrintRoutingTableAt (Seconds (4.5), sta.Get(0), routingStream);
routingHelper.PrintRoutingTableAt (Seconds (1.9), sta.Get(0), routingStream);



  Simulator::Stop (Seconds (100.5));
  Simulator::Run ();
  Simulator::Destroy ();

  return 0;
}

