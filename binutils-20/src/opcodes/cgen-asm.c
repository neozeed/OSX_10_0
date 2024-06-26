/* CGEN generic assembler support code.

   Copyright (C) 1996, 1997, 1998 Free Software Foundation, Inc.

   This file is part of the GNU Binutils and GDB, the GNU debugger.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License along
   with this program; if not, write to the Free Software Foundation, Inc.,
   59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.  */

#include "sysdep.h"
#include <stdio.h>
#include <ctype.h>
#include "ansidecl.h"
#include "libiberty.h"
#include "bfd.h"
#include "symcat.h"
#include "opcode/cgen.h"
#include "opintl.h"

/* Set the cgen_parse_operand_fn callback.  */

void
cgen_set_parse_operand_fn (cd, fn)
     CGEN_CPU_DESC cd;
     cgen_parse_operand_fn fn;
{
  cd->parse_operand_fn = fn;
}

/* Called whenever starting to parse an insn.  */

void
cgen_init_parse_operand (cd)
     CGEN_CPU_DESC cd;
{
  /* This tells the callback to re-initialize.  */
  (void) (* cd->parse_operand_fn)
    (cd, CGEN_PARSE_OPERAND_INIT, NULL, 0, 0, NULL, NULL);
}

/* Subroutine of build_asm_hash_table to add INSNS to the hash table.

   COUNT is the number of elements in INSNS.
   ENTSIZE is sizeof (CGEN_IBASE) for the target.
   ??? No longer used but leave in for now.
   HTABLE points to the hash table.
   HENTBUF is a pointer to sufficiently large buffer of hash entries.
   The result is a pointer to the next entry to use.

   The table is scanned backwards as additions are made to the front of the
   list and we want earlier ones to be prefered.  */

static CGEN_INSN_LIST *
hash_insn_array (cd, insns, count, entsize, htable, hentbuf)
     CGEN_CPU_DESC cd;
     const CGEN_INSN *insns;
     int count;
     int entsize ATTRIBUTE_UNUSED;
     CGEN_INSN_LIST **htable;
     CGEN_INSN_LIST *hentbuf;
{
  int i;

  for (i = count - 1; i >= 0; --i, ++hentbuf)
    {
      unsigned int hash;
      const CGEN_INSN *insn = &insns[i];

      if (! (* cd->asm_hash_p) (insn))
	continue;
      hash = (* cd->asm_hash) (CGEN_INSN_MNEMONIC (insn));
      hentbuf->next = htable[hash];
      hentbuf->insn = insn;
      htable[hash] = hentbuf;
    }

  return hentbuf;
}

/* Subroutine of build_asm_hash_table to add INSNS to the hash table.
   This function is identical to hash_insn_array except the insns are
   in a list.  */

static CGEN_INSN_LIST *
hash_insn_list (cd, insns, htable, hentbuf)
     CGEN_CPU_DESC cd;
     const CGEN_INSN_LIST *insns;
     CGEN_INSN_LIST **htable;
     CGEN_INSN_LIST *hentbuf;
{
  const CGEN_INSN_LIST *ilist;

  for (ilist = insns; ilist != NULL; ilist = ilist->next, ++ hentbuf)
    {
      unsigned int hash;

      if (! (* cd->asm_hash_p) (ilist->insn))
	continue;
      hash = (* cd->asm_hash) (CGEN_INSN_MNEMONIC (ilist->insn));
      hentbuf->next = htable[hash];
      hentbuf->insn = ilist->insn;
      htable[hash] = hentbuf;
    }

  return hentbuf;
}

/* Build the assembler instruction hash table.  */

static void
build_asm_hash_table (cd)
     CGEN_CPU_DESC cd;
{
  int count = cgen_insn_count (cd) + cgen_macro_insn_count (cd);
  CGEN_INSN_TABLE *insn_table = &cd->insn_table;
  CGEN_INSN_TABLE *macro_insn_table = &cd->macro_insn_table;
  unsigned int hash_size = cd->asm_hash_size;
  CGEN_INSN_LIST *hash_entry_buf;
  CGEN_INSN_LIST **asm_hash_table;
  CGEN_INSN_LIST *asm_hash_table_entries;

  /* The space allocated for the hash table consists of two parts:
     the hash table and the hash lists.  */

  asm_hash_table = (CGEN_INSN_LIST **)
    xmalloc (hash_size * sizeof (CGEN_INSN_LIST *));
  memset (asm_hash_table, 0, hash_size * sizeof (CGEN_INSN_LIST *));
  asm_hash_table_entries = hash_entry_buf = (CGEN_INSN_LIST *)
    xmalloc (count * sizeof (CGEN_INSN_LIST));

  /* Add compiled in insns.
     Don't include the first one as it is a reserved entry.  */
  /* ??? It was the end of all hash chains, and also the special
     "invalid insn" marker.  May be able to do it differently now.  */

  hash_entry_buf = hash_insn_array (cd,
				    insn_table->init_entries + 1,
				    insn_table->num_init_entries - 1,
				    insn_table->entry_size,
				    asm_hash_table, hash_entry_buf);

  /* Add compiled in macro-insns.  */

  hash_entry_buf = hash_insn_array (cd, macro_insn_table->init_entries,
				    macro_insn_table->num_init_entries,
				    macro_insn_table->entry_size,
				    asm_hash_table, hash_entry_buf);

  /* Add runtime added insns.
     Later added insns will be prefered over earlier ones.  */

  hash_entry_buf = hash_insn_list (cd, insn_table->new_entries,
				   asm_hash_table, hash_entry_buf);

  /* Add runtime added macro-insns.  */

  hash_insn_list (cd, macro_insn_table->new_entries,
		  asm_hash_table, hash_entry_buf);

  cd->asm_hash_table = asm_hash_table;
  cd->asm_hash_table_entries = asm_hash_table_entries;
}

/* Return the first entry in the hash list for INSN.  */

CGEN_INSN_LIST *
cgen_asm_lookup_insn (cd, insn)
     CGEN_CPU_DESC cd;
     const char *insn;
{
  unsigned int hash;

  if (cd->asm_hash_table == NULL)
    build_asm_hash_table (cd);

  hash = (* cd->asm_hash) (insn);
  return cd->asm_hash_table[hash];
}

/* Keyword parser.
   The result is NULL upon success or an error message.
   If successful, *STRP is updated to point passed the keyword.

   ??? At present we have a static notion of how to pick out a keyword.
   Later we can allow a target to customize this if necessary [say by
   recording something in the keyword table].  */

const char *
cgen_parse_keyword (cd, strp, keyword_table, valuep)
     CGEN_CPU_DESC cd ATTRIBUTE_UNUSED;
     const char **strp;
     CGEN_KEYWORD *keyword_table;
     long *valuep;
{
  const CGEN_KEYWORD_ENTRY *ke;
  char buf[256];
  const char *p,*start;

  p = start = *strp;

  /* Allow any first character.
     Note that this allows recognizing ",a" for the annul flag in sparc
     even though "," is subsequently not a valid keyword char.  */
  if (*p)
    ++p;

  /* Now allow letters, digits, and _.  */
  while (((p - start) < (int) sizeof (buf))
	 && (isalnum ((unsigned char) *p) || *p == '_'))
    ++p;

  if (p - start >= (int) sizeof (buf))
    return _("unrecognized keyword/register name");

  memcpy (buf, start, p - start);
  buf[p - start] = 0;

  ke = cgen_keyword_lookup_name (keyword_table, buf);

  if (ke != NULL)
    {
      *valuep = ke->value;
      /* Don't advance pointer if we recognized the null keyword.  */
      if (ke->name[0] != 0)
	*strp = p;
      return NULL;
    }

  return "unrecognized keyword/register name";
}

/* Parse a small signed integer parser.
   ??? VALUEP is not a bfd_vma * on purpose, though this is confusing.
   Note that if the caller expects a bfd_vma result, it should call
   cgen_parse_address.  */

const char *
cgen_parse_signed_integer (cd, strp, opindex, valuep)
     CGEN_CPU_DESC cd;
     const char **strp;
     int opindex;
     long *valuep;
{
  bfd_vma value;
  enum cgen_parse_operand_result result;
  const char *errmsg;

  errmsg = (* cd->parse_operand_fn)
    (cd, CGEN_PARSE_OPERAND_INTEGER, strp, opindex, BFD_RELOC_NONE,
     &result, &value);
  /* FIXME: Examine `result'.  */
  if (!errmsg)
    *valuep = value;
  return errmsg;
}

/* Parse a small unsigned integer parser.
   ??? VALUEP is not a bfd_vma * on purpose, though this is confusing.
   Note that if the caller expects a bfd_vma result, it should call
   cgen_parse_address.  */

const char *
cgen_parse_unsigned_integer (cd, strp, opindex, valuep)
     CGEN_CPU_DESC cd;
     const char **strp;
     int opindex;
     unsigned long *valuep;
{
  bfd_vma value;
  enum cgen_parse_operand_result result;
  const char *errmsg;

  errmsg = (* cd->parse_operand_fn)
    (cd, CGEN_PARSE_OPERAND_INTEGER, strp, opindex, BFD_RELOC_NONE,
     &result, &value);
  /* FIXME: Examine `result'.  */
  if (!errmsg)
    *valuep = value;
  return errmsg;
}

/* Address parser.  */

const char *
cgen_parse_address (cd, strp, opindex, opinfo, resultp, valuep)
     CGEN_CPU_DESC cd;
     const char **strp;
     int opindex;
     int opinfo;
     enum cgen_parse_operand_result *resultp;
     bfd_vma *valuep;
{
  bfd_vma value;
  enum cgen_parse_operand_result result_type;
  const char *errmsg;

  errmsg = (* cd->parse_operand_fn)
    (cd, CGEN_PARSE_OPERAND_ADDRESS, strp, opindex, opinfo,
     &result_type, &value);
  /* FIXME: Examine `result'.  */
  if (!errmsg)
    {
      if (resultp != NULL)
	*resultp = result_type;
      *valuep = value;
    }
  return errmsg;
}

/* Signed integer validation routine.  */

const char *
cgen_validate_signed_integer (value, min, max)
     long value, min, max;
{
  if (value < min || value > max)
    {
      static char buf[100];

      /* xgettext:c-format */
      sprintf (buf, _("operand out of range (%ld not between %ld and %ld)"),
		      value, min, max);
      return buf;
    }

  return NULL;
}

/* Unsigned integer validation routine.
   Supplying `min' here may seem unnecessary, but we also want to handle
   cases where min != 0 (and max > LONG_MAX).  */

const char *
cgen_validate_unsigned_integer (value, min, max)
     unsigned long value, min, max;
{
  if (value < min || value > max)
    {
      static char buf[100];

      /* xgettext:c-format */
      sprintf (buf, _("operand out of range (%lu not between %lu and %lu)"),
	       value, min, max);
      return buf;
    }

  return NULL;
}
