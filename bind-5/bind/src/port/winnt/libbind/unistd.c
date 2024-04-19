#include "port_before.h"

#include <unistd.h>
#include <stdio.h>
#include <syslog.h>
#include <net/if.h>
#include <errno.h>

#include <sys/socket.h>
#include <io.h>
#include <lmaccess.h>
#include <lmserver.h>
#include <lmapibuf.h>
#include <lmwksta.h>
#include <fcntl.h>

#include "port_after.h"

/* We are going to use these functions here properly,
 * so we undefine them.
 */

#undef rename 
#undef fopen   
#undef fclose 

/*
 * sockets are file descriptors in UNIX.  This is not so in NT
 * We must keep track of what is a socket and what is an FD to
 * make everything flow right.
 */
fddata fdset[FOPEN_MAX];
int fdcount = 0;

/* Interface data */
#define MAX_INTERFACES	600

struct ifdata *IFData[MAX_INTERFACES];
static int numInterfaces = 0;

/*
 * Initialize for FD tracking, ioctl()
 */
void unistdInit()
{
	int nextIndex;
	int i;
	for(i=0; i < FOPEN_MAX; i++)
	{
		fdset[i].fd = 0;
		fdset[i].flags = 0;
		fdset[i].descrip = 0;
	}
	
	numInterfaces = GetInterfaces(IFData, MAX_INTERFACES);
}

/*
 * Clean up - free memory allocated by GetInterfaces()
 */
void unistdCleanup()
{
	int i;

	for(i = 0; i < MAX_INTERFACES; i++)
		if(IFData[i])
			free(IFData[i]);
}


/*
 * Add a fd to the list. Request can get the index
 */
int fdin(int fd)
{
	int i = 0;
	while(fdset[i].fd > 0)
		i++;

	if(i < REAL_FD_SETSIZE)
	{
		fdset[i].fd = fd;
		fdcount ++;
	}
	return i;
}

/*
 * Remove a fd from the list
 */
void fdout(int fd)
{
	int i = 0;
	while(fdset[i].fd != fd && i < FOPEN_MAX)
		i++;

	if(i < FOPEN_MAX)
	{
	    fdset[i].fd = 0;
		fdset[i].flags = 0;
		fdset[i].descrip = 0;
	    fdcount--;
	}
}

/*
 * Note: The only FD flags that Unix supports is the FD_CLOEXEC flag
 * It is 0 - don't close-on-exec or 1 = do close-on-exec.
 * In either case this is meaningless to Windows NT.  We'll set the
 * value in our internal structure, but really do nothing with it.
 */

int getfdflags(int fd)
{
	int i = 0;
	while(fdset[i].fd != fd && i < FOPEN_MAX)
		i++;

	if(i < FOPEN_MAX)
	 	return(fdset[i].descrip);
	else		// Not found, let's add it to the list anyway
	{
		i = fdin(fd);
	}
	return(0);		// No flags anyway
}

int setfdflags(int fd, int flags)
{
	int i = 0;
	while(fdset[i].fd != fd && i < FOPEN_MAX)
		i++;

	if(i < FOPEN_MAX)
	{
 		fdset[i].descrip = flags;
		return(0);
	}
	else		// Not found, let's add it to the list anyway
	{
		i = fdin(fd);
		if(i <FOPEN_MAX)
		{
			fdset[i].descrip = flags;
			return(0);
		}
	}
	return(-1);		// Never happens unless full
}

/*
 * BIND doesn't try and set any file descriptor flags
 * This is here just in case. We could probably only get
 * file flags anyway.
 */

int getfileflags(int fd)
{
	int i = 0;
	while(fdset[i].fd != fd && i < FOPEN_MAX)
		i++;

	if(i < FOPEN_MAX)
	 	return(fdset[i].flags);
	else		// Not found, let's add it to the list anyway
	{
		i = fdin(fd);
	}
	return(0);		// No flags anyway
}

/*
 * The only flags that could be set are O_APPEND, O_NONBLOCK, O_SYNC, O_ASYNC.
 * On a file we are supporting none of them as they are not required by BIND.
 */

