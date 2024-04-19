#ifdef WIN32

#include "port_before.h"

#include <windows.h>
#include <bind_service.h>
#include <bind_registry.h>
#include <syslog.h>
#include <signal.h>

#include <errno.h>
#include <arpa/nameser.h>
#include <arpa/nameser_compat.h>
#include <res_update.h>

#include <isc/eventlib.h>
#include <isc/list.h>
#include <isc/assertions.h>
#include <isc/logging.h>

#include "../../../bin/named/ns_defs.h"
#include "../../../bin/named/ns_glob.h"

#include "port_after.h"

/*
 * Forward declarations
 */
void ServiceControl(DWORD dwCtrlCode);
void GetArgs(int *, char ***, char ***);
int main(int, char *[], char *[]); /* From ns_main.c */

/*
 * From ns_glob.h
 */
extern int desired_debug;
extern int qrylog;

/*
 * Here we change the entry point for the executable to bindmain() from main()
 * This allows us to invoke as a service or from the command line easily.
 */
#pragma comment(linker, "/entry:bindmain")

/*
 * This is the entry point for the executable 
 * We can now call main() explicitly or via StartServiceCtrlDispatcher()
 * as we need to.
 */
int bindmain()
{
	int rc,
		foreground = FALSE,
		i = 1;

	int	argc;
	char **envp, **argv;

	/* We changed the entry point function, so we must initialize argv, etc. ourselves.  Ick. */
	GetArgs(&argc, &argv, &envp);

	/* Command line users should put -f in the options */
	while(argv[i])
	{
		if(!strcmp(argv[i], "-f") || !strcmp(argv[i], "/f"))
		{
			foreground = TRUE;
			break;
		}
		i++;
	}

	if(foreground)
	{
		/* run in console window */
		exit(main(argc, argv, envp));
	}
	else
	{
		/* Start up as service */
		char *SERVICE_NAME = BIND_SERVICE_NAME;

		SERVICE_TABLE_ENTRY dispatchTable[] =
		{
			{ TEXT(SERVICE_NAME), (LPSERVICE_MAIN_FUNCTION)main},
			{ NULL, NULL }
		};

		rc = StartServiceCtrlDispatcher(dispatchTable);
		if(!rc)
		{
			fprintf(stderr, "Use -f to run from the command line.\n");
			exit(GetLastError());
		}
	}
    exit(0);
}

/* 
 * ServiceControl(): Handles requests from the SCM and passes them on to named via signals
 */
void ServiceControl(DWORD dwCtrlCode)
{
	/* Handle the requested control code */
    switch(dwCtrlCode) {

        case SERVICE_CONTROL_INTERROGATE:
           	ns_debug(ns_log_default, 3,"Interrogated");
			/* Update the service status */
			UpdateSCM(0);
			break;

        case SERVICE_CONTROL_STOP:
			ns_debug(ns_log_default, 3, "received SIGTERM");
			sigraise(SIGTERM);
			UpdateSCM(SERVICE_STOPPED);
			break;

        case SERVICE_CONTROL_DUMPDB:
			sigraise(SIGINT);
			break;

        case SERVICE_CONTROL_RELOAD:
			sigraise(SIGHUP);
			break;

        case SERVICE_CONTROL_STATS:
			sigraise(SIGILL);
			break;

        case SERVICE_CONTROL_TRACE:
			sigraise(SIGUSR1);
			ModifyLogLevel(desired_debug);
			break;

        case SERVICE_CONTROL_NOTRACE:
			sigraise(SIGUSR2);
			ModifyLogLevel(0);
			break;

        case SERVICE_CONTROL_QRYLOG:
			SetRegItem(BIND_QUERYLOG, NULL, !qrylog);
			sigraise(SIGWINCH);
			break;

        default:
            break;
    }
}

/*
 * Tell the Service Control Manager the state of the service
 */
void UpdateSCM(DWORD state)
{
	SERVICE_STATUS ss;
	static DWORD dwState = SERVICE_STOPPED;

	if(hServiceStatus)
	{
		if(state)
			dwState = state;

		memset(&ss, 0, sizeof(SERVICE_STATUS));
		ss.dwServiceType |= SERVICE_WIN32_OWN_PROCESS;
		ss.dwCurrentState = dwState;
		ss.dwControlsAccepted = SERVICE_ACCEPT_STOP | SERVICE_ACCEPT_SHUTDOWN;
		ss.dwCheckPoint = 0;
		ss.dwServiceSpecificExitCode = 0;
		ss.dwWin32ExitCode = NO_ERROR;
		ss.dwWaitHint = dwState == SERVICE_STOP_PENDING ? 10000 : 1000;

		if (!SetServiceStatus(hServiceStatus, &ss))
		{
			ss.dwCurrentState = SERVICE_STOPPED;
			SetServiceStatus(hServiceStatus, &ss);
		}
	}
}



/*
 * C-runtime stuff used to initialize the app and
 * get argv, argc, envp
 */

typedef struct
{
	int newmode;
} _startupinfo;

_CRTIMP void __cdecl __set_app_type(int);
_CRTIMP void __cdecl __getmainargs(int *, char ***, char ***, int, _startupinfo *);
void __cdecl _setargv(void);

#ifdef _M_IX86
/* Pentium FDIV adjustment */
extern int _adjust_fdiv;
extern int * _imp___adjust_fdiv;
/* Floating point precision */
extern void _setdefaultprecision();
#endif

extern int _newmode;		/* malloc new() handler mode */
extern int _dowildcard;     /* passed to __getmainargs() */

typedef void (__cdecl *_PVFV)(void);
extern void __cdecl _initterm(_PVFV *, _PVFV *);
extern _PVFV *__onexitbegin;
extern _PVFV *__onexitend;
extern _CRTIMP char **__initenv;

/*
 * Do the work that mainCRTStartup() would normally do
 */
void GetArgs(int *argc, char ***argv, char ***envp)
{
	_startupinfo startinfo;
    
	/* Set the app type to Console (check CRT/SRC/INTERNAL.H: #define _CONSOLE_APP 1) */
	__set_app_type(1);
	
	/* Mark this module as an EXE file */
	__onexitbegin = __onexitend = (_PVFV *)(-1);

	startinfo.newmode = _newmode;
	__getmainargs(argc, argv, envp, _dowildcard, &startinfo);
    __initenv = *envp;

#ifdef _M_IX86
	_adjust_fdiv = * _imp___adjust_fdiv;
	_setdefaultprecision();
#endif
}

#endif