dnl $Id: config.m4,v 1.1.1.1 2000/08/10 02:08:28 wsanchez Exp $

AC_ARG_WITH(filepro,[],[enable_filepro=$withval])

PHP_ARG_ENABLE(filepro,whether to enable the bundled filePro support,
[  --enable-filepro        Enable the bundled read-only filePro support])

if test "$PHP_FILEPRO" = "yes"; then
  AC_DEFINE(HAVE_FILEPRO, 1, [ ])
  PHP_EXTENSION(filepro, $ext_shared)
fi
