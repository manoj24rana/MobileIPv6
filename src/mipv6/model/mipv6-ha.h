#ifndef MIPV6_HA_H
#define MIPV6_HA_H

#include "mipv6-agent.h"
#include "bcache.h"
#include "mipv6-header.h"

namespace ns3
{
class Packet;

class mipv6HA : public mipv6Agent {
public:
  mipv6HA ();
  
  virtual ~mipv6HA ();
   std::list<Ipv6Address> HomeAgentAddressList(); 
   void DoDADForOffLinkAddress (Ipv6Address target, Ptr<Ipv6Interface> interface);
   void FunctionDadTimeoutForOffLinkAddress (Ptr<Ipv6Interface> interface, Ptr<Packet> ba, Ipv6Address homeaddr);

   void DADFailureIndication(Ipv6Address addr);
   bool IsAddress(Ipv6Address addr);
   bool IsAddress2(Ipv6Address addr);

protected:
  virtual void NotifyNewAggregate ();
  
  Ptr<Packet> BuildBA (Ipv6MobilityBindingUpdateHeader bu,Ipv6Address hoa, uint8_t status);
  
  virtual uint8_t HandleBU (Ptr<Packet> packet, const Ipv6Address &src, const Ipv6Address &dst, Ptr<Ipv6Interface> interface);
  bool SetupTunnelAndRouting (BCache::Entry *bce);
  bool ClearTunnelAndRouting (BCache::Entry *bce);

  
private:
Ptr<BCache> m_bCache;
//std::list<Ipv6Address> HaaList;
//typedef sgi::hash_map<Ipv6Address, uint16_t> ::iterator HaaListI;
//HaaList m_HaaList;

};

} /* namespace ns3 */

#endif /* MIPV6_HA_H */

