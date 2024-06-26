dnl $Id: config.m4,v 1.1.1.2 2001/01/25 04:59:23 wsanchez Exp $
dnl config.m4 for extension ingres_ii

PHP_ARG_WITH(ingres, for Ingres II support,
[  --with-ingres[=DIR]     Include Ingres II support. DIR is the Ingres
                          base directory (default $II_SYSTEM/II/ingres)],
no)

if test "$PHP_INGRES" != "no"; then
  AC_DEFINE(HAVE_II, 1, [Whether you have Ingres II])
  PHP_EXTENSION(ingres_ii, $ext_shared)

  if test "$PHP_INGRES" = "yes"; then
    II_DIR=$II_SYSTEM/ingres
  else
    II_DIR=$PHP_INGRES
  fi

  if test -r $II_DIR/files/iiapi.h; then
    II_INC_DIR=$II_DIR/files
  else
    AC_MSG_ERROR(Cannot find iiapi.h under $II_DIR/files)
  fi

  if test -r $II_DIR/lib/libiiapi.a; then
    II_LIB_DIR=$II_DIR/lib
  else
    AC_MSG_ERROR(Cannot find libiiapi.a under $II_DIR/lib)
  fi

  AC_ADD_LIBRARY_WITH_PATH(iiapi, $II_LIB_DIR, II_SHARED_LIBADD)
  AC_ADD_LIBRARY_WITH_PATH(ingres, $II_LIB_DIR, II_SHARED_LIBADD)

  AC_ADD_INCLUDE($II_INC_DIR)
fi
PHP_SUBST(II_SHARED_LIBADD)
