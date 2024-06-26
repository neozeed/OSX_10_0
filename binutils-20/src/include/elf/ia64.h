/* IA-64 ELF support for BFD.
   Copyright (C) 1998, 1999, 2000 Free Software Foundation, Inc.
   Contributed by David Mosberger-Tang <davidm@hpl.hp.com>

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


#ifndef _ELF_IA64_H
#define _ELF_IA64_H

/* Bits in the e_flags field of the Elf64_Ehdr:  */

#define EF_IA_64_MASKOS	 0x0000000f	/* os-specific flags */
#define EF_IA_64_ARCH	 0xff000000	/* arch. version mask */

/* ??? These four definitions are not part of the SVR4 ABI.
   They were present in David's initial code drop, so it is probable
   that they are used by HP/UX.  */
#define EF_IA_64_TRAPNIL (1 << 0)	/* trap NIL pointer dereferences */
#define EF_IA_64_EXT	 (1 << 2)	/* program uses arch. extensions */
#define EF_IA_64_BE	 (1 << 3)	/* PSR BE bit set (big-endian) */
#define EFA_IA_64_EAS2_3 0x23000000	/* ia64 EAS 2.3 */

#define EF_IA_64_ABI64		    (1 << 4) /* 64-bit ABI */
/* Not used yet.  */
#define EF_IA_64_REDUCEDFP	    (1 << 5) /* Only FP6-FP11 used.  */
#define EF_IA_64_CONS_GP	    (1 << 6) /* gp as program wide constant. */
#define EF_IA_64_NOFUNCDESC_CONS_GP (1 << 7) /* And no function descriptors. */
/* Not used yet.  */
#define EF_IA_64_ABSOLUTE	    (1 << 8) /* Load at absolute addresses.  */

#define ELF_STRING_ia64_archext		".IA_64.archext"
#define ELF_STRING_ia64_pltoff		".IA_64.pltoff"
#define ELF_STRING_ia64_unwind		".IA_64.unwind"
#define ELF_STRING_ia64_unwind_info	".IA_64.unwind_info"

/* Bits in the sh_flags field of Elf64_Shdr:  */

#define SHF_IA_64_SHORT		0x10000000	/* section near gp */
#define SHF_IA_64_NORECOV	0x20000000	/* spec insns w/o recovery */

/* Possible values for sh_type in Elf64_Shdr: */

#define SHT_IA_64_EXT		(SHT_LOPROC + 0)	/* extension bits */
#define SHT_IA_64_UNWIND	(SHT_LOPROC + 1)	/* unwind bits */

/* Bits in the p_flags field of Elf64_Phdr:  */

#define PF_IA_64_NORECOV	0x80000000

/* Possible values for p_type in Elf64_Phdr:  */

#define PT_IA_64_ARCHEXT	(PT_LOPROC + 0)	/* arch extension bits */
#define PT_IA_64_UNWIND 	(PT_LOPROC + 1)	/* ia64 unwind bits */

/* Possible values for d_tag in Elf64_Dyn:  */

#define DT_IA_64_PLT_RESERVE	(DT_LOPROC + 0)

/* ia64-specific relocation types: */

/* Relocs apply to specific instructions within a bundle.  The least
   significant 2 bits of the address indicate which instruction in the
   bundle the reloc refers to (0=first slot, 1=second slow, 2=third
   slot, 3=undefined) and the remaining bits give the address of the
   bundle (16 byte aligned).

   The top 5 bits of the reloc code specifies the expression type, the
   low 3 bits the format of the data word being relocated.

   ??? Relocations below marked ## are not part of the SVR4 processor
   suppliment.  They were present in David's initial code drop, so it
   is possible that they are used by HP/UX.  */

#include "elf/reloc-macros.h"

