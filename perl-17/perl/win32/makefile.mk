#
# Makefile to build perl on Windows NT using DMAKE.
# Supported compilers:
#	Visual C++ 2.0 thro 6.0
#	Borland C++ 5.02
#	Mingw32 with gcc-2.95.2 or better  **experimental**
#
# This is set up to build a perl.exe that runs off a shared library
# (perl56.dll).  Also makes individual DLLs for the XS extensions.
#

##
## Make sure you read README.win32 *before* you mess with anything here!
##

##
## Build configuration.  Edit the values below to suit your needs.
##

#
# Set these to wherever you want "dmake install" to put your
# newly built perl.
#
INST_DRV	*= c:
INST_TOP	*= $(INST_DRV)\perl

#
# Comment this out if you DON'T want your perl installation to be versioned.
# This means that the new installation will overwrite any files from the
# old installation at the same INST_TOP location.  Leaving it enabled is
# the safest route, as perl adds the extra version directory to all the
# locations it installs files to.  If you disable it, an alternative
# versioned installation can be obtained by setting INST_TOP above to a
# path that includes an arbitrary version string.
#
INST_VER	*= \5.6.0

#
# Comment this out if you DON'T want your perl installation to have
# architecture specific components.  This means that architecture-
# specific files will be installed along with the architecture-neutral
# files.  Leaving it enabled is safer and more flexible, in case you
# want to build multiple flavors of perl and install them together in
# the same location.  Commenting it out gives you a simpler
# installation that is easier to understand for beginners.
#
INST_ARCH	*= \$(ARCHNAME)

#
# uncomment to enable multiple interpreters.  This is need for fork()
# emulation.
#
#USE_MULTI	*= define

#
# Beginnings of interpreter cloning/threads; still very incomplete.
# This should be enabled to get the fork() emulation.  This needs
# USE_MULTI as well.
#
#USE_ITHREADS	*= define

#
# uncomment to enable the implicit "host" layer for all system calls
# made by perl.  This needs USE_MULTI above.  This is also needed to
# get fork().
#
#USE_IMP_SYS	*= define

#
# WARNING! This option is deprecated and will eventually go away (enable
# USE_ITHREADS instead).
#
# uncomment to enable threads-capabilities.  This is incompatible with
# USE_ITHREADS, and is only here for people who may have come to rely
# on the experimental Thread support that was in 5.005.
#
#USE_5005THREADS	*= define

#
# WARNING! This option is deprecated and will eventually go away (enable
# USE_MULTI instead).
#
# uncomment next line if you want to use the PERL_OBJECT build option.
# DO NOT ENABLE unless you have legacy code that relies on the C++
# CPerlObj class that was available in 5.005.  This cannot be enabled
# if you ask for USE_5005THREADS above.
#
#USE_OBJECT	*= define

#
# uncomment exactly one of the following
# 
# Visual C++ 2.x
#CCTYPE		*= MSVC20
# Visual C++ > 2.x and < 6.x
#CCTYPE		*= MSVC
# Visual C++ >= 6.x
#CCTYPE		*= MSVC60
# Borland 5.02 or later
#CCTYPE		*= BORLAND
# mingw32+gcc-2.95.2 or better
CCTYPE		*= GCC

#
# uncomment this if you are compiling under Windows 95/98 and command.com
# (not needed if you're running under 4DOS/NT 6.01 or later)
#IS_WIN95	*= define

#
# uncomment next line if you want debug version of perl (big,slow)
# If not enabled, we automatically try to use maximum optimization
# with all compilers that are known to have a working optimizer.
#
#CFG		*= Debug

#
# uncomment to enable use of PerlCRT.DLL when using the Visual C compiler.
# It has patches that fix known bugs in older versions of MSVCRT.DLL.
# This currently requires VC 5.0 with Service Pack 3 or later.
# Get it from CPAN at http://www.perl.com/CPAN/authors/id/D/DO/DOUGL/
# and follow the directions in the package to install.
#
# Not recommended if you have VC 6.x and you're not running Windows 9x.
#
#USE_PERLCRT	*= define

#
# uncomment to enable linking with setargv.obj under the Visual C
# compiler. Setting this options enables perl to expand wildcards in
# arguments, but it may be harder to use alternate methods like
# File::DosGlob that are more powerful.  This option is supported only with
# Visual C.
#
#USE_SETARGV	*= define

#
# if you have the source for des_fcrypt(), uncomment this and make sure the
# file exists (see README.win32).  File should be located in the same
# directory as this file.
#
#CRYPT_SRC	*= fcrypt.c

#
# if you didn't set CRYPT_SRC and if you have des_fcrypt() available in a
# library, uncomment this, and make sure the library exists (see README.win32)
# Specify the full pathname of the library.
#
#CRYPT_LIB	*= fcrypt.lib

#
# set this if you wish to use perl's malloc
# WARNING: Turning this on/off WILL break binary compatibility with extensions
# you may have compiled with/without it.  Be prepared to recompile all
# extensions if you change the default.  Currently, this cannot be enabled
# if you ask for USE_IMP_SYS above.
#
#PERL_MALLOC	*= define

#
# set the install locations of the compiler include/libraries
# Running VCVARS32.BAT is *required* when using Visual C.
# Some versions of Visual C don't define MSVCDIR in the environment,
# so you may have to set CCHOME explicitly (spaces in the path name should
# not be quoted)
#
#CCHOME		*= c:\bc5
#CCHOME		*= $(MSVCDIR)
CCHOME		*= c:\gcc-2.95.2-msvcrt
CCINCDIR	*= $(CCHOME)\include
CCLIBDIR	*= $(CCHOME)\lib

#
# Additional compiler flags can be specified here.
#

#
# This should normally be disabled.  Adding -DPERL_POLLUTE enables support
# for old symbols by default, at the expense of extreme pollution.  You most
# probably just want to build modules that won't compile with
#         perl Makefile.PL POLLUTE=1
# instead of enabling this.  Please report such modules to the respective
# authors.
#
#BUILDOPT	+= -DPERL_POLLUTE

#
# This should normally be disabled.  Enabling it will disable the File::Glob
# implementation of CORE::glob.
#
#BUILDOPT	+= -DPERL_EXTERNAL_GLOB

#
# This should normally be disabled.  Enabling it causes perl to read scripts
# in text mode (which is the 5.005 behavior) and will break ByteLoader.
#BUILDOPT	+= -DPERL_TEXTMODE_SCRIPTS

#
# specify semicolon-separated list of extra directories that modules will
# look for libraries (spaces in path names need not be quoted)
#
EXTRALIBDIRS	*=

#
# set this to point to cmd.exe (only needed if you use some
# alternate shell that doesn't grok cmd.exe style commands)
#
#SHELL		*= g:\winnt\system32\cmd.exe

#
# set this to your email address (perl will guess a value from
# from your loginname and your hostname, which may not be right)
#
#EMAIL		*=

##
## Build configuration ends.
##

##################### CHANGE THESE ONLY IF YOU MUST #####################

.IF "$(CRYPT_SRC)$(CRYPT_LIB)" == ""
D_CRYPT		= undef
.ELSE
D_CRYPT		= define
CRYPT_FLAG	= -DHAVE_DES_FCRYPT
.ENDIF

.IF "$(USE_OBJECT)" == "define"
PERL_MALLOC	!= undef
USE_5005THREADS	!= undef
USE_MULTI	!= undef
USE_IMP_SYS	!= define
.ENDIF

PERL_MALLOC	*= undef

USE_5005THREADS	*= undef

.IF "$(USE_5005THREADS)" == "define"
USE_ITHREADS	!= undef
.ENDIF

.IF "$(USE_IMP_SYS)" == "define"
PERL_MALLOC	!= undef
.ENDIF

USE_MULTI	*= undef
USE_OBJECT	*= undef
USE_ITHREADS	*= undef
USE_IMP_SYS	*= undef
USE_PERLCRT	*= undef

.IF "$(USE_IMP_SYS)$(USE_MULTI)$(USE_5005THREADS)$(USE_OBJECT)" == "defineundefundefundef"
USE_MULTI	!= define
.ENDIF

.IF "$(USE_ITHREADS)$(USE_MULTI)$(USE_OBJECT)" == "defineundefundef"
USE_MULTI	!= define
USE_5005THREADS	!= undef
.ENDIF

.IF "$(USE_MULTI)$(USE_5005THREADS)$(USE_OBJECT)" != "undefundefundef"
BUILDOPT	+= -DPERL_IMPLICIT_CONTEXT
.ENDIF

.IF "$(USE_IMP_SYS)" != "undef"
BUILDOPT	+= -DPERL_IMPLICIT_SYS
.ENDIF

.IMPORT .IGNORE : PROCESSOR_ARCHITECTURE

PROCESSOR_ARCHITECTURE *= x86

.IF "$(USE_OBJECT)" == "define"
ARCHNAME	= MSWin32-$(PROCESSOR_ARCHITECTURE)-object
.ELIF "$(USE_5005THREADS)" == "define"
ARCHNAME	= MSWin32-$(PROCESSOR_ARCHITECTURE)-thread
.ELIF "$(USE_MULTI)" == "define"
ARCHNAME	= MSWin32-$(PROCESSOR_ARCHITECTURE)-multi
.ELSE
ARCHNAME	= MSWin32-$(PROCESSOR_ARCHITECTURE)
.ENDIF

.IF "$(USE_ITHREADS)" == "define"
ARCHNAME	!:= $(ARCHNAME)-thread
.ENDIF

# Visual Studio 98 specific
.IF "$(CCTYPE)" == "MSVC60"

# VC 6.0 can load the socket dll on demand.  Makes the test suite
# run in about 10% less time.
DELAYLOAD	*= -DELAYLOAD:wsock32.dll -DELAYLOAD:shell32.dll delayimp.lib 

# VC 6.0 seems capable of compiling perl correctly with optimizations
# enabled.  Anything earlier fails tests.
CFG		*= Optimize
.ENDIF

ARCHDIR		= ..\lib\$(ARCHNAME)
COREDIR		= ..\lib\CORE
AUTODIR		= ..\lib\auto
LIBDIR		= ..\lib
EXTDIR		= ..\ext
PODDIR		= ..\pod
EXTUTILSDIR	= $(LIBDIR)\ExtUtils

#
INST_SCRIPT	= $(INST_TOP)$(INST_VER)\bin
INST_BIN	= $(INST_SCRIPT)$(INST_ARCH)
INST_LIB	= $(INST_TOP)$(INST_VER)\lib
INST_ARCHLIB	= $(INST_LIB)$(INST_ARCH)
INST_COREDIR	= $(INST_ARCHLIB)\CORE
INST_POD	= $(INST_LIB)\pod
INST_HTML	= $(INST_POD)\html

#
# Programs to compile, build .lib files and link
#

.USESHELL :

.IF "$(CCTYPE)" == "BORLAND"

CC		= bcc32
LINK32		= tlink32
LIB32		= tlib /P128
IMPLIB		= implib -c
RSC		= rc

#
# Options
#
INCLUDES	= -I$(COREDIR) -I.\include -I. -I.. -I"$(CCINCDIR)"
#PCHFLAGS	= -H -Hc -H=c:\temp\bcmoduls.pch 
DEFINES		= -DWIN32 $(CRYPT_FLAG)
LOCDEFS		= -DPERLDLL -DPERL_CORE
SUBSYS		= console
CXX_FLAG	= -P

LIBC		= cw32mti.lib
LIBFILES	= $(CRYPT_LIB) import32.lib $(LIBC) odbc32.lib odbccp32.lib

.IF  "$(CFG)" == "Debug"
OPTIMIZE	= -v -D_RTLDLL -DDEBUGGING
LINK_DBG	= -v
.ELSE
OPTIMIZE	= -O2 -D_RTLDLL
LINK_DBG	= 
.ENDIF

CFLAGS		= -w -g0 -tWM -tWD $(INCLUDES) $(DEFINES) $(LOCDEFS) \
		$(PCHFLAGS) $(OPTIMIZE)
LINK_FLAGS	= $(LINK_DBG) -L"$(INST_COREDIR)" -L"$(CCLIBDIR)"
OBJOUT_FLAG	= -o
EXEOUT_FLAG	= -e
LIBOUT_FLAG	= 

.ELIF "$(CCTYPE)" == "GCC"

CC		= gcc
LINK32		= gcc
LIB32		= ar rc
IMPLIB		= dlltool
RSC		= rc

o = .o
a = .a

#
# Options
#

INCLUDES	= -I$(COREDIR) -I.\include -I. -I..
DEFINES		= -DWIN32 $(CRYPT_FLAG)
LOCDEFS		= -DPERLDLL -DPERL_CORE
SUBSYS		= console
CXX_FLAG	= -xc++

LIBC		= -lmsvcrt

# same libs as MSVC
LIBFILES	= $(CRYPT_LIB) $(LIBC) \
		  -lmoldname -lkernel32 -luser32 -lgdi32 \
		  -lwinspool -lcomdlg32 -ladvapi32 -lshell32 -lole32 \
		  -loleaut32 -lnetapi32 -luuid -lwsock32 -lmpr \
		  -lwinmm -lversion -lodbc32

.IF  "$(CFG)" == "Debug"
OPTIMIZE	= -g -DDEBUGGING
LINK_DBG	= -g
.ELSE
OPTIMIZE	= -g -O2
LINK_DBG	= 
.ENDIF

CFLAGS		= $(INCLUDES) $(DEFINES) $(LOCDEFS) $(OPTIMIZE)
LINK_FLAGS	= $(LINK_DBG) -L"$(INST_COREDIR)" -L"$(CCLIBDIR)"
OBJOUT_FLAG	= -o
EXEOUT_FLAG	= -o
LIBOUT_FLAG	= 

# NOTE: we assume that GCC uses MSVCRT.DLL
BUILDOPT	+= -fno-strict-aliasing -DPERL_MSVCRT_READFIX

.ELSE

CC		= cl
LINK32		= link
LIB32		= $(LINK32) -lib
RSC		= rc

#
# Options
#

INCLUDES	= -I$(COREDIR) -I.\include -I. -I..
#PCHFLAGS	= -Fpc:\temp\vcmoduls.pch -YX 
DEFINES		= -DWIN32 -D_CONSOLE -DNO_STRICT $(CRYPT_FLAG)
LOCDEFS		= -DPERLDLL -DPERL_CORE
SUBSYS		= console
CXX_FLAG	= -TP -GX

.IF "$(USE_PERLCRT)" != "define"
LIBC	= msvcrt.lib
.ELSE
LIBC	= PerlCRT.lib
.ENDIF

PERLEXE_RES	=
PERLDLL_RES	=

.IF  "$(CFG)" == "Debug"
.IF "$(CCTYPE)" == "MSVC20"
OPTIMIZE	= -Od -MD -Z7 -DDEBUGGING
.ELSE
OPTIMIZE	= -Od -MD -Zi -DDEBUGGING
.ENDIF
LINK_DBG	= -debug -pdb:none
.ELSE
.IF "$(CFG)" == "Optimize"
# -O1 yields smaller code, which turns out to be faster than -O2
#OPTIMIZE	= -O2 -MD -DNDEBUG
OPTIMIZE	= -O1 -MD -DNDEBUG
.ELSE
OPTIMIZE	= -Od -MD -DNDEBUG
.ENDIF
LINK_DBG	= -release
.ENDIF

LIBBASEFILES	= $(CRYPT_LIB) \
		oldnames.lib kernel32.lib user32.lib gdi32.lib winspool.lib \
		comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib \
		netapi32.lib uuid.lib wsock32.lib mpr.lib winmm.lib \
		version.lib odbc32.lib odbccp32.lib

# we add LIBC here, since we may be using PerlCRT.dll
LIBFILES	= $(LIBBASEFILES) $(LIBC)

CFLAGS		= -nologo -Gf -W3 $(INCLUDES) $(DEFINES) $(LOCDEFS) \
		$(PCHFLAGS) $(OPTIMIZE)
LINK_FLAGS	= -nologo -nodefaultlib $(LINK_DBG) \
		-libpath:"$(INST_COREDIR)" \
		-machine:$(PROCESSOR_ARCHITECTURE)
OBJOUT_FLAG	= -Fo
EXEOUT_FLAG	= -Fe
LIBOUT_FLAG	= /out:

.IF "$(USE_PERLCRT)" != "define"
BUILDOPT	+= -DPERL_MSVCRT_READFIX
.ENDIF

.ENDIF

.IF "$(USE_OBJECT)" == "define"
OPTIMIZE	+= $(CXX_FLAG)
BUILDOPT	+= -DPERL_OBJECT
.ENDIF

CFLAGS_O	= $(CFLAGS) $(BUILDOPT)

# used to allow local linking flags that are not propogated into Config.pm,
# currently unused
#   -- BKS, 12-12-1999
PRIV_LINK_FLAGS	*=
BLINK_FLAGS	= $(PRIV_LINK_FLAGS) $(LINK_FLAGS)

#################### do not edit below this line #######################
############# NO USER-SERVICEABLE PARTS BEYOND THIS POINT ##############

o *= .obj
a *= .lib

LKPRE		= INPUT (
LKPOST		= )

#
# Rules
# 

.SUFFIXES : .c $(o) .dll $(a) .exe .rc .res

.c$(o):
	$(CC) -c $(null,$(<:d) $(NULL) -I$(<:d)) $(CFLAGS_O) $(OBJOUT_FLAG)$@ $<

.y.c:
	$(NOOP)

$(o).dll:
.IF "$(CCTYPE)" == "BORLAND"
	$(LINK32) -Tpd -ap $(BLINK_FLAGS) c0d32$(o) $<,$@,,$(LIBFILES),$(*B).def
	$(IMPLIB) $(*B).lib $@
.ELIF "$(CCTYPE)" == "GCC"
	$(LINK32) -o $@ $(BLINK_FLAGS) $< $(LIBFILES)
	$(IMPLIB) --input-def $(*B).def --output-lib $(*B).a $@
.ELSE
	$(LINK32) -dll -subsystem:windows -implib:$(*B).lib -def:$(*B).def \
	    -out:$@ $(BLINK_FLAGS) $(LIBFILES) $< $(LIBPERL)  
.ENDIF

.rc.res:
	$(RSC) -i.. $<

#
# various targets
MINIPERL	= ..\miniperl.exe
MINIDIR		= .\mini
PERLEXE		= ..\perl.exe
WPERLEXE	= ..\wperl.exe
GLOBEXE		= ..\perlglob.exe
CONFIGPM	= ..\lib\Config.pm
MINIMOD		= ..\lib\ExtUtils\Miniperl.pm
X2P		= ..\x2p\a2p.exe

PL2BAT		= bin\pl2bat.pl
GLOBBAT		= bin\perlglob.bat

UTILS		=			\
		..\utils\h2ph		\
		..\utils\splain		\
		..\utils\dprofpp	\
		..\utils\perlbug	\
		..\utils\pl2pm 		\
		..\utils\c2ph		\
		..\utils\h2xs		\
		..\utils\perldoc	\
		..\utils\perlcc		\
		..\pod\checkpods	\
		..\pod\pod2html		\
		..\pod\pod2latex	\
		..\pod\pod2man		\
		..\pod\pod2text		\
		..\pod\pod2usage	\
		..\pod\podchecker	\
		..\pod\podselect	\
		..\x2p\find2perl	\
		..\x2p\s2p		\
		bin\exetype.pl		\
		bin\runperl.pl		\
		bin\pl2bat.pl		\
		bin\perlglob.pl		\
		bin\search.pl

.IF "$(CCTYPE)" == "BORLAND"

CFGSH_TMPL	= config.bc
CFGH_TMPL	= config_H.bc

.ELIF "$(CCTYPE)" == "GCC"

CFGSH_TMPL	= config.gc
CFGH_TMPL	= config_H.gc
PERLIMPLIB	= ..\libperl56$(a)

.ELSE

CFGSH_TMPL	= config.vc
CFGH_TMPL	= config_H.vc

.ENDIF

PERLIMPLIB	*= ..\perl56$(a)
PERLDLL		= ..\perl56.dll

XCOPY		= xcopy /f /r /i /d
RCOPY		= xcopy /f /r /i /e /d
NOOP		= @echo

#
# filenames given to xsubpp must have forward slashes (since it puts
# full pathnames in #line strings)
XSUBPP		= ..\$(MINIPERL) -I..\..\lib ..\$(EXTUTILSDIR)\xsubpp \
		-C++ -prototypes

MICROCORE_SRC	=		\
		..\av.c		\
		..\deb.c	\
		..\doio.c	\
		..\doop.c	\
		..\dump.c	\
		..\globals.c	\
		..\gv.c		\
		..\hv.c		\
		..\mg.c		\
		..\op.c		\
		..\perl.c	\
		..\perlapi.c	\
		..\perly.c	\
		..\pp.c		\
		..\pp_ctl.c	\
		..\pp_hot.c	\
		..\pp_sys.c	\
		..\regcomp.c	\
		..\regexec.c	\
		..\run.c	\
		..\scope.c	\
		..\sv.c		\
		..\taint.c	\
		..\toke.c	\
		..\universal.c	\
		..\utf8.c	\
		..\util.c	\
		..\xsutils.c

