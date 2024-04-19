#include "port_before.h"

#include <unistd.h>
#include <signal.h>
#include <direct.h>
#include <syslog.h>

#include <isc/eventlib.h>
#include <isc/list.h>
#include <isc/assertions.h>
#include <isc/logging.h>

#include <errno.h>
#include <arpa/nameser.h>
#include <arpa/nameser_compat.h>
#include <res_update.h>

#include "../../../bin/named/ns_defs.h"
#include "../../../bin/named/ns_glob.h"
#include "../../../bin/named/ns_func.h"

#include <windows.h>
#include <signal.h>
#include <bind_service.h>
#include <bind_registry.h>

#include "port_after.h"

/*
 * from ns_main.c
 */
extern evContext ev;
extern int foreground;
extern int debug;
extern int qrylog;
extern char *conffile;
 

/*
 * Handle Ctrl+C
 */
void break_handler(int sig)
{
	ns_debug(ns_log_default, 1, "Ctrl+C interrupt - Shutting down named");
	/* Ignore any other Ctrl+C presses */
	signal(SIGINT, SIG_IGN);
	/* Shut down */
	sigraise(SIGTERM);
}

/*
 * Initialize things that allow named to run on NT
 */
void custom_init()
{
	/* We'll piggyback the logging for the port areas in the DLL with eventlib */
	InitNTLogging(log_get_stream(eventlib_channel), debug);

	/* For foreground run, we'll intercept Ctrl+C and handle it to shut down gracefully */
	if(foreground)
		signal(SIGINT, break_handler);

	/* Register handler with the SCM */
	if(!foreground)
	{
		hServiceStatus = RegisterServiceCtrlHandler(BIND_SERVICE_NAME, (LPHANDLER_FUNCTION)ServiceControl);
		if(!hServiceStatus)
		{
			ns_debug(ns_log_default, 1, "Could not register service control handler");
			UpdateSCM(SERVICE_STOPPED);
			exit(1);
		}
		UpdateSCM(SERVICE_RUNNING);
	}
	else
	{
		/* Running in foreground - set the console window title */ 
//		SetConsoleTitle(BIND_WINDOW_TITLE);
		SetConsoleTitle(ShortVersion);
	}

	_chdir(_PATH_ETC);
	unistdInit();
}

/*
 * Clean up things that need it
 */
void custom_shutdown()
{
	unistdCleanup();
	sigcleanup();
}

int daemon(int param1, int param2)
{
	return(0);
}

void SetRegItem(int key, char *pszKey, DWORD nKey)
{
	HKEY hKey;
	
	if(RegCreateKey(HKEY_LOCAL_MACHINE, BIND_SESSION_SUBKEY, &hKey) != ERROR_SUCCESS)
		return;

	switch(key)
	{
		case BIND_CONFIGFILE:
			RegSetValueEx(hKey, "ConfigFile", 0, REG_SZ, (LPBYTE)pszKey, strlen(pszKey));
			break;
		case BIND_DEBUGLEVEL:
			RegSetValueEx(hKey, "DebugLevel", 0, REG_DWORD, (LPBYTE)&nKey, sizeof(DWORD));
			break;
		case BIND_QUERYLOG:
			RegSetValueEx(hKey, "QueryLog", 0, REG_DWORD, (LPBYTE)&nKey, sizeof(DWORD));
			break;
		case BIND_FOREGROUND:
			RegSetValueEx(hKey, "Foreground", 0, REG_DWORD, (LPBYTE)&nKey, sizeof(DWORD));
			break;
		case BIND_PORT:
			RegSetValueEx(hKey, "MonitorPort", 0, REG_DWORD, (LPBYTE)&nKey, sizeof(DWORD));
			break;
	}
	RegCloseKey(hKey);
}

void CleanRegistry()
{
	HKEY hKey;
	DWORD dwTemp = 0;
	
	if(RegOpenKey(HKEY_LOCAL_MACHINE, BIND_SUBKEY, &hKey) != ERROR_SUCCESS)
		return;

	RegDeleteKey(hKey, BIND_SESSION);
	RegCloseKey(hKey);
}

int InitRegistry()
{
	SetRegItem(BIND_CONFIGFILE, conffile, 0);
	SetRegItem(BIND_DEBUGLEVEL, NULL, debug);
	SetRegItem(BIND_QUERYLOG, NULL, qrylog);
	SetRegItem(BIND_FOREGROUND, NULL, foreground);
	SetRegItem(BIND_PORT, NULL, ntohs(ns_port));
	return(0);
}
