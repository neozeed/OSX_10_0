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
   | Author: Zeev Suraski <zeev@zend.com>                                 |
   +----------------------------------------------------------------------+
*/


#ifndef ZEND_INI_H
#define ZEND_INI_H

#define ZEND_INI_USER	(1<<0)
#define ZEND_INI_PERDIR	(1<<1)
#define ZEND_INI_SYSTEM	(1<<2)

#define ZEND_INI_ALL (ZEND_INI_USER|ZEND_INI_PERDIR|ZEND_INI_SYSTEM)

typedef struct _zend_ini_entry zend_ini_entry;

#define ZEND_INI_MH(name) int name(zend_ini_entry *entry, char *new_value, uint new_value_length, void *mh_arg1, void *mh_arg2, void *mh_arg3, int stage)
#define ZEND_INI_DISP(name) void name(zend_ini_entry *ini_entry, int type)

struct _zend_ini_entry {
	int module_number;
	int modifyable;
	char *name;
	uint name_length;
	ZEND_INI_MH((*on_modify));
	void *mh_arg1;
	void *mh_arg2;
	void *mh_arg3;

	char *value;
	uint value_length;

	char *orig_value;
	uint orig_value_length;
	int modified;

	void (*displayer)(zend_ini_entry *ini_entry, int type);
};


int zend_ini_mstartup(void);
int zend_ini_mshutdown(void);
int zend_ini_rshutdown(void);

void zend_ini_sort_entries(void);

ZEND_API int zend_register_ini_entries(zend_ini_entry *ini_entry, int module_number);
ZEND_API void zend_unregister_ini_entries(int module_number);
ZEND_API void zend_ini_refresh_caches(int stage);
ZEND_API int zend_alter_ini_entry(char *name, uint name_length, char *new_value, uint new_value_length, int modify_type, int stage);
ZEND_API int zend_restore_ini_entry(char *name, uint name_length, int stage);
ZEND_API void display_ini_entries(zend_module_entry *module);

ZEND_API long zend_ini_long(char *name, uint name_length, int orig);
ZEND_API double zend_ini_double(char *name, uint name_length, int orig);
ZEND_API char *zend_ini_string(char *name, uint name_length, int orig);
zend_ini_entry *get_ini_entry(char *name, uint name_length);

ZEND_API int zend_ini_register_displayer(char *name, uint name_length, void (*displayer)(zend_ini_entry *ini_entry, int type));

ZEND_API void zend_ini_apply_with_argument(apply_func_arg_t apply_func, void *arg);

ZEND_API ZEND_INI_DISP(zend_ini_boolean_displayer_cb);
ZEND_API ZEND_INI_DISP(zend_ini_color_displayer_cb);
ZEND_API ZEND_INI_DISP(display_link_numbers);

#define ZEND_INI_BEGIN()		static zend_ini_entry ini_entries[] = {
#define ZEND_INI_END()		{ 0, 0, NULL, 0, NULL, NULL, NULL, NULL, NULL, 0, NULL, 0, 0, NULL } };

#define ZEND_INI_ENTRY3_EX(name, default_value, modifyable, on_modify, arg1, arg2, arg3, displayer) \
	{ 0, modifyable, name, sizeof(name), on_modify, arg1, arg2, arg3, default_value, sizeof(default_value)-1, NULL, 0, 0, displayer },

#define ZEND_INI_ENTRY3(name, default_value, modifyable, on_modify, arg1, arg2, arg3) \
	ZEND_INI_ENTRY3_EX(name, default_value, modifyable, on_modify, arg1, arg2, arg3, NULL)

#define ZEND_INI_ENTRY2_EX(name, default_value, modifyable, on_modify, arg1, arg2, displayer) \
	ZEND_INI_ENTRY3_EX(name, default_value, modifyable, on_modify, arg1, arg2, NULL, displayer)

#define ZEND_INI_ENTRY2(name, default_value, modifyable, on_modify, arg1, arg2) \
	ZEND_INI_ENTRY2_EX(name, default_value, modifyable, on_modify, arg1, arg2, NULL)

#define ZEND_INI_ENTRY1_EX(name, default_value, modifyable, on_modify, arg1, displayer) \
	ZEND_INI_ENTRY3_EX(name, default_value, modifyable, on_modify, arg1, NULL, NULL, displayer)

#define ZEND_INI_ENTRY1(name, default_value, modifyable, on_modify, arg1) \
	ZEND_INI_ENTRY1_EX(name, default_value, modifyable, on_modify, arg1, NULL)
	
#define ZEND_INI_ENTRY_EX(name, default_value, modifyable, on_modify, displayer) \
	ZEND_INI_ENTRY3_EX(name, default_value, modifyable, on_modify, NULL, NULL, NULL, displayer)

