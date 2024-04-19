#define WANT_IRS_NIS
#undef WANT_IRS_PW
#undef WANT_IRS_GR
#define SIG_FN void
#ifndef _POSIX_SOURCE
#define _POSIX_SOURCE 
#endif
#define BIG_ENDIAN	4321
#define BYTE_ORDER	BIG_ENDIAN

#if defined(HAS_PTHREADS) && defined(_REENTRANT)
#define DO_PTHREADS
#endif

#include <sys/types.h>

#include <limits.h>	/* _POSIX_PATH_MAX */
#include <sys/times.h>
#include <sys/time.h>
struct timespec {
	time_t	tv_sec;		/* seconds */
	long	tv_nsec;	/* nanoseconds */
};

