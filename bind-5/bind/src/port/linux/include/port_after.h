#ifndef	PORT_AFTER_H
#define	PORT_AFTER_H
#define USE_POSIX
#define POSIX_SIGNALS
#define USE_UTIME
#define USE_WAITPID
#define HAVE_GETRUSAGE
#define HAVE_FCHMOD
#define USE_SETSID
#define SETGRENT_VOID
#define SIOCGIFCONF_ADDR
#define USE_LOG_CONS
#define HAVE_CHROOT
#define CAN_CHANGE_ID

#if __GLIBC__ >= 2
#  define dprintf bind_dprintf
#  if __GLIBC_MINOR__ < 1
#    define NEED_PSELECT
#    define pselect bind_pselect
#  endif
#else
#  define NEED_PSELECT
#  define NEED_DAEMON
int daemon(int nochdir, int noclose);
#endif

#define SPURIOUS_ECONNREFUSED	/* XXX is this still needed for 2.0 kernels? */
#define _TIMEZONE timezone

#define PORT_NONBLOCK	O_NONBLOCK
#define PORT_WOULDBLK	EAGAIN
#define WAIT_T		int

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

#include <port_ipv6.h>

/*
 * We need to know the IPv6 address family number even on IPv4-only systems.
 * Note that this is NOT a protocol constant, and that if the system has its
 * own AF_INET6, different from ours below, all of BIND's libraries and
 * executables will need to be recompiled after the system <sys/socket.h>
 * has had this type added.  The type number below is correct for Linux
 * systems.
 */
#ifndef AF_INET6
#define AF_INET6	10
#endif

#ifndef	PF_INET6
#define PF_INET6	AF_INET6
#endif

#ifndef HAS_INET6_STRUCTS
/* Replace with structure from later rev of O/S if known. */
struct in6_addr {
	u_int8_t	s6_addr[16];
};

/* Replace with structure from later rev of O/S if known. */
struct sockaddr_in6 {
#ifdef	HAVE_SA_LEN
	u_int8_t	sin6_len;	/* length of this struct */
	u_int8_t	sin6_family;	/* AF_INET6 */
#else
	u_int16_t	sin6_family;	/* AF_INET6 */
#endif
	u_int16_t	sin6_port;	/* transport layer port # */
	u_int32_t	sin6_flowinfo;	/* IPv6 flow information */
	struct in6_addr	sin6_addr;	/* IPv6 address */
	u_int32_t	sin6_scope_id;	/* set of interfaces for a scope */
};
#endif	/* HAS_INET6_STRUCTS */

/*
 * glibc 2.2 #defines strndup to __strndup, which allows this to work.
 */
#ifdef strndup
#define HAVE_STRNDUP
#endif

#ifndef ISC_FACILITY
#define ISC_FACILITY LOG_DAEMON
#endif

#endif /* ! PORT_AFTER_H */
