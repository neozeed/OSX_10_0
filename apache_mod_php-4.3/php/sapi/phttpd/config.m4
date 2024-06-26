dnl ## $Id: config.m4,v 1.2 2000/08/11 20:59:22 wsanchez Exp $ -*- sh -*-

RESULT=no
AC_MSG_CHECKING(for PHTTPD support)
AC_ARG_WITH(phttpd,
[  --with-phttpd=DIR],
[
	if test ! -d $withval ; then
		AC_MSG_ERROR(You did not specify a directory)
	fi
	PHP_BUILD_THREAD_SAFE
	PHTTPD_DIR=$withval
	AC_ADD_INCLUDE($PHTTPD_DIR/include)
	AC_DEFINE(HAVE_PHTTPD,1,[Whether you have phttpd])
	PHP_SAPI=phttpd
	PHP_BUILD_SHARED
	INSTALL_IT="\$(INSTALL) -m 0755 $SAPI_SHARED \$(INSTALL_ROOT)$PHTTPD_DIR/modules/"
	RESULT=yes
])
AC_MSG_RESULT($RESULT)

dnl ## Local Variables:
dnl ## tab-width: 4
dnl ## End:
