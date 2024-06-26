/*
 * ntp_syscall.h - various ways to perform the ntp_adjtime() and ntp_gettime()
 * 		   system calls.
 */

#ifndef NTP_SYSCALL_H
#define NTP_SYSCALL_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#ifdef HAVE_SYS_TIMEX_H
# include <sys/timex.h>
#endif

#ifdef NTP_SYSCALLS_STD
# define ntp_adjtime(t)		syscall(SYS_ntp_adjtime, (t))
# define ntp_gettime(t)		syscall(SYS_ntp_gettime, (t))
#else /* !NTP_SYSCALLS_STD */
# ifdef HAVE___ADJTIMEX
extern	int	__adjtimex	P((struct timex *));

#  define ntp_adjtime(t)	__adjtimex((t))

static inline int
ntp_gettime(
	struct ntptimeval *ntv
	)
{
	struct timex tntx;
	int result;

	tntx.modes = 0;
	result = __adjtimex (&tntx);
	ntv->time = tntx.time;
	ntv->maxerror = tntx.maxerror;
	ntv->esterror = tntx.esterror;
	return(result);
}
# else /* !HAVE__ADJTIMEX */
#  ifdef HAVE___NTP_GETTIME
#   define ntp_gettime(t)	__ntp_gettime((t))
#  endif
# endif /* !HAVE_ADJTIMEX */
#endif /* !NTP_SYSCALLS_STD */

#endif /* NTP_SYSCALL_H */
