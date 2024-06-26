/* Parameters for CISCO m68k.
   Copyright (C) 1994 Free Software Foundation, Inc.

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

#define GDBINIT_FILENAME	".cisco-gdbinit"	/* Init file */

#define DEFAULT_PROMPT		"(cisco-68k-gdb) "	/* Default prompt */

#include "m68k/tm-m68k.h"

/* Offsets (in target ints) into jmp_buf.  Defined in /csc/sys/sun/asm.S. */

#define JB_ELEMENT_SIZE 4

#define JB_PC 0
#define JB_D2 1
#define JB_D3 2
#define JB_D4 3
#define JB_D5 4
#define JB_D6 5
#define JB_D7 6
#define JB_A2 7
#define JB_A3 8
#define JB_A4 9
#define JB_A5 10
#define JB_A6 11
#define JB_SP 12

/* Figure out where the longjmp will land.  Slurp the args out of the stack.
   We expect the first arg to be a pointer to the jmp_buf structure from which
   we extract the pc (JB_PC) that we will land at.  The pc is copied into ADDR.
   This routine returns true on success */

#define GET_LONGJMP_TARGET(ADDR) m68k_get_longjmp_target(ADDR)

/* BFD handles finding the registers in the core file, so they are at
   the start of the BFD .reg section.  */
#define REGISTER_U_ADDR(addr,blockend,regno) (addr = REGISTER_BYTE (regno))
#define KERNEL_U_ADDR 0
