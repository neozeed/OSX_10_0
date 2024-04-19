#include "port_before.h"

#include <syslog.h>
#include <sys/socket.h>
#include <net/if.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "port_after.h"

/* We define this here to make this fixed array size easy
 * to expand as needed. It should not really be done as an
 * array, but it will be sufficient for now.
 */
#define MAX_NT_IF 300
/*
 * Enumerate the TCP/IP interfaces on this machine.
 * Use WSAIoctl() to do this - note that this often
 * does not enumerate the loopback interface.
 *
 * It is the caller's responsibility to free elements
 * of IFData malloc'd by us.
 *
 * For a large number of Interfaces we have upped the array
 * from 20 to 300.  This is a workaround for now.  We should
 * really be dynamically figuring out how many Interfaces there
 * are and allocating the space required.  For now we have
 * hardcoded the number of possible interfaces and hope we don't
 * run out in case some is foolish enough to add too many addresses.
 * This should be fixed to check and retry.
 */
int GetInterfacesWSAIoctl(struct ifdata *IFData[], int *nextIndex, int maxIF)
{
    INTERFACE_INFO InterfaceList[MAX_NT_IF];
	SOCKET s;
    unsigned long bytesReturned, flags;
	int numInterfaces, i;
	int totalInterfaces;
    struct sockaddr_in* pAddress;
	struct in_addr addr, *dest;
	struct ifdata *ifd;
	BOOL ifNamed;

	ntPrintf(6, "Enumerating interfaces via WSAIoctl() - Loopback IF may be missed\n");

	s = socket(AF_INET, SOCK_DGRAM, 0);
    if (s == SOCKET_ERROR)
	{
		ntPrintf(1, "Failed to get a socket.  Error %d\n", GetLastError());
        return 0;
    }

    if (p_WSAIoctl(s, SIO_GET_INTERFACE_LIST, 0, 0, &InterfaceList, sizeof(InterfaceList), &bytesReturned, 0, 0) == SOCKET_ERROR)
	{
		ntPrintf(1, "Failed calling WSAIoctl:  Error %d\n", GetLastError());
        closesocket(s);
        return 0;
    }
    closesocket(s);

    numInterfaces = bytesReturned / sizeof(INTERFACE_INFO);
	totalInterfaces = numInterfaces;
	if (numInterfaces > maxIF) {
		ntPrintf(1, "Maximum Number of Interfaces exceeded. Found %d. Using first %d\n",
					numInterfaces, maxIF);
		numInterfaces = maxIF;
	}

	if(numInterfaces > 0)
	{
		for (i = totalInterfaces -1; i >= (totalInterfaces - numInterfaces); i--)
		{
			ifNamed = FALSE;

			ifd = (struct ifdata *)malloc(sizeof(struct ifdata));
			memset(ifd, 0, sizeof(struct ifdata));


			pAddress = (struct sockaddr_in*)&(InterfaceList[i].iiAddress);
			dest = &((struct sockaddr_in *)&ifd->if_addr)->sin_addr;
			memcpy(dest, &pAddress->sin_addr, sizeof(struct in_addr)); 
			ifd->if_addr.sa_family = AF_INET;

			pAddress = (struct sockaddr_in*)&(InterfaceList[i].iiBroadcastAddress);
			dest = &((struct sockaddr_in *)&ifd->if_broadaddr)->sin_addr;
			memcpy(dest, &pAddress->sin_addr, sizeof(struct in_addr)); 
			ifd->if_broadaddr.sa_family = AF_INET;

			flags = InterfaceList[i].iiFlags;
			if (flags & IFF_UP)
				ifd->if_flags |= IFF_UP;
			if (flags & IFF_POINTTOPOINT)
			{
				sprintf(ifd->if_name, "PPP Interface (interface %d)", numInterfaces -i);
				ifd->if_flags |= IFF_POINTTOPOINT;
				ifNamed = TRUE;
			}
			if (flags & IFF_LOOPBACK)
			{
				sprintf(ifd->if_name, "Loopback Interface (interface %d)", numInterfaces - i);
				ifd->if_flags |= IFF_LOOPBACK;
				ifNamed = TRUE;
			}
			if (flags & IFF_BROADCAST)
				ifd->if_flags |= IFF_BROADCAST;
			if (flags & IFF_MULTICAST)
				ifd->if_flags |= IFF_MULTICAST;

			if(ifNamed == FALSE)
			{
				sprintf(ifd->if_name, "TCP/IP Interface %d", numInterfaces - i);
			}

			IFData[*nextIndex] = ifd;
			*nextIndex = *nextIndex + 1;
		}
	}
	else
	{
		ntPrintf(1, "No interfaces found!\n");
	}
    return numInterfaces;
}

