/*
 * Copyright (c) 1999 Apple Computer, Inc. All rights reserved.
 *
 * @APPLE_LICENSE_HEADER_START@
 * 
 * "Portions Copyright (c) 1999 Apple Computer, Inc.  All Rights
 * Reserved.  This file contains Original Code and/or Modifications of
 * Original Code as defined in and that are subject to the Apple Public
 * Source License Version 1.0 (the 'License').  You may not use this file
 * except in compliance with the License.  Please obtain a copy of the
 * License at http://www.apple.com/publicsource and read it before using
 * this file.
 * 
 * The Original Code and all software distributed under the License are
 * distributed on an 'AS IS' basis, WITHOUT WARRANTY OF ANY KIND, EITHER
 * EXPRESS OR IMPLIED, AND APPLE HEREBY DISCLAIMS ALL SUCH WARRANTIES,
 * INCLUDING WITHOUT LIMITATION, ANY WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE OR NON-INFRINGEMENT.  Please see the
 * License for the specific language governing rights and limitations
 * under the License."
 * 
 * @APPLE_LICENSE_HEADER_END@
 */
/*
 * Copyright (c) 1983, 1989, 1991, 1993
 *	The Regents of the University of California.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *	This product includes software developed by the University of
 *	California, Berkeley and its contributors.
 * 4. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#ifndef lint
static const char copyright[] =
"@(#) Copyright (c) 1983, 1989, 1991, 1993\n\
	The Regents of the University of California.  All rights reserved.\n";
#endif /* not lint */

#ifndef lint
#if 0
static char sccsid[] = "@(#)route.c	8.3 (Berkeley) 3/19/94";
#endif
static const char rcsid[] =
	"$Id: route.c,v 1.1.1.2 2000/01/11 01:49:18 wsanchez Exp $";
#endif /* not lint */

#include <sys/param.h>
#include <sys/file.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <sys/sysctl.h>

#include <net/if.h>
#include <net/route.h>
#include <net/if_dl.h>
#include <netinet/in.h>
#ifdef AT
#include <netatalk/at.h>
#endif
#ifdef NS
#include <netns/ns.h>
#endif
#include <arpa/inet.h>
#include <netdb.h>

#include <ctype.h>
#include <err.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sysexits.h>
#include <unistd.h>

struct keytab {
	char	*kt_cp;
	int	kt_i;
} keywords[] = {
#include "keywords.h"
	{0, 0}
};

struct	ortentry route;
union	sockunion {
	struct	sockaddr sa;
	struct	sockaddr_in sin;
#ifdef AT
	struct	sockaddr_at sat;
#endif
#ifdef NS
	struct	sockaddr_ns sns;
#endif
	struct	sockaddr_dl sdl;
} so_dst, so_gate, so_mask, so_genmask, so_ifa, so_ifp;

typedef union sockunion *sup;
int	pid, rtm_addrs, uid;
int	s;
int	forcehost, forcenet, doflush, nflag, af, qflag, tflag, keyword();
int	iflag, verbose, aflen = sizeof (struct sockaddr_in);
int	locking, lockrest, debugonly;
struct	rt_metrics rt_metrics;
u_long  rtm_inits;
struct	in_addr inet_makeaddr();
#ifdef AT
int	atalk_aton __P((const char *, struct at_addr *));
char	*atalk_ntoa __P((struct at_addr));
#endif
char	*routename(), *netname();
void	flushroutes(), newroute(), monitor(), sockaddr(), sodump(), bprintf();
void	print_getmsg(), print_rtmsg(), pmsg_common(), pmsg_addrs(), mask_addr();
int	getaddr(), rtmsg(), x25_makemask();
extern	char *inet_ntoa(), *iso_ntoa(), *link_ntoa();

void usage __P((const char *));

void
usage(cp)
	const char *cp;
{
	if (cp)
		warnx("bad keyword: %s", cp);
	(void) fprintf(stderr,
	    "usage: route [-dnqtv] command [[modifiers] args]\n");
	exit(EX_USAGE);
	/* NOTREACHED */
}

#define ROUNDUP(a) \
	((a) > 0 ? (1 + (((a) - 1) | (sizeof(long) - 1))) : sizeof(long))
#define ADVANCE(x, n) (x += ROUNDUP((n)->sa_len))

int
main(argc, argv)
	int argc;
	char **argv;
{
	int ch;

	if (argc < 2)
		usage((char *)NULL);

	while ((ch = getopt(argc, argv, "nqdtv")) != -1)
		switch(ch) {
		case 'n':
			nflag = 1;
			break;
		case 'q':
			qflag = 1;
			break;
		case 'v':
			verbose = 1;
			break;
		case 't':
			tflag = 1;
			break;
		case 'd':
			debugonly = 1;
			break;
		case '?':
		default:
			usage((char *)NULL);
		}
	argc -= optind;
	argv += optind;

	pid = getpid();
	uid = getuid();
	if (tflag)
		s = open("/dev/null", O_WRONLY, 0);
	else
		s = socket(PF_ROUTE, SOCK_RAW, 0);
	if (s < 0)
		err(EX_OSERR, "socket");
	setuid(uid);
	if (*argv)
		switch (keyword(*argv)) {
		case K_GET:
			uid = 0;
			/* FALLTHROUGH */

		case K_CHANGE:
		case K_ADD:
		case K_DELETE:
			newroute(argc, argv);
			exit(0);
			/* NOTREACHED */

		case K_MONITOR:
			monitor();
			/* NOTREACHED */

		case K_FLUSH:
			flushroutes(argc, argv);
			exit(0);
			/* NOTREACHED */
		}
	usage(*argv);
	/* NOTREACHED */
}

/*
 * Purge all entries in the routing tables not
 * associated with network interfaces.
 */
