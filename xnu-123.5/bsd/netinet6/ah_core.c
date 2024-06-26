/*
 * Copyright (C) 1995, 1996, 1997, and 1998 WIDE Project.
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
 */

/*
 * RFC1826/2402 authentication header.
 */
#define _IP_VHL
#if (defined(__FreeBSD__) && __FreeBSD__ >= 3) || defined(__NetBSD__)
#include "opt_inet.h"
#if __NetBSD__	/*XXX*/
#include "opt_ipsec.h"
#endif
#endif

/* Some of operating systems have standard crypto checksum library */
#if __NetBSD__
#define HAVE_MD5
#define HAVE_SHA1
#endif
#if defined(__FreeBSD__) || defined(__APPLE__)
#define HAVE_MD5 1
#endif

#include <sys/param.h>
#include <sys/systm.h>
#include <sys/malloc.h>
#include <sys/mbuf.h>
#include <sys/domain.h>
#include <sys/protosw.h>
#include <sys/socket.h>
#include <sys/socketvar.h>
#include <sys/errno.h>
#include <sys/time.h>
#include <sys/kernel.h>
#include <sys/syslog.h>

#include <net/if.h>
#include <net/route.h>

#include <netinet/in.h>
#include <netinet/in_systm.h>
#include <netinet/ip.h>
#include <netinet/in_var.h>

#if INET6
#include <netinet/ip6.h>
#include <netinet6/ip6_var.h>
#include <netinet/icmp6.h>
#endif

#include <netinet6/ipsec.h>
#include <netinet6/ah.h>
#if IPSEC_ESP
#include <netinet6/esp.h>
#endif
#include <net/pfkeyv2.h>
#include <netkey/keydb.h>
#if HAVE_MD5
#include <sys/md5.h>
#else
#include <crypto/md5.h>
#endif
#if HAVE_SHA1
#include <sys/sha1.h>
#define SHA1_RESULTLEN	20
#else
#include <crypto/sha1.h>
#endif

#include <net/net_osdep.h>

#define	HMACSIZE	16

static int ah_sumsiz_1216 __P((struct secasvar *));
static int ah_sumsiz_zero __P((struct secasvar *));
static int ah_none_mature __P((struct secasvar *));
static void ah_none_init __P((struct ah_algorithm_state *,
	struct secasvar *));
static void ah_none_loop __P((struct ah_algorithm_state *, caddr_t, size_t));
static void ah_none_result __P((struct ah_algorithm_state *, caddr_t));
static int ah_keyed_md5_mature __P((struct secasvar *));
static void ah_keyed_md5_init __P((struct ah_algorithm_state *,
	struct secasvar *));
static void ah_keyed_md5_loop __P((struct ah_algorithm_state *, caddr_t,
	size_t));
static void ah_keyed_md5_result __P((struct ah_algorithm_state *, caddr_t));
static int ah_keyed_sha1_mature __P((struct secasvar *));
static void ah_keyed_sha1_init __P((struct ah_algorithm_state *,
	struct secasvar *));
static void ah_keyed_sha1_loop __P((struct ah_algorithm_state *, caddr_t,
	size_t));
static void ah_keyed_sha1_result __P((struct ah_algorithm_state *, caddr_t));
static int ah_hmac_md5_mature __P((struct secasvar *));
static void ah_hmac_md5_init __P((struct ah_algorithm_state *,
	struct secasvar *));
static void ah_hmac_md5_loop __P((struct ah_algorithm_state *, caddr_t,
	size_t));
static void ah_hmac_md5_result __P((struct ah_algorithm_state *, caddr_t));
static int ah_hmac_sha1_mature __P((struct secasvar *));
static void ah_hmac_sha1_init __P((struct ah_algorithm_state *,
	struct secasvar *));
static void ah_hmac_sha1_loop __P((struct ah_algorithm_state *, caddr_t,
	size_t));
static void ah_hmac_sha1_result __P((struct ah_algorithm_state *, caddr_t));

static void ah_update_mbuf __P((struct mbuf *, int, int, struct ah_algorithm *,
	struct ah_algorithm_state *));

