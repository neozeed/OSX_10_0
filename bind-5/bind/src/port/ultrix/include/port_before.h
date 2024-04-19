#ifndef PORT_BEFORE_H
#define PORT_BEFORE_H

/* Hack to get the proper *_MAX definitions for comparisons */
#ifndef __STDC__
#define PORT_BEFORE_H_STDC
#define __STDC__ 1
#endif
#include <limits.h>
#ifdef PORT_BEFORE_H_STDC
#undef __STDC__
#endif

#define WANT_IRS_NIS
#undef WANT_IRS_PW
#undef WANT_IRS_GR
#define SIG_FN void

#include <time.h>
struct timespec {
	time_t	tv_sec;		/* seconds */
	long	tv_nsec;	/* nanoseconds */
};

#if defined(HAS_PTHREADS) && defined(_REENTRANT)
#define DO_PTHREADS
#endif

#define LITTLE_ENDIAN	1234
#define BYTE_ORDER	LITTLE_ENDIAN

#endif /*PORT_BEFORE_H*/
