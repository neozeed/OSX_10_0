/* BFD back-end for raw ARM a.out binaries.
   Copyright (C) 1994, 95, 96, 97, 98, 1999 Free Software Foundation, Inc.
   Contributed by Richard Earnshaw (rwe@pegasus.esprit.ec.org)

This file is part of BFD, the Binary File Descriptor library.

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

#define N_TXTADDR(x) \
  ((N_MAGIC(x) == NMAGIC) ? 0x8000 : \
   (N_MAGIC(x) != ZMAGIC) ? 0 : \
   (N_SHARED_LIB(x)) ? ((x).a_entry & ~(TARGET_PAGE_SIZE - 1)) : \
   TEXT_START_ADDR)

#define TEXT_START_ADDR 0x8000
#define TARGET_PAGE_SIZE 0x8000
#define SEGMENT_SIZE TARGET_PAGE_SIZE
#define DEFAULT_ARCH bfd_arch_arm

#define MY(OP) CAT(aoutarm_,OP)
#define N_BADMAG(x) ((((x).a_info & ~007200) != ZMAGIC) && \
                     (((x).a_info & ~006000) != OMAGIC) && \
                     ((x).a_info != NMAGIC))
#define N_MAGIC(x) ((x).a_info & ~07200)

#include "bfd.h"
#include "libbfd.h"

#define MYARM(OP) CAT(aoutarm_,OP)
reloc_howto_type 	*MYARM(bfd_reloc_type_lookup)
				PARAMS((bfd *, bfd_reloc_code_real_type));
static boolean 		MYARM(write_object_contents)	PARAMS((bfd *));

/* Avoid multiple defininitions from aoutx if supporting standarad a.out
   as well as our own.  */
#define NAME(x,y) CAT3(aoutarm,_32_,y)

#define MY_bfd_reloc_type_lookup aoutarm_bfd_reloc_type_lookup

#include "libaout.h"
#include "aout/aout64.h"

static bfd_reloc_status_type
MY(fix_pcrel_26_done)	PARAMS ((bfd *, arelent *, asymbol *, PTR,
				 asection *, bfd *, char **));

static bfd_reloc_status_type
MY(fix_pcrel_26)	 PARAMS ((bfd *, arelent *, asymbol *, PTR,
				  asection *, bfd *, char **));
static void MY(swap_std_reloc_in) PARAMS ((bfd *, struct reloc_std_external *,
					   arelent *, asymbol **,
					   bfd_size_type));
void MY(swap_std_reloc_out) PARAMS ((bfd *, arelent *,
				     struct reloc_std_external *));

reloc_howto_type MY(howto_table)[] =
{
  /* type rs size bsz pcrel bitpos ovrf sf name part_inpl readmask setmask
     pcdone */
  HOWTO (0, 0, 0, 8, false, 0, complain_overflow_bitfield, 0, "8", true,
	 0x000000ff, 0x000000ff, false),
  HOWTO (1, 0, 1, 16, false, 0, complain_overflow_bitfield, 0, "16", true,
	 0x0000ffff, 0x0000ffff, false),
  HOWTO (2, 0, 2, 32, false, 0, complain_overflow_bitfield, 0, "32", true,
	 0xffffffff, 0xffffffff, false),
  HOWTO (3, 2, 2, 26, true, 0, complain_overflow_signed, MY(fix_pcrel_26),
	 "ARM26", true, 0x00ffffff, 0x00ffffff, true),
  HOWTO (4, 0, 0, 8, true, 0, complain_overflow_signed, 0, "DISP8", true,
	 0x000000ff, 0x000000ff, true),
  HOWTO (5, 0, 1, 16, true, 0, complain_overflow_signed, 0, "DISP16", true,
	 0x0000ffff, 0x0000ffff, true),
  HOWTO (6, 0, 2, 32, true, 0, complain_overflow_signed, 0, "DISP32", true,
	 0xffffffff, 0xffffffff, true),
  HOWTO (7, 2, 2, 26, false, 0, complain_overflow_signed,
	 MY(fix_pcrel_26_done), "ARM26D", true, 0x0, 0x0,
	 false),
  EMPTY_HOWTO (-1),
  HOWTO (9, 0, -1, 16, false, 0, complain_overflow_bitfield, 0, "NEG16", true,
	 0x0000ffff, 0x0000ffff, false),
  HOWTO (10, 0, -2, 32, false, 0, complain_overflow_bitfield, 0, "NEG32", true,
	 0xffffffff, 0xffffffff, false)
};