/* checksum algorithms */
/* NOTE: The order depends on SADB_AALG_x in net/pfkeyv2.h */
struct ah_algorithm ah_algorithms[] = {
	{ 0, 0, 0, 0, 0, 0, },
	{ ah_sumsiz_1216, ah_hmac_md5_mature, 128, 128,
		ah_hmac_md5_init, ah_hmac_md5_loop, ah_hmac_md5_result, },
	{ ah_sumsiz_1216, ah_hmac_sha1_mature, 160, 160,
		ah_hmac_sha1_init, ah_hmac_sha1_loop, ah_hmac_sha1_result, },
	{ ah_sumsiz_1216, ah_keyed_md5_mature, 128, 128,
		ah_keyed_md5_init, ah_keyed_md5_loop, ah_keyed_md5_result, },
	{ ah_sumsiz_1216, ah_keyed_sha1_mature, 160, 160,
		ah_keyed_sha1_init, ah_keyed_sha1_loop, ah_keyed_sha1_result, },
	{ ah_sumsiz_zero, ah_none_mature, 0, 2048,
		ah_none_init, ah_none_loop, ah_none_result, },
};

static int
ah_sumsiz_1216(sav)
	struct secasvar *sav;
{
	if (!sav)
		return -1;
	if (sav->flags & SADB_X_EXT_OLD)
		return 16;
	else
		return 12;
}

static int
ah_sumsiz_zero(sav)
	struct secasvar *sav;
{
	if (!sav)
		return -1;
	return 0;
}

static int
ah_none_mature(sav)
	struct secasvar *sav;
{
	if (sav->sah->saidx.proto == IPPROTO_AH) {
		ipseclog((LOG_ERR,
		    "ah_none_mature: protocol and algorithm mismatch.\n"));
		return 1;
	}
	return 0;
}

static void
ah_none_init(state, sav)
	struct ah_algorithm_state *state;
	struct secasvar *sav;
{
	state->foo = NULL;
}

static void
ah_none_loop(state, addr, len)
	struct ah_algorithm_state *state;
	caddr_t addr;
	size_t len;
{
}

static void
ah_none_result(state, addr)
	struct ah_algorithm_state *state;
	caddr_t addr;
{
}

static int
ah_keyed_md5_mature(sav)
	struct secasvar *sav;
{
	/* anything is okay */
	return 0;
}

static void
ah_keyed_md5_init(state, sav)
	struct ah_algorithm_state *state;
	struct secasvar *sav;
{
	if (!state)
		panic("ah_keyed_md5_init: what?");

	state->sav = sav;
	state->foo = (void *)_MALLOC(sizeof(MD5_CTX), M_TEMP, M_NOWAIT);
	if (state->foo == NULL)
		panic("ah_keyed_md5_init: what?");
	MD5Init((MD5_CTX *)state->foo);
	if (state->sav) {
		MD5Update((MD5_CTX *)state->foo,
			(u_int8_t *)_KEYBUF(state->sav->key_auth),
			(u_int)_KEYLEN(state->sav->key_auth));

	    {
		/*
		 * Pad after the key.
		 * We cannot simply use md5_pad() since the function
		 * won't update the total length.
		 */
		size_t padlen;
		size_t keybitlen;
		u_int8_t buf[32];

		if (_KEYLEN(state->sav->key_auth) < 56)
			padlen = 64 - 8 - _KEYLEN(state->sav->key_auth);
		else
			padlen = 64 + 64 - 8 - _KEYLEN(state->sav->key_auth);
		keybitlen = _KEYLEN(state->sav->key_auth);
		keybitlen *= 8;

		buf[0] = 0x80;
		MD5Update((MD5_CTX *)state->foo, &buf[0], 1);
		padlen--;

		bzero(buf, sizeof(buf));
		while (sizeof(buf) < padlen) {
			MD5Update((MD5_CTX *)state->foo, &buf[0], sizeof(buf));
			padlen -= sizeof(buf);
		}
		if (padlen) {
			MD5Update((MD5_CTX *)state->foo, &buf[0], padlen);
		}

		buf[0] = (keybitlen >> 0) & 0xff;
		buf[1] = (keybitlen >> 8) & 0xff;
		buf[2] = (keybitlen >> 16) & 0xff;
		buf[3] = (keybitlen >> 24) & 0xff;
		MD5Update((MD5_CTX *)state->foo, buf, 8);
	    }
	}
}