#define ZEND_INI_ENTRY(name, default_value, modifyable, on_modify) \
	ZEND_INI_ENTRY_EX(name, default_value, modifyable, on_modify, NULL)

#ifdef ZTS
#define STD_ZEND_INI_ENTRY(name, default_value, modifyable, on_modify, property_name, struct_type, struct_ptr) \
	ZEND_INI_ENTRY2(name, default_value, modifyable, on_modify, (void *) XtOffsetOf(struct_type, property_name), (void *) &struct_ptr##_id)
#define STD_ZEND_INI_ENTRY_EX(name, default_value, modifyable, on_modify, property_name, struct_type, struct_ptr, displayer) \
	ZEND_INI_ENTRY2_EX(name, default_value, modifyable, on_modify, (void *) XtOffsetOf(struct_type, property_name), (void *) &struct_ptr##_id, displayer)
#define STD_ZEND_INI_BOOLEAN(name, default_value, modifyable, on_modify, property_name, struct_type, struct_ptr) \
	ZEND_INI_ENTRY3_EX(name, default_value, modifyable, on_modify, (void *) XtOffsetOf(struct_type, property_name), (void *) &struct_ptr##_id, NULL, zend_ini_boolean_displayer_cb)
#else
#define STD_ZEND_INI_ENTRY(name, default_value, modifyable, on_modify, property_name, struct_type, struct_ptr) \
	ZEND_INI_ENTRY2(name, default_value, modifyable, on_modify, (void *) XtOffsetOf(struct_type, property_name), (void *) &struct_ptr)
#define STD_ZEND_INI_ENTRY_EX(name, default_value, modifyable, on_modify, property_name, struct_type, struct_ptr, displayer) \
	ZEND_INI_ENTRY2_EX(name, default_value, modifyable, on_modify, (void *) XtOffsetOf(struct_type, property_name), (void *) &struct_ptr, displayer)
#define STD_ZEND_INI_BOOLEAN(name, default_value, modifyable, on_modify, property_name, struct_type, struct_ptr) \
	ZEND_INI_ENTRY3_EX(name, default_value, modifyable, on_modify, (void *) XtOffsetOf(struct_type, property_name), (void *) &struct_ptr, NULL, zend_ini_boolean_displayer_cb)
#endif

#define INI_INT(name) zend_ini_long((name), sizeof(name), 0)
#define INI_FLT(name) zend_ini_double((name), sizeof(name), 0)
#define INI_STR(name) zend_ini_string((name), sizeof(name), 0)
#define INI_BOOL(name) ((zend_bool) INI_INT(name))

#define INI_ORIG_INT(name)	zend_ini_long((name), sizeof(name), 1)
#define INI_ORIG_FLT(name)	zend_ini_double((name), sizeof(name), 1)
#define INI_ORIG_STR(name)	zend_ini_string((name), sizeof(name), 1)
#define INI_ORIG_BOOL(name) ((zend_bool) INI_ORIG_INT(name))


#define REGISTER_INI_ENTRIES() zend_register_ini_entries(ini_entries, module_number)
#define UNREGISTER_INI_ENTRIES() zend_unregister_ini_entries(module_number)
#define DISPLAY_INI_ENTRIES() display_ini_entries(zend_module)

#define REGISTER_INI_DISPLAYER(name, displayer) zend_ini_register_displayer((name), sizeof(name), displayer)
#define REGISTER_INI_BOOLEAN(name) REGISTER_INI_DISPLAYER(name, zend_ini_boolean_displayer_cb)

zval *cfg_get_entry(char *name, uint name_length);


/* Standard message handlers */
ZEND_API ZEND_INI_MH(OnUpdateBool);
ZEND_API ZEND_INI_MH(OnUpdateInt);
ZEND_API ZEND_INI_MH(OnUpdateReal);
ZEND_API ZEND_INI_MH(OnUpdateString);
ZEND_API ZEND_INI_MH(OnUpdateStringUnempty);


#define ZEND_INI_DISPLAY_ORIG	1
#define ZEND_INI_DISPLAY_ACTIVE	2

#define ZEND_INI_STAGE_STARTUP		(1<<0)
#define ZEND_INI_STAGE_SHUTDOWN		(1<<1)
#define ZEND_INI_STAGE_ACTIVATE		(1<<2)
#define ZEND_INI_STAGE_DEACTIVATE	(1<<3)
#define ZEND_INI_STAGE_RUNTIME		(1<<4)

/* INI parsing engine */
typedef void (*zend_ini_parser_cb_t)(zval *arg1, zval *arg2, int callback_type, void *arg);
int zend_parse_ini_file(zend_file_handle *fh, zend_bool unbuffered_errors, zend_ini_parser_cb_t ini_parser_cb, void *arg);
#define ZEND_INI_PARSER_ENTRY	1
#define ZEND_INI_PARSER_SECTION	2

#endif /* ZEND_INI_H */