EXTRACORE_SRC	+= perllib.c

.IF "$(PERL_MALLOC)" == "define"
EXTRACORE_SRC	+= ..\malloc.c
.ENDIF

.IF "$(USE_OBJECT)" != "define"
EXTRACORE_SRC	+= ..\perlio.c
.ENDIF

WIN32_SRC	=		\
		.\win32.c	\
		.\win32sck.c	\
		.\win32thread.c 

.IF "$(CRYPT_SRC)" != ""
WIN32_SRC	+= .\$(CRYPT_SRC)
.ENDIF

DLL_SRC		= $(DYNALOADER).c

X2P_SRC		=		\
		..\x2p\a2p.c	\
		..\x2p\hash.c	\
		..\x2p\str.c	\
		..\x2p\util.c	\
		..\x2p\walk.c

CORE_NOCFG_H	=		\
		..\av.h		\
		..\cop.h	\
		..\cv.h		\
		..\dosish.h	\
		..\embed.h	\
		..\form.h	\
		..\gv.h		\
		..\handy.h	\
		..\hv.h		\
		..\iperlsys.h	\
		..\mg.h		\
		..\nostdio.h	\
		..\op.h		\
		..\opcode.h	\
		..\perl.h	\
		..\perlapi.h	\
		..\perlsdio.h	\
		..\perlsfio.h	\
		..\perly.h	\
		..\pp.h		\
		..\proto.h	\
		..\regexp.h	\
		..\scope.h	\
		..\sv.h		\
		..\thread.h	\
		..\unixish.h	\
		..\utf8.h	\
		..\util.h	\
		..\warnings.h	\
		..\XSUB.h	\
		..\EXTERN.h	\
		..\perlvars.h	\
		..\intrpvar.h	\
		..\thrdvar.h	\
		.\include\dirent.h	\
		.\include\netdb.h	\
		.\include\sys\socket.h	\
		.\win32.h

CORE_H		= $(CORE_NOCFG_H) .\config.h

MICROCORE_OBJ	= $(MICROCORE_SRC:db:+$(o))
CORE_OBJ	= $(MICROCORE_OBJ) $(EXTRACORE_SRC:db:+$(o))
WIN32_OBJ	= $(WIN32_SRC:db:+$(o))
MINICORE_OBJ	= $(MINIDIR)\{$(MICROCORE_OBJ:f) miniperlmain$(o) perlio$(o)}
MINIWIN32_OBJ	= $(MINIDIR)\{$(WIN32_OBJ:f)}
MINI_OBJ	= $(MINICORE_OBJ) $(MINIWIN32_OBJ)
DLL_OBJ		= $(DLL_SRC:db:+$(o))
X2P_OBJ		= $(X2P_SRC:db:+$(o))

PERLDLL_OBJ	= $(CORE_OBJ)
PERLEXE_OBJ	= perlmain$(o)

PERLDLL_OBJ	+= $(WIN32_OBJ) $(DLL_OBJ)

.IF "$(USE_SETARGV)" != ""
SETARGV_OBJ	= setargv$(o)
.ENDIF

DYNAMIC_EXT	= Socket IO Fcntl Opcode SDBM_File POSIX attrs Thread B re \
		Data/Dumper Devel/Peek ByteLoader Devel/DProf File/Glob \
		Sys/Hostname
STATIC_EXT	= DynaLoader
NONXS_EXT	= Errno

DYNALOADER	= $(EXTDIR)\DynaLoader\DynaLoader
SOCKET		= $(EXTDIR)\Socket\Socket
FCNTL		= $(EXTDIR)\Fcntl\Fcntl
OPCODE		= $(EXTDIR)\Opcode\Opcode
SDBM_FILE	= $(EXTDIR)\SDBM_File\SDBM_File
IO		= $(EXTDIR)\IO\IO
POSIX		= $(EXTDIR)\POSIX\POSIX
ATTRS		= $(EXTDIR)\attrs\attrs
THREAD		= $(EXTDIR)\Thread\Thread
B		= $(EXTDIR)\B\B
RE		= $(EXTDIR)\re\re
DUMPER		= $(EXTDIR)\Data\Dumper\Dumper
ERRNO		= $(EXTDIR)\Errno\Errno
PEEK		= $(EXTDIR)\Devel\Peek\Peek
BYTELOADER	= $(EXTDIR)\ByteLoader\ByteLoader
DPROF		= $(EXTDIR)\Devel\DProf\DProf
GLOB		= $(EXTDIR)\File\Glob\Glob
HOSTNAME	= $(EXTDIR)\Sys\Hostname\Hostname

SOCKET_DLL	= $(AUTODIR)\Socket\Socket.dll
FCNTL_DLL	= $(AUTODIR)\Fcntl\Fcntl.dll
OPCODE_DLL	= $(AUTODIR)\Opcode\Opcode.dll
SDBM_FILE_DLL	= $(AUTODIR)\SDBM_File\SDBM_File.dll
IO_DLL		= $(AUTODIR)\IO\IO.dll
POSIX_DLL	= $(AUTODIR)\POSIX\POSIX.dll
ATTRS_DLL	= $(AUTODIR)\attrs\attrs.dll
THREAD_DLL	= $(AUTODIR)\Thread\Thread.dll
B_DLL		= $(AUTODIR)\B\B.dll
DUMPER_DLL	= $(AUTODIR)\Data\Dumper\Dumper.dll
PEEK_DLL	= $(AUTODIR)\Devel\Peek\Peek.dll
RE_DLL		= $(AUTODIR)\re\re.dll
BYTELOADER_DLL	= $(AUTODIR)\ByteLoader\ByteLoader.dll
DPROF_DLL	= $(AUTODIR)\Devel\DProf\DProf.dll
GLOB_DLL	= $(AUTODIR)\File\Glob\Glob.dll
HOSTNAME_DLL	= $(AUTODIR)\Sys\Hostname\Hostname.dll

ERRNO_PM	= $(LIBDIR)\Errno.pm

EXTENSION_C	=		\
		$(SOCKET).c	\
		$(FCNTL).c	\
		$(OPCODE).c	\
		$(SDBM_FILE).c	\
		$(IO).c		\
		$(POSIX).c	\
		$(ATTRS).c	\
		$(THREAD).c	\
		$(RE).c		\
		$(DUMPER).c	\
		$(PEEK).c	\
		$(B).c		\
		$(BYTELOADER).c	\
		$(DPROF).c	\
		$(GLOB).c	\
		$(HOSTNAME).c

EXTENSION_DLL	=		\
		$(SOCKET_DLL)	\
		$(FCNTL_DLL)	\
		$(OPCODE_DLL)	\
		$(SDBM_FILE_DLL)\
		$(IO_DLL)	\
		$(POSIX_DLL)	\
		$(ATTRS_DLL)	\
		$(DUMPER_DLL)	\
		$(PEEK_DLL)	\
		$(B_DLL)	\
		$(RE_DLL)	\
		$(THREAD_DLL)	\
		$(BYTELOADER_DLL)	\
		$(DPROF_DLL)	\
		$(GLOB_DLL)	\
		$(HOSTNAME_DLL)

