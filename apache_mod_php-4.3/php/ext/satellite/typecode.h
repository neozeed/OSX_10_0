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
   | Author: David Eriksson <david@2good.com>                            |
   +----------------------------------------------------------------------+
 */

/*
 * $Id: typecode.h,v 1.1.1.1 2001/01/25 04:59:56 wsanchez Exp $
 * vim: syntax=c tabstop=2 shiftwidth=2
 */

#ifndef __typecode_h__
#define __typecode_h__

#include <libIDL/IDL.h>
#include <orb/orbit.h>
CORBA_TypeCode orbit_idl_tree_type_to_typecode(IDL_tree type);

void satellite_release_typecode(CORBA_TypeCode typecode);

#endif	/* __typecode_h__ */

