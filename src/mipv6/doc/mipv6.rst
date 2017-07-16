.. include:: replace.txt
.. highlight:: cpp


MIPv6
*****

MIPv6 module is RFC 6275 compliance and supports seamless intra and inter-technology IP
handoff. Whenever a mobile node (MN) resides in foreign network, all data transmission
happens in bi-directional tunneling mode (supported tunneling protocol: IPv6 in IPv6).
If MN attaches with its home network, data transmission does not follow the tunnel (RFC
compliance). As |ns3| currently supports three main layer 2 technologies: WiFi, WiMAX
and LTE; MIPv6 module can be run over the mixture of these technologies.


Module Description
*****************

The source code for the new module lives in the directory ``src/mipv6``.

The MIPv6 class diagram is illustrated in figure :ref:`fig-mipv6-class`. The white boxes
represent the classes which already exist in |ns3| and the classes with colored boxes are
newly defined to implement MIPv6. These classes are divided into four modules:


.. _fig-mipv6-class:
   
.. figure:: figures/mipv6-class.*
   :align: center

   MIPv6 class digram


header
------
In figure :ref: `fig-header`, the main data members of MIPv6 header class are shown.
As the type field of the options are used by the MIPv6 mobility messages, the existing |ns3|
Ipv6OptionHeader cannot be extended for the MIPv6 specific option headers.

.. _fig-header:
   
.. figure:: figures/mipv6-header.*
   :align: center

   MIPv6 header class diagram

Mobility header (BU, BA,HoT etc.) class inherits the MIPv6OptionField classes too which
manages all the options available in the MIPv6 header and also performs the necessary padding
to ensure that the whole header remains a multiple of 8 octets. The padding mechanism uses
CalculatePad() method with the help of GetAlignment() method in MIPv6OptionHeader, when
Addoption() method includes an additional new option with a header. The GetAlignment() function
returns information about padding rule [3].

Using these classes, it is easy to create a mobility message or, insert an option header in a packet. For
example, to create a BU message, containing an option of AlternateCoA, the Packet class is
instantiated and the mobility header is inserted using the AddHeader() method. The mobility header
contains the main MIPv6BUHeader class instance. To Create an AlternetCoA class instance, the
AddOption() method of the header class is called to insert the option within the BU header. Thus the
header instantiation is completed and the mobility message is formed.


internetstack, netdevice
------------------------
MIPv6Demux class is implemented to handle mobility message classes (MIPv6BU, MIPv6BA etc.) which
are inherited from MIPv6Mobility class. The functionality of MIPv6Demux class is similar as the
IPv6OptionDemux class which de-multiplexes IPv6 options. The recognition and forwarding of mobility
messages with the mobility header is done by the existing lower layer IPv6 class (Ipv6L3Protocol).
In MIPv6 layer, the mobility message specific operation starts after the recognition made by the
MIPv6Demux class. It defines three functions:

* InsertMobility()

* GetMobility() and

* RemoveMobility()

At the time of MIPv6 installation to an entity (MN, HA and CN), MIPv6Demux class inserts all
required mobility types (BU, BA, HoTI, CoTI, HoT and CoT) for that node by calling InsertMobility()
function. It is done in advance such that whenever it has to send/receive such messages, it can
recognize it through GetMobility() function and handle it appropriately. If there is a need to define
new mobility message (e.g., BU, BA), one must create a subclass (e.g. MIPv6BU, MIPv6BA) of MIPv6Mobility
class and insert it into MIPv6Demux class. Alongside, MIPv6Mobility declares two abstract methods which
are implemented by its sub classes:

* GetMobilityNumber() and

* Process()

According to RFC 6275, each mobility message contains a mobility number, which is returned by the
GetMobilityNumber() function. If a mobility message is received from the lower layer, it is processed
by the Process() function. It checks whether all fields of the message is correct or, not. If it finds
all the fields are meaningful to process further, it calls the appropriate Receive() function of the node
(i.e., MN, HA, and CN) to handle it. For example, the Receive() function of MIPv6HA is called to send a BA
message to the MN.

Secondly, we have placed the MIPv6 layer above IP layer by two main classes:

* MIPv6L4Protocol and

* Ipv6TunnelL4Protocol

The MIPv6L4Protocol class handles mobility messages and the Ipv6TunnelL4Protocol class deals
with data packets.

Whenever a packet is received by Ipv6L3Protocol from lower layer, it forwards it to MIPv6 layer.
The encapsulated data packet or, a mobility message contains a type value (MH type) in its header
which is different from normal data packet. Based on this value, the appropriate L4 protocol class
is associated. Both of them derived the Receive() function of the base IpL4Protocol class. After
receiving a mobility message in MIPv6L4Protocol, it calls the Process() function of the MIPv6Demux
class to process all the fields. Then, it calls the appropriate handler of that message defined in
the entity class (e.g., mipv6MN).

On the other hand, the Receive() function of the Ipv6TunnelL4Protocol class checks an encapsulated
packet’s tunnel interface which has been used to de-capsulate the packet. If there is no matching
tunnel interface in its binding cache/list, it drops the packet. Ipv6TunnelL4Protocol class can also
add, remove and modify a tunnel to a node. A tunnel is created by a virtual mac layer net device to
a node using the TunnelNetDevice class, derived from the base NetDevice class. The TunnelNetDevice
class re-implements the Send() function of the base class. Receiving a packet from the upper layer,
the Send() function performs IPv6-in-IPv6 encapsulation by forming a new IPv6 header with new source
and destination address. So, the instantiation of the TunnelNetDevice class is the main functionality
of the Ipv6TunnelL4Protocol class. All the tunnel related methods are triggered at the time of sending
/receiving BU/BA messages of the agent classes (mipv6MN/HA/CN) as defined in RFC 6275.

After the implementation of the above header classes, MIPv6 layer specific classes and routing
classes, we implement the agent based classes:

* mipv6MN and

* mipv6HA/CN

All of them are derived from the base mipv6Agent class, which implements two main methods:

* Receive() and

* HandleXX()

The Receive() function is called by the Process() function of MIPv6Mobility class, after receiving a
message from the MIPv6L4Protocol class. After receiving such mobility message, it calls the proper
handler function such as HandleBU(), HandleBA() etc. These handler functions actually handle the
mobility messages and trigger the next event. For example, after receiving BU message, HandleBU()
forms a BA message and sends it. However mipv6Agent class has not defined the handler classes as
its function is specific to the agent classes.

mipv6MN and mipv6HA classes can also insert and delete tunnel functionalities by calling specific
functions defined in IPv6TunnelL4Protocol class. At the time of receiving BU at HA and receiving
BA at MN, an AddHostRouteTo() function is called to insert host route in the routing table. If MN
detaches from its current link, a RemoveRouteTo() function is also called that removes the host
route from the routing table. How mipv6MN class sends BU after configuring IPv6 address and how
mipv6HA class receives BU and sends BA is discussed in the next sub sections.

Lastly, two independent classes: BCache and BList are associated with the mipv6HA/CN and
mipv6MN classes respectively. They contain data members: CoA, HoA, lifetime, HA address,
tunnel interface index, sequence number, BU state, and nonce indices as defined in [3]. To
handle the information of multiple MNs and HAs, each entry in the BCache and BList class is
keyed by the HoA.

helper
------
Helper is also a supporting implementation module as the Header module. It basically works
above all the main implemented classes. Three helper classes are created for three agent
classes. Users who want to run MIPv6, uses the helper classes to install the MIPv6 functionality
to corresponding nodes. These classes implement an important function called Install(). This
method takes a node pointer and the required data member like HA addresses as arguments and install
over a node. It installs the MIPv6L4Protocol and Ipv6TunnelL4Protocol following the CreateObject()
and AggregateObject() functions, defined in the core part of |ns3|. After that it registers the
corresponding mobility messages using MIPv6L4Protocol class.

BU Process
----------
The BU process starts after MN configures an IPv6 address in a new subnet. If the state reaches in
“PREFERRED” state, the address configuration is successfully completed. Figure :ref: `fig-bu`
shows the BU process of MIPv6 in |ns3|. After the DAD timeout event, it sets the state by calling the
SetState() method of Ipv6Interface class. The mipv6MN class has set a function prototype, m_NewIpCallback
at Ipv6Interface class at the time of initialization. It invokes the BU sending process at mipv6MN after
address configuration. It passes the packet to the default WifiNetDevice.

