/*
 * Copyright (c) 1999 Apple Computer, Inc. All rights reserved.
 *
 * @APPLE_LICENSE_HEADER_START@
 * 
 * Portions Copyright (c) 1999 Apple Computer, Inc.  All Rights
 * Reserved.  This file contains Original Code and/or Modifications of
 * Original Code as defined in and that are subject to the Apple Public
 * Source License Version 1.1 (the "License").  You may not use this file
 * except in compliance with the License.  Please obtain a copy of the
 * License at http://www.apple.com/publicsource and read it before using
 * this file.
 * 
 * The Original Code and all software distributed under the License are
 * distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY KIND, EITHER
 * EXPRESS OR IMPLIED, AND APPLE HEREBY DISCLAIMS ALL SUCH WARRANTIES,
 * INCLUDING WITHOUT LIMITATION, ANY WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE OR NON- INFRINGEMENT.  Please see the
 * License for the specific language governing rights and limitations
 * under the License.
 * 
 * @APPLE_LICENSE_HEADER_END@
 */
#ifdef SHLIB
#include "shlib.h"
#endif SHLIB
/*
 * This file contains the routines to do relocation for SPARC processor architecture.
 */
#include <stdlib.h>
#if !(defined(KLD) && defined(__STATIC__))
#include <stdio.h>
#include <mach/mach.h>
#else /* defined(KLD) && defined(__STATIC__) */
#include <mach/kern_return.h>
#endif /* !(defined(KLD) && defined(__STATIC__)) */
#include <string.h>
#include <stdarg.h>
#include <mach-o/loader.h>
#include <mach-o/reloc.h>
#include <mach-o/sparc/reloc.h>
#include <mach-o/nlist.h>
#include "stuff/bool.h"
#include "stuff/bytesex.h"

#include "ld.h"
#include "objects.h"
#include "sections.h"
#include "pass1.h"
#include "symbols.h"
#include "pass2.h"
#include "generic_reloc.h"
#include "sparc_reloc.h"
#include "indirect_sections.h"
#include "dylibs.h"

#define U_ABS(l) (((long)(l))<0 ? (unsigned long)(-(l)) : (l))

#undef DEBUG1

/*
 * sparc_reloc() relocates the contents of the specified section for the 
 * relocation entries using the section map from the current object (cur_obj).
 */
__private_extern__
void
sparc_reloc(
char *contents,
struct relocation_info *relocs,
struct section_map *section_map)
{
    unsigned long i, j, symbolnum, value, input_pc, output_pc;
    unsigned long instruction, immediate;
    struct nlist *nlists;
    char *strings;
    enum bool force_extern_reloc;
    struct undefined_map *undefined_map;
    struct merged_symbol *merged_symbol, **hash_pointer;
    struct section_map *local_map, *pair_local_map;
    struct relocation_info *reloc, *pair_reloc;
    struct scattered_relocation_info *sreloc, *spair_reloc;
    unsigned long r_address, r_symbolnum, r_pcrel, r_length, r_extern,
		  r_scattered, r_value, pair_r_symbolnum, pair_r_value;
    enum reloc_type_sparc r_type, pair_r_type;
    unsigned long other_half;
    unsigned long offset;

#if defined(DEBUG) || defined(RLD)
	/*
	 * The compiler "warnings: ... may be used uninitialized in this
	 * function" can safely be ignored
	 */
	merged_symbol = NULL;
	local_map = NULL;
	instruction = 0;
	other_half = 0;
	immediate = 0;
	offset = 0;
	pair_r_symbolnum = 0;
	pair_r_value = 0;
	pair_local_map = NULL;
#endif defined(DEBUG) || defined(RLD)