EXTENSION_PM	=		\
		$(ERRNO_PM)

POD2HTML	= $(PODDIR)\pod2html
POD2MAN		= $(PODDIR)\pod2man
POD2LATEX	= $(PODDIR)\pod2latex
POD2TEXT	= $(PODDIR)\pod2text

# vars must be separated by "\t+~\t+", since we're using the tempfile
# version of config_sh.pl (we were overflowing someone's buffer by
# trying to fit them all on the command line)
#	-- BKS 10-17-1999
CFG_VARS	=					\
		INST_DRV=$(INST_DRV)		~	\
		INST_TOP=$(INST_TOP:s/\/\\/)	~	\
		INST_VER=$(INST_VER:s/\/\\/)	~	\
		INST_ARCH=$(INST_ARCH)		~	\
		archname=$(ARCHNAME)		~	\
		cc=$(CC)			~	\
		ccflags=$(OPTIMIZE) $(DEFINES) $(BUILDOPT)	~	\
		cf_email=$(EMAIL)		~	\
		d_crypt=$(D_CRYPT)		~	\
		d_mymalloc=$(PERL_MALLOC)	~	\
		libs=$(LIBFILES:f)		~	\
		incpath=$(CCINCDIR:s/\/\\/)	~	\
		libperl=$(PERLIMPLIB:f)		~	\
		libpth=$(CCLIBDIR:s/\/\\/);$(EXTRALIBDIRS:s/\/\\/)	~	\
		libc=$(LIBC)			~	\
		make=dmake			~	\
		_o=$(o)	obj_ext=$(o)		~	\
		_a=$(a)	lib_ext=$(a)		~	\
		static_ext=$(STATIC_EXT)	~	\
		dynamic_ext=$(DYNAMIC_EXT)	~	\
		nonxs_ext=$(NONXS_EXT)		~	\
		use5005threads=$(USE_5005THREADS)	~	\
		useithreads=$(USE_ITHREADS)	~	\
		usethreads=$(USE_5005THREADS)	~	\
		usemultiplicity=$(USE_MULTI)	~	\
		LINK_FLAGS=$(LINK_FLAGS:s/\/\\/)		~	\
		optimize=$(OPTIMIZE)

#
# set up targets varying between Win95 and WinNT builds
#

.IF "$(IS_WIN95)" == "define"
MK2 		= .\makefile.95
RIGHTMAKE	= __switch_makefiles
NOOP		= @rem
.ELSE
MK2		= __not_needed
RIGHTMAKE	= __not_needed
.ENDIF

#
# Top targets
#

all : .\config.h $(GLOBEXE) $(MINIPERL) $(MK2)		\
	$(RIGHTMAKE) $(MINIMOD) $(CONFIGPM) $(PERLEXE)	\
	$(X2P) $(EXTENSION_DLL) $(EXTENSION_PM)

$(DYNALOADER)$(o) : $(DYNALOADER).c $(CORE_H) $(EXTDIR)\DynaLoader\dlutils.c

#----------------------------------------------------------------

#-------------------- BEGIN Win95 SPECIFIC ----------------------

# this target is a jump-off point for Win95
#  1. it switches to the Win95-specific makefile if it exists
#     (__do_switch_makefiles)
#  2. it prints a message when the Win95-specific one finishes (__done)
#  3. it then kills this makefile by trying to make __no_such_target

__switch_makefiles: __do_switch_makefiles __done __no_such_target

