#include "port_before.h"

#include <sys/socket.h>
#include <errno.h>
#include <stdio.h>

#include "port_after.h"

static HINSTANCE winsockDLLHandle;

/*
 * sockets are file descriptors in UNIX.  This is not so in NT
 * We must keep track of what is a socket and what is an FD to
 * make everything flow right.
 */

/*
 * A list of the sockets currently open
 */
sockdata sockset[REAL_FD_SETSIZE];
int sockdescrip[REAL_FD_SETSIZE];
int sockcount = 0;

/*
 * unistd.c - open file descriptor list
 */
extern int fdset[FOPEN_MAX];

/*
 * Initialize for FD tracking, ioctl()
 */
void socketInit()
{
	int i;
	for(i=0; i < REAL_FD_SETSIZE; i++)
	{
		sockset[i].sock = 0;
		sockset[i].flags = 0;
		sockdescrip[i] = 0;
	}
}


/*
 * Add a socket to the list.  Request can get the index
 */
int sockin(SOCKET s)
{
	int i = 0;
	while(sockset[i].sock > 0)
		i++;

	if(i < REAL_FD_SETSIZE)
	{
		sockset[i].sock = s;
		sockcount++;
	}
	return i;
}

/*
 * Remove a socket from the list
 */
void sockout(SOCKET s)
{
	int i = 0;
	while(sockset[i].sock != s && i < REAL_FD_SETSIZE)
		i++;

	if(i < REAL_FD_SETSIZE)
	{
		sockset[i].sock = 0;
		sockcount--;
	}
}

/*
 * The only flags that could be set are O_APPEND, O_NONBLOCK, O_SYNC, O_ASYNC.
 * For now we only will support O_NONBLOCK as it's the only one BIND uses.
 */
int setsockflags(SOCKET s, int flag)
{
	int i;
	int fl;
	/* Extract the NON_BLOCKING flag */
	fl = flag & PORT_NONBLOCK;
	ioctlsocket(s, FIONBIO, &fl);
	
/* We're not supporting any other flags right now as they are not in the code.
 * We'll just store the requested flags.
 */

	i = 0;
	while(sockset[i].sock != s && i < REAL_FD_SETSIZE)
		i++;

	if(i < REAL_FD_SETSIZE)
	{
 		sockset[i].flags = flag;
		return(0);
	}
	else 
	{
		i = sockin(s);
		if(i < REAL_FD_SETSIZE)
		{
			sockset[i].flags = flag;
			return(0);
		}
	}
	return(-1);		//Never happens unless full
}

int getsockflags(SOCKET s)
{
	int i = 0;
	while(sockset[i].sock != s && i < REAL_FD_SETSIZE)
		i++;

	if(i < REAL_FD_SETSIZE)
	 	return(sockset[i].flags);
	else
	{
		i = sockin(s);
	}
	return(0);		// No flags anyway
}

/*
 * Note: The only FD flags that Unix supports is the FD_CLOEXEC flag
 * It is 0 - don't close-on-exec or 1 = do close-on-exec.
 * In either case this is meaningless to Windows NT.  We'll set the
 * value in our internal structure, but really do nothing with it.
 */
int setsockdescrip(SOCKET s, int flag)
{
	int i = 0;
	while(sockset[i].sock != s && i < REAL_FD_SETSIZE)
		i++;

	if(i < REAL_FD_SETSIZE)
	{
 		sockdescrip[i] = flag;
		return(0);
	}
	else 
	{
		i = sockin(s);
		if(i < REAL_FD_SETSIZE)
		{
			sockdescrip[i] = flag;
			return(0);
		}
	}
	return(-1);		//Never happens unless full
}

int getsockdescrip(SOCKET s)
{
	int i = 0;
	while(sockset[i].sock != s && i < REAL_FD_SETSIZE)
		i++;

	if(i < REAL_FD_SETSIZE)
	 	return(sockdescrip[i]);
	else
	{
		i = sockin(s);
	}
	return(0);		// No flags anyway
}

/*
 * Check to see whether fd is a socket or an actual FD
 */
