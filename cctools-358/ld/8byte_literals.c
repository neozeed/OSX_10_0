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
 * This file contains the routines that deal with 8 byte literals sections.
 * A literal in this section must beable to me moved freely with respect to
 * other literals.  This means relocation must not reach outside the size of
 * the literal.  The size of this this type of section must be a multiple of
 * 8 bytes in all input files.
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
#include <mach-o/loader.h>
#include "stuff/bool.h"
#include "stuff/bytesex.h"

#include "ld.h"
#include "objects.h"
#include "sections.h"
#include "8byte_literals.h"
#include "pass2.h"

/*
 * literal8_merge() merges 8 byte literals from the specified section in the
 * current object file (cur_obj).  It allocates a fine relocation map and
 * sets the fine_relocs field in the section_map to it (as well as the count).
 */
__private_extern__
void
literal8_merge(
struct literal8_data *data,
struct merged_section *ms,
struct section *s,
struct section_map *section_map)
{
    unsigned long nliteral8s, i;
    struct literal8 *literal8s;
    struct fine_reloc *fine_relocs;

	if(s->size == 0){
	    section_map->fine_relocs = NULL;
	    section_map->nfine_relocs = 0;
	    return;
	}
	/*
	 * Calcualte the number of literals so the size of the fine relocation
	 * structures can be allocated.
	 */
	if(s->size % 8 != 0){
	    error_with_cur_obj("8 byte literal section (%.16s,%.16s) size is "
			       "not a multiple of 8 bytes", ms->s.segname,
			       ms->s.sectname);
	    return;
	}
	nliteral8s = s->size / 8;
#ifdef DEBUG
	data->nfiles++;
	data->nliterals += nliteral8s;
#endif DEBUG

	fine_relocs = allocate(nliteral8s * sizeof(struct fine_reloc));
	memset(fine_relocs, '\0', nliteral8s * sizeof(struct fine_reloc));

	/*
	 * lookup and enter each C string in the section and record the offsets
	 * in the input file and in the output file.
	 */
	literal8s = (struct literal8 *)(cur_obj->obj_addr + s->offset);
	for(i = 0; i < nliteral8s; i++){
	    fine_relocs[i].input_offset = i * 8;
	    fine_relocs[i].output_offset =
					lookup_literal8(literal8s[i], data, ms);
	}
	section_map->fine_relocs = fine_relocs;
	section_map->nfine_relocs = nliteral8s;
}

/*
 * literal8_order() enters 8 byte literals from the order_file from the merged
 * section structure.  Since this is called before any call to literal8_merge
 * and it enters the literals in the order of the file it causes the section
 * to be ordered.
 */
__private_extern__
void
literal8_order(
struct literal8_data *data,
struct merged_section *ms)
{
    unsigned long i, line_number;
    struct literal8 literal8;

	line_number = 1;
	i = 0;
	while(i < ms->order_size){
	    if(get_hex_from_sectorder(ms, &i, &(literal8.long0),
				      line_number) == TRUE){
		if(get_hex_from_sectorder(ms, &i, &(literal8.long1),
					  line_number) == TRUE)
		    (void)lookup_literal8(literal8, data, ms);
		else
		    error("format error in -sectorder file: %s line %lu for "
			  "section (%.16s,%.16s) (missing second hex number)",
			  ms->order_filename, line_number, ms->s.segname,
			  ms->s.sectname);
	    }
	    while(i < ms->order_size && ms->order_addr[i] != '\n')
		i++;
	    if(i < ms->order_size && ms->order_addr[i] == '\n')
		i++;
	    line_number++;
	}
}

/*
 * get_hex_from_sectorder() gets a hex number of the form 0x<hex digits> for a
 * 32 bit value from a sectorder file.  The sectorder file is for the merged
 * section passed to it (ms).  The index to start scaning from is in *index and
 * is set to the index after the hex number on return.  If a hex number is found
 * it is returned indirectly through *value and TRUE is returned (if a hex
 * number is not found FALSE is returned).  If an error is incountered then an
 * error message is printed stating the order file and the section it is for
 * and the line_number (passed in) it occured on.
 */