#define RELOC_ARM_BITS_NEG_BIG      ((unsigned int) 0x08)
#define RELOC_ARM_BITS_NEG_LITTLE   ((unsigned int) 0x10)

reloc_howto_type *
MY(reloc_howto)(abfd, rel, r_index, r_extern, r_pcrel)
     bfd *abfd;
     struct reloc_std_external *rel;
     int *r_index;
     int *r_extern;
     int *r_pcrel;
{
  unsigned int r_length;
  unsigned int r_pcrel_done;
  unsigned int r_neg;
  int index;

  *r_pcrel = 0;
  if (bfd_header_big_endian (abfd))
    {
      *r_index     =  ((rel->r_index[0] << 16)
		       | (rel->r_index[1] << 8)
		       | rel->r_index[2]);
      *r_extern    = (0 != (rel->r_type[0] & RELOC_STD_BITS_EXTERN_BIG));
      r_pcrel_done = (0 != (rel->r_type[0] & RELOC_STD_BITS_PCREL_BIG));
      r_neg 	   = (0 != (rel->r_type[0] & RELOC_ARM_BITS_NEG_BIG));
      r_length     = ((rel->r_type[0] & RELOC_STD_BITS_LENGTH_BIG)
		      >> RELOC_STD_BITS_LENGTH_SH_BIG);
    }
  else
    {
      *r_index     = ((rel->r_index[2] << 16)
		      | (rel->r_index[1] << 8)
		      | rel->r_index[0]);
      *r_extern    = (0 != (rel->r_type[0] & RELOC_STD_BITS_EXTERN_LITTLE));
      r_pcrel_done = (0 != (rel->r_type[0] & RELOC_STD_BITS_PCREL_LITTLE));
      r_neg 	   = (0 != (rel->r_type[0] & RELOC_ARM_BITS_NEG_LITTLE));
      r_length     = ((rel->r_type[0] & RELOC_STD_BITS_LENGTH_LITTLE)
		      >> RELOC_STD_BITS_LENGTH_SH_LITTLE);
    }
  index = r_length + 4 * r_pcrel_done + 8 * r_neg;
  if (index == 3)
    *r_pcrel = 1;

  return MY(howto_table) + index;
}

#define MY_reloc_howto(BFD, REL, IN, EX, PC) \
	MY(reloc_howto) (BFD, REL, &IN, &EX, &PC)

void
MY(put_reloc)(abfd, r_extern, r_index, value, howto, reloc)
     bfd *abfd;
     int r_extern;
     int r_index;
     long value;
     reloc_howto_type *howto;
     struct reloc_std_external *reloc;
{
  unsigned int r_length;
  int r_pcrel;
  int r_neg;

  PUT_WORD (abfd, value, reloc->r_address);
  r_length = howto->size ;	/* Size as a power of two */

  /* Special case for branch relocations.  */
  if (howto->type == 3 || howto->type == 7)
    r_length = 3;

  r_pcrel  = howto->type & 4; 	/* PC Relative done? */
  r_neg = howto->type & 8;	/* Negative relocation */
  if (bfd_header_big_endian (abfd))
    {
      reloc->r_index[0] = r_index >> 16;
      reloc->r_index[1] = r_index >> 8;
      reloc->r_index[2] = r_index;
      reloc->r_type[0] =
	((r_extern ?     RELOC_STD_BITS_EXTERN_BIG : 0)
	 | (r_pcrel ?    RELOC_STD_BITS_PCREL_BIG : 0)
	 | (r_neg ?	 RELOC_ARM_BITS_NEG_BIG : 0)
	 | (r_length <<  RELOC_STD_BITS_LENGTH_SH_BIG));
    }
  else
    {
      reloc->r_index[2] = r_index >> 16;
      reloc->r_index[1] = r_index >> 8;
      reloc->r_index[0] = r_index;
      reloc->r_type[0] =
	((r_extern ?     RELOC_STD_BITS_EXTERN_LITTLE : 0)
	 | (r_pcrel ?    RELOC_STD_BITS_PCREL_LITTLE : 0)
	 | (r_neg ?	 RELOC_ARM_BITS_NEG_LITTLE : 0)
	 | (r_length <<  RELOC_STD_BITS_LENGTH_SH_LITTLE));
    }
}

#define MY_put_reloc(BFD, EXT, IDX, VAL, HOWTO, RELOC) \
  MY(put_reloc)(BFD, EXT, IDX, VAL, HOWTO, RELOC)

void
MY(relocatable_reloc)(howto, abfd, reloc, amount, r_addr)
     reloc_howto_type *howto;
     bfd *abfd;
     struct reloc_std_external *reloc;
     bfd_vma *amount;
     bfd_vma r_addr;
{
  if (howto->type == 3)
    {
      if (reloc->r_type[0]
	  & (bfd_header_big_endian (abfd)
	     ? RELOC_STD_BITS_EXTERN_BIG : RELOC_STD_BITS_EXTERN_LITTLE))
	{
	  /* The reloc is still external, so don't modify anything.  */
	  *amount = 0;
	}
      else
	{
	  *amount -= r_addr;
	  /* Change the r_pcrel value -- on the ARM, this bit is set once the
	     relocation is done.  */
	  if (bfd_header_big_endian (abfd))
	    reloc->r_type[0] |= RELOC_STD_BITS_PCREL_BIG;
	  else
	    reloc->r_type[0] |= RELOC_STD_BITS_PCREL_LITTLE;
	}
    }
  else if (howto->type == 7)
    *amount = 0;
}

#define MY_relocatable_reloc(HOW, BFD, REL, AMOUNT, ADDR) \
  MY(relocatable_reloc)(HOW, BFD, REL, &(AMOUNT), ADDR)

static bfd_reloc_status_type
MY(fix_pcrel_26_done) (abfd, reloc_entry, symbol, data, input_section,
		       output_bfd, error_message)
     bfd *abfd ATTRIBUTE_UNUSED;
     arelent *reloc_entry ATTRIBUTE_UNUSED;
     asymbol *symbol ATTRIBUTE_UNUSED;
     PTR data ATTRIBUTE_UNUSED;
     asection *input_section ATTRIBUTE_UNUSED;
     bfd *output_bfd ATTRIBUTE_UNUSED;
     char **error_message ATTRIBUTE_UNUSED;
{
  /* This is dead simple at present.  */
  return bfd_reloc_ok;
}

