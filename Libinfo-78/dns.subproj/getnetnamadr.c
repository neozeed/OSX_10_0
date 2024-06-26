/*
 * Copyright (c) 1999 Apple Computer, Inc. All rights reserved.
 *
 * @APPLE_LICENSE_HEADER_START@
 * 
 * Portions Copyright (c) 1999 Apple Computer, Inc.  All Rights
 * Reserved.  This file contains Original Code and/or Modifications of
 * Original Code as defined in and that are subject to the Apple Public
 * Source License Version 1.1 (the "License").  You may not use this file
 * except in compliance with the License.  Please obtain a copy of the
 * License at http://www.apple.com/publicsource and read it before using
 * this file.
 * 
 * The Original Code and all software distributed under the License are
 * distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY KIND, EITHER
 * EXPRESS OR IMPLIED, AND APPLE HEREBY DISCLAIMS ALL SUCH WARRANTIES,
 * INCLUDING WITHOUT LIMITATION, ANY WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE OR NON- INFRINGEMENT.  Please see the
 * License for the specific language governing rights and limitations
 * under the License.
 * 
 * @APPLE_LICENSE_HEADER_END@
 */
/* Copyright (c) 1993 Carlos Leandro and Rui Salgueiro
 *	Dep. Matematica Universidade de Coimbra, Portugal, Europe
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 */
/*
 * Copyright (c) 1983, 1993
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

#if defined(LIBC_SCCS) && !defined(lint)
static char sccsid[] = "@(#)getnetbyaddr.c	8.1 (Berkeley) 6/4/93";
static char sccsid_[] = "from getnetnamadr.c	1.4 (Coimbra) 93/06/03";
static char rcsid[] = "$Id: getnetnamadr.c,v 1.2 1999/10/14 21:56:44 wsanchez Exp $";
#endif /* LIBC_SCCS and not lint */

#include <sys/param.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <arpa/nameser.h>

#include <stdio.h>
#include <netdb.h>
#include <resolv.h>
#include <ctype.h>
#include <errno.h>
#include <string.h>

extern int h_errno;

#if defined(mips) && defined(SYSTYPE_BSD43)
extern int errno;
#endif

struct netent *_getnetbyaddr __P((long net, int type));
struct netent *_getnetbyname __P((const char *name));

#define BYADDR 0
#define BYNAME 1
#define	MAXALIASES	35

#if PACKETSZ > 1024
#define	MAXPACKET	PACKETSZ
#else
#define	MAXPACKET	1024
#endif

typedef union {
	HEADER	hdr;
	u_char	buf[MAXPACKET];
} querybuf;

typedef union {
	long	al;
	char	ac;
} align;

static struct netent *
getnetanswer(answer, anslen, net_i)
	querybuf *answer;
	int anslen;
	int net_i;
{

	register HEADER *hp;
	register u_char *cp;
	register int n;
	u_char *eom;
	int type, class, buflen, ancount, qdcount, haveanswer, i, nchar;
	char aux1[30], aux2[30], ans[30], *in, *st, *pauxt, *bp, **ap,
		*paux1 = &aux1[0], *paux2 = &aux2[0], flag = 0;
static	struct netent net_entry;
static	char *net_aliases[MAXALIASES], netbuf[BUFSIZ+1];

	/*
	 * find first satisfactory answer
	 *
	 *      answer --> +------------+  ( MESSAGE )
	 *		   |   Header   |
	 *		   +------------+
	 *		   |  Question  | the question for the name server
	 *		   +------------+
	 *		   |   Answer   | RRs answering the question
	 *		   +------------+
	 *		   | Authority  | RRs pointing toward an authority
	 *		   | Additional | RRs holding additional information
	 *		   +------------+
	 */
	eom = answer->buf + anslen;
	hp = &answer->hdr;
	ancount = ntohs(hp->ancount); /* #/records in the answer section */
	qdcount = ntohs(hp->qdcount); /* #/entries in the question section */
	bp = netbuf;
	buflen = sizeof(netbuf);
	cp = answer->buf + HFIXEDSZ;
	if (!qdcount) {
		if (hp->aa)
			h_errno = HOST_NOT_FOUND;
		else
			h_errno = TRY_AGAIN;
		return (NULL);
	}
	while (qdcount-- > 0)
		cp += __dn_skipname(cp, eom) + QFIXEDSZ;
	ap = net_aliases;
	*ap = NULL;
	net_entry.n_aliases = net_aliases;
	haveanswer = 0;
	while (--ancount >= 0 && cp < eom) {
		n = dn_expand(answer->buf, eom, cp, bp, buflen);
		if (n < 0)
			break;
		cp += n;
		ans[0] = '\0';
		(void)strcpy(&ans[0], bp);
		GETSHORT(type, cp);
		GETSHORT(class, cp);
		cp += INT32SZ;		/* TTL */
		GETSHORT(n, cp);
		if (class == C_IN && type == T_PTR) {
			n = dn_expand(answer->buf, eom, cp, bp, buflen);
			if (n < 0) {
				cp += n;
				return (NULL);
			}
			cp += n; 
			*ap++ = bp;
			bp += strlen(bp) + 1;
			net_entry.n_addrtype =
				(class == C_IN) ? AF_INET : AF_UNSPEC;
			haveanswer++;
		}
	}
	if (haveanswer) {
		*ap = NULL;
		switch (net_i) {
		case BYADDR:
			net_entry.n_name = *net_entry.n_aliases;
			net_entry.n_net = 0L;
			break;
		case BYNAME:
			in = *net_entry.n_aliases;
			net_entry.n_name = &ans[0];
			aux2[0] = '\0';
			for (i = 0; i < 4; i++) {
				for (st = in, nchar = 0;
				     *st != '.';
				     st++, nchar++)
					;
				if (nchar != 1 || *in != '0' || flag) {
					flag = 1;
					(void)strncpy(paux1,
						      (i==0) ? in : in-1,
						      (i==0) ?nchar : nchar+1);
					paux1[(i==0) ? nchar : nchar+1] = '\0';
					pauxt = paux2;
					paux2 = strcat(paux1, paux2);
					paux1 = pauxt;
				}
				in = ++st;
			}		  
			net_entry.n_net = inet_network(paux2);
			break;
		}
		net_entry.n_aliases++;
		return (&net_entry);
	}
	h_errno = TRY_AGAIN;
	return (NULL);
}

struct netent *
getnetbyaddr(net, net_type)
	register long net;
	register int net_type;
{
	unsigned int netbr[4];
	int nn, anslen;
	querybuf buf;
	char qbuf[MAXDNAME];
	unsigned long net2;
	struct netent *net_entry;

	if (net_type != AF_INET)
#if defined(__APPLE__)
		return NULL;
#else
		return (_getnetbyaddr(net, net_type));
#endif /* !NeXT */

	for (nn = 4, net2 = net; net2; net2 >>= 8)
		netbr[--nn] = net2 & 0xff;
	switch (nn) {
	case 3: 	/* Class A */
		sprintf(qbuf, "0.0.0.%u.in-addr.arpa", netbr[3]);
		break;
	case 2: 	/* Class B */
		sprintf(qbuf, "0.0.%u.%u.in-addr.arpa", netbr[3], netbr[2]);
		break;
	case 1: 	/* Class C */
		sprintf(qbuf, "0.%u.%u.%u.in-addr.arpa", netbr[3], netbr[2],
		    netbr[1]);
		break;
	case 0: 	/* Class D - E */
		sprintf(qbuf, "%u.%u.%u.%u.in-addr.arpa", netbr[3], netbr[2],
		    netbr[1], netbr[0]);
		break;
	}
	anslen = res_query(qbuf, C_IN, T_PTR, (u_char *)&buf, sizeof(buf));
	if (anslen < 0) {
#ifdef DEBUG
		if (_res.options & RES_DEBUG)
			printf("res_query failed\n");
#endif
#if !defined(__APPLE__)
		if (errno == ECONNREFUSED)
			return (_getnetbyaddr(net, net_type));
#endif /* !NeXT */
		return (NULL);
	}
	net_entry = getnetanswer(&buf, anslen, BYADDR);
	if (net_entry) {
		unsigned u_net = net;	/* maybe net should be unsigned ? */

		/* Strip trailing zeros */
		while ((u_net & 0xff) == 0 && u_net != 0)
			u_net >>= 8;
		net_entry->n_net = u_net;
		return (net_entry);
	}
#if defined(__APPLE__)
	return NULL;
#else
	return (_getnetbyaddr(net, net_type));
#endif /* !NeXT */
}

struct netent *
getnetbyname(net)
	register const char *net;
{
	int anslen;
	querybuf buf;
	char qbuf[MAXDNAME];
	struct netent *net_entry;

	if ((_res.options & RES_INIT) == 0 && res_init() == -1) {
		h_errno = NETDB_INTERNAL;
		return (NULL);
	}
	strcpy(&qbuf[0], net);
	anslen = res_search(qbuf, C_IN, T_PTR, (u_char *)&buf, sizeof(buf));
	if (anslen < 0) {
#ifdef DEBUG
		if (_res.options & RES_DEBUG)
			printf("res_query failed\n");
#endif
#if defined(__APPLE__)
		return NULL;
#else
		if (errno == ECONNREFUSED)
			return (_getnetbyname(net));
		return (_getnetbyname(net));
#endif /* !NeXT */
	}
	net_entry = getnetanswer(&buf, anslen, BYNAME);
	if (net_entry)
		return (net_entry);
#if defined(__APPLE__)
	return NULL;
#else
	return (_getnetbyname(net));
#endif /* !NeXT */
}
