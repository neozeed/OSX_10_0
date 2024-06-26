dnl $Id: config.m4,v 1.1.1.2 2001/01/25 04:59:14 wsanchez Exp $
dnl config.m4 for extension domxml

AC_DEFUN(PHP_DOM_CHECK_VERSION,[
  old_CPPFLAGS=$CPPFLAGS
  CPPFLAGS="-I$DOMXML_DIR/include"
  AC_MSG_CHECKING(for libxml version)
  AC_EGREP_CPP(yes,[
  #include <libxml/xmlversion.h>
  #if LIBXML_VERSION >= 20207
  yes
  #endif
  ],[
    AC_MSG_RESULT(>= 2.2.7)
  ],[
    AC_MSG_ERROR(libxml version 2.2.7 or greater required.)
  ])
  CPPFLAGS=$old_CPPFLAGS
])

PHP_ARG_WITH(dom, for DOM support,
[  --with-dom[=DIR]        Include DOM support (requires libxml >= 2.2.7).
                          DIR is the libxml install directory,
                          defaults to /usr.])

if test "$PHP_DOM" != "no"; then

  if test -r $PHP_DOM/include/libxml/tree.h; then
    DOMXML_DIR=$PHP_DOM
  else
    AC_MSG_CHECKING(for DOM in default path)
    for i in /usr/local /usr; do
      if test -r $i/include/libxml/tree.h; then
        DOMXML_DIR=$i
        AC_MSG_RESULT(found in $i)
      fi
    done
  fi

  if test -z "$DOMXML_DIR"; then
    AC_MSG_RESULT(not found)
    AC_MSG_ERROR(Please reinstall the libxml >= 2.2.7 distribution)
  fi

  PHP_DOM_CHECK_VERSION

  AC_ADD_INCLUDE($DOMXML_DIR/include)

  PHP_SUBST(DOMXML_SHARED_LIBADD)
  AC_ADD_LIBRARY_WITH_PATH(xml, $DOMXML_DIR/lib, DOMXML_SHARED_LIBADD)

  if test $HAVE_ZLIB; then
    old_withval=$withval
    AC_MSG_CHECKING([for zlib (needed by DOM support)])
    AC_ARG_WITH(zlib-dir,
    [  --with-zlib-dir[=DIR]   zlib dir for libxml or include zlib support],[
     AC_MSG_RESULT( )
     if test -z $withval; then
       withval="/usr/local"
     fi
     LIBS="$LIBS -L$withval/lib -lz"
     AC_CHECK_LIB(z,deflate, ,[AC_MSG_RESULT(no)],)
     AC_ADD_LIBRARY_WITH_PATH(z, $withval/lib)
    ],[
     AC_MSG_RESULT(no)
     AC_MSG_WARN(If configure fails try --with-zlib=<DIR>)
    ])
    withval=$old_withval
  else
    echo "checking for libz needed by libxml ... already zlib support"
    LIBS="$LIBS -L$withval/lib -lz"
  fi

  AC_DEFINE(HAVE_DOMXML,1,[ ])
  AC_ADD_LIBRARY(z)

  PHP_EXTENSION(domxml, $ext_shared)
fi