	for(i = 0; i < section_map->s->nreloc; i++){
	    force_extern_reloc = FALSE;
	    /*
	     * Break out the fields of the relocation entry and set pointer to
	     * the type of relocation entry it is (for updating later).
	     */
	    if((relocs[i].r_address & R_SCATTERED) != 0){
		sreloc = (struct scattered_relocation_info *)(relocs + i);
		reloc = NULL;
		r_scattered = 1;
		r_address = sreloc->r_address;
		r_pcrel = sreloc->r_pcrel;
		r_length = sreloc->r_length;
		r_value = sreloc->r_value;
		r_type = (enum reloc_type_sparc)sreloc->r_type;
		r_extern = 0;
		/*
		 * Since the r_value field is reserved in a SPARC_RELOC_PAIR
		 * type to report the correct error a check for a stray
		 * SPARC_RELOC_PAIR relocation types needs to be done before
		 * it is assumed that r_value is legal.  A SPARC_RELOC_PAIR
		 * follows SPARC_RELOC_{HI22,LO10,SECTDIFF, HI22_SECTDIFF,LO10_SECTDIFF} 
		 * relocation types and it is an error to see one otherwise.
		 */
		if(r_type == SPARC_RELOC_PAIR){
		    error_with_cur_obj("stray relocation SPARC_RELOC_PAIR entry "
			"(%lu) in section (%.16s,%.16s)", i,
			section_map->s->segname, section_map->s->sectname);
		    continue;
		}
		/* calculate the r_symbolnum (n_sect) from the r_value */
		r_symbolnum = 0;
		for(j = 0; j < cur_obj->nsection_maps; j++){
		    if(r_value >= cur_obj->section_maps[j].s->addr &&
		       r_value < cur_obj->section_maps[j].s->addr +
				 cur_obj->section_maps[j].s->size){
			r_symbolnum = j + 1;
			break;
		    }
		}
		if(r_symbolnum == 0){
		    /*
		     * The edge case where the last address past then end of
		     * of the last section is referenced.
		     */
		    for(j = 0; j < cur_obj->nsection_maps; j++){
			if(r_value == cur_obj->section_maps[j].s->addr +
				      cur_obj->section_maps[j].s->size){
			    r_symbolnum = j + 1;
			    break;
			}
		    }
		    if(r_symbolnum == 0){
			error_with_cur_obj("r_value (0x%x) field of relocation "
			    "entry %lu in section (%.16s,%.16s) out of range",
			    (unsigned int)r_value, i, section_map->s->segname,
			    section_map->s->sectname);
			return;
		    }
		}
	    }
	    else{
		reloc = relocs + i;
		sreloc = NULL;
		r_scattered = 0;
		r_address = reloc->r_address;
		r_pcrel = reloc->r_pcrel;
		r_length = reloc->r_length;
		r_extern = reloc->r_extern;
		r_symbolnum = reloc->r_symbolnum;
		r_type = (enum reloc_type_sparc)reloc->r_type;
		r_value = 0;
	    }
	    /*
	     * SPARC_RELOC_PAIR relocation types only follows SPARC_RELOC_
	     * {HI22,LO10,SECTDIFFi,HI22_SECTDIFF,LO10_SECTDIFF}
	     * relocation types and it is an error to
	     * see one otherwise.
	     */
	    if(r_type == SPARC_RELOC_PAIR){
		error_with_cur_obj("stray relocation SPARC_RELOC_PAIR entry "
		    "(%lu) in section (%.16s,%.16s)", i,
		    section_map->s->segname, section_map->s->sectname);
		continue;
	    }
	    /*
	     * The r_address field is really an offset into the contents of the
	     * section and must reference something inside the section (Note
	     * that this is not the case for SPARC_RELOC_PAIR entries but this
	     * can't be one with the above checks).
	     */
	    if(r_address >= section_map->s->size){
		error_with_cur_obj("r_address (0x%x) field of relocation entry "
		    "%lu in section (%.16s,%.16s) out of range",
		    (unsigned int)r_address, i, section_map->s->segname,
		    section_map->s->sectname);
		return;
	    }
	    /*
	     * If this relocation type is to have a pair make sure it is there
	     * and then break out it's fields.
	     */
	    pair_r_type = (enum reloc_type_sparc)0;
	    pair_reloc = NULL;
	    spair_reloc = NULL;
	    if(r_type == SPARC_RELOC_HI22 || r_type == SPARC_RELOC_LO10) {
		if(i + 1 < section_map->s->nreloc){
		    pair_reloc = relocs + i + 1;
		    if((pair_reloc->r_address & R_SCATTERED) != 0){
			spair_reloc = (struct scattered_relocation_info *)
				      pair_reloc;
			pair_reloc  = NULL;
			pair_r_type = (enum reloc_type_sparc)
				      spair_reloc->r_type;
			other_half  = spair_reloc->r_address;
		    }
		    else{
			pair_r_type = (enum reloc_type_sparc)
				      pair_reloc->r_type;
			other_half  = pair_reloc->r_address;
		    }
		}
		if((pair_reloc == NULL && spair_reloc == NULL) ||
		   pair_r_type != SPARC_RELOC_PAIR){
		    error_with_cur_obj("relocation entry (%lu) in section "
			"(%.16s,%.16s) missing following associated "
			"SPARC_RELOC_PAIR entry", i, section_map->s->segname,
			section_map->s->sectname);
		    continue;
		}
	    }
	    else if(r_type == SPARC_RELOC_SECTDIFF ||
		    r_type == SPARC_RELOC_HI22_SECTDIFF ||
		    r_type == SPARC_RELOC_LO10_SECTDIFF){
		if(r_scattered != 1){
		    error_with_cur_obj("relocation entry (%lu) in section "
			"(%.16s,%.16s) r_type is SPARC_RELOC_SECTDIFF but "
			"relocation entry not scattered type", i,
			section_map->s->segname, section_map->s->sectname);
		    continue;
		}
		if(i + 1 < section_map->s->nreloc){
		    pair_reloc = relocs + i + 1;
		    if((pair_reloc->r_address & R_SCATTERED) != 0){
			spair_reloc = (struct scattered_relocation_info *)
				      pair_reloc;
			pair_reloc = NULL;
			pair_r_type = (enum reloc_type_sparc)
				      spair_reloc->r_type;
			pair_r_value = spair_reloc->r_value;
			other_half  = spair_reloc->r_address;
		    }
		    else{
			error_with_cur_obj("relocation entry (%lu) in section "
			    "(%.16s,%.16s) following associated relocation "
			    "entry not scattered type", i,
			    section_map->s->segname, section_map->s->sectname);
			continue;
		    }
		}
		if((pair_reloc == NULL && spair_reloc == NULL) ||
		   pair_r_type != SPARC_RELOC_PAIR){
		    error_with_cur_obj("relocation entry (%lu) in section "
			"(%.16s,%.16s) missing following associated "
			"SPARC_RELOC_PAIR entry", i, section_map->s->segname,
			section_map->s->sectname);
		    continue;
		}
		/*
		 * Calculate the pair_r_symbolnum (n_sect) from the
		 * pair_r_value.
		 */
		pair_r_symbolnum = 0;
		for(j = 0; j < cur_obj->nsection_maps; j++){
		    if(pair_r_value >= cur_obj->section_maps[j].s->addr &&
		       pair_r_value < cur_obj->section_maps[j].s->addr +
				 cur_obj->section_maps[j].s->size){
			pair_r_symbolnum = j + 1;
			break;
		    }
		}
		if(pair_r_symbolnum == 0){
		    error_with_cur_obj("r_value (0x%x) field of relocation "
			"entry %lu in section (%.16s,%.16s) out of range",
			(unsigned int)r_value, i + 1, section_map->s->segname,
			section_map->s->sectname);
		    return;
		}
	    }
	    /*
	     * If r_extern is set this relocation entry is an external entry
	     * else it is a local entry (or scattered entry).
	     */
	    input_pc = section_map->s->addr + r_address;

	    if(r_extern){
		/*
		 * This is an external relocation entry.  So the value to be
		 * added to the item to be relocated is the value of the symbol.
		 * r_symbolnum is an index into the input file's symbol table
		 * of the symbol being refered to.  The symbol must be an
		 * undefined or coalesced symbol to be used in an external
		 * relocation entry.
		 */
		if(r_symbolnum >= cur_obj->symtab->nsyms){
		    error_with_cur_obj("r_symbolnum (%lu) field of external "
			"relocation entry %lu in section (%.16s,%.16s) out of "
			"range", r_symbolnum, i, section_map->s->segname,
			section_map->s->sectname);
		    return;
		}
		symbolnum = r_symbolnum;
		undefined_map = bsearch(&symbolnum, cur_obj->undefined_maps,
		    cur_obj->nundefineds, sizeof(struct undefined_map),
		    (int (*)(const void *, const void *))undef_bsearch);
		if(undefined_map != NULL){
		    merged_symbol = undefined_map->merged_symbol;
		}
		else{
		    nlists = (struct nlist *)(cur_obj->obj_addr +
					      cur_obj->symtab->symoff);
		    strings = (char *)(cur_obj->obj_addr +
				       cur_obj->symtab->stroff);
		    if((nlists[symbolnum].n_type & N_EXT) != N_EXT){
			error_with_cur_obj("r_symbolnum (%lu) field of external"
			    " relocation entry %lu in section (%.16s,%.16s) "
			    "refers to a non-external symbol", symbolnum, i,
			    section_map->s->segname, section_map->s->sectname);
			return;
		    }
		    /*
		     * We must allow and create references to defined global
		     * coalesced symbols with external relocation entries so
		     * that the dynamic linker can relocate all references to
		     * the same symbol.
		     */
		    if((nlists[symbolnum].n_type & N_TYPE) == N_SECT &&
		       (cur_obj->section_maps[nlists[symbolnum].n_sect-1].
			s->flags & SECTION_TYPE) == S_COALESCED){
			hash_pointer = lookup_symbol(strings +
					     nlists[symbolnum].n_un.n_strx);
			if(hash_pointer == NULL){
			    fatal("internal error, in sparc_reloc() failed to "
			          "lookup coalesced symbol %s", strings +
				  nlists[symbolnum].n_un.n_strx);
			}
			merged_symbol = *hash_pointer;
			if(((merged_symbol->nlist.n_type & N_PEXT) == N_PEXT &&
			    keep_private_externs == FALSE) ||
			   dynamic == FALSE ||
			   (output_for_dyld && has_dynamic_linker_command))
			    force_extern_reloc = FALSE;
			else
			    force_extern_reloc = TRUE;
		    }
		    else{
			if(nlists[symbolnum].n_type != (N_EXT | N_UNDF)){
			    error_with_cur_obj("r_symbolnum (%lu) field of "
				"external relocation entry %lu in section "
				"(%.16s,%.16s) refers to a non-undefined "
				"symbol", symbolnum, i, section_map->s->segname,
				 section_map->s->sectname);
			    return;
			}
			print_obj_name(cur_obj);
			fatal("internal error, in sparc_reloc() symbol index "
			    "%lu in above file not in undefined map",symbolnum);
		    }
		}
		/*
		 * If this is an indirect symbol resolve indirection (all chains
		 * of indirect symbols have been resolved so that they point at
		 * a symbol that is not an indirect symbol).
		 */
		if(merged_symbol->nlist.n_type == (N_EXT | N_INDR))
		    merged_symbol = (struct merged_symbol *)
				    merged_symbol->nlist.n_value;
		/*
		 * If the symbol is undefined (or common) or a global coalesced 
		 * symbol where we need to force an external relocation entry
		 * and we are not prebinding no relocation is done.
		 * Or if the output file is MH_DYLIB no relocation is done
		 * unless the symbol is a private extern or we are prebinding.
		 */
		if(((merged_symbol->nlist.n_type & N_TYPE) == N_UNDF) ||
		   (force_extern_reloc == TRUE && prebinding == FALSE) ||
		   (filetype == MH_DYLIB &&
		    (((merged_symbol->nlist.n_type & N_PEXT) != N_PEXT) &&
		     prebinding == FALSE) ) )
		    value = 0;
		else{
		    value = merged_symbol->nlist.n_value;
		    /*
		     * To know which type (local or scattered) of relocation
		     * entry to convert this one to (if relocation entries are
		     * saved) the offset to be added to the symbol's value is
		     * needed to see if it reaches outside the block in which
		     * the symbol is in.  In here if the offset is not zero then
		     * it is assumed to reach out of the block and a scattered
		     * relocation entry is used.
		     */
		    if(r_type == SPARC_RELOC_VANILLA){
			switch(r_length){
			case 0: /* byte */
			    offset = get_byte((char *)(contents + r_address));
			    break;
			case 1: /* word (2 byte) */
			    offset = get_short((short *)(contents + r_address));
			    break;
			case 2: /* long (4 byte) */
			    offset = get_long((long *)(contents + r_address));
			    break;
			default:
			    /* the error check is catched below */
			    break;
			}
			if(r_pcrel)
			    offset += input_pc;
		    }
		    else{
			instruction = get_long((long *)(contents + r_address));
			switch(r_type){
			case SPARC_RELOC_HI22:
			    offset = ((instruction & 0x3fffff) << 10) |
			      (other_half & 0x3ff);
			    break;
			case SPARC_RELOC_LO10:
			    offset = (instruction & 0x3ff) | 
			      ((other_half & 0x3fffff) << 10);
			    break;
			case SPARC_RELOC_WDISP22:
			    offset = (instruction & 0x3fffff);
			    /* sign extend offset */
			    if ((offset & 0x200000) != 0)
			      offset |= 0xffc00000;
			    offset <<= 2; /* offset is long boundary */
			    offset += input_pc;
			    break;
			case SPARC_RELOC_WDISP30:
			    offset = (instruction & 0x3fffffff);
			    offset <<= 2; /* offset is on a long boundary */
			    offset += input_pc;
			    break;
			default:
			    /* the error check is caught below */
			    break;
			}
		    }
		}

		if(merged_symbol->nlist.n_type == (N_EXT | N_SECT))
		    output_sections[merged_symbol->nlist.n_sect]->referenced =
									   TRUE;
	    }
	    else{
		/*
		 * This is a local relocation entry (the value to which the item
		 * to be relocated is refering to is defined in section number
		 * r_symbolnum).  So the address of that section in the input
		 * file is subtracted and the value of that section in the
		 * output is added to the item being relocated.
		 */
		value = 0;
		/*
		 * If the symbol is not in any section the value to be added to
		 * the item to be relocated is the zero above and any pc
		 * relative change in value added below.
		 */
		if(r_symbolnum != R_ABS){
		    if(r_symbolnum > cur_obj->nsection_maps){
			error_with_cur_obj("r_symbolnum (%lu) field of local "
			    "relocation entry %lu in section (%.16s,%.16s) "
			    "out of range", r_symbolnum, i,
			    section_map->s->segname, section_map->s->sectname);
			return;
		    }
		    local_map = &(cur_obj->section_maps[r_symbolnum - 1]);
		    local_map->output_section->referenced = TRUE;
		    pair_local_map = NULL;

		    if(r_type == SPARC_RELOC_SECTDIFF ||
		       r_type == SPARC_RELOC_HI22_SECTDIFF ||
		       r_type == SPARC_RELOC_LO10_SECTDIFF){
			pair_local_map =
			    &(cur_obj->section_maps[pair_r_symbolnum - 1]);
			pair_local_map->output_section->referenced = TRUE;
		    }
		    if(local_map->nfine_relocs == 0 && 
		       (pair_local_map == NULL ||
			pair_local_map->nfine_relocs == 0) ){
			if(r_type == SPARC_RELOC_SECTDIFF ||
			   r_type == SPARC_RELOC_HI22_SECTDIFF ||
			   r_type == SPARC_RELOC_LO10_SECTDIFF){
			    value = - local_map->s->addr
				    + (local_map->output_section->s.addr +
				       local_map->offset)
				    + pair_local_map->s->addr
				    - (pair_local_map->output_section->s.addr +
				       pair_local_map->offset);
			}
			else{
			    value = - local_map->s->addr
				    + (local_map->output_section->s.addr +
				       local_map->offset);
			}
		    }
		    else{
			/*
			 * For items to be relocated that refer to a section
			 * with fine relocation the value is set (not adjusted
			 * with addition).  So the new value is directly
			 * calculated from the old value.
			 */
			if(r_pcrel) {
			    if(section_map->nfine_relocs == 0)
				output_pc = section_map->output_section->s.addr
					    + section_map->offset +
					    r_address;
			    else
			      output_pc = section_map->output_section->s.addr + 
				fine_reloc_output_offset(section_map, r_address);
			}
			else{
			    input_pc = 0;
			    output_pc = 0;
			}
			/*
			 * Get the value of the expresion of the item to be
			 * relocated.
			 */
			if(r_type == SPARC_RELOC_VANILLA ||
			   r_type == SPARC_RELOC_SECTDIFF){
			    switch(r_length){
			    case 0: /* byte */
				value = get_byte((char *)(contents +
							  r_address));
				break;
			    case 1: /* word (2 byte) */
				value = get_short((short *)(contents +
							    r_address));
				break;
			    case 2: /* long (4 byte) */
				value = get_long((long *)(contents +
							  r_address));
				break;
			    default:
				/* the error check is catched below */
				break;
			    }
			}
			else{
			    instruction = get_long((long *)(contents +
							    r_address));
			    switch(r_type){
			    case SPARC_RELOC_HI22:
			    case SPARC_RELOC_HI22_SECTDIFF:
				value = ((instruction & 0x3fffff) << 10) | 
				  (other_half & 0x3ff) ;
				break;
			    case SPARC_RELOC_LO10:
			    case SPARC_RELOC_LO10_SECTDIFF:
				value = (instruction & 0x3ff) | 
				  ((other_half & 0x3fffff) << 10);
				break;
			    case SPARC_RELOC_WDISP22:
				value = (instruction & 0x3fffff);
				if ((value & 0x200000) != 0)
				  value |= 0xffc00000;
				value <<= 2;
				break;
			    case SPARC_RELOC_WDISP30:
				value = (instruction & 0x3fffffff);
				value <<= 2;
				break;
			    default:
				/* the error check is caught below */
				break;
			    }
			}
			if(r_type == SPARC_RELOC_SECTDIFF ||
			   r_type == SPARC_RELOC_HI22_SECTDIFF ||
			   r_type == SPARC_RELOC_LO10_SECTDIFF){
			    /*
			     * For SPARC_RELOC_SECTDIFF's the item to be
			     * relocated, in value, is the value of the
			     * expression:
			     *     r_value - pair_r_value + offset
			     * To set the value of the relocated expression,
			     * it is set from relocating the two r_value's and
			     * adding back in the offset.  So here get the
			     * offset from the value of the expression.
			     */
			    value += input_pc; /* adjust for pcrel */
			    offset = value - r_value + pair_r_value;

			    /*
			     * Now build up the value of the relocated
			     * expression one part at a time.  First set the
			     * new value to the relocated r_value.
			     */
		    	    if(local_map->nfine_relocs != 0){
				/*
				 * Check to see if this reference is legal with
				 * respect to indirect sections.
				 */
				legal_reference(section_map, r_address,
				    local_map, r_value - local_map->s->addr +
				    offset, i, TRUE);
				value = fine_reloc_output_address(local_map,
					    r_value - local_map->s->addr,
					    local_map->output_section->s.addr);
			    }
			    else{
				value = local_map->output_section->s.addr +
					local_map->offset +
					r_value - local_map->s->addr;
			    }
			    /* Second subtract the relocated pair_r_value. */
			    if(pair_local_map->nfine_relocs != 0){
				/*
				 * Check to see if this reference is legal with
				 * respect to indirect sections.
				 */
				legal_reference(section_map, r_address,
				    pair_local_map, pair_r_value -
				    pair_local_map->s->addr, i, TRUE);
				value -=
				    fine_reloc_output_address(pair_local_map,
					pair_r_value - pair_local_map->s->addr,
				        pair_local_map->output_section->s.addr);
			    }
			    else{
				value -=
				    pair_local_map->output_section->s.addr +
				    pair_local_map->offset +
				    pair_r_value - pair_local_map->s->addr;
			    }
			    /* Third add in the offset. */
			    value += offset;
			    value -= output_pc; /* adjust for pcrel */
			}
			else{
			    /*
			     * If the relocation entry is not a scattered
			     * relocation entry then the relocation is based on
			     * the value of value of the expresion of the item
			     * to be relocated.  If it is a scattered relocation
			     * entry then the relocation is based on the r_value
			     * in the relocation entry and the offset part of
			     * the expression at the item to be relocated is
			     * extracted so it can be added after the relocation
			     * is done.
			     */
			    value += input_pc;
			    if(r_scattered == 0){
				r_value = value;
				offset = 0;
			    }
			    else{
				offset = value - r_value;
			    }
			    /*
			     * Check to see if this reference is legal with
			     * respect to indirect sections.
			     */
			    legal_reference(section_map, r_address, local_map,
				    r_value - local_map->s->addr + offset, i,
				    FALSE);
			    value = fine_reloc_output_address(local_map,
					r_value - local_map->s->addr,
					local_map->output_section->s.addr);
			    value -= output_pc;
			    value += offset;
			}
			if(r_type == SPARC_RELOC_VANILLA ||
			   r_type == SPARC_RELOC_SECTDIFF){
			    switch(r_length){
			    case 0: /* byte */
				if( (value & 0xffffff00) &&
				   ((value & 0xffffff80) != 0xffffff80))
				    error_with_cur_obj("relocation for entry "
					"%lu in section (%.16s,%.16s) does not "
					"fit in 1 byte", i,
					section_map->s->segname,
					section_map->s->sectname);
				set_byte((char *)(contents + r_address), value);
				break;
			    case 1: /* word (2 byte) */
				if( (value & 0xffff0000) &&
				   ((value & 0xffff8000) != 0xffff8000))
				    error_with_cur_obj("relocation for entry "
					"%lu in section (%.16s,%.16s) does not "
					"fit in 2 bytes", i,
					section_map->s->segname,
					section_map->s->sectname);
				set_short((short *)(contents + r_address),
					  value);
				break;
			    case 2: /* long (4 byte) */
				set_long((long *)(contents + r_address), value);
				break;
			    default:
				error_with_cur_obj("r_length field of "
				    "relocation entry %lu in section (%.16s,"
				    "%.16s) invalid", i,
				    section_map->s->segname,
				    section_map->s->sectname);
				return;
			    }
			}
			else{
			    switch(r_type){
			    case SPARC_RELOC_HI22:
			    case SPARC_RELOC_HI22_SECTDIFF:
				other_half = value & 0x3ff;
				instruction = (instruction & 0xffc00000) |
					      ((value >> 10) & 0x3fffff);
				break;
			    case SPARC_RELOC_LO10:
			    case SPARC_RELOC_LO10_SECTDIFF:
				other_half = (value >> 10) & 0x3fffff;
				instruction = (instruction & 0xfffffc00) |
					      (value & 0x3ff);
				break;
			      case SPARC_RELOC_WDISP22:
				if ((value & 0xff800000) != 0xff800000 &&
				    (value & 0xff800000) != 0x00000000)
				    error_with_cur_obj("relocation overflow "
					"for relocation entry %lu in section "
					"(%.16s,%.16s) (displacement too large)"
					, i, section_map->s->segname,
					section_map->s->sectname);
				value >>= 2;
				instruction = (instruction & 0xffc00000) |
				  (value & 0x003fffff);
				break;
			      case SPARC_RELOC_WDISP30:
				value >>= 2;
				instruction = (instruction & 0xc0000000) |
				  (value & 0x3fffffff);
				break;
			    default:
				error_with_cur_obj("r_type field of "
				    "relocation entry %lu in section (%.16s,"
				    "%.16s) invalid", i,
				    section_map->s->segname,
				    section_map->s->sectname);
				return;
			    }
			    set_long((long *)(contents + r_address),
				     instruction);
			}

			goto update_reloc;
		    }
		}
	    }
	    if(r_pcrel){
		/*
		 * This is a relocation entry is also pc relative which means
		 * the value of the pc will get added to it when it is executed.
		 * The item being relocated has the value of the pc in the input
		 * file subtracted from it.  So to relocate this the value of
		 * pc in the input file is added and then value of the output
		 * pc is subtracted (since the offset into the section remains
		 * constant it is not added in and then subtracted out).
		 */
		if(section_map->nfine_relocs == 0)
		    value += + section_map->s->addr + /* r_address */
		             - (section_map->output_section->s.addr +
				section_map->offset /* + r_address */);
		else
		    value += + section_map->s->addr + r_address
			     - (section_map->output_section->s.addr +
			        fine_reloc_output_offset(section_map,
							 r_address));
	    }
	    if(r_type == SPARC_RELOC_VANILLA ||
	       r_type == SPARC_RELOC_SECTDIFF){
		switch(r_length){
		case 0: /* byte */
		    value += get_byte((char *)(contents + r_address));
		    if( (value & 0xffffff00) &&
		       ((value & 0xffffff80) != 0xffffff80))
			error_with_cur_obj("relocation for entry %lu in section"
			    " (%.16s,%.16s) does not fit in 1 byte", i,
			    section_map->s->segname, section_map->s->sectname);
		    set_byte((char *)(contents + r_address), value);
		    break;
		case 1: /* word (2 byte) */
		    value += get_short((short *)(contents + r_address));
		    if( (value & 0xffff0000) &&
		       ((value & 0xffff8000) != 0xffff8000))
			error_with_cur_obj("relocation for entry %lu in section"
			    " (%.16s,%.16s) does not fit in 2 bytes", i,
			    section_map->s->segname, section_map->s->sectname);
		    set_short((short *)(contents + r_address), value);
		    break;
		case 2: /* long (4 byte) */
		    value += get_long((long *)(contents + r_address));
		    set_long((long *)(contents + r_address), value);
		    break;
		default:
		    error_with_cur_obj("r_length field of relocation entry %lu "
			"in section (%.16s,%.16s) invalid", i,
			section_map->s->segname, section_map->s->sectname);
		    return;
		}
	    }
	    /*
	     * Do SPARC specific relocation based on the r_type.
	     */
	    else {
		instruction = get_long((long *)(contents + r_address));
		switch(r_type){
		case SPARC_RELOC_HI22_SECTDIFF:
		    immediate = ((instruction & 0x3fffff) << 10) | other_half;
		    immediate += value;
		    instruction = (instruction & 0xffc00000) |
				  ((immediate >> 10) & 0x3fffff);
		    other_half = immediate & 0x3ff;
		    break;

		case SPARC_RELOC_HI22:
		    immediate = ((instruction & 0x3fffff) << 10) | other_half;
		    immediate += value;
		    instruction = (instruction & 0xffc00000) |
				  ((immediate >> 10) & 0x3fffff);
		    other_half = immediate & 0x3ff;
		    break;
		case SPARC_RELOC_LO10:
		    immediate = (instruction & 0x3ff) | (other_half << 10);
		    immediate += value;
		    instruction = (instruction & 0xfffffc00) | 
					(immediate & 0x3ff);
		    other_half = (immediate >> 10) & 0x3fffff;
		    break;
		case SPARC_RELOC_LO10_SECTDIFF:
		    immediate = (instruction & 0x3ff) | (other_half << 10);
		    immediate += value;
		    instruction = (instruction & 0xfffffc00) |
				  (immediate & 0x3ff);
		    other_half = (immediate >> 10) & 0x3fffff;
		    break;
		  case SPARC_RELOC_WDISP22:
		    immediate = (instruction & 0x3fffff);
		    if ((immediate & 0x200000) != 0)
		      immediate |= 0xffc00000;
		    immediate <<= 2;
		    immediate += value;
		    if ((immediate & 0xff800000) != 0xff800000 &&
		      (immediate & 0xff800000) != 0x00)
			error_with_cur_obj("relocation overflow for relocation "
			    "entry %lu in section (%.16s,%.16s) (displacement "
			    "too large)", i, section_map->s->segname,
			    section_map->s->sectname);
		    immediate >>= 2;
		    instruction = (instruction & 0xffc00000) | 
					(immediate & 0x3fffff);
		    break;
		  case SPARC_RELOC_WDISP30:
		    immediate = (instruction & 0x3fffffff);
		    immediate <<= 2;
		    immediate += value;
		    immediate >>= 2;
		    instruction = (instruction & 0xc0000000) | 
					(immediate & 0x3fffffff);
		    break;
		default:
		    error_with_cur_obj("r_type field of relocation entry %lu "
			"in section (%.16s,%.16s) invalid", i,
			section_map->s->segname, section_map->s->sectname);
		    continue;
		}
		set_long((long *)(contents + r_address), instruction);
	    }

	    /*
	     * If relocation entries are to be saved in the output file then
	     * update the entry for the output file.
	     */
update_reloc:
	    ;
#ifndef RLD
	    if(save_reloc || output_for_dyld){
		if(r_extern){
		    /*
		     * If we are prebinding and this is a lazy pointer section
		     * change the relocation entry to a SPARC_RELOC_PB_LA_PTR
		     * type.  This stuffs the value of the lazy pointer as it
		     * wouldn't be prebound in the r_value field.  So if the
		     * prebounding can't be used at runtime the value of the
		     * lazy pointer will get set back to the r_value by dyld.
		     */
		    if(prebinding == TRUE &&
		       (section_map->s->flags & SECTION_TYPE) ==
			S_LAZY_SYMBOL_POINTERS){
			sreloc = (struct scattered_relocation_info *)reloc;
			r_scattered = 1;
			sreloc->r_scattered = r_scattered;
			sreloc->r_address = r_address;
			sreloc->r_pcrel = r_pcrel;
			sreloc->r_length = r_length;
			sreloc->r_type = SPARC_RELOC_PB_LA_PTR;
			sreloc->r_value = value;
		    }
		    /*
		     * For external relocation entries that the symbol is
		     * defined (not undefined or common) but not when we are
		     * forcing an external relocation entry for a global
		     * coalesced symbol and if the output file is not MH_DYLIB
		     * or the symbol is a private extern it is changed to a
		     * local relocation entry using the section that symbol is
		     * defined in.  If still undefined or forcing an external
		     * relocation entry for a global coalesced symbol then the
		     * index of the symbol in the output file is set into
		     * r_symbolnum.
		     */
		    else if((merged_symbol->nlist.n_type & N_TYPE) != N_UNDF &&
		            (merged_symbol->nlist.n_type & N_TYPE) != N_PBUD &&
		            force_extern_reloc == FALSE &&
		            (filetype != MH_DYLIB ||
			     (merged_symbol->nlist.n_type & N_PEXT) == N_PEXT)){
			reloc->r_extern = 0;
			/*
			 * If this symbol was in the base file then no futher
			 * relocation can ever be done (the symbols in the base
			 * file are fixed). Or if the symbol was an absolute
			 * symbol.
			 */
			if(merged_symbol->definition_object == base_obj ||
			   (merged_symbol->nlist.n_type & N_TYPE) == N_ABS){
				reloc->r_symbolnum = R_ABS;
			}
			else{
			    /*
			     * The symbol that this relocation entry is refering
			     * to is defined so convert this external relocation
			     * entry into a local or scattered relocation entry.
			     * If the item to be relocated has an offset added
			     * to the symbol's value make it a scattered
			     * relocation entry else make it a local relocation
			     * entry.
			     */
			    if(offset == 0){
				reloc->r_symbolnum =merged_symbol->nlist.n_sect;
			    }
			    else{
				sreloc = (struct scattered_relocation_info *)
					 reloc;
				r_scattered = 1;
				sreloc->r_scattered = r_scattered;
				sreloc->r_address = r_address;
				sreloc->r_pcrel = r_pcrel;
				sreloc->r_length = r_length;
				sreloc->r_type = r_type;
				sreloc->r_value = merged_symbol->nlist.n_value;
			    }
			}
		    }
		    else{
			reloc->r_symbolnum =
				      merged_symbol_output_index(merged_symbol);
		    }
		}
		else if(r_scattered == 0){
		    /*
		     * If we are prebinding and this is a lazy pointer section
		     * change the relocation entry to a SPARC_RELOC_PB_LA_PTR
		     * type.  This stuffs the value of the lazy pointer as it
		     * wouldn't be prebound in the r_value field.  So if the
		     * prebounding can't be used at runtime the value of the
		     * lazy pointer will get set back to the r_value by dyld.
		     */
		    if(prebinding == TRUE &&
		       (section_map->s->flags & SECTION_TYPE) ==
			S_LAZY_SYMBOL_POINTERS){
			sreloc = (struct scattered_relocation_info *)reloc;
			r_scattered = 1;
			sreloc->r_scattered = r_scattered;
			sreloc->r_address = r_address;
			sreloc->r_pcrel = r_pcrel;
			sreloc->r_length = r_length;
			sreloc->r_type = SPARC_RELOC_PB_LA_PTR;
			sreloc->r_value = value;
		    }
		    /*
		     * For local relocation entries the section number is
		     * changed to the section number in the output file.
		     */
		    else if(reloc->r_symbolnum != R_ABS){
			if(local_map->nfine_relocs == 0){
			    reloc->r_symbolnum =
				      local_map->output_section->output_sectnum;
			}
			else{
			    reloc->r_symbolnum =
				fine_reloc_output_sectnum(local_map,
						r_value - local_map->s->addr);
			}
		    }
		}
		else{
		    /*
		     * For scattered relocation entries the r_value field is
		     * relocated.
		     */
		    if(local_map->nfine_relocs == 0)
			sreloc->r_value += - local_map->s->addr
					   + local_map->output_section->s.addr +
					   local_map->offset;
		    else
			sreloc->r_value = fine_reloc_output_address(local_map,
				     		r_value - local_map->s->addr,
					   local_map->output_section->s.addr);
		}
		/*
		 * If this section that the reloation is being done for has fine
		 * relocation then the offset in the r_address field has to be
		 * set to where it will end up in the output file.  Otherwise
		 * it simply has to have the offset to where this contents
		 * appears in the output file. 
		 */
		if(r_scattered == 0){
		    if(section_map->nfine_relocs == 0){
			reloc->r_address += section_map->offset;
		    }
		    else{
			reloc->r_address = fine_reloc_output_offset(section_map,
								    r_address);
		    }
		}
		else{
		    if(section_map->nfine_relocs == 0){
			sreloc->r_address += section_map->offset;
		    }
		    else{
			sreloc->r_address =fine_reloc_output_offset(section_map,
								    r_address);
		    }
		}
		/*
		 * If their was a paired relocation entry then update the
		 * paired relocation entry.
		 */
		if(pair_r_type == SPARC_RELOC_PAIR){
		    if(pair_reloc != NULL)
			pair_reloc->r_address = other_half;
		    else if(spair_reloc != NULL){
			if(r_type == SPARC_RELOC_SECTDIFF ||
			   r_type == SPARC_RELOC_HI22_SECTDIFF ||
			   r_type == SPARC_RELOC_LO10_SECTDIFF){
			    /*
			     * For SPARC_RELOC_SECTDIFF relocation entries (which
			     * are always scattered types) the r_value field is
			     * relocated.
			     */
			    if(pair_local_map->nfine_relocs == 0)
				spair_reloc->r_value +=
				    - pair_local_map->s->addr
				    + (pair_local_map->output_section->s.addr +
				       pair_local_map->offset);
			    else
				spair_reloc->r_value =
				    fine_reloc_output_address(pair_local_map,
					pair_r_value - pair_local_map->s->addr,
					pair_local_map->output_section->s.addr);
			    if(r_type == SPARC_RELOC_HI22_SECTDIFF ||
			       r_type == SPARC_RELOC_LO10_SECTDIFF)
				spair_reloc->r_address = other_half;
			}
			else{
			    spair_reloc->r_address = other_half;
			}
		    }
		    else{
			fatal("internal error, in sparc_reloc() pair_r_type "
			    "is SPARC_RELOC_PAIR but pair_reloc and spair_reloc "
			    "are NULL");
		    }
		}
	    }
#endif !defined(RLD)
	    /*
	     * If their was a paired relocation entry then it has been processed
	     * so skip it by incrementing the index of the relocation entry that
	     * is being processed.
	     */
	    if(pair_r_type == SPARC_RELOC_PAIR)
		i++;
	}
}
