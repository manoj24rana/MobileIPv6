#include <vector>

#include "ns3/log.h"
#include "ns3/ptr.h"
#include "ns3/names.h"
#include "ns3/node.h"
#include "ns3/ipv6.h"
#include "ns3/ipv6-route.h"
#include "ns3/ipv6-list-routing.h"
#include "ns3/assert.h"
#include "ns3/ipv6-address.h"
#include "ns3/ipv6-routing-protocol.h"
#include "ipv6-static-source-routing-helper.h"

NS_LOG_COMPONENT_DEFINE ("Ipv6StaticSourceRoutingHelper");

namespace ns3 {

Ipv6StaticSourceRoutingHelper::Ipv6StaticSourceRoutingHelper ()
{}

Ipv6StaticSourceRoutingHelper::Ipv6StaticSourceRoutingHelper (const Ipv6StaticSourceRoutingHelper &o)
{
}

Ipv6StaticSourceRoutingHelper* 
Ipv6StaticSourceRoutingHelper::Copy (void) const 
{
  return new Ipv6StaticSourceRoutingHelper (*this); 
}

Ptr<Ipv6RoutingProtocol> 
Ipv6StaticSourceRoutingHelper::Create (Ptr<Node> node) const
{
  return CreateObject<Ipv6StaticSourceRouting> ();
}

Ptr<Ipv6StaticSourceRouting>
Ipv6StaticSourceRoutingHelper::GetStaticSourceRouting (Ptr<Ipv6> ipv6) const
{
  NS_LOG_FUNCTION (this);
  Ptr<Ipv6RoutingProtocol> ipv6rp = ipv6->GetRoutingProtocol ();
  NS_ASSERT_MSG (ipv6rp, "No routing protocol associated with Ipv6");
  if (DynamicCast<Ipv6StaticSourceRouting> (ipv6rp))
    {
      NS_ASSERT_MSG (false, "Static source routing cannot exist main routing protocol");
      return 0;
    } 
	
  if (DynamicCast<Ipv6ListRouting> (ipv6rp))
    {
      Ptr<Ipv6ListRouting> lrp = DynamicCast<Ipv6ListRouting> (ipv6rp);
      int16_t priority;
      for (uint32_t i = 0; i < lrp->GetNRoutingProtocols ();  i++)
        {
          NS_LOG_LOGIC ("Searching for static routing in list");
          Ptr<Ipv6RoutingProtocol> temp = lrp->GetRoutingProtocol (i, priority);
          if (DynamicCast<Ipv6StaticSourceRouting> (temp))
            {
              NS_LOG_LOGIC ("Found static routing in list");
              return DynamicCast<Ipv6StaticSourceRouting> (temp);
            }
        }
    }
  NS_LOG_LOGIC ("Static routing not found");
  return 0;
}

} // namespace ns3
