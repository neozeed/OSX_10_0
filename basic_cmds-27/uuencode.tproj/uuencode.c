/* 
 * Copyright (c) 1995 NeXT Computer, Inc. All Rights Reserved
 *
 * Copyright (c) 1983, 1993
 *	The Regents of the University of California.  All rights reserved.
 *
 * The NEXTSTEP Software License Agreement specifies the terms
 * and conditions for redistribution.
 *
 *	@(#)uuencode.c	8.2 (Berkeley) 4/2/94
 */


#ifndef lint
char copyright[] =
"@(#) Copyright (c) 1983, 1993\n\
	The Regents of the University of California.  All rights reserved.\n";
#endif /* not lint */

/*
 * uuencode [input] output
 *
 * Encode a file so it can be mailed to a remote system.
 */
#include <sys/types.h>
#include <sys/stat.h>

#include <stdio.h>

int
main(argc, argv)
	int argc;
	char *argv[];
{
	extern int optind;
	extern int errno;
	struct stat sb;
	int mode;
	char *strerror();

	while (getopt(argc, argv, "") != EOF)
		usage();
	argv += optind;
	argc -= optind;

	switch(argc) {
	case 2:			/* optional first argument is input file */
		if (!freopen(*argv, "r", stdin) || fstat(fileno(stdin), &sb)) {
			(void)fprintf(stderr, "uuencode: %s: %s.\n",
			    *argv, strerror(errno));
			exit(1);
		}
#define	RWX	(S_IRWXU|S_IRWXG|S_IRWXO)
		mode = sb.st_mode & RWX;
		++argv;
		break;
	case 1:
#define	RW	(S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH|S_IWOTH)
		mode = RW & ~umask(RW);
		break;
	case 0:
	default:
		usage();
	}

	(void)printf("begin %o %s\n", mode, *argv);
	encode();
	(void)printf("end\n");
	if (ferror(stdout)) {
		(void)fprintf(stderr, "uuencode: write error.\n");
		exit(1);
	}
	exit(0);
}

/* ENC is the basic 1 character encoding function to make a char printing */
#define	ENC(c) ((c) ? ((c) & 077) + ' ': '`')

/*
 * copy from in to out, encoding as you go along.
 */
encode()
{
	register int ch, n;
	register char *p;
	char buf[80];

	while (n = fread(buf, 1, 45, stdin)) {
		ch = ENC(n);
		if (putchar(ch) == EOF)
			break;
		for (p = buf; n > 0; n -= 3, p += 3) {
			ch = *p >> 2;
			ch = ENC(ch);
			if (putchar(ch) == EOF)
				break;
			ch = (*p << 4) & 060 | (p[1] >> 4) & 017;
			ch = ENC(ch);
			if (putchar(ch) == EOF)
				break;
			ch = (p[1] << 2) & 074 | (p[2] >> 6) & 03;
			ch = ENC(ch);
			if (putchar(ch) == EOF)
				break;
			ch = p[2] & 077;
			ch = ENC(ch);
			if (putchar(ch) == EOF)
				break;
		}
		if (putchar('\n') == EOF)
			break;
	}
	if (ferror(stdin)) {
		(void)fprintf(stderr, "uuencode: read error.\n");
		exit(1);
	}
	ch = ENC('\0');
	(void)putchar(ch);
	(void)putchar('\n');
}

usage()
{
	(void)fprintf(stderr,"usage: uuencode [infile] remotefile\n");
	exit(1);
}
