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
 * Copyright (c) 1990, 1991, 1992, 1993
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
 */

#include <sys/param.h>
#include <sys/types.h>
#include <sys/time.h>

#if defined(sparc) || defined(hppa)
#define BPF_ALIGN
#endif

#ifndef BPF_ALIGN
#define EXTRACT_SHORT(p)	((u_short)ntohs(*(u_short *)p))
#define EXTRACT_LONG(p)		(ntohl(*(u_int32_t *)p))
#else
#define EXTRACT_SHORT(p)\
	((u_short)\
		((u_short)*((u_char *)p+0)<<8|\
		 (u_short)*((u_char *)p+1)<<0))
#define EXTRACT_LONG(p)\
		((u_int32_t)*((u_char *)p+0)<<24|\
		 (u_int32_t)*((u_char *)p+1)<<16|\
		 (u_int32_t)*((u_char *)p+2)<<8|\
		 (u_int32_t)*((u_char *)p+3)<<0)
#endif

#ifdef KERNEL
#include <sys/mbuf.h>
#define MINDEX(m, k) \
{ \
	register int len = m->m_len; \
 \
	while (k >= len) { \
		k -= len; \
		m = m->m_next; \
		if (m == 0) \
			return 0; \
		len = m->m_len; \
	} \
}

static int
m_xword(m, k, err)
	register struct mbuf *m;
	register int k, *err;
{
	register int len;
	register u_char *cp, *np;
	register struct mbuf *m0;

	len = m->m_len;
	while (k >= len) {
		k -= len;
		m = m->m_next;
		if (m == 0)
			goto bad;
		len = m->m_len;
	}
	cp = mtod(m, u_char *) + k;
	if (len - k >= 4) {
		*err = 0;
		return EXTRACT_LONG(cp);
	}
	m0 = m->m_next;
	if (m0 == 0 || m0->m_len + len - k < 4)
		goto bad;
	*err = 0;
	np = mtod(m0, u_char *);
	switch (len - k) {

	case 1:
		return (cp[k] << 24) | (np[0] << 16) | (np[1] << 8) | np[2];

	case 2:
		return (cp[k] << 24) | (cp[k + 1] << 16) | (np[0] << 8) | 
			np[1];

	default:
		return (cp[k] << 24) | (cp[k + 1] << 16) | (cp[k + 2] << 8) |
			np[0];
	}
    bad:
	*err = 1;
	return 0;
}

static int
m_xhalf(m, k, err)
	register struct mbuf *m;
	register int k, *err;
{
	register int len;
	register u_char *cp;
	register struct mbuf *m0;

	len = m->m_len;
	while (k >= len) {
		k -= len;
		m = m->m_next;
		if (m == 0)
			goto bad;
		len = m->m_len;
	}
	cp = mtod(m, u_char *) + k;
	if (len - k >= 2) {
		*err = 0;
		return EXTRACT_SHORT(cp);
	}
	m0 = m->m_next;
	if (m0 == 0)
		goto bad;
	*err = 0;
	return (cp[k] << 8) | mtod(m0, u_char *)[0];
 bad:
	*err = 1;
	return 0;
}
#endif

#include <net/bpf.h>

/*
 * Execute the filter program starting at pc on the packet p
 * wirelen is the length of the original packet
 * buflen is the amount of data present
 */
u_int
bpf_filter(pc, p, wirelen, buflen)
	register struct bpf_insn *pc;
	register u_char *p;
	u_int wirelen;
	register u_int buflen;
{
	register u_int32_t A, X;
	register int k;
	int32_t mem[BPF_MEMWORDS];

	if (pc == 0)
		/*
		 * No filter means accept all.
		 */
		return (u_int)-1;
#ifdef lint
	A = 0;
	X = 0;
#endif
	--pc;
	while (1) {
		++pc;
		switch (pc->code) {

		default:
#ifdef KERNEL
			return 0;
#else
			abort();
#endif			
		case BPF_RET|BPF_K:
			return (u_int)pc->k;

		case BPF_RET|BPF_A:
			return (u_int)A;

		case BPF_LD|BPF_W|BPF_ABS:
			k = pc->k;
			if (k + sizeof(int32_t) > buflen) {
#ifdef KERNEL
				int merr;

				if (buflen != 0)
					return 0;
				A = m_xword((struct mbuf *)p, k, &merr);
				if (merr != 0)
					return 0;
				continue;
#else
				return 0;
#endif
			}
#ifdef BPF_ALIGN
			if (((int)(p + k) & 3) != 0)
				A = EXTRACT_LONG(&p[k]);
			else
#endif
				A = ntohl(*(long *)(p + k));
			continue;

		case BPF_LD|BPF_H|BPF_ABS:
			k = pc->k;
			if (k + sizeof(short) > buflen) {
#ifdef KERNEL
				int merr;

				if (buflen != 0)
					return 0;
				A = m_xhalf((struct mbuf *)p, k, &merr);
				continue;
#else
				return 0;
#endif
			}
			A = EXTRACT_SHORT(&p[k]);
			continue;

		case BPF_LD|BPF_B|BPF_ABS:
			k = pc->k;
			if (k >= buflen) {
#ifdef KERNEL
				register struct mbuf *m;

				if (buflen != 0)
					return 0;
				m = (struct mbuf *)p;
				MINDEX(m, k);
				A = mtod(m, u_char *)[k];
				continue;
#else
				return 0;
#endif
			}
			A = p[k];
			continue;

		case BPF_LD|BPF_W|BPF_LEN:
			A = wirelen;
			continue;

		case BPF_LDX|BPF_W|BPF_LEN:
			X = wirelen;
			continue;

		case BPF_LD|BPF_W|BPF_IND:
			k = X + pc->k;
			if (k + sizeof(int32_t) > buflen) {
#ifdef KERNEL
				int merr;

				if (buflen != 0)
					return 0;
				A = m_xword((struct mbuf *)p, k, &merr);
				if (merr != 0)
					return 0;
				continue;
#else
				return 0;
#endif
			}
#ifdef BPF_ALIGN
			if (((int)(p + k) & 3) != 0)
				A = EXTRACT_LONG(&p[k]);
			else
#endif
				A = ntohl(*(long *)(p + k));
			continue;

		case BPF_LD|BPF_H|BPF_IND:
			k = X + pc->k;
			if (k + sizeof(short) > buflen) {
#ifdef KERNEL
				int merr;

				if (buflen != 0)
					return 0;
				A = m_xhalf((struct mbuf *)p, k, &merr);
				if (merr != 0)
					return 0;
				continue;
#else
				return 0;
#endif
			}
			A = EXTRACT_SHORT(&p[k]);
			continue;

		case BPF_LD|BPF_B|BPF_IND:
			k = X + pc->k;
			if (k >= buflen) {
#ifdef KERNEL
				register struct mbuf *m;

				if (buflen != 0)
					return 0;
				m = (struct mbuf *)p;
				MINDEX(m, k);
				A = mtod(m, char *)[k];
				continue;
#else
				return 0;
#endif
			}
			A = p[k];
			continue;

		case BPF_LDX|BPF_MSH|BPF_B:
			k = pc->k;
			if (k >= buflen) {
#ifdef KERNEL
				register struct mbuf *m;

				if (buflen != 0)
					return 0;
				m = (struct mbuf *)p;
				MINDEX(m, k);
				X = (mtod(m, char *)[k] & 0xf) << 2;
				continue;
#else
				return 0;
#endif
			}
			X = (p[pc->k] & 0xf) << 2;
			continue;

		case BPF_LD|BPF_IMM:
			A = pc->k;
			continue;

		case BPF_LDX|BPF_IMM:
			X = pc->k;
			continue;

		case BPF_LD|BPF_MEM:
			A = mem[pc->k];
			continue;
			
		case BPF_LDX|BPF_MEM:
			X = mem[pc->k];
			continue;

		case BPF_ST:
			mem[pc->k] = A;
			continue;

		case BPF_STX:
			mem[pc->k] = X;
			continue;

		case BPF_JMP|BPF_JA:
			pc += pc->k;
			continue;

		case BPF_JMP|BPF_JGT|BPF_K:
			pc += (A > pc->k) ? pc->jt : pc->jf;
			continue;

		case BPF_JMP|BPF_JGE|BPF_K:
			pc += (A >= pc->k) ? pc->jt : pc->jf;
			continue;

		case BPF_JMP|BPF_JEQ|BPF_K:
			pc += (A == pc->k) ? pc->jt : pc->jf;
			continue;

		case BPF_JMP|BPF_JSET|BPF_K:
			pc += (A & pc->k) ? pc->jt : pc->jf;
			continue;

		case BPF_JMP|BPF_JGT|BPF_X:
			pc += (A > X) ? pc->jt : pc->jf;
			continue;

		case BPF_JMP|BPF_JGE|BPF_X:
			pc += (A >= X) ? pc->jt : pc->jf;
			continue;

		case BPF_JMP|BPF_JEQ|BPF_X:
			pc += (A == X) ? pc->jt : pc->jf;
			continue;

		case BPF_JMP|BPF_JSET|BPF_X:
			pc += (A & X) ? pc->jt : pc->jf;
			continue;

		case BPF_ALU|BPF_ADD|BPF_X:
			A += X;
			continue;
			
		case BPF_ALU|BPF_SUB|BPF_X:
			A -= X;
			continue;
			
		case BPF_ALU|BPF_MUL|BPF_X:
			A *= X;
			continue;
			
		case BPF_ALU|BPF_DIV|BPF_X:
			if (X == 0)
				return 0;
			A /= X;
			continue;
			
		case BPF_ALU|BPF_AND|BPF_X:
			A &= X;
			continue;
			
		case BPF_ALU|BPF_OR|BPF_X:
			A |= X;
			continue;

		case BPF_ALU|BPF_LSH|BPF_X:
			A <<= X;
			continue;

		case BPF_ALU|BPF_RSH|BPF_X:
			A >>= X;
			continue;

		case BPF_ALU|BPF_ADD|BPF_K:
			A += pc->k;
			continue;
			
		case BPF_ALU|BPF_SUB|BPF_K:
			A -= pc->k;
			continue;
			
		case BPF_ALU|BPF_MUL|BPF_K:
			A *= pc->k;
			continue;
			
		case BPF_ALU|BPF_DIV|BPF_K:
			A /= pc->k;
			continue;
			
		case BPF_ALU|BPF_AND|BPF_K:
			A &= pc->k;
			continue;
			
		case BPF_ALU|BPF_OR|BPF_K:
			A |= pc->k;
			continue;

		case BPF_ALU|BPF_LSH|BPF_K:
			A <<= pc->k;
			continue;

		case BPF_ALU|BPF_RSH|BPF_K:
			A >>= pc->k;
			continue;

		case BPF_ALU|BPF_NEG:
			A = -A;
			continue;

		case BPF_MISC|BPF_TAX:
			X = A;
			continue;

		case BPF_MISC|BPF_TXA:
			A = X;
			continue;
		}
	}
}

#ifdef KERNEL
/*
 * Return true if the 'fcode' is a valid filter program.
 * The constraints are that each jump be forward and to a valid
 * code.  The code must terminate with either an accept or reject. 
 * 'valid' is an array for use by the routine (it must be at least
 * 'len' bytes long).  
 *
 * The kernel needs to be able to verify an application's filter code.
 * Otherwise, a bogus program could easily crash the system.
 */
int
bpf_validate(f, len)
	struct bpf_insn *f;
	int len;
{
	register int i;
	register struct bpf_insn *p;

	for (i = 0; i < len; ++i) {
		/*
		 * Check that that jumps are forward, and within 
		 * the code block.
		 */
		p = &f[i];
		if (BPF_CLASS(p->code) == BPF_JMP) {
			register int from = i + 1;

			if (BPF_OP(p->code) == BPF_JA) {
				if (from + p->k >= len)
					return 0;
			}
			else if (from + p->jt >= len || from + p->jf >= len)
				return 0;
		}
		/*
		 * Check that memory operations use valid addresses.
		 */
		if ((BPF_CLASS(p->code) == BPF_ST ||
		     (BPF_CLASS(p->code) == BPF_LD && 
		      (p->code & 0xe0) == BPF_MEM)) &&
		    (p->k >= BPF_MEMWORDS || p->k < 0))
			return 0;
		/*
		 * Check for constant division by 0.
		 */
		if (p->code == (BPF_ALU|BPF_DIV|BPF_K) && p->k == 0)
			return 0;
	}
	return BPF_CLASS(f[len - 1].code) == BPF_RET;
}
#endif
