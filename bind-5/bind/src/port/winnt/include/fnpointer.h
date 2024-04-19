#ifndef _FNPOINTER_H
#define _FNPOINTER_H

/* Pointers to the functions in the Mib DLL */

typedef BOOL (WINAPI *pSnmpExtensionInit)(DWORD               dwTimeZeroReference,
										  HANDLE              *hPollForTrapEvent,
										  AsnObjectIdentifier *supportedView);

typedef BOOL (WINAPI *pSnmpExtensionTrap)(AsnObjectIdentifier *enterprise,
										  AsnInteger          *genericTrap,
										  AsnInteger          *specificTrap,
										  AsnTimeticks        *timeStamp,
										  RFC1157VarBindList  *variableBindings);

typedef BOOL (WINAPI *pSnmpExtensionQuery)(BYTE                   requestType,
										   OUT RFC1157VarBindList *variableBindings,
										   AsnInteger            *errorStatus,
										   AsnInteger            *errorIndex);

typedef BOOL (WINAPI *pSnmpExtensionInitEx)(AsnObjectIdentifier *supportedView);

#endif