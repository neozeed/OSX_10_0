#
# Generated by the NeXT Project Builder.
#
# NOTE: Do NOT change this file -- Project Builder maintains it.
#
# Put all of your customizations in files called Makefile.preamble
# and Makefile.postamble (both optional), and Makefile will include them.
#

NAME = mountd

PROJECTVERSION = 2.8
PROJECT_TYPE = Tool

HFILES = pathnames.h

CFILES = mountd.c

OTHERSRCS = Makefile.preamble Makefile.postamble Makefile Makefile.dist exports.5\
            netgroup.5 mountd.8


MAKEFILEDIR = $(MAKEFILEPATH)/pb_makefiles
CODE_GEN_STYLE = DYNAMIC
MAKEFILE = tool.make
NEXTSTEP_INSTALLDIR = /usr/sbin
LIBS = 
DEBUG_LIBS = $(LIBS)
PROF_LIBS = $(LIBS)


NEXTSTEP_PB_CFLAGS = -DNFS -DMFS -DCD9660
WINDOWS_PB_CFLAGS = -DNFS -DMFS -DCD9660
PDO_UNIX_PB_CFLAGS = -DNFS -DMFS -DCD9660


NEXTSTEP_BUILD_OUTPUT_DIR = /$(USER)/BUILD

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