static void
ah_keyed_md5_loop(state, addr, len)
	struct ah_algorithm_state *state;
	caddr_t addr;
	size_t len;
{
	if (!state)
		panic("ah_keyed_md5_loop: what?");

	MD5Update((MD5_CTX *)state->foo, addr, len);
}

static void
ah_keyed_md5_result(state, addr)
	struct ah_algorithm_state *state;
	caddr_t addr;
{
	u_char digest[16];

	if (!state)
		panic("ah_keyed_md5_result: what?");

	if (state->sav) {
		MD5Update((MD5_CTX *)state->foo,
			(u_int8_t *)_KEYBUF(state->sav->key_auth),
			(u_int)_KEYLEN(state->sav->key_auth));
	}
	MD5Final(&digest[0], (MD5_CTX *)state->foo);
	_FREE(state->foo, M_TEMP);
	bcopy(&digest[0], (void *)addr, sizeof(digest));
}

static int
ah_keyed_sha1_mature(sav)
	struct secasvar *sav;
{
	struct ah_algorithm *algo;

	if (!sav->key_auth) {
		ipseclog((LOG_ERR, "ah_keyed_sha1_mature: no key is given.\n"));
		return 1;
	}
	algo = &ah_algorithms[sav->alg_auth];
	if (sav->key_auth->sadb_key_bits < algo->keymin
	 || algo->keymax < sav->key_auth->sadb_key_bits) {
		ipseclog((LOG_ERR,
		    "ah_keyed_sha1_mature: invalid key length %d.\n",
		    sav->key_auth->sadb_key_bits));
		return 1;
	}

	return 0;
}

static void
ah_keyed_sha1_init(state, sav)
	struct ah_algorithm_state *state;
	struct secasvar *sav;
{
	SHA1_CTX *ctxt;

	if (!state)
		panic("ah_keyed_sha1_init: what?");

	state->sav = sav;
	state->foo = (void *)_MALLOC(sizeof(SHA1_CTX), M_TEMP, M_NOWAIT);
	if (!state->foo)
		panic("ah_keyed_sha1_init: what?");

	ctxt = (SHA1_CTX *)state->foo;
	SHA1Init(ctxt);

	if (state->sav) {
		SHA1Update(ctxt, (u_int8_t *)_KEYBUF(state->sav->key_auth),
			(u_int)_KEYLEN(state->sav->key_auth));

	    {
		/*
		 * Pad after the key.
		 */
		size_t padlen;
		size_t keybitlen;
		u_int8_t buf[32];

		if (_KEYLEN(state->sav->key_auth) < 56)
			padlen = 64 - 8 - _KEYLEN(state->sav->key_auth);
		else
			padlen = 64 + 64 - 8 - _KEYLEN(state->sav->key_auth);
		keybitlen = _KEYLEN(state->sav->key_auth);
		keybitlen *= 8;

		buf[0] = 0x80;
		SHA1Update(ctxt, &buf[0], 1);
		padlen--;

		bzero(buf, sizeof(buf));
		while (sizeof(buf) < padlen) {
			SHA1Update(ctxt, &buf[0], sizeof(buf));
			padlen -= sizeof(buf);
		}
		if (padlen) {
			SHA1Update(ctxt, &buf[0], padlen);
		}

		buf[0] = (keybitlen >> 0) & 0xff;
		buf[1] = (keybitlen >> 8) & 0xff;
		buf[2] = (keybitlen >> 16) & 0xff;
		buf[3] = (keybitlen >> 24) & 0xff;
		SHA1Update(ctxt, buf, 8);
	    }
	}
}

static void
ah_keyed_sha1_loop(state, addr, len)
	struct ah_algorithm_state *state;
	caddr_t addr;
	size_t len;
{
	SHA1_CTX *ctxt;

	if (!state || !state->foo)
		panic("ah_keyed_sha1_loop: what?");
	ctxt = (SHA1_CTX *)state->foo;

	SHA1Update(ctxt, (caddr_t)addr, (size_t)len);
}

