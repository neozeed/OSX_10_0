/*
   +----------------------------------------------------------------------+
   | Zend Engine                                                          |
   +----------------------------------------------------------------------+
   | Copyright (c) 1998-2000 Zend Technologies Ltd. (http://www.zend.com) |
   +----------------------------------------------------------------------+
   | This source file is subject to version 0.92 of the Zend license,     |
   | that is bundled with this package in the file LICENSE, and is        |
   | available at through the world-wide-web at                           |
   | http://www.zend.com/license/0_92.txt.                                |
   | If you did not receive a copy of the Zend license and are unable to  |
   | obtain it through the world-wide-web, please send a note to          |
   | license@zend.com so we can mail you a copy immediately.              |
   +----------------------------------------------------------------------+
   | Authors: Andi Gutmans <andi@zend.com>                                |
   |          Zeev Suraski <zeev@zend.com>                                |
   +----------------------------------------------------------------------+
*/


#include "zend.h"
#include "zend_API.h"
#include "zend_builtin_functions.h"
#include "zend_constants.h"

#undef ZEND_TEST_EXCEPTIONS

static ZEND_FUNCTION(zend_version);
static ZEND_FUNCTION(func_num_args);
static ZEND_FUNCTION(func_get_arg);
static ZEND_FUNCTION(func_get_args);
static ZEND_NAMED_FUNCTION(zend_if_strlen);
static ZEND_FUNCTION(strcmp);
static ZEND_FUNCTION(strncmp);
static ZEND_FUNCTION(strcasecmp);
static ZEND_FUNCTION(strncasecmp);
static ZEND_FUNCTION(each);
static ZEND_FUNCTION(error_reporting);
static ZEND_FUNCTION(define);
static ZEND_FUNCTION(defined);
static ZEND_FUNCTION(get_class);
static ZEND_FUNCTION(get_parent_class);
static ZEND_FUNCTION(method_exists);
static ZEND_FUNCTION(class_exists);
static ZEND_FUNCTION(function_exists);
static ZEND_FUNCTION(leak);
#ifdef ZEND_TEST_EXCEPTIONS
static ZEND_FUNCTION(crash);
#endif
static ZEND_FUNCTION(get_included_files);
static ZEND_FUNCTION(is_subclass_of);
static ZEND_FUNCTION(get_class_vars);
static ZEND_FUNCTION(get_object_vars);
static ZEND_FUNCTION(get_class_methods);
static ZEND_FUNCTION(trigger_error);
static ZEND_FUNCTION(set_error_handler);
static ZEND_FUNCTION(restore_error_handler);
static ZEND_FUNCTION(get_declared_classes);
static ZEND_FUNCTION(get_defined_functions);
static ZEND_FUNCTION(get_defined_vars);
static ZEND_FUNCTION(create_function);
static ZEND_FUNCTION(get_resource_type);
#if ZEND_DEBUG
static ZEND_FUNCTION(zend_test_func);
#endif

unsigned char first_arg_force_ref[] = { 1, BYREF_FORCE };
unsigned char first_arg_allow_ref[] = { 1, BYREF_ALLOW };
unsigned char second_arg_force_ref[] = { 2, BYREF_NONE, BYREF_FORCE };
unsigned char second_arg_allow_ref[] = { 2, BYREF_NONE, BYREF_ALLOW };

static zend_function_entry builtin_functions[] = {
	ZEND_FE(zend_version,		NULL)
	ZEND_FE(func_num_args,		NULL)
	ZEND_FE(func_get_arg,		NULL)
	ZEND_FE(func_get_args,		NULL)
	{ "strlen", zend_if_strlen, NULL },
	ZEND_FE(strcmp,				NULL)
	ZEND_FE(strncmp,			NULL)
	ZEND_FE(strcasecmp,			NULL)
	ZEND_FE(strncasecmp,		NULL)
	ZEND_FE(each,				first_arg_force_ref)
	ZEND_FE(error_reporting,	NULL)
	ZEND_FE(define,				NULL)
	ZEND_FE(defined,			NULL)
	ZEND_FE(get_class,			NULL)
	ZEND_FE(get_parent_class,	NULL)
	ZEND_FE(method_exists,		NULL)
	ZEND_FE(class_exists,		NULL)
	ZEND_FE(function_exists,	NULL)
	ZEND_FE(leak,				NULL)
#ifdef ZEND_TEST_EXCEPTIONS
	ZEND_FE(crash,				NULL)
#endif
	ZEND_FE(get_included_files,	NULL)
	ZEND_FALIAS(get_required_files,	get_included_files,		NULL)
	ZEND_FE(is_subclass_of,		NULL)
	ZEND_FE(get_class_vars,		NULL)
	ZEND_FE(get_object_vars,	NULL)
	ZEND_FE(get_class_methods,	NULL)
	ZEND_FE(trigger_error,		NULL)
	ZEND_FALIAS(user_error,		trigger_error,		NULL)
	ZEND_FE(set_error_handler,		NULL)
	ZEND_FE(restore_error_handler,	NULL)
	ZEND_FE(get_declared_classes, NULL)
	ZEND_FE(get_defined_functions, NULL)
	ZEND_FE(get_defined_vars,	NULL)
	ZEND_FE(create_function,	NULL)
	ZEND_FE(get_resource_type,	NULL)
#if ZEND_DEBUG
	ZEND_FE(zend_test_func,		NULL)
#endif
	{ NULL, NULL, NULL }
};


