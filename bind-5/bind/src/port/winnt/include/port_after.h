#ifndef _PORT_AFTER_H
#define _PORT_AFTER_H

#include <pathnames.h>

/* This is a reserved keyword for COM.  BIND wants to use it. */
#undef interface

/*
 * Define fnctl() for NT
 */
#include <nt_fcntl.h>
/*
 * Signal handling
 */
#include <nt_signal.h>
/*
 * _chdir()
 */
#include <direct.h>
/*
 * strerror() et. al.
 */
#include <string.h>

/*
 * This is only used in conjunction with AF_UNIX, but we define it
 * so the compiler shuts up
 */
struct sockaddr_un {
	short int sun_family;
	char sun_path[108];
};

/*
 * Winsock defines this struct as in_addr6.  res_mkupdate.c wants in6_addr.  Odd.
 */
struct in6_addr {
		u_char	s6_addr[16];	/* IPv6 address */
};

/* From Solaris 2.5 sys/netinet/in.h */
#define IN_EXPERIMENTAL(i) (((long)(i) & 0xe0000000) == 0xe0000000)

/* NT event log does not support facility level */
#define LOG_KERN	0
#define LOG_USER	0
#define LOG_MAIL	0
#define LOG_DAEMON	0
#define LOG_AUTH	0
#define LOG_SYSLOG	0
#define LOG_LPR		0
#define LOG_LOCAL0	0
#define LOG_LOCAL1	0
#define LOG_LOCAL2	0
#define LOG_LOCAL3	0
#define LOG_LOCAL4	0
#define LOG_LOCAL5	0
#define LOG_LOCAL6	0
#define LOG_LOCAL7	0

#define LOG_EMERG       0       /* system is unusable */
#define LOG_ALERT       1       /* action must be taken immediately */
#define LOG_CRIT        2       /* critical conditions */
#define LOG_ERR         3       /* error conditions */
#define LOG_WARNING     4       /* warning conditions */
#define LOG_NOTICE      5       /* normal but signification condition */
#define LOG_INFO        6       /* informational */
#define LOG_DEBUG       7       /* debug-level messages */

# define STDIN_FILENO	0
# define STDOUT_FILENO	1
# define STDERR_FILENO	2

#define EADDRINUSE		WSAEADDRINUSE
#define EMSGSIZE		WSAEMSGSIZE
#define EWOULDBLOCK		WSAEWOULDBLOCK
#define EAFNOSUPPORT	WSAEAFNOSUPPORT
#define ECONNREFUSED	WSAECONNREFUSED
#define ETIMEDOUT		WSAETIMEDOUT
#define ECONNRESET		WSAECONNRESET
#define EINPROGRESS		WSAEINPROGRESS
#define ECONNABORTED	WSAECONNABORTED
#define EHOSTUNREACH	WSAEHOSTUNREACH
#define EHOSTDOWN		WSAEHOSTDOWN
#define ENETUNREACH		WSAENETUNREACH
#define ENETDOWN		WSAENETDOWN
#define ENOTSOCK		WSAENOTSOCK
#define ENOTCONN		WSAENOTCONN
#define ETOOMANYREFS	WSAETOOMANYREFS
#define EPROTOTYPE		WSAEPROTOTYPE
#define EDESTADDRREQ	WSAEDESTADDRREQ

/* setitimer does not exist in NT */
#define NEED_SETITIMER

/* NT has no nice - just make dummy */
#define nice(x)

#define WAIT_T int

#define NEED_STRSEP
extern char *strsep(char **stringp, const char *delim);

#define NEED_READV
#define readv __readv
extern int __readv(int, const struct iovec*, int);

#define NEED_WRITEV
#define writev __writev
extern int __writev(int, const struct iovec*, int);

#define NEED_GETTIMEOFDAY
extern int gettimeofday(struct timeval *tvp, struct _TIMEZONE *tzp);

#define NEED_UTIMES
#define utimes __utimes
int __utimes(char *, struct timeval *);

#ifndef MAXHOSTNAMELEN
#define MAXHOSTNAMELEN 256
#endif

#ifndef MIN
# define MIN(x, y)	((x > y) ?y :x)
#endif
#ifndef MAX
# define MAX(x, y)	((x > y) ?x :y)
#endif

/* open() under unix allows setting of read/write permissions
 * at the owner, group and other levels.  These don't exist in NT
 * We'll just map them all to the NT equivalent 
 */

#define S_IREAD	_S_IREAD	/* read permission, owner */
#define S_IWRITE _S_IWRITE	/* write permission, owner */
#define S_IRUSR _S_IREAD	/* Owner read permission */
#define S_IWUSR _S_IWRITE	/* Owner write permission */
#define S_IRGRP _S_IREAD	/* Group read permission */
#define S_IWGRP _S_IWRITE	/* Group write permission */
#define S_IROTH _S_IREAD	/* Other read permission */
#define S_IWOTH _S_IWRITE	/* Other write permission */

#ifndef S_ISCHR
# ifndef S_IFMT
#  define S_IFMT 0170000
# endif
# ifndef S_IFCHR
#  define S_IFCHR 0020000
# endif
# define S_ISCHR(m)   ((m & S_IFMT) == S_IFCHR) 
#endif

#ifndef S_ISDIR
# ifndef S_IFMT
#  define S_IFMT 0170000
# endif
# ifndef S_IFDIR
#  define S_IFDIR 0040000
# endif
# define S_ISDIR(m)	((m & S_IFMT) == S_IFDIR)
#endif

#ifndef S_ISREG
# ifndef S_IFMT
#  define S_IFMT 0170000
# endif
# ifndef S_IFREG
#  define S_IFREG 0100000
# endif
# define S_ISREG(m)	((m & S_IFMT) == S_IFREG)
#endif

#ifndef S_ISFIFO
# ifndef S_IFMT
#  define S_IFMT 0170000
# endif
# ifndef S_IFIFO
#  define S_IFIFO 0010000
# endif
# define S_ISFIFO(m)	((m & S_IFMT) == S_IFIFO)
#endif

#define sleep(t) Sleep(t*1000)

/* Define these to allow dig to build properly */
#define WCOREDUMP(status)	0
#define vfork() -1
#define wait(status) -1

/* This gets used by the command parser in nslookup */

#define YY_SKIP_YYWRAP
#define yywrap() 1

#ifndef ISC_FACILITY
#define ISC_FACILITY LOG_DAEMON
#endif

#endif /* _PORT_AFTER_H */
