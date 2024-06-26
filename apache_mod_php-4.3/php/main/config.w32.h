/* config.w32.h.  Configure file for win32 platforms  */
/* tested only with MS Visual C++ V5 */


/* Define if PHP to setup it's own SIGCHLD handler (not needed on Win32) */
#define PHP_SIGCHILD 0

/* if you have resolv.lib and lib44bsd95.lib you can compile the extra 
   dns functions located in dns.c.  Set this to 1.  add resolv.lib and
   lib33bsd95.lib to the project settings, and add the path to the
   bind include directory to the preprocessor settings.  These libs
   are availabe in the ntbind distribution */
#define HAVE_BINDLIB 1

/* set to enable bcmath */
#define WITH_BCMATH 1

/* set to enable mysql */
#define HAVE_MYSQL 1

/* set to enable FTP support */
#define HAVE_FTP 1


/* set to enable bundled PCRE library */
#define HAVE_BUNDLED_PCRE	1

/* set to enable bundled expat library */
#define HAVE_LIBEXPAT 1
#define HAVE_WDDX 1

/* set to enable the crypt command */
#define HAVE_CRYPT 0

/* set to enable trans sid */
#define TRANS_SID 1

/* should be added to runtime config*/
#define PHP_URL_FOPEN 1

#define STDIN_FILENO 0
#define STDOUT_FILENO 1
#define STDERR_FILENO 2

/* ----------------------------------------------------------------
   The following are defaults for run-time configuration
   ---------------------------------------------------------------*/

#define PHP_SAFE_MODE 0
#define MAGIC_QUOTES 0
/* This is the default configuration file to read */
#define CONFIGURATION_FILE_PATH "php.ini"
#define USE_CONFIG_FILE 1

#define PHP_INCLUDE_PATH	NULL


/* Undefine if you want stricter XML/SGML compliance by default */
/* this disables "<?expression?>" and "<?=expression?>" */
#define DEFAULT_SHORT_OPEN_TAG 1


/* ----------------------------------------------------------------
   The following defines are for those modules which require
   external libraries to compile.  These will be removed from 
   here in a future beta, as these modules will be moved out to dll's 
   ---------------------------------------------------------------*/
#define HAVE_ERRMSG_H 0 /*needed for mysql 3.21.17 and up*/
#undef HAVE_ADABAS
#undef HAVE_SOLID


/* ----------------------------------------------------------------
   The following may or may not be (or need to be) ported to the
   windows environment.
   ---------------------------------------------------------------*/

/* Define if you have the link function.  */
#undef HAVE_LINK

/* Define if you have the lockf function.  */
/* #undef HAVE_LOCKF */

/* Define if you have the lrand48 function.  */
/* #undef HAVE_LRAND48 */

/* Define if you have the srand48 function.  */
/* #undef HAVE_SRAND48 */

/* Define if you have the symlink function.  */
#undef HAVE_SYMLINK

/* Define if you have the usleep function.  */
#undef HAVE_USLEEP


#define HAVE_GETCWD 1
#define HAVE_POSIX_READDIR_R 1

#define NEED_ISBLANK 1
/* ----------------------------------------------------------------
   The following may be changed and played with right now, but
   will move to the "don't touch" list below eventualy.
   ---------------------------------------------------------------*/


/* ----------------------------------------------------------------
   The following should never need to be played with
   Functions defined to 0 or remarked out are either already
   handled by the standard VC libraries, or are no longer needed, or
   simply will/can not be ported.

   DONT TOUCH!!!!!  Unless you realy know what your messing with!
   ---------------------------------------------------------------*/

#define DISCARD_PATH 0
#undef HAVE_SETITIMER
#undef HAVE_IODBC
#define HAVE_UODBC 1
#define HAVE_LIBDL 1
#define HAVE_SENDMAIL 1
#define HAVE_GETTIMEOFDAY 1
#define HAVE_PUTENV 1
#define HAVE_LIMITS_H 1

#define HAVE_TZSET 1
/* Define if you have the flock function.  */
#undef HAVE_FLOCK

/* Define if using alloca.c.  */
/* #undef C_ALLOCA */

/* Define to one of _getb67, GETB67, getb67 for Cray-2 and Cray-YMP systems.
   This function is required for alloca.c support on those systems.  */
/* #undef CRAY_STACKSEG_END */

/* Define to `int' if <sys/types.h> doesn't define.  */
/* #undef gid_t */

/* Define if you have alloca, as a function or macro.  */
#define HAVE_ALLOCA 1

/* Define if you have <alloca.h> and it should be used (not on Ultrix).  */
/* #undef HAVE_ALLOCA_H */

/* Define if you have <sys/time.h> */
#undef HAVE_SYS_TIME_H

/* Define if you have <signal.h> */
#define HAVE_SIGNAL_H 1

/* Define if you don't have vprintf but do have _doprnt.  */
/* #undef HAVE_DOPRNT */

/* Define if your struct stat has st_blksize.  */
#undef HAVE_ST_BLKSIZE

/* Define if your struct stat has st_blocks.  */
#undef HAVE_ST_BLOCKS