int zend_startup_builtin_functions()
{
	return zend_register_functions(builtin_functions, NULL, MODULE_PERSISTENT);
}


ZEND_FUNCTION(zend_version)
{
	RETURN_STRINGL(ZEND_VERSION, sizeof(ZEND_VERSION)-1, 1);
}


ZEND_FUNCTION(func_num_args)
{
	void **p;
	int arg_count;

	p = EG(argument_stack).top_element-1-1;
	arg_count = (ulong) *p;		/* this is the amount of arguments passed to func_num_args(); */
	p -= 1+arg_count;
	if (*p) {
		zend_error(E_ERROR, "func_num_args(): Can't be used as a function parameter");
	}
	--p;
	if (p>=EG(argument_stack).elements) {
		RETURN_LONG((ulong) *p);
	} else {
		zend_error(E_WARNING, "func_num_args():  Called from the global scope - no function context");
		RETURN_LONG(-1);
	}
}


ZEND_FUNCTION(func_get_arg)
{
	void **p;
	int arg_count;
	zval **z_requested_offset;
	zval *arg;
	long requested_offset;

	if (ZEND_NUM_ARGS()!=1 || zend_get_parameters_ex(1, &z_requested_offset)==FAILURE) {
		RETURN_FALSE;
	}
	convert_to_long_ex(z_requested_offset);
	requested_offset = (*z_requested_offset)->value.lval;

	p = EG(argument_stack).top_element-1-1;
	arg_count = (ulong) *p;		/* this is the amount of arguments passed to func_get_arg(); */
	p -= 1+arg_count;
	if (*p) {
		zend_error(E_ERROR, "func_get_arg(): Can't be used as a function parameter");
	}
	--p;
	if (p<EG(argument_stack).elements) {
		zend_error(E_WARNING, "func_get_arg():  Called from the global scope - no function context");
		RETURN_FALSE;
	}
	arg_count = (ulong) *p;

	if (requested_offset>=arg_count) {
		zend_error(E_WARNING, "func_get_arg():  Argument %d not passed to function", requested_offset);
		RETURN_FALSE;
	}

	arg = *(p-(arg_count-requested_offset));
	*return_value = *arg;
	zval_copy_ctor(return_value);
}


ZEND_FUNCTION(func_get_args)
{
	void **p;
	int arg_count;
	int i;

	p = EG(argument_stack).top_element-1-1;
	arg_count = (ulong) *p;		/* this is the amount of arguments passed to func_get_args(); */
	p -= 1+arg_count;
	if (*p) {
		zend_error(E_ERROR, "func_get_args(): Can't be used as a function parameter");
	}
	--p;

	if (p<EG(argument_stack).elements) {
		zend_error(E_WARNING, "func_get_args():  Called from the global scope - no function context");
		RETURN_FALSE;
	}
	arg_count = (ulong) *p;


	array_init(return_value);
	for (i=0; i<arg_count; i++) {
		zval *element;

		ALLOC_ZVAL(element);
		*element = **((zval **) (p-(arg_count-i)));
		zval_copy_ctor(element);
		INIT_PZVAL(element);
		zend_hash_next_index_insert(return_value->value.ht, &element, sizeof(zval *), NULL);
	}
}


/* {{{ proto int strlen(string str)
   Get string length */
ZEND_NAMED_FUNCTION(zend_if_strlen)
{
	zval **str;
	
	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &str) == FAILURE) {
		ZEND_WRONG_PARAM_COUNT();
	}
	convert_to_string_ex(str);
	RETVAL_LONG((*str)->value.str.len);
}
/* }}} */

