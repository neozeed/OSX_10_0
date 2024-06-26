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
   | Authors: Thies C. Arntzen (thies@thieso.net)                         |
   +----------------------------------------------------------------------+
*/

/* $Id: readline.c,v 1.1.1.3 2001/01/25 04:59:55 wsanchez Exp $ */

/* {{{ includes & prototypes */

#include "php.h"
#include "php_readline.h"

#if HAVE_LIBREADLINE

#include <readline/readline.h>
#include <readline/history.h>

PHP_FUNCTION(readline);
PHP_FUNCTION(readline_add_history);
PHP_FUNCTION(readline_info);
PHP_FUNCTION(readline_clear_history);
PHP_FUNCTION(readline_list_history);
PHP_FUNCTION(readline_read_history);
PHP_FUNCTION(readline_write_history);
PHP_FUNCTION(readline_completion_function);

static char *_readline_completion = NULL;
static zval _readline_array;

PHP_MINIT_FUNCTION(readline);
PHP_RSHUTDOWN_FUNCTION(readline);

/* }}} */
/* {{{ module stuff */

static zend_function_entry php_readline_functions[] = {
	PHP_FE(readline,	   		        NULL)
	PHP_FE(readline_info,  	            NULL)
	PHP_FE(readline_add_history, 		NULL)
	PHP_FE(readline_clear_history, 		NULL)
	PHP_FE(readline_list_history, 		NULL)
	PHP_FE(readline_read_history, 		NULL)
	PHP_FE(readline_write_history, 		NULL)
	PHP_FE(readline_completion_function,NULL)
	{NULL, NULL, NULL}
};

zend_module_entry readline_module_entry = { 
	"readline", 
	php_readline_functions, 
	PHP_MINIT(readline), 
	NULL,
	NULL,
	PHP_RSHUTDOWN(readline),
	NULL, 
	STANDARD_MODULE_PROPERTIES
};

#ifdef COMPILE_DL_READLINE
ZEND_GET_MODULE(readline)
#endif

PHP_MINIT_FUNCTION(readline)
{
    using_history();
	return SUCCESS;
}

PHP_RSHUTDOWN_FUNCTION(readline)
{
	if (_readline_completion) 
		efree(_readline_completion);

	return SUCCESS;
}

/* }}} */
/* {{{ proto string readline([string prompt]) 
   Reads a line */
PHP_FUNCTION(readline)
{
	char *result;
	pval **arg;
	int ac = ZEND_NUM_ARGS();

	if (ac < 0 || ac > 1 || zend_get_parameters_ex(ac, &arg) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	if (ac == 1) {
		convert_to_string_ex(arg);
	}

	result = readline(ac?(*arg)->value.str.val:NULL);

	if (! result) {
		RETURN_FALSE;
	} else {
		RETVAL_STRING(result,1);
		free(result);
	}
}

/* }}} */
/* {{{ proto mixed readline_info([string varname] [, string newvalue]) 
   Gets/sets various internal readline variables. */

#define SAFE_STRING(s) ((s)?(s):"")

PHP_FUNCTION(readline_info)
{
	zval **what;
	zval **value;
	int oldval;
	char *oldstr;
	int ac = ZEND_NUM_ARGS();

	if (ac < 0 || ac > 2 || zend_get_parameters_ex(ac, &what, &value) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	if (ac == 0) {
		array_init(return_value);
		add_assoc_string(return_value,"line_buffer",SAFE_STRING(rl_line_buffer),1);
		add_assoc_long(return_value,"point",rl_point);
		add_assoc_long(return_value,"end",rl_end);
		add_assoc_long(return_value,"mark",rl_mark);
		add_assoc_long(return_value,"done",rl_done);
		add_assoc_long(return_value,"pending_input",rl_pending_input);
#if HAVE_ERASE_EMPTY_LINE
		add_assoc_long(return_value,"erase_empty_line",rl_erase_empty_line);
#endif
		add_assoc_string(return_value,"prompt",SAFE_STRING(rl_prompt),1);
		add_assoc_string(return_value,"library_version",SAFE_STRING(rl_library_version),1);
		add_assoc_string(return_value,"terminal_name",SAFE_STRING(rl_terminal_name),1);
		add_assoc_string(return_value,"readline_name",SAFE_STRING(rl_readline_name),1);
	} else {
		convert_to_string_ex(what);

		if (! strcasecmp((*what)->value.str.val,"line_buffer")) {
			oldstr = rl_line_buffer;
			if (ac == 2) {
				/* XXX if (rl_line_buffer) free(rl_line_buffer); */
				convert_to_string_ex(value);
				rl_line_buffer = strdup((*value)->value.str.val);
			}
			RETVAL_STRING(SAFE_STRING(oldstr),1);
		} else if (! strcasecmp((*what)->value.str.val,"point")) {
			RETVAL_LONG(rl_point);
		} else if (! strcasecmp((*what)->value.str.val,"end")) {
			RETVAL_LONG(rl_end);
		} else if (! strcasecmp((*what)->value.str.val,"mark")) {
			RETVAL_LONG(rl_mark);
		} else if (! strcasecmp((*what)->value.str.val,"done")) {
			oldval = rl_done;
			if (ac == 2) {
				convert_to_long_ex(value);
				rl_done = (*value)->value.lval;
			}
			RETVAL_LONG(oldval);
		} else if (! strcasecmp((*what)->value.str.val,"pending_input")) {
			oldval = rl_pending_input;
			if (ac == 2) {
				convert_to_string_ex(value);
				rl_pending_input = (*value)->value.str.val[0];
			}
			RETVAL_LONG(oldval);
#if HAVE_ERASE_EMPTY_LINE
		} else if (! strcasecmp((*what)->value.str.val,"erase_empty_line")) {
			oldval = rl_erase_empty_line;
			if (ac == 2) {
				convert_to_long_ex(value);
				rl_erase_empty_line = (*value)->value.lval;
			}
			RETVAL_LONG(oldval);
#endif
		} else if (! strcasecmp((*what)->value.str.val,"prompt")) {
			RETVAL_STRING(SAFE_STRING(rl_prompt),1);
		} else if (! strcasecmp((*what)->value.str.val,"library_version")) {
			RETVAL_STRING(SAFE_STRING(rl_library_version),1);
		} else if (! strcasecmp((*what)->value.str.val,"terminal_name")) {
			RETVAL_STRING(SAFE_STRING(rl_terminal_name),1);
		} else if (! strcasecmp((*what)->value.str.val,"readline_name")) {
			oldstr = rl_readline_name;
			if (ac == 2) {
				/* XXX if (rl_readline_name) free(rl_readline_name); */
				convert_to_string_ex(value);
				rl_readline_name = strdup((*value)->value.str.val);;
			}
			RETVAL_STRING(SAFE_STRING(oldstr),1);
		} 
	}
}

/* }}} */
/* {{{ proto void readline_add_history([string prompt]) 
   Adds a line to the history */
PHP_FUNCTION(readline_add_history)
{
	pval **arg;
	int ac = ZEND_NUM_ARGS();

	if (ac < 1 || ac > 1 || zend_get_parameters_ex(ac, &arg) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_string_ex(arg);

	add_history((*arg)->value.str.val);

	RETURN_TRUE;
}

/* }}} */
/* {{{ proto void readline_clear_history(void) 
   Clears the history */
PHP_FUNCTION(readline_clear_history)
{
	int ac = ZEND_NUM_ARGS();

	if (ac < 0 || ac > 0) {
		WRONG_PARAM_COUNT;
	}

	clear_history();

	RETURN_TRUE;
}

/* }}} */
/* {{{ proto array readline_list_history(void) 
   Lists the history */
PHP_FUNCTION(readline_list_history)
{
	HIST_ENTRY **history;
	int ac = ZEND_NUM_ARGS();

	if (ac) {
		WRONG_PARAM_COUNT;
	}

	history = history_list();
	
	array_init(return_value);

	if (history) {
		int i;
		for (i = 0; history[i]; i++) {
			add_next_index_string(return_value,history[i]->line,1);
		}
	}
}

/* }}} */
/* {{{ proto int readline_read_history([string filename] [, int from] [,int to]) 
   Reads the history */
PHP_FUNCTION(readline_read_history)
{
	pval **arg;
	char *filename = NULL;
	int ac = ZEND_NUM_ARGS();

	if (ac < 0 || ac > 1 || zend_get_parameters_ex(ac, &arg) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	/* XXX from & to NYI */

	if (ac == 1) {
		convert_to_string_ex(arg);
		filename = (*arg)->value.str.val;
	}

	if (read_history(filename)) {
		RETURN_FALSE;
	} else {
		RETURN_TRUE;
	}
}

/* }}} */
/* {{{ proto int readline_write_history([string filename]) 
   Writes the history */
PHP_FUNCTION(readline_write_history)
{
	pval **arg;
	char *filename = NULL;
	int ac = ZEND_NUM_ARGS();

	if (ac < 0 || ac > 1 || zend_get_parameters_ex(ac, &arg) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	if (ac == 1) {
		convert_to_string_ex(arg);
		filename = (*arg)->value.str.val;
	}

	if (write_history(filename)) {
		RETURN_FALSE;
	} else {
		RETURN_TRUE;
	}
}

/* }}} */
/* {{{ proto void readline_completion_function(string funcname) 
   Readline completion function? */
char *test[] = { "bleibt", "da", "helfen", "keine", "pillen", "und" , "heissen", "umschlaege","hallo", "pallo", "egon", "thies", "ist", "doof", "tubu", "tata", 0 };

static char *_readline_command_generator(char *text,int state)
{
	HashTable  *myht = _readline_array.value.ht;
	zval **entry;
	
	/*
	printf("\n_readline_command_generator(\"%s\",%d)\n",text,state);
	*/
	
	if (! state)	{
		zend_hash_internal_pointer_reset(myht);
	}
	
	while (zend_hash_get_current_data(myht, (void **)&entry) == SUCCESS) {
		zend_hash_move_forward(myht);

		convert_to_string_ex(entry);
		if (strncmp ((*entry)->value.str.val, text, strlen(text)) == 0) {
			return (strdup((*entry)->value.str.val));
		}
	}

	return NULL;
}

static zval *_readline_string_zval(const char *str)
{
	zval *ret;
	int len = strlen(str);
	MAKE_STD_ZVAL(ret);

	ret->type = IS_STRING;
	ret->value.str.len = len;
	ret->value.str.val = estrndup(str, len);
	return ret;
}

static zval *_readline_long_zval(long l)
{
	zval *ret;
	MAKE_STD_ZVAL(ret);

	ret->type = IS_LONG;
	ret->value.lval = l;
	return ret;
}

static char **_readline_completion_cb(char *text, int start, int end)
{ 
	zval *params[4];
	int i;
	char **matches = NULL;
	CLS_FETCH();

	params[0]=_readline_string_zval(_readline_completion);
	params[1]=_readline_string_zval(text);
	params[2]=_readline_long_zval(start);
	params[3]=_readline_long_zval(end);

	if (call_user_function(CG(function_table), NULL, params[0], &_readline_array, 3, params+1) == SUCCESS) {
		if (_readline_array.type == IS_ARRAY) {
			matches = completion_matches(text,_readline_command_generator);
		}
	}
	
	for (i = 0; i < 4; i++) {
		zval_del_ref(&params[i]);
	}
	zval_dtor(&_readline_array);
	
	return matches; 
}

PHP_FUNCTION(readline_completion_function)
{
	pval **arg;
	int ac = ZEND_NUM_ARGS();

	if (ac < 0 || ac > 1 || zend_get_parameters_ex(ac, &arg) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	if (ac == 1) {
		convert_to_string_ex(arg);

		if (_readline_completion)
			efree(_readline_completion);

		_readline_completion = estrdup((*arg)->value.str.val);
		rl_attempted_completion_function = _readline_completion_cb;
	}

	RETURN_TRUE;
}

/* }}} */

#endif /* HAVE_LIBREADLINE */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 */
