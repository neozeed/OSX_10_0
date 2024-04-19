#ifndef _SIGNAL_H
#define _SIGNAL_H 1

#include <signal.h>

#define POSIX_SIGNALS

#undef SIGINT
#undef SIGILL
#undef SIGTERM

#define SIGINT		0x0002
#define SIGILL		0x0004
#define SIGTERM		0x0008
#define SIGUSR1		0x0010
#define SIGUSR2		0x0020
#define SIGCHLD		0x0040
#define SIGPIPE		0x0080
#define SIGXFSZ		0x0100
#define SIGKILL		0x0200
#define SIGWINCH	0x0400
#define SIGQUIT		0x0800
#define SIGALRM		0x1000
#define SIGHUP		0x2000

#define SIG_COUNT	13

#define SIG_BLOCK	0x001
#define SIG_UNBLOCK	0x002
#define SIG_SETMASK	0x004
/* #define SIG_IGN		0x005 */

#define SIG_FN void

typedef struct
{
	/* signal set type */
	unsigned long sigbits;
} sigset_t;

struct sigaction
{
	int sa_flags;
	SIG_FN (*sa_handler)();
	sigset_t sa_mask;
	int sa_resv[2];
};                                                                     

void siginit();
void sigcleanup();

int sigraise(int);
int sigaddset(sigset_t *set, int signal);
int sigfillset(sigset_t *set);
int sigemptyset(sigset_t *);
int sigaction(int, const struct sigaction *, struct sigaction *);
int sigprocmask(int, const sigset_t *, sigset_t *);
int alarm(int);

typedef long pid_t;
int kill(pid_t, int);
int waitpid(pid_t, int *, int);

void *pSignals;
#endif
