# Note that the macros are not used for the install target any more with the
# change to the install(1) program.  Edit the install commands.

ifneq "" "$(wildcard /bin/mkdirs)"
  MKDIRS = /bin/mkdirs
else
  MKDIRS = /bin/mkdir -p
endif

FILES = mach-o/ldsyms.h mach-o/reloc.h mach-o/dyld.h \
	mach-o/nlist.h mach-o/stab.h mach-o/loader.h mach-o/fat.h \
	mach-o/dyld_debug.h mach-o/arch.h mach-o/gmon.h mach-o/ranlib.h \
	mach-o/swap.h mach-o/getsect.h mach-o/i386/swap.h mach-o/dyld_gdb.h \
	mach-o/ppc/swap.h mach-o/ppc/reloc.h \

LOCFILES = mach-o/rld_state.h mach-o/rld.h mach-o/sarld.h mach-o/kld.h \
	   mach-o/i860/swap.h mach-o/i860/reloc.h \
	   mach-o/hppa/swap.h mach-o/hppa/reloc.h \
	   mach-o/m88k/swap.h mach-o/m88k/reloc.h \
	   mach-o/m68k/swap.h \
	   mach-o/sparc/swap.h mach-o/sparc/reloc.h

# Note that OTHER_SRCS do NOT get installed
GAS_OTHER_SRCS = stuff/bytesex.h stuff/bool.h stuff/round.h stuff/errors.h \
		stuff/openstep_mach.h
OTHER_SRCS = notes gnu/symseg.h \
	     stuff/allocate.h stuff/arch.h stuff/errors.h stuff/execute.h \
	     stuff/ofile.h \
	     stuff/hash_string.h stuff/breakout.h stuff/best_arch.h \
	     stuff/hppa.h stuff/reloc.h stuff/vm_flush_cache.h \
	     stuff/print.h stuff/version_number.h sys/gmon.h \
	     stuff/openstep_mach.h stuff/seg_addr_table.h stuff/dylib_table.h \
	     stuff/SymLoc.h stuff/dylib_roots.h stuff/guess_short_name.h \
	     $(GAS_OTHER_SRCS)
ENCUMBERED_SRCS = gnu/a.out.h gnu/exec.h

# Note that MISSING_SRCS are those may not be on all build machines
GAS_MISSING_SRCS = mach/i860/thread_status.h \
	       mach/ppc/thread_status.h \
	       architecture/nrw/reg_help.h \
	       architecture/nrw/macro_help.h \
	       mach/m88k/thread_status.h \
	       architecture/m88k/fp_regs.h \
	       architecture/m88k/reg_help.h \
	       mach/hppa/thread_status.h \
	       mach/sparc/thread_status.h \
	       architecture/sparc/reg.h \
	       mach/m68k/thread_status.h
MISSING_SRCS = mach/machine.h \
	       architecture/i386/fpu.h architecture/i386/frame.h \
	       standalone/libsa.h \
	       $(GAS_MISSING_SRCS)

nextstep_INCDIR = /NextDeveloper/Headers
nextstep_LOCINCDIR = /LocalDeveloper/Headers

teflon_INCDIR = /System/Library/Frameworks/System.framework/Versions/B/Headers
teflon_LOCINCDIR = /System/Library/Frameworks/System.framework/Versions/B/PrivateHeaders

ifeq "macos" "$(RC_OS)"
 macos_INCDIR := $(shell if [ "$(RC_RELEASE)" = "Beaker" ] ||    \
			    [ "$(RC_RELEASE)" = "Bunsen" ] ||    \
			    [ "$(RC_RELEASE)" = "Gonzo"  ] ||    \
			    [ "$(RC_RELEASE)" = "Kodiak" ]; then \
	 echo /System/Library/Frameworks/System.framework/Versions/B/Headers; \
	 else echo /usr/include; \
	 fi; )
 macos_LOCINCDIR := $(shell if [ "$(RC_RELEASE)" = "Beaker" ] ||    \
			       [ "$(RC_RELEASE)" = "Bunsen" ] ||    \
			       [ "$(RC_RELEASE)" = "Gonzo"  ] ||    \
			       [ "$(RC_RELEASE)" = "Kodiak" ]; then \
 echo /System/Library/Frameworks/System.framework/Versions/B/PrivateHeaders; \
 else echo /usr/local/include; \
 fi; )
else
  macos_INCDIR = /System/Library/Frameworks/System.framework/Versions/B/Headers
  macos_LOCINCDIR = /System/Library/Frameworks/System.framework/Versions/B/PrivateHeaders
