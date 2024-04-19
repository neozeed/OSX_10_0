#ifndef UNISTD_H
#define UNISTD_H		1

#include "port_before.h"
#include "port_after.h"

#define _SC_OPEN_MAX 1	/* sysconf() query max open FDs */

typedef struct _fddata {
	int fd;
	int flags;
	int descrip;
} fddata;

/* Used in the call to itimer by some callers */
#define ITIMER_REAL	1
#define MAXALIASES 35

/* Map UNIX functions onto MSVC ANSI functions */
/* definitions are in libport.lib */
int fchown(int, uid_t, gid_t);
int chown(const char *, uid_t, gid_t);
int pipe(int[2]);
int fsync(int);
int S_ISSOCK(int);

/*
 * Define a few values for the access routine
 */
#define F_OK  00	/* File Existance */
#define W_OK  02	/* Write Permission */
#define R_OK  04	/* Read Permission */
#define X_OK  06	/* Read and Write Permission */

/* UNIX functions we wrote */
/* Our implementation of ioctl() gives the IF addresses with a call to SIOGCIFCONF */
#define SIOCGIFCONF_ADDR

#define SIOCGIFCONF		1 /* Get interface configuration list */
#define SIOCGIFADDR		2 /* Get interface address */
#define SIOCGIFFLAGS	3 /* Get interface flags */
#define SIOCGIFDSTADDR	4 /* Get interface point to point address */
int ioctl(int, int, ...);

uid_t getuid();
gid_t getgid();

long sysconf(int);				

void unistdInit();
void unistdCleanup();
#endif
