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
 * This file contains the routines that deal with module initialization and
 * termination function pointer sections. 
 */
#include <stdlib.h>
#if !(defined(KLD) && defined(__STATIC__))
#include <stdio.h>
#include <mach/mach.h>
#else /* defined(KLD) && defined(__STATIC__) */
#include <mach/kern_return.h>
#endif /* !(defined(KLD) && defined(__STATIC__)) */
#include <stdarg.h>
#include <string.h>
#include "stuff/openstep_mach.h"
#include <mach-o/loader.h>
#include <mach-o/reloc.h>
#include "stuff/bytesex.h"

#include "ld.h"
#include "objects.h"
#include "sections.h"
#include "mod_sections.h"

#ifndef RLD
/*
 * For MH_DYLIB formats only one module initialization and one module
 * termination function pointer section is allowed.  The module table has
 * indexes and counts of the number of entries in this section for each module
 * in a MH_DYLIB format file.  These variables are used for this.
 */
static struct merged_section *dylib_ms_init = NULL;
static struct merged_section *dylib_ms_term = NULL;
static unsigned long ninit = 0;
static unsigned long nterm = 0;
#endif /* !defined(RLD) */

/*
 * mod_section_merge() is used to check the section looks ok for a module
 * initialization or termination function pointer section.  After this the rest
 * of the link editor treats it like a regular section in how it's relocated and
 * output.
 */
__private_extern__
void
mod_section_merge(
void *data, 
struct merged_section *ms,
struct section *s, 
struct section_map *section_map)
{
    unsigned long i;
    struct relocation_info *relocs, reloc;
    struct scattered_relocation_info *sreloc;
    unsigned long r_address, r_pcrel, r_length, r_type;
    char *type_name;
#ifdef __MWERKS__
    void *dummy;
        dummy = data;
#endif
	if((ms->s.flags & SECTION_TYPE) == S_MOD_INIT_FUNC_POINTERS)
	    type_name = "initialization";
	else
	    type_name = "termination";

	/*
	 * Module initialization and termination function pointer sections must
	 * contain only pointers to functions.  Each of these pointers should be
	 * 4 bytes and have a relocation entry.  A good number of checks are
	 * done here to make sure that things are ok but it is not exaustive.
	 * There could be multiple relocation entries for the same address and
	 * there is no check for what the relocation entry points at (a check
	 * could be done to see that it points to a section with the attribute
	 * S_ATTR_PURE_INSTRUCTIONS).
	 */
	if(s->size % 4 != 0){
	    error_with_cur_obj("module %s function pointer section (%.16s,"
		"%.16s) size is not a multiple of 4 bytes", type_name,
		s->segname, s->sectname);
	    return;
	}
	if(s->nreloc != s->size / 4){
	    error_with_cur_obj("module %s function pointer section (%.16s,"
		"%.16s) does not have is exactly one relocation entry for each "
		"pointer\n", type_name, s->segname, s->sectname);
	    return;
	}
	relocs = (struct relocation_info *)(cur_obj->obj_addr + s->reloff);
	for(i = 0; i < s->nreloc; i++){
	    reloc = relocs[i];
	    if(cur_obj->swapped)
		swap_relocation_info(&reloc, 1, host_byte_sex);
	    /*
	     * Break out the fields of the relocation entry we need here.
	     */
	    if((reloc.r_address & R_SCATTERED) != 0){
		sreloc = (struct scattered_relocation_info *)(&reloc);
		r_address = sreloc->r_address;
		r_pcrel = sreloc->r_pcrel;
		r_length = sreloc->r_length;
		r_type = sreloc->r_type;
	    }
	    else{
		r_address = reloc.r_address;
		r_pcrel = reloc.r_pcrel;
		r_length = reloc.r_length;
		r_type = reloc.r_type;
	    }
	    /*
	     * For a module initialization and termination function pointer
	     * sections all relocation entries must be for one of the pointers
	     * and therefore the offset must be a multiple of 4, have an
	     * r_length field of 2 (long), a r_pcrel field of 0 (FALSE) and a
	     * r_type of 0 (VANILLA).
	     */
	    if(r_address % 4 != 0){
		error_with_cur_obj("r_address (0x%x) field of relocation entry "
		    "%ld in module %s function pointer section (%.16s,%.16s) "
		    "is not a multiple of 4", (unsigned int)r_address, i,
		    type_name, s->segname, s->sectname);
		continue;
	    }
	    if(r_length != 2){
		error_with_cur_obj("r_length (0x%x) field of relocation entry "
		    "%ld in module %s function pointer section (%.16s,%.16s) "
		    "is not 2 (long)", (unsigned int)r_length, i, type_name,
		    s->segname, s->sectname);
		continue;
	    }
	    if(r_pcrel != 0){
		error_with_cur_obj("r_pcrel (0x%x) field of relocation entry "
		    "%ld in module %s function pointer section (%.16s,%.16s) "
		    "is not 0 (FALSE)", (unsigned int)r_pcrel, i, type_name,
		    s->segname, s->sectname);
		continue;
	    }
	    if(r_type != 0){
		error_with_cur_obj("r_type (0x%x) field of relocation entry "
		    "%ld in module %s function pointer section (%.16s,%.16s) "
		    "is not 0 (VANILLA)", (unsigned int)r_type, i, type_name,
		    s->segname, s->sectname);
		continue;
	    }
	}

#ifndef RLD
	/*
	 * For MH_DYLIB formats make sure there is only one module
	 * initialization and termination function pointer section and record
	 * the index into the output section and the number of enties for this
	 * object.
	 */
	if(filetype == MH_DYLIB){
	    if((ms->s.flags & SECTION_TYPE) == S_MOD_INIT_FUNC_POINTERS){
		if(dylib_ms_init != NULL && dylib_ms_init != ms)
		    error("multiple module initialization function pointer "
			  "sections (%.16s,%.16s) and (%.16s,%.16s) only one "
			  "allowed in MH_DYLIB output", s->segname, s->sectname,
			  dylib_ms_init->s.segname, dylib_ms_init->s.sectname);
		dylib_ms_init = ms;
		cur_obj->ninit = s->size / 4;
		cur_obj->iinit = ninit;
		ninit += cur_obj->ninit;
		if(ninit > 0xffff)
		    error("too many module initialization function pointer in "
			  "section (%.16s,%.16s) in MH_DYLIB output (maximum "
			  "is %d)", s->segname, s->sectname, 0xffff);
	    }
	    else{
		if(dylib_ms_term != NULL && dylib_ms_term != ms)
		    error("multiple module termination function pointer "
			  "sections (%.16s,%.16s) and (%.16s,%.16s) only one "
			  "allowed in MH_DYLIB output", s->segname, s->sectname,
			  dylib_ms_term->s.segname, dylib_ms_term->s.sectname);
		dylib_ms_term = ms;
		cur_obj->nterm = s->size / 4;
		cur_obj->iterm = nterm;
		nterm += cur_obj->nterm;
		if(nterm > 0xffff)
		    error("too many module termination function pointer in "
			  "section (%.16s,%.16s) in MH_DYLIB output (maximum "
			  "is %d)", s->segname, s->sectname, 0xffff);
	    }
	}
#endif /* !defined(RLD) */