Once the Ipv6 packet arrives at the netdevice of the HA, it is delivered to the Ipv6L3Protocol through
ProtocolHandler in Node class. Then, Ipv6L3Protocol passes the packet to the right transport handler,
checking the Ipv6 next header field in the packet. So, the BU packet is received at the HA in the
MIPv6L4Protocol. It then demultiplexes the packet in the MIPv6BU, based on mobility header type in BU
packet. MIPv6BU passes the packet to the mipv6HA which creates or updates the binding cache and sets
up tunnel for this new source. A BA is sent to the MN in the similar process. The MN now creates or
updates the BU list and sets up the tunnel towards the HA. Thus, the BU process is completed. The BU
process with CN is similar, but the tunnel setup and routing setup at MN and CN do not occur (users are
instructed to skip the CN part as route optimization is currently not supported).

.. _fig-bu:
   
.. Figure:: figures/bu.*
   :align: center

   MIPv6 binding update process


Data Packet Processing
----------------------

At the completion of BU process, a bidirectional tunnel is set up between the HA and the MN. In
figure :ref: `fig-mntoha`, the data packet processing through the tunnel in the direction for MN to
the HA is shown.

Figure :ref: `fig-hatomn` shows the data packet processing path in the direction from the HA to
the MN. When a data packet arrives from application layer, Ipv6L3Protocol performs routing query
for incoming packet by calling RouteInput() method of Ipv6StaticRouting class. It gives the outgoing
tunnel netdevice interface as output to forward the packet. In TunnelNetDevice, the packet is
encapsulated with another Ipv6 header and is passed to Ipv6L3Protocol which sends the packet
through the WifiNetDevice.

.. _fig-mntoha:
   
.. figure:: figures/mn-to-ha.*
   :align: center

   Data packet processing from the MN to the HA direction



.. _fig-hatomn:
   
.. Figure:: figures/ha-to-mn.*
   :align: center

   Data packet processing from the HA to the MN direction

At the HA, encapsulated data packet is forwarded to the Ipv6TunnelL4Protocol following the
Ipv6L3Protocol. The routing query calls the LocalDeliver() function as the tunnel end point is
at the HA’s global address. The Receive() function of the Ipv6TunnelL4Protocol de-capsulates
the packet and sends to Ipv6L3Protocol to send it to the CN.

On the other hand, packets from the CN are received by the HA and passed to Ipv6L3Protocol.
Following Ipv6StaticRouting, the IpForward() method sends the packet to TunNetDevice as the
routing table already contains a route entry for the MN’s HoA. After encapsulation, Ipv6L3Protocol
sends the packet to the MN. The MN processes the packet similarly as the HA. Instead of sending the
packet again to Ipv6L3Protocol, the Ipv6TunnelL4Protocol forwards it to the upper layer. Thus, The
IP mobility remains transparent to the upper layer. The trigger to start BU process with the CN
starts after the reception of BA message from the HA. The BU process is initiated by exchanging HoTI,
CoTI, HoT, CoT messages used by MIPv6MN and MIPv6CN class.

In case of route optimization (although this part is not implemented yet), the CN uses the MN’s CoA
as destination address. The event is triggered at Ipv6L3Protocol class that sends data packet. For
this, we could use a function prototype, m_NewRouteCallback at Ipv6L3Protocol class. It changes the
destination address to MN’s CoA. The value of this variable becomes true while the CN gets BU from
MN and successfully accepts it. Hence, all further packets would use the changed destination address.
The value of the variable becomes false when the CN gets a disconnection message from the HA. The HA
sends this message to the CN immediately after sending BA to the MN.


Design
======

The underlying design approaches are followed:

* Event driven triggering techniques : |ns3| callback procedure is followed to trgger an event. For
example, MIPv6 binding update (BU) process starts just after the completion of the duplicate address
detection mechanism of ICMPv6. For this, we have used a trigger in the ICMPv6 protocol to invoke the
binding process.