endif

SRC_FILES = Makefile $(FILES) $(LOCFILES) $(OTHER_SRCS) $(MISSING_SRCS)
GAS_SRC_FILES = Makefile $(GAS_OTHER_SRCS) $(GAS_MISSING_SRCS)
ENCUMBERED_SRC_FILES = $(SRC_FILES) $(ENCUMBERED_SRCS)

installsrc:
	$(MKDIRS) $(SRCROOT)
	chmod 755 $(SRCROOT)
	gnutar cf - $(ENCUMBERED_SRC_FILES) | (cd $(SRCROOT); gnutar xf -)

installGASsrc:
	$(MKDIRS) $(SRCROOT)
	chmod 755 $(SRCROOT)
	gnutar cf - $(GAS_SRC_FILES) | (cd $(SRCROOT); gnutar xf -)

install: dirs $(RC_OS)_install

teflon_install macos_install: common_install

nextstep_install: common_install
	cd mach-o; \
	    install -c -m 444 ${IFLAGS} gmon.h \
	    ${DSTROOT}${$(RC_OS)_INCDIR}/mach-o

common_install:
	cd mach-o; \
	    install -c -m 444 ${IFLAGS} arch.h ldsyms.h reloc.h dyld.h \
	    dyld_debug.h nlist.h stab.h loader.h fat.h swap.h getsect.h \
	    ranlib.h ${DSTROOT}${$(RC_OS)_INCDIR}/mach-o
	cd mach-o/i386; \
	    install -c -m 444 ${IFLAGS} swap.h \
	    ${DSTROOT}${$(RC_OS)_INCDIR}/mach-o/i386
	cd mach-o/ppc; \
	    install -c -m 444 ${IFLAGS} reloc.h swap.h \
	    ${DSTROOT}${$(RC_OS)_INCDIR}/mach-o/ppc
	cd mach-o/m68k; \
	    install -c -m 444 ${IFLAGS} swap.h \
	    ${DSTROOT}${$(RC_OS)_LOCINCDIR}/mach-o/m68k
	cd mach-o/sparc; \
	    install -c -m 444 ${IFLAGS} reloc.h swap.h \
	    ${DSTROOT}${$(RC_OS)_LOCINCDIR}/mach-o/sparc
	cd mach-o/hppa; \
	    install -c -m 444 ${IFLAGS} reloc.h swap.h \
	    ${DSTROOT}${$(RC_OS)_LOCINCDIR}/mach-o/hppa
	cd mach-o/i860; \
	    install -c -m 444 ${IFLAGS} reloc.h swap.h \
	    ${DSTROOT}${$(RC_OS)_LOCINCDIR}/mach-o/i860
	cd mach-o/m88k; \
	    install -c -m 444 ${IFLAGS} reloc.h swap.h \
	    ${DSTROOT}${$(RC_OS)_LOCINCDIR}/mach-o/m88k
	cd mach-o; \
	    install -c -m 444 ${IFLAGS} rld.h rld_state.h dyld_gdb.h \
	    ${DSTROOT}${$(RC_OS)_LOCINCDIR}/mach-o
	cd mach-o; \
	    install -c -m 444 ${IFLAGS} sarld.h kld.h \
	    ${DSTROOT}${$(RC_OS)_LOCINCDIR}/mach-o
	cd stuff; \
	    install -c -m 444 ${IFLAGS} bool.h \
	    ${DSTROOT}${$(RC_OS)_LOCINCDIR}/dyld

dirs:
	$(MKDIRS) ${DSTROOT}${$(RC_OS)_INCDIR}/mach-o/i386
	$(MKDIRS) ${DSTROOT}${$(RC_OS)_INCDIR}/mach-o/ppc
	$(MKDIRS) ${DSTROOT}${$(RC_OS)_LOCINCDIR}/mach-o/m68k
	$(MKDIRS) ${DSTROOT}${$(RC_OS)_LOCINCDIR}/mach-o/sparc
	$(MKDIRS) ${DSTROOT}${$(RC_OS)_LOCINCDIR}/mach-o/hppa
	$(MKDIRS) ${DSTROOT}${$(RC_OS)_LOCINCDIR}/mach-o/i860
	$(MKDIRS) ${DSTROOT}${$(RC_OS)_LOCINCDIR}/mach-o/m88k
	$(MKDIRS) ${DSTROOT}${$(RC_OS)_LOCINCDIR}/dyld

depend:

clean shlib_clean:

all:
