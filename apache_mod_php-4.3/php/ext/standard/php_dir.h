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
   | Authors:                                                             |
   | PHP 4.0 patches by Thies C. Arntzen (thies@thieso.net)               |
   +----------------------------------------------------------------------+
 */

/* $Id: php_dir.h,v 1.1.1.3 2001/01/25 05:00:10 wsanchez Exp $ */

#ifndef PHP_DIR_H
#define PHP_DIR_H

/* directory functions */
PHP_MINIT_FUNCTION(dir);
PHP_RINIT_FUNCTION(dir);
PHP_FUNCTION(opendir);
PHP_FUNCTION(closedir);
PHP_FUNCTION(chdir);
PHP_FUNCTION(getcwd);
PHP_FUNCTION(rewinddir);
PHP_NAMED_FUNCTION(php_if_readdir);
PHP_FUNCTION(getdir);

#endif /* PHP_DIR_H */
