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
   | Authors: Rasmus Lerdorf <rasmus@lerdorf.on.ca>                       |
   +----------------------------------------------------------------------+
*/

/* $Id: exec.h,v 1.1.1.3 2001/01/25 05:00:04 wsanchez Exp $ */

#ifndef EXEC_H
#define EXEC_H

PHP_FUNCTION(system);
PHP_FUNCTION(exec);
PHP_FUNCTION(escapeshellcmd);
PHP_FUNCTION(escapeshellarg);
PHP_FUNCTION(passthru);
PHP_FUNCTION(shell_exec);

char *php_escape_shell_cmd(char *);
#endif /* EXEC_H */
