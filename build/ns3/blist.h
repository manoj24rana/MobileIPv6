#ifndef B_LIST_H
#define B_LIST_H

#include <stdint.h>

#include <list>

#include "ns3/packet.h"
#include "ns3/nstime.h"
#include "ns3/net-device.h"
#include "ns3/ipv6-address.h"
#include "ns3/ptr.h"
#include "ns3/timer.h"
#include "ns3/sgi-hashmap.h"

#include "identifier.h"

namespace ns3
{

class BList : public Object
{
public:

  static TypeId GetTypeId ();

  BList(std::list<Ipv6Address> haalist);
  
  ~BList();

  
  Ptr<Node> GetNode() const;
  void SetNode(Ptr<Node> node);

	bool IsHomeUnreachable() const;
	bool IsHomeUpdating() const;
	bool IsHomeRefreshing() const;
	bool IsHomeReachable() const;

	void MarkHomeUnreachable();
	void MarkHomeUpdating();
	void MarkHomeRefreshing();
	void MarkHomeReachable();

	//timer processing
	void StartHomeRetransTimer();
	void StopHomeRetransTimer();
	
	void StartHomeReachableTimer();
	void StopHomeReachableTimer();
	
	void StartHomeRefreshTimer();
	void StopHomeRefreshTimer();
	
	void FunctionHomeRetransTimeout();
	void FunctionHomeReachableTimeout();
	void FunctionHomeRefreshTimeout();

	Time GetHomeReachableTime() const;
	void SetHomeReachableTime(Time tm);

	uint8_t GetHomeRetryCount() const;
	void IncreaseHomeRetryCount();
	void ResetHomeRetryCount();
	


	Time GetHomeInitialLifeTime() const;
	void SetHomeInitialLifeTime(Time tm);
	
	Time GetHomeRemainingLifeTime() const;
  	
	Time GetLastHomeBindingUpdateTime() const;
	void SetLastHomeBindingUpdateTime(Time tm);
	
	
	uint16_t GetHomeMaxBindingUpdateSequence() const;
	void SetHomeMaxBindingUpdateSequence(uint16_t seq);

	uint16_t GetHomeLastBindingUpdateSequence() const;
	void SetHomeLastBindingUpdateSequence(uint16_t seq);	

	Time GetHomeLastBindingUpdateTime() const;
	void SetHomeLastBindingUpdateTime(Time tm);


	int16_t GetTunnelIfIndex() const;
	void SetTunnelIfIndex(int16_t tunnelif);
	
	void SetHoa(Ipv6Address hoa);
	Ipv6Address GetHoa() const;
	
	Ipv6Address GetCoa() const;
	void SetCoa(Ipv6Address addr);

	void SetHA(Ipv6Address ha);	
	Ipv6Address GetHA() const;
 
	std::list<Ipv6Address> GetHomeAgentList() const;
	void SetHomeAgentList(std::list<Ipv6Address> haalist);

	bool GetHomeBUFlag() const;
	void SetHomeBUFlag(bool f);


	Ptr<Packet> GetHomeBUPacket() const;
	void SetHomeBUPacket(Ptr<Packet> pkt);


	bool IsCNUnreachable() const;
	bool IsCNUpdating() const;
	bool IsCNRefreshing() const;
	bool IsCNReachable() const;

	void MarkCNUnreachable();
	void MarkCNUpdating();
	void MarkCNRefreshing();
	void MarkCNReachable();

	//timer processing
	void StartCNRetransTimer();
	void StopCNRetransTimer();

	void StartHoTIRetransTimer();
	void StopHoTIRetransTimer();

	void StartCoTIRetransTimer();
	void StopCoTIRetransTimer();
	
	void StartCNReachableTimer();
	void StopCNReachableTimer();
	
	void StartCNRefreshTimer();
	void StopCNRefreshTimer();
	
	void FunctionCNRetransTimeout();
	void FunctionCNReachableTimeout();
	void FunctionCNRefreshTimeout();
	void FunctionHoTIRetransTimeout();
	void FunctionCoTIRetransTimeout();

	Time GetCNReachableTime() const;
	void SetCNReachableTime(Time tm);

	uint8_t GetCNRetryCount() const;
	void IncreaseCNRetryCount();
	void ResetCNRetryCount();

	uint8_t GetHoTIRetryCount() const;
	void IncreaseHoTIRetryCount();
	void ResetHoTIRetryCount();

	uint8_t GetCoTIRetryCount() const;
	void IncreaseCoTIRetryCount();
	void ResetCoTIRetryCount();


	Time GetCNInitialLifeTime() const;
	void SetCNInitialLifeTime(Time tm);
	
	Time GetCNRemainingLifeTime() const;
  	
	Time GetCNLastBindingUpdateTime() const;
	void SetCNLastBindingUpdateTime(Time tm);
	

	uint16_t GetCNLastBindingUpdateSequence() const;
	void SetCNLastBindingUpdateSequence(uint16_t seq);
	

	
	Ipv6Address GetCN() const;
	void SetCN(Ipv6Address addr);

	bool GetCNBUFlag() const;
	void SetCNBUFlag(bool f);


	Ptr<Packet> GetCNBUPacket() const;
	void SetCNBUPacket(Ptr<Packet> pkt);

	Ptr<Packet> GetHoTIPacket() const;
	void SetHoTIPacket(Ptr<Packet> pkt);

	Ptr<Packet> GetCoTIPacket() const;
	void SetCoTIPacket(Ptr<Packet> pkt);


	uint64_t GetHomeInitCookie() const;
	void SetHomeInitCookie(uint64_t hcookie);

	uint64_t GetCareOfInitCookie() const;
	void SetCareOfInitCookie(uint64_t ccookie);

	uint64_t GetHomeKeygenToken() const;
	void SetHomeKeygenToken(uint64_t htoken);

	uint64_t GetCareOfKeygenToken() const;
	void SetCareOfKeygenToken(uint64_t ctoken);

	uint16_t GetHomeNonceIndex() const;
	void SetHomeNonceIndex(uint16_t hnonce);

	uint16_t GetCareOfNonceIndex() const;
	void SetCareOfNonceIndex(uint16_t cnonce);

	void Flush ();
	void SetHomeAddressRegistered(bool flag);
	bool IsHomeAddressRegistered();
	


private:
	enum BindingUpdateState_e {
      UNREACHABLE,
	  UPDATING,
	  REFRESHING,
	  REACHABLE,
	};
	
	BindingUpdateState_e m_hstate;

	int16_t m_tunnelIfIndex;
	Ptr<Packet> m_hpktbu;
	Time m_hinitiallifetime;
	uint16_t m_hlastBindingUpdateSequence;
	bool m_hflag;
	Time m_hbulastsent;
	Ipv6Address m_hoa;
        Ipv6Address m_coa;
        Ipv6Address m_ha;
	std::list<Ipv6Address> m_HaaList;
	Time m_hreachableTime;	
	Timer m_hretransTimer;	
	Timer m_hreachableTimer;	
	Timer m_hrefreshTimer;
	uint8_t m_hretryCount;




	BindingUpdateState_e m_cnstate;
	Ptr<Packet> m_cnpktbu;
	Ptr<Packet> m_pkthoti;
	Ptr<Packet> m_pktcoti;
	Time m_cninitiallifetime;
	uint16_t m_cnlastBindingUpdateSequence;
	Time m_cnbulastsent;
	Time m_cnreachableTime;	
	Timer m_cnretransTimer;
	Timer m_cnreachableTimer;	
	Timer m_cnrefreshTimer;
	Timer m_hotiretransTimer;
	Timer m_cotiretransTimer;	
	uint8_t m_cnretryCount;
	bool m_cnflag;
	Ipv6Address m_cn;
	uint8_t m_hotiretryCount;
	uint8_t m_cotiretryCount;
	uint64_t m_homeinitcookie;
	uint64_t m_careofinitcookie;
	uint64_t m_homekeygentoken;
	uint64_t m_careofkeygentoken;
	uint16_t m_homenonceindex;
	uint16_t m_careofnonceindex;
	bool m_HomeAddressRegisteredFlag;

  
  void DoDispose();
  
  
  Ptr<Node> m_node;
};

} /* ns3 */

#endif /* BINDING_UPDATE_LIST_H */