static void
ah_keyed_sha1_result(state, addr)
	struct ah_algorithm_state *state;
	caddr_t addr;
{
	u_char digest[SHA1_RESULTLEN];	/* SHA-1 generates 160 bits */
	SHA1_CTX *ctxt;

	if (!state || !state->foo)
		panic("ah_keyed_sha1_result: what?");
	ctxt = (SHA1_CTX *)state->foo;

	if (state->sav) {
		SHA1Update(ctxt, (u_int8_t *)_KEYBUF(state->sav->key_auth),
			(u_int)_KEYLEN(state->sav->key_auth));
	}
	SHA1Final((caddr_t)&digest[0], ctxt);
	bcopy(&digest[0], (void *)addr, HMACSIZE);

	_FREE(state->foo, M_TEMP);
}

static int
ah_hmac_md5_mature(sav)
	struct secasvar *sav;
{
	struct ah_algorithm *algo;

	if (!sav->key_auth) {
		ipseclog((LOG_ERR, "ah_hmac_md5_mature: no key is given.\n"));
		return 1;
	}
	algo = &ah_algorithms[sav->alg_auth];
	if (sav->key_auth->sadb_key_bits < algo->keymin
	 || algo->keymax < sav->key_auth->sadb_key_bits) {
		ipseclog((LOG_ERR,
		    "ah_hmac_md5_mature: invalid key length %d.\n",
		    sav->key_auth->sadb_key_bits));
		return 1;
	}

	return 0;
}

static void
ah_hmac_md5_init(state, sav)
	struct ah_algorithm_state *state;
	struct secasvar *sav;
{
	u_char *ipad;
	u_char *opad;
	u_char tk[16];
	u_char *key;
	size_t keylen;
	size_t i;
	MD5_CTX *ctxt;

	if (!state)
		panic("ah_hmac_md5_init: what?");

	state->sav = sav;
	state->foo = (void *)_MALLOC(64 + 64 + sizeof(MD5_CTX), M_TEMP, M_NOWAIT);
	if (!state->foo)
		panic("ah_hmac_md5_init: what?");

	ipad = (u_char *)state->foo;
	opad = (u_char *)(ipad + 64);
	ctxt = (MD5_CTX *)(opad + 64);

	/* compress the key if necessery */
	if (64 < _KEYLEN(state->sav->key_auth)) {
		MD5Init(ctxt);
		MD5Update(ctxt, _KEYBUF(state->sav->key_auth),
			_KEYLEN(state->sav->key_auth));
		MD5Final(&tk[0], ctxt);
		key = &tk[0];
		keylen = 16;
	} else {
		key = _KEYBUF(state->sav->key_auth);
		keylen = _KEYLEN(state->sav->key_auth);
	}

	bzero(ipad, 64);
	bzero(opad, 64);
	bcopy(key, ipad, keylen);
	bcopy(key, opad, keylen);
	for (i = 0; i < 64; i++) {
		ipad[i] ^= 0x36;
		opad[i] ^= 0x5c;
	}

	MD5Init(ctxt);
	MD5Update(ctxt, ipad, 64);
}

static void
ah_hmac_md5_loop(state, addr, len)
	struct ah_algorithm_state *state;
	caddr_t addr;
	size_t len;
{
	MD5_CTX *ctxt;

	if (!state || !state->foo)
		panic("ah_hmac_md5_loop: what?");
	ctxt = (MD5_CTX *)(((caddr_t)state->foo) + 128);
	MD5Update(ctxt, addr, len);
}

static void
ah_hmac_md5_result(state, addr)
	struct ah_algorithm_state *state;
	caddr_t addr;
{
	u_char digest[16];
	u_char *ipad;
	u_char *opad;
	MD5_CTX *ctxt;

	if (!state || !state->foo)
		panic("ah_hmac_md5_result: what?");

	ipad = (u_char *)state->foo;
	opad = (u_char *)(ipad + 64);
	ctxt = (MD5_CTX *)(opad + 64);

	MD5Final(&digest[0], ctxt);

	MD5Init(ctxt);
	MD5Update(ctxt, opad, 64);
	MD5Update(ctxt, &digest[0], sizeof(digest));
	MD5Final(&digest[0], ctxt);

	bcopy(&digest[0], (void *)addr, HMACSIZE);

	_FREE(state->foo, M_TEMP);
}

static int
ah_hmac_sha1_mature(sav)
	struct secasvar *sav;
{
	struct ah_algorithm *algo;

	if (!sav->key_auth) {
		ipseclog((LOG_ERR, "ah_hmac_sha1_mature: no key is given.\n"));
		return 1;
	}
	algo = &ah_algorithms[sav->alg_auth];
	if (sav->key_auth->sadb_key_bits < algo->keymin
	 || algo->keymax < sav->key_auth->sadb_key_bits) {
		ipseclog((LOG_ERR,
		    "ah_hmac_sha1_mature: invalid key length %d.\n",
		    sav->key_auth->sadb_key_bits));
		return 1;
	}

	return 0;
}

static void
ah_hmac_sha1_init(state, sav)
	struct ah_algorithm_state *state;
	struct secasvar *sav;
{
	u_char *ipad;
	u_char *opad;
	SHA1_CTX *ctxt;
	u_char tk[SHA1_RESULTLEN];	/* SHA-1 generates 160 bits */
	u_char *key;
	size_t keylen;
	size_t i;

	if (!state)
		panic("ah_hmac_sha1_init: what?");

	state->sav = sav;
	state->foo = (void *)_MALLOC(64 + 64 + sizeof(SHA1_CTX),
			M_TEMP, M_NOWAIT);
	if (!state->foo)
		panic("ah_hmac_sha1_init: what?");

	ipad = (u_char *)state->foo;
	opad = (u_char *)(ipad + 64);
	ctxt = (SHA1_CTX *)(opad + 64);

	/* compress the key if necessery */
	if (64 < _KEYLEN(state->sav->key_auth)) {
		SHA1Init(ctxt);
		SHA1Update(ctxt, _KEYBUF(state->sav->key_auth),
			_KEYLEN(state->sav->key_auth));
		SHA1Final(&tk[0], ctxt);
		key = &tk[0];
		keylen = SHA1_RESULTLEN;
	} else {
		key = _KEYBUF(state->sav->key_auth);
		keylen = _KEYLEN(state->sav->key_auth);
	}

	bzero(ipad, 64);
	bzero(opad, 64);
	bcopy(key, ipad, keylen);
	bcopy(key, opad, keylen);
	for (i = 0; i < 64; i++) {
		ipad[i] ^= 0x36;
		opad[i] ^= 0x5c;
	}

	SHA1Init(ctxt);
	SHA1Update(ctxt, ipad, 64);
}

static void
ah_hmac_sha1_loop(state, addr, len)
	struct ah_algorithm_state *state;
	caddr_t addr;
	size_t len;
{
	SHA1_CTX *ctxt;

	if (!state || !state->foo)
		panic("ah_hmac_sha1_loop: what?");

	ctxt = (SHA1_CTX *)(((u_char *)state->foo) + 128);
	SHA1Update(ctxt, (caddr_t)addr, (size_t)len);
}

static void
ah_hmac_sha1_result(state, addr)
	struct ah_algorithm_state *state;
	caddr_t addr;
{
	u_char digest[SHA1_RESULTLEN];	/* SHA-1 generates 160 bits */
	u_char *ipad;
	u_char *opad;
	SHA1_CTX *ctxt;

	if (!state || !state->foo)
		panic("ah_hmac_sha1_result: what?");

	ipad = (u_char *)state->foo;
	opad = (u_char *)(ipad + 64);
	ctxt = (SHA1_CTX *)(opad + 64);

	SHA1Final((caddr_t)&digest[0], ctxt);

	SHA1Init(ctxt);
	SHA1Update(ctxt, opad, 64);
	SHA1Update(ctxt, (caddr_t)&digest[0], sizeof(digest));
	SHA1Final((caddr_t)&digest[0], ctxt);

	bcopy(&digest[0], (void *)addr, HMACSIZE);

	_FREE(state->foo, M_TEMP);
}

/*------------------------------------------------------------*/

/*
 * go generate the checksum.
 */
static void
ah_update_mbuf(m, off, len, algo, algos)
	struct mbuf *m;
	int off;
	int len;
	struct ah_algorithm *algo;
	struct ah_algorithm_state *algos;
{
	struct mbuf *n;
	int tlen;

	/* easy case first */
	if (off + len <= m->m_len) {
		(algo->update)(algos, mtod(m, caddr_t) + off, len);
		return;
	}

	for (n = m; n; n = n->m_next) {
		if (off < n->m_len)
			break;

		off -= n->m_len;
	}

	if (!n)
		panic("ah_update_mbuf: wrong offset specified");

