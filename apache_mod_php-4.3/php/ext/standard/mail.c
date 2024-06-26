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
   | Authors:                                                             |
   +----------------------------------------------------------------------+
 */

/* $Id: mail.c,v 1.1.1.2 2000/09/07 00:06:05 wsanchez Exp $ */

#include <stdlib.h>
#include <ctype.h>
#include <stdio.h>
#include "php.h"
#include "ext/standard/info.h"
#if !defined(PHP_WIN32)
#include "build-defs.h"
#include <sysexits.h>
#endif
#include "php_mail.h"
#include "php_ini.h"

#if HAVE_SENDMAIL
#ifdef PHP_WIN32
#include "win32/sendmail.h"
#endif

#ifdef COMPILE_DL_STANDARD
ZEND_GET_MODULE(odbc)
#endif

/* {{{ proto int ezmlm_hash(string addr)
   Calculate EZMLM list hash value. */
PHP_FUNCTION(ezmlm_hash)
{
	pval **pstr = NULL;
	char *str=NULL;
	unsigned long h = 5381L;
	int j, l;
	
	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &pstr) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_string_ex(pstr);
	if ((*pstr)->value.str.val) {
		str = (*pstr)->value.str.val;
	} else {
		php_error(E_WARNING, "Must give string parameter to ezmlm_hash()");
		RETURN_FALSE;
	}
	
	l = strlen(str);
	for (j=0; j<l; j++) {
		h = (h + (h<<5)) ^ (unsigned long) (unsigned char) tolower(str[j]);
	}
	
	h = (h%53);
	
	RETURN_LONG((int) h);
}

/* {{{ proto int mail(string to, string subject, string message [, string additional_headers])
   Send an email message */
PHP_FUNCTION(mail)
{
	pval **argv[4];
	char *to=NULL, *message=NULL, *headers=NULL, *subject=NULL;
	int argc;
	
	argc = ZEND_NUM_ARGS();
	if (argc < 3 || argc > 4 || zend_get_parameters_array_ex(argc, argv) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	/* To: */
	convert_to_string_ex(argv[0]);
	if ((*argv[0])->value.str.val) {
		to = (*argv[0])->value.str.val;
	} else {
		php_error(E_WARNING, "No to field in mail command");
		RETURN_FALSE;
	}

	/* Subject: */
	convert_to_string_ex(argv[1]);
	if ((*argv[1])->value.str.val) {
		subject = (*argv[1])->value.str.val;
	} else {
		php_error(E_WARNING, "No subject field in mail command");
		RETURN_FALSE;
	}

	/* message body */
	convert_to_string_ex(argv[2]);
	if ((*argv[2])->value.str.val) {
		message = (*argv[2])->value.str.val;
	} else {
		/* this is not really an error, so it is allowed. */
		php_error(E_WARNING, "No message string in mail command");
		message = NULL;
	}

	if (argc == 4) {			/* other headers */
		convert_to_string_ex(argv[3]);
		headers = (*argv[3])->value.str.val;
	}
	
	if (php_mail(to, subject, message, headers)){
		RETURN_TRUE;
	} else {
		RETURN_FALSE;
	}
}
/* }}} */

int php_mail(char *to, char *subject, char *message, char *headers)
{
#ifdef PHP_WIN32
	int tsm_err;
#else
	FILE *sendmail;
	int ret;
	char *sendmail_path = INI_STR("sendmail_path");
#endif

#ifdef PHP_WIN32
	if (TSendMail(INI_STR("SMTP"), &tsm_err, headers, subject, to, message) != SUCCESS){
		php_error(E_WARNING, GetSMErrorText(tsm_err));
		return 0;
	}
#else
	if (!sendmail_path) {
		return 0;
	}
	sendmail = popen(sendmail_path, "w");

	if (sendmail) {
		fprintf(sendmail, "To: %s\n", to);
		fprintf(sendmail, "Subject: %s\n", subject);
		if (headers != NULL) {
			fprintf(sendmail, "%s\n", headers);
		}
		fprintf(sendmail, "\n%s\n", message);
		ret = pclose(sendmail);
		if ((ret != EX_OK)&&(ret != EX_TEMPFAIL)) {
			return 0;
		} else {
			return 1;
		}
	} else {
		php_error(E_WARNING, "Could not execute mail delivery program");
		return 0;
	}
#endif
	return 1;
}

PHP_MINFO_FUNCTION(mail)
{
#ifdef PHP_WIN32
        php_info_print_table_row(2, "Internal Sendmail Support for Windows 4", "enabled");
#else
        php_info_print_table_row(2, "Path to sendmail", INI_STR("sendmail_path") );
#endif
}

#else

PHP_FUNCTION(mail) {}
PHP_MINFO_FUNCTION(mail) {}

#endif


/*
 * Local variables:
 * tab-width: 4
 * End:
 */
