##
# Darwin (Mac OS) hints
# Wilfredo Sanchez <wsanchez@apple.com>
##

##
# Paths
##

# BSD paths
prefix='/usr';
siteprefix='/usr/local';
vendorprefix='/usr/local'; usevendorprefix='define';

# 4BSD uses /usr/share/man, not /usr/man.
# Don't put man pages in /usr/lib; that's goofy.
man1dir='/usr/share/man/man1';
man3dir='/usr/share/man/man3';

# Where to put modules.
privlib='/System/Library/Perl';
sitelib='/Library/Perl';
vendorlib='/Network/Library/Perl';

# Comment out /usr/local paths to work around some ongoing compiler work.
# This is not necessary outside of Apple's internal build environment.
glibpth="/usr/lib";
#locincpth="/usr/local/include";
#loclibpth="/usr/local/lib";
locincpth="";
loclibpth="";

##
# Tool chain settings
##

# Since we can build fat, the archname doesn't need the processor type
archname='darwin';

# nm works.
usenm='true';

# Libc is in libsystem.
libc='/System/Library/Frameworks/System.framework/System';

# Optimize.
optimize='-O3';

# We have a prototype for telldir, we don't like commons.
ccflags="${ccflags} -pipe -fno-common -DHAS_TELLDIR_PROTOTYPE";

# Shared library extension is .dylib.
# Bundle extension is .bundle.
ld='cc';
so='dylib';
dlext='bundle';
dlsrc='dl_dyld.xs'; usedl='define';
cccdlflags='';
lddlflags="${ldflags} -bundle -undefined suppress";
ldlibpthname='DYLD_LIBRARY_PATH';
useshrplib='true';

##
# System libraries
##

# vfork works
usevfork='true';

# malloc works
usemymalloc='n';
