#include "ns3/log.h"
#include "ns3/packet.h"
#include "ns3/ipv6-route.h"
#include "ns3/net-device.h"

#include "ns3/ipv6-routing-table-entry.h"

#include "ipv6-static-source-routing.h"

namespace ns3
{

NS_LOG_COMPONENT_DEFINE ("Ipv6StaticSourceRouting");
NS_OBJECT_ENSURE_REGISTERED (Ipv6StaticSourceRouting);

TypeId Ipv6StaticSourceRouting::GetTypeId ()
{
  static TypeId tid = TypeId ("ns3::Ipv6StaticSourceRouting")
    .SetParent<Ipv6RoutingProtocol> ()
    .AddConstructor<Ipv6StaticSourceRouting> ()
    ;
  return tid;
}

Ipv6StaticSourceRouting::Ipv6StaticSourceRouting ()
  : m_ipv6 (0)
{
  NS_LOG_FUNCTION_NOARGS ();
}

Ipv6StaticSourceRouting::~Ipv6StaticSourceRouting ()
{
  NS_LOG_FUNCTION_NOARGS ();
}

void Ipv6StaticSourceRouting::SetIpv6 (Ptr<Ipv6> ipv6)
{
  NS_LOG_FUNCTION (this << ipv6);
  NS_ASSERT (m_ipv6 == 0 && ipv6 != 0);
  uint32_t i = 0; 
  m_ipv6 = ipv6;

  for (i = 0 ; i < m_ipv6->GetNInterfaces () ; i++)
    {
      if (m_ipv6->IsUp (i))
        {
          NotifyInterfaceUp (i);
        }
      else
        {
          NotifyInterfaceDown (i);
        }
    }
}

void Ipv6StaticSourceRouting::AddHostRouteFrom (Ipv6Address src, Ipv6Address nextHop, uint32_t interface, Ipv6Address prefixToUse, uint32_t metric)
{
  NS_LOG_FUNCTION (this << src << nextHop << interface << prefixToUse << metric);
  AddNetworkRouteFrom (src, Ipv6Prefix::GetOnes (), nextHop, interface, prefixToUse, metric);
}

void Ipv6StaticSourceRouting::AddHostRouteFrom (Ipv6Address src, uint32_t interface, uint32_t metric)
{
  NS_LOG_FUNCTION (this << src << interface << metric);
  AddNetworkRouteFrom (src, Ipv6Prefix::GetOnes (), interface, metric);
}

void Ipv6StaticSourceRouting::AddNetworkRouteFrom (Ipv6Address network, Ipv6Prefix networkPrefix, Ipv6Address nextHop, uint32_t interface, uint32_t metric)
{
  NS_LOG_FUNCTION (this << network << networkPrefix << nextHop << interface << metric);
  Ipv6RoutingTableEntry* route = new Ipv6RoutingTableEntry ();
  *route = Ipv6RoutingTableEntry::CreateNetworkRouteTo (network, networkPrefix, nextHop, interface);
  m_networkRoutes.push_back (std::make_pair (route, metric));
}

void Ipv6StaticSourceRouting::AddNetworkRouteFrom (Ipv6Address network, Ipv6Prefix networkPrefix, Ipv6Address nextHop, uint32_t interface, Ipv6Address prefixToUse, uint32_t metric)
{
  NS_LOG_FUNCTION (this << network << networkPrefix << nextHop << interface << prefixToUse << metric);
  Ipv6RoutingTableEntry* route = new Ipv6RoutingTableEntry ();
  *route = Ipv6RoutingTableEntry::CreateNetworkRouteTo (network, networkPrefix, nextHop, interface, prefixToUse);
  m_networkRoutes.push_back (std::make_pair (route, metric));
}

void Ipv6StaticSourceRouting::AddNetworkRouteFrom (Ipv6Address network, Ipv6Prefix networkPrefix, uint32_t interface, uint32_t metric)
{
  NS_LOG_FUNCTION (this << network << networkPrefix << interface);
  Ipv6RoutingTableEntry* route = new Ipv6RoutingTableEntry ();
  *route = Ipv6RoutingTableEntry::CreateNetworkRouteTo (network, networkPrefix, interface);
  m_networkRoutes.push_back (std::make_pair (route, metric));
}

Ptr<Ipv6Route> Ipv6StaticSourceRouting::LookupStatic (Ipv6Address src, Ipv6Address dst)
{
  NS_LOG_FUNCTION (this << src << dst);
  Ptr<Ipv6Route> rtentry = 0;
  uint16_t longestMask = 0;
  uint32_t shortestMetric = 0xffffffff;

  /* when sending on link-local multicast, there have to be interface specified */
  if (src == Ipv6Address::GetAllNodesMulticast () || src.IsSolicitedMulticast () || 
      src == Ipv6Address::GetAllRoutersMulticast () || src == Ipv6Address::GetAllHostsMulticast ())
    {
      return 0;
    }

  for (NetworkRoutesI it = m_networkRoutes.begin () ; it != m_networkRoutes.end () ; it++)
    {
      Ipv6RoutingTableEntry* j = it->first;
      uint32_t metric = it->second;
      Ipv6Prefix mask = j->GetDestNetworkPrefix ();
      uint16_t maskLen = mask.GetPrefixLength ();
      Ipv6Address entry = j->GetDestNetwork ();

      NS_LOG_LOGIC ("Searching for route from " << src << ", mask length " << maskLen << ", metric " << metric);

      if (mask.IsMatch (src, entry))
        {
          NS_LOG_LOGIC ("Found global network route " << j << ", mask length " << maskLen << ", metric " << metric);

		  if (maskLen < longestMask)
			{
			  NS_LOG_LOGIC ("Previous match longer, skipping");
			  continue;
			}

		  if (maskLen > longestMask)
			{
			  shortestMetric = 0xffffffff;
			}

		  longestMask = maskLen;
		  if (metric > shortestMetric)
			{
			  NS_LOG_LOGIC ("Equal mask length, but previous metric shorter, skipping");
			  continue;
			}

		  shortestMetric = metric;
		  Ipv6RoutingTableEntry* route = j;
		  uint32_t interfaceIdx = route->GetInterface ();
		  rtentry = Create<Ipv6Route> ();

		  rtentry->SetSource (route->GetDest());
		  rtentry->SetDestination (dst);
		  rtentry->SetGateway (route->GetGateway ());
		  rtentry->SetOutputDevice (m_ipv6->GetNetDevice (interfaceIdx));
        }
    }

  if(rtentry)
    {
      NS_LOG_LOGIC ("Matching route via " << rtentry->GetDestination () << " (throught " << rtentry->GetGateway () << ") at the end");
    }
  return rtentry;
}

void Ipv6StaticSourceRouting::DoDispose ()
{
  NS_LOG_FUNCTION_NOARGS ();

  for (NetworkRoutesI j = m_networkRoutes.begin () ;  j != m_networkRoutes.end () ; j = m_networkRoutes.erase (j))
    {
      delete j->first;
    }
  m_networkRoutes.clear ();

  m_ipv6 = 0;
  Ipv6RoutingProtocol::DoDispose ();
}

uint32_t Ipv6StaticSourceRouting::GetNRoutes ()
{
  return m_networkRoutes.size ();
}

Ipv6RoutingTableEntry Ipv6StaticSourceRouting::GetRoute (uint32_t index)
{
  NS_LOG_FUNCTION (this << index);
  uint32_t tmp = 0;

  for (NetworkRoutesI it = m_networkRoutes.begin () ; it != m_networkRoutes.end () ; it++)
    {
      if (tmp == index)
        {
          return it->first;
        }
      tmp++;
    }
  NS_ASSERT (false);
  // quiet compiler.
  return 0;
}

uint32_t Ipv6StaticSourceRouting::GetMetric (uint32_t index)
{
  NS_LOG_FUNCTION_NOARGS ();
  uint32_t tmp = 0;

  for (NetworkRoutesI it = m_networkRoutes.begin () ; it != m_networkRoutes.end () ; it++)
    {
      if (tmp == index)
        {
          return it->second;
        }
      tmp++;
    }
  NS_ASSERT (false);
  // quiet compiler.
  return 0;
}

void Ipv6StaticSourceRouting::RemoveRoute (uint32_t index)
{
  NS_LOG_FUNCTION (this << index);
  uint32_t tmp = 0;

  for (NetworkRoutesI it = m_networkRoutes.begin () ; it != m_networkRoutes.end () ; it++)
    {
      if (tmp == index)
        {
          delete it->first;
          m_networkRoutes.erase (it);
          return;
        }
      tmp++;
    }
  NS_ASSERT (false);
}

void Ipv6StaticSourceRouting::RemoveRoute (Ipv6Address network, Ipv6Prefix prefix, uint32_t ifIndex, Ipv6Address prefixToUse)
{
  NS_LOG_FUNCTION (this << network << prefix << ifIndex);

  for (NetworkRoutesI it = m_networkRoutes.begin () ; it != m_networkRoutes.end () ; it++)
    {
      Ipv6RoutingTableEntry* rtentry = it->first;
      if (network == rtentry->GetDest () && rtentry->GetInterface () == ifIndex && 
          rtentry->GetPrefixToUse () == prefixToUse)
        {
          delete it->first;
          m_networkRoutes.erase (it);
          return;
        }
    }
}

Ptr<Ipv6Route> Ipv6StaticSourceRouting::RouteOutput (Ptr<Packet> p, const Ipv6Header &header, Ptr<NetDevice> oif, Socket::SocketErrno &sockerr)
{
  NS_LOG_FUNCTION (this << header << oif);
  Ipv6Address src = header.GetSourceAddress ();
  Ipv6Address dst = header.GetDestinationAddress ();
  Ptr<Ipv6Route> rtentry = 0;
  
  if( src.IsAny() )
    {
	  NS_LOG_LOGIC("Source Address is ZERO, Nothing related with source routing");
	  sockerr = Socket::ERROR_NOTERROR;
	  return 0;
	}

  rtentry = LookupStatic (src, dst);
  if (rtentry)
    {
      sockerr = Socket::ERROR_NOTERROR;
    }
  else
    {
      sockerr = Socket::ERROR_NOROUTETOHOST;
    }
  return rtentry;
}

bool Ipv6StaticSourceRouting::RouteInput (Ptr<const Packet> p, const Ipv6Header &header, Ptr<const NetDevice> idev,
                                    UnicastForwardCallback ucb, MulticastForwardCallback mcb,
                                    LocalDeliverCallback lcb, ErrorCallback ecb)
{
  NS_LOG_FUNCTION (this << p << header << header.GetSourceAddress () << header.GetDestinationAddress () << idev);
  NS_ASSERT (m_ipv6 != 0);
  // Check if input device supports IP
  NS_ASSERT (m_ipv6->GetInterfaceForDevice (idev) >= 0);
  uint32_t iif = m_ipv6->GetInterfaceForDevice (idev);
  Ipv6Address src = header.GetSourceAddress();
  Ipv6Address dst = header.GetDestinationAddress ();

  // TODO:  Configurable option to enable RFC 1222 Strong End System Model
  // Right now, we will be permissive and allow a source to send us
  // a packet to one of our other interface addresses; that is, the
  // destination unicast address does not match one of the iif addresses,
  // but we check our other interfaces.  This could be an option
  // (to remove the outer loop immediately below and just check iif).
  for (uint32_t j = 0; j < m_ipv6->GetNInterfaces (); j++)
    {
      for (uint32_t i = 0; i < m_ipv6->GetNAddresses (j); i++)
        {
          Ipv6InterfaceAddress iaddr = m_ipv6->GetAddress (j, i);
          Ipv6Address addr = iaddr.GetAddress ();
          if (addr.IsEqual (dst))
            {
              if (j == iif)
                {
                  NS_LOG_LOGIC ("For me (destination " << addr << " match)");
                }
              else
                {
                  NS_LOG_LOGIC ("For me (destination " << addr << " match) on another interface " << dst );
                }
              lcb (p, header, iif);
              return true;
            }
          NS_LOG_LOGIC ("Address "<< addr << " not a match");
        }
    }
  // Check if input device supports IP forwarding
  if (m_ipv6->IsForwarding (iif) == false)
    {
      NS_LOG_LOGIC ("Forwarding disabled for this interface");
      ecb (p, header, Socket::ERROR_NOROUTETOHOST);
      return false;
    }
  // Next, try to find a route
  Ptr<Ipv6Route> rtentry = LookupStatic (src, dst);

  if (rtentry != 0)
    {
      NS_LOG_LOGIC ("Found unicast destination- calling unicast callback");
      ucb (idev,rtentry, p, header);  // unicast forwarding callback
      return true;
    }
  else
    {
      NS_LOG_LOGIC ("Did not find unicast destination- returning false");
      return false; // Let other routing protocols try to handle this
    }
}

void Ipv6StaticSourceRouting::NotifyInterfaceUp (uint32_t i)
{
  NS_LOG_FUNCTION (this << i);
}

void Ipv6StaticSourceRouting::NotifyInterfaceDown (uint32_t i)
{
  NS_LOG_FUNCTION (this << i);
  uint32_t j = 0;
  uint32_t max = GetNRoutes ();

  /* remove all static routes that are going through this interface */
  while (j < max)
    {
      Ipv6RoutingTableEntry route = GetRoute (j);

      if (route.GetInterface () == i)
        {
          RemoveRoute (j);
        }
      else
        {
          j++;
        }
    }
}

void Ipv6StaticSourceRouting::NotifyAddAddress (uint32_t interface, Ipv6InterfaceAddress address)
{
}

void Ipv6StaticSourceRouting::NotifyRemoveAddress (uint32_t interface, Ipv6InterfaceAddress address)
{
}

void Ipv6StaticSourceRouting::NotifyAddRoute (Ipv6Address dst, Ipv6Prefix mask, Ipv6Address nextHop, uint32_t interface, Ipv6Address prefixToUse)
{
  NS_LOG_INFO (this << dst << mask << nextHop << interface << prefixToUse);
}

void Ipv6StaticSourceRouting::NotifyRemoveRoute (Ipv6Address dst, Ipv6Prefix mask, Ipv6Address nextHop, uint32_t interface, Ipv6Address prefixToUse)
{
  NS_LOG_FUNCTION (this << dst << mask << nextHop << interface);
}

void
Ipv6StaticSourceRouting::PrintRoutingTable (Ptr<OutputStreamWrapper> stream) const
{
  NS_LOG_FUNCTION (this<< stream);
}

} /* namespace ns3 */

