/* PEF symbol-table support for Mach, for GDB.
   Copyright 1997  Free Software Foundation, Inc.
   Written by Klee Dienes.  Contributed by Apple Computer, Inc.

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
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.  */

#include "defs.h"
#include "symtab.h"
#include "gdbtypes.h"
#include "breakpoint.h"
#include "bfd.h"
#include "symfile.h"
#include "objfiles.h"
#include "buildsym.h"
#include "obstack.h"
#include "gdb-stabs.h"
#include "stabsread.h"
#include "gdbcmd.h"

#include "mach-o.h"

#include <string.h>

struct pef_symfile_info {
  asymbol **syms;
  long nsyms;
};

static void
pef_new_init (objfile)
     struct objfile *objfile;
{
}

static void
pef_symfile_init (objfile)
     struct objfile *objfile;
{
  objfile->sym_stab_info = 
    xmmalloc (objfile->md, sizeof (struct dbx_symfile_info));

  memset ((PTR) objfile->sym_stab_info, 0, sizeof (struct dbx_symfile_info));

  objfile->sym_private = 
    xmmalloc (objfile->md,sizeof (struct pef_symfile_info));

  memset (objfile->sym_private, 0, sizeof (struct pef_symfile_info));

  objfile->flags |= OBJF_REORDERED;
  init_entry_point_info (objfile);
}

static void
pef_symfile_read (objfile, mainline)
     struct objfile *objfile;
     int mainline;
{
  bfd *abfd = objfile->obfd;
  size_t storage_needed;
  size_t number_of_symbols;
  asymbol **symbol_table;
  asymbol *sym;
  struct cleanup *back_to;
  CORE_ADDR symaddr;
  CORE_ADDR firstaddr = 0;
  size_t i;
  enum minimal_symbol_type ms_type;

  int ret;

  CHECK_FATAL (objfile != NULL);
  CHECK_FATAL (abfd != NULL);
  CHECK_FATAL (abfd->filename != NULL);

  init_minimal_symbol_collection ();
  make_cleanup_discard_minimal_symbols ();
    
  storage_needed = bfd_get_symtab_upper_bound (abfd);

  firstaddr = (CORE_ADDR) -1;
  if (storage_needed > 0)
    {
      symbol_table = (asymbol **) xmalloc (storage_needed);
      back_to = make_cleanup (free, (PTR) symbol_table);
      number_of_symbols = bfd_canonicalize_symtab (abfd, symbol_table);

      for (i = 0; i < number_of_symbols; i++)
        {
          sym = symbol_table[i];

	  symaddr = sym->section->vma + sym->value;
	  symaddr += ANOFFSET (objfile->section_offsets, SECT_OFF_TEXT (objfile));

	  /* For non-absolute symbols, use the type of the section
	     they are relative to, to intuit text/data.  BFD provides
	     no way of figuring this out for absolute symbols. */

	  if (sym->section->flags & SEC_CODE)
	    ms_type = mst_text;
	  else if (sym->section->flags & SEC_DATA)
	    ms_type = mst_data;
	  else
	    ms_type = mst_unknown;
	  
	  if (sym->name[0] == '\0') {
	    /* warning ("ignoring symbol with empty name"); */
	    continue;
	  }

	  prim_record_minimal_symbol (sym->name, symaddr, ms_type, objfile);
	  if (firstaddr > symaddr) { firstaddr = symaddr; }
        }
      do_cleanups (back_to);
    }

  if (firstaddr > ANOFFSET (objfile->section_offsets, SECT_OFF_TEXT (objfile))) {
    prim_record_minimal_symbol ("unknown_text", ANOFFSET (objfile->section_offsets, SECT_OFF_TEXT (objfile)), mst_text, objfile);
  }

  install_minimal_symbols (objfile);
}

static void
pef_symfile_finish (objfile)
     struct objfile *objfile;
{
}

static void
pef_symfile_offsets (objfile, addrs)
     struct objfile *objfile;
     struct section_addr_info *addrs;
{
  unsigned int i;

  objfile->num_sections = SECT_OFF_MAX;
  objfile->section_offsets = (struct section_offsets *)
    obstack_alloc (&objfile->psymbol_obstack, SIZEOF_SECTION_OFFSETS);
  memset (objfile->section_offsets, 0, SIZEOF_SECTION_OFFSETS);

  for (i = 0; i < objfile->sections_end - objfile->sections; i++) {
    objfile->sections[i].addr += addrs->other[0].addr;
    objfile->sections[i].endaddr += addrs->other[0].addr;
  }

  for (i = 0; i < MAX_SECTIONS; i++) {
    objfile->section_offsets->offsets[i] = (long) addrs->other[0].addr;
  }

  objfile->sect_index_text = 0;
  objfile->sect_index_data = 0;
  objfile->sect_index_bss = 0;
  objfile->sect_index_rodata = 0;
}

static struct sym_fns pef_sym_fns =
{
  bfd_target_pef_flavour,

  pef_new_init,			/* sym_new_init: init anything gbl to entire symtab */
  pef_symfile_init,		/* sym_init: read initial info, setup for sym_read() */
  pef_symfile_read,		/* sym_read: read a symbol file into symtab */
  pef_symfile_finish,		/* sym_finish: finished with file, cleanup */
  pef_symfile_offsets,		/* sym_offsets:  xlate external to internal form */
  NULL				/* next: pointer to next struct sym_fns */
};

void
_initialize_pefread ()
{
  add_symtab_fns (&pef_sym_fns);
}
