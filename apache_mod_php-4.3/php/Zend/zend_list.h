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


#ifndef ZEND_LIST_H
#define ZEND_LIST_H

#include "zend_hash.h"
#include "zend_globals.h"


#define ZEND_RESOURCE_LIST_TYPE_STD	1
#define ZEND_RESOURCE_LIST_TYPE_EX	2

typedef struct _zend_rsrc_list_entry {
	void *ptr;
	int type;
	int refcount;
	zend_bool valid;
} zend_rsrc_list_entry;

typedef void (*rsrc_dtor_func_t)(zend_rsrc_list_entry *rsrc);
#define ZEND_RSRC_DTOR_FUNC(name)		void name(zend_rsrc_list_entry *rsrc)

typedef struct _zend_rsrc_list_dtors_entry {
	/* old style destructors */
	void (*list_dtor)(void *);
	void (*plist_dtor)(void *);

	/* new style destructors */
	rsrc_dtor_func_t list_dtor_ex;
	rsrc_dtor_func_t plist_dtor_ex;

	char *type_name;

	int module_number;
	int resource_id;
	unsigned char type;
} zend_rsrc_list_dtors_entry;


#define register_list_destructors(ld, pld) zend_register_list_destructors((void (*)(void *))ld, (void (*)(void *))pld, module_number);
ZEND_API int zend_register_list_destructors(void (*ld)(void *), void (*pld)(void *), int module_number);
ZEND_API int zend_register_list_destructors_ex(rsrc_dtor_func_t ld, rsrc_dtor_func_t pld, char *type_name, int module_number);

enum list_entry_type {
	LE_DB=1000
};

void list_entry_destructor(void *ptr);
void plist_entry_destructor(void *ptr);

void zend_clean_module_rsrc_dtors(int module_number);
int zend_init_rsrc_list(ELS_D);
int zend_init_rsrc_plist(ELS_D);
void zend_destroy_rsrc_list(ELS_D);
void zend_destroy_rsrc_plist(ELS_D);
int zend_init_rsrc_list_dtors();
void zend_destroy_rsrc_list_dtors();

ZEND_API int zend_list_insert(void *ptr, int type);
ZEND_API int zend_plist_insert(void *ptr, int type);
ZEND_API int zend_list_addref(int id);
ZEND_API int zend_list_delete(int id);
ZEND_API int zend_plist_delete(int id);
ZEND_API int zend_list_convert_to_number(int id);
ZEND_API void *zend_list_find(int id, int *type);
ZEND_API void *zend_plist_find(int id, int *type);

ZEND_API int zend_register_resource(zval *rsrc_result, void *rsrc_pointer, int rsrc_type);
ZEND_API void *zend_fetch_resource(zval **passed_id, int default_id, char *resource_type_name, int *found_resource_type, int num_resource_types, ...);

ZEND_API char *zend_rsrc_list_get_rsrc_type(int resource);

extern ZEND_API int le_index_ptr;  /* list entry type for index pointers */

#define ZEND_VERIFY_RESOURCE(rsrc)		\
	if (!rsrc) {						\
		RETURN_NULL();					\
	}

#define ZEND_FETCH_RESOURCE(rsrc, rsrc_type, passed_id, default_id, resource_type_name, resource_type)	\
	rsrc = (rsrc_type) zend_fetch_resource(passed_id, default_id, resource_type_name, NULL, 1, resource_type);	\
	ZEND_VERIFY_RESOURCE(rsrc);

#define ZEND_FETCH_RESOURCE2(rsrc, rsrc_type, passed_id, default_id, resource_type_name, resource_type1,resource_type2)	\
	rsrc = (rsrc_type) zend_fetch_resource(passed_id, default_id, resource_type_name, NULL, 2, resource_type1, resource_type2);	\
	ZEND_VERIFY_RESOURCE(rsrc);

#define ZEND_REGISTER_RESOURCE(rsrc_result, rsrc_pointer, rsrc_type)  \
    zend_register_resource(rsrc_result, rsrc_pointer, rsrc_type);

#endif
