#	$Id: NEWS-OS.6.x,v 1.1.1.1 2000/06/10 00:40:47 wsanchez Exp $
define(`confCC', `/bin/cc')
define(`confBEFORE', `sysexits.h ndbm.o')
define(`confMAPDEF', `-DNDBM -DNIS')
define(`confLIBS', `ndbm.o -lelf -lsocket -lnsl')
define(`confMBINDIR', `/usr/lib')
define(`confSBINDIR', `/usr/etc')
define(`confUBINDIR', `/usr/ucb')
define(`confEBINDIR', `/usr/lib')
define(`confSBINGRP', `sys')
define(`confINSTALL', `/usr/ucb/install')
PUSHDIVERT(3)
sysexits.h:
	ln -s /usr/ucbinclude/sysexits.h .

ndbm.o:
	if [ ! -f /usr/include/ndbm.h ]; then \
		ln -s /usr/ucbinclude/ndbm.h .; \
	fi; \
	if [ -f /usr/lib/libndbm.a ]; then \
		ar x /usr/lib/libndbm.a ndbm.o; \
	else \
		ar x /usr/ucblib/libucb.a ndbm.o; \
	fi;
POPDIVERT
