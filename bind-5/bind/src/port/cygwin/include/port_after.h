#ifndef	PORT_AFTER_H
#define	PORT_AFTER_H
#define BSD 44
#define USE_POSIX
#define POSIX_SIGNALS
#define USE_WAITPID
#define SPRINTF_INT
#define VSPRINTF_INT
#define __BIND_RES_TEXT
#define SIOCGIFCONF_ADDR
#define GET_HOST_ID_MISSING
#define BROKEN_RECVFROM
#define _PATH_NDCSOCK	"127.0.0.1/101"

#define PORT_NONBLOCK	O_NDELAY
#define PORT_WOULDBLK	EWOULDBLOCK
#define WAIT_T		int
#ifndef PATH_MAX
#define PATH_MAX	_POSIX_PATH_MAX
#endif
#define INADDR_NONE	0xffffffff

/* #define _TIMEZONE timezone */

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/param.h>
#if (!defined(BSD)) || (BSD < 199306)
#include <sys/bitypes.h>
#endif

#ifndef MIN
# define MIN(x, y)	((x > y) ?y :x)
#endif
#ifndef MAX
# define MAX(x, y)	((x > y) ?x :y)
#endif

#define NEED_PSELECT

#define NEED_DAEMON
int daemon(int nochdir, int noclose);

/* SunOS provides vsprintf but doesn't prototype it. */
#include <stdarg.h>

#ifndef RAND_MAX
#define RAND_MAX 0x7fffffff
#endif

/*
 * We need to know the IPv6 address family number even on IPv4-only systems.
 * Note that this is NOT a protocol constant, and that if the system has its
 * own AF_INET6, different from ours below, all of BIND's libraries and
 * executables will need to be recompiled after the system <sys/socket.h>
 * has had this type added.  The type number below is correct on most BSD-
 * derived systems for which AF_INET6 is defined.
 */
#ifndef AF_INET6
#define AF_INET6	24
#endif

#ifndef	PF_INET6
#define PF_INET6	AF_INET6
#endif

/* Replace with structure from later rev of O/S if known. */
struct in6_addr {
	u_int8_t	s6_addr[16];
};

/* Cygwin provides this but there is no include file that prototypes it. */
int getopt(int, char **, char *);
extern char *optarg;
extern int optind, opterr;

#ifndef ISC_FACILITY
#define ISC_FACILITY LOG_DAEMON
#endif

#endif /* ! PORT_AFTER_H */