* Implementation of non-derived base classes : apart from using classes derived from |ns3| base classes,
we have also defined some new base classes, not provided in |ns3|. For instance, the binding cache class
(that stores handoff signaling information at home agent (HA)) is a non-derived base class, whereas the
IPv6 mobility header class is derived from the |ns3| Header class.

* Helper class : it interconnects the low level APIs, used in the core MIPv6 module, to make the MIPv6
function installation easier for naive users. For example, the tedious and quiet long code for a mobile
node (MN) can be installed on a network entity by just using the install() method, defined in the MIPv6
MN helper class.


Scope and Limitations
=====================

* Current implementation only supports bidirectional tunnelling mode and does not support
route optimization mode and the corresponding return routability test which has been
specified in RFC (Although the current version contains the necessary implementation code
within it such that, it could be easily implemented later).

* RFC specifies multi-homing support for mobile node. A mechanism known as "dynamic
home agent address discovery" allows a mobile node to dynamically discover the IP address
of a home agent on its home link, even when the mobile node is away from home. This
mechanism is not supported in our current implementation. Our implementation is coded
under the assumption that the mobile node is configured with the home prefix which is RFC
compliance.

* Mobile IPv6 security is also not supported as there would be very less researchers who seek
to get this support in simulation. This feature generally interests researchers, working in
real-time mode.

* The support of RFC 6275, described in Section 11.5.5 i.e. ‘returning home’ feature is still not
supported and would be resolved as soon as possible.


References
==========

* C.E. Perkins , D.B. Johnson , J. Arkko , "Mobility support in IPv6", IETF, July 2011, RFC 6275 .