void
flushroutes(argc, argv)
	int argc;
	char *argv[];
{
	size_t needed;
	int mib[6], rlen, seqno;
	char *buf, *next, *lim;
	register struct rt_msghdr *rtm;

	if (uid) {
		errx(EX_NOPERM, "must be root to alter routing table");
	}
	shutdown(s, 0); /* Don't want to read back our messages */
	if (argc > 1) {
		argv++;
		if (argc == 2 && **argv == '-')
		    switch (keyword(*argv + 1)) {
			case K_INET:
				af = AF_INET;
				break;

#ifdef
			case K_ATALK:
				af = AF_APPLETALK;
				break;
#endif
#ifdef NS
			case K_XNS:
				af = AF_NS;
				break;
#endif
			case K_LINK:
				af = AF_LINK;
				break;
			default:
				goto bad;
		} else
bad:			usage(*argv);
	}
	mib[0] = CTL_NET;
	mib[1] = PF_ROUTE;
	mib[2] = 0;		/* protocol */
	mib[3] = 0;		/* wildcard address family */
	mib[4] = NET_RT_DUMP;
	mib[5] = 0;		/* no flags */
	if (sysctl(mib, 6, NULL, &needed, NULL, 0) < 0)
		err(EX_OSERR, "route-sysctl-estimate");
	if ((buf = malloc(needed)) == NULL)
		errx(EX_OSERR, "malloc failed");
	if (sysctl(mib, 6, buf, &needed, NULL, 0) < 0)
		err(EX_OSERR, "route-sysctl-get");
	lim = buf + needed;
	if (verbose)
		(void) printf("Examining routing table from sysctl\n");
	seqno = 0;		/* ??? */
	for (next = buf; next < lim; next += rtm->rtm_msglen) {
		rtm = (struct rt_msghdr *)next;
		if (verbose)
			print_rtmsg(rtm, rtm->rtm_msglen);
		if ((rtm->rtm_flags & RTF_GATEWAY) == 0)
			continue;
		if (af) {
			struct sockaddr *sa = (struct sockaddr *)(rtm + 1);

			if (sa->sa_family != af)
				continue;
		}
		if (debugonly)
			continue;
		rtm->rtm_type = RTM_DELETE;
		rtm->rtm_seq = seqno;
		rlen = write(s, next, rtm->rtm_msglen);
		if (rlen < (int)rtm->rtm_msglen) {
			warn("write to routing socket");
			(void) printf("got only %d for rlen\n", rlen);
			break;
		}
		seqno++;
		if (qflag)
			continue;
		if (verbose)
			print_rtmsg(rtm, rlen);
		else {
			struct sockaddr *sa = (struct sockaddr *)(rtm + 1);
			(void) printf("%-20.20s ", rtm->rtm_flags & RTF_HOST ?
			    routename(sa) : netname(sa));
			sa = (struct sockaddr *)(sa->sa_len + (char *)sa);
			(void) printf("%-20.20s ", routename(sa));
			(void) printf("done\n");
		}
	}
}

char *
routename(sa)
	struct sockaddr *sa;
{
	register char *cp;
	static char line[MAXHOSTNAMELEN + 1];
	struct hostent *hp;
	static char domain[MAXHOSTNAMELEN + 1];
	static int first = 1;
#ifdef NS
	char *ns_print();
#endif

	if (first) {
		first = 0;
		if (gethostname(domain, MAXHOSTNAMELEN) == 0 &&
		    (cp = index(domain, '.'))) {
			domain[MAXHOSTNAMELEN] = '\0';
			(void) strcpy(domain, cp + 1);
		} else
			domain[0] = 0;
	}

	if (sa->sa_len == 0)
		strcpy(line, "default");
	else switch (sa->sa_family) {

	case AF_INET:
	    {	struct in_addr in;
		in = ((struct sockaddr_in *)sa)->sin_addr;

		cp = 0;
		if (in.s_addr == INADDR_ANY || sa->sa_len < 4)
			cp = "default";
		if (cp == 0 && !nflag) {
			hp = gethostbyaddr((char *)&in, sizeof (struct in_addr),
				AF_INET);
			if (hp) {
				if ((cp = index(hp->h_name, '.')) &&
				    !strcmp(cp + 1, domain))
					*cp = 0;
				cp = hp->h_name;
			}
		}
		if (cp) {
			strncpy(line, cp, sizeof(line) - 1);
			line[sizeof(line) - 1] = '\0';
		} else {
			/* XXX - why not inet_ntoa()? */
#define C(x)	(unsigned)((x) & 0xff)
			in.s_addr = ntohl(in.s_addr);
			(void) sprintf(line, "%u.%u.%u.%u", C(in.s_addr >> 24),
			   C(in.s_addr >> 16), C(in.s_addr >> 8), C(in.s_addr));
		}
		break;
	    }
#ifdef AT
	case AF_APPLETALK:
		(void) snprintf(line, sizeof(line), "atalk %s",
			atalk_ntoa(((struct sockaddr_at *)sa)->sat_addr));
		break;
#endif
#ifdef NS
	case AF_NS:
		return (ns_print((struct sockaddr_ns *)sa));
#endif

	case AF_LINK:
		return (link_ntoa((struct sockaddr_dl *)sa));

	default:
	    {	u_short *s = (u_short *)sa;
		u_short *slim = s + ((sa->sa_len + 1) >> 1);
		char *cp = line + sprintf(line, "(%d)", sa->sa_family);
		char *cpe = line + sizeof(line);

		while (++s < slim && cp < cpe) /* start with sa->sa_data */
			cp += snprintf(cp, cpe - cp, " %x", *s);
		break;
	    }
	}
	return (line);
}

/*
 * Return the name of the network whose address is given.
 * The address is assumed to be that of a net or subnet, not a host.
 */
char *
netname(sa)
	struct sockaddr *sa;
{
	char *cp = 0;
	static char line[MAXHOSTNAMELEN + 1];
	struct netent *np = 0;
	u_long net, mask;
	register u_long i;
	int subnetshift;
#ifdef NS
	char *ns_print();
#endif

