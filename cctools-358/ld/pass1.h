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
#if defined(__MWERKS__) && !defined(__private_extern__)
#define __private_extern__ __declspec(private_extern)
#endif

/*
 * Global types, variables and routines declared in the file pass1.c.
 *
 * The following include file need to be included before this file:
 * #include "ld.h"
 */

#ifndef RLD
/* the user specified directories to search for -lx filenames, and the number
   of them */
__private_extern__ char **search_dirs;
__private_extern__ unsigned long nsearch_dirs;

/* the standard directories to search for -lx filenames */
__private_extern__ char *standard_dirs[];

/*
 * The user specified directories to search for "-framework Foo" names, and the
 * number of them.  These are specified with -F options.
 */
__private_extern__ char **framework_dirs;
__private_extern__ unsigned long nframework_dirs;

/* the standard framework directories to search for "-framework Foo" names */
__private_extern__ char *standard_framework_dirs[];

/* the pointer to the head of the base object file's segments */
__private_extern__ struct merged_segment *base_obj_segments;

/*
 * The lists of libraries to be search with dynamic library search semantics.
 */
enum library_type {
    DYLIB,
    SORTED_ARCHIVE,
    UNSORTED_ARCHIVE
};
struct dynamic_library {
    enum library_type type;

    /* following used for dynamic libraries */
    char *dylib_name;
    struct dylib_command *dl;
    char *umbrella_name;
    enum bool indirect_twolevel_ref_flagged;
    struct object_file *definition_obj;
    char *dylib_file; /* argument to -dylib_file "install_name:file_name" */
    struct dylib_table_of_contents *tocs;
    struct nlist *symbols;
    char *strings;
    struct dylib_module *mods;
    struct prebound_dylib_command *pbdylib;
    char *linked_modules;

    /* following used for archive libraries */
    char *file_name;
    char *file_addr;
    unsigned long file_size;
    unsigned long nranlibs;
    struct ranlib *ranlibs;
    char *ranlib_strings;
    enum bool rc_trace_archive_printed;

    struct dynamic_library *next;
};

/*
 * The list of dynamic libraries to search.  The list of specified libraries
 * can contain archive libraries when archive libraries appear after dynamic
 * libraries on the link line.
 */
__private_extern__ struct dynamic_library *dynamic_libs;
#endif !defined(RLD)

__private_extern__ void pass1(
    char *filename,
    enum bool lname,
    enum bool base_name,
    enum bool framework_name);
__private_extern__ void merge(
    enum bool dylib_only);
__private_extern__ void check_fat(
    char *file_name,
    unsigned long file_size,
    struct fat_header *fat_header,
    struct fat_arch *fat_archs,
    char *ar_name,
    unsigned long ar_name_size);

#ifndef RLD
__private_extern__ void search_dynamic_libs(
    void);
__private_extern__ void prebinding_check_for_dylib_override_symbols(
    void);
__private_extern__ struct dynamic_library *add_dynamic_lib(
    enum library_type type,
    struct dylib_command *dl,
    struct object_file *definition_obj);
#endif !defined(RLD)

#ifdef RLD
__private_extern__ void merge_base_program(
    char *basefile_name,
    struct mach_header *basefile_addr,
    struct segment_command *seg_linkedit,
    struct nlist *symtab,
    unsigned long nsyms,
    char *strtab,
    unsigned long strsize);
#endif RLD
