dnl $Id: config.m4,v 1.1.1.1 2001/01/25 04:59:47 wsanchez Exp $
dnl config.m4 for extension ovrimos
dnl don't forget to call PHP_EXTENSION(ovrimos)

PHP_ARG_WITH(ovrimos, for Ovrimos SQL Server support,
[  --with-ovrimos[=DIR]    Include Ovrimos SQL Server support. DIR is the
                          Ovrimos' libsqlcli install directory.])

if test "$PHP_OVRIMOS" != "no"; then
  for i in /usr/local /usr $PHP_OVRIMOS; do
    if test -f $i/include/sqlcli.h; then
      OVRIMOS_DIR=$i
    fi
  done

  if test -z "$OVRIMOS_DIR"; then
    AC_MSG_ERROR(Please reinstall Ovrimos' libsqlcli - I cannot find sqlcli.h)
  fi

  AC_ADD_INCLUDE($OVRIMOS_DIR/include)
  PHP_SUBST(OVRIMOS_SHARED_LIBADD)
  LDFLAGS="$LDFLAGS $ld_runpath_switch$OVRIMOS_DIR/lib -L$OVRIMOS_DIR/lib"
  AC_CHECK_LIB(sqlcli, main)
  AC_ADD_LIBRARY_WITH_PATH(sqlcli, $OVRIMOS_DIR/lib, OVRIMOS_SHARED_LIBADD)
  AC_DEFINE(HAVE_LIBSQLCLI,1,[ ])

  PHP_EXTENSION(ovrimos, $ext_shared)
fi
