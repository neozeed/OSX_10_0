# Target: Intel 386 running NetBSD
TDEPFILES= i386-tdep.o i387-tdep.o i386nbsd-tdep.o solib.o solib-svr4.o
TM_FILE= tm-nbsd.h

GDBSERVER_DEPFILES= low-nbsd.o
