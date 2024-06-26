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
   | Author: Sascha Schumann <sascha@schumann.cx>                         |
   +----------------------------------------------------------------------+
 */
/* $Id: url_scanner.h,v 1.1.1.3 2001/01/25 05:00:14 wsanchez Exp $ */

#ifndef URI_SCANNER_H
#define URI_SCANNER_H

PHP_RINIT_FUNCTION(url_scanner);
PHP_RSHUTDOWN_FUNCTION(url_scanner);

char *url_adapt(const char *src, size_t srclen, const char *data, size_t *newlen);

enum url_state { 
	STATE_NORMAL, 
	STATE_TAG_START, 
	STATE_TAG, 
	STATE_IN_TAG, 
	STATE_TAG_ATTR, 
	STATE_TAG_IS, 
	STATE_TAG_IS2, 
	STATE_TAG_VAL, 
	STATE_TAG_VAL2, 
	STATE_TAG_QVAL1,
	STATE_TAG_QVAL2, 
	STATE_TAG_QVAL2b 
};

typedef struct url_adapt_struct {
 	enum url_state state;
	char *tag;
	char *attr;
	char *val;
	char delim;
	char *p;
	int l,ml;
	int attr_done;
} url_adapt_state_t;

#endif
