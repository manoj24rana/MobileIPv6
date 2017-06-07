#ifndef MIPV6_CN_H
#define MIPV6_CN_H

#include "mipv6-agent.h"
#include "bcache.h"
#include "mipv6-header.h"

namespace ns3
{
class Packet;

class mipv6CN : public mipv6Agent {
public:
  mipv6CN ();
  
  virtual ~mipv6CN ();
  //Ipv6Address tempadds;
  //uint16_t temptunnelIf;
protected:
  virtual void NotifyNewAggregate ();
  
  Ptr<Packet> BuildBA (Ipv6MobilityBindingUpdateHeader bu, Ipv6Address hoa, uint8_t status);
  Ptr<Packet> BuildHoT (Ipv6HoTIHeader hoti, Ipv6Address hoa);
  Ptr<Packet> BuildCoT (Ipv6CoTIHeader coti, Ipv6Address hoa); 
  virtual uint8_t HandleBU (Ptr<Packet> packet, const Ipv6Address &src, const Ipv6Address &dst, Ptr<Ipv6Interface> interface);
  virtual uint8_t HandleHoTI (Ptr<Packet> packet, const Ipv6Address &src, const Ipv6Address &dst, Ptr<Ipv6Interface> interface);
  virtual uint8_t HandleCoTI (Ptr<Packet> packet, const Ipv6Address &src, const Ipv6Address &dst, Ptr<Ipv6Interface> interface);
  
private:
Ptr<BCache> m_bCache;

};

} /* namespace ns3 */

#endif /* MIPV6_CN_H */