	for (/*nothing*/; n && len > 0; n = n->m_next) {
		if (n->m_len == 0)
			continue;
		if (n->m_len - off < len)
			tlen = n->m_len - off;
		else
			tlen = len;

		(algo->update)(algos, mtod(n, caddr_t) + off, tlen);

		len -= tlen;
		off = 0;
	}
}

/*
 * Go generate the checksum. This function won't modify the mbuf chain
 * except AH itself.
 *
 * NOTE: the function does not free mbuf on failure.
 * Don't use m_copy(), it will try to share cluster mbuf by using refcnt.
 */
int
ah4_calccksum(m, ahdat, algo, sav)
	struct mbuf *m;
	caddr_t ahdat;
	struct ah_algorithm *algo;
	struct secasvar *sav;
{
	int off;
	int hdrtype;
	size_t advancewidth;
	struct ah_algorithm_state algos;
	u_char sumbuf[AH_MAXSUMSIZE];
	int error = 0;
	int ahseen;
	struct mbuf *n = NULL;

	if ((m->m_flags & M_PKTHDR) == 0)
		return EINVAL;

	ahseen = 0;
	hdrtype = -1;	/*dummy, it is called IPPROTO_IP*/

	off = 0;

	(algo->init)(&algos, sav);

	advancewidth = 0;	/*safety*/

again:
	/* gory. */
	switch (hdrtype) {
	case -1:	/*first one only*/
	    {
		/*
		 * copy ip hdr, modify to fit the AH checksum rule,
		 * then take a checksum.
		 */
		struct ip iphdr;
		size_t hlen;

		m_copydata(m, off, sizeof(iphdr), (caddr_t)&iphdr);
#ifdef _IP_VHL
		hlen = IP_VHL_HL(iphdr.ip_vhl) << 2;
#else
		hlen = iphdr.ip_hl << 2;
#endif
		iphdr.ip_ttl = 0;
		iphdr.ip_sum = htons(0);
		if (ip4_ah_cleartos)
			iphdr.ip_tos = 0;
		iphdr.ip_off = htons(ntohs(iphdr.ip_off) & ip4_ah_offsetmask);
		(algo->update)(&algos, (caddr_t)&iphdr, sizeof(struct ip));

		if (hlen != sizeof(struct ip)) {
			u_char *p;
			int i, l, skip;

			if (hlen > MCLBYTES) {
				error = EMSGSIZE;
				goto fail;
			}
			MGET(n, M_DONTWAIT, MT_DATA);
			if (n && hlen > MLEN) {
				MCLGET(n, M_DONTWAIT);
				if ((n->m_flags & M_EXT) == 0) {
					m_free(n);
					n = NULL;
				}
			}
			if (n == NULL) {
				error = ENOBUFS;
				goto fail;
			}
			m_copydata(m, off, hlen, mtod(n, caddr_t));

			/*
			 * IP options processing.
			 * See RFC2402 appendix A.
			 */
			p = mtod(n, u_char *);
			i = sizeof(struct ip);
			while (i < hlen) {
				skip = 1;
				switch (p[i + IPOPT_OPTVAL]) {
				case IPOPT_EOL:
				case IPOPT_NOP:
					l = 1;
					skip = 0;
					break;
				case IPOPT_SECURITY:	/* 0x82 */
				case 0x85:	/* Extended security */
				case 0x86:	/* Commercial security */
				case 0x94:	/* Router alert */
				case 0x95:	/* RFC1770 */
					l = p[i + IPOPT_OLEN];
					skip = 0;
					break;
				default:
					l = p[i + IPOPT_OLEN];
					skip = 1;
					break;
				}
				if (l <= 0 || hlen - i < l) {
					ipseclog((LOG_ERR,
					    "ah4_calccksum: invalid IP option "
					    "(type=%02x len=%02x)\n",
					    p[i + IPOPT_OPTVAL],
					    p[i + IPOPT_OLEN]));
					m_free(n);
					n = NULL;
					error = EINVAL;
					goto fail;
				}
				if (skip)
					bzero(p + i, l);
				if (p[i + IPOPT_OPTVAL] == IPOPT_EOL)
					break;
				i += l;
			}
			p = mtod(n, u_char *) + sizeof(struct ip);
			(algo->update)(&algos, p, hlen - sizeof(struct ip));

			m_free(n);
			n = NULL;
		}

		hdrtype = (iphdr.ip_p) & 0xff;
		advancewidth = hlen;
		break;
	    }

	case IPPROTO_AH:
	    {
		struct ah ah;
		int siz;
		int hdrsiz;
		int totlen;

		m_copydata(m, off, sizeof(ah), (caddr_t)&ah);
		hdrsiz = (sav->flags & SADB_X_EXT_OLD)
				? sizeof(struct ah)
				: sizeof(struct newah);
		siz = (*algo->sumsiz)(sav);
		totlen = (ah.ah_len + 2) << 2;

		/*
		 * special treatment is necessary for the first one, not others
		 */
		if (!ahseen) {
			if (totlen > m->m_pkthdr.len - off ||
			    totlen > MCLBYTES) {
				error = EMSGSIZE;
				goto fail;
			}
			MGET(n, M_DONTWAIT, MT_DATA);
			if (n && totlen > MLEN) {
				MCLGET(n, M_DONTWAIT);
				if ((n->m_flags & M_EXT) == 0) {
					m_free(n);
					n = NULL;
				}
			}
			if (n == NULL) {
				error = ENOBUFS;
				goto fail;
			}
			m_copydata(m, off, totlen, mtod(n, caddr_t));
			n->m_len = totlen;
			bzero(mtod(n, caddr_t) + hdrsiz, siz);
			(algo->update)(&algos, mtod(n, caddr_t), n->m_len);
			m_free(n);
			n = NULL;
		} else
			ah_update_mbuf(m, off, totlen, algo, &algos);
		ahseen++;

		hdrtype = ah.ah_nxt;
		advancewidth = totlen;
		break;
	    }

	default:
		ah_update_mbuf(m, off, m->m_pkthdr.len - off, algo, &algos);
		advancewidth = m->m_pkthdr.len - off;
		break;
	}

