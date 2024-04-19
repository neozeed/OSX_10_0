#include "port_before.h"

#include <net/if.h>

#include "port_after.h"


/*
 * Forward Declarations
 */
int GetInterfacesWSAIoctl(struct ifdata *IFData[], int *nextIndex, int MaxIF);
//int GetInterfacesSNMP(struct ifdata *IFData[]);

/*
 * Enumerate the TCP/IP interfaces on this machine.
 *
 * GetInterfacesXXXX() will malloc elements in the IFData array
 * It is the caller's responsibility to free this memory
 */
int GetInterfaces(struct ifdata *IFData[], int MaxIF)
{

	/* Next available index on interface list */
	int nextIndex = 0;
	int numInterfaces = 0;  /* IP v4 */
	int numInterfaces6 = 0;	/* IP v6 */

	/* ioctl_if.c */
	/* This is for the IPv4 interfaces 
     * gets ALL IP v4 interfaces on the machine - including virtual IF */

	numInterfaces = GetInterfacesWSAIoctl(IFData, &nextIndex, MaxIF);

	/* This is for the IPv6 interfaces
	 * gets ALL IP v6 interfaces on the machine - including virtual IF
	 * This should return nothing if there is no IP v6 stack on the machine.
	 */
	numInterfaces6 = GetInterfaces6WSAIoctl(IFData, &nextIndex, MaxIF - nextIndex);
	
	return (numInterfaces + numInterfaces6);

}
