# Columbia SIP server makefile fragment for resparse-1.3
# Copyright 1998-1999 by Columbia University; all rights reserved 

libresparse_dir := $(topdir)/resparse-1.3

LIBRESPARSE_SRC := \
	$(libresparse_dir)/res_copy.c \
	$(libresparse_dir)/res_free.c \
	$(libresparse_dir)/res_info.c \
	$(libresparse_dir)/res_parse.c \
	$(libresparse_dir)/res_print.c

LIBRESPARSE_HEADERS := \
	$(libresparse_dir)/res_info.h \
	$(libresparse_dir)/rr.h \
	$(libresparse_dir)/rr-internal.h

LIBRESPARSE_MISC := \
	$(libresparse_dir)/ChangeLog \
	$(libresparse_dir)/HPUX.Install \
	$(libresparse_dir)/Makefile.orig \
	$(libresparse_dir)/README \
	$(libresparse_dir)/README.HPPD \
	$(libresparse_dir)/resparse.3 \
	$(libresparse_dir)/module.mk \
        $(libresparse_dir)/resparse.dsp

LIBRESPARSE_MISCCLEAN := \
	$(libresparse_dir)/libresparse.a \
	$(libresparse_dir)/testres

# How other parts of the build find us...
# Target for make:
LIBRESPARSE := $(libresparse_dir)/libresparse.a

# Incantations for compile and build:
LIBRESPARSE_INC := -I$(topsrcdir)/$(libresparse_dir)
LIBRESPARSE_LIB := -L$(libresparse_dir) -lresparse


LIBRESPARSE_OBJ := $(patsubst %.c,%.o,$(filter %.c,$(LIBRESPARSE_SRC)))

$(LIBRESPARSE): $(LIBRESPARSE_OBJ)
	@echo "creating library $@..."
	$(AR) rc $@ $^
	$(RANLIB) $@


LIBRESPARSE_OTHER_SRC := \
	$(libresparse_dir)/testres.c

$(libresparse_dir)/testres: $(libresparse_dir)/testres.o \
		$(LIBRESPARSE)
	@echo "linking $@..."
	$(CC) $(CFLAGS) -o $@ $^ $(RESOLVLIBS) $(SOCKLIBS)

test: $(libresparse_dir)/testres


SRC += $(LIBRESPARSE_SRC) $(LIBRESPARSE_OTHER_SRC)
HEADERS += $(LIBRESPARSE_HEADERS)
MISCCLEAN += $(LIBRESPARSE_MISCCLEAN)

# Files for distributions:
LIBRESPARSE_DIST := $(LIBRESPARSE_SRC) $(LIBRESPARSE_OTHER_SRC) \
	$(LIBRESPARSE_HEADERS) $(LIBRESPARSE_MISC)