	off += advancewidth;
	if (off < m->m_pkthdr.len)
		goto again;

	(algo->result)(&algos, &sumbuf[0]);
	bcopy(&sumbuf[0], ahdat, (*algo->sumsiz)(sav));

	if (n)
		m_free(n);
	return error;

fail:
	if (n)
		m_free(n);
	return error;
}

#if INET6
/*
 * Go generate the checksum. This function won't modify the mbuf chain
 * except AH itself.
 *
 * NOTE: the function does not free mbuf on failure.
 * Don't use m_copy(), it will try to share cluster mbuf by using refcnt.
 */
int
ah6_calccksum(m, ahdat, algo, sav)
	struct mbuf *m;
	caddr_t ahdat;
	struct ah_algorithm *algo;
	struct secasvar *sav;
{
	int newoff, off;
	int proto, nxt;
	struct mbuf *n = NULL;
	int error;
	int ahseen;
	struct ah_algorithm_state algos;
	u_char sumbuf[AH_MAXSUMSIZE];

	if ((m->m_flags & M_PKTHDR) == 0)
		return EINVAL;

	(algo->init)(&algos, sav);

	off = 0;
	proto = IPPROTO_IPV6;
	nxt = -1;
	ahseen = 0;

 again:
	newoff = ip6_nexthdr(m, off, proto, &nxt);
	if (newoff < 0)
		newoff = m->m_pkthdr.len;
	else if (newoff <= off) {
		error = EINVAL;
		goto fail;
	}

