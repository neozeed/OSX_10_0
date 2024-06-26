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
   | Authors: Andi Gutmans <andi@zend.com>                                |
   |          Zeev Suraski <zeev@zend.com>                                |
   |          Rasmus Lerdorf <rasmus@php.net>                             |
   |          Andrei Zmievski <andrei@ispi.net>                           |
   +----------------------------------------------------------------------+
*/

/* $Id: php_array.h,v 1.1.1.3 2001/01/25 05:00:09 wsanchez Exp $ */

#ifndef PHP_ARRAY_H
#define PHP_ARRAY_H

PHP_MINIT_FUNCTION(array);
PHP_MSHUTDOWN_FUNCTION(array);

PHP_FUNCTION(ksort);
PHP_FUNCTION(krsort);
PHP_FUNCTION(natsort);
PHP_FUNCTION(natcasesort);
PHP_FUNCTION(asort);
PHP_FUNCTION(arsort);
PHP_FUNCTION(sort);
PHP_FUNCTION(rsort);
PHP_FUNCTION(usort);
PHP_FUNCTION(uasort);
PHP_FUNCTION(uksort);
PHP_FUNCTION(array_walk);
PHP_FUNCTION(count);
PHP_FUNCTION(end);
PHP_FUNCTION(prev);
PHP_FUNCTION(next);
PHP_FUNCTION(reset);
PHP_FUNCTION(current);
PHP_FUNCTION(key);
PHP_FUNCTION(min);
PHP_FUNCTION(max);
PHP_FUNCTION(in_array);
PHP_FUNCTION(extract);
PHP_FUNCTION(compact);
PHP_FUNCTION(range);
PHP_FUNCTION(shuffle);
PHP_FUNCTION(array_multisort);
PHP_FUNCTION(array_push);
PHP_FUNCTION(array_pop);
PHP_FUNCTION(array_shift);
PHP_FUNCTION(array_unshift);
PHP_FUNCTION(array_splice);
PHP_FUNCTION(array_slice);
PHP_FUNCTION(array_merge);
PHP_FUNCTION(array_merge_recursive);
PHP_FUNCTION(array_keys);
PHP_FUNCTION(array_values);
PHP_FUNCTION(array_count_values);
PHP_FUNCTION(array_reverse);
PHP_FUNCTION(array_pad);
PHP_FUNCTION(array_flip);
PHP_FUNCTION(array_rand);
PHP_FUNCTION(array_unique);
PHP_FUNCTION(array_intersect);
PHP_FUNCTION(array_diff);
PHP_FUNCTION(array_sum);

HashTable* php_splice(HashTable *, int, int, zval ***, int, HashTable **);
int multisort_compare(const void *a, const void *b);

typedef struct {
	int *multisort_flags[2];
	int (*compare_func)(zval *result, zval *op1, zval *op2);
} php_array_globals;

#ifdef ZTS
#define ARRAYLS_D php_array_globals *array_globals
#define ARRAYLS_DC , ARRAYLS_D
#define ARRAYLS_C array_globals
#define ARRAYLS_CC , ARRAYLS_C
#define ARRAYG(v) (array_globals->v)
#define ARRAYLS_FETCH() php_array_globals *array_globals = ts_resource(array_globals_id)
extern int array_globals_id;
#else
#define ARRAYLS_D
#define ARRAYLS_DC
#define ARRAYLS_C
#define ARRAYLS_CC
#define ARRAYG(v) (array_globals.v)
#define ARRAYLS_FETCH()
extern php_array_globals array_globals;
#endif

#endif /* PHP_ARRAY_H */
