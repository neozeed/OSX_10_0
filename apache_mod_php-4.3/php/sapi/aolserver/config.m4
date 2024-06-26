dnl ## $Id: config.m4,v 1.3 2001/01/25 07:05:22 wsanchez Exp $ -*- sh -*-

AC_MSG_CHECKING(for AOLserver support)
AC_ARG_WITH(aolserver,
[  --with-aolserver=DIR    Specify path to the installed AOLserver],[
  PHP_AOLSERVER=$withval
],[
  PHP_AOLSERVER=no
])
AC_MSG_RESULT($PHP_AOLSERVER)

if test "$PHP_AOLSERVER" != "no"; then
  if test -d "$PHP_AOLSERVER/include"; then
    PHP_AOLSERVER_SRC="$PHP_AOLSERVER"
  fi
  if test -z "$PHP_AOLSERVER_SRC" || test ! -d $PHP_AOLSERVER_SRC/include; then
    AC_MSG_ERROR(Please specify the path to the source distribution of AOLserver using --with-aolserver-src=DIR)
  fi
  if test ! -d $PHP_AOLSERVER/bin ; then
    AC_MSG_ERROR(Please specify the path to the root of AOLserver using --with-aolserver=DIR)
  fi
  PHP_BUILD_THREAD_SAFE
  AC_ADD_INCLUDE($PHP_AOLSERVER_SRC/include)
  AC_DEFINE(HAVE_AOLSERVER,1,[Whether you have AOLserver])
  PHP_SAPI=aolserver
  PHP_BUILD_SHARED
  INSTALL_IT="\$(INSTALL) -m 0755 $SAPI_SHARED \$(INSTALL_ROOT)$PHP_AOLSERVER/bin/"
fi

dnl ## Local Variables:
dnl ## tab-width: 4
dnl ## End:
