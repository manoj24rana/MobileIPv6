/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2017 Jadavpur University, India
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * Author: Manoj Kumar Rana <manoj24.rana@gmail.com>
 */

#ifndef B_LIST_H
#define B_LIST_H

#include <list>
#include "ns3/packet.h"
#include "ns3/nstime.h"
#include "ns3/ipv6-address.h"
#include "ns3/ptr.h"
#include "ns3/timer.h"
#include "ns3/sgi-hashmap.h"

namespace ns3 {

class BList : public Object
{
public:
  /**
   * \brief Get the type identifier.
   * \return type identifier
   */
  static TypeId GetTypeId ();
  /**
   * \brief constructor.
   * \param haalist home agent address list
   */
  BList (std::list<Ipv6Address> haalist);
  /**
   * \brief destructor
   */
  ~BList ();

  /**
   * \brief get the node pointer.
   * \returns the node pointer 
   */
  Ptr<Node> GetNode () const;

  /**
   * \brief set the node pointer.
   * \param node the node pointer 
   */
  void SetNode (Ptr<Node> node);


  /**
   * \brief whether the home agent is unreachable.
   * \returns the unreachability status of the home agent 
   */
  bool IsHomeUnreachable () const;

  /**
   * \brief whether the home agent is updating now.
   * \returns the updating status of the home agent 
   */
  bool IsHomeUpdating () const;

  /**
   * \brief whether the MN currently performing BU process.
   * \returns the status of the home refreshment 
   */
  bool IsHomeRefreshing () const;

  /**
   * \brief whether the home agent is reachable.
   * \returns the reachability status of the home agent 
   */
  bool IsHomeReachable () const;

  /**
   * \brief mark the reachability status of the home agent as unreachable.
   */
  void MarkHomeUnreachable ();

  /**
   * \brief mark the updating status of the home agent as updating.
   */
  void MarkHomeUpdating ();

  /**
   * \brief mark the refreshing status of the home agent as refreshing.
   */
  void MarkHomeRefreshing ();

  /**
   * \brief mark the reachability status of the home agent as reachable.
   */
  void MarkHomeReachable ();

  //timer processing

  /**
   * \brief If BU transmission failed it starts the BU transmission process.
   */
  void StartHomeRetransTimer ();

  /**
   * \brief If BU transmission succeded or, time out happens, it stops the retransmission timer.
   */
  void StopHomeRetransTimer ();

  /**
   * \brief not used.
   */
  void StartHomeReachableTimer ();

  /**
   * \brief not used.
   */
  void StopHomeReachableTimer ();

  /**
   * \brief not used.
   */
  void StartHomeRefreshTimer ();

  /**
   * \brief not used.
   */
  void StopHomeRefreshTimer ();

  /**
   * \brief not used.
   */
  void FunctionHomeRetransTimeout ();

  /**
   * \brief not used.
   */
  void FunctionHomeReachableTimeout ();

  /**
   * \brief not used.
   */
  void FunctionHomeRefreshTimeout ();

  /**
   * \brief not used.
   */
  Time GetHomeReachableTime () const;

  /**
   * \brief not used.
   */
  void SetHomeReachableTime (Time tm);


  /**
   * \brief no of retransmission tried.
   * \return home BU retry count
   */
  uint8_t GetHomeRetryCount () const;

  /**
   * \brief increase home retransmission counter.
   */
  void IncreaseHomeRetryCount ();

  /**
   * \brief reset home retransmission counter.
   */
  void ResetHomeRetryCount ();

  /**
   * \brief get home bu initial lifetime.
   * \return home bu initial lifetime
   */
  Time GetHomeInitialLifeTime () const;
  /**
   * \brief set home bu initial lifetime.
   * \param tm home bu initial lifetime
   */
  void SetHomeInitialLifeTime (Time tm);

  /**
   * \brief get home bu remaining lifetime.
   * \return home bu remaining lifetime
   */
  Time GetHomeRemainingLifeTime () const;

  /**
   * \brief get last home bu time.
   * \return last home bu lifetime
   */
  Time GetLastHomeBindingUpdateTime () const;
  /**
   * \brief set last home bu time.
   * \param tm last home bu time
   */
  void SetLastHomeBindingUpdateTime (Time tm);

  /**
   * \brief get max home bu sequence.
   * \return max home bu sequence
   */
  uint16_t GetHomeMaxBindingUpdateSequence () const;
  /**
   * \brief set max home bu sequence.
   * \param seq max home bu sequence
   */
  void SetHomeMaxBindingUpdateSequence (uint16_t seq);

  /**
   * \brief get last home bu sequence.
   * \return last home bu sequence
   */
  uint16_t GetHomeLastBindingUpdateSequence () const;

