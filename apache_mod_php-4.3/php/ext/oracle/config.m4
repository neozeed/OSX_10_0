dnl $Id: config.m4,v 1.1.1.2 2001/01/25 04:59:46 wsanchez Exp $

AC_DEFUN(AC_ORACLE_VERSION,[
  AC_MSG_CHECKING([Oracle version])
  if test -s "$ORACLE_DIR/orainst/unix.rgs"; then
  	ORACLE_VERSION=`grep '"ocommon"' $ORACLE_DIR/orainst/unix.rgs | sed 's/[ ][ ]*/:/g' | cut -d: -f 6 | cut -c 2-4`
	test -z "$ORACLE_VERSION" && ORACLE_VERSION=7.3
  elif test -f $ORACLE_DIR/lib/libclntsh.s?.8.0; then
    ORACLE_VERSION=8.1
  elif test -f $ORACLE_DIR/lib/libclntsh.s?.1.0; then
    ORACLE_VERSION=8.0
  elif test -f $ORACLE_DIR/lib/libclntsh.a; then # AIX - XXX is this check still right for 8.1?
    ORACLE_VERSION=8.0
  else
  	AC_MSG_ERROR(Oracle needed libraries not found)
  fi
  AC_MSG_RESULT($ORACLE_VERSION)
])

PHP_ARG_WITH(oracle, for Oracle-ORACLE support,
[  --with-oracle[=DIR]     Include Oracle-oci7 support. Default DIR is ORACLE_HOME.])

