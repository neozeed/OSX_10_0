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
   | Author:  Andrei Zmievski <andrei@ispi.net>                           |
   +----------------------------------------------------------------------+
 */

/* $Id: php_wddx_api.h,v 1.1.1.2 2000/09/07 00:06:16 wsanchez Exp $ */

#ifndef PHP_WDDX_API_H
#define PHP_WDDX_API_H

#define WDDX_ARRAY_S			"<array length='%d'>"
#define WDDX_ARRAY_E			"</array>"
#define WDDX_BINARY_S			"<binary>"
#define WDDX_BINARY_E			"</binary>"
#define WDDX_BOOLEAN			"<boolean value='%s'/>"
#define WDDX_CHAR				"<char code='%02X'/>"
#define WDDX_COMMENT_S			"<comment>"
#define WDDX_COMMENT_E			"</comment>"
#define WDDX_DATA_S				"<data>"
#define WDDX_DATA_E				"</data>"
#define WDDX_HEADER				"<header/>"
#define WDDX_HEADER_S			"<header>"
#define WDDX_HEADER_E			"</header>"
#define WDDX_NULL				"<null/>"
#define WDDX_NUMBER				"<number>%s</number>"
#define WDDX_PACKET_S			"<wddxPacket version='1.0'>"
#define WDDX_PACKET_E			"</wddxPacket>"
#define WDDX_STRING_S			"<string>"
#define WDDX_STRING_E			"</string>"
#define WDDX_STRUCT_S			"<struct>"
#define WDDX_STRUCT_E			"</struct>"
#define WDDX_VAR_S				"<var name='%s'>"
#define WDDX_VAR_E				"</var>"

#define php_wddx_add_chunk(packet, str) { \
		char *__s = (str); \
		php_wddx_add_chunk_ex(packet, __s, strlen(__s)); \
	}
#define php_wddx_add_chunk_static(packet, str) \
	php_wddx_add_chunk_ex(packet, str, sizeof(str)-1);

typedef struct _wddx_packet wddx_packet;

wddx_packet *php_wddx_constructor(void);
void 		 php_wddx_destructor(wddx_packet *packet);

void 		 php_wddx_packet_start(wddx_packet *packet, char *comment, int comment_len);
void 		 php_wddx_packet_end(wddx_packet *packet);

void 		 php_wddx_serialize_var(wddx_packet *packet, zval *var, char *name);
void 		 php_wddx_add_chunk_ex(wddx_packet *packet, char *str, int length);
int 		 php_wddx_deserialize_ex(char *, int, zval *return_value);
char		*php_wddx_gather(wddx_packet *packet);

#endif /* PHP_WDDX_API_H */