	/*
	 * Check to see if the alignment will leave zero pointers.
	 */
	if(s->align > 2)
	    warning_with_cur_obj("aligment greater than 2 (2^2, 4 bytes) for "
		"module %s function pointer section (%.16s,%.16s) (will leave "
		"holes with zeroes for pointers)", type_name, s->segname,
		s->sectname);

	/*
	 * Now that every thing looks ok do what would have been done in
	 * merge_sections() for regular sections.
	 */
	section_map->flush_offset = ms->s.size;
	ms->s.size = round(ms->s.size, 1 << s->align);
	section_map->offset = ms->s.size;
	ms->s.size   += s->size;
	ms->s.nreloc += s->nreloc;
	nreloc += s->nreloc;
}

/*
 * mod_section_order() is here to generate an error if a module initialization
 * or termination function pointer has a sectorder file.  Since the functions
 * are called in the order they appear the pointers appear in the section and
 * that order is guaranteed the section can't be order differently.
 */
__private_extern__
void
mod_section_order(
void *data, 
struct merged_section *ms)
{
#ifndef RLD
    kern_return_t r;
    char *type_name;
#ifdef __MWERKS__
    void *dummy;
        dummy = data;
#endif
	if((ms->s.flags & SECTION_TYPE) == S_MOD_INIT_FUNC_POINTERS)
	    type_name = "initialization";
	else
	    type_name = "termination";

	error("section ordering for module %s function pointer sections not "
	      "allowed (-sectorder %s %s %s ignored)", type_name, ms->s.segname,
	      ms->s.sectname, ms->order_filename);
	/*
	 * Deallocate the memory for the load order file now that it is
	 * nolonger needed.
	 */
	if((r = vm_deallocate(mach_task_self(), (vm_address_t)
	    ms->order_addr, ms->order_size)) != KERN_SUCCESS)
	    mach_fatal(r, "can't vm_deallocate() memory for -sectorder file: "
		       "%s for section (%.16s,%.16s)", ms->order_filename,
		       ms->s.segname, ms->s.sectname);
	ms->order_addr = NULL;
#else /* RLD */
#ifdef __MWERKS__
    void *dummy1;
    struct merged_section *dummy2;
        dummy1 = data;
        dummy2 = ms;
#endif
#endif /* RLD */
}