	switch (proto) {
	case IPPROTO_IPV6:
		/*
		 * special treatment is necessary for the first one, not others
		 */
		if (off == 0) {
			struct ip6_hdr ip6copy;

			if (newoff - off != sizeof(struct ip6_hdr)) {
				error = EINVAL;
				goto fail;
			}

			m_copydata(m, off, newoff - off, (caddr_t)&ip6copy);
			/* RFC2402 */
			ip6copy.ip6_flow = 0;
			ip6copy.ip6_vfc &= ~IPV6_VERSION_MASK;
			ip6copy.ip6_vfc |= IPV6_VERSION;
			ip6copy.ip6_hlim = 0;
			if (IN6_IS_ADDR_LINKLOCAL(&ip6copy.ip6_src))
				ip6copy.ip6_src.s6_addr16[1] = 0x0000;
			if (IN6_IS_ADDR_LINKLOCAL(&ip6copy.ip6_dst))
				ip6copy.ip6_dst.s6_addr16[1] = 0x0000;
			(algo->update)(&algos, (caddr_t)&ip6copy,
				       sizeof(struct ip6_hdr));
		} else {
			newoff = m->m_pkthdr.len;
			ah_update_mbuf(m, off, m->m_pkthdr.len - off, algo,
			    &algos);
		}
		break;

	case IPPROTO_AH:
	    {
		int siz;
		int hdrsiz;

		hdrsiz = (sav->flags & SADB_X_EXT_OLD)
				? sizeof(struct ah)
				: sizeof(struct newah);
		siz = (*algo->sumsiz)(sav);

		/*
		 * special treatment is necessary for the first one, not others
		 */
		if (!ahseen) {
			if (newoff - off > MCLBYTES) {
				error = EMSGSIZE;
				goto fail;
			}
			MGET(n, M_DONTWAIT, MT_DATA);
			if (n && newoff - off > MLEN) {
				MCLGET(n, M_DONTWAIT);
				if ((n->m_flags & M_EXT) == 0) {
					m_free(n);
					n = NULL;
				}
			}
			if (n == NULL) {
				error = ENOBUFS;
				goto fail;
			}
			m_copydata(m, off, newoff - off, mtod(n, caddr_t));
			n->m_len = newoff - off;
			bzero(mtod(n, caddr_t) + hdrsiz, siz);
			(algo->update)(&algos, mtod(n, caddr_t), n->m_len);
			m_free(n);
			n = NULL;
		} else
			ah_update_mbuf(m, off, newoff - off, algo, &algos);
		ahseen++;
		break;
	    }

	 case IPPROTO_HOPOPTS:
	 case IPPROTO_DSTOPTS:
	 {
		struct ip6_ext *ip6e;
		int hdrlen, optlen;
		u_int8_t *p, *optend, *optp;

		if (newoff - off > MCLBYTES) {
			error = EMSGSIZE;
			goto fail;
		}
		MGET(n, M_DONTWAIT, MT_DATA);
		if (n && newoff - off > MLEN) {
			MCLGET(n, M_DONTWAIT);
			if ((n->m_flags & M_EXT) == 0) {
				m_free(n);
				n = NULL;
			}
		}
		if (n == NULL) {
			error = ENOBUFS;
			goto fail;
		}
		m_copydata(m, off, newoff - off, mtod(n, caddr_t));
		n->m_len = newoff - off;

		ip6e = mtod(n, struct ip6_ext *);
		hdrlen = (ip6e->ip6e_len + 1) << 3;
		if (newoff - off < hdrlen) {
			 error = EINVAL;
			 m_free(n);
			 n = NULL;
			 goto fail;
		}
		p = mtod(n, u_int8_t *);
		optend = p + hdrlen;

		/*
		 * ICV calculation for the options header including all
		 * options.  This part is a little tricky since there are
		 * two type of options; mutable and immutable.  We try to
		 * null-out mutable ones here.
		 */
		optp = p + 2;
		while (optp < optend) {
			if (optp[0] == IP6OPT_PAD1)
				optlen = 1;
			else {
				if (optp + 2 > optend) {
					error = EINVAL;
					m_free(n);
					n = NULL;
					goto fail;
				}
				optlen = optp[1] + 2;

				if (optp[0] & IP6OPT_MUTABLE)
					bzero(optp + 2, optlen - 2);
			}

			optp += optlen;
		}

		(algo->update)(&algos, mtod(n, caddr_t), n->m_len);
		m_free(n);
		n = NULL;
		break;
	 }

	 case IPPROTO_ROUTING:
		/*
		 * For an input packet, we can just calculate `as is'.
		 * For an output packet, we assume ip6_output have already
		 * made packet how it will be received at the final
		 * destination.
		 */
		/* FALLTHROUGH */

	default:
		ah_update_mbuf(m, off, newoff - off, algo, &algos);
		break;
	}

	if (newoff < m->m_pkthdr.len) {
		proto = nxt;
		off = newoff;
		goto again;
	}

	(algo->result)(&algos, &sumbuf[0]);
	bcopy(&sumbuf[0], ahdat, (*algo->sumsiz)(sav));

	/* just in case */
	if (n)
		m_free(n);
	return 0;
fail:
	/* just in case */
	if (n)
		m_free(n);
	return error;
}
#endif
