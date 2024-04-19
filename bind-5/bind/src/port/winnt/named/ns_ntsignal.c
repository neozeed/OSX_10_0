#include "port_before.h"

#include <memory.h>
#include <sys/wait.h>
#include <signal.h>
#include <syslog.h>

#include <netinet/in.h>
#include <arpa/inet.h>
#include <arpa/nameser.h>

#include <isc/eventlib.h>
#include <isc/logging.h>
#include <isc/memcluster.h>
#include <isc/dst.h>
#include <isc/list.h>

#include "port_after.h"

#include "../../../bin/named/ns_defs.h"

extern evContext ctx;

HANDLE sigMutex = NULL;
#define MUTEX_WAIT INFINITE

/* 
 * (very) simple hash into sigactions array 
 */
#define MOD 13

static struct sigaction sigactions[MOD];
static int procmask = 0;
static UINT timerID = 0;

/*
 * Signal names for signame()
 */
const char *signames[] = {
	"SIGTERM",
	"SIGINT",
	"SIGILL",
	"SIGHUP",
	"SIGUSR1",
	"SIGUSR2",
	"SIGCHLD",
	"SIGPIPE",
	"SIGXFSZ",
	"SIGKILL",
	"SIGWINCH",
	"SIGQUIT",
	"SIGALRM",
	"Invalid signal value"
};

/*
 * Return a string name for a signal number
 */
const char *signame(int sig)
{
	switch(sig)
	{
		case SIGTERM:
			return(signames[0]);
		case SIGINT:
			return(signames[1]);
		case SIGILL:
			return(signames[2]);
		case SIGHUP:
			return(signames[3]);
		case SIGUSR1:
			return(signames[4]);
		case SIGUSR2:
			return(signames[5]);
		case SIGCHLD:
			return(signames[6]);
		case SIGPIPE:
			return(signames[7]);
		case SIGXFSZ:
			return(signames[8]);
		case SIGKILL:
			return(signames[9]);
		case SIGWINCH:
			return(signames[10]);
		case SIGQUIT:
			return(signames[11]);
		case SIGALRM:
			return(signames[12]);
		default:
			return(signames[13]);
	}
}

/*
 * Initialize signals
 * Guard signals with a mutex.
 */
void siginit()
{
	int i;

	sigMutex = CreateMutex(NULL, TRUE, "Signal Mutex");

	procmask = 0;
	memset(sigactions, 0, sizeof(struct sigaction) * MOD);

	ReleaseMutex(sigMutex);
}	

/*
 * Clean up signals
 */
void sigcleanup()
{
	CloseHandle(sigMutex);
}

/*
 * Terminate a process
 */
int kill(pid_t pid, int signal)
{
	if(signal == SIGKILL)
	{
		TerminateProcess((HANDLE)pid, 0);
		return 0;
	}
	return -1;
}


static void CALLBACK TimerCallback(HWND hWnd, UINT nMsg, UINT nIDEvent, DWORD dwTime)
{
	KillTimer(NULL, timerID);
	timerID = 0;
	sigraise(SIGALRM);
}


/*
 * Raise SIGALRM after t seconds
 */
int alarm(int t)
{
	int tt = t * 1000;

	/* t = 0 means cancel pending alarm */
	if(t == 0 && timerID > 0)
	{
		KillTimer(NULL, timerID);
		return(0);

	}
	
	/* New alarm call supercedes old countdown */
	if(timerID > 0)
		KillTimer(NULL, timerID);

	timerID = SetTimer(NULL, 0, tt, TimerCallback);
	return((timerID > 0) ? t : -1);
}

/*
 * Invoke a signal handler
 */
int sigraise(int signal)
{
	DWORD waitObj;
	int rc = -1;
	
	/* Wait for the Mutex */
	waitObj = WaitForSingleObject(sigMutex, MUTEX_WAIT);

	/* don't raise if signal is blocked */
	if(!(procmask & signal))
	{
		if(!sigactions[signal % MOD].sa_handler)
		{
			ns_debug(ns_log_default, 9, "Signal handler not defined for %s\n", signame(signal));
		}
		else
		{
			sigactions[signal % MOD].sa_handler();
			rc = 0;
		}
	}
	ReleaseMutex(sigMutex);
	return(rc);
}

/*
 * sigemptyset(), sigfillset(), sigaddset()
 * operations on sigset_t
 */
int sigemptyset(sigset_t *set)
{
	set->sigbits = 0;
	return 0;
}

int sigfillset(sigset_t *set)
{
	set->sigbits |= SIGINT | SIGILL | SIGUSR1 | SIGUSR2 | SIGHUP | SIGCHLD | SIGPIPE | SIGTERM | SIGXFSZ;
	return 0;
}

int sigaddset(sigset_t *set, int signal)
{
	set->sigbits |= signal;
	return 0;
}	

/*
 * Associate a signal handler with a signal
 */
int sigaction(int sig, const struct sigaction *act, struct sigaction *oact)
{
	DWORD waitObj;
	int rc = -1;
	
	if(sigMutex == NULL)
		siginit();

	/* Wait for the Mutex */
	waitObj = WaitForSingleObject(sigMutex, MUTEX_WAIT);

	if(oact)
	{
		memcpy(oact, &sigactions[sig % MOD], sizeof(struct sigaction));
		rc = 0;
	}
	if(act)
	{
		int i = sig % MOD;
		memcpy(&sigactions[i], act, sizeof(struct sigaction));
		ns_debug(ns_log_default, 9, "Set sigaction for %s to %p in index %d\n", signame(sig), sigactions[sig % MOD].sa_handler, i);
		rc = 0;
	}
	ReleaseMutex(sigMutex);
	return(rc);
}

/*
 * Manipulate the signal procmask
 */
int sigprocmask(int how, const sigset_t *set, sigset_t *oset)
{
	DWORD waitObj;

	if(sigMutex == NULL)
		siginit();

	/* Wait for the Mutex */
	waitObj = WaitForSingleObject(sigMutex, MUTEX_WAIT);
	
	if(oset)
	{
		/* Caller wants to know current procmask */
		oset->sigbits = procmask;
	}
	if(set)
	{
		/* Caller is changing current procmask */
		if(how == SIG_BLOCK)
			procmask |= set->sigbits;
		else
		{
			if(how == SIG_UNBLOCK)
				procmask &= ~set->sigbits;
			else /* how == SIG_SETMASK */
				procmask = set->sigbits;
		}
	}
	ReleaseMutex(sigMutex);
	return(0);
}
