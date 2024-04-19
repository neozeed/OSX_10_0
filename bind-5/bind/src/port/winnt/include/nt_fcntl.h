#ifndef _NT_FCNTL_H
#define _NT_FCNTL_H

/* None of these are defined in NT, so define them for our use */
#define O_NONBLOCK 1
#define PORT_NONBLOCK O_NONBLOCK

/*
 * fcntl() commands
 */
#define F_SETFL 0
#define F_GETFL 1
#define F_SETFD 2
#define F_GETFD 3
/*
 * Enough problems not having full fcntl() without worrying about this!
 */
#undef F_DUPFD 

int fcntl(int, int,...);

#endif