static bfd_reloc_status_type
MY(fix_pcrel_26) (abfd, reloc_entry, symbol, data, input_section,
		  output_bfd, error_message)
     bfd *abfd;
     arelent *reloc_entry;
     asymbol *symbol;
     PTR data;
     asection *input_section;
     bfd *output_bfd;
     char **error_message ATTRIBUTE_UNUSED;
{
  bfd_vma relocation;
  bfd_size_type addr = reloc_entry->address;
  long target = bfd_get_32 (abfd, (bfd_byte *) data + addr);
  bfd_reloc_status_type flag = bfd_reloc_ok;

  /* If this is an undefined symbol, return error */
  if (symbol->section == &bfd_und_section
      && (symbol->flags & BSF_WEAK) == 0)
    return output_bfd ? bfd_reloc_ok : bfd_reloc_undefined;

  /* If the sections are different, and we are doing a partial relocation,
     just ignore it for now.  */
  if (symbol->section->name != input_section->name
      && output_bfd != (bfd *)NULL)
    return bfd_reloc_ok;

  relocation = (target & 0x00ffffff) << 2;
  relocation = (relocation ^ 0x02000000) - 0x02000000; /* Sign extend */
  relocation += symbol->value;
  relocation += symbol->section->output_section->vma;
  relocation += symbol->section->output_offset;
  relocation += reloc_entry->addend;
  relocation -= input_section->output_section->vma;
  relocation -= input_section->output_offset;
  relocation -= addr;
  if (relocation & 3)
    return bfd_reloc_overflow;

  /* Check for overflow */
  if (relocation & 0x02000000)
    {
      if ((relocation & ~ (bfd_vma) 0x03ffffff) != ~ (bfd_vma) 0x03ffffff)
	flag = bfd_reloc_overflow;
    }
  else if (relocation & ~0x03ffffff)
    flag = bfd_reloc_overflow;

  target &= ~0x00ffffff;
  target |= (relocation >> 2) & 0x00ffffff;
  bfd_put_32 (abfd, target, (bfd_byte *) data + addr);

  /* Now the ARM magic... Change the reloc type so that it is marked as done.
     Strictly this is only necessary if we are doing a partial relocation.  */
  reloc_entry->howto = &MY(howto_table)[7];

  return flag;
}

reloc_howto_type *
MY(bfd_reloc_type_lookup)(abfd,code)
     bfd *abfd;
     bfd_reloc_code_real_type code;
{
#define ASTD(i,j)       case i: return &MY(howto_table)[j]
  if (code == BFD_RELOC_CTOR)
    switch (bfd_get_arch_info (abfd)->bits_per_address)
      {
      case 32:
        code = BFD_RELOC_32;
        break;
      default: return (CONST struct reloc_howto_struct *) 0;
      }

  switch (code)
    {
      ASTD (BFD_RELOC_16, 1);
      ASTD (BFD_RELOC_32, 2);
      ASTD (BFD_RELOC_ARM_PCREL_BRANCH, 3);
      ASTD (BFD_RELOC_8_PCREL, 4);
      ASTD (BFD_RELOC_16_PCREL, 5);
      ASTD (BFD_RELOC_32_PCREL, 6);
    default: return (CONST struct reloc_howto_struct *) 0;
    }
}

#define MY_swap_std_reloc_in MY(swap_std_reloc_in)
#define MY_swap_std_reloc_out MY(swap_std_reloc_out)
#define MY_get_section_contents _bfd_generic_get_section_contents
/* #define MY_bfd_link_hash_table_create _bfd_generic_link_hash_table_create */
/* #define MY_bfd_link_add_symbols _bfd_generic_link_add_symbols */
/* #define MY_bfd_final_link _bfd_generic_final_link */

#include "aoutx.h"

static void
MY_swap_std_reloc_in (abfd, bytes, cache_ptr, symbols, symcount)
     bfd *abfd;
     struct reloc_std_external *bytes;
     arelent *cache_ptr;
     asymbol **symbols;
     bfd_size_type symcount ATTRIBUTE_UNUSED;
{
  int r_index;
  int r_extern;
  int r_pcrel;
  struct aoutdata *su = &(abfd->tdata.aout_data->a);

  cache_ptr->address = bfd_h_get_32 (abfd, bytes->r_address);

  cache_ptr->howto = MY_reloc_howto (abfd, bytes, r_index, r_extern, r_pcrel);

  MOVE_ADDRESS (0);
}

void
MY_swap_std_reloc_out (abfd, g, natptr)
     bfd *abfd;
     arelent *g;
     struct reloc_std_external *natptr;
{
  int r_index;
  asymbol *sym = *(g->sym_ptr_ptr);
  int r_extern;
  int r_length;
  int r_pcrel;
  int r_neg = 0;	/* Negative relocs use the BASEREL bit.  */
  asection *output_section = sym->section->output_section;