	switch (sa->sa_family) {

	case AF_INET:
	    {	struct in_addr in;
		in = ((struct sockaddr_in *)sa)->sin_addr;

		i = in.s_addr = ntohl(in.s_addr);
		if (in.s_addr == 0)
			cp = "default";
		else if (!nflag) {
			if (IN_CLASSA(i)) {
				mask = IN_CLASSA_NET;
				subnetshift = 8;
			} else if (IN_CLASSB(i)) {
				mask = IN_CLASSB_NET;
				subnetshift = 8;
			} else {
				mask = IN_CLASSC_NET;
				subnetshift = 4;
			}
			/*
			 * If there are more bits than the standard mask
			 * would suggest, subnets must be in use.
			 * Guess at the subnet mask, assuming reasonable
			 * width subnet fields.
			 */
			while (in.s_addr &~ mask)
				mask = (long)mask >> subnetshift;
			net = in.s_addr & mask;
			while ((mask & 1) == 0)
				mask >>= 1, net >>= 1;
			np = getnetbyaddr(net, AF_INET);
			if (np)
				cp = np->n_name;
		}
		if (cp)
			strncpy(line, cp, sizeof(line));
		else if ((in.s_addr & 0xffffff) == 0)
			(void) sprintf(line, "%u", C(in.s_addr >> 24));
		else if ((in.s_addr & 0xffff) == 0)
			(void) sprintf(line, "%u.%u", C(in.s_addr >> 24),
			    C(in.s_addr >> 16));
		else if ((in.s_addr & 0xff) == 0)
			(void) sprintf(line, "%u.%u.%u", C(in.s_addr >> 24),
			    C(in.s_addr >> 16), C(in.s_addr >> 8));
		else
			(void) sprintf(line, "%u.%u.%u.%u", C(in.s_addr >> 24),
			    C(in.s_addr >> 16), C(in.s_addr >> 8),
			    C(in.s_addr));
		break;
	    }
#ifdef AT
	case AF_APPLETALK:
		(void) snprintf(line, sizeof(line), "atalk %s",
			atalk_ntoa(((struct sockaddr_at *)sa)->sat_addr));
		break;
#endif
#ifdef NS
	case AF_NS:
		return (ns_print((struct sockaddr_ns *)sa));
		break;
#endif

	case AF_LINK:
		return (link_ntoa((struct sockaddr_dl *)sa));


	default:
	    {	u_short *s = (u_short *)sa->sa_data;
		u_short *slim = s + ((sa->sa_len + 1)>>1);
		char *cp = line + sprintf(line, "af %d:", sa->sa_family);
		char *cpe = line + sizeof(line);

		while (s < slim && cp < cpe)
			cp += snprintf(cp, cpe - cp, " %x", *s++);
		break;
	    }
	}
	return (line);
}

void
set_metric(value, key)
	char *value;
	int key;
{
	int flag = 0;
	u_long noval, *valp = &noval;

	switch (key) {
#define caseof(x, y, z)	case x: valp = &rt_metrics.z; flag = y; break
	caseof(K_MTU, RTV_MTU, rmx_mtu);
	caseof(K_HOPCOUNT, RTV_HOPCOUNT, rmx_hopcount);
	caseof(K_EXPIRE, RTV_EXPIRE, rmx_expire);
	caseof(K_RECVPIPE, RTV_RPIPE, rmx_recvpipe);
	caseof(K_SENDPIPE, RTV_SPIPE, rmx_sendpipe);
	caseof(K_SSTHRESH, RTV_SSTHRESH, rmx_ssthresh);
	caseof(K_RTT, RTV_RTT, rmx_rtt);
	caseof(K_RTTVAR, RTV_RTTVAR, rmx_rttvar);
	}
	rtm_inits |= flag;
	if (lockrest || locking)
		rt_metrics.rmx_locks |= flag;
	if (locking)
		locking = 0;
	*valp = atoi(value);
}

void
newroute(argc, argv)
	int argc;
	register char **argv;
{
	char *cmd, *dest = "", *gateway = "", *err;
	int ishost = 0, ret, attempts, oerrno, flags = RTF_STATIC;
	int key;
	struct hostent *hp = 0;

	if (uid) {
		errx(EX_NOPERM, "must be root to alter routing table");
	}
	cmd = argv[0];
	if (*cmd != 'g')
		shutdown(s, 0); /* Don't want to read back our messages */
	while (--argc > 0) {
		if (**(++argv)== '-') {
			switch (key = keyword(1 + *argv)) {
			case K_LINK:
				af = AF_LINK;
				aflen = sizeof(struct sockaddr_dl);
				break;
			case K_INET:
				af = AF_INET;
				aflen = sizeof(struct sockaddr_in);
				break;
#ifdef AT
			case K_ATALK:
				af = AF_APPLETALK;
				aflen = sizeof(struct sockaddr_at);
				break;
#endif
			case K_SA:
				af = PF_ROUTE;
				aflen = sizeof(union sockunion);
				break;
#ifdef NS
			case K_XNS:
				af = AF_NS;
				aflen = sizeof(struct sockaddr_ns);
				break;
#endif
			case K_IFACE:
			case K_INTERFACE:
				iflag++;
				break;
			case K_NOSTATIC:
				flags &= ~RTF_STATIC;
				break;

			case K_LLINFO:
				flags |= RTF_LLINFO;
				break;

			case K_LOCK:
				locking = 1;
				break;
			case K_LOCKREST:
				lockrest = 1;
				break;
			case K_HOST:
				forcehost++;
				break;
			case K_REJECT:
				flags |= RTF_REJECT;
				break;
			case K_BLACKHOLE:
				flags |= RTF_BLACKHOLE;
				break;
			case K_PROTO1:
				flags |= RTF_PROTO1;
				break;
			case K_PROTO2:
				flags |= RTF_PROTO2;
				break;
			case K_CLONING:
				flags |= RTF_CLONING;
				break;
			case K_XRESOLVE:
				flags |= RTF_XRESOLVE;
				break;
			case K_STATIC:
				flags |= RTF_STATIC;
				break;
			case K_IFA:
				argc--;
				(void) getaddr(RTA_IFA, *++argv, 0);
				break;
			case K_IFP:
				argc--;
				(void) getaddr(RTA_IFP, *++argv, 0);
				break;
			case K_GENMASK:
				argc--;
				(void) getaddr(RTA_GENMASK, *++argv, 0);
				break;
			case K_GATEWAY:
				argc--;
				(void) getaddr(RTA_GATEWAY, *++argv, 0);
				break;
			case K_DST:
				argc--;
				ishost = getaddr(RTA_DST, *++argv, &hp);
				dest = *argv;
				break;
			case K_NETMASK:
				argc--;
				(void) getaddr(RTA_NETMASK, *++argv, 0);
				/* FALLTHROUGH */
			case K_NET:
				forcenet++;
				break;
			case K_MTU:
			case K_HOPCOUNT:
			case K_EXPIRE:
			case K_RECVPIPE:
			case K_SENDPIPE:
			case K_SSTHRESH:
			case K_RTT:
			case K_RTTVAR:
				argc--;
				set_metric(*++argv, key);
				break;
			default:
				usage(1+*argv);
			}
		} else {
			if ((rtm_addrs & RTA_DST) == 0) {
				dest = *argv;
				ishost = getaddr(RTA_DST, *argv, &hp);
			} else if ((rtm_addrs & RTA_GATEWAY) == 0) {
				gateway = *argv;
				(void) getaddr(RTA_GATEWAY, *argv, &hp);
			} else {
				(void) getaddr(RTA_NETMASK, *argv, 0);
			}
		}
	}
	if (forcehost)
		ishost = 1;
	if (forcenet)
		ishost = 0;
	flags |= RTF_UP;
	if (ishost)
		flags |= RTF_HOST;
	if (iflag == 0)
		flags |= RTF_GATEWAY;
	for (attempts = 1; ; attempts++) {
		errno = 0;
		if ((ret = rtmsg(*cmd, flags)) == 0)
			break;
		if (errno != ENETUNREACH && errno != ESRCH)
			break;
		if (af == AF_INET && *gateway && hp && hp->h_addr_list[1]) {
			hp->h_addr_list++;
			bcopy(hp->h_addr_list[0], &so_gate.sin.sin_addr,
			    MIN(hp->h_length, sizeof(so_gate.sin.sin_addr)));
		} else
			break;
	}
	if (*cmd == 'g')
		exit(0);
	oerrno = errno;
	(void) printf("%s %s %s", cmd, ishost? "host" : "net", dest);
	if (*gateway) {
		(void) printf(": gateway %s", gateway);
		if (attempts > 1 && ret == 0 && af == AF_INET)
		    (void) printf(" (%s)",
			inet_ntoa(((struct sockaddr_in *)&route.rt_gateway)->sin_addr));
	}
	if (ret == 0)
		(void) printf("\n");
	else {
		switch (oerrno) {
		case ESRCH:
			err = "not in table";
			break;
		case EBUSY:
			err = "entry in use";
			break;
		case ENOBUFS:
			err = "routing table overflow";
			break;
		default:
			err = strerror(oerrno);
			break;
		}
		(void) printf(": %s\n", err);
	}
}

void
inet_makenetandmask(net, sin, bits)
	u_long net, bits;
	register struct sockaddr_in *sin;
{
	u_long addr, mask = 0;
	register char *cp;

	rtm_addrs |= RTA_NETMASK;
	if (net == 0)
		mask = addr = 0;
	else if (bits) {
		addr = net;
		mask = 0xffffffff << (32 - bits);
	} else if (net < 128) {
		addr = net << IN_CLASSA_NSHIFT;
		mask = IN_CLASSA_NET;
	} else if (net < 65536) {
		addr = net << IN_CLASSB_NSHIFT;
		mask = IN_CLASSB_NET;
	} else if (net < 16777216L) {
		addr = net << IN_CLASSC_NSHIFT;
		mask = IN_CLASSC_NET;
	} else {
		addr = net;
		if ((addr & IN_CLASSA_HOST) == 0)
			mask =  IN_CLASSA_NET;
		else if ((addr & IN_CLASSB_HOST) == 0)
			mask =  IN_CLASSB_NET;
		else if ((addr & IN_CLASSC_HOST) == 0)
			mask =  IN_CLASSC_NET;
		else
			mask = -1;
	}
	sin->sin_addr.s_addr = htonl(addr);
	sin = &so_mask.sin;
	sin->sin_addr.s_addr = htonl(mask);
	sin->sin_len = 0;
	sin->sin_family = 0;
	cp = (char *)(&sin->sin_addr + 1);
	while (*--cp == 0 && cp > (char *)sin)
		;
	sin->sin_len = 1 + cp - (char *)sin;
}

/*
 * Interpret an argument as a network address of some kind,
 * returning 1 if a host address, 0 if a network address.
 */
int
getaddr(which, s, hpp)
	int which;
	char *s;
	struct hostent **hpp;
{
	register sup su;
#ifdef NS
	struct ns_addr ns_addr();
#endif
	struct hostent *hp;
	struct netent *np;
	u_long val;
	char *q,qs;

	if (af == 0) {
		af = AF_INET;
		aflen = sizeof(struct sockaddr_in);
	}
	rtm_addrs |= which;
	switch (which) {
	case RTA_DST:
		su = &so_dst;
		break;
	case RTA_GATEWAY:
		su = &so_gate;
		if (iflag) {
			#define MAX_IFACES	400
			int			sock;
			struct ifreq		iflist[MAX_IFACES];
			struct ifconf		ifconf;
			struct ifreq		*ifr, *ifr_end;
			struct sockaddr_dl	*dl, *sdl = NULL;

			/* Get socket */
			if ((sock = socket(PF_INET, SOCK_DGRAM, 0)) < 0)
				err(1, "socket");

			/* Get interface list */
			ifconf.ifc_req = iflist;
			ifconf.ifc_len = sizeof(iflist);
			if (ioctl(sock, SIOCGIFCONF, &ifconf) < 0)
				err(1, "ioctl(SIOCGIFCONF)");
			close(sock);

			/* Look for this interface in the list */
			for (ifr = ifconf.ifc_req,
			    ifr_end = (struct ifreq *)
				(ifconf.ifc_buf + ifconf.ifc_len);
			    ifr < ifr_end;
			    ifr = (struct ifreq *) ((char *) &ifr->ifr_addr
						    + ifr->ifr_addr.sa_len)) {
				dl = (struct sockaddr_dl *)&ifr->ifr_addr;
				if (ifr->ifr_addr.sa_family == AF_LINK
				    && (ifr->ifr_flags & IFF_POINTOPOINT)
				    && !strncmp(s, dl->sdl_data, dl->sdl_nlen)
				    && s[dl->sdl_nlen] == 0) {
					sdl = dl;
					break;
				}
			}

			/* If we found it, then use it */
			if (sdl) {
				su->sdl = *sdl;
				return(1);
			}
		}
		break;
	case RTA_NETMASK:
		su = &so_mask;
		break;
	case RTA_GENMASK:
		su = &so_genmask;
		break;
	case RTA_IFP:
		su = &so_ifp;
		break;
	case RTA_IFA:
		su = &so_ifa;
		break;
	default:
		usage("internal error");
		/*NOTREACHED*/
	}
	su->sa.sa_len = aflen;
	su->sa.sa_family = af; /* cases that don't want it have left already */
	if (strcmp(s, "default") == 0) {
		/*
		 * Default is net 0.0.0.0/0 
		 */
		switch (which) {
		case RTA_DST:
			forcenet++;
			/* bzero(su, sizeof(*su)); *//* for readability */
			(void) getaddr(RTA_NETMASK, s, 0);
			break;
#if 0
		case RTA_NETMASK:
		case RTA_GENMASK:
			/* bzero(su, sizeof(*su)); *//* for readability */
#endif
		}
		return (0);
	}
	switch (af) {
#ifdef NS
	case AF_NS:
		if (which == RTA_DST) {
			extern short ns_bh[3];
			struct sockaddr_ns *sms = &(so_mask.sns);
			bzero((char *)sms, sizeof(*sms));
			sms->sns_family = 0;
			sms->sns_len = 6;
			sms->sns_addr.x_net = *(union ns_net *)ns_bh;
			rtm_addrs |= RTA_NETMASK;
		}
		su->sns.sns_addr = ns_addr(s);
		return (!ns_nullhost(su->sns.sns_addr));
#endif

#ifdef AT
	case AF_APPLETALK:
		if (!atalk_aton(s, &su->sat.sat_addr))
			errx(EX_NOHOST, "bad address: %s", s);
		rtm_addrs |= RTA_NETMASK;
		return(forcehost || su->sat.sat_addr.s_node != 0);
#endif
	case AF_LINK:
		link_addr(s, &su->sdl);
		return (1);


	case PF_ROUTE:
		su->sa.sa_len = sizeof(*su);
		sockaddr(s, &su->sa);
		return (1);

	case AF_INET:
	default:
		break;
	}

	if (hpp == NULL)
		hpp = &hp;
	*hpp = NULL;

	q = strchr(s,'/');
	if (q && which == RTA_DST) {
		qs = *q;
		*q = '\0';
		if (((val = inet_addr(s)) != INADDR_NONE)) {
			inet_makenetandmask(
				htonl(val), &su->sin, strtoul(q+1, 0, 0));
			return (0);
		}
		*q =qs;
	}
	if (((val = inet_addr(s)) != INADDR_NONE) &&
	    (which != RTA_DST || forcenet == 0)) {
		su->sin.sin_addr.s_addr = val;
		if (inet_lnaof(su->sin.sin_addr) != INADDR_ANY)
			return (1);
		else {
			val = ntohl(val);
			goto netdone;
		}
	}
	if ((val = inet_network(s)) != INADDR_NONE ||
	    (forcehost == 0 && (np = getnetbyname(s)) != NULL &&
		    (val = np->n_net) != 0)) {
netdone:
		if (which == RTA_DST)
			inet_makenetandmask(val, &su->sin, 0);
		return (0);
	}
	hp = gethostbyname(s);
	if (hp) {
		*hpp = hp;
		su->sin.sin_family = hp->h_addrtype;
		bcopy(hp->h_addr, (char *)&su->sin.sin_addr, 
		    MIN(hp->h_length, sizeof(su->sin.sin_addr)));
		return (1);
	}
	errx(EX_NOHOST, "bad address: %s", s);
}


#ifdef NS
short ns_nullh[] = {0,0,0};
short ns_bh[] = {-1,-1,-1};

char *
ns_print(sns)
	struct sockaddr_ns *sns;
{
	struct ns_addr work;
	union { union ns_net net_e; u_long long_e; } net;
	u_short port;
	static char mybuf[50+MAXHOSTNAMELEN], cport[10], chost[25];
	char *host = "";
	register char *p;
	register u_char *q;

	work = sns->sns_addr;
	port = ntohs(work.x_port);
	work.x_port = 0;
	net.net_e  = work.x_net;
	if (ns_nullhost(work) && net.long_e == 0) {
		if (!port)
			return ("*.*");
		(void) sprintf(mybuf, "*.%XH", port);
		return (mybuf);
	}

	if (bcmp((char *)ns_bh, (char *)work.x_host.c_host, 6) == 0)
		host = "any";
	else if (bcmp((char *)ns_nullh, (char *)work.x_host.c_host, 6) == 0)
		host = "*";
	else {
		q = work.x_host.c_host;
		(void) sprintf(chost, "%02X%02X%02X%02X%02X%02XH",
			q[0], q[1], q[2], q[3], q[4], q[5]);
		for (p = chost; *p == '0' && p < chost + 12; p++)
			/* void */;
		host = p;
	}
	if (port)
		(void) sprintf(cport, ".%XH", htons(port));
	else
		*cport = 0;

	(void) snprintf(mybuf, sizeof(mybuf), "%lxH.%s%s", 
                    (unsigned long)ntohl(net.long_e),
		       host, cport);
	return (mybuf);
}
#endif

void
interfaces()
{
	size_t needed;
	int mib[6];
	char *buf, *lim, *next;
	register struct rt_msghdr *rtm;

	mib[0] = CTL_NET;
	mib[1] = PF_ROUTE;
	mib[2] = 0;		/* protocol */
	mib[3] = 0;		/* wildcard address family */
	mib[4] = NET_RT_IFLIST;
	mib[5] = 0;		/* no flags */
	if (sysctl(mib, 6, NULL, &needed, NULL, 0) < 0)
		err(EX_OSERR, "route-sysctl-estimate");
	if ((buf = malloc(needed)) == NULL)
		errx(EX_OSERR, "malloc failed");
	if (sysctl(mib, 6, buf, &needed, NULL, 0) < 0)
		err(EX_OSERR, "actual retrieval of interface table");
	lim = buf + needed;
	for (next = buf; next < lim; next += rtm->rtm_msglen) {
		rtm = (struct rt_msghdr *)next;
		print_rtmsg(rtm, rtm->rtm_msglen);
	}
}

void
monitor()
{
	int n;
	char msg[2048];

	verbose = 1;
	if (debugonly) {
		interfaces();
		exit(0);
	}
	for(;;) {
		n = read(s, msg, 2048);
		(void) printf("got message of size %d\n", n);
		print_rtmsg((struct rt_msghdr *)msg, n);
	}
}

struct {
	struct	rt_msghdr m_rtm;
	char	m_space[512];
} m_rtmsg;

int
rtmsg(cmd, flags)
	int cmd, flags;
{
	static int seq;
	int rlen;
	register char *cp = m_rtmsg.m_space;
	register int l;

#define NEXTADDR(w, u) \
	if (rtm_addrs & (w)) {\
	    l = ROUNDUP(u.sa.sa_len); bcopy((char *)&(u), cp, l); cp += l;\
	    if (verbose) sodump(&(u),"u");\
	}

	errno = 0;
	bzero((char *)&m_rtmsg, sizeof(m_rtmsg));
	if (cmd == 'a')
		cmd = RTM_ADD;
	else if (cmd == 'c')
		cmd = RTM_CHANGE;
	else if (cmd == 'g') {
		cmd = RTM_GET;
		if (so_ifp.sa.sa_family == 0) {
			so_ifp.sa.sa_family = AF_LINK;
			so_ifp.sa.sa_len = sizeof(struct sockaddr_dl);
			rtm_addrs |= RTA_IFP;
		}
	} else
		cmd = RTM_DELETE;
#define rtm m_rtmsg.m_rtm
	rtm.rtm_type = cmd;
	rtm.rtm_flags = flags;
	rtm.rtm_version = RTM_VERSION;
	rtm.rtm_seq = ++seq;
	rtm.rtm_addrs = rtm_addrs;
	rtm.rtm_rmx = rt_metrics;
	rtm.rtm_inits = rtm_inits;

	if (rtm_addrs & RTA_NETMASK)
		mask_addr();
	NEXTADDR(RTA_DST, so_dst);
	NEXTADDR(RTA_GATEWAY, so_gate);
	NEXTADDR(RTA_NETMASK, so_mask);
	NEXTADDR(RTA_GENMASK, so_genmask);
	NEXTADDR(RTA_IFP, so_ifp);
	NEXTADDR(RTA_IFA, so_ifa);
	rtm.rtm_msglen = l = cp - (char *)&m_rtmsg;
	if (verbose)
		print_rtmsg(&rtm, l);
	if (debugonly)
		return (0);
	if ((rlen = write(s, (char *)&m_rtmsg, l)) < 0) {
		warn("writing to routing socket");
		return (-1);
	}
	if (cmd == RTM_GET) {
		do {
			l = read(s, (char *)&m_rtmsg, sizeof(m_rtmsg));
		} while (l > 0 && (rtm.rtm_seq != seq || rtm.rtm_pid != pid));
		if (l < 0)
			warn("read from routing socket");
		else
			print_getmsg(&rtm, l);
	}
#undef rtm
	return (0);
}

void
mask_addr()
{
	int olen = so_mask.sa.sa_len;
	register char *cp1 = olen + (char *)&so_mask, *cp2;

	for (so_mask.sa.sa_len = 0; cp1 > (char *)&so_mask; )
		if (*--cp1 != 0) {
			so_mask.sa.sa_len = 1 + cp1 - (char *)&so_mask;
			break;
		}
	if ((rtm_addrs & RTA_DST) == 0)
		return;
	switch (so_dst.sa.sa_family) {
#ifdef NS
	case AF_NS:
#endif
	case AF_INET:
	case AF_APPLETALK:
	case 0:
		return;
	}
	cp1 = so_mask.sa.sa_len + 1 + (char *)&so_dst;
	cp2 = so_dst.sa.sa_len + 1 + (char *)&so_dst;
	while (cp2 > cp1)
		*--cp2 = 0;
	cp2 = so_mask.sa.sa_len + 1 + (char *)&so_mask;
	while (cp1 > so_dst.sa.sa_data)
		*--cp1 &= *--cp2;
}

char *msgtypes[] = {
	"",
	"RTM_ADD: Add Route",
	"RTM_DELETE: Delete Route",
	"RTM_CHANGE: Change Metrics or flags",
	"RTM_GET: Report Metrics",
	"RTM_LOSING: Kernel Suspects Partitioning",
	"RTM_REDIRECT: Told to use different route",
	"RTM_MISS: Lookup failed on this address",
	"RTM_LOCK: fix specified metrics",
	"RTM_OLDADD: caused by SIOCADDRT",
	"RTM_OLDDEL: caused by SIOCDELRT",
	"RTM_RESOLVE: Route created by cloning",
	"RTM_NEWADDR: address being added to iface",
	"RTM_DELADDR: address being removed from iface",
	"RTM_IFINFO: iface status change",
	"RTM_NEWMADDR: new multicast group membership on iface",
	"RTM_DELMADDR: multicast group membership removed from iface",
	0,
};

