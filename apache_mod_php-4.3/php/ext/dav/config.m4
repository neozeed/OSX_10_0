dnl $Id: config.m4,v 1.1.1.1 2000/08/10 02:08:27 wsanchez Exp $
dnl config.m4 for extension dav
dnl don't forget to call PHP_EXTENSION(dav)

AC_MSG_CHECKING(whether to enable DAV support through mod_dav)
AC_ARG_WITH(mod-dav,
[  --with-mod-dav=DIR      Include DAV support through Apache's mod_dav,
                          DIR is mod_dav's installation directory (Apache
                          module version only!)],
[
  if test "$withval" = "yes"; then
    AC_MSG_ERROR(Must give parameter to --with-mod-dav!)
  else
    if test "$withval" != "no"; then
      AC_MSG_RESULT(yes)
      AC_DEFINE(HAVE_MOD_DAV, 1, [Whether you have mod_dav])
      CFLAGS="$CFLAGS -DHAVE_MOD_DAV -I$withval"
      INCLUDES="$INCLUDES -I$withval"
      PHP_EXTENSION(dav)
    else
      AC_MSG_RESULT(no)
      AC_DEFINE(HAVE_MOD_DAV, 0, [Whether you have mod_dav])
    fi
  fi
],[
  AC_MSG_RESULT(no)
  AC_DEFINE(HAVE_MOD_DAV, 0, [Whether you have mod_dav])
])