int S_ISSOCK(int fd)
{
	int i = 0;
	SOCKET s = 0;

	/* Search the socket list */
	while(sockset[i].sock != fd && i < REAL_FD_SETSIZE)
		i++;

	/* It's in there */
	if(i < REAL_FD_SETSIZE)
		s = sockset[i].sock;

	/* Could have a socket and fd with the same number */
	/* In this case, take the time to check definitively */
	if(s > 0 && fd < FOPEN_MAX && fdset[fd])
	{
		/* Try EOF */
		if(_eof(fd) == -1)
			if(errno == EBADF)
				/* EOF says not a valid fd, so we must be a socket */
				return(TRUE);
	}
	/*
	 * Determine if we have a socket
	 * Woe if you get here and it is not actually a socket!
	 */
	return(s > 0);
}

/*
 * Holds flags, sets block/nonblock for sockets
 * not much else
 */

int fcntlsocket(int fd, int cmd, u_long arg)
{
	int rc = -1;
	

	switch(cmd)
	{
		/* Set status flags */
		case F_SETFL:
            rc = setsockflags(fd, arg);
			break;
		/* Get status flags */
		case F_GETFL:
            rc = getsockflags(fd);
			break;
		/* Set FD flags */
		case F_SETFD:
            rc = setsockdescrip(fd, arg);
			break;
		/* Get FD flags */
		case F_GETFD:
            rc = getsockdescrip(fd);
 			break;
		default:
			break;
	}
	return rc;
}


/*
 * Holds flags, sets block/nonblock for sockets
 * not much else
 */

/*
 * Shutdown socket services
 */
void SocketShutdown()
{
	p_WSACleanup();
	FreeLibrary(winsockDLLHandle);
}

/*
 * Initialize socket services
 */
