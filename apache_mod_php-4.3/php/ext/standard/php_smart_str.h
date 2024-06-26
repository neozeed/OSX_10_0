/*
   +----------------------------------------------------------------------+
   | PHP version 4.0                                                      |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997, 1998, 1999, 2000 The PHP Group                   |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.02 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available at through the world-wide-web at                           |
   | http://www.php.net/license/2_02.txt.                                 |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Sascha Schumann <sascha@schumann.cx>                        |
   +----------------------------------------------------------------------+
 */

#ifndef PHP_SMART_STR_H
#define PHP_SMART_STR_H

#include "php_smart_str_public.h"

#define smart_str_0(x) ((x)->c[(x)->len] = '\0')

#define smart_str_alloc(d,n,what) {\
	if (!d->c) d->len = d->a = 0; \
	newlen = d->len + n; \
	if (newlen >= d->a) {\
		d->c = perealloc(d->c, newlen + 129, what); \
		d->a = newlen + 128; \
	}\
}

#define smart_str_appends_ex(dest, src, what) smart_str_appendl_ex(dest, src, strlen(src), what)
#define smart_str_appends(dest, src) smart_str_appendl(dest, src, strlen(src))

#define smart_str_appendc(dest, c) smart_str_appendc_ex(dest, c, 0)
#define smart_str_free(s) smart_str_free_ex(s, 0)
#define smart_str_appendl(dest,src,len) smart_str_appendl_ex(dest,src,len,0)
#define smart_str_append(dest, src) smart_str_append_ex(dest,src,0)

static inline void smart_str_appendc_ex(smart_str *dest, char c, int what)
{
	size_t newlen;

	smart_str_alloc(dest, 1, what);
	dest->len = newlen;
	dest->c[dest->len - 1] = c;
}


static inline void smart_str_free_ex(smart_str *s, int what)
{
	if (s->c) {
		pefree(s->c, what);
		s->c = NULL;
	}
	s->a = s->len = 0;
}

static inline void smart_str_appendl_ex(smart_str *dest, const char *src, size_t len, int what)
{
	size_t newlen;

	smart_str_alloc(dest, len, what);
	memcpy(dest->c + dest->len, src, len);
	dest->len = newlen;
}

static inline void smart_str_append_ex(smart_str *dest, smart_str *src, int what)
{
	smart_str_appendl_ex(dest, src->c, src->len, what);
}

static inline void smart_str_setl(smart_str *dest, const char *src, size_t len)
{
	dest->len = len;
	dest->a = len + 1;
	dest->c = (char *) src;
}

static inline void smart_str_sets(smart_str *dest, const char *src)
{
	smart_str_setl(dest, src, strlen(src));
}

#endif
