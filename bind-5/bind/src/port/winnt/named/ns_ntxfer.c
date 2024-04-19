#include "port_before.h"

#include <sys/wait.h>

#include <netinet/in.h>
#include <arpa/inet.h>
#include <arpa/nameser.h>

#include <errno.h>
#include <signal.h>
#include <resolv.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <syslog.h>
#include <time.h>
#include <unistd.h>

#include <isc/eventlib.h>
#include <isc/logging.h>
#include <isc/memcluster.h>
#include <isc/dst.h>
#include <isc/list.h>
#include <res_update.h>

#include "../../../bin/named/ns_defs.h"
#include "../../../bin/named/ns_glob.h"

#include "port_after.h"

#include <winbase.h>

/*
 * Forward Declarations 
 */
DWORD WINAPI named_xfer(LPVOID param);

/*
 * Keep track of the status of spawned xfers
 */
struct _xferstatus {
	HANDLE pid;
	DWORD status;
} xfers[MAX_XFERS_RUNNING];

/*
 * Use this to pass needed info to the spawned thread for xfer
 */
struct xferInfo {
	char xferProg[MAX_PATH];
	char xferArgs[256];
	int index;
};

/*
 * Spawn a thread that will execute named-xfer
 * The thread will wait for named-xfer to finish,
 * then report the status back to named
 */
pid_t spawnxfer(char *const *argv, struct zoneinfo *zp)
{
	HANDLE hThread = NULL;
	DWORD dwID = 0;

	int i = 0, j;
	char *pos, *cp;

	/* Freed by the spawned thread */
	struct xferInfo *args = (struct xferInfo *)malloc(sizeof(struct xferInfo));

	/* need the arguments in the form of a string not in argv format */
	pos = args->xferArgs;

	/* Xfer program location
	 * Enclose in quotes in case we're in
	 * Program Files or something
	 */
	memset(args->xferProg, 0, MAX_PATH);
	args->xferProg[0] = '"';
	strcpy(args->xferProg, argv[0]);
	args->xferProg[strlen(argv[0]) + 1] = '"';

	/*
	 * The rest of the arguments in a string form
	 * for CreateProcess()
	 */
	while(argv[i] != NULL)
	{
		/*
		 * argv[0] will be "C:\...\named-xfer.exe
		 * named-xfer is going to strrchr(argv[0], "/")
		 * to get just the executable name for its call
		 * to openlog().  We'll convert the string to
		 * "C:/.../named-xfer" for two reasons:
		 * 1) To make xfer happy
		 * 2) We register named-xfer as 'named-xfer'
		 *    for event logging, so we want the call
		 *    to openlog() to work correctly.
		 *
		if(i == 0)
		{
			do {
				cp = strchr(argv[0], '\\');
				if(cp)
					*cp = '/';
			} while(cp);
			cp = strstr(argv[0], ".exe");
			j = cp - argv[0];
			if(j > 0)
				argv[0][j] = '\0';
   		}
		*/
		sprintf(pos, argv[i]);
		pos += strlen(pos);
		*pos++ = ' ';
		i++;
	}
	*pos = '\0';

	/* Get a spot in the status array for this xfer */
	for(i = 0; i < MAX_XFERS_RUNNING; i++)
	{
		if(xfers[i].pid == NULL)
		{
			args->index = i;
			break;
		}
	}

	hThread = CreateThread(NULL, 0, named_xfer, (LPVOID)args, 0, &dwID);
	if(!hThread)
	{
		DWORD err = GetLastError();
		ns_debug(ns_log_default, 1, "spawnxfer: CreateThread() failed(%d)\n", err);
		free(args);
		return(-1);
	}
	
	xfers[i].pid = hThread;
	xfers[i].status = STILL_ACTIVE;
	
	return((pid_t)hThread);
}


/*
 * Spawn named-xfer using CreateProcess(), then
 * wait for it to finish, and report the results back
 */
DWORD WINAPI named_xfer(LPVOID param)
{
	DWORD dwWait;
	DWORD err;
	DWORD dwFlags = CREATE_NO_WINDOW;

	BOOL rc;
	int index = 0;
	STARTUPINFO sui;
	struct xferInfo *args = (struct xferInfo *)param;
	PROCESS_INFORMATION pi;

	memset(&sui, 0, sizeof(STARTUPINFO));
	sui.cb = sizeof(STARTUPINFO);
	rc = CreateProcess(args->xferProg, args->xferArgs, NULL, NULL, FALSE,
			   0, NULL, server_options->directory, &sui, &pi);

	index = args->index;
	free(args);

	/* Process did not start */
	if(!rc)
	{
		DWORD err = GetLastError();
		ns_debug(ns_log_default, 1, "named_xfer: CreateProcess() failed(%d)\n", err);
		return(XFER_FAIL);
	}

	/* Wait for named-xfer to finish */
	dwWait = WaitForSingleObject(pi.hProcess, (DWORD) MAX_XFER_TIME * 1000);

    /* now check reason we woke up */
    /* remove the return now so let it go into endxfer even if the wait fails so we can resched. the xfer */
	switch (dwWait)
	{
		case WAIT_TIMEOUT:
			ns_debug(ns_log_default, 1, "named_xfer: WaitForSingleObject(xfer child %d) timed out\n", pi.hProcess);
			break;
		case WAIT_FAILED:
			err = GetLastError();
			ns_debug(ns_log_default, 1, "named_xfer: WaitForSingleObject(xfer child %d) failed(%d)\n", pi.hProcess, err);
			break;
		case WAIT_OBJECT_0:
			/* All done - signal main program to reapchild() */
			GetExitCodeProcess(pi.hProcess, &xfers[index].status);
			CloseHandle(pi.hProcess);
			CloseHandle(pi.hThread);
			sigraise(SIGCHLD);
			break;
		default:
			break;
	}
	return(0);
}

/*
 * Called by reapchild() int ns_maint.c
 * emulate UNIX waitpid() for named
 */
int waitpid(pid_t pid, int *stat_loc, int options)
{
	int i;
	pid_t rc = 0;
	BOOL finished = FALSE;
	
	do
	{
		/* only check once if WNOHANG */
		if(options == WNOHANG)
			finished = TRUE;

		for(i = 0; i < MAX_XFERS_RUNNING; i++)
		{
			/* -1 indicates any child process, nonzero is a specific process */
			if(pid == -1 || (HANDLE)pid == xfers[i].pid)
			{
				/* Check the state of the process */
				if(xfers[i].pid && xfers[i].status != STILL_ACTIVE)
				{	
					finished = TRUE;
					if(stat_loc)
					{
					/* Put the exit code in the high byte of the high word */
						*stat_loc = xfers[i].status;
						*stat_loc <<= 24;
						/* Set the status */
						*stat_loc |= WAIT_EXITED;
					}
					/* Clear this process from the record */
					rc = (pid_t)xfers[i].pid;
					xfers[i].pid = NULL;
					ns_debug(ns_log_default, 15, "waitpid() returns %d", rc);
					/* We want to return the pid of this process */
					return(rc);
				}
			}
		}
	} while(!finished);
	return(rc);
}