  /**
   * \brief set last home bu sequence.
   * \param seq last home bu sequence
   */
  void SetHomeLastBindingUpdateSequence (uint16_t seq);

  /**
   * \brief get last home bu time.
   * \return last home bu time
   */
  Time GetHomeLastBindingUpdateTime () const;

  /**
   * \brief set last home bu time.
   * \param tm last home bu time
   */
  void SetHomeLastBindingUpdateTime (Time tm);

  /**
   * \brief get tunnel interface index.
   * \return tunnel interface index
   */
  int16_t GetTunnelIfIndex () const;

  /**
   * \brief set tunnel interface index.
   * \param tunnelif tunnel interface index
   */
  void SetTunnelIfIndex (int16_t tunnelif);

  /**
   * \brief set home address.
   * \param hoa home address
   */
  void SetHoa (Ipv6Address hoa);
  /**
   * \brief get home address.
   * \return home address
   */
  Ipv6Address GetHoa () const;
  /**
   * \brief get care-of-address.
   * \return care-of-address
   */
  Ipv6Address GetCoa () const;
  /**
   * \brief set care-of-address.
   * \param addr care-of-address
   */
  void SetCoa (Ipv6Address addr);
  /**
   * \brief set home agent address.
   * \param ha home agent address
   */
  void SetHA (Ipv6Address ha);
  /**
   * \brief get home agent address.
   * \return home agent address
   */
  Ipv6Address GetHA () const;

  /**
   * \brief get home agent address list.
   * \return home agent address list
   */
  std::list<Ipv6Address> GetHomeAgentList () const;
  /**
   * \brief set home agent address list.
   * \param haalist home agent address list
   */
  void SetHomeAgentList (std::list<Ipv6Address> haalist);

  /**
   * \brief get home BU flag.
   * \return home BU flag
   */
  bool GetHomeBUFlag () const;
  /**
   * \brief set home BU flag.
   * \param f home BU flag
   */
  void SetHomeBUFlag (bool f);

  /**
   * \brief get home BU packet.
   * \return home BU packet
   */
  Ptr<Packet> GetHomeBUPacket () const;
  /**
   * \brief set home BU packet.
   * \param pkt home BU packet
   */
  void SetHomeBUPacket (Ptr<Packet> pkt);

  /**
   * \brief CN unreachability checking.
   * \return status
   */
  bool IsCNUnreachable () const;
  /**
   * \brief CN updating checking.
   * \return status
   */
  bool IsCNUpdating () const;
  /**
   * \brief CN refreshing checking.
   * \return status
   */
  bool IsCNRefreshing () const;
  /**
   * \brief CN reachability checking.
   * \return status
   */
  bool IsCNReachable () const;

  /**
   * \brief mark as CN unreachable.
   */
  void MarkCNUnreachable ();
  /**
   * \brief mark as CN updating.
   */
  void MarkCNUpdating ();
  /**
   * \brief mark as CN refreshing.
   */
  void MarkCNRefreshing ();
  /**
   * \brief mark as CN reachable.
   */
  void MarkCNReachable ();

  //timer processing

  /**
   * \brief start bu retransmission timer.
   */
  void StartCNRetransTimer ();

  /**
   * \brief stop bu retransmission timer.
   */
  void StopCNRetransTimer ();

  /**
   * \brief start hoti retransmission timer.
   */
  void StartHoTIRetransTimer ();
  /**
   * \brief stop hoti retransmission timer.
   */
  void StopHoTIRetransTimer ();

  /**
   * \brief start coti retransmission timer.
   */
  void StartCoTIRetransTimer ();
  /**
   * \brief stop coti retransmission timer.
   */
  void StopCoTIRetransTimer ();

  /**
   * \brief start CN reachability timer.
   */
  void StartCNReachableTimer ();

  /**
   * \brief stop CN reachability timer.
   */
  void StopCNReachableTimer ();

  /**
   * \brief start CN refreshing timer.
   */
  void StartCNRefreshTimer ();
  /**
   * \brief stop CN refreshing timer.
   */
  void StopCNRefreshTimer ();

  /**
   * \brief function timeout.
   */
  void FunctionCNRetransTimeout ();

  /**
   * \brief function timeout.
   */
  void FunctionCNReachableTimeout ();

  /**
   * \brief function timeout.
   */
  void FunctionCNRefreshTimeout ();

  /**
   * \brief function timeout.
   */
  void FunctionHoTIRetransTimeout ();

  /**
   * \brief function timeout.
   */
  void FunctionCoTIRetransTimeout ();

  /**
   * \brief get CN reachable time.
   * \return CN reachable time
   */
  Time GetCNReachableTime () const;

  /**
   * \brief set CN reachable time.
   * \param tm CN reachable time
   */
  void SetCNReachableTime (Time tm);

