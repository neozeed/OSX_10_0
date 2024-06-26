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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <limits.h>
#include <libc.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <mach/mach.h>
#include <mach-o/loader.h>
#include <mach-o/nlist.h>
#include <mach-o/stab.h>
#include "stuff/bool.h"
#include "stuff/ofile.h"
#include "stuff/errors.h"
#include "stuff/allocate.h"
#include "stuff/dylib_table.h"
#include "stuff/seg_addr_table.h"
#include "stuff/guess_short_name.h"

/* used by error routines as the name of the program */
char *progname = NULL;

static int exit_status = EXIT_SUCCESS;

/* flags set from the command line arguments */
struct cmd_flags {
    unsigned long nfiles;
    enum bool rldtype;
    enum bool detail;
    enum bool verification;
    enum bool trey;
    enum bool check_dynamic_binary;
};

static void usage(
    void);

static void checksyms(
    struct ofile *ofile,
    char *arch_name,
    void *cookie);

static void check_dynamic_binary(
    struct ofile *ofile,
    char *arch_name,
    enum bool detail,
    enum bool verification);

static void check_dylib(
    struct ofile *ofile,
    char *arch_name,
    enum bool detail,
    enum bool verification,
    enum bool *debug);

/*
 * The dylib table.  This is specified with the -dylib_table option.
 */
static struct dylib_table *dylib_table = NULL;
static char *dylib_table_name = NULL;

/*
 * The segment address table.  This is specified with the -seg_addr_option.
 */
static struct seg_addr_table *seg_addr_table = NULL;
static char *seg_addr_table_name;
static enum bool seg_addr_table_specified = FALSE;

int
main(
int argc,
char **argv,
char **envp)
{
    struct cmd_flags cmd_flags;
    unsigned long i, j, table_size;
    struct arch_flag *arch_flags;
    unsigned long narch_flags;
    enum bool all_archs;
    char **files;

	progname = argv[0];

	arch_flags = NULL;
	narch_flags = 0;
	all_archs = FALSE;

	cmd_flags.nfiles = 0;
	cmd_flags.rldtype = FALSE;
	cmd_flags.detail = FALSE;
	cmd_flags.verification = FALSE;
	cmd_flags.trey = FALSE;
	cmd_flags.check_dynamic_binary = TRUE;

        files = allocate(sizeof(char *) * argc);
	for(i = 1; i < argc; i++){
	    if(argv[i][0] == '-'){
		if(argv[i][1] == '\0'){
		    for( ; i < argc; i++)
			files[cmd_flags.nfiles++] = argv[i];
		    break;
		}
		else if(strcmp(argv[i], "-arch") == 0){
		    if(i + 1 == argc){
			error("missing argument(s) to %s option", argv[i]);
			usage();
		    }
		    if(strcmp("all", argv[i+1]) == 0){
			all_archs = TRUE;
		    }
		    else{
			arch_flags = reallocate(arch_flags,
				(narch_flags + 1) * sizeof(struct arch_flag));
			if(get_arch_from_flag(argv[i+1],
					      arch_flags + narch_flags) == 0){
			    error("unknown architecture specification flag: "
				  "%s %s", argv[i], argv[i+1]);
			    arch_usage();
			    usage();
			}
			narch_flags++;
		    }
		    i++;
		}
		else if(strcmp(argv[i], "-dylib_table") == 0){
		    if(i + 1 == argc){
			error("missing argument(s) to %s option", argv[i]);
			usage();
		    }
		    if(dylib_table_name != NULL){
			error("more than one: %s option", argv[i]);
			usage();
		    }
		    dylib_table_name = argv[i+1];
		    dylib_table = parse_dylib_table(argv[i+1], argv[i],
						    argv[i+1]);
		    i++;
		}
		else if(strcmp(argv[i], "-seg_addr_table") == 0){
		    if(i + 1 == argc){
			error("missing argument(s) to %s option", argv[i]);
			usage();
		    }
		    if(seg_addr_table_specified == TRUE){
			error("more than one: %s option", argv[i]);
			usage();
		    }
		    seg_addr_table_specified = TRUE;
		    seg_addr_table_name = argv[i+1];
		    seg_addr_table = parse_seg_addr_table(argv[i+1],
					      argv[i], argv[i+1], &table_size);
		    i++;
		}
		else{
		    for(j = 1; argv[i][j] != '\0'; j++){
			switch(argv[i][j]){
			case 'r':
			    cmd_flags.rldtype = TRUE;
			    break;
			case 'd':
			    cmd_flags.detail = TRUE;
			    break;
			case 'v':
			    cmd_flags.verification = TRUE;
			    break;
			case 't':
			    cmd_flags.trey = TRUE;
			    break;
			case 'b':
			    cmd_flags.check_dynamic_binary = TRUE;
			    break;
			default:
			    error("invalid argument -%c", argv[i][j]);
			    usage();
			}
		    }
		}
		continue;
	    }
	    files[cmd_flags.nfiles++] = argv[i];
	}

	if(arch_flags == NULL)
	    all_archs = TRUE;

	if(cmd_flags.nfiles != 1)
	    usage();

	for(i = 0; i < cmd_flags.nfiles; i++)
	    ofile_process(files[i], arch_flags, narch_flags, all_archs, TRUE,
			  TRUE, checksyms, &cmd_flags);

	if(errors == 0)
	    return(exit_status);
	else
	    return(EXIT_FAILURE);
}

