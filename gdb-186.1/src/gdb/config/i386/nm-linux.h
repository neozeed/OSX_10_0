/* Native support for Linux/x86.
   Copyright 1986, 1987, 1989, 1992, 1996, 1998, 2000
   Free Software Foundation, Inc.

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

#ifndef NM_LINUX_H
#define NM_LINUX_H

#include "i386/nm-i386v.h"
#include "nm-linux.h"

#include <sys/ptrace.h>
#include <asm/ptrace.h>

/* Return sizeof user struct to callers in less machine dependent routines */

#define KERNEL_U_SIZE kernel_u_size()
extern int kernel_u_size (void);

#define U_REGS_OFFSET 0

/* GNU/Linux supports the 386 hardware debugging registers.  */

#define TARGET_HAS_HARDWARE_WATCHPOINTS

#define TARGET_CAN_USE_HARDWARE_WATCHPOINT(type, cnt, ot) 1

/* After a watchpoint trap, the PC points to the instruction after
   the one that caused the trap.  Therefore we don't need to step over it.
   But we do need to reset the status register to avoid another trap.  */
#define HAVE_CONTINUABLE_WATCHPOINT

#define STOPPED_BY_WATCHPOINT(W)  \
  i386_stopped_by_watchpoint (inferior_pid)

/* Use these macros for watchpoint insertion/removal.  */

#define target_insert_watchpoint(addr, len, type)  \
  i386_insert_watchpoint (inferior_pid, addr, len, type)

#define target_remove_watchpoint(addr, len, type)  \
  i386_remove_watchpoint (inferior_pid, addr, len)

/* We define this if link.h is available, because with ELF we use SVR4 style
   shared libraries. */

#ifdef HAVE_LINK_H
#define SVR4_SHARED_LIBS
#include "solib.h"		/* Support for shared libraries. */
#endif

/* Override copies of {fetch,store}_inferior_registers in infptrace.c.  */
#define FETCH_INFERIOR_REGISTERS

/* Nevertheless, define CANNOT_{FETCH,STORE}_REGISTER, because we might fall
   back on the code `infptrace.c' (well a copy of that code in
   `i386-linux-nat.c' for now) and we can access only the
   general-purpose registers in that way.  */
extern int cannot_fetch_register (int regno);
extern int cannot_store_register (int regno);
#define CANNOT_FETCH_REGISTER(regno) cannot_store_register (regno)
#define CANNOT_STORE_REGISTER(regno) cannot_fetch_register (regno)

/* Override child_resume in `infptrace.c'.  */
#define CHILD_RESUME

extern CORE_ADDR i386_stopped_by_watchpoint (int);
extern int i386_insert_watchpoint (int pid, CORE_ADDR addr, int len, int rw);
extern int i386_remove_watchpoint (int pid, CORE_ADDR addr, int len);

/* FIXME: kettenis/2000-09-03: This should be moved to ../nm-linux.h
   once we have converted all Linux targets to use the new threads
   stuff (without the #undef of course).  */

extern int lin_lwp_prepare_to_proceed (void);
#undef PREPARE_TO_PROCEED
#define PREPARE_TO_PROCEED(select_it) lin_lwp_prepare_to_proceed ()

extern void lin_lwp_attach_lwp (int pid, int verbose);
#define ATTACH_LWP(pid, verbose) lin_lwp_attach_lwp ((pid), (verbose))

#include <signal.h>

extern void lin_thread_get_thread_signals (sigset_t *mask);
#define GET_THREAD_SIGNALS(mask) lin_thread_get_thread_signals (mask)

#endif /* nm_linux.h */
