dnl $Id: config.m4,v 1.1.1.1 2000/08/10 02:08:30 wsanchez Exp $

dnl
dnl Test mSQL version by checking if msql.h has "IDX_TYPE" defined.
dnl
AC_DEFUN(PHP_MSQL_VERSION,[
  AC_MSG_CHECKING([mSQL version])
  ac_php_oldcflags=$CFLAGS
  CFLAGS="$INCLUDES $CFLAGS"
  AC_TRY_COMPILE([#include <sys/types.h>
#include "msql.h"],[int i = IDX_TYPE],[
    AC_DEFINE(MSQL1,0,[ ])
    MSQL_VERSION="2.0 or newer"
  ],[
    AC_DEFINE(MSQL1,1,[ ])
    MSQL_VERSION="1.0"
  ])
  CFLAGS=$ac_php_oldcflags
  AC_MSG_RESULT($MSQL_VERSION)
])

PHP_ARG_WITH(msql,for mSQL support,
[  --with-msql[=DIR]       Include mSQL support.  DIR is the mSQL base
                          install directory, defaults to /usr/local/Hughes.])

  if test "$PHP_MSQL" != "no"; then
    if test "$PHP_MSQL" = "yes"; then
      MSQL_INCDIR=/usr/local/Hughes/include
      MSQL_LIBDIR=/usr/local/Hughes/lib
    else
      MSQL_INCDIR=$PHP_MSQL/include
      MSQL_LIBDIR=$PHP_MSQL/lib
    fi
    AC_ADD_LIBRARY_WITH_PATH(msql, $MSQL_LIBDIR, MSQL_SHARED_LIBADD)
    AC_ADD_INCLUDE($MSQL_INCDIR)
    AC_DEFINE(HAVE_MSQL,1,[ ])
    PHP_SUBST(MSQL_SHARED_LIBADD)
    PHP_EXTENSION(msql,$ext_shared)
    PHP_MSQL_VERSION
  fi