char metricnames[] =
"\011pksent\010rttvar\7rtt\6ssthresh\5sendpipe\4recvpipe\3expire\2hopcount"
"\1mtu";
char routeflags[] =
"\1UP\2GATEWAY\3HOST\4REJECT\5DYNAMIC\6MODIFIED\7DONE\010MASK_PRESENT"
"\011CLONING\012XRESOLVE\013LLINFO\014STATIC\015BLACKHOLE\016b016"
"\017PROTO2\020PROTO1\021PRCLONING\022WASCLONED\023PROTO3\024CHAINDELETE"
"\025PINNED\026LOCAL\027BROADCAST\030MULTICAST";
char ifnetflags[] =
"\1UP\2BROADCAST\3DEBUG\4LOOPBACK\5PTP\6b6\7RUNNING\010NOARP"
"\011PPROMISC\012ALLMULTI\013OACTIVE\014SIMPLEX\015LINK0\016LINK1"
"\017LINK2\020MULTICAST";
char addrnames[] =
"\1DST\2GATEWAY\3NETMASK\4GENMASK\5IFP\6IFA\7AUTHOR\010BRD";

void
print_rtmsg(rtm, msglen)
	register struct rt_msghdr *rtm;
	int msglen;
{
	struct if_msghdr *ifm;
	struct ifa_msghdr *ifam;
#ifdef RTM_NEWMADDR
	struct ifma_msghdr *ifmam;
#endif

	if (verbose == 0)
		return;
	if (rtm->rtm_version != RTM_VERSION) {
		(void) printf("routing message version %d not understood\n",
		    rtm->rtm_version);
		return;
	}
	(void)printf("%s: len %d, ", msgtypes[rtm->rtm_type], rtm->rtm_msglen);
	switch (rtm->rtm_type) {
	case RTM_IFINFO:
		ifm = (struct if_msghdr *)rtm;
		(void) printf("if# %d, flags:", ifm->ifm_index);
		bprintf(stdout, ifm->ifm_flags, ifnetflags);
		pmsg_addrs((char *)(ifm + 1), ifm->ifm_addrs);
		break;
	case RTM_NEWADDR:
	case RTM_DELADDR:
		ifam = (struct ifa_msghdr *)rtm;
		(void) printf("metric %d, flags:", ifam->ifam_metric);
		bprintf(stdout, ifam->ifam_flags, routeflags);
		pmsg_addrs((char *)(ifam + 1), ifam->ifam_addrs);
		break;
#ifdef RTM_NEWMADDR
	case RTM_NEWMADDR:
	case RTM_DELMADDR:
		ifmam = (struct ifma_msghdr *)rtm;
		pmsg_addrs((char *)(ifmam + 1), ifmam->ifmam_addrs);
		break;
#endif
	default:
		(void) printf("pid: %ld, seq %d, errno %d, flags:",
			(long)rtm->rtm_pid, rtm->rtm_seq, rtm->rtm_errno);
		bprintf(stdout, rtm->rtm_flags, routeflags);
		pmsg_common(rtm);
	}
}

void
print_getmsg(rtm, msglen)
	register struct rt_msghdr *rtm;
	int msglen;
{
	struct sockaddr *dst = NULL, *gate = NULL, *mask = NULL;
	struct sockaddr_dl *ifp = NULL;
	register struct sockaddr *sa;
	register char *cp;
	register int i;

	(void) printf("   route to: %s\n", routename(&so_dst));
	if (rtm->rtm_version != RTM_VERSION) {
		warnx("routing message version %d not understood",
		     rtm->rtm_version);
		return;
	}
	if (rtm->rtm_msglen > msglen) {
		warnx("message length mismatch, in packet %d, returned %d",
		      rtm->rtm_msglen, msglen);
	}
	if (rtm->rtm_errno)  {
		errno = rtm->rtm_errno;
		warn("message indicates error %d", errno);
		return;
	}
	cp = ((char *)(rtm + 1));
	if (rtm->rtm_addrs)
		for (i = 1; i; i <<= 1)
			if (i & rtm->rtm_addrs) {
				sa = (struct sockaddr *)cp;
				switch (i) {
				case RTA_DST:
					dst = sa;
					break;
				case RTA_GATEWAY:
					gate = sa;
					break;
				case RTA_NETMASK:
					mask = sa;
					break;
				case RTA_IFP:
					if (sa->sa_family == AF_LINK &&
					   ((struct sockaddr_dl *)sa)->sdl_nlen)
						ifp = (struct sockaddr_dl *)sa;
					break;
				}
				ADVANCE(cp, sa);
			}
	if (dst && mask)
		mask->sa_family = dst->sa_family;	/* XXX */
	if (dst)
		(void)printf("destination: %s\n", routename(dst));
	if (mask) {
		int savenflag = nflag;

		nflag = 1;
		(void)printf("       mask: %s\n", routename(mask));
		nflag = savenflag;
	}
	if (gate && rtm->rtm_flags & RTF_GATEWAY)
		(void)printf("    gateway: %s\n", routename(gate));
	if (ifp)
		(void)printf("  interface: %.*s\n",
		    ifp->sdl_nlen, ifp->sdl_data);
	(void)printf("      flags: ");
	bprintf(stdout, rtm->rtm_flags, routeflags);

#define lock(f)	((rtm->rtm_rmx.rmx_locks & __CONCAT(RTV_,f)) ? 'L' : ' ')
#define msec(u)	(((u) + 500) / 1000)		/* usec to msec */

	(void) printf("\n%s\n", "\
 recvpipe  sendpipe  ssthresh  rtt,msec    rttvar  hopcount      mtu     expire");
	printf("%8ld%c ", rtm->rtm_rmx.rmx_recvpipe, lock(RPIPE));
	printf("%8ld%c ", rtm->rtm_rmx.rmx_sendpipe, lock(SPIPE));
	printf("%8ld%c ", rtm->rtm_rmx.rmx_ssthresh, lock(SSTHRESH));
	printf("%8ld%c ", msec(rtm->rtm_rmx.rmx_rtt), lock(RTT));
	printf("%8ld%c ", msec(rtm->rtm_rmx.rmx_rttvar), lock(RTTVAR));
	printf("%8ld%c ", rtm->rtm_rmx.rmx_hopcount, lock(HOPCOUNT));
	printf("%8ld%c ", rtm->rtm_rmx.rmx_mtu, lock(MTU));
	if (rtm->rtm_rmx.rmx_expire)
		rtm->rtm_rmx.rmx_expire -= time(0);
	printf("%8ld%c\n", rtm->rtm_rmx.rmx_expire, lock(EXPIRE));
#undef lock
#undef msec
#define	RTA_IGN	(RTA_DST|RTA_GATEWAY|RTA_NETMASK|RTA_IFP|RTA_IFA|RTA_BRD)
	if (verbose)
		pmsg_common(rtm);
	else if (rtm->rtm_addrs &~ RTA_IGN) {
		(void) printf("sockaddrs: ");
		bprintf(stdout, rtm->rtm_addrs, addrnames);
		putchar('\n');
	}
#undef	RTA_IGN
}

