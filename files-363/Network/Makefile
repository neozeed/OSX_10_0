Destination = $(DSTROOT)/Network

# Common Makefile
include $(MAKEFILEPATH)/CoreOS/ReleaseControl/Common.make

# Subdirectories with their own makefiles
SubDirs = 

install::
	$(_v) for subdir in $(SubDirs); do						\
		(cd "$$subdir" && $(MAKE) $@ Destination="$(Destination)/$$subdir");	\
	      done

install::
	@echo "Installing $(Destination)"
	$(_v) $(INSTALL_DIRECTORY) "$(Destination)"
	$(_v) $(INSTALL_DIRECTORY) "$(Destination)/Applications"
	$(_v) $(INSTALL_DIRECTORY) "$(Destination)/Library"
	$(_v) (cd ../Users          && $(MAKE) $@ Destination="$(Destination)/Users" SubDirs="");
