/* GDB-friendly replacement for <assert.h>.
   Copyright 2000 Free Software Foundation, Inc.

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

#ifndef GDB_ASSERT_H
#define GDB_ASSERT_H

#define gdb_assert(expr)                                                      \
  ((void) ((expr) ? 0 :                                                       \
	   (gdb_assert_fail (#expr, __FILE__, __LINE__, ASSERT_FUNCTION), 0)))

/* Version 2.4 and later of GCC define a magical variable `__PRETTY_FUNCTION__'
   which contains the name of the function currently being defined.
   This is broken in G++ before version 2.6.
   C9x has a similar variable called __func__, but prefer the GCC one since
   it demangles C++ function names.  */
#if (GCC_VERSION >= 2004)
#define ASSERT_FUNCTION		__PRETTY_FUNCTION__
#else
#if defined __STDC_VERSION__ && __STDC_VERSION__ >= 199901L
#define ASSERT_FUNCTION		__func__
#else
#define ASSERT_FUNCTION		((const char *) 0)
#endif
#endif

/* This prints an "Assertion failed" message, aksing the user if they
   want to continue, dump core, or just exit.  */
#define gdb_assert_fail(assertion, file, line, function)                      \
  internal_error ("%s:%u: %s%sAssertion `%s' failed.",                        \
		  file, line,                                                 \
		  function ? function : "", function ? ": " : "",             \
		  assertion)

#endif /* gdb_assert.h */