* M. K. Rana, B. Sardar, S. Mandal, Debashis Saha, "Implementation and performance evaluation of a mobile IPv6 (MIPv6) simulation model for ns-3", Simulation Modelling Practice and Theory, Volume 72, 2017, Pages 1-22, ISSN 1569-190X, http://dx.doi.org/10.1016/j.simpat.2016.12.005.
(http://www.sciencedirect.com/science/article/pii/S1569190X16302714)

* J. Xie, I. Howitt, I. Shibeika, "IEEE 802.11-based mobile IP fast handoff latency analysis", in Proceedings of IEEE International Conference on Communications (ICC ’07), 2007, pp. 6055–6060, doi: 10.1109/ICC.2007.1003.

* F.Z. Yousaf , C. Bauer , C. Wietfeld , An accurate and extensible mobile IPv6 (xMIPV6) simulation model for OMNeT ++ , in: Proceedings of Simulation and Tools (Simutools ’08), ACM, 2008 Article No. 88. ISBN: 978-963-9799-20-2 .

Usage
*****

MIPv6 feature can be used on a topology by following:

* To install MIPv6, simply use the install () function of the helper class (Although, the current
implementation does not support route optimization, so, avoid to use the Correspondent node helper
class).

* The order you should follow at the time of installation is that: MIPv6 installation must be
done after the basic device installation and IP stack installation, but, before any application
installation.

* Don’t install MIPv6 on MN before HA, because, after the HA functionality installation, the
home address list would be returned that would be used at the time of MN installation. The
home address allocation to MN is done automatically.

The following assumptions on system topology are made:

* Current implementation assumes routing table is populated with the proper routing entry.
If a new topology is configured by a user, apart from the given scenarios in the examples,
the routing table entry must be populated as done in the examples. Otherwise, packet may
be lost for routing failure.

* It is assumed that in the user-defined scenario, HA, CN and MN are different. Also, HA must
not be mobile i.e. address auto configuration must not be done in case of HA.

Helpers
=======

See previous sub section: 'helper'.

Attributes
==========

What classes hold attributes, and what are the key ones worth mentioning?

Output
======

What kind of data does the model generate?  What are the key trace
sources?   What kind of logging output can be enabled?

Advanced Usage
==============

This modulu can be extended to implement more advanced modules like:

* NEMO basic support protocol, RFC 3963

* PMIPv6, RFC 5213

* HMIPv6, RFC 5380 etc.

Examples
========

All sample examples are in ../src/mipv6/examples directory. These examples
illustrates the following:

* single MN, supporting only bu, no data transmission (mipv6-simple-single.cc)

* multiple MNs, supporting only bu, no data transmission (mipv6-simple-multiple.cc)

* single MN, supporting both bu and data transmission (mipv6-single.cc)

* multiple MNs, supporting both bu and data transmission (mipv6-multiple.cc)

* server application running on MN and client application running on CN (mipv6-mn-server.cc)

* server application running on CN and client application running on MN (mipv6-cn-server.cc)

* MN starts at home and goes to foreign (mipv6-ho-to-fo.cc)

* wiFi to wimax (mipv6-wifi-wimax.cc)

* wimax to wimax (mipv6-wimax-wimax.cc)


Validation
**********

Describe how the model has been tested/validated.  What tests run in the
test suite?  How much API and code is covered by the tests?  Again, 
references to outside published work may help here.

* Validation against Linux Testbed results:

J. S. Lee et al. conducted simulations for MIPv6 using linux based MIPL 1.0. They considered
only movement detection (tmd) and address configuration (tac) time components of handoff delay
(thandoff) in their simulations. In table :ref:`tab-exp-comp-1`, our simulation
results are compared with their results for two cases:

  case 1:  MaxRtrAdvInterval - MinRtrAdvInterval: 1.50 - 0.05 second, and
  case 2:  MaxRtrAdvInterval - MinRtrAdvInterval: 3.00 - 1.00 second, and

The results are not too different except the tmd value. This can be explained in terms of the
seed value used to generate the random value of MAX_RTR_SOLICITATION_DELAY and also the number
of experiments carried out in the test bed (30 experimental runs) compared to number of simulation
runs (100 runs). Obviously, the seed in linux based MIPL 1.0 generates lower values in small number
of runs compared to |ns3|. Hence the difference is pronounced in table :ref:`tab-exp-comp-1`. The
reason for low confidence interval (CI) to determine tmd , tac , and thandoff is due to the random
nature of router advertisement interval and MAX_RTR_SOLICITATION_DELAY component.

CI of a simulation parameter value ``v`` is x% means x runs (out of total 100 runs) have produced
``v`` as result

.. _tab-exp-comp-1:

.. table:: tables/tab-exp-comp-1.*
   :align: center

   Simulation Results for Handoff Delay (In Milliseconds)


* Validation against CNI-MIPv6 Testbed and xMIPv6 Simulation Model results:

F. Z. Yousaf et al. measured the MIPv6 handoff delay in both the CNI-MIPv6 test bed and xMIPv6
simulation model. The parameters they used and defined slightly differs from our definitions.
To compute handoff delay they used movement detection delay (tmd), home binding delay (Thr),
RR delay (trr) and correspondent binding delay (tcr).

Hence, the overall delay (tho) and tmd can be expressed as follows:

tho = tmd + thr + trr + tcr
and
tmd = tl2 + tsd + tDAD

where, tl2, tsd and tDAD are L2 handoff delay, subnet detection delay and DAD period respectively.
The same simulation setup and same reference RA intervals (i.e. MaxRtrAdvInterval - MinRtrAdvInterval)
as specified them:

3.0-1.0 second (test 1),
1.5-0.05 second (test 2) and
0.03-0.07 second (test 3) are used.

The comparison between the results (Table II) clearly shows that the delay components are almost same
with their results. Although a significant variation of tmd is seen in case of CNI-MIPv6 testbed in test1.
It happens because of the high value of RA intervals and also WLAN network setup in test bed and WLAN AP
configuration in |ns3| are different. So depending on |ns3| wifi specification, the
obtained value for tmd differs from their results. thr is 1.0 second more than the actual value because
HA runs DAD process (that takes 1.0 second) before acknowledging home BU. In comparing trr and tcr, there
is a high variation between testbed, OMNET++ based simulator and our simulator results. It is due to the
queuing delay. However, during home registration, the queuing delay is not seen in between AR and HA, and
so, there is no such variation in thr . The reason for low CI while computing tmd and tHO is same as discussed
previously.



