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
   | Author: Sterling Hughes <Sterling.Hughes@pentap.net>                 |
   +----------------------------------------------------------------------+
*/

/* $Id: php_curl.h,v 1.1.1.2 2001/01/25 04:59:09 wsanchez Exp $ */

#ifndef _PHP_CURL_H
#define _PHP_CURL_H

#ifdef COMPILE_DL_CURL
#undef HAVE_CURL
#define HAVE_CURL 1
#endif

#if HAVE_CURL

#include <curl/curl.h>

extern zend_module_entry curl_module_entry;
#define curl_module_ptr &curl_module_entry

#define CURLOPT_RETURNTRANSFER 19913

PHP_MINIT_FUNCTION(curl);
PHP_MSHUTDOWN_FUNCTION(curl);
PHP_MINFO_FUNCTION(curl);
PHP_FUNCTION(curl_version);
PHP_FUNCTION(curl_init);
PHP_FUNCTION(curl_setopt);
PHP_FUNCTION(curl_exec);
#if LIBCURL_VERSION_NUM >= 0x070401
PHP_FUNCTION(curl_getinfo);
#endif
PHP_FUNCTION(curl_error);
PHP_FUNCTION(curl_errno);
PHP_FUNCTION(curl_close);

typedef struct {
	int return_transfer;
	int output_file;
	int php_stdout;
	int cerrno;
	char error[CURL_ERROR_SIZE+1];
	CURL *cp;
	zend_llist to_free;
} php_curl;

typedef struct {

} php_curl_globals;

#ifdef ZTS
#define CURLG(v) (curl_globals->v)
#define CURLLS_FETCH() php_curl_globals *curl_globals = ts_resource(curl_globals_id)
#else
#define CURLG(v) (curl_globals.v)
#define CURLLS_FETCH()
#endif


#else
#define curl_module_ptr NULL
#endif /* HAVE_CURL */
#define phpext_curl_ptr curl_module_ptr
#endif  /* _PHP_CURL_H */