BOOL InitSockets()
{
	WORD wVersionRequested;
	WSADATA wsaData;
	int i, err;

	winsockDLLHandle = LoadLibrary("ws2_32.dll");
	p_WSACleanup = GetProcAddress(winsockDLLHandle, "WSACleanup");
	p_WSAStartup = GetProcAddress(winsockDLLHandle, "WSAStartup");

	/* Need Winsock 2.0 or better */
	wVersionRequested = MAKEWORD(2, 0);
 
	err = p_WSAStartup(wVersionRequested, &wsaData);
	if ( err != 0 )
	{
		/* Tell the user that we could not find a usable Winsock DLL */
		return(FALSE);
	}
 
	/* 
	 * Confirm that the WinSock DLL supports 2.0
	 * Note that if the DLL supports versions greater
 	 * than 2.2 in addition to 2.2, it will still return
	 * 2.2 in wVersion since that is the version we
	 * requested.                                 
	 */
 	if ( LOBYTE( wsaData.wVersion ) != 2 || HIBYTE( wsaData.wVersion ) != 0 )
	{
		/* Tell the user that we could not find a usable Winsock DLL */
		p_WSACleanup();
		return(FALSE); 
	}
 
	socketInit();  //Initialize storage structure above.

	/* Pointers to socket functions imported by us */
	p_ioctlsocket = GetProcAddress(winsockDLLHandle, "ioctlsocket");
	p_closesocket = GetProcAddress(winsockDLLHandle, "closesocket");
	p_setsockopt = GetProcAddress(winsockDLLHandle, "setsockopt");
	p_getsockopt = GetProcAddress(winsockDLLHandle, "getsockopt");

	p_recv = GetProcAddress(winsockDLLHandle, "recv");
	p_recvfrom = GetProcAddress(winsockDLLHandle, "recvfrom");
	p_send = GetProcAddress(winsockDLLHandle, "send");
	p_sendto = GetProcAddress(winsockDLLHandle, "sendto");

	p_shutdown = GetProcAddress(winsockDLLHandle, "shutdown");
	p_socket = GetProcAddress(winsockDLLHandle, "socket");
	p_select = GetProcAddress(winsockDLLHandle, "select");
	p_connect = GetProcAddress(winsockDLLHandle, "connect");
	p_bind = GetProcAddress(winsockDLLHandle, "bind");
	p_accept = GetProcAddress(winsockDLLHandle, "accept");
	p_listen = GetProcAddress(winsockDLLHandle, "listen");

	p_ntohl = GetProcAddress(winsockDLLHandle, "ntohl");
	p_htonl = GetProcAddress(winsockDLLHandle, "htonl");
	p_htons = GetProcAddress(winsockDLLHandle, "htons");
	p_ntohs = GetProcAddress(winsockDLLHandle, "ntohs");

	p_getpeername = GetProcAddress(winsockDLLHandle, "getpeername");
	p_getsockname = GetProcAddress(winsockDLLHandle, "getsockname");

	p_inet_addr = GetProcAddress(winsockDLLHandle, "inet_addr");
	p_inet_ntoa = GetProcAddress(winsockDLLHandle, "inet_ntoa");

	p_gethostbyaddr = GetProcAddress(winsockDLLHandle, "gethostbyaddr");
	p_gethostbyname = GetProcAddress(winsockDLLHandle, "inet_addr"); 
	p_gethostname = GetProcAddress(winsockDLLHandle, "gethostname");
	p_getprotobyname = GetProcAddress(winsockDLLHandle, "getprotobyname");
	p_getprotobynumber = GetProcAddress(winsockDLLHandle, "getprotobynumber");
	p_getservbyname = GetProcAddress(winsockDLLHandle, "getservbyname");
	p_getservbyport = GetProcAddress(winsockDLLHandle, "getservbyport");
	 
 	p___WSAFDIsSet = GetProcAddress(winsockDLLHandle, "__WSAFDIsSet"); 
	p_WSAIoctl = GetProcAddress(winsockDLLHandle, "WSAIoctl");

	/* Winsock functions not used by us */

/*
     p_WSAGetLastError = GetProcAddress(winsockDLLHandle, "");
	 p_WSAAccept = GetProcAddress(winsockDLLHandle, "");
	 p_WSAAsyncSelect = GetProcAddress(winsockDLLHandle, "");
	 p_WSACancelBlockingCall = GetProcAddress(winsockDLLHandle, "");
	 p_WSACloseEvent = GetProcAddress(winsockDLLHandle, "");
	 p_WSAConnect = GetProcAddress(winsockDLLHandle, "");
	 p_WSACreateEvent = GetProcAddress(winsockDLLHandle, "");
	 p_WSADuplicateSocketA = GetProcAddress(winsockDLLHandle, "");
	 p_WSAEnumNetworkEvents = GetProcAddress(winsockDLLHandle, "");
	 p_WSAEnumProtocolsA = GetProcAddress(winsockDLLHandle, "");
	 p_WSAEventSelect = GetProcAddress(winsockDLLHandle, "");
	 p_WSAGetOverlappedResult = GetProcAddress(winsockDLLHandle, "");
	 p_WSAGetQOSByName = GetProcAddress(winsockDLLHandle, "");
	 p_WSAHtonl = GetProcAddress(winsockDLLHandle, "");
	 p_WSAHtons = GetProcAddress(winsockDLLHandle, "");
	 p_WSAIsBlocking = GetProcAddress(winsockDLLHandle, "");
	 p_WSAJoinLeaf = GetProcAddress(winsockDLLHandle, "");
	 p_WSANtohl = GetProcAddress(winsockDLLHandle, "");
	 p_WSANtohs = GetProcAddress(winsockDLLHandle, "");
	 p_WSARecv = GetProcAddress(winsockDLLHandle, "");
	 p_WSARecvDisconnect = GetProcAddress(winsockDLLHandle, "");
	 p_WSARecvFrom = GetProcAddress(winsockDLLHandle, "");
	 p_WSAResetEvent = GetProcAddress(winsockDLLHandle, "");
	 p_WSASend = GetProcAddress(winsockDLLHandle, "");
	 p_WSASendDisconnect = GetProcAddress(winsockDLLHandle, "");
	 p_WSASendTo = GetProcAddress(winsockDLLHandle, "");
	 p_WSASetBlockingHook = GetProcAddress(winsockDLLHandle, "");
	 p_WSASetEvent = GetProcAddress(winsockDLLHandle, "");
	 p_WSASetLastError = GetProcAddress(winsockDLLHandle, "");
	 p_WSASocketA = GetProcAddress(winsockDLLHandle, "");
	 p_WSAUnhookBlockingHook = GetProcAddress(winsockDLLHandle, "");
	 p_WSAWaitForMultipleEvents = GetProcAddress(winsockDLLHandle, "");
	 p_WSAAsyncGetHostByAddr = GetProcAddress(winsockDLLHandle, "");
	 p_WSAAsyncGetHostByName = GetProcAddress(winsockDLLHandle, "");
	 p_WSAAsyncGetProtoByName = GetProcAddress(winsockDLLHandle, "");
	 p_WSAAsyncGetProtoByNumber = GetProcAddress(winsockDLLHandle, "");
	 p_WSAAsyncGetServByName = GetProcAddress(winsockDLLHandle, "");
	 p_WSAAsyncGetServByPort = GetProcAddress(winsockDLLHandle, "");
	 p_WSACancelAsyncRequest = GetProcAddress(winsockDLLHandle, "");
	 p_WSPAccept = GetProcAddress(winsockDLLHandle, "");
	 p_WSPAsyncSelect = GetProcAddress(winsockDLLHandle, "");
	 p_WSPBind = GetProcAddress(winsockDLLHandle, "");
	 p_WSPCancelBlockingCall = GetProcAddress(winsockDLLHandle, "");
	 p_WSPCleanup = GetProcAddress(winsockDLLHandle, "");
	 p_WSPCloseSocket = GetProcAddress(winsockDLLHandle, "");
	 p_WSPConnect = GetProcAddress(winsockDLLHandle, "");
	 p_WSPDuplicateSocket = GetProcAddress(winsockDLLHandle, "");
	 p_WSPEnumNetworkEvents = GetProcAddress(winsockDLLHandle, "");
	 p_WSPEventSelect = GetProcAddress(winsockDLLHandle, "");
	 p_WSPGetOverlappedResult = GetProcAddress(winsockDLLHandle, "");
	 p_WSPGetPeerName = GetProcAddress(winsockDLLHandle, "");
	 p_WSPGetSockName = GetProcAddress(winsockDLLHandle, "");
	 p_WSPGetSockOpt = GetProcAddress(winsockDLLHandle, "");
	 p_WSPGetQOSByName = GetProcAddress(winsockDLLHandle, "");
	 p_WSPIoctl = GetProcAddress(winsockDLLHandle, "");
	 p_WSPJoinLeaf = GetProcAddress(winsockDLLHandle, "");
	 p_WSPListen = GetProcAddress(winsockDLLHandle, "");
	 p_WSPRecv = GetProcAddress(winsockDLLHandle, "");
	 p_WSPRecvDisconnect = GetProcAddress(winsockDLLHandle, "");
	 p_WSPRecvFrom = GetProcAddress(winsockDLLHandle, "");
	 p_WSPSelect = GetProcAddress(winsockDLLHandle, "");
	 p_WSPSend = GetProcAddress(winsockDLLHandle, "");
	 p_WSPSendDisconnect = GetProcAddress(winsockDLLHandle, "");
	 p_WSPSendTo = GetProcAddress(winsockDLLHandle, "");
	 p_WSPSetSockOpt = GetProcAddress(winsockDLLHandle, "");
	 p_WSPShutdown = GetProcAddress(winsockDLLHandle, "");
	 p_WSPSocket = GetProcAddress(winsockDLLHandle, "");
	 p_WSPStartup = GetProcAddress(winsockDLLHandle, "");
	 p_WPUCloseEvent = GetProcAddress(winsockDLLHandle, "");
	 p_WPUCloseSocketHandle = GetProcAddress(winsockDLLHandle, "");
	 p_WPUCreateEvent = GetProcAddress(winsockDLLHandle, "");
	 p_WPUCreateSocketHandle = GetProcAddress(winsockDLLHandle, "");
	 p_WSCDeinstallProvider = GetProcAddress(winsockDLLHandle, "");
	 p_WSCInstallProvider = GetProcAddress(winsockDLLHandle, "");
	 p_WPUModifyIFSHandle = GetProcAddress(winsockDLLHandle, "");
	 p_WPUQueryBlockingCallback = GetProcAddress(winsockDLLHandle, "");
	 p_WPUQuerySocketHandleContext = GetProcAddress(winsockDLLHandle, "");
	 p_WPUQueueApc = GetProcAddress(winsockDLLHandle, "");
	 p_WPUResetEvent = GetProcAddress(winsockDLLHandle, "");
	 p_WPUSetEvent = GetProcAddress(winsockDLLHandle, "");
	 p_WSCEnumProtocols = GetProcAddress(winsockDLLHandle, "");
	 p_WPUGetProviderPath = GetProcAddress(winsockDLLHandle, "");
	 p_WPUPostMessage = GetProcAddress(winsockDLLHandle, "");
	 p_WPUFDIsSet = GetProcAddress(winsockDLLHandle, "");
	 p_WSADuplicateSocketW = GetProcAddress(winsockDLLHandle, "");
	 p_WSAEnumProtocolsW = GetProcAddress(winsockDLLHandle, "");
	 p_WSASocketW = GetProcAddress(winsockDLLHandle, "");
	 p_WSAAddressToStringA = GetProcAddress(winsockDLLHandle, "");
	 p_WSAAddressToStringW = GetProcAddress(winsockDLLHandle, "");
	 p_WSAStringToAddressA = GetProcAddress(winsockDLLHandle, "");
	 p_WSAStringToAddressW = GetProcAddress(winsockDLLHandle, "");
	 p_WSALookupServiceBeginA = GetProcAddress(winsockDLLHandle, "");
	 p_WSALookupServiceBeginW = GetProcAddress(winsockDLLHandle, "");
	 p_WSALookupServiceNextA = GetProcAddress(winsockDLLHandle, "");
	 p_WSALookupServiceNextW = GetProcAddress(winsockDLLHandle, "");
	 p_WSALookupServiceEnd = GetProcAddress(winsockDLLHandle, "");
	 p_WSAGetProcAddressByNameA = GetProcAddress(winsockDLLHandle, "");
	 p_WSAGetProcAddressByNameW = GetProcAddress(winsockDLLHandle, "");
	 p_WSAInstallServiceClassA = GetProcAddress(winsockDLLHandle, "");
	 p_WSAInstallServiceClassW = GetProcAddress(winsockDLLHandle, "");
	 p_WSASetServiceA = GetProcAddress(winsockDLLHandle, "");
	 p_WSASetServiceW = GetProcAddress(winsockDLLHandle, "");
	 p_WSARemoveServiceClass = GetProcAddress(winsockDLLHandle, "");
	 p_WSAGetServiceClassInfoA = GetProcAddress(winsockDLLHandle, "");
	 p_WSAGetServiceClassInfoW = GetProcAddress(winsockDLLHandle, "");
	 p_WSAEnumNameSpaceProvidersA = GetProcAddress(winsockDLLHandle, "");
	 p_WSAEnumNameSpaceProvidersW = GetProcAddress(winsockDLLHandle, "");
	 p_WSAGetServiceClassNameByClassIdA = GetProcAddress(winsockDLLHandle, "");
	 p_WSAGetServiceClassNameByClassIdW = GetProcAddress(winsockDLLHandle, "");
	 p_WSPAddressToString = GetProcAddress(winsockDLLHandle, "");
	 p_WSPStringToAddress = GetProcAddress(winsockDLLHandle, "");
	 p_NSPLookupServiceBegin = GetProcAddress(winsockDLLHandle, "");
	 p_NSPLookupServiceNext = GetProcAddress(winsockDLLHandle, "");
	 p_NSPLookupServiceEnd = GetProcAddress(winsockDLLHandle, "");
	 p_NSPGetProcAddressByName = GetProcAddress(winsockDLLHandle, "");
	 p_NSPInstallServiceClass = GetProcAddress(winsockDLLHandle, "");
	 p_NSPSetService = GetProcAddress(winsockDLLHandle, "");
	 p_NSPRemoveServiceClass = GetProcAddress(winsockDLLHandle, "");
	 p_NSPGetServiceClassInfo = GetProcAddress(winsockDLLHandle, "");
	 p_NSPEnumNameSpaceProviders = GetProcAddress(winsockDLLHandle, "");
	 p_NSPGetServiceClassNameByClassId = GetProcAddress(winsockDLLHandle, "");
*/
	return(TRUE);
}

/*
 * Function wrappers - call through to Winsock via (p_xxx)()
 * Could use #defines, but that's not type safe, 
 * and some functions need to do other things, too.
 */
int WSAAPI bind(SOCKET s,const struct sockaddr FAR * name,int namelen)
{return(p_bind)(s,name,namelen);}

int WSAAPI ioctlsocket(SOCKET s,long cmd,u_long FAR* argp)
{return(p_ioctlsocket)(s,cmd,argp);}
 
int WSAAPI closesocket(SOCKET s)
{
      if(sockcount > 0)
      {
              sockout(s);
              return(p_closesocket)(s);
      }
      else
      {
              errno = EBADF;
              return(-1);
      }

}

int WSAAPI setsockopt(SOCKET s,int level,int optname,const char FAR * optval,int optlen)
{return(p_setsockopt)(s,level,optname,optval,optlen);}

int WSAAPI getsockopt(SOCKET s,int level,int optname,char FAR * optval,int FAR * optlen)
{return(p_getsockopt)(s,level,optname,optval,optlen);}

int WSAAPI recv(SOCKET s,char FAR * buf,int len,int flags)
{return(p_recv)(s,buf,len,flags);}

int WSAAPI recvfrom(SOCKET s,char FAR * buf,int len,int flags,struct sockaddr FAR * from,int FAR * fromlen)
{
	int rc = (p_recvfrom)(s,buf,len,flags,from,fromlen);
	if(rc == -1 && GetLastError() == WSAEWOULDBLOCK)
		errno = EWOULDBLOCK;	
	return(rc);
}

