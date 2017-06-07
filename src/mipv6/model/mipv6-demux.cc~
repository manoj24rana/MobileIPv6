#include "ns3/assert.h"
#include "ns3/address-utils.h"
#include "ns3/log.h"
#include "ns3/object-vector.h"

#include "mipv6-demux.h"
#include "mipv6-mobility.h"

namespace ns3
{

NS_OBJECT_ENSURE_REGISTERED (MIPv6Demux);

TypeId MIPv6Demux::GetTypeId ()
{
  static TypeId tid = TypeId ("ns3::MIPv6Demux")
    .SetParent<Object> ()
    .AddAttribute ("Mobilities", "The set of IPv6 Mobilities registered with this demux.",
                   ObjectVectorValue (),
                   MakeObjectVectorAccessor (&MIPv6Demux::m_mobilities),
                   MakeObjectVectorChecker<MIPv6Mobility> ())
    ;
  return tid;
}

MIPv6Demux::MIPv6Demux ()
{
}

MIPv6Demux::~MIPv6Demux ()
{
}

void MIPv6Demux::DoDispose ()
{
  for (Ipv6MobilityList_t::iterator it = m_mobilities.begin (); it != m_mobilities.end (); it++)
    {
      (*it)->Dispose ();
      *it = 0;
    }
  m_mobilities.clear ();
  m_node = 0;
  Object::DoDispose ();
}

void MIPv6Demux::SetNode (Ptr<Node> node)
{
  m_node = node;
}

void MIPv6Demux::Insert (Ptr<MIPv6Mobility> mobility)
{
  m_mobilities.push_back (mobility);
}

Ptr<MIPv6Mobility> MIPv6Demux::GetMobility (int mobilityNumber)
{
  for (Ipv6MobilityList_t::iterator i = m_mobilities.begin (); i != m_mobilities.end (); ++i)
    {
      if ((*i)->GetMobilityNumber () == mobilityNumber)
        {
          return *i;
        }
    }
  return 0;
}

void MIPv6Demux::Remove (Ptr<MIPv6Mobility> mobility)
{
  m_mobilities.remove (mobility);
}

} /* namespace ns3 */
