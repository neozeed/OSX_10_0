#
# Generated by the NeXT Project Builder.
#
# NOTE: Do NOT change this file -- Project Builder maintains it.
#
# Put all of your customizations in files called Makefile.preamble
# and Makefile.postamble (both optional), and Makefile will include them.
#

NAME = bootx

PROJECTVERSION = 2.8
PROJECT_TYPE = Tool

SUBPROJECTS = include.subproj sl.subproj ci.subproj fs.subproj\
              libclite.subproj

OTHERSRCS = Makefile.preamble Makefile Makefile.postamble bootinfo.hdr


MAKEFILEDIR = $(MAKEFILEPATH)/pb_makefiles
CODE_GEN_STYLE = DYNAMIC
MAKEFILE = tool.make
NEXTSTEP_INSTALLDIR = /bin
WINDOWS_INSTALLDIR = /Library/Executables
PDO_UNIX_INSTALLDIR = /bin
LIBS = -lcc_kext
DEBUG_LIBS = $(LIBS)
PROF_LIBS = $(LIBS)


HEADER_PATHS = -I$(SRCROOT)/bootx.tproj/include.subproj
NEXTSTEP_PB_CFLAGS = -static
NEXTSTEP_PB_LDFLAGS = -nostdlib -e _StartTVector -seg1addr 01C00000


NEXTSTEP_OBJCPLUS_COMPILER = /usr/bin/cc
WINDOWS_OBJCPLUS_COMPILER = $(DEVDIR)/gcc
PDO_UNIX_OBJCPLUS_COMPILER = $(NEXTDEV_BIN)/gcc
NEXTSTEP_JAVA_COMPILER = /usr/bin/javac
WINDOWS_JAVA_COMPILER = $(JDKBINDIR)/javac.exe
PDO_UNIX_JAVA_COMPILER = $(NEXTDEV_BIN)/javac

include $(MAKEFILEDIR)/platform.make

-include Makefile.preamble

include $(MAKEFILEDIR)/$(MAKEFILE)

-include Makefile.postamble

-include Makefile.dependencies
