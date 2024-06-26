#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include "stuff/ofile.h"
#include "stuff/errors.h"
#include "stuff/seg_addr_table.h"

struct check_block {
    char *install_name;
    enum bool check_result;
    struct seg_addr_table *entry;
};

/* used by error routines as the name of the program */
char *progname = NULL;

static void usage(
    void);

static void check_for_install_name(
    struct ofile *ofile,
    char *arch_name,
    void *cookie);

static void check_for_addresses(
    struct ofile *ofile,
    char *arch_name,
    void *cookie);

/*
 * The check_dylib program.  It takes a dynamic library file, an -install_name
 * argument and a -seg_addr_table argument.  Then it preforms the following
 * checks in the following order and if the specific check fails it returns a
 * specific error code:
 *
 * Check:
 *	Checks the install_name of the dynamic library file against the
 * 	specified -install_name argument and if it does not match it
 * Returns: 2
 *
 * Check:
 *	Checks the specified -seg_addr_table for the install_name and if
 *	not found in the table it
 * Returns: 3
 *
 * Check:
 *	Checks that the specified address in the -seg_addr_table for the
 *	install_name matches the dynamic library file.  If not it
 * Returns: 4
 *
 * Check:
 *	Checks that the specified address in the -seg_addr_table for the
 *	install_name and if it is zero then it
 * Returns: 5
 *
 * If there is any other errors it returns 1 (EXIT_FAILURE).  If all checks
 * pass then it returns 0 (EXIT_SUCCESS).
 */
int
main(
int argc,
char **argv,
char **envp)
{
    unsigned long i, table_size;
    char *install_name, *image_file_name, *seg_addr_table_name;
    struct check_block block;
    struct seg_addr_table *seg_addr_table, *entry;

	progname = argv[0];
	install_name = NULL;
	image_file_name = NULL;
	seg_addr_table = NULL;

	for(i = 1; i < argc; i++){
	    if(argv[i][0] == '-'){
		if(strcmp(argv[i], "-install_name") == 0){
		    if(i + 1 == argc){
			error("missing argument(s) to %s option", argv[i]);
			usage();
		    }
		    if(install_name != NULL){
			error("more than one: %s option", argv[i]);
			usage();
		    }
		    install_name = argv[i+1];
		    i++;
		}
		else if(strcmp(argv[i], "-seg_addr_table") == 0){
		    if(i + 1 == argc){
			error("missing argument(s) to %s option", argv[i]);
			usage();
		    }
		    if(seg_addr_table != NULL){
			error("more than one: %s option", argv[i]);
			usage();
		    }
		    seg_addr_table_name = argv[i+1];
		    seg_addr_table = parse_seg_addr_table(argv[i+1],
					  argv[i], argv[i+1], &table_size);
		    i++;
		}
	    }
	    else{
		if(image_file_name != NULL){
		    error("more than file name specified (%s and %s)",
			  image_file_name, argv[i]);
		    usage();
		}
		image_file_name = argv[i];
	    }
	}
	if(image_file_name == NULL){
	    error("must specify a file name to be checked");
	    usage();
	}
	if(install_name == NULL){
	    error("must specify the -install_name <install_name> option");
	    usage();
	}
	if(seg_addr_table == NULL){
	    error("must specify the -seg_addr_table <table_name> option");
	    usage();
	}

	/*
	 * The first check to perform is checking the install name to match
	 * the -install_name option.
	 */
	block.install_name = install_name;
	block.check_result = TRUE;
	ofile_process(image_file_name, NULL, 0, TRUE,
		      TRUE, TRUE, check_for_install_name, &block);
	if(block.check_result == FALSE)
	    return(2);


	/*
	 * The next check to perform is to see if the install name has an entry
	 * in the specified -seg_addr_table.
	 */
	entry = search_seg_addr_table(seg_addr_table, install_name);
	if(entry == NULL)
	    return(3);

	/*
	 * The next check to perform is to see if the address in the
	 * -seg_addr_table entry matches the dynamic library file.
	 */
	block.entry = entry;
	block.check_result = TRUE;
	ofile_process(image_file_name, NULL, 0, TRUE,
		      TRUE, TRUE, check_for_addresses, &block);
	if(block.check_result == FALSE)
	    return(4);

	/*
	 * The next check to perform is to see address in the -seg_addr_table
	 * for the install_name is zero.
	 */
	if((entry->split == FALSE && entry->seg1addr == 0) ||
	   (entry->split == TRUE && (entry->segs_read_only_addr == 0 ||
				     entry->segs_read_write_addr == 0)) )
	    return(5);

	return(EXIT_SUCCESS);
}

/*
 * usage() prints the current usage message and exits indicating failure.
 */
static
void
usage(
void)
{
	fprintf(stderr, "Usage: %s <file_name> -install_name <install_name> "
		"-seg_addr_table <table_name>\n", progname);
	exit(EXIT_FAILURE);
}

static
void
check_for_install_name(
struct ofile *ofile,
char *arch_name,
void *cookie)
{
    unsigned long i;
    struct check_block *block;
    struct load_command *lc;
    struct dylib_command *dl;
    char *name;

#ifdef DEBUG
	printf("In check_for_install_name() ofile->file_name = %s",
	       ofile->file_name);
	if(arch_name != NULL)
	    printf(" arch_name = %s\n", arch_name);
	else
	    printf("\n");
#endif /* DEBUG */

	block = (struct check_block *)cookie;
	if(ofile->mh == NULL){
	    block->check_result = FALSE;
	    return;
	}

	lc = ofile->load_commands;
	for(i = 0; i < ofile->mh->ncmds; i++){
	    if(lc->cmd == LC_ID_DYLIB){
		dl = (struct dylib_command *)lc;
		name = (char *)lc + dl->dylib.name.offset;
		if(strcmp(name, block->install_name) != 0)
		    block->check_result = FALSE;
		return;
	    }
	    lc = (struct load_command *)((char *)lc + lc->cmdsize);
	}
	block->check_result = FALSE;
	return;
}

static
void
check_for_addresses(
struct ofile *ofile,
char *arch_name,
void *cookie)
{
    unsigned long i, segs_read_only_addr, segs_read_write_addr;
    struct load_command *lc;
    struct segment_command *sg, *first;
    enum bool split;
    struct check_block *block;

#ifdef DEBUG
	printf("In check_for_addresses() ofile->file_name = %s",
	       ofile->file_name);
	if(arch_name != NULL)
	    printf(" arch_name = %s\n", arch_name);
	else
	    printf("\n");
#endif /* DEBUG */

	block = (struct check_block *)cookie;
	if(ofile->mh == NULL){
	    block->check_result = FALSE;
	    return;
	}

	split = (ofile->mh->flags & MH_SPLIT_SEGS) == MH_SPLIT_SEGS;
	if(block->entry->split != split){
	    block->check_result = FALSE;
	    return;
	}
	lc = ofile->load_commands;
	first = NULL;
	segs_read_only_addr = ULONG_MAX;
	segs_read_write_addr = ULONG_MAX;
	for(i = 0; i < ofile->mh->ncmds; i++){
	    if(lc->cmd == LC_SEGMENT){
		sg = (struct segment_command *)lc;
		if(first == NULL){
		    first = sg;
		    if(split == FALSE &&
		       first->vmaddr != block->entry->seg1addr){
			block->check_result = FALSE;
			return;
		    }
		}
		if((sg->initprot & VM_PROT_WRITE) == 0){
		    if(split == TRUE && sg->vmaddr < segs_read_only_addr)
			segs_read_only_addr = sg->vmaddr;
		}
		else{
		    if(split == TRUE && sg->vmaddr < segs_read_write_addr)
			segs_read_write_addr = sg->vmaddr;
		}
	    }
	    lc = (struct load_command *)((char *)lc + lc->cmdsize);
	}
	if(split == TRUE){
	    if(segs_read_only_addr != block->entry->segs_read_only_addr ||
	       segs_read_write_addr != block->entry->segs_read_write_addr){
		block->check_result = FALSE;
		return;
	    }
	}
}
