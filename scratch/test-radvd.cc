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
#include "ns3/internet-apps-module.h"
//#include "ns3/applications-module.h"
#include "ns3/mobility-module.h"
#include "ns3/wifi-module.h"
//#include "ns3/csma-module.h"
//#include "ns3/point-to-point-module.h"

#include "ns3/ipv6-static-routing.h"
#include "ns3/ipv6-routing-table-entry.h"

#include "ns3/internet-trace-helper.h"
#include "ns3/trace-helper.h"
#include "ns3/wimax-module.h"
#include <iostream>
#include <iomanip>
#include <fstream>
#include <vector>
#include <string>

NS_LOG_COMPONENT_DEFINE ("test-radvd");

using namespace ns3;

int main (int argc, char *argv[])
{
  
  NodeContainer ar;
  NodeContainer sta;
  
  
  NetDeviceContainer arDev;
  NetDeviceContainer staDev;
  
  Ipv6InterfaceContainer arIfs;
  Ipv6InterfaceContainer staIfs;
  Ipv6InterfaceContainer iifc;
  
  CommandLine cmd;
  cmd.Parse (argc, argv);
  
  ar.Create(1);
  sta.Create(1);

  InternetStackHelper internet;
  


  internet.Install (ar);
  internet.Install (sta);
   
  

  
  MobilityHelper mobility;
  Ptr<ListPositionAllocator> positionAlloc;
  


  positionAlloc = CreateObject<ListPositionAllocator> ();  
  positionAlloc->Add (Vector (100.0, 40.0, 0.0)); //AR
  mobility.SetPositionAllocator (positionAlloc);
  mobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
  mobility.Install (ar);



  positionAlloc = CreateObject<ListPositionAllocator> ();
  positionAlloc->Add (Vector (100.0, 120.0, 0.0));  //STA
  mobility.SetPositionAllocator (positionAlloc);
  mobility.SetMobilityModel ("ns3::ConstantPositionMobilityModel");
  mobility.Install (sta);
  


  //Setting wifi in AR
  
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

  

  arDev = wifi.Install (wifiPhy, wifiMac, ar.Get(0));




  Ipv6AddressHelper ipv62;
  ipv62.SetBase (Ipv6Address ("7777:56AC::"), Ipv6Prefix (64));
  iifc = ipv62.Assign (arDev);
  arIfs.Add(iifc);
  arIfs.SetForwarding (0, true);
  arIfs.SetDefaultRouteInAllNodes (0);
  
 Ipv6AddressHelper ipv6;

  //WLAN interface of STA
  wifiMac.SetType ("ns3::StaWifiMac",
	               "Ssid", SsidValue (ssid),
	               "ActiveProbing", BooleanValue (false));
  staDev.Add( wifi.Install (wifiPhy, wifiMac, sta));
  iifc = ipv6.AssignWithoutAddress (staDev);   
  staIfs.Add(iifc);
  




/*radvd configuration for AR */
Ipv6Address prefix ("7777:56AC::"); /* create the prefix */

uint32_t indexRouter = arIfs.GetInterfaceIndex (0); /* AR interface (n-AR1) */

Ptr<Radvd> radvd=CreateObject<Radvd> ();
Ptr<RadvdInterface> routerInterface= Create<RadvdInterface> (indexRouter, 7000, 5000);
Ptr<RadvdPrefix> routerPrefix = Create<RadvdPrefix> (prefix, 64,8,10);

routerInterface->AddPrefix(routerPrefix);

radvd->AddConfiguration(routerInterface);

ar.Get(0)->AddApplication(radvd);
radvd->SetStartTime(Seconds (0.0));
radvd->SetStopTime(Seconds (1000.0));

//  Ptr<Icmpv6L4Protocol> icmp = (ar.Get(0))->GetObject<Icmpv6L4Protocol> ();
//  icmp->SetRadvd(radvd);



  internet.EnablePcapIpv6 ("ar", ar.Get(0));
  internet.EnablePcapIpv6 ("sta", sta.Get(0));


  LogComponentEnable("Icmpv6L4Protocol", LOG_LEVEL_ALL);
 

  Simulator::Stop (Seconds (200.0));
  Simulator::Run ();
  Simulator::Destroy ();

  return 0;
}