int setfileflags(int fd, int flags)
{
	int i = 0;
	while(fdset[i].fd != fd && i < FOPEN_MAX)
		i++;

	if(i < FOPEN_MAX)
	{
 		fdset[i].flags = flags;
		return(0);
	}
	else		// Not found, let's add it to the list anyway
	{
		i = fdin(fd);
		if(i <FOPEN_MAX)
		{
			fdset[i].flags = flags;
			return(0);
		}
	}
	return(-1);		// Never happens unless full
}

/*
 * ioctl() is the root of all socket evil in UNIX.
 */
int ioctl(int fd, int request, ...)
{
	struct ifconf *ifc;
	struct ifreq *ifr;
	char *buf;
	int buflen;
	int i, rc = -1;
	va_list ap;

	va_start(ap, request);
	
	switch(request)
	{
		case SIOCGIFCONF:
			ifc = va_arg(ap, struct ifconf *);
			buflen = ifc->ifc_len;
			/*
			 * Do we have enough space to load the Interface Data?
			 */
			if(buflen < numInterfaces*sizeof(struct ifreq))
			{
				errno = EINVAL;
				return (-1);	/* Too small return with error code */
			}
			ifc->ifc_len = 0;
			buf = ifc->ifc_buf;
			rc = 0;

			for(i = 0; i < numInterfaces; i++)
			{
				ifr = (struct ifreq *)buf;
				memset(ifr->ifr_oname, 0, IFNAMSIZ);

				strcpy(ifr->ifr_name,IFData[i]->if_name);
				memcpy(&(ifr->ifr_addr), &(IFData[i]->if_addr), sizeof(struct sockaddr)); 
				
				/* now increment pointer */
				buf += sizeof(struct ifreq);
				ifc->ifc_len += sizeof(struct ifreq);
			}
			break;
		case SIOCGIFADDR:
			ifr = va_arg(ap, struct ifreq *);
			memset(ifr->ifr_oname, 0, IFNAMSIZ);
			for(i = 0; i < numInterfaces; i++)
			{
				if(!strcmp(ifr->ifr_name, IFData[i]->if_name))
				{
					memcpy(&(ifr->ifr_addr), &(IFData[i]->if_addr), sizeof(struct sockaddr));
					rc = 0;
					break;
				}
			}
			break;
		case SIOCGIFFLAGS:
			ifr = va_arg(ap, struct ifreq *);
			memset(ifr->ifr_oname, 0, IFNAMSIZ);
			for(i = 0; i < numInterfaces; i++)
			{
				if(!strcmp(ifr->ifr_name, IFData[i]->if_name))
				{
					ifr->ifr_flags  = IFData[i]->if_flags;
					rc = 0;
					break;
				}
			}
			break;
		case SIOCGIFDSTADDR:
			ifr = va_arg(ap, struct ifreq *);
			memset(ifr->ifr_oname, 0, IFNAMSIZ);
			for(i = 0; i < numInterfaces; i++)
			{
				if(!strcmp(ifr->ifr_name, IFData[i]->if_name))
				{
					memcpy(&(ifr->ifr_dstaddr), &(IFData[i]->if_dstaddr), sizeof(struct sockaddr)); 
					rc = 0;
					break;
				}
			}
			break;
		default:
			break;
	}	
	
	va_end(ap);
	return (rc);
}


/*
 * _SC_OPEN_MAX should return FOPEN_MAX, but BIND thinks that file
 * descriptors can be sockets or files.  A socket number can be over 1000,
 * so we need to return a high number so evHighestFD() doesn't crap out on us.
 */ 
long sysconf(int which)
{
	long rc = -1;

	switch(which)
	{
		case _SC_OPEN_MAX:
			rc = 16384;
			break;
		default:
			break;
	}
	return rc;
}

int gethostid()
{
	/* Build a hostid out of the hostname */

	NET_API_STATUS status;
	LPBYTE buf;
	LPTSTR hostname;
	int id = 0;
	int	i = 0,
		nulCount = 0;
	
	status = NetServerGetInfo(NULL, 100, &buf);
	hostname = ((LPSERVER_INFO_100)buf)->sv100_name;
	/* hostname is Unicode, so it is terminated double-NULL */
	while(nulCount < 2)
	{
		if(hostname[i] == 0)
			nulCount++;
		else
		{
			nulCount = 0;
			id += hostname[i];
		}
		i++;
	}
	NetApiBufferFree(buf);
	return(id);
}

int getppid()
{
	/****
	***** If we ever need this for anything but seeding the HMAC key in prandom, 
	***** something clever needs to happen here.
	****/
	
	/* No NT equivalent.  We'll use the our own process ID */
	return _getpid();
}

uid_t getuid()
{
	/* Build a uid out of the username string */
	
	char username[64];
	int id = 0;
	int	i = 0,
		size = 64;
	
	GetUserName(username, &size);
	for(i = 0; i < size; i++)
		id += username[i];
	return(id);
}

gid_t getgid()
{
	/* Groups are a pain on NT.  Return a dummy */
	return 42; /* d. adams */
}

/*
 * open(), close(), read(), write(), fsync()
 * sockets are file descriptors in UNIX.  This is not so in NT
 * We keep track of what is a socket and what is an FD to
 * make everything flow right.
 */
int open(const char *fn, int flags, ...)
{
	va_list args;
	int pmode;
	int fd;

       if(fdcount == FOPEN_MAX)
       {
               errno = EMFILE;
               return(-1);
       }
	/* Extract the cmd parameter */
	va_start(args, flags);
	pmode = va_arg(args, int);
	fd = _open(fn, flags, pmode);

	/* Only add to the list if we succeeded. */
	if(fd > 0)
		fdin(fd);
	return fd;
}

int close(int fd)
{
	if(S_ISSOCK(fd))
	{
		return closesocket(fd);
	}
	else
	{
              if(fdcount > 0)
              {
                      fdout(fd);
                      return _close(fd);
              }
              else
              {
                      errno = EBADF;
                      return(-1);
              }
	}
}

int read(int fd, char *buf, int len)
{
	if(S_ISSOCK(fd))
		/* If request to read a length of zero bytes, return zero  ie. zero bytes read. 
		   Windows recv() function seems to have a problem in handling a len parameter of zero.*/
		if (len == 0)
			return 0;
		else
		return recv(fd, buf, len, 0);
	else
		return _read(fd, buf, len);
}

int write(int fd, char *buf, int len)
{
	if(S_ISSOCK(fd))
		return send(fd, buf, len, 0);
	else
		return _write(fd, buf, len);
}

int fsync(int fd)
{
	if(S_ISSOCK(fd))
	{
		/* No way to flush a socket */
		return 0;
	}
	else
	{
		/* Sync memory contents with disk */
		return _commit(fd);
	}
}

/*
 * fopen and fclose need to be handled here in order to
 * register their file descriptors.
 */

FILE *NTfopen( const char *filename, const char *mode)
{
	FILE *fp;
	int fd;
	int flags= 0;
	fp = fopen(filename, mode);
	if(fp != NULL)
	{
		fd = _fileno(fp);
		fdin(fd);
		/* This sets up the flag bits based on the mode */
		if(strcasecmp(mode, "a") == 0)
		{
			flags = _O_WRONLY|_O_CREAT|O_APPEND;
		}
		else if(strcasecmp(mode, "a+")== 0)
		{
			flags = _O_RDWR|_O_CREAT|O_APPEND;
		}
		else if(strcasecmp(mode, "r")== 0)
		{
			flags = _O_RDONLY;
		}
		else if(strcasecmp(mode, "r+")== 0)
		{
			flags = _O_RDWR;
		}
		else if(strcasecmp(mode, "w")== 0)
		{
			flags = _O_WRONLY|_O_CREAT|O_TRUNC;
		}
		else if(strcasecmp(mode, "w+")== 0)
		{
			flags = _O_RDWR|_O_CREAT|O_TRUNC;
		}
		else if(strcasecmp(mode, "b")== 0)
		{
			flags = _O_BINARY;
		}
		else if(strcasecmp(mode, "t")== 0)
		{
			flags = _O_TEXT;
		}

		/* Now set the flags */

		setfileflags(fd, flags);
	}
	return (fp);
}

