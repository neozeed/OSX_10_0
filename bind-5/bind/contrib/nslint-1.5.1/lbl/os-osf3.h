/*
 * Copyright (c) 1995, 1996
 *	The Regents of the University of California.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that: (1) source code distributions
 * retain the above copyright notice and this paragraph in its entirety, (2)
 * distributions including binary code include the above copyright notice and
 * this paragraph in its entirety in the documentation or other materials
 * provided with the distribution, and (3) all advertising materials mentioning
 * features or use of this software display the following acknowledgement:
 * ``This product includes software developed by the University of California,
 * Lawrence Berkeley Laboratory and its contributors.'' Neither the name of
 * the University nor the names of its contributors may be used to endorse
 * or promote products derived from this software without specific prior
 * written permission.
 * THIS SOFTWARE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 *
 * @(#) $Header: /cvs/Darwin/Services/bind/bind/contrib/nslint-1.5.1/lbl/os-osf3.h,v 1.1.1.2 2001/01/31 03:59:08 zarzycki Exp $ (LBL)
 */

/* Signal routines are this type */
#define SIGRET void
/* Signal routines use "return SIGRETVAL;" */
#define SIGRETVAL
/* The wait() status variable is this type */
#define WAITSTATUS int

/* Prototypes missing in osf3 */
int	ioctl(int, int, caddr_t);
int	iruserok(u_int, int, char *, char *);
int	pfopen(char *, int);
int	rcmd(char **, u_short, const char *, const char *, const char *, int *);
int	rresvport(int *);
int	snprintf(char *, size_t, const char *, ...);