__private_extern__
enum bool
get_hex_from_sectorder(
struct merged_section *ms,
unsigned long *index,
unsigned long *value,
unsigned long line_number)
{
    unsigned long i, j;
    char hex[9];

	i = *index;
	/* trim leading white space */
	while(i < ms->order_size &&
	      (ms->order_addr[i] == ' ' ||
	       ms->order_addr[i] == '\t'))
	    i++;

	/*
	 * If after skipping leading white space we are at the end of the file
	 * then just return FALSE but print no error.
	 */
	if(i > ms->order_size){
	    *index = i;
	    return(FALSE);
	}

	/* look for a leading 0x */
	if(i > ms->order_size || ms->order_addr[i] != '0'){
	    error("format error in -sectorder file: %s line %lu for section "
		  "(%.16s,%.16s) (missing hex number, no leading 0x found)",
		  ms->order_filename,line_number,ms->s.segname,ms->s.sectname);
	    *index = i;
	    return(FALSE);
	}
	i++;
	if(i > ms->order_size || ms->order_addr[i] != 'x'){
	    error("format error in -sectorder file: %s line %lu for section "
		  "(%.16s,%.16s) (missing hex number, no leading 0x found)",
		  ms->order_filename,line_number,ms->s.segname,ms->s.sectname);
	    *index = i;
	    return(FALSE);
	}
	i++;

	/* pick-up all hex digits and save the first 8 */
	j = 0;
	while(i < ms->order_size &&
	      ((ms->order_addr[i] >= '0' && ms->order_addr[i] <= '9') ||
	       (ms->order_addr[i] >= 'a' && ms->order_addr[i] <= 'f') ||
	       (ms->order_addr[i] >= 'A' && ms->order_addr[i] <= 'F')) ){
	    if(j <= 8)
		hex[j++] = ms->order_addr[i++];
	    else
		i++;
	}
	if(j > 8){
	    error("format error in -sectorder file: %s line %lu for section "
		  "(%.16s,%.16s) (too many hex digits for 32 bit value)",
		  ms->order_filename,line_number,ms->s.segname,ms->s.sectname);
	    *index = i;
	    return(FALSE);
	}
	hex[j] = '\0';
	*value = strtoul(hex, NULL, 16);
	*index = i;
	return(TRUE);
}

/*
 * lookup_literal8() looks up the 8 byte literal passed to it in the
 * literal8_data passed to it and returns the offset the 8 byte literal will
 * have in the output file.  It creates the blocks to store the literals and
 * attaches them to the literal8_data passed to it.  The total size of the
 * section is accumulated in ms->s.size which is the merged section for this
 * literal section.  The literal is aligned to the alignment in the merged
 * section (ms->s.align).
 */
__private_extern__
unsigned long
lookup_literal8(
struct literal8 literal8,
struct literal8_data *data,
struct merged_section *ms)
{
    struct literal8_block **p, *literal8_block;
    unsigned long align_multiplier, output_offset, i;

	align_multiplier = 1;
 	if((1 << ms->s.align) > 8)
	    align_multiplier = (1 << ms->s.align) / 8;

	output_offset = 0;
	for(p = &(data->literal8_blocks); *p ; p = &(literal8_block->next)){
	    literal8_block = *p;
	    for(i = 0; i < literal8_block->used; i++){
		if(literal8.long0 == literal8_block->literal8s[i].long0 &&
		   literal8.long1 == literal8_block->literal8s[i].long1)
		    return(output_offset + i * 8 * align_multiplier);
	    }
	    if(literal8_block->used != LITERAL8_BLOCK_SIZE){
		literal8_block->literal8s[i].long0 = literal8.long0;
		literal8_block->literal8s[i].long1 = literal8.long1;
		literal8_block->used++;
		ms->s.size += 8 * align_multiplier;
		return(output_offset + i * 8 * align_multiplier);
	    }
	    output_offset += literal8_block->used * 8 * align_multiplier;
	}
	*p = allocate(sizeof(struct literal8_block));
	literal8_block = *p;
	literal8_block->used = 1;
	literal8_block->literal8s[0].long0 = literal8.long0;
	literal8_block->literal8s[0].long1 = literal8.long1;
	literal8_block->next = NULL;

