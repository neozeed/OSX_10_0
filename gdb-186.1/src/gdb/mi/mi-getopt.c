/* MI Command Set - MI Option Parser.
   Copyright (C) 2000, Free Software Foundation, Inc.
   Contributed by Cygnus Solutions (a Red Hat company).

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
#include "mi-getopt.h"
#include "string.h"

int
mi_getopt (const char *prefix,
	   int argc, char **argv,
	   struct mi_opt *opts,
	   int *optind, char **optarg)
{
  char *arg;
  struct mi_opt *opt;
  /* We assume that argv/argc are ok. */
  if (*optind > argc || *optind < 0)
    internal_error ("mi_getopt_long: optind out of bounds");
  if (*optind == argc)
    return -1;
  arg = argv[*optind];
  /* ``--''? */
  if (strcmp (arg, "--") == 0)
    {
      *optind += 1;
      *optarg = NULL;
      return -1;
    }
  /* End of option list. */
  if (arg[0] != '-')
    {
      *optarg = NULL;
      return -1;
    }
  /* Look the option up. */
  for (opt = opts; opt->name != NULL; opt++)
    {
      if (strcmp (opt->name, arg + 1) != 0)
	continue;
      if (opt->arg_p)
	{
	  /* A non-simple optarg option. */
	  if (argc < *optind + 2)
	    error ("%s: Option %s requires an argument", prefix, arg);
	  *optarg = argv[(*optind) + 1];
	  *optind = (*optind) + 2;
	  return opt->index;
	}
      else
	{
	  *optarg = NULL;
	  *optind = (*optind) + 1;
	  return opt->index;
	}
    }
  error ("%s: Unknown option ``%s''", prefix, arg + 1);
}
