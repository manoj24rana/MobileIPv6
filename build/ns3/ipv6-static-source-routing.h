#ifndef IPV6_STATIC_SOURCE_ROUTING_H
#define IPV6_STATIC_SOURCE_ROUTING_H

#include <stdint.h>

#include <list>

#include "ns3/ptr.h"
#include "ns3/ipv6-address.h"
#include "ns3/ipv6.h"
#include "ns3/ipv6-header.h"
#include "ns3/ipv6-routing-protocol.h"

namespace ns3
{

class Packet;
class NetDevice;
class Ipv6Interface;
class Ipv6Route;
class Node;
class Ipv6RoutingTableEntry;
class Ipv6MulticastRoutingTableEntry;

/**
 * \ingroup routing
 * \defgroup Ipv6StaticSourceRouting Ipv6StaticSourceRouting
 */
/**
 * \ingroup Ipv6StaticSourceRouting
 * \class Ipv6StaticSourceRouting
 * \brief Static routing protocol for IP version 6 stack.
 * \see Ipv6RoutingProtocol
 * \see Ipv6ListRouting
 */
class Ipv6StaticSourceRouting : public Ipv6RoutingProtocol
{
public:
  /**
   * \brief The interface Id associated with this class.
   * \return type identifier
   */
  static TypeId GetTypeId ();

  /**
   * \brief Constructor.
   */
  Ipv6StaticSourceRouting ();

  /**
   * \brief Destructor.
   */
  virtual ~Ipv6StaticSourceRouting ();

  /**
   * \brief Add route to host.
   * \param dest destination address
   * \param nextHop next hop address to route the packet
   * \param interface interface index
   * \param prefixToUse prefix that should be used for source address for this destination
   * \param metric metric of route in case of multiple routes to same destination
   */
  void AddHostRouteFrom (Ipv6Address src, Ipv6Address nextHop, uint32_t interface, Ipv6Address prefixToUse = Ipv6Address ("::"), uint32_t metric = 0);

  /**
   * \brief Add route to host.
   * \param dest destination address.
   * \param interface interface index
   * \param metric metric of route in case of multiple routes to same destination
   */
  void AddHostRouteFrom (Ipv6Address src, uint32_t interface, uint32_t metric = 0);

  /**
   * \brief Add route to network.
   * \param network network address
   * \param networkPrefix network prefix*
   * \param nextHop next hop address to route the packet
   * \param interface interface index
   * \param metric metric of route in case of multiple routes to same destination
   */
  void AddNetworkRouteFrom (Ipv6Address network, Ipv6Prefix networkPrefix, Ipv6Address nextHop, uint32_t interface, uint32_t metric = 0);

  /**
   * \brief Add route to network.
   * \param network network address
   * \param networkPrefix network prefix*
   * \param nextHop next hop address to route the packet
   * \param interface interface index
   * \param prefixToUse prefix that should be used for source address for this destination
   * \param metric metric of route in case of multiple routes to same destination
   */
  void AddNetworkRouteFrom (Ipv6Address network, Ipv6Prefix networkPrefix, Ipv6Address nextHop, uint32_t interface, Ipv6Address prefixToUse, uint32_t metric = 0);

  /**
   * \brief Add route to network.
   * \param network network address
   * \param networkPrefix network prefix
   * \param interface interface index
   * \param metric metric of route in case of multiple routes to same destination
   */
  void AddNetworkRouteFrom (Ipv6Address network, Ipv6Prefix networkPrefix, uint32_t interface, uint32_t metric = 0);

  /**
   * \brief Get the number or entries in the routing table.
   * \return number of entries
   */
  uint32_t GetNRoutes ();

  /**
   * \brief Get a specified route.
   * \param i index
   * \return the route whose index is i
   */
  Ipv6RoutingTableEntry GetRoute (uint32_t i);

  /**
   * \brief Get a metric for route from the static unicast routing table.
   * \param index The index (into the routing table) of the route to retrieve.  
   * \return If route is set, the metric is returned. If not, an infinity metric (0xffffffff) is returned
   */
  uint32_t GetMetric (uint32_t index);

  /**
   * \brief Remove a route from the routing table.
   * \param i index
   */
  void RemoveRoute (uint32_t i);

  /**
   * \brief Remove a route from the routing table.
   * \param network IPv6 network
   * \param prefix IPv6 prefix
   * \param ifIndex interface index
   * \param prefixToUse IPv6 prefix to use with this route (multihoming)
   */
  void RemoveRoute (Ipv6Address network, Ipv6Prefix prefix, uint32_t ifIndex, Ipv6Address prefixToUse);

  virtual Ptr<Ipv6Route> RouteOutput (Ptr<Packet> p, const Ipv6Header &header, Ptr<NetDevice> oif, Socket::SocketErrno &sockerr);

  virtual bool RouteInput  (Ptr<const Packet> p, const Ipv6Header &header, Ptr<const NetDevice> idev,
                            UnicastForwardCallback ucb, MulticastForwardCallback mcb,
                            LocalDeliverCallback lcb, ErrorCallback ecb);

  virtual void NotifyInterfaceUp (uint32_t interface);
  virtual void NotifyInterfaceDown (uint32_t interface);
  virtual void NotifyAddAddress (uint32_t interface, Ipv6InterfaceAddress address);
  virtual void NotifyRemoveAddress (uint32_t interface, Ipv6InterfaceAddress address);
  virtual void NotifyAddRoute (Ipv6Address dst, Ipv6Prefix mask, Ipv6Address nextHop, uint32_t interface, Ipv6Address prefixToUse = Ipv6Address::GetZero ());
  virtual void NotifyRemoveRoute (Ipv6Address dst, Ipv6Prefix mask, Ipv6Address nextHop, uint32_t interface, Ipv6Address prefixToUse = Ipv6Address::GetZero ());
  virtual void SetIpv6 (Ptr<Ipv6> ipv6);
    virtual void PrintRoutingTable (Ptr<OutputStreamWrapper> stream) const;

protected:
  /**
   * \brief Dispose this object.
   */
  void DoDispose ();

private:
  typedef std::list<std::pair <Ipv6RoutingTableEntry *, uint32_t> > NetworkRoutes;
  typedef std::list<std::pair <Ipv6RoutingTableEntry *, uint32_t> >::const_iterator NetworkRoutesCI;
  typedef std::list<std::pair <Ipv6RoutingTableEntry *, uint32_t> >::iterator NetworkRoutesI;

  /**
   * \brief Lookup in the forwarding table for destination.
   * \param dest destination address
   * \param interface output interface if any (put 0 otherwise)
   * \return Ipv6Route to route the packet to reach dest address
   */
  Ptr<Ipv6Route> LookupStatic (Ipv6Address src, Ipv6Address dst);


  /**
   * \brief the forwarding table for network.
   */
  NetworkRoutes m_networkRoutes;

  /**
   * \brief Ipv6 reference.
   */
  Ptr<Ipv6> m_ipv6;
};

} /* namespace ns3 */

#endif /* IPV6_STATIC_SOURCE_ROUTING_H */