void
pmsg_common(rtm)
	register struct rt_msghdr *rtm;
{
	(void) printf("\nlocks: ");
	bprintf(stdout, rtm->rtm_rmx.rmx_locks, metricnames);
	(void) printf(" inits: ");
	bprintf(stdout, rtm->rtm_inits, metricnames);
	pmsg_addrs(((char *)(rtm + 1)), rtm->rtm_addrs);
}

void
pmsg_addrs(cp, addrs)
	char	*cp;
	int	addrs;
{
	register struct sockaddr *sa;
	int i;

	if (addrs == 0)
		return;
	(void) printf("\nsockaddrs: ");
	bprintf(stdout, addrs, addrnames);
	(void) putchar('\n');
	for (i = 1; i; i <<= 1)
		if (i & addrs) {
			sa = (struct sockaddr *)cp;
			(void) printf(" %s", routename(sa));
			ADVANCE(cp, sa);
		}
	(void) putchar('\n');
	(void) fflush(stdout);
}

void
bprintf(fp, b, s)
	register FILE *fp;
	register int b;
	register u_char *s;
{
	register int i;
	int gotsome = 0;

	if (b == 0)
		return;
	while ((i = *s++) != 0) {
		if (b & (1 << (i-1))) {
			if (gotsome == 0)
				i = '<';
			else
				i = ',';
			(void) putc(i, fp);
			gotsome = 1;
			for (; (i = *s) > 32; s++)
				(void) putc(i, fp);
		} else
			while (*s > 32)
				s++;
	}
	if (gotsome)
		(void) putc('>', fp);
}

int
keyword(cp)
	char *cp;
{
	register struct keytab *kt = keywords;

	while (kt->kt_cp && strcmp(kt->kt_cp, cp))
		kt++;
	return kt->kt_i;
}

void
sodump(su, which)
	register sup su;
	char *which;
{
	switch (su->sa.sa_family) {
	case AF_LINK:
		(void) printf("%s: link %s; ",
		    which, link_ntoa(&su->sdl));
		break;
	case AF_INET:
		(void) printf("%s: inet %s; ",
		    which, inet_ntoa(su->sin.sin_addr));
		break;
#ifdef AT
	case AF_APPLETALK:
		(void) printf("%s: atalk %s; ",
		    which, atalk_ntoa(su->sat.sat_addr));
		break;
#endif
#ifdef NS
	case AF_NS:
		(void) printf("%s: xns %s; ",
		    which, ns_ntoa(su->sns.sns_addr));
		break;
#endif
	}
	(void) fflush(stdout);
}

/* States*/
#define VIRGIN	0
#define GOTONE	1
#define GOTTWO	2
/* Inputs */
#define	DIGIT	(4*0)
#define	END	(4*1)
#define DELIM	(4*2)

void
sockaddr(addr, sa)
	register char *addr;
	register struct sockaddr *sa;
{
	register char *cp = (char *)sa;
	int size = sa->sa_len;
	char *cplim = cp + size;
	register int byte = 0, state = VIRGIN, new = 0 /* foil gcc */;

	bzero(cp, size);
	cp++;
	do {
		if ((*addr >= '0') && (*addr <= '9')) {
			new = *addr - '0';
		} else if ((*addr >= 'a') && (*addr <= 'f')) {
			new = *addr - 'a' + 10;
		} else if ((*addr >= 'A') && (*addr <= 'F')) {
			new = *addr - 'A' + 10;
		} else if (*addr == 0)
			state |= END;
		else
			state |= DELIM;
		addr++;
		switch (state /* | INPUT */) {
		case GOTTWO | DIGIT:
			*cp++ = byte; /*FALLTHROUGH*/
		case VIRGIN | DIGIT:
			state = GOTONE; byte = new; continue;
		case GOTONE | DIGIT:
			state = GOTTWO; byte = new + (byte << 4); continue;
		default: /* | DELIM */
			state = VIRGIN; *cp++ = byte; byte = 0; continue;
		case GOTONE | END:
		case GOTTWO | END:
			*cp++ = byte; /* FALLTHROUGH */
		case VIRGIN | END:
			break;
		}
		break;
	} while (cp < cplim);
	sa->sa_len = cp - (char *)sa;
}

#ifdef AT
int
atalk_aton(const char *text, struct at_addr *addr)
{
	u_int net, node;

	if (sscanf(text, "%u.%u", &net, &node) != 2
	    || net > 0xffff || node > 0xff)
		return(0);
	addr->s_net = htons(net);
	addr->s_node = node;
	return(1);
}

char *
atalk_ntoa(struct at_addr at)
{
	static char buf[20];

	(void) snprintf(buf, sizeof(buf), "%u.%u", ntohs(at.s_net), at.s_node);
	return(buf);
}
#endif
