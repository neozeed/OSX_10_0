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
   | Authors: clayton collie <clcollie@mindspring.com>                    |
   +----------------------------------------------------------------------+
*/

/* $Id: scanf.h,v 1.1.1.2 2000/09/07 00:06:11 wsanchez Exp $ */

#ifndef  SCANF_H
#define  SCANF_H


#define SCAN_MAX_ARGS   0xFF    /* Maximum number of variable which can be      */
                                /* passed to (f|s)scanf. This is an artifical   */
                                /* upper limit to keep resources in check and   */
                                /* minimize the possibility of exploits         */

#define SCAN_MAX_FSCANF_BUFSIZE		512  /* Max input buffer allocated for fscanf */
#define SCAN_SUCCESS			SUCCESS	
#define SCAN_ERROR_EOF			-1	/* indicates premature termination of scan 	*/
									/* can be caused by bad parameters or format*/
									/* string.									*/
#define SCAN_ERROR_INVALID_FORMAT		(SCAN_ERROR_EOF - 1)
#define SCAN_ERROR_VAR_PASSED_BYVAL		(SCAN_ERROR_INVALID_FORMAT - 1)
#define SCAN_ERROR_WRONG_PARAM_COUNT	(SCAN_ERROR_VAR_PASSED_BYVAL - 1)
#define SCAN_ERROR_INTERNAL             (SCAN_ERROR_WRONG_PARAM_COUNT - 1)


/*  
 * The following are here solely for the benefit of the scanf type functions
 * e.g. fscanf
 */
PHPAPI int ValidateFormat(char *format, int numVars, int *totalVars);
PHPAPI int php_sscanf_internal(char *string,char *format,int argCount,zval ***args, 
				int varStart,pval **return_value);
inline void scan_set_error_return(int numVars,pval **return_value);


#endif /* SCANF_H */