  PUT_WORD(abfd, g->address, natptr->r_address);

  r_length = g->howto->size ;   /* Size as a power of two */
  if (r_length < 0)
    {
      r_length = -r_length;
      r_neg = 1;
    }

  r_pcrel  = (int) g->howto->pc_relative; /* Relative to PC? */

  /* For RISC iX, in pc-relative relocs the r_pcrel bit means that the
     relocation has been done already (Only for the 26-bit one I think)???!!!
     */

  if (g->howto->type == 3)
    {
      r_length = 3;
      r_pcrel = 0;
    }
  else if (g->howto->type == 7)
    {
      r_length = 3;
      r_pcrel = 1;
    }

#if 0
  /* For a standard reloc, the addend is in the object file.  */
  r_addend = g->addend + (*(g->sym_ptr_ptr))->section->output_section->vma;
#endif

  /* name was clobbered by aout_write_syms to be symbol index */

  /* If this relocation is relative to a symbol then set the
     r_index to the symbols index, and the r_extern bit.

     Absolute symbols can come in in two ways, either as an offset
     from the abs section, or as a symbol which has an abs value.
     check for that here
     */

  if (bfd_is_com_section (output_section)
      || output_section == &bfd_abs_section
      || output_section == &bfd_und_section)
    {
      if (bfd_abs_section.symbol == sym)
	{
	  /* Whoops, looked like an abs symbol, but is really an offset
	     from the abs section */
	  r_index = 0;
	  r_extern = 0;
	}
      else
	{
	  /* Fill in symbol */
	  r_extern = 1;
	  r_index = (*(g->sym_ptr_ptr))->KEEPIT;
	}
    }
  else
    {
      /* Just an ordinary section */
      r_extern = 0;
      r_index  = output_section->target_index;
    }

  /* now the fun stuff */
  if (bfd_header_big_endian (abfd))
    {
      natptr->r_index[0] = r_index >> 16;
      natptr->r_index[1] = r_index >> 8;
      natptr->r_index[2] = r_index;
      natptr->r_type[0] =
	(  (r_extern ?   RELOC_STD_BITS_EXTERN_BIG: 0)
	 | (r_pcrel  ?   RELOC_STD_BITS_PCREL_BIG: 0)
	 | (r_neg    ?   RELOC_ARM_BITS_NEG_BIG: 0)
	 | (r_length <<  RELOC_STD_BITS_LENGTH_SH_BIG));
    }
  else
    {
      natptr->r_index[2] = r_index >> 16;
      natptr->r_index[1] = r_index >> 8;
      natptr->r_index[0] = r_index;
      natptr->r_type[0] =
	(  (r_extern ?   RELOC_STD_BITS_EXTERN_LITTLE: 0)
	 | (r_pcrel  ?   RELOC_STD_BITS_PCREL_LITTLE: 0)
	 | (r_neg    ?   RELOC_ARM_BITS_NEG_LITTLE: 0)
	 | (r_length <<  RELOC_STD_BITS_LENGTH_SH_LITTLE));
    }
}

#define MY_BFD_TARGET

#include "aout-target.h"

extern const bfd_target aout_arm_big_vec;

const bfd_target aout_arm_little_vec =
{
  "a.out-arm-little",           /* name */
  bfd_target_aout_flavour,
  BFD_ENDIAN_LITTLE,            /* target byte order (little) */
  BFD_ENDIAN_LITTLE,            /* target headers byte order (little) */
  (HAS_RELOC | EXEC_P |         /* object flags */
   HAS_LINENO | HAS_DEBUG |
   HAS_SYMS | HAS_LOCALS | DYNAMIC | WP_TEXT | D_PAGED),
  (SEC_HAS_CONTENTS | SEC_ALLOC | SEC_LOAD | SEC_RELOC | SEC_CODE | SEC_DATA),
  MY_symbol_leading_char,
  AR_PAD_CHAR,                  /* ar_pad_char */
  15,                           /* ar_max_namelen */
  bfd_getl64, bfd_getl_signed_64, bfd_putl64,
     bfd_getl32, bfd_getl_signed_32, bfd_putl32,
     bfd_getl16, bfd_getl_signed_16, bfd_putl16, /* data */
  bfd_getl64, bfd_getl_signed_64, bfd_putl64,
     bfd_getl32, bfd_getl_signed_32, bfd_putl32,
     bfd_getl16, bfd_getl_signed_16, bfd_putl16, /* hdrs */
    {_bfd_dummy_target, MY_object_p, /* bfd_check_format */
       bfd_generic_archive_p, MY_core_file_p},
    {bfd_false, MY_mkobject,    /* bfd_set_format */
       _bfd_generic_mkarchive, bfd_false},
    {bfd_false, MY_write_object_contents, /* bfd_write_contents */
       _bfd_write_archive_contents, bfd_false},

     BFD_JUMP_TABLE_GENERIC (MY),
     BFD_JUMP_TABLE_COPY (MY),
     BFD_JUMP_TABLE_CORE (MY),
     BFD_JUMP_TABLE_ARCHIVE (MY),
     BFD_JUMP_TABLE_SYMBOLS (MY),
     BFD_JUMP_TABLE_RELOCS (MY),
     BFD_JUMP_TABLE_WRITE (MY),
     BFD_JUMP_TABLE_LINK (MY),
     BFD_JUMP_TABLE_DYNAMIC (MY),

  & aout_arm_big_vec,

  (PTR) MY_backend_data,
};

const bfd_target aout_arm_big_vec =
{
  "a.out-arm-big",           /* name */
  bfd_target_aout_flavour,
  BFD_ENDIAN_BIG,               /* target byte order (big) */
  BFD_ENDIAN_BIG,               /* target headers byte order (big) */
  (HAS_RELOC | EXEC_P |         /* object flags */
   HAS_LINENO | HAS_DEBUG |
   HAS_SYMS | HAS_LOCALS | DYNAMIC | WP_TEXT | D_PAGED),
  (SEC_HAS_CONTENTS | SEC_ALLOC | SEC_LOAD | SEC_RELOC | SEC_CODE | SEC_DATA),
  MY_symbol_leading_char,
  AR_PAD_CHAR,                  /* ar_pad_char */
  15,                           /* ar_max_namelen */
  bfd_getb64, bfd_getb_signed_64, bfd_putb64,
     bfd_getb32, bfd_getb_signed_32, bfd_putb32,
     bfd_getb16, bfd_getb_signed_16, bfd_putb16, /* data */
  bfd_getb64, bfd_getb_signed_64, bfd_putb64,
     bfd_getb32, bfd_getb_signed_32, bfd_putb32,
     bfd_getb16, bfd_getb_signed_16, bfd_putb16, /* hdrs */
    {_bfd_dummy_target, MY_object_p, /* bfd_check_format */
       bfd_generic_archive_p, MY_core_file_p},
    {bfd_false, MY_mkobject,    /* bfd_set_format */
       _bfd_generic_mkarchive, bfd_false},
    {bfd_false, MY_write_object_contents, /* bfd_write_contents */
       _bfd_write_archive_contents, bfd_false},

     BFD_JUMP_TABLE_GENERIC (MY),
     BFD_JUMP_TABLE_COPY (MY),
     BFD_JUMP_TABLE_CORE (MY),
     BFD_JUMP_TABLE_ARCHIVE (MY),
     BFD_JUMP_TABLE_SYMBOLS (MY),
     BFD_JUMP_TABLE_RELOCS (MY),
     BFD_JUMP_TABLE_WRITE (MY),
     BFD_JUMP_TABLE_LINK (MY),
     BFD_JUMP_TABLE_DYNAMIC (MY),

  & aout_arm_little_vec,

  (PTR) MY_backend_data,
};
