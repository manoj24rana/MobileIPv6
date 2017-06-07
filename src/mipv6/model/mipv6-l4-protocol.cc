#include <stdio.h>
#include <sstream>
#include <string.h>

#include "ns3/log.h"
#include "ns3/assert.h"
#include "ns3/packet.h"
#include "ns3/node.h"
#include "ns3/boolean.h"
#include "ns3/ipv6-routing-protocol.h"
#include "ns3/ipv6-route.h"
#include "ns3/wifi-net-device.h"
#include "ns3/wifi-mac.h"
#include "ns3/ipv6-l3-protocol.h"
#include "ns3/ipv6-interface.h"
#include "ns3/object-base.h"
#include "ns3/header.h"
#include "ns3/chunk.h"
#include "ns3/type-id.h"

#include "identifier.h"
#include "mipv6-mobility.h"
#include "mipv6-option.h"
#include "mipv6-header.h"
#include "mipv6-demux.h"
#include "mipv6-option-demux.h"
#include "mipv6-l4-protocol.h"
#include "ns3/ip-l4-protocol.h"

using namespace std;

NS_LOG_COMPONENT_DEFINE ("MIPv6L4Protocol");

namespace ns3
{

NS_OBJECT_ENSURE_REGISTERED (MIPv6L4Protocol);

const uint8_t MIPv6L4Protocol::PROT_NUMBER = 135;

const double MIPv6L4Protocol::MAX_BINDING_LIFETIME = (int)0xffff<<2;

const double MIPv6L4Protocol::INITIAL_BINDING_ACK_TIMEOUT_FIRSTREG = 1.5;

const double MIPv6L4Protocol::INITIAL_BINDING_ACK_TIMEOUT_REREG = 1.0;

const uint8_t MIPv6L4Protocol::MAX_BINDING_UPDATE_RETRY_COUNT = 3;

const uint8_t MIPv6L4Protocol::MAX_HOTI_RETRY_COUNT = 3;

const uint8_t MIPv6L4Protocol::MAX_COTI_RETRY_COUNT = 3;

const uint32_t MIPv6L4Protocol::MIN_DELAY_BEFORE_BCE_DELETE = 10000;

const uint32_t MIPv6L4Protocol::MIN_DELAY_BEFORE_NEW_BCE_ASSIGN = 1500;

const uint32_t MIPv6L4Protocol::TIMESTAMP_VALIDITY_WINDOW = 300;

TypeId MIPv6L4Protocol::GetTypeId ()
{
  static TypeId tid = TypeId ("ns3::MIPv6L4Protocol")
    .SetParent<IpL4Protocol> ()
    .AddConstructor<MIPv6L4Protocol> ();
  return tid;
}

MIPv6L4Protocol::MIPv6L4Protocol ()
  : m_node (0)
{
  NS_LOG_FUNCTION_NOARGS ();
}

MIPv6L4Protocol::~MIPv6L4Protocol ()
{
  NS_LOG_FUNCTION_NOARGS ();
}

void MIPv6L4Protocol::DoDispose ()
{
  NS_LOG_FUNCTION_NOARGS ();

  m_node = 0;
  IpL4Protocol::DoDispose ();
}

void MIPv6L4Protocol::NotifyNewAggregate ()
{
  NS_LOG_FUNCTION_NOARGS ();

  if (m_node == 0)
    {
      Ptr<Node> node = this->GetObject<Node> ();
      if (node != 0)
        {
          Ptr<Ipv6L3Protocol> ipv6 = this->GetObject<Ipv6L3Protocol> ();
          if (ipv6 != 0)
            {
              this->SetNode (node);
              ipv6->Insert (this);
            }
        }
    }
  IpL4Protocol::NotifyNewAggregate ();
}

void MIPv6L4Protocol::SetNode (Ptr<Node> node)
{
  NS_LOG_FUNCTION (this << node);
  m_node = node;
}

Ptr<Node> MIPv6L4Protocol::GetNode (void)
{
  NS_LOG_FUNCTION_NOARGS();
  return m_node;
}

int MIPv6L4Protocol::GetProtocolNumber () const
{
  //NS_LOG_FUNCTION_NOARGS ();
  return PROT_NUMBER;
}

int MIPv6L4Protocol::GetVersion () const
{
  NS_LOG_FUNCTION_NOARGS ();
  return 1;
}

void MIPv6L4Protocol::SendMessage (Ptr<Packet> packet, Ipv6Address src, Ipv6Address dst, uint8_t ttl)
{
  NS_LOG_FUNCTION (this << packet << src << dst << (uint32_t)ttl);
  Ptr<Ipv6L3Protocol> ipv6 = m_node->GetObject<Ipv6L3Protocol> ();
  SocketIpTtlTag tag;
  NS_ASSERT (ipv6 != 0);

  tag.SetTtl (ttl);
  packet->AddPacketTag (tag);
  ipv6->Send (packet, src, dst, PROT_NUMBER, 0);
}

enum IpL4Protocol::RxStatus MIPv6L4Protocol::Receive (Ptr<Packet> packet, Ipv6Address const &src, Ipv6Address const &dst, Ptr<Ipv6Interface> interface)
{
  NS_LOG_FUNCTION (this << packet << src << dst << interface<<"VANCH");
  Ptr<Packet> p = packet->Copy ();
  Ptr<MIPv6Demux> ipv6MobilityDemux = GetObject<MIPv6Demux>();
  Ptr<MIPv6Mobility> ipv6Mobility = 0;
  MIPv6Header mh;
  
  p->PeekHeader (mh);
  
  ipv6Mobility = ipv6MobilityDemux -> GetMobility ( mh.GetMhType() );
  
  if(ipv6Mobility)
    {
	  ipv6Mobility -> Process (p, src, dst, interface);
	}
  else
    {
	  NS_LOG_FUNCTION( "Mobility Packet with Unknown MhType (" << (uint32_t)mh.GetMhType() << ")" );
	}

  return IpL4Protocol::RX_OK;
}

enum IpL4Protocol::RxStatus MIPv6L4Protocol::Receive(Ptr<Packet> p, Ipv6Header const &header, Ptr<Ipv6Interface> incomingInterface)
{

  NS_LOG_FUNCTION (this << p << header << incomingInterface<<"VAMCH");
  Ptr<Packet> packet = p->Copy ();
  Ptr<MIPv6Demux> ipv6MobilityDemux = GetObject<MIPv6Demux>();
  Ptr<MIPv6Mobility> ipv6Mobility = 0;
  MIPv6Header mh;
  
  packet->PeekHeader (mh);
  ipv6Mobility = ipv6MobilityDemux -> GetMobility ( mh.GetMhType() );
 if(ipv6Mobility)
    {
Ipv6Address src=header.GetSourceAddress ();
Ipv6Address dst=header.GetDestinationAddress ();	  

ipv6Mobility -> Process (packet, src, dst, incomingInterface);
	}
  else
    {
	  NS_LOG_FUNCTION( "Mobility Packet with Unknown MhType (" << (uint32_t)mh.GetMhType() << ")" );
	}

  return IpL4Protocol::RX_OK; 

}

enum IpL4Protocol::RxStatus MIPv6L4Protocol::Receive(Ptr<Packet> p, Ipv4Header const &header, Ptr<Ipv4Interface> incomingInterface)
{


  return IpL4Protocol::RX_OK; 

}


void MIPv6L4Protocol::RegisterMobility()
{
  Ptr<MIPv6Demux> ipv6MobilityDemux = CreateObject<MIPv6Demux>();
  ipv6MobilityDemux -> SetNode( m_node );
  
  m_node -> AggregateObject( ipv6MobilityDemux );
  
  Ptr<Ipv6MobilityBindingUpdate> bu = CreateObject<Ipv6MobilityBindingUpdate>();
  bu->SetNode(m_node);
  ipv6MobilityDemux->Insert(bu);
  
  Ptr<Ipv6MobilityBindingAck> ba = CreateObject<Ipv6MobilityBindingAck>();
  ba->SetNode(m_node);
  ipv6MobilityDemux->Insert(ba);

  Ptr<Ipv6MobilityHoTI> hoti = CreateObject<Ipv6MobilityHoTI>();
  hoti->SetNode(m_node);
  ipv6MobilityDemux->Insert(hoti);
  
  Ptr<Ipv6MobilityCoTI> coti = CreateObject<Ipv6MobilityCoTI>();
  coti->SetNode(m_node);
  ipv6MobilityDemux->Insert(coti);
  
  Ptr<Ipv6MobilityHoT> hot = CreateObject<Ipv6MobilityHoT>();
  hot->SetNode(m_node);
  ipv6MobilityDemux->Insert(hot);
  
  Ptr<Ipv6MobilityCoT> cot = CreateObject<Ipv6MobilityCoT>();
  cot->SetNode(m_node);
  ipv6MobilityDemux->Insert(cot);
    
}
void MIPv6L4Protocol::SetDownTarget (IpL4Protocol::DownTargetCallback cb){}
void MIPv6L4Protocol::SetDownTarget6 (IpL4Protocol::DownTargetCallback6 cb){}
IpL4Protocol::DownTargetCallback MIPv6L4Protocol::GetDownTarget (void) const
{IpL4Protocol::DownTargetCallback t;return t;}
IpL4Protocol::DownTargetCallback6 MIPv6L4Protocol::GetDownTarget6 (void) const
{IpL4Protocol::DownTargetCallback6 y;return y;}

void MIPv6L4Protocol::RegisterMobilityOptions()
{
  Ptr<MIPv6OptionDemux> ipv6MobilityOptionDemux = CreateObject<MIPv6OptionDemux>();
  ipv6MobilityOptionDemux -> SetNode( m_node );
  
  m_node -> AggregateObject( ipv6MobilityOptionDemux );
  
  Ptr<Ipv6MobilityOptionPad1> pad1 = CreateObject<Ipv6MobilityOptionPad1>();
  pad1->SetNode(m_node);
  ipv6MobilityOptionDemux->Insert(pad1);
  
  Ptr<Ipv6MobilityOptionPadn> padn = CreateObject<Ipv6MobilityOptionPadn>();
  padn->SetNode(m_node);
  ipv6MobilityOptionDemux->Insert(padn);
  
  //for MIPv6
  Ptr<Ipv6MobilityOptionBindingRefreshAdvice> adv = CreateObject<Ipv6MobilityOptionBindingRefreshAdvice>();
  adv->SetNode(m_node);
  ipv6MobilityOptionDemux->Insert(adv);
  
  Ptr<Ipv6MobilityOptionAlternateCareofAddress> acoa = CreateObject<Ipv6MobilityOptionAlternateCareofAddress>();
  acoa->SetNode(m_node);
  ipv6MobilityOptionDemux->Insert(acoa);
  
  Ptr<Ipv6MobilityOptionNonceIndices> ni = CreateObject<Ipv6MobilityOptionNonceIndices>();
  ni->SetNode(m_node);
  ipv6MobilityOptionDemux->Insert(ni);
  
  Ptr<Ipv6MobilityOptionBindingAuthorizationData> auth = CreateObject<Ipv6MobilityOptionBindingAuthorizationData>();
  auth->SetNode(m_node);
  ipv6MobilityOptionDemux->Insert(auth);
  
}

} /* namespace ns3 */