int WSAAPI send(SOCKET s,const char FAR * buf,int len,int flags)
{return(p_send)(s,buf,len,flags);}

int WSAAPI sendto(SOCKET s,const char FAR * buf,int len,int flags,const struct sockaddr FAR * to,int tolen)
{return(p_sendto)(s,buf,len,flags,to,tolen);}

int WSAAPI shutdown(SOCKET s,int how)
{return(p_shutdown)(s, how);}

SOCKET WSAAPI socket(int af,int type,int protocol)
{
	if(af == AF_UNIX)
	{
		errno = ECONNREFUSED;
		return((SOCKET)-1);
	}

      /* Don't allow more sockets than an fd_set can hold */
      if(sockcount == REAL_FD_SETSIZE)
      {
              errno = EMFILE;
              return((SOCKET)-1);
      }
      else

	{
              SOCKET s = (p_socket)(af,type,protocol);
              if(s != SOCKET_ERROR)
              {
                      sockin(s);
                      return s;
              }
	}
	errno = GetLastError();
	return((SOCKET)-1);
}

int WSAAPI select(int nfds,fd_set FAR * readfds,fd_set FAR * writefds,fd_set FAR *exceptfds,const struct timeval FAR * timeout)
{return(p_select)(nfds,readfds,writefds,exceptfds,timeout);}

