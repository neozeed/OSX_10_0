/*
 * Copyright (c) 2000 Apple Computer, Inc. All rights reserved.
 *
 * @APPLE_LICENSE_HEADER_START@
 * 
 * The contents of this file constitute Original Code as defined in and
 * are subject to the Apple Public Source License Version 1.1 (the
 * "License").  You may not use this file except in compliance with the
 * License.  Please obtain a copy of the License at
 * http://www.apple.com/publicsource and read it before using this file.
 * 
 * This Original Code and all software distributed under the License are
 * distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY KIND, EITHER
 * EXPRESS OR IMPLIED, AND APPLE HEREBY DISCLAIMS ALL SUCH WARRANTIES,
 * INCLUDING WITHOUT LIMITATION, ANY WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE OR NON-INFRINGEMENT.  Please see the
 * License for the specific language governing rights and limitations
 * under the License.
 * 
 * @APPLE_LICENSE_HEADER_END@
 */
/*
 * Copyright (C) 1999 WIDE Project.
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the project nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE PROJECT AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE PROJECT OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * $Id: ip_ecn.c,v 1.3 2000/11/22 01:12:12 zarzycki Exp $
 */
/*
 * ECN consideration on tunnel ingress/egress operation.
 * http://www.aciri.org/floyd/papers/draft-ipsec-ecn-00.txt
 */

#if defined(__FreeBSD__) && __FreeBSD__ >= 3
#include "opt_inet.h"
#endif

#include <sys/param.h>
#include <sys/systm.h>
#include <sys/malloc.h>
#include <sys/mbuf.h>
#include <sys/errno.h>

#if INET
#include <netinet/in.h>
#include <netinet/in_systm.h>
#include <netinet/ip.h>
#endif

#if INET6
#ifndef INET
#include <netinet/in.h>
#endif
#include <netinet/ip6.h>
#endif

#include <netinet/ip_ecn.h>

/*
 * modify outer ECN (TOS) field on ingress operation (tunnel encapsulation).
 * call it after you've done the default initialization/copy for the outer.
 */
void
ip_ecn_ingress(mode, outer, inner)
	int mode;
	u_int8_t *outer;
	u_int8_t *inner;
{
	if (!outer || !inner)
		panic("NULL pointer passed to ip_ecn_ingress");

	switch (mode) {
	case ECN_ALLOWED:		/* ECN allowed */
		*outer &= ~IPTOS_CE;
		break;
	case ECN_FORBIDDEN:		/* ECN forbidden */
		*outer &= ~(IPTOS_ECT | IPTOS_CE);
		break;
	case ECN_NOCARE:	/* no consideration to ECN */
		break;
	}
}

/*
 * modify inner ECN (TOS) field on egress operation (tunnel decapsulation).
 * call it after you've done the default initialization/copy for the inner.
 */
void
ip_ecn_egress(mode, outer, inner)
	int mode;
	u_int8_t *outer;
	u_int8_t *inner;
{
	if (!outer || !inner)
		panic("NULL pointer passed to ip_ecn_egress");

	switch (mode) {
	case ECN_ALLOWED:
		if (*outer & IPTOS_CE)
			*inner |= IPTOS_CE;
		break;
	case ECN_FORBIDDEN:		/* ECN forbidden */
	case ECN_NOCARE:	/* no consideration to ECN */
		break;
	}
}

#if INET6
void
ip6_ecn_ingress(mode, outer, inner)
	int mode;
	u_int32_t *outer;
	u_int32_t *inner;
{
	u_int8_t outer8, inner8;

	if (!outer || !inner)
		panic("NULL pointer passed to ip6_ecn_ingress");

	outer8 = (ntohl(*outer) >> 20) & 0xff;
	inner8 = (ntohl(*inner) >> 20) & 0xff;
	ip_ecn_ingress(mode, &outer8, &inner8);
	*outer &= ~htonl(0xff << 20);
	*outer |= htonl((u_int32_t)outer8 << 20);
}

void
ip6_ecn_egress(mode, outer, inner)
	int mode;
	u_int32_t *outer;
	u_int32_t *inner;
{
	u_int8_t outer8, inner8;

	if (!outer || !inner)
		panic("NULL pointer passed to ip6_ecn_egress");

	outer8 = (ntohl(*outer) >> 20) & 0xff;
	inner8 = (ntohl(*inner) >> 20) & 0xff;
	ip_ecn_egress(mode, &outer8, &inner8);
	*inner &= ~htonl(0xff << 20);
	*inner |= htonl((u_int32_t)inner8 << 20);
}
#endif