if test "$PHP_ORACLE" != "no"; then
  AC_MSG_CHECKING([Oracle Install-Dir])
  if test "$PHP_ORACLE" = "yes"; then
  	ORACLE_DIR="$ORACLE_HOME"
  else
  	ORACLE_DIR="$PHP_ORACLE"
  fi
  AC_MSG_RESULT($ORACLE_DIR)

  if test "$PHP_SIGCHILD" != "yes"; then
    echo "+--------------------------------------------------------------------+"
    echo "| Notice:                                                            |"
    echo "| If you encounter <defunc> processes when using a local Oracle-DB   |"
    echo "| please recompile PHP and specify --enable-sigchild when configuring|"
    echo "| (This problem has been reported un Linux using Oracle >= 8.1.5)    |"
    echo "+--------------------------------------------------------------------+"
  fi                                                                                          

  if test -d "$ORACLE_DIR/rdbms/public"; then
  	AC_ADD_INCLUDE($ORACLE_DIR/rdbms/public)
  fi
  if test -d "$ORACLE_DIR/rdbms/demo"; then
  	AC_ADD_INCLUDE($ORACLE_DIR/rdbms/demo)
  fi
  if test -d "$ORACLE_DIR/network/public"; then
  	AC_ADD_INCLUDE($ORACLE_DIR/network/public)
  fi
  if test -d "$ORACLE_DIR/plsql/public"; then
  	AC_ADD_INCLUDE($ORACLE_DIR/plsql/public)
  fi

  if test -f "$ORACLE_DIR/lib/sysliblist"; then
  	PHP_EVAL_LIBLINE(`cat $ORACLE_DIR/lib/sysliblist`, ORACLE_SHARED_LIBADD)
  elif test -f "$ORACLE_DIR/rdbms/lib/sysliblist"; then
  	PHP_EVAL_LIBLINE(`cat $ORACLE_DIR/rdbms/lib/sysliblist`, ORACLE_SHARED_LIBADD)
  fi

  AC_ORACLE_VERSION($ORACLE_DIR)
  case $ORACLE_VERSION in
	7.0|7.1)
	  if test -f $ORACLE_DIR/lib/liboracle.s?; then
	    AC_ADD_LIBRARY_WITH_PATH(oracle, $ORACLE_DIR/lib, ORACLE_SHARED_LIBADD)
	  else
	    AC_ADD_LIBRARY_WITH_PATH(core, $ORACLE_DIR/lib, ORACLE_SHARED_LIBADD)
	    AC_ADD_LIBRARY_WITH_PATH(cv6, $ORACLE_DIR/lib, ORACLE_SHARED_LIBADD)
	    AC_ADD_LIBRARY_WITH_PATH(nlsrtl, $ORACLE_DIR/lib, ORACLE_SHARED_LIBADD)
	    AC_ADD_LIBRARY_WITH_PATH(core, $ORACLE_DIR/lib, ORACLE_SHARED_LIBADD)
	    AC_ADD_LIBRARY_WITH_PATH(cv6, $ORACLE_DIR/lib, ORACLE_SHARED_LIBADD)
	    AC_ADD_LIBRARY_WITH_PATH(nlsrtl, $ORACLE_DIR/lib, ORACLE_SHARED_LIBADD)
	    AC_ADD_LIBRARY_WITH_PATH(sqlnet, $ORACLE_DIR/lib, ORACLE_SHARED_LIBADD)
		AC_ADD_LIBRARY_WITH_PATH(ora, $ORACLE_DIR/lib, ORACLE_SHARED_LIBADD)
		AC_ADD_LIBRARY_WITH_PATH(sqlnet, $ORACLE_DIR/lib, ORACLE_SHARED_LIBADD)
		if test "$ext_shared" = "yes"; then
		  ORACLE_SHARED_LIBADD="$ORACLE_DIR/lib/osntab.o $ORACLE_SHARED_LIBADD";
		else
		  LIBS="$ORACLE_DIR/lib/osntab.o $LIBS";
		fi
		AC_ADD_LIBRARY_WITH_PATH(ocic, $ORACLE_DIR/lib, ORACLE_SHARED_LIBADD)
		if test "`uname -s 2>/dev/null`" = "AIX"; then
		  if test "$ext_shared" = "yes"; then
		  	ORACLE_SHARED_LIBADD="$ORACLE_SHARED_LIBADD -bI:$ORACLE_DIR/lib/mili.exp"
		  else
		  	LIBS="$LIBS -bI:$ORACLE_DIR/lib/mili.exp"
		  fi
		fi
	  fi
	  ;;

	7.2)
	  if test -f $ORACLE_DIR/lib/libclntsh.s?; then
	    AC_ADD_LIBRARY_WITH_PATH(clntsh, $ORACLE_DIR/lib, ORACLE_SHARED_LIBADD)
	  else
	    AC_ADD_LIBRARY_WITH_PATH(core3, $ORACLE_DIR/lib, ORACLE_SHARED_LIBADD)
	    AC_ADD_LIBRARY_WITH_PATH(nlsrtl3, $ORACLE_DIR/lib, ORACLE_SHARED_LIBADD)
	    AC_ADD_LIBRARY_WITH_PATH(c3v6, $ORACLE_DIR/lib, ORACLE_SHARED_LIBADD)
	    AC_ADD_LIBRARY_WITH_PATH(nlsrtl3, $ORACLE_DIR/lib, ORACLE_SHARED_LIBADD)
	    AC_ADD_LIBRARY_WITH_PATH(ora, $ORACLE_DIR/lib, ORACLE_SHARED_LIBADD)
	    AC_ADD_LIBRARY_WITH_PATH(sqlnet, $ORACLE_DIR/lib, ORACLE_SHARED_LIBADD)
	    AC_ADD_LIBRARY_WITH_PATH(ora, $ORACLE_DIR/lib, ORACLE_SHARED_LIBADD)
	    AC_ADD_LIBRARY_WITH_PATH(sqlnet, $ORACLE_DIR/lib, ORACLE_SHARED_LIBADD)
		if test "$ext_shared" = "yes"; then
		  ORACLE_SHARED_LIBADD="$ORACLE_DIR/lib/osntab.o $ORACLE_SHARED_LIBADD";
		else
		  LIBS="$ORACLE_DIR/lib/osntab.o $LIBS";
		fi
	    AC_ADD_LIBRARY_WITH_PATH(ocic, $ORACLE_DIR/lib, ORACLE_SHARED_LIBADD)
	  fi
	  ;;

	7.3)
	  if test -f $ORACLE_DIR/lib/libclntsh.s?; then
	    AC_ADD_LIBRARY_WITH_PATH(clntsh, $ORACLE_DIR/lib, ORACLE_SHARED_LIBADD)
	  else
	    AC_ADD_LIBRARY_WITH_PATH(core3, $ORACLE_DIR/lib, ORACLE_SHARED_LIBADD)
	    AC_ADD_LIBRARY_WITH_PATH(nlsrtl3, $ORACLE_DIR/lib, ORACLE_SHARED_LIBADD)
	    AC_ADD_LIBRARY_WITH_PATH(core3, $ORACLE_DIR/lib, ORACLE_SHARED_LIBADD)
	    AC_ADD_LIBRARY_WITH_PATH(c3v6, $ORACLE_DIR/lib, ORACLE_SHARED_LIBADD)
	    AC_ADD_LIBRARY_WITH_PATH(nlsrtl3, $ORACLE_DIR/lib, ORACLE_SHARED_LIBADD)
	    AC_ADD_LIBRARY_WITH_PATH(epc, $ORACLE_DIR/lib, ORACLE_SHARED_LIBADD)
	    AC_ADD_LIBRARY_WITH_PATH(generic, $ORACLE_DIR/lib, ORACLE_SHARED_LIBADD)
	    AC_ADD_LIBRARY_WITH_PATH(common, $ORACLE_DIR/lib, ORACLE_SHARED_LIBADD)
	    AC_ADD_LIBRARY_WITH_PATH(client, $ORACLE_DIR/lib, ORACLE_SHARED_LIBADD)
	    AC_ADD_LIBRARY_WITH_PATH(sqlnet, $ORACLE_DIR/lib, ORACLE_SHARED_LIBADD)
	    AC_ADD_LIBRARY_WITH_PATH(ncr, $ORACLE_DIR/lib, ORACLE_SHARED_LIBADD)
	    AC_ADD_LIBRARY_WITH_PATH(sqlnet, $ORACLE_DIR/lib, ORACLE_SHARED_LIBADD)
	    AC_ADD_LIBRARY_WITH_PATH(generic, $ORACLE_DIR/lib, ORACLE_SHARED_LIBADD)
	    AC_ADD_LIBRARY_WITH_PATH(common, $ORACLE_DIR/lib, ORACLE_SHARED_LIBADD)
	    AC_ADD_LIBRARY_WITH_PATH(client, $ORACLE_DIR/lib, ORACLE_SHARED_LIBADD)
	    AC_ADD_LIBRARY_WITH_PATH(sqlnet, $ORACLE_DIR/lib, ORACLE_SHARED_LIBADD)
	    AC_ADD_LIBRARY_WITH_PATH(ncr, $ORACLE_DIR/lib, ORACLE_SHARED_LIBADD)
	    AC_ADD_LIBRARY_WITH_PATH(sqlnet, $ORACLE_DIR/lib, ORACLE_SHARED_LIBADD)
	    AC_ADD_LIBRARY_WITH_PATH(client, $ORACLE_DIR/lib, ORACLE_SHARED_LIBADD)
	  fi
	  ;;
	  
	8.0)
  	  AC_ADD_LIBRARY_WITH_PATH(nlsrtl3, "", ORACLE_SHARED_LIBADD)
  	  AC_ADD_LIBRARY_WITH_PATH(core4, "", ORACLE_SHARED_LIBADD)
  	  AC_ADD_LIBRARY_WITH_PATH(psa, "", ORACLE_SHARED_LIBADD)
  	  AC_ADD_LIBRARY_WITH_PATH(clntsh, $ORACLE_DIR/lib, ORACLE_SHARED_LIBADD)
	  ;;

	8.1)
  	  AC_ADD_LIBRARY(clntsh, 1, ORACLE_SHARED_LIBADD)
  	  AC_ADD_LIBPATH($ORACLE_DIR/lib, ORACLE_SHARED_LIBADD)
	  ;;
	*)
      AC_MSG_ERROR(Unsupported Oracle version!)
	  ;;
  esac

  PHP_EXTENSION(oracle, $ext_shared)
  AC_DEFINE(HAVE_ORACLE,1,[ ])

  PHP_SUBST(ORACLE_SHARED_LIBADD)
  PHP_SUBST(ORACLE_DIR)
  PHP_SUBST(ORACLE_VERSION)
fi
