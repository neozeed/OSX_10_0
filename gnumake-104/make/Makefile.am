# This is a -*-Makefile-*-, or close enough

AUTOMAKE_OPTIONS = 1.4

SUBDIRS =	$(GLOBDIR) i18n

bin_PROGRAMS =	make

# These source files also have gettext references
SRCS	     =  ar.c arscan.c commands.c dir.c expand.c file.c function.c \
		getopt.c implicit.c job.c main.c misc.c read.c remake.c \
		rule.c signame.c variable.c vpath.c

make_SOURCES =	$(SRCS) default.c remote-$(REMOTE).c version.c \
		getopt1.c next.c

noinst_HEADERS = commands.h dep.h filedef.h job.h make.h rule.h variable.h \
		debug.h signame.h getopt.h gettext.h

make_LDADD =	$(LIBOBJS) @ALLOCA@ $(GLOBLIB)

man_MANS =	make.1
info_TEXINFOS =	make.texinfo

DEFS =		-DALIASPATH=\"$(aliaspath)\" -DLOCALEDIR=\"$(localedir)\" -DLIBDIR=\"$(libdir)\" -DINCLUDEDIR=\"$(includedir)\" @DEFS@

INCLUDES =	-I. -I$(srcdir) $(GLOBINC)

EXTRA_DIST = 	README build.sh.in $(man_MANS) README.customs remote-cstms.c\
		make-stds.texi SCOPTIONS SMakefile\
		README.Amiga Makefile.ami config.ami make.lnk amiga.c amiga.h\
		README.DOS Makefile.DOS configure.bat dosbuild.bat configh.dos\
		README.W32 NMakefile config.h.W32 build_w32.bat subproc.bat\
		readme.vms makefile.vms makefile.com config.h-vms vmsdir.h\
		vmsfunctions.c vmsify.c\
		gettext.c\
		glob/COPYING.LIB glob/ChangeLog glob/Makefile.am\
		glob/Makefile.ami glob/Makefile.in glob/SCOPTIONS\
		glob/SMakefile glob/configure.bat glob/fnmatch.c\
		glob/fnmatch.h glob/glob.c glob/glob.h

MOSTLYCLEANFILES = loadavg.c
CLEANFILES =	loadavg

MAKE_HOST =	@MAKE_HOST@


# --------------- Internationalization Section

POTFILES     =	$(SRCS) remote-cstms.c vmsfunctions.c

localedir    =	$(prefix)/share/locale
aliaspath    =	$(localedir):.

all-local: stamp-pot

stamp-pot: $(POTFILES)
	@echo "$(POTFILES)" > $@


# --------------- Local INSTALL Section

# If necessary, change the gid of the app and turn on the setgid flag.
#

# Whether or not make needs to be installed setgid.
# The value should be either `true' or `false'.
# On many systems, the getloadavg function (used to implement the `-l'
# switch) will not work unless make is installed setgid kmem.
#
inst_setgid = @NEED_SETGID@

# Install make setgid to this group so it can get the load average.
#
inst_group = @KMEM_GROUP@

install-exec-local:
	@if $(inst_setgid); then \
	   app=$(DESTDIR)$(bindir)/`echo $(bin_PROGRAMS)|sed '$(transform)'`; \
	   if chgrp $(inst_group) $$app && chmod g+s $$app; then \
	     echo "chgrp $(inst_group) $$app && chmod g+s $$app"; \
	   else \
	     echo "$$app needs to be owned by group $(inst_group) and setgid;"; \
	     echo "otherwise the \`-l' option will probably not work."; \
	     echo "You may need special privileges to complete the installation"; \
	     echo "of $$app."; \
	   fi; \
	 else true; fi

# --------------- Local DIST Section

# Install the w32 and tests subdirectories
#
dist-hook:
	(cd $(srcdir); \
	 sub=`find w32 tests -follow \( -name CVS -prune -o -name work -prune \) -o \( -name \*.orig -o -name \*.rej -o -name \*~ -prune \) -o -type f -print`; \
	 tar chf - $$sub) \
	| (cd $(distdir); tar xfBp -)


# --------------- Local CHECK Section

check-local: check-regression check-loadavg
	@banner=" Regression PASSED: GNU Make $(VERSION) ($(MAKE_HOST)) built with $(CC) "; \
	dashes=`echo "$$banner" | sed s/./=/g`; \
	echo; \
	echo "$$dashes"; \
	echo "$$banner"; \
	echo "$$dashes"; \
	echo

.PHONY: check-loadavg check-regression

# > check-loadavg
#
loadavg: loadavg.c config.h
	@rm -f loadavg
	$(LINK) -DTEST $(DEFS) $(INCLUDES) $(AM_CPPFLAGS) $(CPPFLAGS) $(make_LDFLAGS) loadavg.c $(LIBS)

# We copy getloadavg.c into a different file rather than compiling it
# directly because some compilers clobber getloadavg.o in the process.
#
loadavg.c: getloadavg.c
	ln $(srcdir)/getloadavg.c loadavg.c || \
	  cp $(srcdir)/getloadavg.c loadavg.c

check-loadavg: loadavg
	@echo The system uptime program believes the load average to be:
	-uptime
	@echo The GNU load average checking code believes:
	-./loadavg

# > check-regression
#
# Look for the make test suite, and run it if found and we can find perl.
# If we're building outside the tree, we use symlinks to make a local copy of
# the test suite.  Unfortunately the test suite itself isn't localizable yet.
#
MAKETESTFLAGS =

check-regression:
	@if test -f "$(srcdir)/tests/run_make_tests"; then \
	  if $(PERL) -v >/dev/null 2>&1; then \
	    case `cd $(srcdir); pwd` in `pwd`) : ;; \
	      *) test -d tests || mkdir tests; \
		 rm -f srctests; \
		 if ln -s "$(srcdir)/tests" srctests; then \
		   for f in run_make_tests run_make_tests.pl test_driver.pl scripts; do \
		     rm -f tests/$$f; ln -s ../srctests/$$f tests; \
		   done; fi ;; \
	    esac; \
	    echo "cd tests && $(PERL) ./run_make_tests.pl -make ../make $(MAKETESTFLAGS)"; \
	    cd tests && $(PERL) ./run_make_tests.pl -make ../make $(MAKETESTFLAGS); \
	  else \
	    echo "Can't find a working Perl ($(PERL)); the test suite requires Perl."; \
	  fi; \
	 else \
	  echo "Can't find the GNU Make test suite ($(srcdir)/tests)."; \
	 fi


# --------------- Local CLEAN section

maintainer-clean-local:
	-test -z "$(MAINTAINERCLEANFILES)" || rm -f $(MAINTAINERCLEANFILES)


# --------------- Maintainer's Section

@MAINT_MAKEFILE@