/*
 * usage() prints the current usage message and exits indicating failure.
 */
static
void
usage(
void)
{
	fprintf(stderr, "Usage: %s [-r] [-d] [-t] [-b] [-] [-dylib_table file] "
		"[-seg_addr_table file] [[-arch <arch_flag>] ...] file\n",
		progname);
	exit(EXIT_FAILURE);
}

/*
 * checksyms() is the routine that gets called by ofile_process() to process
 * single object files.
 */
static
void
checksyms(
struct ofile *ofile,
char *arch_name,
void *cookie)
{
    struct cmd_flags *cmd_flags;
    unsigned long i;
    struct load_command *lc;
    struct symtab_command *st;
    struct nlist *symbols;
    unsigned long nsymbols;
    char *strings;
    unsigned long strsize;
    unsigned long nfiledefs, ncats, nlocal, nstabs, nfun;
    unsigned long filedef_strings, cat_strings, local_strings, stab_strings;
    enum bool debug;

	if(ofile->mh == NULL)
	    return;

	debug = FALSE;
	cmd_flags = (struct cmd_flags *)cookie;

	if(cmd_flags->check_dynamic_binary == TRUE)
	    if((ofile->mh->flags & MH_DYLDLINK) == MH_DYLDLINK)
		check_dynamic_binary(ofile, arch_name, cmd_flags->detail,
				     cmd_flags->verification);

	if(ofile->mh->filetype == MH_DYLIB)
	    check_dylib(ofile, arch_name, cmd_flags->detail,
			cmd_flags->verification, &debug);

	st = NULL;
	lc = ofile->load_commands;
	for(i = 0; i < ofile->mh->ncmds; i++){
	    if(st == NULL && lc->cmd == LC_SYMTAB){
		st = (struct symtab_command *)lc;
	    }
	    lc = (struct load_command *)((char *)lc + lc->cmdsize);
	}
	if(st == NULL || st->nsyms == 0){
	    return;
	}

	if(cmd_flags->rldtype == FALSE &&
	   cmd_flags->trey == FALSE &&
	   (ofile->mh->flags & MH_DYLDLINK) == 0 &&
	    (ofile->file_type != OFILE_FAT ||
	     ofile->arch_type != OFILE_ARCHIVE) &&
	    ofile->file_type != OFILE_ARCHIVE &&
	    ofile->mh->filetype != MH_FVMLIB){
	    if(st->nsyms == 0)
		return;

	    if(cmd_flags->detail == TRUE){
		if(arch_name != NULL)
		    printf("(for architecture %s):", arch_name);
		if(ofile->member_ar_hdr != NULL){
		    printf("%s:%.*s:", ofile->file_name,
			   (int)ofile->member_name_size, ofile->member_name);
		}
		else
		    printf("%s:", ofile->file_name);
		printf(" has %lu symbols and %lu string bytes\n", st->nsyms,
		       st->strsize);
	    }
	    if(cmd_flags->verification == TRUE)
		printf("unstripped_binary\n");
	    exit_status = EXIT_FAILURE;
	    return;
	}

	symbols = (struct nlist *)(ofile->object_addr + st->symoff);
	nsymbols = st->nsyms;
	if(ofile->object_byte_sex != get_host_byte_sex())
	    swap_nlist(symbols, nsymbols, get_host_byte_sex());

	strings = ofile->object_addr + st->stroff;
	strsize = st->strsize;
	for(i = 0; i < nsymbols; i++){
	    if(symbols[i].n_un.n_strx == 0)
		symbols[i].n_un.n_name = "";
	    else if(symbols[i].n_un.n_strx < 0 ||
		    symbols[i].n_un.n_strx > st->strsize)
		symbols[i].n_un.n_name = "bad string index";
	    else
		symbols[i].n_un.n_name = symbols[i].n_un.n_strx + strings;

	    if((symbols[i].n_type & N_TYPE) == N_INDR){
		if(symbols[i].n_value == 0)
		    symbols[i].n_value = (long)"";
		else if(symbols[i].n_value > st->strsize)
		    symbols[i].n_value = (long)"bad string index";
		else
		    symbols[i].n_value =
				(long)(symbols[i].n_value + strings);
	    }
	}

	nfiledefs = 0;
	ncats = 0;
	nlocal = 0;
	nstabs = 0;
	nfun = 0;
	filedef_strings = 0;
	cat_strings = 0;
	local_strings = 0;
	stab_strings = 0;
	for(i = 0; i < nsymbols; i++){
	    if(ofile->mh->filetype == MH_EXECUTE){
		if(symbols[i].n_type == (N_ABS | N_EXT) &&
		   symbols[i].n_value == 0){
		    if(strncmp(symbols[i].n_un.n_name, ".file_definition_",
			       sizeof(".file_definition_") - 1) == 0){
			nfiledefs++;
			filedef_strings += strlen(symbols[i].n_un.n_name);
		    }
		    if(strncmp(symbols[i].n_un.n_name, ".objc_category_name_",
			       sizeof(".objc_category_name_") - 1) == 0){
			ncats++;
			cat_strings += strlen(symbols[i].n_un.n_name);
		    }
		}
	    }
	    if((symbols[i].n_type & N_EXT) == 0){
		nlocal++;
		local_strings += strlen(symbols[i].n_un.n_name);
	    }
	    if(symbols[i].n_type & N_STAB){
		nstabs++;
		stab_strings += strlen(symbols[i].n_un.n_name);
		if(symbols[i].n_type == N_FUN)
		    nfun++;
	    }
	}

	if(nfiledefs == 0 && ncats == 0 && nlocal == 0 && nstabs == 0)
	    return;
	if(cmd_flags->rldtype == TRUE && nstabs == 0)
	    return;
	if((ofile->mh->flags & MH_DYLDLINK) == MH_DYLDLINK &&
	   (nstabs == 0 && nlocal == 0))
	    return;
	if(nstabs == 0 &&
	   ((ofile->file_type == OFILE_FAT &&
	     ofile->arch_type == OFILE_ARCHIVE) ||
	    ofile->file_type == OFILE_ARCHIVE ||
	    ofile->mh->filetype == MH_FVMLIB))
	    return;
	if((ofile->mh->filetype == MH_DYLIB ||
	    ofile->mh->filetype == MH_FVMLIB) &&
	    (nfun == 0 || debug == TRUE))
	    return;

	if(cmd_flags->detail == TRUE){
	    if(arch_name != NULL)
		printf("(for architecture %s):", arch_name);
	    if(ofile->member_ar_hdr != NULL){
		printf("%s:%.*s:", ofile->file_name,
		       (int)ofile->member_name_size, ofile->member_name);
	    }
	    else
		printf("%s:", ofile->file_name);
	    printf("\n");
	    if(nfiledefs != 0)
		printf(" has %lu .file_definition_ symbols and %lu string "
		       "bytes\n", nfiledefs, filedef_strings);
	    if(ncats != 0)
		printf(" has %lu .objc_category_name_ symbols and %lu string "
		       "bytes\n", ncats, cat_strings);
	    if(nlocal != 0)
		printf(" has %lu local symbols and %lu string "
		       "bytes\n", nlocal, local_strings);
	    if(nstabs != 0)
		printf(" has %lu debugging symbols and %lu string "
		       "bytes\n", nstabs, stab_strings);
	}
	if(cmd_flags->verification == TRUE)
	    printf("unstripped_binary\n");
	if(cmd_flags->trey == TRUE && nstabs == 0)
	    return;

	exit_status = EXIT_FAILURE;
	return;
}

/*
 * check_dynamic_binary checks to see a dynamic is built correctly.  That is it
 * has no read-only-relocs, and is prebound.
 */
static
void
check_dynamic_binary(
struct ofile *ofile,
char *arch_name,
enum bool detail,
enum bool verification)
{
    unsigned long i, j, section_attributes;
    struct load_command *lc;
    struct segment_command *sg;
    struct section *s;

	/*
	 * First check for relocation entries in read only segments.
	 */
	lc = ofile->load_commands;
	for(i = 0; i < ofile->mh->ncmds; i++){
	    if(lc->cmd == LC_SEGMENT){
		sg = (struct segment_command *)lc;
		s = (struct section *)((char *)lc +
					sizeof(struct segment_command));
		for(j = 0; j < sg->nsects; j++){
		    section_attributes = s->flags & SECTION_ATTRIBUTES;
		    if((sg->initprot & VM_PROT_WRITE) == 0 &&
		       ((section_attributes & S_ATTR_EXT_RELOC) != 0 ||
		        (section_attributes & S_ATTR_LOC_RELOC) != 0)){
			if(detail == TRUE){
			    if(arch_name != NULL)
				printf("(for architecture %s):", arch_name);
			    printf("%s: relocation entries in read-only section"
				   " (%.16s,%.16s)\n", ofile->file_name,
				   s->segname, s->sectname);
			}
			if(verification == TRUE)
			    printf("read_only_relocs\n");
			exit_status = EXIT_FAILURE;
		    }
		    s++;
		}
	    }
	    lc = (struct load_command *)((char *)lc + lc->cmdsize);
	}

	/*
	 * If the file is an executable or a dynamic library and has no
	 * undefined references it should be prebound.
	 */
	if((ofile->mh->filetype == MH_EXECUTE ||
	    ofile->mh->filetype == MH_DYLIB) &&
	   (ofile->mh->flags & MH_NOUNDEFS) == MH_NOUNDEFS){

	    if((ofile->mh->flags & MH_PREBOUND) != MH_PREBOUND){
		if(detail == TRUE){
		    if(arch_name != NULL)
			printf("(for architecture %s):", arch_name);
		    printf("%s: is not prebound\n", ofile->file_name);
		}
		if(verification == TRUE)
		    printf("not_prebound\n");
		exit_status = EXIT_FAILURE;
	    }
	}
}

/*
 * check_dylib() checks the dynamic library against the Apple conventions.
 * This includes the linked address and the setting of compatibility and
 * current versions.
 */
static
void
check_dylib(
struct ofile *ofile,
char *arch_name,
enum bool detail,
enum bool verification,
enum bool *debug)
{
    unsigned long i, seg1addr, segs_read_only_addr, segs_read_write_addr;
    unsigned long table_size;
    struct load_command *lc;
    struct segment_command *sg;
    struct dylib_command *dlid;
    char *install_name, *suffix;
    struct stat stat_buf;
    struct seg_addr_table *entry;
    char *env_seg_addr_table_name, *short_name;
    enum bool is_framework;

