/* Target-dependent code for NetBSD/i386, for GDB.
   Copyright 1988, 1989, 1991, 1992, 1994, 1996, 2000
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

#include "defs.h"
#include "gdbtypes.h"
#if 0
#include <sys/types.h>
#include <sys/ptrace.h>
#include <machine/reg.h>
#include <machine/frame.h>
#include "inferior.h"
#include "gdbcore.h" /* for registers_fetched() */
#endif

int
i386nbsd_use_struct_convention (int gcc_p, struct type *type)
{
  return !(TYPE_LENGTH (type) == 1
	   || TYPE_LENGTH (type) == 2
	   || TYPE_LENGTH (type) == 4
	   || TYPE_LENGTH (type) == 8);
}