/* {{{ proto int strcmp(string str1, string str2)
   Binary safe string comparison */
ZEND_FUNCTION(strcmp)
{
	zval **s1, **s2;
	
	if (ZEND_NUM_ARGS() != 2 || zend_get_parameters_ex(2, &s1, &s2) == FAILURE) {
		ZEND_WRONG_PARAM_COUNT();
	}
	convert_to_string_ex(s1);
	convert_to_string_ex(s2);
	RETURN_LONG(zend_binary_zval_strcmp(*s1,*s2));
}
/* }}} */

/* {{{ proto int strncmp(string str1, string str2, int len)
   Binary safe string comparison */
ZEND_FUNCTION(strncmp)
{
	zval **s1, **s2, **s3;
	
	if (ZEND_NUM_ARGS() != 3 || zend_get_parameters_ex(3, &s1, &s2, &s3) == FAILURE) {
		ZEND_WRONG_PARAM_COUNT();
	}
	convert_to_string_ex(s1);
	convert_to_string_ex(s2);
	convert_to_long_ex(s3);
	RETURN_LONG(zend_binary_zval_strncmp(*s1,*s2,*s3));
}
/* }}} */

/* {{{ proto int strcasecmp(string str1, string str2)
   Binary safe case-insensitive string comparison */
ZEND_FUNCTION(strcasecmp)
{
	zval **s1, **s2;
	
	if (ZEND_NUM_ARGS()!=2 || zend_get_parameters_ex(2, &s1, &s2) == FAILURE) {
		ZEND_WRONG_PARAM_COUNT();
	}
	convert_to_string_ex(s1);
	convert_to_string_ex(s2);
	RETURN_LONG(zend_binary_zval_strcasecmp(*s1, *s2));
}
/* }}} */

/* {{{ proto int strncasecmp(string str1, string str2, int len)
   Binary safe string comparison */
ZEND_FUNCTION(strncasecmp)
{
	zval **s1, **s2, **s3;
	
	if (ZEND_NUM_ARGS() != 3 || zend_get_parameters_ex(3, &s1, &s2, &s3) == FAILURE) {
		ZEND_WRONG_PARAM_COUNT();
	}
	convert_to_string_ex(s1);
	convert_to_string_ex(s2);
	convert_to_long_ex(s3);
	RETURN_LONG(zend_binary_zval_strncasecmp(*s1,*s2,*s3));
}
/* }}} */

ZEND_FUNCTION(each)
{
	zval **array,*entry,**entry_ptr, *tmp;
	char *string_key;
	ulong num_key;
	zval **inserted_pointer;
	HashTable *target_hash;
	
	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &array) == FAILURE) {
		ZEND_WRONG_PARAM_COUNT();
	}
	target_hash = HASH_OF(*array);
	if (!target_hash) {
		zend_error(E_WARNING,"Variable passed to each() is not an array or object");
		return;
	}
	if (zend_hash_get_current_data(target_hash, (void **) &entry_ptr)==FAILURE) {
		RETURN_FALSE;
	}
	array_init(return_value);
	entry = *entry_ptr;

	/* add value elements */
	if (entry->is_ref) {
		ALLOC_ZVAL(tmp);
		*tmp = *entry;
		zval_copy_ctor(tmp);
		tmp->is_ref=0;
		tmp->refcount=0;
		entry=tmp;
	}
	zend_hash_index_update(return_value->value.ht, 1, &entry, sizeof(zval *), NULL);
	entry->refcount++;
	zend_hash_update(return_value->value.ht, "value", sizeof("value"), &entry, sizeof(zval *), NULL);
	entry->refcount++;

	/* add the key elements */
	switch (zend_hash_get_current_key(target_hash, &string_key, &num_key)) {
		case HASH_KEY_IS_STRING:
			add_get_index_string(return_value,0,string_key,(void **) &inserted_pointer,0);
			break;
		case HASH_KEY_IS_LONG:
			add_get_index_long(return_value,0,num_key, (void **) &inserted_pointer);
			break;
	}
	zend_hash_update(return_value->value.ht, "key", sizeof("key"), inserted_pointer, sizeof(zval *), NULL);
	(*inserted_pointer)->refcount++;
	zend_hash_move_forward(target_hash);
}

