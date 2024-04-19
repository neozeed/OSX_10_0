#ifndef OID_H
#define OID_H

#define OIDSize(x) sizeof(x)/sizeof(UINT)

/*OIDs for SNMP MIB*/

/* Number of interfaces present */
static UINT ifNumber[]			= {1,3,6,1,2,1,2,1};

/* For OID 1.3.6.1.2.1.2.2.1.3 */
#define IF_LOOPBACK 24
#define IF_PPP		23

/* For OID 1.3.6.1.2.1.2.2.1.8 */
#define IF_UP		1
#define IF_DOWN		2
#define IF_TESTING	3

/* Interface Table */
static UINT ifIndex[]			= {1,3,6,1,2,1,2,2,1,1};
static UINT ifDescr[]			= {1,3,6,1,2,1,2,2,1,2};
static UINT ifType[]			= {1,3,6,1,2,1,2,2,1,3};
static UINT ifMtu[]				= {1,3,6,1,2,1,2,2,1,4};
static UINT ifSpeed[]			= {1,3,6,1,2,1,2,2,1,5};
static UINT ifPhysAddress[]		= {1,3,6,1,2,1,2,2,1,6};
static UINT ifAdminStatus[]		= {1,3,6,1,2,1,2,2,1,7};
static UINT ifOperStatus[]		= {1,3,6,1,2,1,2,2,1,8};
static UINT ifLastChange[]		= {1,3,6,1,2,1,2,2,1,9};
static UINT ifInOctets[]		= {1,3,6,1,2,1,2,2,1,10};
static UINT ifInUcastPkts[]		= {1,3,6,1,2,1,2,2,1,11};
static UINT ifInNUcastPkts[]	= {1,3,6,1,2,1,2,2,1,12};
static UINT ifInDiscards[]		= {1,3,6,1,2,1,2,2,1,13};
static UINT ifInErrors[]		= {1,3,6,1,2,1,2,2,1,14};
static UINT ifInUnknownProtos[]	= {1,3,6,1,2,1,2,2,1,15};
static UINT ifOutOctets[]		= {1,3,6,1,2,1,2,2,1,16};
static UINT ifOutUcastPkts[]	= {1,3,6,1,2,1,2,2,1,17};
static UINT ifOutNUcastPkts[]	= {1,3,6,1,2,1,2,2,1,18};
static UINT ifOutDiscards[]		= {1,3,6,1,2,1,2,2,1,19};
static UINT ifOutErrors[]		= {1,3,6,1,2,1,2,2,1,20};
static UINT ifOutQLen[]			= {1,3,6,1,2,1,2,2,1,21};
static UINT ifSpecific[]		= {1,3,6,1,2,1,2,2,1,22};

/* IP */
static UINT ipForwarding[]		= {1,3,6,1,2,1,4,1}; 
static UINT ipDefaultTTL[]		= {1,3,6,1,2,1,4,2}; 
static UINT ipInReceives[]		= {1,3,6,1,2,1,4,3}; 
static UINT ipInHdrErrors[]		= {1,3,6,1,2,1,4,4}; 
static UINT ipInAddrErrors[]	= {1,3,6,1,2,1,4,5}; 
static UINT ipForwDatagrams[]	= {1,3,6,1,2,1,4,6}; 
static UINT ipInUnknownProtos[]	= {1,3,6,1,2,1,4,7}; 
static UINT ipInDiscards[]		= {1,3,6,1,2,1,4,8}; 
static UINT ipInDelivers[]		= {1,3,6,1,2,1,4,9}; 
static UINT ipOutRequests[]		= {1,3,6,1,2,1,4,10}; 
static UINT ipOutDiscards[]		= {1,3,6,1,2,1,4,11}; 
static UINT ipOutNoRoutes[]		= {1,3,6,1,2,1,4,12}; 
static UINT ipReasmTimeout[]	= {1,3,6,1,2,1,4,13}; 
static UINT ipReasmReqds[]		= {1,3,6,1,2,1,4,14}; 
static UINT ipReasmOKs[]		= {1,3,6,1,2,1,4,15}; 
static UINT ipReasmFails[]		= {1,3,6,1,2,1,4,16}; 
static UINT ipFragOKs[]			= {1,3,6,1,2,1,4,17}; 
static UINT ipFragFails[]		= {1,3,6,1,2,1,4,18}; 
static UINT ipFragCreates[]		= {1,3,6,1,2,1,4,19}; 
static UINT ipRoutingDiscards[]	= {1,3,6,1,2,1,4,23}; 

/* IP Address Table */
static UINT ipAdEntAddr[]			= {1,3,6,1,2,1,4,20,1,1};
static UINT ipAdEntIfIndex[]		= {1,3,6,1,2,1,4,20,1,2};
static UINT ipAdEntNetMask[]		= {1,3,6,1,2,1,4,20,1,3};
static UINT ipAdEntBcastAddr[]		= {1,3,6,1,2,1,4,20,1,4};
static UINT ipAdEntReasmMaxSize[]	= {1,3,6,1,2,1,4,20,1,5};

/* TCP Connection Table */
static UINT tcpConnState[]			= {1,3,6,1,2,1,6,13,1,1};
static UINT tcpConnLocalAddress[]	= {1,3,6,1,2,1,6,13,1,2};
static UINT tcpConnLocalPort[]		= {1,3,6,1,2,1,6,13,1,3};
static UINT tcpConnRemAddress[]		= {1,3,6,1,2,1,6,13,1,4};
static UINT tcpConnRemPort[]		= {1,3,6,1,2,1,6,13,1,5};

/* IP Net to Media Table */
static UINT ipNetToMediaIfIndex[]		= {1,3,6,1,2,1,4,22,1,1};
static UINT ipNetToMediaPhysAddress[]	= {1,3,6,1,2,1,4,22,1,2};
static UINT ipNetToMediaNetAddress[]	= {1,3,6,1,2,1,4,22,1,3};
static UINT ipNetToMediaType[]			= {1,3,6,1,2,1,4,22,1,4};

#endif
