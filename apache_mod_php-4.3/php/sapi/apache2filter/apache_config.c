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

#include "php.h"
#include "php_ini.h"

#include "apr_strings.h"
#include "ap_config.h"
#include "util_filter.h"
#include "httpd.h"
#include "http_config.h"
#include "http_request.h"
#include "http_core.h"
#include "http_protocol.h"
#include "http_log.h"
#include "http_main.h"
#include "util_script.h"
#include "http_core.h"                         

typedef struct {
	HashTable config;
} php_conf_rec;

typedef struct {
	char *value;
	size_t value_len;
	char status;
} php_dir_entry;

static const char *real_value_hnd(cmd_parms *cmd, void *dummy, const char *name, const char *value, int status)
{
	php_conf_rec *d = dummy;
	php_dir_entry e;
	php_dir_entry *pe;
	size_t str_len;

	fprintf(stderr, "Getting %s=%s for %p (%d)\n", name, value, dummy, zend_hash_num_elements(&d->config));
	e.value = apr_pstrdup(cmd->pool, value);
	e.value_len = strlen(value);
	e.status = status;
	
	str_len = strlen(name);
	
	if (zend_hash_find(&d->config, name, str_len + 1, (void **) &pe) == SUCCESS) {
		if (pe->status > status)
			return NULL;
	}
	
	zend_hash_update(&d->config, name, strlen(name) + 1, &e, sizeof(e),
			NULL);
	return NULL;
}

static const char *php_apache_value_handler(cmd_parms *cmd, void *dummy, const char *name, const char *value)
{
	return real_value_hnd(cmd, dummy, name, value, PHP_INI_USER);
}

static const char *php_apache_admin_value_handler(cmd_parms *cmd, void *dummy, const char *name, const char *value)
{
	return real_value_hnd(cmd, dummy, name, value, PHP_INI_SYSTEM);
}

void *merge_php_config(apr_pool_t *p, void *base_conf, void *new_conf)
{
	php_conf_rec *d = base_conf, *e = new_conf;
	php_dir_entry *pe;
	php_dir_entry *data;
	char *str;
	ulong str_len;
	ulong num_index;

	fprintf(stderr, "Merge dir (%p) (%p)\n", base_conf, new_conf);
	for (zend_hash_internal_pointer_reset(&d->config);
			zend_hash_get_current_key_ex(&d->config, &str, &str_len, &num_index, NULL) == HASH_KEY_IS_STRING;
			zend_hash_move_forward(&d->config)) {
		pe = NULL;
		zend_hash_get_current_data(&d->config, (void **) &data);
		if (zend_hash_find(&e->config, str, str_len, (void **) &pe) == SUCCESS) {
			if (pe->status >= data->status) continue;
		}
		zend_hash_update(&e->config, str, str_len, data, sizeof(*data), NULL);
		fprintf(stderr, "ADDING/OVERWRITING %s (%d vs. %d)\n", str, data->status, pe?pe->status:-1);
	}
	return new_conf;
}

void apply_config(void *dummy)
{
	php_conf_rec *d = dummy;
	char *str;
	ulong str_len;
	php_dir_entry *data;
	
	for (zend_hash_internal_pointer_reset(&d->config);
			zend_hash_get_current_key_ex(&d->config, &str, &str_len, NULL, NULL) == HASH_KEY_IS_STRING;
			zend_hash_move_forward(&d->config)) {
		zend_hash_get_current_data(&d->config, (void **) &data);
		fprintf(stderr, "APPLYING (%s)(%s)\n", str, data->value);
		if (zend_alter_ini_entry(str, str_len, data->value, data->value_len + 1, 
				data->status, PHP_INI_STAGE_RUNTIME) == FAILURE)
			fprintf(stderr, "..FAILED\n");
	}
}

const command_rec php_dir_cmds[] =
{
	AP_INIT_TAKE2("php_value", php_apache_value_handler, NULL, OR_OPTIONS,
                  "PHP Value Modifier"),
	AP_INIT_TAKE2("php_admin_value", php_apache_admin_value_handler, NULL, OR_NONE,
                  "PHP Value Modifier"),
   {NULL}
};

static apr_status_t destroy_php_config(void *data)
{
	php_conf_rec *d = data;

	fprintf(stderr, "Destroying config %p\n", data);	
	zend_hash_destroy(&d->config);

	return APR_SUCCESS;
}

void *create_php_config(apr_pool_t *p, char *dummy)
{
    php_conf_rec *newx =
    (php_conf_rec *) apr_pcalloc(p, sizeof(*newx));

	fprintf(stderr, "Creating new config (%p) for %s\n", newx, dummy);
	zend_hash_init(&newx->config, 0, NULL, NULL, 1);
	apr_register_cleanup(p, newx, destroy_php_config, NULL);
    return (void *) newx;
}