ZEND_FUNCTION(error_reporting)
{
	zval **arg;
	int old_error_reporting;

	old_error_reporting = EG(error_reporting);
	switch (ZEND_NUM_ARGS()) {
		case 0:
			break;
		case 1:
			if (zend_get_parameters_ex(1,&arg) == FAILURE) {
				RETURN_FALSE;
			}
			convert_to_long_ex(arg);
			EG(error_reporting)=(*arg)->value.lval;
			break;
		default:
			ZEND_WRONG_PARAM_COUNT();
			break;
	}

	RETVAL_LONG(old_error_reporting);
}

ZEND_FUNCTION(define)
{
	zval **var, **val, **non_cs;
	int case_sensitive;
	zend_constant c;
	
	switch(ZEND_NUM_ARGS()) {
		case 2:
			if (zend_get_parameters_ex(2, &var, &val)==FAILURE) {
				RETURN_FALSE;
			}
			case_sensitive = CONST_CS;
			break;
		case 3:
			if (zend_get_parameters_ex(3, &var, &val, &non_cs)==FAILURE) {
				RETURN_FALSE;
			}
			convert_to_long_ex(non_cs);
			if ((*non_cs)->value.lval) {
				case_sensitive = 0;
			} else {
				case_sensitive = CONST_CS;
			}
			break;
		default:
			ZEND_WRONG_PARAM_COUNT();
			break;
	}

	switch((*val)->type) {
		case IS_LONG:
		case IS_DOUBLE:
		case IS_STRING:
		case IS_BOOL:
		case IS_RESOURCE:
		case IS_NULL:
			break;
		default:
			zend_error(E_WARNING,"Constants may only evaluate to scalar values");
			RETURN_FALSE;
			break;
	}
	convert_to_string_ex(var);
	
	c.value = **val;
	zval_copy_ctor(&c.value);
	c.flags = case_sensitive; /* non persistent */
	c.name = zend_strndup((*var)->value.str.val, (*var)->value.str.len);
	c.name_len = (*var)->value.str.len+1;
	if(zend_register_constant(&c ELS_CC) == SUCCESS) {
	  RETURN_TRUE;
	} else {
	  RETURN_FALSE;
	}
}


ZEND_FUNCTION(defined)
{
	zval **var;
	zval c;
		
	if (ZEND_NUM_ARGS()!=1 || zend_get_parameters_ex(1, &var)==FAILURE) {
		ZEND_WRONG_PARAM_COUNT();
	}
	
	convert_to_string_ex(var);
	if (zend_get_constant((*var)->value.str.val, (*var)->value.str.len, &c)) {
		zval_dtor(&c);
		RETURN_LONG(1);
	} else {
		RETURN_LONG(0);
	}
}

/* {{{ proto string get_class(object object)
   Retrieves the class name */
ZEND_FUNCTION(get_class)
{
	zval **arg;
	
	if (ZEND_NUM_ARGS()!=1 || zend_get_parameters_ex(1, &arg)==FAILURE) {
		ZEND_WRONG_PARAM_COUNT();
	}
	if ((*arg)->type != IS_OBJECT) {
		RETURN_FALSE;
	}
	RETURN_STRINGL((*arg)->value.obj.ce->name,	(*arg)->value.obj.ce->name_length, 1);
}
/* }}} */

/* {{{ proto string get_parent_class(object object)
   Retrieves the parent class name */
ZEND_FUNCTION(get_parent_class)
{
	zval **arg;
	
	if (ZEND_NUM_ARGS()!=1 || zend_get_parameters_ex(1, &arg)==FAILURE) {
		ZEND_WRONG_PARAM_COUNT();
	}
	if (((*arg)->type != IS_OBJECT) || !(*arg)->value.obj.ce->parent) {
		RETURN_FALSE;
	}
	RETURN_STRINGL((*arg)->value.obj.ce->parent->name,	(*arg)->value.obj.ce->parent->name_length, 1);
}
/* }}} */

/* {{{ proto bool is_subclass_of(object object, string class_name)
   Returns true if the object has this class as one of its parents */
ZEND_FUNCTION(is_subclass_of)
{
	zval **obj, **class_name;
	char *lcname;
	zend_class_entry *parent_ce = NULL;

	if (ZEND_NUM_ARGS() != 2 || zend_get_parameters_ex(2, &obj, &class_name)==FAILURE) {
		ZEND_WRONG_PARAM_COUNT();
	}

	if ((*obj)->type != IS_OBJECT) {
		RETURN_FALSE;
	}

	convert_to_string_ex(class_name);
	lcname = estrndup((*class_name)->value.str.val, (*class_name)->value.str.len);
	zend_str_tolower(lcname, (*class_name)->value.str.len);

	for (parent_ce = (*obj)->value.obj.ce->parent; parent_ce != NULL; parent_ce = parent_ce->parent) {
		if (!strcmp(parent_ce->name, lcname)) {
			efree(lcname);
			RETURN_TRUE;
		}
	}
	efree(lcname);
	RETURN_FALSE;
}
/* }}} */

