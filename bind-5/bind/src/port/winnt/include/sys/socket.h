#ifndef SOCKET_H
#define SOCKET_H 1

/* Winsock will give us the prototypes for all the socket functions */
#include "port_before.h"
#include "port_after.h"

typedef struct _sockdata {
	SOCKET sock;
	int flags;
} sockdata;

/* Handle to Wsock32.dll */
extern HINSTANCE winsockDLLHandle;

/*These get redefined in netdb.h */
#undef HOST_NOT_FOUND
#undef TRY_AGAIN
#undef NO_RECOVERY
#undef NO_DATA
#undef NO_ADDRESS

/* errno values not included by Windows */
#define EADDRNOTAVAIL	126
#define EOPNOTSUPP		122

BOOL InitSockets();
void SocketShutdown();
int S_ISSOCK(int fd);

/* Winsock functions */
PROC p_accept;
PROC p_bind;
PROC p_closesocket;
PROC p_connect;
PROC p_getpeername;
PROC p_getsockname;
PROC p_getsockopt;
PROC p_htonl;
PROC p_htons;
PROC p_ioctlsocket;
PROC p_listen;
PROC p_ntohl;
PROC p_ntohs;
PROC p_recv;
PROC p_recvfrom;
PROC p_select;
PROC p_send;
PROC p_sendto;
PROC p_setsockopt;
PROC p_shutdown;
PROC p_socket;
PROC p_gethostbyaddr;
PROC p_gethostbyname;
PROC p_gethostname;
PROC p_getprotobyname;
PROC p_getprotobynumber;
PROC p_getservbyname;
PROC p_getservbyport;
PROC p_gethostbyaddr;
PROC p_gethostbyname;
PROC p_gethostname;
PROC p_getprotobyname;
PROC p_getprotobynumber;
PROC p_getservbyname;
PROC p_getservbyport;
PROC p_inet_addr;
PROC p_inet_ntoa;

PROC p_WSAAccept;
PROC p_WSAAsyncSelect;
PROC p_WSACancelBlockingCall;
PROC p_WSACleanup;
PROC p_WSACloseEvent;
PROC p_WSAConnect;
PROC p_WSACreateEvent;
PROC p_WSADuplicateSocketA;
PROC p_WSAEnumNetworkEvents;
PROC p_WSAEnumProtocolsA;
PROC p_WSAEventSelect;
PROC p_WSAGetLastError;
PROC p_WSAGetOverlappedResult;
PROC p_WSAGetQOSByName;
PROC p_WSAHtonl;
PROC p_WSAHtons;
PROC p_WSAIoctl;
PROC p_WSAIsBlocking;
PROC p_WSAJoinLeaf;
PROC p_WSANtohl;
PROC p_WSANtohs;
PROC p_WSARecv;
PROC p_WSARecvDisconnect;
PROC p_WSARecvFrom;
PROC p_WSAResetEvent;
PROC p_WSASend;
PROC p_WSASendDisconnect;
PROC p_WSASendTo;
PROC p_WSASetBlockingHook;
PROC p_WSASetEvent;
PROC p_WSASetLastError;
PROC p_WSASocketA;
PROC p_WSAStartup;
PROC p_WSAUnhookBlockingHook;
PROC p_WSAWaitForMultipleEvents;
PROC p_WSAAsyncGetHostByAddr;
PROC p_WSAAsyncGetHostByName;
PROC p_WSAAsyncGetProtoByName;
PROC p_WSAAsyncGetProtoByNumber;
PROC p_WSAAsyncGetServByName;
PROC p_WSAAsyncGetServByPort;
PROC p_WSACancelAsyncRequest;
PROC p_WSPAccept;
PROC p_WSPAsyncSelect;
PROC p_WSPBind;
PROC p_WSPCancelBlockingCall;
PROC p_WSPCleanup;
PROC p_WSPCloseSocket;
PROC p_WSPConnect;
PROC p_WSPDuplicateSocket;
PROC p_WSPEnumNetworkEvents;
PROC p_WSPEventSelect;
PROC p_WSPGetOverlappedResult;
PROC p_WSPGetPeerName;
// The  WSPGetProcTable  function  has  been removed, but the code numbers have
// been kept the same.
// PROC p_WSPGetProcTable 85
PROC p_WSPGetSockName;
PROC p_WSPGetSockOpt;
PROC p_WSPGetQOSByName;
PROC p_WSPIoctl;
PROC p_WSPJoinLeaf;
PROC p_WSPListen;
PROC p_WSPRecv;
PROC p_WSPRecvDisconnect;
PROC p_WSPRecvFrom;
PROC p_WSPSelect;
PROC p_WSPSend;
PROC p_WSPSendDisconnect;
PROC p_WSPSendTo;
PROC p_WSPSetSockOpt;
PROC p_WSPShutdown;
PROC p_WSPSocket;
PROC p_WSPStartup;
PROC p_WPUCloseEvent;
PROC p_WPUCloseSocketHandle;
PROC p_WPUCreateEvent;
PROC p_WPUCreateSocketHandle;
PROC p_WSCDeinstallProvider;
PROC p_WSCInstallProvider;
PROC p_WPUModifyIFSHandle;
PROC p_WPUQueryBlockingCallback;
PROC p_WPUQuerySocketHandleContext;
PROC p_WPUQueueApc;
PROC p_WPUResetEvent;
PROC p_WPUSetEvent;
PROC p_WSCEnumProtocols;
PROC p_WPUGetProviderPath;
PROC p_WPUPostMessage;
PROC p_WPUFDIsSet;
PROC p_WSADuplicateSocketW;
PROC p_WSAEnumProtocolsW;
PROC p_WSASocketW;
PROC p___WSAFDIsSet;
PROC p_WSAAddressToStringA;
PROC p_WSAAddressToStringW;
PROC p_WSAStringToAddressA;
PROC p_WSAStringToAddressW;
PROC p_WSALookupServiceBeginA;
PROC p_WSALookupServiceBeginW;
PROC p_WSALookupServiceNextA;
PROC p_WSALookupServiceNextW;
PROC p_WSALookupServiceEnd;
PROC p_WSAGetAddressByNameA;
PROC p_WSAGetAddressByNameW;
PROC p_WSAInstallServiceClassA;
PROC p_WSAInstallServiceClassW;
PROC p_WSASetServiceA;
PROC p_WSASetServiceW;
PROC p_WSARemoveServiceClass;
PROC p_WSAGetServiceClassInfoA;
PROC p_WSAGetServiceClassInfoW;
PROC p_WSAEnumNameSpaceProvidersA;
PROC p_WSAEnumNameSpaceProvidersW;
PROC p_WSAGetServiceClassNameByClassIdA;
PROC p_WSAGetServiceClassNameByClassIdW;
PROC p_WSPAddressToString;
PROC p_WSPStringToAddress;
PROC p_NSPLookupServiceBegin;
PROC p_NSPLookupServiceNext;
PROC p_NSPLookupServiceEnd;
PROC p_NSPGetAddressByName;
PROC p_NSPInstallServiceClass;
PROC p_NSPSetService;
PROC p_NSPRemoveServiceClass;
PROC p_NSPGetServiceClassInfo;
PROC p_NSPEnumNameSpaceProviders;
PROC p_NSPGetServiceClassNameByClassId;

#endif