  /**
   * \brief get CN retry count.
   * \return CN retry count
   */
  uint8_t GetCNRetryCount () const;

  /**
   * \brief increase CN retry count.
   */
  void IncreaseCNRetryCount ();

  /**
   * \brief reset CN retry count.
   */
  void ResetCNRetryCount ();

  /**
   * \brief get HoTI retry count.
   * \return HoTI retry count
   */
  uint8_t GetHoTIRetryCount () const;

  /**
   * \brief increase HoTI retry count.
   */
  void IncreaseHoTIRetryCount ();

  /**
   * \brief reset HoTI retry count.
   */
  void ResetHoTIRetryCount ();

  /**
   * \brief get CoTI retry count.
   * \return CoTI retry count
   */
  uint8_t GetCoTIRetryCount () const;

  /**
   * \brief increase CoTI retry count.
   */
  void IncreaseCoTIRetryCount ();

  /**
   * \brief reset CoTI retry count.
   */
  void ResetCoTIRetryCount ();

  /**
   * \brief get CN initial lifetime.
   * \return CN initial lifetime
   */
  Time GetCNInitialLifeTime () const;

  /**
   * \brief set CN initial lifetime.
   * \param tm CN initial lifetime
   */
  void SetCNInitialLifeTime (Time tm);


  /**
   * \brief get CN remaining lifetime.
   * \return CN remaining lifetime
   */
  Time GetCNRemainingLifeTime () const;

  /**
   * \brief get CN last binding update time.
   * \return CN last binding update time
   */
  Time GetCNLastBindingUpdateTime () const;
  /**
   * \brief set CN last binding update time.
   * \param tm CN last binding update time
   */
  void SetCNLastBindingUpdateTime (Time tm);

  /**
   * \brief get CN last binding update sequence.
   * \return CN last binding update sequence
   */
  uint16_t GetCNLastBindingUpdateSequence () const;

  /**
   * \brief set CN last binding update sequence.
   * \param seq CN last binding update sequence
   */
  void SetCNLastBindingUpdateSequence (uint16_t seq);


  /**
   * \brief get CN address.
   * \return CN address
   */
  Ipv6Address GetCN () const;

  /**
   * \brief set CN address.
   * \param addr CN address
   */
  void SetCN (Ipv6Address addr);


  /**
   * \brief get CN BU Flag.
   * \return CN bu flag
   */
  bool GetCNBUFlag () const;

  /**
   * \brief set CN bu flag.
   * \param f CN bu flag
   */
  void SetCNBUFlag (bool f);

  /**
   * \brief get CN bu packet.
   * \return CN bu packet
   */
  Ptr<Packet> GetCNBUPacket () const;

  /**
   * \brief set CN bu packet.
   * \param pkt CN bu packet
   */
  void SetCNBUPacket (Ptr<Packet> pkt);

  /**
   * \brief get HoTI packet.
   * \return HoTI packet
   */
  Ptr<Packet> GetHoTIPacket () const;

  /**
   * \brief set HoTI packet.
   * \param pkt HoTI packet
   */
  void SetHoTIPacket (Ptr<Packet> pkt);

  /**
   * \brief get CoTI packet.
   * \return CoTI packet
   */
  Ptr<Packet> GetCoTIPacket () const;

  /**
   * \brief set CoTI packet.
   * \param pkt CoTI packet
   */
  void SetCoTIPacket (Ptr<Packet> pkt);


  /**
   * \brief get Home init cookie.
   * \return Home init cookie
   */
  uint64_t GetHomeInitCookie () const;

  /**
   * \brief set Home init cookie.
   * \param hcookie Home init cookie
   */
  void SetHomeInitCookie (uint64_t hcookie);

  /**
   * \brief get care-of init cookie.
   * \return care-of init cookie
   */
  uint64_t GetCareOfInitCookie () const;

  /**
   * \brief set care-of init cookie.
   * \param ccookie care-of init cookie
   */
  void SetCareOfInitCookie (uint64_t ccookie);

  /**
   * \brief get home keygen token.
   * \return home keygen token
   */
  uint64_t GetHomeKeygenToken () const;

  /**
   * \brief set home keygen token.
   * \param htoken home keygen token
   */
  void SetHomeKeygenToken (uint64_t htoken);

  /**
   * \brief get care-of keygen token.
   * \return care-of keygen token
   */
  uint64_t GetCareOfKeygenToken () const;

  /**
   * \brief set care-of keygen token.
   * \param ctoken care-of keygen token
   */
  void SetCareOfKeygenToken (uint64_t ctoken);

  /**
   * \brief get home nonce index.
   * \return home nonce index
   */
  uint16_t GetHomeNonceIndex () const;

  /**
   * \brief set home nonce index.
   * \param hnonce home nonce index
   */
  void SetHomeNonceIndex (uint16_t hnonce);