/* {{{ proto array get_class_vars(string class_name)
   Returns an array of default properties of the class */
ZEND_FUNCTION(get_class_vars)
{
	zval **class_name;
	char *lcname;
	zend_class_entry *ce;
	zval *tmp;
	CLS_FETCH();

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &class_name)==FAILURE) {
		ZEND_WRONG_PARAM_COUNT();
	}

	convert_to_string_ex(class_name);
	lcname = estrndup((*class_name)->value.str.val, (*class_name)->value.str.len);
	zend_str_tolower(lcname, (*class_name)->value.str.len);

	if (zend_hash_find(CG(class_table), lcname, (*class_name)->value.str.len+1, (void **)&ce)==FAILURE) {
		efree(lcname);
		RETURN_FALSE;
	} else {
		efree(lcname);
		array_init(return_value);
		if (!ce->constants_updated) {
			zend_hash_apply_with_argument(&ce->default_properties, (int (*)(void *,void *)) zval_update_constant, (void *) 1);
			ce->constants_updated = 1;
		}
		zend_hash_copy(return_value->value.ht, &ce->default_properties, (copy_ctor_func_t) zval_add_ref, (void *) &tmp, sizeof(zval *));
	}
}
/* }}} */

/* {{{ proto array get_object_vars(object obj)
   Returns an array of object properties */
ZEND_FUNCTION(get_object_vars)
{
	zval **obj;
	zval *tmp;

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &obj) == FAILURE) {
		ZEND_WRONG_PARAM_COUNT();
	}

	if ((*obj)->type != IS_OBJECT) {
		RETURN_FALSE;
	}

	array_init(return_value);
	zend_hash_copy(return_value->value.ht, (*obj)->value.obj.properties,
				   (copy_ctor_func_t) zval_add_ref, (void *) &tmp, sizeof(zval *));
}
/* }}} */

/* {{{ proto array get_class_methods(string class_name)
   Returns an array of class methods' names */
ZEND_FUNCTION(get_class_methods)
{
	zval **class_name;
	zval *method_name;
	char *lcname;
	zend_class_entry *ce;
	char *string_key;
	ulong num_key;
	int key_type;
	CLS_FETCH();

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &class_name)==FAILURE) {
		ZEND_WRONG_PARAM_COUNT();
	}

	convert_to_string_ex(class_name);
	lcname = estrndup((*class_name)->value.str.val, (*class_name)->value.str.len);
	zend_str_tolower(lcname, (*class_name)->value.str.len);

	if (zend_hash_find(CG(class_table), lcname, (*class_name)->value.str.len+1, (void **)&ce)==FAILURE) {
		efree(lcname);
		RETURN_NULL();
	} else {
		efree(lcname);
		array_init(return_value);
		zend_hash_internal_pointer_reset(&ce->function_table);
		while ((key_type = zend_hash_get_current_key(&ce->function_table, &string_key, &num_key)) != HASH_KEY_NON_EXISTANT) {
			if (key_type == HASH_KEY_IS_STRING) {
				MAKE_STD_ZVAL(method_name);
				ZVAL_STRING(method_name, string_key, 0);
				zend_hash_next_index_insert(return_value->value.ht, &method_name, sizeof(zval *), NULL);
			}
			zend_hash_move_forward(&ce->function_table);
		}
	}
}
/* }}} */

/* {{{ proto bool method_exists(object object, string method)
   Checks if the class method exists */