/*
 * This is the IPv6 version for getting the Interfaces.  It is kept separate
 * but follows the same methodology as the IPv4 version.
 */
int GetInterfaces6WSAIoctl(struct ifdata *IFData[], int *nextIndex, int maxIF)
{
    INTERFACE_INFO InterfaceList[MAX_NT_IF];
	SOCKET s;
    unsigned long bytesReturned, flags;
	int numInterfaces, i;
	int totalInterfaces;
    struct sockaddr_in6* pAddress;
	struct in_addr6 addr, *dest;
	struct ifdata *ifd;
	BOOL ifNamed;

	ntPrintf(6, "Enumerating IP v6 interfaces via WSAIoctl() - Loopback IF may be missed\n");

	s = socket(AF_INET6, SOCK_DGRAM, 0);
    if (s == SOCKET_ERROR)
	{
		ntPrintf(1, "IPv6 socket not available.  Error %d\n", GetLastError());
        return 0;
    }

    if (p_WSAIoctl(s, SIO_GET_INTERFACE_LIST, 0, 0, &InterfaceList, sizeof(InterfaceList), &bytesReturned, 0, 0) == SOCKET_ERROR)
	{
		ntPrintf(1, "Failed calling WSAIoctl for IP v6:  Error %d\n", GetLastError());
        closesocket(s);
        return 0;
    }
    closesocket(s);

    numInterfaces = bytesReturned / sizeof(INTERFACE_INFO);
	totalInterfaces = numInterfaces;
	if (numInterfaces > maxIF) {
		ntPrintf(1, "Maximum Number of IP v6 Interfaces exceeded. Found %d. Using first %d\n",
					numInterfaces, maxIF);
		numInterfaces = maxIF;
	}

	if(numInterfaces > 0)
	{
		for (i = totalInterfaces -1; i >= (totalInterfaces - numInterfaces); i--)
		{
			ifNamed = FALSE;

			ifd = (struct ifdata *)malloc(sizeof(struct ifdata));
			memset(ifd, 0, sizeof(struct ifdata));


			pAddress = (struct sockaddr_in6*)&(InterfaceList[i].iiAddress);
			dest = &((struct sockaddr_in6 *)&ifd->if_addr)->sin6_addr;
			memcpy(dest, &pAddress->sin6_addr, sizeof(struct in_addr6)); 
			ifd->if_addr.sa_family = AF_INET6;

			pAddress = (struct sockaddr_in6*)&(InterfaceList[i].iiBroadcastAddress);
			dest = &((struct sockaddr_in6 *)&ifd->if_broadaddr)->sin6_addr;
			memcpy(dest, &pAddress->sin6_addr, sizeof(struct in_addr6)); 
			ifd->if_broadaddr.sa_family = AF_INET6;

			flags = InterfaceList[i].iiFlags;
			if (flags & IFF_UP)
				ifd->if_flags |= IFF_UP;
			if (flags & IFF_POINTTOPOINT)
			{
				sprintf(ifd->if_name, "PPP Interface (interface %d)", numInterfaces - i);
				ifd->if_flags |= IFF_POINTTOPOINT;
				ifNamed = TRUE;
			}
			if (flags & IFF_LOOPBACK)
			{
				sprintf(ifd->if_name, "Loopback Interface (interface %d)", numInterfaces - i);
				ifd->if_flags |= IFF_LOOPBACK;
				ifNamed = TRUE;
			}
			if (flags & IFF_BROADCAST)
				ifd->if_flags |= IFF_BROADCAST;
			if (flags & IFF_MULTICAST)
				ifd->if_flags |= IFF_MULTICAST;

			if(ifNamed == FALSE)
			{
				sprintf(ifd->if_name, "TCP/IP Interface %d", numInterfaces - i);
			}

			IFData[*nextIndex] = ifd;
			*nextIndex = *nextIndex + 1;
		}
	}
	else
	{
		ntPrintf(1, "No IP v6 interfaces found!\n");
	}
    return numInterfaces;
}
