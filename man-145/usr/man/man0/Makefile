#
# Copyright (c) 1986 Regents of the University of California.
# All rights reserved.  The Berkeley software License Agreement
# specifies the terms and conditions for redistribution.
#
#	@(#)Makefile	6.2 (Berkeley) 5/15/86
#
MAN0DIR=${DSTROOT}/usr/man/man0
MAN0FILES=title.urm title.prm preface.ms intro.ms toc.in ptx.in \
	break cshcmd ignore tocrc

TOCS=	toc1 toc2 toc3 toc3f toc4 toc5 toc6 toc7 toc8 \
	tocx1 tocx2 tocx3 tocx3f tocx4 tocx5 tocx6 tocx7 tocx8
MACROS=	-msU
PRINTER=PS
TROFF=	ptroff

all: title.urm.${PRINTER} preface.${PRINTER} intro.${PRINTER} \
     toc.${PRINTER} ptx.${PRINTER} title.prm.${PRINTER}
	lpr -${PRINTER} -n title.urm.${PRINTER}
	lpr -${PRINTER} -n preface.${PRINTER}
	lpr -${PRINTER} -n intro.${PRINTER}
	lpr -${PRINTER} -n toc.${PRINTER}
	lpr -${PRINTER} -n ptx.${PRINTER}
	lpr -${PRINTER} -n title.prm.${PRINTER}

title.urm.${PRINTER}: title.urm
	${TROFF} -t title.urm >title.urm.${PRINTER}

preface.${PRINTER}: preface.ms
	${TROFF} ${MACROS} -t preface.ms >preface.${PRINTER}

intro.${PRINTER}: intro.ms
	${TROFF} ${MACROS} -t intro.ms >intro.${PRINTER}

toc.${PRINTER}: toc.in ptxx
	${TROFF} -t toc.in >toc.${PRINTER}

ptx.${PRINTER}: ptx.in ptxx
	${TROFF} -t ptx.in >ptx.${PRINTER}

title.prm.${PRINTER}: title.prm ptxx
	${TROFF} -t title.prm >title.prm.${PRINTER}

ptxx:
	tocrc all
	tocrc t

clean:
	rm -f title.urm.[PT]* preface.[PT]* intro.[PT]* toc.[PT]* ptx.[PT]* \
	title.prm.[PT]* ${TOCS} ptxx *.spell errs Errs make.out

install: ckdstroot ${MAN0DIR}
	for i in ${MAN0FILES}; \
	do \
		install $$i ${MAN0DIR}; \
	done

ckdstroot:
	@if [ -n "${DSTROOT}" ]; \
	then \
		exit 0; \
	else \
		echo Must define DSTROOT; \
		exit 1; \
	fi

${MAN0DIR}:
	mkdir -p $@