__do_switch_makefiles:
.IF "$(NOTFIRST)" != "true"
	if exist $(MK2) $(MAKE:s/-S//) -f $(MK2) $(MAKETARGETS) NOTFIRST=true
.ELSE
	$(NOOP)
.ENDIF

.IF "$(NOTFIRST)" != "true"
__done:
	@echo Build process complete. Ignore any errors after this message.
	@echo Run "dmake test" to test and "dmake install" to install

.ELSE
# dummy targets for Win95-specific makefile

__done:
	$(NOOP)

__no_such_target:
	$(NOOP)

.ENDIF

# This target is used to generate the new makefile (.\makefile.95) for Win95

.\makefile.95: .\makefile.mk
	$(MINIPERL) genmk95.pl makefile.mk $(MK2)

#--------------------- END Win95 SPECIFIC ---------------------

# a blank target for when builds don't need to do certain things
# this target added for Win95 port but used to keep the WinNT port able to 
# use this file
__not_needed:
	$(NOOP)

$(GLOBEXE) : perlglob$(o)
.IF "$(CCTYPE)" == "BORLAND"
	$(CC) -c -w -v -tWM -I"$(CCINCDIR)" perlglob.c
	$(LINK32) -Tpe -ap $(BLINK_FLAGS) c0x32$(o) perlglob$(o) \
	    "$(CCLIBDIR)\32BIT\wildargs$(o)",$@,,import32.lib cw32mt.lib,
.ELIF "$(CCTYPE)" == "GCC"
	$(LINK32) $(BLINK_FLAGS) -mconsole -o $@ perlglob$(o) $(LIBFILES)
.ELSE
	$(LINK32) $(BLINK_FLAGS) $(LIBFILES) -out:$@ -subsystem:$(SUBSYS) \
	    perlglob$(o) setargv$(o) 
.ENDIF

perlglob$(o)  : perlglob.c

config.w32 : $(CFGSH_TMPL)
	copy $(CFGSH_TMPL) config.w32

.\config.h : $(CFGH_TMPL) $(CORE_NOCFG_H)
	-del /f config.h
	copy $(CFGH_TMPL) config.h

..\config.sh : config.w32 $(MINIPERL) config_sh.PL
	$(MINIPERL) -I..\lib config_sh.PL --cfgsh-option-file \
	    $(mktmp $(CFG_VARS)) config.w32 > ..\config.sh

# this target is for when changes to the main config.sh happen
# edit config.{b,v,g}c and make this target once for each supported
# compiler (e.g. `dmake CCTYPE=BORLAND regen_config_h`)
regen_config_h:
	perl config_sh.PL --cfgsh-option-file $(mktmp $(CFG_VARS)) \
	    $(CFGSH_TMPL) > ..\config.sh
	-cd .. && del /f perl.exe
	cd .. && perl configpm
	-del /f $(CFGH_TMPL)
	-mkdir $(COREDIR)
	-perl -I..\lib config_h.PL "INST_VER=$(INST_VER)"
	rename config.h $(CFGH_TMPL)

$(CONFIGPM) : $(MINIPERL) ..\config.sh config_h.PL ..\minimod.pl
	cd .. && miniperl configpm
	if exist lib\* $(RCOPY) lib\*.* ..\lib\$(NULL)
	$(XCOPY) ..\*.h $(COREDIR)\*.*
	$(XCOPY) *.h $(COREDIR)\*.*
	$(XCOPY) ..\ext\re\re.pm $(LIBDIR)\*.*
	$(RCOPY) include $(COREDIR)\*.*
	$(MINIPERL) -I..\lib config_h.PL "INST_VER=$(INST_VER)" \
	    || $(MAKE) $(MAKEMACROS) $(CONFIGPM) $(MAKEFILE)

$(MINIPERL) : $(MINIDIR) $(MINI_OBJ) $(CRTIPMLIBS)
.IF "$(CCTYPE)" == "BORLAND"
	$(LINK32) -Tpe -ap $(BLINK_FLAGS) \
	    @$(mktmp c0x32$(o) $(MINI_OBJ:s,\,\\),$(@:s,\,\\),,$(LIBFILES),)
.ELIF "$(CCTYPE)" == "GCC"
	$(LINK32) -v -mconsole -o $@ $(BLINK_FLAGS) \
	    $(mktmp $(LKPRE) $(MINI_OBJ:s,\,\\) $(LIBFILES) $(LKPOST)) 
.ELSE
	$(LINK32) -subsystem:console -out:$@ \
	    @$(mktmp $(BLINK_FLAGS) $(LIBFILES) $(MINI_OBJ:s,\,\\))
.ENDIF

$(MINIDIR) :
	if not exist "$(MINIDIR)" mkdir "$(MINIDIR)"

$(MINICORE_OBJ) : $(CORE_NOCFG_H)
	$(CC) -c $(CFLAGS) -DPERL_EXTERNAL_GLOB $(OBJOUT_FLAG)$@ ..\$(*B).c

$(MINIWIN32_OBJ) : $(CORE_NOCFG_H)
	$(CC) -c $(CFLAGS) $(OBJOUT_FLAG)$@ $(*B).c

# -DPERL_IMPLICIT_SYS needs C++ for perllib.c
# rules wrapped in .IFs break Win9X build (we end up with unbalanced []s unless
# unless the .IF is true), so instead we use a .ELSE with the default.
# This is the only file that depends on perlhost.h, vmem.h, and vdir.h

perllib$(o)	: perllib.c .\perlhost.h .\vdir.h .\vmem.h
.IF "$(USE_IMP_SYS)$(USE_OBJECT)" == "defineundef"
	$(CC) -c -I. $(CFLAGS_O) $(CXX_FLAG) $(OBJOUT_FLAG)$@ perllib.c
.ELSE
	$(CC) -c -I. $(CFLAGS_O) $(OBJOUT_FLAG)$@ perllib.c
.ENDIF

# 1. we don't want to rebuild miniperl.exe when config.h changes
# 2. we don't want to rebuild miniperl.exe with non-default config.h
$(MINI_OBJ)	: $(CORE_NOCFG_H)

$(WIN32_OBJ)	: $(CORE_H)
$(CORE_OBJ)	: $(CORE_H)
$(DLL_OBJ)	: $(CORE_H)
$(X2P_OBJ)	: $(CORE_H)

perldll.def : $(MINIPERL) $(CONFIGPM) ..\global.sym ..\pp.sym ..\makedef.pl
	$(MINIPERL) -w ..\makedef.pl PLATFORM=win32 $(OPTIMIZE) $(DEFINES) \
	$(BUILDOPT) CCTYPE=$(CCTYPE) > perldll.def

$(PERLDLL): perldll.def $(PERLDLL_OBJ) $(PERLDLL_RES)
.IF "$(CCTYPE)" == "BORLAND"
	$(LINK32) -Tpd -ap $(BLINK_FLAGS) \
	    @$(mktmp c0d32$(o) $(PERLDLL_OBJ:s,\,\\)\n \
		$@,\n \
		$(LIBFILES)\n \
		perldll.def\n)
	$(IMPLIB) $*.lib $@
.ELIF "$(CCTYPE)" == "GCC"
	$(LINK32) -mdll -o $@ -Wl,--base-file -Wl,perl.base $(BLINK_FLAGS) \
	    $(mktmp $(LKPRE) $(PERLDLL_OBJ:s,\,\\) $(LIBFILES) $(LKPOST))
	dlltool --output-lib $(PERLIMPLIB) \
		--dllname $(PERLDLL:b).dll \
		--def perldll.def \
		--base-file perl.base \
		--output-exp perl.exp
	$(LINK32) -mdll -o $@ $(BLINK_FLAGS) \
	    $(mktmp $(LKPRE) $(PERLDLL_OBJ:s,\,\\) $(LIBFILES) \
		perl.exp $(LKPOST))
.ELSE
	$(LINK32) -dll -def:perldll.def -out:$@ \
	    @$(mktmp -base:0x28000000 $(BLINK_FLAGS) $(DELAYLOAD) $(LIBFILES) \
	        $(PERLDLL_RES) $(PERLDLL_OBJ:s,\,\\))
.ENDIF
	$(XCOPY) $(PERLIMPLIB) $(COREDIR)

$(MINIMOD) : $(MINIPERL) ..\minimod.pl
	cd .. && miniperl minimod.pl > lib\ExtUtils\Miniperl.pm

..\x2p\a2p$(o) : ..\x2p\a2p.c
	$(CC) -I..\x2p $(CFLAGS) $(OBJOUT_FLAG)$@ -c ..\x2p\a2p.c

..\x2p\hash$(o) : ..\x2p\hash.c
	$(CC) -I..\x2p  $(CFLAGS) $(OBJOUT_FLAG)$@ -c ..\x2p\hash.c

..\x2p\str$(o) : ..\x2p\str.c
	$(CC) -I..\x2p  $(CFLAGS) $(OBJOUT_FLAG)$@ -c ..\x2p\str.c

..\x2p\util$(o) : ..\x2p\util.c
	$(CC) -I..\x2p  $(CFLAGS) $(OBJOUT_FLAG)$@ -c ..\x2p\util.c

..\x2p\walk$(o) : ..\x2p\walk.c
	$(CC) -I..\x2p  $(CFLAGS) $(OBJOUT_FLAG)$@ -c ..\x2p\walk.c

$(X2P) : $(MINIPERL) $(X2P_OBJ)
	$(MINIPERL) ..\x2p\find2perl.PL
	$(MINIPERL) ..\x2p\s2p.PL
.IF "$(CCTYPE)" == "BORLAND"
	$(LINK32) -Tpe -ap $(BLINK_FLAGS) \
	    @$(mktmp c0x32$(o) $(X2P_OBJ:s,\,\\),$(@:s,\,\\),,$(LIBFILES),)
.ELIF "$(CCTYPE)" == "GCC"
	$(LINK32) -v -o $@ $(BLINK_FLAGS) \
	    $(mktmp $(LKPRE) $(X2P_OBJ:s,\,\\) $(LIBFILES) $(LKPOST))
.ELSE
	$(LINK32) -subsystem:console -out:$@ \
	    @$(mktmp $(BLINK_FLAGS) $(LIBFILES) $(X2P_OBJ:s,\,\\))
.ENDIF

perlmain.c : runperl.c 
	copy runperl.c perlmain.c

perlmain$(o) : perlmain.c
	$(CC) $(CFLAGS_O) -UPERLDLL $(OBJOUT_FLAG)$@ -c perlmain.c

$(PERLEXE): $(PERLDLL) $(CONFIGPM) $(PERLEXE_OBJ) $(PERLEXE_RES)
.IF "$(CCTYPE)" == "BORLAND"
	$(LINK32) -Tpe -ap $(BLINK_FLAGS) \
	    @$(mktmp c0x32$(o) $(PERLEXE_OBJ:s,\,\\)\n \
	    $(@:s,\,\\),\n \
	    $(PERLIMPLIB) $(LIBFILES)\n)
.ELIF "$(CCTYPE)" == "GCC"
	$(LINK32) -mconsole -o $@ $(BLINK_FLAGS)  \
	    $(PERLEXE_OBJ) $(PERLIMPLIB) $(LIBFILES)
.ELSE
	$(LINK32) -subsystem:console -out:$@ -stack:0x8000000 $(BLINK_FLAGS) \
	    $(LIBFILES) $(PERLEXE_OBJ) $(SETARGV_OBJ) $(PERLIMPLIB) $(PERLEXE_RES)
.ENDIF
	copy $(PERLEXE) $(WPERLEXE)
	$(MINIPERL) -I..\lib bin\exetype.pl $(WPERLEXE) WINDOWS
	copy splittree.pl .. 
	$(MINIPERL) -I..\lib ..\splittree.pl "../LIB" $(AUTODIR)

$(DYNALOADER).c: $(MINIPERL) $(EXTDIR)\DynaLoader\dl_win32.xs $(CONFIGPM)
	if not exist $(AUTODIR) mkdir $(AUTODIR)
	cd $(EXTDIR)\$(*B) && ..\$(MINIPERL) -I..\..\lib $(*B)_pm.PL
	cd $(EXTDIR)\$(*B) && ..\$(MINIPERL) -I..\..\lib XSLoader_pm.PL
	$(XCOPY) $(EXTDIR)\$(*B)\$(*B).pm $(LIBDIR)\$(NULL)
	$(XCOPY) $(EXTDIR)\$(*B)\XSLoader.pm $(LIBDIR)\$(NULL)
	cd $(EXTDIR)\$(*B) && $(XSUBPP) dl_win32.xs > $(*B).c
	$(XCOPY) $(EXTDIR)\$(*B)\dlutils.c .

$(EXTDIR)\DynaLoader\dl_win32.xs: dl_win32.xs
	copy dl_win32.xs $(EXTDIR)\DynaLoader\dl_win32.xs

$(DUMPER_DLL): $(PERLEXE) $(DUMPER).xs
	cd $(EXTDIR)\Data\$(*B) && \
	..\..\..\miniperl -I..\..\..\lib Makefile.PL INSTALLDIRS=perl
	cd $(EXTDIR)\Data\$(*B) && $(MAKE)

$(DPROF_DLL): $(PERLEXE) $(DPROF).xs
	cd $(EXTDIR)\Devel\$(*B) && \
	..\..\..\miniperl -I..\..\..\lib Makefile.PL INSTALLDIRS=perl
	cd $(EXTDIR)\Devel\$(*B) && $(MAKE)

$(GLOB_DLL): $(PERLEXE) $(GLOB).xs
	cd $(EXTDIR)\File\$(*B) && \
	..\..\..\miniperl -I..\..\..\lib Makefile.PL INSTALLDIRS=perl
	cd $(EXTDIR)\File\$(*B) && $(MAKE)

$(PEEK_DLL): $(PERLEXE) $(PEEK).xs
	cd $(EXTDIR)\Devel\$(*B) && \
	..\..\..\miniperl -I..\..\..\lib Makefile.PL INSTALLDIRS=perl
	cd $(EXTDIR)\Devel\$(*B) && $(MAKE)

$(RE_DLL): $(PERLEXE) $(RE).xs
	cd $(EXTDIR)\$(*B) && \
	..\..\miniperl -I..\..\lib Makefile.PL INSTALLDIRS=perl
	cd $(EXTDIR)\$(*B) && $(MAKE)

$(B_DLL): $(PERLEXE) $(B).xs
	cd $(EXTDIR)\$(*B) && \
	..\..\miniperl -I..\..\lib Makefile.PL INSTALLDIRS=perl
	cd $(EXTDIR)\$(*B) && $(MAKE)

$(THREAD_DLL): $(PERLEXE) $(THREAD).xs
	cd $(EXTDIR)\$(*B) && \
	..\..\miniperl -I..\..\lib Makefile.PL INSTALLDIRS=perl
	cd $(EXTDIR)\$(*B) && $(MAKE)

$(ATTRS_DLL): $(PERLEXE) $(ATTRS).xs
	cd $(EXTDIR)\$(*B) && \
	..\..\miniperl -I..\..\lib Makefile.PL INSTALLDIRS=perl
	cd $(EXTDIR)\$(*B) && $(MAKE)

$(POSIX_DLL): $(PERLEXE) $(POSIX).xs
	cd $(EXTDIR)\$(*B) && \
	..\..\miniperl -I..\..\lib Makefile.PL INSTALLDIRS=perl
	cd $(EXTDIR)\$(*B) && $(MAKE)

$(IO_DLL): $(PERLEXE) $(IO).xs
	cd $(EXTDIR)\$(*B) && \
	..\..\miniperl -I..\..\lib Makefile.PL INSTALLDIRS=perl
	cd $(EXTDIR)\$(*B) && $(MAKE)

$(SDBM_FILE_DLL) : $(PERLEXE) $(SDBM_FILE).xs
	cd $(EXTDIR)\$(*B) && \
	..\..\miniperl -I..\..\lib Makefile.PL INSTALLDIRS=perl
	cd $(EXTDIR)\$(*B) && $(MAKE)

$(FCNTL_DLL): $(PERLEXE) $(FCNTL).xs
	cd $(EXTDIR)\$(*B) && \
	..\..\miniperl -I..\..\lib Makefile.PL INSTALLDIRS=perl
	cd $(EXTDIR)\$(*B) && $(MAKE)

$(OPCODE_DLL): $(PERLEXE) $(OPCODE).xs
	cd $(EXTDIR)\$(*B) && \
	..\..\miniperl -I..\..\lib Makefile.PL INSTALLDIRS=perl
	cd $(EXTDIR)\$(*B) && $(MAKE)

$(SOCKET_DLL): $(PERLEXE) $(SOCKET).xs
	cd $(EXTDIR)\$(*B) && \
	..\..\miniperl -I..\..\lib Makefile.PL INSTALLDIRS=perl
	cd $(EXTDIR)\$(*B) && $(MAKE)

$(HOSTNAME_DLL): $(PERLEXE) $(HOSTNAME).xs
	cd $(EXTDIR)\Sys\$(*B) && \
	..\..\..\miniperl -I..\..\..\lib Makefile.PL INSTALLDIRS=perl
	cd $(EXTDIR)\Sys\$(*B) && $(MAKE)

$(BYTELOADER_DLL): $(PERLEXE) $(BYTELOADER).xs
	cd $(EXTDIR)\$(*B) && \
	..\..\miniperl -I..\..\lib Makefile.PL INSTALLDIRS=perl
	cd $(EXTDIR)\$(*B) && $(MAKE)

$(ERRNO_PM): $(PERLEXE) $(ERRNO)_pm.PL
	cd $(EXTDIR)\$(*B) && \
	..\..\miniperl -I..\..\lib Makefile.PL INSTALLDIRS=perl
	cd $(EXTDIR)\$(*B) && $(MAKE)

doc: $(PERLEXE)
	$(PERLEXE) -I..\lib ..\installhtml --podroot=.. --htmldir=./html \
	    --podpath=pod:lib:ext:utils --htmlroot="file://$(INST_HTML:s,:,|,)"\
	    --libpod=perlfunc:perlguts:perlvar:perlrun:perlop --recurse

utils: $(PERLEXE) $(X2P)
	cd ..\utils && $(MAKE) PERL=$(MINIPERL)
	copy ..\README.amiga ..\pod\perlamiga.pod
	copy ..\README.cygwin ..\pod\perlcygwin.pod
	copy ..\README.dos ..\pod\perldos.pod
	copy ..\README.hpux ..\pod\perlhpux.pod
	copy ..\README.machten ..\pod\perlmachten.pod
	copy ..\README.os2 ..\pod\perlos2.pod
	copy ..\README.os2 ..\pod\perlos2.pod
	copy ..\vms\perlvms.pod ..\pod\perlvms.pod
	cd ..\pod && $(MAKE) -f ..\win32\pod.mak converters
	$(PERLEXE) $(PL2BAT) $(UTILS)

distclean: clean
	-del /f $(MINIPERL) $(PERLEXE) $(PERLDLL) $(GLOBEXE) \
		$(PERLIMPLIB) ..\miniperl$(a) $(MINIMOD)
	-del /f *.def *.map
	-del /f $(EXTENSION_DLL) $(EXTENSION_PM)
	-del /f $(EXTENSION_C) $(DYNALOADER).c $(ERRNO).pm
	-del /f $(EXTDIR)\DynaLoader\dl_win32.xs
	-del /f $(LIBDIR)\.exists $(LIBDIR)\attrs.pm $(LIBDIR)\DynaLoader.pm
	-del /f $(LIBDIR)\XSLoader.pm
	-del /f $(LIBDIR)\Fcntl.pm $(LIBDIR)\IO.pm $(LIBDIR)\Opcode.pm
	-del /f $(LIBDIR)\ops.pm $(LIBDIR)\Safe.pm $(LIBDIR)\Thread.pm
	-del /f $(LIBDIR)\SDBM_File.pm $(LIBDIR)\Socket.pm $(LIBDIR)\POSIX.pm
	-del /f $(LIBDIR)\B.pm $(LIBDIR)\O.pm $(LIBDIR)\re.pm
	-del /f $(LIBDIR)\Data\Dumper.pm $(LIBDIR)\ByteLoader.pm
	-del /f $(LIBDIR)\Devel\Peek.pm $(LIBDIR)\Devel\DProf.pm
	-del /f $(LIBDIR)\File\Glob.pm
	-rmdir /s /q $(LIBDIR)\IO || rmdir /s $(LIBDIR)\IO
	-rmdir /s /q $(LIBDIR)\Thread || rmdir /s $(LIBDIR)\Thread
	-rmdir /s /q $(LIBDIR)\B || rmdir /s $(LIBDIR)\B
	-rmdir /s /q $(LIBDIR)\Data || rmdir /s $(LIBDIR)\Data
	-del /f $(PODDIR)\*.html
	-del /f $(PODDIR)\*.bat
	-cd ..\utils && del /f h2ph splain perlbug pl2pm c2ph h2xs perldoc \
	    dprofpp *.bat
	-cd ..\x2p && del /f find2perl s2p *.bat
	-del /f ..\config.sh ..\splittree.pl perlmain.c dlutils.c config.h.new
	-del /f $(CONFIGPM)
	-del /f bin\*.bat
	-cd $(EXTDIR) && del /s *$(a) *.def *.map *.pdb *.bs Makefile *$(o) \
	    pm_to_blib
	-rmdir /s /q $(AUTODIR) || rmdir /s $(AUTODIR)
	-rmdir /s /q $(COREDIR) || rmdir /s $(COREDIR)

install : all installbare installhtml

installbare : $(RIGHTMAKE) utils
	$(PERLEXE) ..\installperl
	if exist $(WPERLEXE) $(XCOPY) $(WPERLEXE) $(INST_BIN)\*.*
	$(XCOPY) $(GLOBEXE) $(INST_BIN)\*.*
	$(XCOPY) bin\*.bat $(INST_SCRIPT)\*.*

installhtml : doc
	$(RCOPY) html\*.* $(INST_HTML)\*.*

inst_lib : $(CONFIGPM)
	copy splittree.pl .. 
	$(MINIPERL) -I..\lib ..\splittree.pl "../LIB" $(AUTODIR)
	$(RCOPY) ..\lib $(INST_LIB)\*.*

minitest : $(MINIPERL) $(GLOBEXE) $(CONFIGPM) utils
	$(XCOPY) $(MINIPERL) ..\t\perl.exe
.IF "$(CCTYPE)" == "BORLAND"
	$(XCOPY) $(GLOBBAT) ..\t\$(NULL)
.ELSE
	$(XCOPY) $(GLOBEXE) ..\t\$(NULL)
.ENDIF
	attrib -r ..\t\*.*
	copy test ..\t
	cd ..\t && \
	$(MINIPERL) -I..\lib test base/*.t comp/*.t cmd/*.t io/*.t op/*.t pragma/*.t

test-prep : all utils
	$(XCOPY) $(PERLEXE) ..\t\$(NULL)
	$(XCOPY) $(PERLDLL) ..\t\$(NULL)
.IF "$(CCTYPE)" == "BORLAND"
	$(XCOPY) $(GLOBBAT) ..\t\$(NULL)
.ELSE
	$(XCOPY) $(GLOBEXE) ..\t\$(NULL)
.ENDIF

test : $(RIGHTMAKE) test-prep
	cd ..\t && $(PERLEXE) -I..\lib harness

test-notty : test-prep
	set PERL_SKIP_TTY_TEST=1 && \
	    cd ..\t && $(PERLEXE) -I.\lib harness

test-wide : test-prep
	set HARNESS_PERL_SWITCHES=-C && \
	    cd ..\t && $(PERLEXE) -I..\lib harness

test-wide-notty : test-prep
	set PERL_SKIP_TTY_TEST=1 && \
	    set HARNESS_PERL_SWITCHES=-C && \
	    cd ..\t && $(PERLEXE) -I..\lib harness

clean : 
	-@erase miniperlmain$(o)
	-@erase $(MINIPERL)
	-@erase perlglob$(o)
	-@erase perlmain$(o)
	-@erase config.w32
	-@erase /f config.h
	-@erase $(GLOBEXE)
	-@erase $(PERLEXE)
	-@erase $(WPERLEXE)
	-@erase $(PERLDLL)
	-@erase $(CORE_OBJ)
	-rmdir /s /q $(MINIDIR) || rmdir /s $(MINIDIR)
	-@erase $(WIN32_OBJ)
	-@erase $(DLL_OBJ)
	-@erase $(X2P_OBJ)
	-@erase ..\*$(o) ..\*$(a) ..\*.exp *$(o) *$(a) *.exp *.res
	-@erase ..\t\*.exe ..\t\*.dll ..\t\*.bat
	-@erase ..\x2p\*.exe ..\x2p\*.bat
	-@erase *.ilk
	-@erase *.pdb
