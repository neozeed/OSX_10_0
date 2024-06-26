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
   | Authors: Stephanie Wehner <_@r4k.net>                                |
   +----------------------------------------------------------------------+
 */
/* $Id: yp.c,v 1.1.1.1 2000/08/10 02:08:37 wsanchez Exp $ */

#include "php.h"
#include "ext/standard/info.h"

#if HAVE_YP

#include "php_yp.h"

#include <rpcsvc/ypclnt.h>

function_entry yp_functions[] = {
	PHP_FE(yp_get_default_domain, NULL)
	PHP_FE(yp_order, NULL)
	PHP_FE(yp_master, NULL)
	PHP_FE(yp_match, NULL)
	PHP_FE(yp_first, NULL)
	PHP_FE(yp_next, NULL)
	{NULL, NULL, NULL}
};

zend_module_entry yp_module_entry = {
	"yp",
	yp_functions,
	NULL,
	NULL,
	NULL,
	NULL,
	PHP_MINFO(yp),
	STANDARD_MODULE_PROPERTIES
};

#ifdef COMPILE_DL_YP
ZEND_GET_MODULE(yp)
#endif

/* {{{ proto string yp_get_default_domain(void)
   Returns the domain or false */
PHP_FUNCTION(yp_get_default_domain) {
	char *outdomain;

	if(yp_get_default_domain(&outdomain)) {
		RETURN_FALSE;
	}
	RETVAL_STRING(outdomain,1);
}
/* }}} */

/* {{{ proto int yp_order(string domain, string map)            
   Returns the order number or false */
PHP_FUNCTION(yp_order) {
	pval **domain, **map;

#if SOLARIS_YP
	unsigned long outval;
#else
      int outval;
#endif

	if((ZEND_NUM_ARGS() != 2) || zend_get_parameters_ex(2,&domain,&map) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_string_ex(domain);
	convert_to_string_ex(map);

	if(yp_order((*domain)->value.str.val,(*map)->value.str.val,&outval)) {
		RETURN_FALSE;
	}

	RETVAL_LONG(outval);
}
/* }}} */

/* {{{ proto string yp_master(string domain, string map)
   Returns the machine name of the master */
PHP_FUNCTION(yp_master) {
	pval **domain, **map;
	char *outname;

	if((ZEND_NUM_ARGS() != 2) || zend_get_parameters_ex(2,&domain,&map) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_string_ex(domain);
	convert_to_string_ex(map);

	if(yp_master((*domain)->value.str.val,(*map)->value.str.val,&outname)) {
		RETURN_FALSE;
	}

	RETVAL_STRING(outname,1);
}
/* }}} */

/* {{{ proto string yp_match(string domain, string map, string key)
   Returns the matched line or false */
PHP_FUNCTION(yp_match) {
	pval **domain, **map, **key;
	char *outval;
	int outvallen;

	if((ZEND_NUM_ARGS() != 3) || zend_get_parameters_ex(3,&domain,&map,&key) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_string_ex(domain);
	convert_to_string_ex(map);
	convert_to_string_ex(key);

	if(yp_match((*domain)->value.str.val,(*map)->value.str.val,(*key)->value.str.val,(*key)->value.str.len,&outval,&outvallen)) {
		RETURN_FALSE;
	}

	RETVAL_STRING(outval,1);
}
/* }}} */

/* {{{ proto array yp_first(string domain, string map)
   Returns the first key as $var["key"] and the first line as $var["value"] */
PHP_FUNCTION(yp_first) {
	pval **domain, **map;
	char *outval, *outkey;
	int outvallen, outkeylen;

	if((ZEND_NUM_ARGS() != 2) || zend_get_parameters_ex(2,&domain,&map) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_string_ex(domain);
	convert_to_string_ex(map);

	if(yp_first((*domain)->value.str.val,(*map)->value.str.val,&outkey,&outkeylen,&outval,&outvallen)) {
		RETURN_FALSE;
	}
	array_init(return_value);
	add_assoc_string(return_value,"key",outkey,1);
	add_assoc_string(return_value,"value",outval,1);
}
/* }}} */

/* {{{ proto array yp_next(string domain, string map, string key)
   Returns an array with $var[$key] and the the line as the value */
PHP_FUNCTION(yp_next) {
	pval **domain, **map, **key;
	char *outval, *outkey;
	int outvallen, outkeylen;

	if((ZEND_NUM_ARGS() != 3) || zend_get_parameters_ex(3,&domain,&map,&key) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	convert_to_string_ex(domain);
	convert_to_string_ex(map);
	convert_to_string_ex(key);

	if(yp_next((*domain)->value.str.val,(*map)->value.str.val,(*key)->value.str.val,(*key)->value.str.len,&outkey,&outkeylen,&outval,&outvallen)) {
		RETURN_FALSE;
	}
	
	array_init(return_value);
	add_assoc_string(return_value,outkey,outval,1);
}
/* }}} */

PHP_MINFO_FUNCTION(yp) {
        php_info_print_table_start();
        php_info_print_table_row(2, "YP Support", "enabled");
        php_info_print_table_end();
}
#endif /* HAVE_YP */
