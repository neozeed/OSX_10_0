/* Longjump free calls to gdb internal routines.
   Copyright 1999 Free Software Foundation, Inc.

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

#ifndef WRAPPER_H
#define WRAPPER_H 1

/* Use this struct to pass arguments to wrapper routines. */
struct gdb_wrapper_arguments;

extern int gdb_parse_exp_1 (char **, struct block *,
			    int, struct expression **);

extern int gdb_evaluate_expression (struct expression *, value_ptr *);

extern int gdb_value_fetch_lazy (value_ptr);

extern int gdb_value_equal (value_ptr, value_ptr, int *);

extern int gdb_value_assign (value_ptr, value_ptr, value_ptr *);

extern int gdb_value_subscript (value_ptr, value_ptr, value_ptr *);

extern int gdb_value_ind (value_ptr val, value_ptr * rval);

extern int gdb_parse_and_eval_type (char *, int, struct type **);

extern int safe_execute_command (char *command, int from_tty);
#endif /* WRAPPER_H */
