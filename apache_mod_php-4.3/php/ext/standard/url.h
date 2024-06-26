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
   | Authors: Jim Winstead (jimw@php.net)                                 |
   +----------------------------------------------------------------------+
 */
/* $Id: url.h,v 1.1.1.2 2000/09/07 00:06:12 wsanchez Exp $ */

#ifndef URL_H
#define URL_H

typedef struct php_url {
	char *scheme;
	char *user;
	char *pass;
	char *host;
	unsigned short port;
	char *path;
	char *query;
	char *fragment;
} php_url;

void free_url(php_url *);
extern php_url *url_parse(char *);
extern int php_url_decode(char *, int); /* return value: length of decoded string */
extern char *php_url_encode(char *, int);
extern int php_raw_url_decode(char *, int); /* return value: length of decoded string */
extern char *php_raw_url_encode(char *, int);

PHP_FUNCTION(parse_url);
PHP_FUNCTION(urlencode);
PHP_FUNCTION(urldecode);
PHP_FUNCTION(rawurlencode);
PHP_FUNCTION(rawurldecode);

#endif /* URL_H */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 */
