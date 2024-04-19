#ifndef _PORT_BEFORE_H
#define _PORT_BEFORE_H 

/*
 * An attempt to cut down the number of warnings generated during compilation.
 * All of these should be benign to disable.
 */
#pragma warning(disable: 4028) /* formal parameter different from declaration */
#pragma warning(disable: 4013) /* 'x' undefined, assuming extern returning int */
#pragma warning(disable: 4101) /* unreferenced local variable */
#pragma warning(disable: 4102) /* unreferenced label */
#pragma warning(disable: 4244) /* conversion - possible loss of data */
#pragma warning(disable: 4018) /* signed/unsigned mismatch */
#pragma warning(disable: 4761) /* integral size mismatch */
#pragma warning(disable: 4142) /* benign redefinition of type - mostly in the cylink stuff */

#ifdef WIN32

/*
 * Turn on IXFR
 */
#define BIND_IXFR

/*
 * this does things like #define O_CREAT _O_CREAT
 */
#define _POSIX_

/*
 * everyone needs stdio.h!
 */
#include <stdio.h>
/*
 * Structs defined in the socket library that are used by others
 * plus some useful typedefs (u_char, etc.)
 */
#include <bindwsock.h>
/*
 * Other Winsock2 stuff we need
 */
#include <ws2tcpip.h>
/*
 * These types are not native to NT
 */
#include <sys/bitypes.h>
/*
 * Various limits (duh)
 */
#include <limits.h>
/*
 * _getpid()
 */
#include <process.h>	

/*
 * This is safe - WINNT defines this as 0, and so does BIND
 * (This gets rid of about 1,000,000 warning C4005 messages)
 */
#undef NOERROR

/* This _should_ be safe. */
#undef DELETE

/* Make sure IRS password stuff is turned off */
#undef WANT_IRS_PW

/* NT does not support groups in the fashion BIND wants */
#undef WANT_IRS_GR

/* DDNS */
#define BIND_UPDATE

/* we don't use group or user stuff */
/* but this shuts up the compiler in irs.h */
typedef long gid_t; 

struct  group {
        char    *gr_name;
        char    *gr_passwd;
        gid_t   gr_gid;
        char    **gr_mem;
};

/*
 * NT rename won't overwrite a file, so we define our own isc_movefile 
 * in libbind
 */
#define HAVE_MOVEFILE

/* NT has no vfork, so we do our own thing for spawnxfer() (ns_maint.c) */
#define HAVE_SPAWNXFER

/* Use the BIND-implemented strcasecmp() */
#define NEED_STRCASECMP

/* We have strerror */
#undef NEED_STRERROR

/* For eventlib.c - NT needs the defined pselect */
#define NEED_PSELECT

/*
 * ZXFER explicitly calls gzip.  Turn it off.
 * If you have gzip for NT, turn it on
 */
#define NO_ZXFER

/*
 * h_errno - what a mess
 */
#undef _REENTRANT

/*
 * We do libbind as a DLL, so the externs found in resolv.h 
 * make everything break when compiled.  This #ifdefs out
 * those externs
 */ 
#define SHARED_LIBBIND

 /*
 * We use a .DEF file for the exported functions and data
 * import the exported data explicitly if we are not the DLL
 */
#ifndef LIB
/* Import these if we are not the DLL */
__declspec(dllimport) int optind;
__declspec(dllimport) char *optarg;

/* resolver */
__declspec(dllimport) const struct res_sym __p_class_syms[];
__declspec(dllimport) const struct res_sym __p_key_syms[];
__declspec(dllimport) const struct res_sym __p_cert_syms[];
__declspec(dllimport) const struct res_sym __p_type_syms[];
__declspec(dllimport) const char *_res_opcodes[];
#define HAVE_DECL_RES_OPCODES 1
#else

#endif /* LIBBIND */

/*
 * Resolve paths to things (see paths.h)
 */
char *BINDPath(int id);

/* Set of routines to pick up
 * Domain name and nameserver addresses
 */

#define NO_RESOLV_CONF 1
char *get_host_domain();
char *get_nameserver_addr(int);
int init_nameserver_list();

/*
 * ANSI C compliance enabled
 * MSVC will define _read() but not read(), so we can do our unistd chicanery
 * we must do all the #define x _x for structs and functions ourselves, though.
 */
#define __STDC__ 1

/*
 * Functions
 */
#define fstat	_fstat
#define isascii	__isascii
#define getpid	_getpid
#define popen	_popen
#define pclose	_pclose
#define snprintf _snprintf
#define getcwd  _getcwd

/*NT has its own function OpenFile */

#define OpenFile ISCOpenFile

/*NT needs to handle these functions separately */
#define rename  NTrename
#define fopen   NTfopen
#define fclose  NTfclose

FILE *NTfopen( const char *filename, const char *mode);

/*
 * Define these other functions the other way
 */

#undef __res_opcodes

/*
 * This is why we don't have lib/bsd/ftruncate.c
 */
#define ftruncate _chsize

/*
 * Simple Types
 */
#define off_t _off_t
typedef int ssize_t;
typedef char *caddr_t;
typedef long pid_t;
typedef long uid_t; 
typedef long gid_t;
typedef int mode_t;
typedef unsigned short ushort;

/*
 * Structs 
 */
#include <sys/stat.h>
#define stat _stat

/*
 * struct _TIMEZONE 
 */
#include <sys/time.h>	
#ifndef TIMESPEC_T
#define TIMESPEC_T
typedef struct  timespec {            
        time_t          tv_sec;         /* seconds */
        long            tv_nsec;        /* and nanoseconds */
} timespec_t, timespec;
#endif /* TIMESPEC_T */

struct iovec {
	char *  iov_base;
	int 	iov_len;
};

#define MAXPATHLEN _MAX_PATH

/* MSVC is a c++ compiler.  Undef this just in case */
#undef try

#endif /* WIN32 */

#endif /* _PORT_BEFORE_H */
