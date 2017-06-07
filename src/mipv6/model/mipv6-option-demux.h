
#ifndef MIPV6_OPTION_DEMUX_H
#define MIPV6_OPTION_DEMUX_H

#include <list>

#include "ns3/header.h"
#include "ns3/object.h"
#include "ns3/ptr.h"

namespace ns3
{

class MIPv6Option;
class Node;

/**
 * \class MIPv6OptionDemux
 * \brief Ipv6 Mobility Option Demux.
 */
class MIPv6OptionDemux : public Object
{
public:
  /**
   * \brief The interface ID.
   * \return type ID
   */
  static TypeId GetTypeId (void);

  /**
   * \brief Constructor.
   */
  MIPv6OptionDemux ();

  /**
   * \brief Destructor.
   */
  virtual ~MIPv6OptionDemux ();

  /**
   * \brief Set the node.
   * \param node the node to set
   */
  void SetNode (Ptr<Node> node);

  /**
   * \brief Insert a new IPv6 Mobility Option.
   * \param option the option to insert
   */
  void Insert (Ptr<MIPv6Option> option);

  /**
   * \brief Get the option corresponding to optionNumber.
   * \param optionNumber the option number of the option to retrieve
   * \return a matching IPv6 Mobility option
   */
  Ptr<MIPv6Option> GetOption (int optionNumber);

  /**
   * \brief Remove an option from this demux.
   * \param option pointer on the option to remove
   */
  void Remove (Ptr<MIPv6Option> option);

protected:
  /**
   * \brief Dispose this object.
   */
  virtual void DoDispose();

private:
  typedef std::list<Ptr<MIPv6Option> > Ipv6MobilityOptionList_t;

  /**
   * \brief List of IPv6 Options supported.
   */
  Ipv6MobilityOptionList_t m_options;

  /**
   * \brief The node.
   */
  Ptr<Node> m_node;
};

} /* namespace ns3 */

#endif /* IPV6_MOBILITY_OPTION_DEMUX_H */