	ms->s.size += 8 * align_multiplier;
	return(output_offset);
}

/*
 * literal8_output() copies the 8 byte literals for the data passed to it into
 * the output file's buffer.  The pointer to the merged section passed to it is
 * used to tell where in the output file this section goes.  Then this routine
 * calls literal8_free() to free up all space used by the data block except the
 * data block itself.
 */
__private_extern__
void
literal8_output(
struct literal8_data *data,
struct merged_section *ms)
{
    unsigned long align_multiplier, i, offset;
    struct literal8_block **p, *literal8_block;

	align_multiplier = 1;
 	if((1 << ms->s.align) > 8)
	    align_multiplier = (1 << ms->s.align) / 8;

	/*
	 * Copy the literals into the output file.
	 */
	offset = ms->s.offset;
	for(p = &(data->literal8_blocks); *p ;){
	    literal8_block = *p;
	    for(i = 0; i < literal8_block->used; i++){
		memcpy(output_addr + offset,
		       literal8_block->literal8s + i,
		       sizeof(struct literal8));
		offset += 8 * align_multiplier;
	    }
	    p = &(literal8_block->next);
	}
#ifndef RLD
	output_flush(ms->s.offset, offset - ms->s.offset);
#endif !defined(RLD)
	literal8_free(data);
}

/*
 * literal8_free() free()'s up all space used by the data block except the
 * data block itself.
 */
__private_extern__
void
literal8_free(
struct literal8_data *data)
{
    struct literal8_block *literal8_block, *next_literal8_block;

	/*
	 * Free all data for this block.
	 */
	for(literal8_block = data->literal8_blocks; literal8_block ;){
	    next_literal8_block = literal8_block->next;
	    free(literal8_block);
	    literal8_block = next_literal8_block;
	}
	data->literal8_blocks = NULL;
}

#ifdef DEBUG
/*
 * print_literal8_data() prints a literal8_data.  Used for debugging.
 */
__private_extern__
void
print_literal8_data(
struct literal8_data *data,
char *indent)
{
    unsigned long i;
    struct literal8_block **p, *literal8_block;

	print("%s8 byte literal data at 0x%x\n", indent, (unsigned int)data);
	if(data == NULL)
	    return;
	print("%s   literal8_blocks 0x%x\n", indent,
	      (unsigned int)(data->literal8_blocks));
	for(p = &(data->literal8_blocks); *p ; p = &(literal8_block->next)){
	    literal8_block = *p;
	    print("%s\tused %lu\n", indent, literal8_block->used);
	    print("%s\tnext 0x%x\n", indent,
		  (unsigned int)(literal8_block->next));
	    print("%s\tliteral8s\n", indent);
	    for(i = 0; i < literal8_block->used; i++){
		print("%s\t    0x%08x 0x%08x\n", indent,
		      (unsigned int)(literal8_block->literal8s[i].long0),
		      (unsigned int)(literal8_block->literal8s[i].long1));
	    }
	}
}

/*
 * literal8_data_stats() prints the literal8_data stats.  Used for tuning.
 */
__private_extern__
void
literal8_data_stats(
struct literal8_data *data,
struct merged_section *ms)
{
	if(data == NULL)
	    return;
	print("literal8 section (%.16s,%.16s) contains:\n",
	      ms->s.segname, ms->s.sectname);
	print("    %lu merged literals \n", ms->s.size / 8);
	print("    from %lu files and %lu total literals from those "
	      "files\n", data->nfiles, data->nliterals);
	print("    average number of literals per file %g\n",
	      (double)((double)data->nliterals / (double)(data->nfiles)));
}
#endif DEBUG