START_RELOC_NUMBERS (elf_ia64_reloc_type)
  RELOC_NUMBER (R_IA64_NONE, 0x00)	/* none */

  RELOC_NUMBER (R_IA64_IMM14, 0x21)	/* symbol + addend, add imm14 */
  RELOC_NUMBER (R_IA64_IMM22, 0x22)	/* symbol + addend, add imm22 */
  RELOC_NUMBER (R_IA64_IMM64, 0x23)	/* symbol + addend, mov imm64 */
  RELOC_NUMBER (R_IA64_DIR32MSB, 0x24)	/* symbol + addend, data4 MSB */
  RELOC_NUMBER (R_IA64_DIR32LSB, 0x25)	/* symbol + addend, data4 LSB */
  RELOC_NUMBER (R_IA64_DIR64MSB, 0x26)	/* symbol + addend, data8 MSB */
  RELOC_NUMBER (R_IA64_DIR64LSB, 0x27)	/* symbol + addend, data8 LSB */

  RELOC_NUMBER (R_IA64_GPREL22, 0x2a)	/* @gprel(sym + add), add imm22 */
  RELOC_NUMBER (R_IA64_GPREL64I, 0x2b)	/* @gprel(sym + add), mov imm64 */
  RELOC_NUMBER (R_IA64_GPREL32MSB, 0x2c) /* @gprel(sym + add), data4 MSB ## */
  RELOC_NUMBER (R_IA64_GPREL32LSB, 0x2d) /* @gprel(sym + add), data4 LSB ## */
  RELOC_NUMBER (R_IA64_GPREL64MSB, 0x2e) /* @gprel(sym + add), data8 MSB */
  RELOC_NUMBER (R_IA64_GPREL64LSB, 0x2f) /* @gprel(sym + add), data8 LSB */

  RELOC_NUMBER (R_IA64_LTOFF22, 0x32)	/* @ltoff(sym + add), add imm22 */
  RELOC_NUMBER (R_IA64_LTOFF64I, 0x33)	/* @ltoff(sym + add), mov imm64 */

  RELOC_NUMBER (R_IA64_PLTOFF22, 0x3a)	/* @pltoff(sym + add), add imm22 */
  RELOC_NUMBER (R_IA64_PLTOFF64I, 0x3b)	/* @pltoff(sym + add), mov imm64 */
  RELOC_NUMBER (R_IA64_PLTOFF64MSB, 0x3e) /* @pltoff(sym + add), data8 MSB */
  RELOC_NUMBER (R_IA64_PLTOFF64LSB, 0x3f) /* @pltoff(sym + add), data8 LSB */

  RELOC_NUMBER (R_IA64_FPTR64I, 0x43)	/* @fptr(sym + add), mov imm64 */
  RELOC_NUMBER (R_IA64_FPTR32MSB, 0x44)	/* @fptr(sym + add), data4 MSB */
  RELOC_NUMBER (R_IA64_FPTR32LSB, 0x45)	/* @fptr(sym + add), data4 LSB */
  RELOC_NUMBER (R_IA64_FPTR64MSB, 0x46)	/* @fptr(sym + add), data8 MSB */
  RELOC_NUMBER (R_IA64_FPTR64LSB, 0x47)	/* @fptr(sym + add), data8 LSB */

  RELOC_NUMBER (R_IA64_PCREL60B, 0x48)	/* @pcrel(sym + add), brl */
  RELOC_NUMBER (R_IA64_PCREL21B, 0x49)	/* @pcrel(sym + add), ptb, call */
  RELOC_NUMBER (R_IA64_PCREL21M, 0x4a)	/* @pcrel(sym + add), chk.s */
  RELOC_NUMBER (R_IA64_PCREL21F, 0x4b)	/* @pcrel(sym + add), fchkf */
  RELOC_NUMBER (R_IA64_PCREL32MSB, 0x4c) /* @pcrel(sym + add), data4 MSB */
  RELOC_NUMBER (R_IA64_PCREL32LSB, 0x4d) /* @pcrel(sym + add), data4 LSB */
  RELOC_NUMBER (R_IA64_PCREL64MSB, 0x4e) /* @pcrel(sym + add), data8 MSB */
  RELOC_NUMBER (R_IA64_PCREL64LSB, 0x4f) /* @pcrel(sym + add), data8 LSB */

  RELOC_NUMBER (R_IA64_LTOFF_FPTR22, 0x52) /* @ltoff(@fptr(s+a)), imm22 */
  RELOC_NUMBER (R_IA64_LTOFF_FPTR64I, 0x53) /* @ltoff(@fptr(s+a)), imm64 */
  RELOC_NUMBER (R_IA64_LTOFF_FPTR64MSB, 0x56) /* @ltoff(@fptr(s+a)), 8 MSB ##*/
  RELOC_NUMBER (R_IA64_LTOFF_FPTR64LSB, 0x57) /* @ltoff(@fptr(s+a)), 8 LSB ##*/

  RELOC_NUMBER (R_IA64_SEGBASE, 0x58)	/* set segment base for @segrel ## */
  RELOC_NUMBER (R_IA64_SEGREL32MSB, 0x5c) /* @segrel(sym + add), data4 MSB */
  RELOC_NUMBER (R_IA64_SEGREL32LSB, 0x5d) /* @segrel(sym + add), data4 LSB */
  RELOC_NUMBER (R_IA64_SEGREL64MSB, 0x5e) /* @segrel(sym + add), data8 MSB */
  RELOC_NUMBER (R_IA64_SEGREL64LSB, 0x5f) /* @segrel(sym + add), data8 LSB */

  RELOC_NUMBER (R_IA64_SECREL32MSB, 0x64) /* @secrel(sym + add), data4 MSB */
  RELOC_NUMBER (R_IA64_SECREL32LSB, 0x65) /* @secrel(sym + add), data4 LSB */
  RELOC_NUMBER (R_IA64_SECREL64MSB, 0x66) /* @secrel(sym + add), data8 MSB */
  RELOC_NUMBER (R_IA64_SECREL64LSB, 0x67) /* @secrel(sym + add), data8 LSB */

  RELOC_NUMBER (R_IA64_REL32MSB, 0x6c)	/* data 4 + REL */
  RELOC_NUMBER (R_IA64_REL32LSB, 0x6d)	/* data 4 + REL */
  RELOC_NUMBER (R_IA64_REL64MSB, 0x6e)	/* data 8 + REL */
  RELOC_NUMBER (R_IA64_REL64LSB, 0x6f)	/* data 8 + REL */

  RELOC_NUMBER (R_IA64_LTV32MSB, 0x70)	/* symbol + addend, data4 MSB */
  RELOC_NUMBER (R_IA64_LTV32LSB, 0x71)	/* symbol + addend, data4 LSB */
  RELOC_NUMBER (R_IA64_LTV64MSB, 0x72)	/* symbol + addend, data8 MSB */
  RELOC_NUMBER (R_IA64_LTV64LSB, 0x73)	/* symbol + addend, data8 LSB */

  RELOC_NUMBER (R_IA64_PCREL21BI, 0x79)	/* @pcrel(sym + add), ptb, call */
  RELOC_NUMBER (R_IA64_PCREL22, 0x7a)	/* @pcrel(sym + add), imm22 */
  RELOC_NUMBER (R_IA64_PCREL64I, 0x7b)	/* @pcrel(sym + add), imm64 */

  RELOC_NUMBER (R_IA64_IPLTMSB, 0x80)	/* dynamic reloc, imported PLT, MSB */
  RELOC_NUMBER (R_IA64_IPLTLSB, 0x81)	/* dynamic reloc, imported PLT, LSB */
  RELOC_NUMBER (R_IA64_EPLTMSB, 0x82)	/* dynamic reloc, exported PLT, ## */
  RELOC_NUMBER (R_IA64_EPLTLSB, 0x83)	/* dynamic reloc, exported PLT, ## */
  RELOC_NUMBER (R_IA64_COPY, 0x84)	/* dynamic reloc, data copy ## */
  RELOC_NUMBER (R_IA64_LTOFF22X, 0x86)  /* LTOFF22, relaxable.  */
  RELOC_NUMBER (R_IA64_LDXMOV, 0x87)	/* Use of LTOFF22X.  */

  RELOC_NUMBER (R_IA64_TPREL22, 0x92)	 /* sym-TP+add, add imm22 ## */
  RELOC_NUMBER (R_IA64_TPREL64MSB, 0x96) /* sym-TP+add, data8 MSB ## */
  RELOC_NUMBER (R_IA64_TPREL64LSB, 0x97) /* sym-TP+add, data8 LSB ## */

  RELOC_NUMBER (R_IA64_LTOFF_TP22, 0x9a) /* @ltoff(sym-TP+add), add imm22 ## */

  FAKE_RELOC (R_IA64_MAX_RELOC_CODE, 0x9a)
END_RELOC_NUMBERS (R_IA64_max)

#endif /* _ELF_IA64_H */