	*debug = FALSE;

	/*
	 * First pick up the linked address and the dylib id command.
	 */
	seg1addr = ULONG_MAX;
	segs_read_only_addr = ULONG_MAX;
	segs_read_write_addr = ULONG_MAX;
	dlid = NULL;
	install_name = NULL;
	lc = ofile->load_commands;
	for(i = 0; i < ofile->mh->ncmds; i++){
	    switch(lc->cmd){
	    case LC_SEGMENT:
		sg = (struct segment_command *)lc;
		if(ofile->mh->flags & MH_SPLIT_SEGS){
		    if((sg->initprot & VM_PROT_WRITE) == 0){
			if(sg->vmaddr < segs_read_only_addr)
			    segs_read_only_addr = sg->vmaddr;
		    }
		    else{
			if(sg->vmaddr < segs_read_write_addr)
			    segs_read_write_addr = sg->vmaddr;
		    }
		}
		else{
		    if(sg->vmaddr < seg1addr)
			seg1addr = sg->vmaddr;
		}
		break;
	    case LC_ID_DYLIB:
		if(dlid == NULL){
		    dlid = (struct dylib_command *)lc;
		    install_name = (char *)dlid + dlid->dylib.name.offset;
		}
		break;
	    }
	    lc = (struct load_command *)((char *)lc + lc->cmdsize);
	}
	if(dlid == NULL){
	    printf("%s: ", ofile->file_name);
	    if(arch_name != NULL)
		printf("(for architecture %s): ", arch_name);
	    printf("malformed dynamic library (no LC_ID_DYLIB command)\n");
	    exit_status = EXIT_FAILURE;
	    return;
	}

	/*
	 * Check for compatibility and current version being set (non-zero).
	 */
	if(dlid->dylib.compatibility_version == 0){
	    if(detail == TRUE){
		printf("%s: ", ofile->file_name);
		if(arch_name != NULL)
		    printf("(for architecture %s): ", arch_name);
		printf("compatibility_version for dynamic library not set\n");
	    }
	    if(verification == TRUE)
		printf("no_compatibility_version\n");
	    exit_status = EXIT_FAILURE;
	}
	if(dlid->dylib.current_version == 0){
	    if(detail == TRUE){
		printf("%s: ", ofile->file_name);
		if(arch_name != NULL)
		    printf("(for architecture %s): ", arch_name);
		printf("current_version for dynamic library not set\n");
	    }
	    if(verification == TRUE)
		printf("no_current_version\n");
	    exit_status = EXIT_FAILURE;
	}

	short_name = guess_short_name(install_name, &is_framework, &suffix);
	if(suffix != NULL && strcmp(suffix, "_debug") == 0)
	    *debug = TRUE;
	else
	    *debug = FALSE;