int NTfclose(FILE *fp)
{
	int fd = _fileno(fp);
	fdout(fd);
	return(fclose(fp));
}

/* Rename is needed to be defined here to ensure that any file with
 * the new name is deleted first.
 */

int NTrename(char *from, char *to)
{
	DeleteFile(to);
	return(rename(from, to));
}

int fchown(int fd, uid_t owner, gid_t group)
{
	/* Do nothing, return all OK. */
	return 0;
}

int chown(const char *path, uid_t owner, gid_t group)
{
	/* Do nothing, return all OK. */
	return 0;
}

/*
 * Passthrough to NT _pipe()
 */
int pipe(int filedes[2])
{
	return _pipe(filedes, 65535, _O_TEXT);
}

/*
 * Holds flags, sets block/nonblock for files
 * not much else
 */

int fcntlfile(int fd, int cmd, u_long arg)
{
	int i, rc = -1;	

	switch(cmd)
	{
		/* Set status flags */
		case F_SETFL:
            rc = setfileflags(fd, arg);
			break;
		/* Get status flags */
		case F_GETFL:
            rc = getfileflags(fd);
			break;
		/* Set FD flags */
		case F_SETFD:
            rc = setfdflags(fd, arg);
			break;
		/* Get FD flags */
		case F_GETFD:
            rc = getfdflags(fd);
			break;
		default:
			break;
	}
	return rc;
}

/*
 * NT version of fcntl. Needs to determine if it's a Socket or a File.
 * Forward to appropriate place to be processed.
 */

int fcntl(int fd, int cmd, ...)
{
	va_list args;
	u_long arg;
	int i, rc = -1;
	va_start(args, cmd);

	arg = va_arg(args, u_long);	// Get the argument.  There's really only one
	
	// Determine if this is a socket or a file
	if(S_ISSOCK(fd))
		rc = fcntlsocket(fd, cmd, arg);
    else
		rc = fcntlfile(fd, cmd, arg);
	return rc;
}


/*
 * Emulate UNIX mkstemp, which returns an open FD to the new file
 *
 */
static int
gettemp(char *path, int *doopen) {
	char *start, *trv;
	struct stat sbuf;
	u_int pid;

	pid = getpid();
	for (trv = path; *trv; ++trv);		/* extra X's get set to 0's */
	while (*--trv == 'X') {
		*trv = (pid % 10) + '0';
		pid /= 10;
	}
	/*
	 * check the target directory; if you have six X's and it
	 * doesn't exist this runs for a *very* long time.
	 */
	for (start = trv + 1;; --trv) {
		if (trv <= path)
			break;
		if (*trv == '/') {
			*trv = '\0';
			if (stat(path, &sbuf))
				return(0);
			if (!S_ISDIR(sbuf.st_mode)) {
				errno = ENOTDIR;
				return(0);
			}
			*trv = '/';
			break;
		}
	}

	for (;;) {
		if (doopen) {
			if ((*doopen =
			    open(path, O_CREAT|O_EXCL|O_RDWR, _S_IREAD | _S_IWRITE)) >= 0)
				return(1);
			if (errno != EEXIST)
				return(0);
		}
		else if (stat(path, &sbuf))
			return(errno == ENOENT ? 1 : 0);

		/* tricky little algorithm for backward compatibility */
		for (trv = start;;) {
			if (!*trv)
				return(0);
			if (*trv == 'z')
				*trv++ = 'a';
			else {
				if (isdigit(*trv))
					*trv = 'a';
				else
					++*trv;
				break;
			}
		}
	}
	/*NOTREACHED*/
}

mkstemp(char *path) {
	int fd;

	return (gettemp(path, &fd) ? fd : -1);
}
