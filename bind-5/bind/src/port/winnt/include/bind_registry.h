#ifndef _BIND_REGISTRY_H
#define _BIND_REGISTRY_H

#define BIND_SUBKEY				"Software\\ISC\\BIND"
#define BIND_SESSION			"CurrentSession"
#define BIND_SESSION_SUBKEY		"Software\\ISC\\BIND\\CurrentSession"
#define BIND_UNINSTALL_SUBKEY	"Software\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\ISC BIND"

#define EVENTLOG_APP_SUBKEY		"SYSTEM\\CurrentControlSet\\Services\\EventLog\\Application"
#define BIND_MESSAGE_SUBKEY		"SYSTEM\\CurrentControlSet\\Services\\EventLog\\Application\\named"
#define XFER_MESSAGE_SUBKEY		"SYSTEM\\CurrentControlSet\\Services\\EventLog\\Application\\named-xfer"
#define BIND_MESSAGE_NAME		"named"
#define XFER_MESSAGE_NAME		"named-xfer"

#define BIND_CONFIGFILE	0
#define BIND_DEBUGLEVEL	1
#define BIND_QUERYLOG	2
#define BIND_FOREGROUND	3
#define BIND_PORT		4

#endif