	/*
	 * If there is no -seg_addr_table argument then check to see if the
	 * environment variable LD_SEG_ADDR_TABLE (used to set the addresses
	 * when building) is set and use that as the segment address table if
	 * that file exists.  THIS IS A HACK until some time in the future
	 * we switch over to using a segment address table.
	 */
	if(seg_addr_table == NULL){
#ifndef FUTURE
	    env_seg_addr_table_name = getenv("LD_SEG_ADDR_TABLE");
	    if(env_seg_addr_table_name != NULL){
		if(stat(env_seg_addr_table_name, &stat_buf) != -1){
		    seg_addr_table_name = env_seg_addr_table_name;
		    seg_addr_table = parse_seg_addr_table(
					    seg_addr_table_name,
					    "LD_SEG_ADDR_TABLE",
					    "environment variable",
					    &table_size);
		}
	    } 
#else /* defined(FUTURE) */
	    /*
	     * In the future when we cut over to using the segment address table
	     * instead of the dylib table we will open the default segment
	     * address table if none were specified via the -seg_addr_table.
	     */
	    seg_addr_table = parse_default_seg_addr_table(
				&seg_addr_table_name, &table_size);
#endif
	}
	/*
	 * Use the segment address table if there is one to check the addresses
	 * if not falling back to using the the dylib table.
	 */
	if(seg_addr_table != NULL){
	    entry = search_seg_addr_table(seg_addr_table, install_name);
	    if(entry != NULL){
		if(entry->split == TRUE){
		    if(entry->segs_read_only_addr != segs_read_only_addr ||
		       entry->segs_read_write_addr != segs_read_write_addr){
			if(detail == TRUE){
			    printf("%s: ", ofile->file_name);
			    if(arch_name != NULL)
				printf("(for architecture %s): ", arch_name);
			    printf("dynamic library (%s) not linked at its "
				   "expected segs_read_only_addr (0x%x) and "
				   "segs_read_write_addr (0x%x)\n",
				   install_name, 
				   (unsigned int)entry->segs_read_only_addr,
				   (unsigned int)entry->segs_read_write_addr);
			}
			if(verification == TRUE)
			    printf("dylib_wrong_address\n");
			exit_status = EXIT_FAILURE;
		    }
		    return;
		}
		else{
		    if(entry->seg1addr != seg1addr){
			if(detail == TRUE){
			    printf("%s: ", ofile->file_name);
			    if(arch_name != NULL)
				printf("(for architecture %s): ", arch_name);
			    printf("dynamic library (%s) not linked at its "
				   "expected seg1addr (0x%x)\n",
				   install_name,
				   (unsigned int)entry->seg1addr);
			}
			if(verification == TRUE)
			    printf("dylib_wrong_address\n");
			exit_status = EXIT_FAILURE;
		    }
		    return;
		}
	    }
	    else{
		/*
		 * This case is that there was a segment address table but this
		 * library did not have an entry in it.  In this case the B&I
		 * tools will automaticly add it to the table and assign it an
		 * address.  So NO error is to be generated in this case and
		 * the dylib table is not to be used as the segment address
		 * table exists.
		 */
		return;
	    }
	}

	/*
	 * If there is no dylib table open the default table and use it.
	 */
	if(dylib_table == NULL)
	    dylib_table = parse_default_dylib_table(&dylib_table_name);

	/*
	 * Now short_name points to the name of the library.  This short_name 
	 * must be found in the table and the seg1addr must match with what is
	 * in the table.
	 */
	if(short_name != NULL){
	    for(i = 0; dylib_table[i].name != NULL; i++){
		if(strcmp(dylib_table[i].name, short_name) == 0)
		    break;
	    }
	}
	if(short_name == NULL || dylib_table[i].name == NULL){
	    if(detail == TRUE){
		printf("%s: ", ofile->file_name);
		if(arch_name != NULL)
		    printf("(for architecture %s): ", arch_name);
		printf("dynamic library name (%s) unknown to %s and "
		    "checking of its seg1addr can't be done ",
		    short_name != NULL ? short_name : install_name, progname);
		printf("(dylib table: %s must be updated to include this "
		       "library, contact Build & Integration to assign an "
		       "address to this library)\n", dylib_table_name);
	    }
	    if(verification == TRUE)
		printf("unknown_dylib\n");
	    exit_status = EXIT_FAILURE;
	}
	else if(dylib_table[i].seg1addr != seg1addr){
	    if(detail == TRUE){
		printf("%s: ", ofile->file_name);
		if(arch_name != NULL)
		    printf("(for architecture %s): ", arch_name);
		printf("dynamic library (%s) not linked at its expected "
		       "seg1addr (0x%x)\n", short_name,
		       (unsigned int)dylib_table[i].seg1addr);
	    }
	    if(verification == TRUE)
		printf("dylib_wrong_address\n");
	    exit_status = EXIT_FAILURE;
	}

	return;
}