/* Define if your struct stat has st_rdev.  */
#define HAVE_ST_RDEV 1

/* Define if utime(file, NULL) sets file's timestamp to the present.  */
#define HAVE_UTIME_NULL 1

/* Define if you have the vprintf function.  */
#define HAVE_VPRINTF 1

/* Define to `unsigned' if <sys/types.h> doesn't define.  */
/* #undef size_t */

/* If using the C implementation of alloca, define if you know the
   direction of stack growth for your system; otherwise it will be
   automatically deduced at run-time.
 STACK_DIRECTION > 0 => grows toward higher addresses
 STACK_DIRECTION < 0 => grows toward lower addresses
 STACK_DIRECTION = 0 => direction of growth unknown
 */
/* #undef STACK_DIRECTION */

/* Define if you have the ANSI C header files.  */
#define STDC_HEADERS 1

/* Define if your <sys/time.h> declares struct tm.  */
/* #undef TM_IN_SYS_TIME */

/* Define to `int' if <sys/types.h> doesn't define.  */
/* #undef uid_t */

/* Define both of these if you want the bundled REGEX library */
#define REGEX 1
#define HSREGEX 1

#define HAVE_PCRE 1

/* Define if you have the gcvt function.  */
#define HAVE_GCVT 1

/* Define if you have the getlogin function.  */
#define HAVE_GETLOGIN 1

/* Define if you have the gettimeofday function.  */
#define HAVE_GETTIMEOFDAY 1

/* Define if you have the memcpy function.  */
#define HAVE_MEMCPY 1

/* Define if you have the strlcat function.  */
/* #undef HAVE_STRLCAT */

/* Define if you have the strlcpy function.  */
/* #undef HAVE_STRLCPY */

/* Define if you have the memmove function.  */
#define HAVE_MEMMOVE 1

/* Define if you have the putenv function.  */
#define HAVE_PUTENV 1

/* Define if you have the regcomp function.  */
#define HAVE_REGCOMP 1

/* Define if you have the setlocale function.  */
#define HAVE_SETLOCALE 1

#define HAVE_LOCALE_H 1

/* Define if you have the setvbuf function.  */
#ifndef HAVE_BINDLIB
#define HAVE_SETVBUF 1
#endif

/* Define if you have the snprintf function.  */
#define HAVE_SNPRINTF 1
#define HAVE_VSNPRINTF 1
/* Define if you have the strcasecmp function.  */
#define HAVE_STRCASECMP 1

/* Define if you have the strdup function.  */
#define HAVE_STRDUP 1

/* Define if you have the strerror function.  */
#define HAVE_STRERROR 1

/* Define if you have the strstr function.  */
#define HAVE_STRSTR 1

/* Define if you have the tempnam function.  */
#define HAVE_TEMPNAM 1

/* Define if you have the utime function.  */
#define HAVE_UTIME 1

/* Define if you have the <crypt.h> header file.  */
/* #undef HAVE_CRYPT_H */

/* Define if you have the <dirent.h> header file.  */
#undef HAVE_DIRENT_H

/* Define if you have the <fcntl.h> header file.  */
#define HAVE_FCNTL_H 1

/* Define if you have the <grp.h> header file.  */
#define HAVE_GRP_H 0

/* Define if you have the <ndir.h> header file.  */
/* #undef HAVE_NDIR_H */

/* Define if you have the <pwd.h> header file.  */
#define HAVE_PWD_H 1

/* Define if you have the <string.h> header file.  */
#define HAVE_STRING_H 1

/* Define if you have the <sys/dir.h> header file.  */
/* #undef HAVE_SYS_DIR_H */

/* Define if you have the <sys/file.h> header file.  */
#undef HAVE_SYS_FILE_H

/* Define if you have the <sys/ndir.h> header file.  */
/* #undef HAVE_SYS_NDIR_H */

/* Define if you have the <sys/socket.h> header file.  */
#undef HAVE_SYS_SOCKET_H

/* Define if you have the <sys/wait.h> header file.  */
#undef HAVE_SYS_WAIT_H

/* Define if you have the <syslog.h> header file.  */
#define HAVE_SYSLOG_H 1

/* Define if you have the <unistd.h> header file.  */
#undef HAVE_UNISTD_H

/* Define if you have the crypt library (-lcrypt).  */
/* #undef HAVE_LIBCRYPT */

/* Define if you have the dl library (-ldl).  */
#define HAVE_LIBDL 1

/* Define if you have the m library (-lm).  */
#define HAVE_LIBM 1

/* Define if you have the nsl library (-lnsl).  */
/* #undef HAVE_LIBNSL */

/* Define if you have the socket library (-lsocket).  */
/* #undef HAVE_LIBSOCKET */

/* Define if you have the cuserid function.  */
#define HAVE_CUSERID 0

/* Define if you have the rint function.  */
#undef HAVE_RINT

#define HAVE_STRFTIME 1

/* Default directory for loading extensions.  */
#define PHP_EXTENSION_DIR NULL

#define SIZEOF_INT 4