  /**
   * \brief get care-of nonce index.
   * \return care-of nonce index
   */
  uint16_t GetCareOfNonceIndex () const;

  /**
   * \brief set care-of nonce index.
   * \param cnonce care-of nonce index
   */
  void SetCareOfNonceIndex (uint16_t cnonce);

  /**
   * \brief flush
   */
  void Flush ();

  /**
   * \brief set home address registered flag.
   * \param flag home address registered flag
   */
  void SetHomeAddressRegistered (bool flag);

  /**
   * \brief check home address registered flag.
   * \return home address registered flag
   */
  bool IsHomeAddressRegistered ();



private:

  /**
   * \enum BindingUpdateState_e
   * \brief binding update state
   */
  enum BindingUpdateState_e
  {
    UNREACHABLE,
    UPDATING,
    REFRESHING,
    REACHABLE,
  };

  /**
   * \brief binding update state variable
   */
  BindingUpdateState_e m_hstate;

  /**
   * \brief tunnel interface index
   */
  int16_t m_tunnelIfIndex;

  /**
   * \brief home bu packet
   */
  Ptr<Packet> m_hpktbu;

  /**
   * \brief home initial lifetime
   */
  Time m_hinitiallifetime;

  /**
   * \brief home last binding update sequence
   */
  uint16_t m_hlastBindingUpdateSequence;

  /**
   * \brief home bu flag
   */
  bool m_hflag;

  /**
   * \brief home bu last sent
   */
  Time m_hbulastsent;

  /**
   * \brief home address
   */
  Ipv6Address m_hoa;

  /**
   * \brief CoA
   */
  Ipv6Address m_coa;

  /**
   * \brief home agent address
   */
  Ipv6Address m_ha;

  /**
   * \brief home agent address list
   */
  std::list<Ipv6Address> m_HaaList;

  /**
   * \brief home reachable time
   */
  Time m_hreachableTime;

  /**
   * \brief home retransmission timer
   */
  Timer m_hretransTimer;

  /**
   * \brief home reachable timer
   */
  Timer m_hreachableTimer;

  /**
   * \brief home refresh timer
   */
  Timer m_hrefreshTimer;

  /**
   * \brief home bu retry count
   */
  uint8_t m_hretryCount;



  /**
   * \brief cn bu state
   */
  BindingUpdateState_e m_cnstate;

  /**
   * \brief cn bu packet
   */
  Ptr<Packet> m_cnpktbu;

  /**
   * \brief cn hoti packet
   */
  Ptr<Packet> m_pkthoti;

  /**
   * \brief cn coti packet
   */
  Ptr<Packet> m_pktcoti;

  /**
   * \brief cn initial lifetime
   */
  Time m_cninitiallifetime;

  /**
   * \brief cn last binding update sequence
   */
  uint16_t m_cnlastBindingUpdateSequence;

  /**
   * \brief last cn bu sent time 
   */
  Time m_cnbulastsent;

  /**
   * \brief cn recheable time
   */
  Time m_cnreachableTime;

  /**
   * \brief cn retransmission timer
   */
  Timer m_cnretransTimer;

  /**
   * \brief cn reachable timer
   */
  Timer m_cnreachableTimer;

  /**
   * \brief cn refresh timer
   */
  Timer m_cnrefreshTimer;

  /**
   * \brief hoti retransmission timer
   */
  Timer m_hotiretransTimer;

  /**
   * \brief coti retransmission timer
   */
  Timer m_cotiretransTimer;

  /**
   * \brief cn bu retry count
   */
  uint8_t m_cnretryCount;

  /**
   * \brief cn bu status
   */
  bool m_cnflag;

  /**
   * \brief cn
   */
  Ipv6Address m_cn;

  /**
   * \brief hoti retry count
   */
  uint8_t m_hotiretryCount;

  /**
   * \brief coti retry count
   */
  uint8_t m_cotiretryCount;

  /**
   * \brief home init cookie
   */
  uint64_t m_homeinitcookie;

  /**
   * \brief care-of init cookie
   */
  uint64_t m_careofinitcookie;

  /**
   * \brief home keygen token
   */
  uint64_t m_homekeygentoken;

  /**
   * \brief care-of keygen token
   */
  uint64_t m_careofkeygentoken;

  /**
   * \brief home nonce index
   */
  uint16_t m_homenonceindex;

  /**
   * \brief care-of nonce index
   */
  uint16_t m_careofnonceindex;

  /**
   * \brief home address registered flag
   */
  bool m_HomeAddressRegisteredFlag;

  /**
   * \brief Dispose this object.
   */
  void DoDispose ();

  /**
   * \brief the MN.
   */
  Ptr<Node> m_node;
};

} /* ns3 */

#endif /* BINDING_UPDATE_LIST_H */