ZEND_FUNCTION(method_exists)
{
	zval **klass, **method_name;
	char *lcname;
	
	if (ZEND_NUM_ARGS()!=2 || zend_get_parameters_ex(2, &klass, &method_name)==FAILURE) {
		ZEND_WRONG_PARAM_COUNT();
	}
	if ((*klass)->type != IS_OBJECT) {
		RETURN_FALSE;
	}
	convert_to_string_ex(method_name);
	lcname = estrndup((*method_name)->value.str.val, (*method_name)->value.str.len);
	zend_str_tolower(lcname, (*method_name)->value.str.len);
	if(zend_hash_exists(&(*klass)->value.obj.ce->function_table, lcname, (*method_name)->value.str.len+1)) {
		efree(lcname);
		RETURN_TRUE;
	} else {
		efree(lcname);
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ proto bool class_exists(string classname)
   Checks if the class exists */
ZEND_FUNCTION(class_exists)
{
	zval **class_name;
	char *lcname;
	CLS_FETCH();

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &class_name)==FAILURE) {
		ZEND_WRONG_PARAM_COUNT();
	}
	convert_to_string_ex(class_name);
	lcname = estrndup((*class_name)->value.str.val, (*class_name)->value.str.len);
	zend_str_tolower(lcname, (*class_name)->value.str.len);
	if (zend_hash_exists(CG(class_table), lcname, (*class_name)->value.str.len+1)) {
		efree(lcname);
		RETURN_TRUE;
	} else {
		efree(lcname);
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ proto bool function_exists(string function_name) 
   Checks if the function exists */
ZEND_FUNCTION(function_exists)
{
	zval **function_name;
	char *lcname;
	int retval;
	
	if (ZEND_NUM_ARGS()!=1 || zend_get_parameters_ex(1, &function_name)==FAILURE) {
		ZEND_WRONG_PARAM_COUNT();
	}
	convert_to_string_ex(function_name);
	lcname = estrndup((*function_name)->value.str.val, (*function_name)->value.str.len);
	zend_str_tolower(lcname, (*function_name)->value.str.len);

	retval = zend_hash_exists(EG(function_table), lcname, (*function_name)->value.str.len+1);
	efree(lcname);

	RETURN_BOOL(retval);
}
/* }}} */


ZEND_FUNCTION(leak)
{
	int leakbytes=3;
	zval **leak;

	if (ZEND_NUM_ARGS()>=1) {
		if (zend_get_parameters_ex(1, &leak)==SUCCESS) {
			convert_to_long_ex(leak);
			leakbytes = (*leak)->value.lval;
		}
	}
	
	emalloc(leakbytes);
}


#ifdef ZEND_TEST_EXCEPTIONS
ZEND_FUNCTION(crash)
{
	char *nowhere=NULL;

	memcpy(nowhere, "something", sizeof("something"));
}
#endif


static int copy_import_use_file(zend_file_handle *fh, zval *array)
{
	if (fh->filename) {
		char *extension_start;

		extension_start = strstr(fh->filename, zend_uv.import_use_extension);
		if (extension_start) {
			*extension_start = 0;
			if (fh->opened_path) {
				add_assoc_string(array, fh->filename, fh->opened_path, 1);
			} else {
				add_assoc_stringl(array, fh->filename, "N/A", sizeof("N/A")-1, 1);
			}
			*extension_start = zend_uv.import_use_extension[0];
		}
	}
	return 0;
}


/* {{{ proto array get_included_files(void)
   Returns an array with the file names that were include_once()'d */
ZEND_FUNCTION(get_included_files)
{
	char *entry;
	if (ZEND_NUM_ARGS() != 0) {
		ZEND_WRONG_PARAM_COUNT();
	}

	array_init(return_value);
	zend_hash_internal_pointer_reset(&EG(included_files));
	while(zend_hash_get_current_key(&EG(included_files), &entry,NULL) == HASH_KEY_IS_STRING) {
		add_next_index_string(return_value,entry,0);
		zend_hash_move_forward(&EG(included_files));
	}

	/*	zend_hash_apply_with_argument(&EG(included_files), (apply_func_arg_t) copy_import_use_file, return_value); */
}
/* }}} */


/* {{{ proto void trigger_error(string messsage [, int error_type])
   Generates a user-level error/warning/notice message */
ZEND_FUNCTION(trigger_error)
{
	int error_type = E_USER_NOTICE;
	zval **z_error_type, **z_error_message;

	switch(ZEND_NUM_ARGS()) {
		case 1:
			if (zend_get_parameters_ex(1, &z_error_message)==FAILURE) {
				ZEND_WRONG_PARAM_COUNT();
			}
			break;
		case 2:
			if (zend_get_parameters_ex(2, &z_error_message, &z_error_type)==FAILURE) {
				ZEND_WRONG_PARAM_COUNT();
			}
			convert_to_long_ex(z_error_type);
			error_type = (*z_error_type)->value.lval;
			switch (error_type) {
				case E_USER_ERROR:
				case E_USER_WARNING:
				case E_USER_NOTICE:
					break;
				default:
					zend_error(E_WARNING, "Invalid error type specified");
					RETURN_FALSE;
					break;
			}
			break;
		default:
			ZEND_WRONG_PARAM_COUNT();	
	}
	convert_to_string_ex(z_error_message);
	zend_error(error_type, "%s", (*z_error_message)->value.str.val);
	RETURN_TRUE;
}
/* }}} */


/* {{{ proto string set_error_handler(string error_handler)
   Sets a user-defined error handler function.  Returns the previously defined error handler, or false on error */
ZEND_FUNCTION(set_error_handler)
{
	zval **error_handler;
	zend_bool had_orig_error_handler=0;

	if (ZEND_NUM_ARGS()!=1 || zend_get_parameters_ex(1, &error_handler)==FAILURE) {
		ZEND_WRONG_PARAM_COUNT();
	}

	convert_to_string_ex(error_handler);
	if (EG(user_error_handler)) {
		had_orig_error_handler = 1;
		*return_value = *EG(user_error_handler);
		zval_copy_ctor(return_value);
		zend_ptr_stack_push(&EG(user_error_handlers), EG(user_error_handler));
	}
	ALLOC_ZVAL(EG(user_error_handler));

	if (Z_STRLEN_PP(error_handler)==0) { /* unset user-defined handler */
		FREE_ZVAL(EG(user_error_handler));
		EG(user_error_handler) = NULL;
		RETURN_TRUE;
	}

	*EG(user_error_handler) = **error_handler;
	zval_copy_ctor(EG(user_error_handler));

	if (!had_orig_error_handler) {
		RETURN_NULL();
	}
}
/* }}} */



/* {{{ proto void restore_error_handler(void)
   Restores the previously defined error handler function */
ZEND_FUNCTION(restore_error_handler)
{
	if (EG(user_error_handler)) {
		zval_ptr_dtor(&EG(user_error_handler));
	}
	if (zend_ptr_stack_num_elements(&EG(user_error_handlers))==0) {
		EG(user_error_handler) = NULL;
	} else {
		EG(user_error_handler) = zend_ptr_stack_pop(&EG(user_error_handlers));
	}
	RETURN_TRUE;
}


static int copy_class_name(zend_class_entry *ce, int num_args, va_list args, zend_hash_key *hash_key)
{
	zval *array = va_arg(args, zval *);

	if (hash_key->nKeyLength==0 || hash_key->arKey[0]!=0) {
		add_next_index_stringl(array, ce->name, ce->name_length, 1);
	}
	return 0;
}

/* {{{ proto array get_declared_classes(void)
   Returns an array of all declared classes. */
ZEND_FUNCTION(get_declared_classes)
{
	CLS_FETCH();

	if (ZEND_NUM_ARGS() != 0) {
		ZEND_WRONG_PARAM_COUNT();
	}

	array_init(return_value);
	zend_hash_apply_with_arguments(CG(class_table), (apply_func_args_t)copy_class_name, 1, return_value);
}
/* }}} */

static int copy_function_name(zend_function *func, int num_args, va_list args, zend_hash_key *hash_key)
{
	zval *internal_ar = va_arg(args, zval *),
	     *user_ar     = va_arg(args, zval *);

	if (hash_key->nKeyLength == 0 || hash_key->arKey[0] == 0) {
		return 0;
	}

	if (func->type == ZEND_INTERNAL_FUNCTION) {
		add_next_index_stringl(internal_ar, hash_key->arKey, hash_key->nKeyLength, 1);
	} else if (func->type == ZEND_USER_FUNCTION) {
		add_next_index_stringl(user_ar, hash_key->arKey, hash_key->nKeyLength, 1);
	}
	
	return 0;
}

/* {{{ proto array get_defined_functions(void)
   Returns an array of all defined functions */
ZEND_FUNCTION(get_defined_functions)
{
	zval *internal;
	zval *user;
	
	if (ZEND_NUM_ARGS() != 0) {
		ZEND_WRONG_PARAM_COUNT();
	}
	
	MAKE_STD_ZVAL(internal);
	MAKE_STD_ZVAL(user);
	
	array_init(internal);
	array_init(user);
	array_init(return_value);
	
	zend_hash_apply_with_arguments(EG(function_table), (apply_func_args_t)copy_function_name, 2, internal, user);
	
	if (zend_hash_add(return_value->value.ht, "internal", sizeof("internal"), (void **)&internal, sizeof(zval *), NULL) == FAILURE) {
		zend_error(E_WARNING, "Cannot add internal functions to return value from get_defined_functions()");
		RETURN_FALSE;
	}
	
	if (zend_hash_add(return_value->value.ht, "user", sizeof("user"), (void **)&user, sizeof(zval *), NULL) == FAILURE) {
		zend_error(E_WARNING, "Cannot add user functions to return value from get_defined_functions()");
		RETURN_FALSE;
	}
}
/* }}} */

/* {{{ proto array get_defined_vars(void)
   Returns an associative array of names and values of all currently defined variable names (variables in the current scope) */
ZEND_FUNCTION(get_defined_vars)
{	
	zval *tmp;
	
	array_init(return_value);
	
    zend_hash_copy(return_value->value.ht, EG(active_symbol_table),
                   (copy_ctor_func_t)zval_add_ref, &tmp, sizeof(zval *));
}
/* }}} */

#define LAMBDA_TEMP_FUNCNAME	"__lambda_func"

/* {{{ proto string create_function(string args, string code)
   Creates an anonymous function, and returns its name (funny, eh?) */
ZEND_FUNCTION(create_function)
{
	char *eval_code, *function_name;
	int eval_code_length, function_name_length;
	zval **z_function_args, **z_function_code;
	int retval;
	char *eval_name;
	CLS_FETCH();

	if (ZEND_NUM_ARGS()!=2 || zend_get_parameters_ex(2, &z_function_args, &z_function_code)==FAILURE) {
		ZEND_WRONG_PARAM_COUNT();
	}

	convert_to_string_ex(z_function_args);
	convert_to_string_ex(z_function_code);

	eval_code_length = sizeof("function " LAMBDA_TEMP_FUNCNAME)
			+Z_STRLEN_PP(z_function_args)
			+2	/* for the args parentheses */
			+2	/* for the curly braces */
			+Z_STRLEN_PP(z_function_code);

	eval_code = (char *) emalloc(eval_code_length);
	sprintf(eval_code, "function " LAMBDA_TEMP_FUNCNAME "(%s){%s}", Z_STRVAL_PP(z_function_args), Z_STRVAL_PP(z_function_code));

	eval_name = zend_make_compiled_string_description("runtime-created function");
	retval = zend_eval_string(eval_code, NULL, eval_name CLS_CC ELS_CC);
	efree(eval_code);
	efree(eval_name);

	if (retval==SUCCESS) {
		zend_function *func;

		if (zend_hash_find(EG(function_table), LAMBDA_TEMP_FUNCNAME, sizeof(LAMBDA_TEMP_FUNCNAME), (void **) &func)==FAILURE) {
			zend_error(E_ERROR, "Unexpected inconsistency in create_function()");
			RETURN_FALSE;
		}
		function_add_ref(func);

		function_name = (char *) emalloc(sizeof("0lambda_")+MAX_LENGTH_OF_LONG);

		do {
			sprintf(function_name, "%clambda_%d", 0, ++EG(lambda_count));
			function_name_length = strlen(function_name+1)+1;
		} while (zend_hash_add(EG(function_table), function_name, function_name_length+1, func, sizeof(zend_function), NULL)==FAILURE);
		zend_hash_del(EG(function_table), LAMBDA_TEMP_FUNCNAME, sizeof(LAMBDA_TEMP_FUNCNAME));
		RETURN_STRINGL(function_name, function_name_length, 0);
	} else {
		RETURN_FALSE;
	}
}
/* }}} */


#if ZEND_DEBUG
ZEND_FUNCTION(zend_test_func)
{
	zval *arg1, *arg2;

	zend_get_parameters(ht, 2, &arg1, &arg2);
}
#endif


ZEND_FUNCTION(get_resource_type)
{
	char *resource_type;
	zval **z_resource_type;

	if (ZEND_NUM_ARGS()!=1 || zend_get_parameters_ex(1, &z_resource_type)==FAILURE) {
		ZEND_WRONG_PARAM_COUNT();
	}

	if (Z_TYPE_PP(z_resource_type) != IS_RESOURCE) {
		zend_error(E_WARNING, "Supplied argument is not a valid resource handle");
		RETURN_FALSE;
	}

	resource_type = zend_rsrc_list_get_rsrc_type(Z_LVAL_PP(z_resource_type));
	if (resource_type) {
		RETURN_STRING(resource_type, 1);
	} else {
		RETURN_STRING("Unknown", 1);
	}
}