int WSAAPI connect(SOCKET s,const struct sockaddr FAR * name,int namelen)
{return(p_connect)(s,name,namelen);}

SOCKET WSAAPI accept(SOCKET s,struct sockaddr FAR * addr,int FAR * addrlen)
{
      if(sockcount == REAL_FD_SETSIZE)
      {
              errno = EMFILE;
              return((SOCKET)-1);
      }
	s = (p_accept)(s,addr,addrlen);
	if(s != SOCKET_ERROR)
		sockin(s);
	return s;
}

int WSAAPI listen(SOCKET s,int backlog)
{return(p_listen)(s,backlog);}

u_long WSAAPI ntohl(u_long netlong)
{return(p_ntohl)(netlong);}

u_long WSAAPI htonl(u_long hostlong)
{return(p_htonl)(hostlong);}

u_short WSAAPI htons(u_short hostshort)
{return(p_htons)(hostshort);}

u_short WSAAPI ntohs(u_short netshort)
{return(p_ntohs)(netshort);}

int WSAAPI getpeername(SOCKET s,struct sockaddr FAR * name,int FAR * namelen)
{return(p_getpeername)(s,name,namelen);}

int WSAAPI getsockname(SOCKET s,struct sockaddr FAR * name,int FAR * namelen)
{return(p_getsockname)(s,name,namelen);}

unsigned long WSAAPI inet_addr(const char FAR * cp)
{return(p_inet_addr)(cp);}

char FAR * WSAAPI inet_ntoa(struct in_addr in)
{return(char FAR *)(p_inet_ntoa)(in);}

int WSAAPI gethostname(char FAR * name,int namelen)
{return(p_gethostname)(name, namelen);}

int WSAAPI __WSAFDIsSet(SOCKET s, fd_set FAR *fds)
{return(p___WSAFDIsSet)(s,fds);}
