/* Native-dependent definitions for FreeBSD/i386.
   Copyright (C) 1986, 87, 89, 92, 96, 2000 Free Software Foundation, Inc.

   This file is part of GDB.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.  */

#ifndef NM_FBSD_H
#define NM_FBSD_H

/* Type of the third argument to the `ptrace' system call.  */
#define PTRACE_ARG3_TYPE caddr_t

/* Override copies of {fetch,store}_inferior_registers in `infptrace.c'.  */
#define FETCH_INFERIOR_REGISTERS

/* We can attach and detach.  */
#define ATTACH_DETACH


/* Support for the user struct.  */

/* Return the size of the user struct.  */

#define KERNEL_U_SIZE kernel_u_size ()
extern int kernel_u_size (void);

/* This is the amount to subtract from u.u_ar0
   to get the offset in the core file of the register values.  */

#include <machine/vmparam.h>
#define KERNEL_U_ADDR USRSTACK

#define REGISTER_U_ADDR(addr, blockend, regno) \
  (addr) = register_u_addr ((blockend), (regno))
extern CORE_ADDR register_u_addr (CORE_ADDR blockend, int regno);


/* Shared library support.  */

/* The FreeBSD <link.h> uses the same condition to distinguish ELF
   from a.out.  ELF implies SVR4 shared libraries.  */
#if (defined (FREEBSD_ELF) || defined (__ELF__)) && !defined (FREEBSD_AOUT)
#define SVR4_SHARED_LIBS
#endif

#include "solib.h"		/* Support for shared libraries. */
#ifdef SVR4_SHARED_LIBS
#include "elf/common.h"		/* Additional ELF shared library info. */
#endif

extern int i386_register_u_addr (int, int);

#ifndef SVR4_SHARED_LIBS

/* Make structure definitions match up with those expected in `solib.c'.  */

#define link_object	sod
#define lo_name		sod_name
#define lo_library	sod_library
#define lo_unused	sod_reserved
#define lo_major	sod_major
#define lo_minor	sod_minor
#define lo_next		sod_next

#define link_map	so_map
#define lm_addr		som_addr
#define lm_name		som_path
#define lm_next		som_next
#define lm_lop		som_sod
#define lm_lob		som_sodbase
#define lm_rwt		som_write
#define lm_ld		som_dynamic
#define lm_lpd		som_spd

#define link_dynamic_2	section_dispatch_table
#define ld_loaded	sdt_loaded
#define ld_need		sdt_sods
#define ld_rules	sdt_filler1
#define ld_got		sdt_got
#define ld_plt		sdt_plt
#define ld_rel		sdt_rel
#define ld_hash		sdt_hash
#define ld_stab		sdt_nzlist
#define ld_stab_hash	sdt_filler2
#define ld_buckets	sdt_buckets
#define ld_symbols	sdt_strings
#define ld_symb_size	sdt_str_sz
#define ld_text		sdt_text_sz
#define ld_plt_sz	sdt_plt_sz

#define rtc_symb	rt_symbol
#define rtc_sp		rt_sp
#define rtc_next	rt_next

#define ld_debug	so_debug
#define ldd_version	dd_version
#define ldd_in_debugger	dd_in_debugger
#define ldd_sym_loaded	dd_sym_loaded
#define ldd_bp_addr	dd_bpt_addr
#define ldd_bp_inst	dd_bpt_shadow
#define ldd_cp		dd_cc

#define link_dynamic	_dynamic
#define ld_version	d_version
#define ldd		d_debug
#define ld_un		d_un
#define ld_2		d_sdt

/* Return sizeof user struct to callers in less machine dependent routines */

#define KERNEL_U_SIZE kernel_u_size()
extern int kernel_u_size (void);

#endif /* !SVR4_SHARED_LIBS */

#endif /* NM_FBSD_H */
