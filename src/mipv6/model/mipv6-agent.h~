#ifndef MIPV6_AGENT_H
#define MIPV6_AGENT_H

#include "ns3/object.h"
#include "ns3/ipv6-address.h"
#include "bcache.h"

namespace ns3
{

class Node;
class Packet;
class Ipv6Interface;
class mipv6Agent : public Object
{
public:
  /**
   * \brief Interface ID
   */
  static TypeId GetTypeId ();

  /**
   * \brief Constructor.
   */
  mipv6Agent ();

  /**
   * \brief Destructor.
   */
  virtual ~mipv6Agent ();

  /**
   * \brief Set the node.
   * \param node the node to set
   */
  void SetNode (Ptr<Node> node);
  
  /**
   * \brief Get the node.
   * \return node
   */
  Ptr<Node> GetNode (void);
  
  virtual uint8_t Receive (Ptr<Packet> packet, const Ipv6Address &src, const Ipv6Address &dst, Ptr<Ipv6Interface> interface);
  
  void SendMessage(Ptr<Packet> packet, Ipv6Address dst, uint32_t ttl);
  
protected:
  virtual uint8_t HandleBU (Ptr<Packet> packet, const Ipv6Address &src, const Ipv6Address &dst, Ptr<Ipv6Interface> interface);
  virtual uint8_t HandleBA (Ptr<Packet> packet, const Ipv6Address &src, const Ipv6Address &dst, Ptr<Ipv6Interface> interface);
  virtual uint8_t HandleHoTI (Ptr<Packet> packet, const Ipv6Address &src, const Ipv6Address &dst, Ptr<Ipv6Interface> interface);
  virtual uint8_t HandleHoT (Ptr<Packet> packet, const Ipv6Address &src, const Ipv6Address &dst, Ptr<Ipv6Interface> interface);
  virtual uint8_t HandleCoTI (Ptr<Packet> packet, const Ipv6Address &src, const Ipv6Address &dst, Ptr<Ipv6Interface> interface);
  virtual uint8_t HandleCoT (Ptr<Packet> packet, const Ipv6Address &src, const Ipv6Address &dst, Ptr<Ipv6Interface> interface);

  /**
   * \brief Dispose this object.
   */
  virtual void DoDispose ();
  
private:
  uint8_t count;
  /**
   * \brief The node.
   */
  Ptr<Node> m_node;
  
};

} /* namespace ns3 */

#endif /* MIPV6_AGENT_H */

