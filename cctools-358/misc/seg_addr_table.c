#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <limits.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>

#include "stuff/ofile.h"
#include "stuff/allocate.h"
#include "stuff/round.h"
#include "stuff/errors.h"
#include "stuff/seg_addr_table.h"
#include "stuff/guess_short_name.h"
#include "stuff/dylib_table.h"
#include "stuff/dylib_roots.h"

/*
 * These are the default addresses use when re-laying out the images.
 */
#define DEFAULT_SEG1ADDR	0x41300000
#define DEFAULT_READ_ONLY_ADDR	0x70000000
#define DEFAULT_READ_WRITE_ADDR	0x80000000
/*
 * The 256meg regions can only have at most 128meg allocated from them leaving
 * half of them for the "alternate" area.  So if any library starts at an
 * address and ends at an address with these top bits different then we have
 * overflowed the lower 128meg part that is to be allocated.
 */
#define SPLIT_OVERFLOW_MASK 0xf8000000

/*
 * For intel the kernel limits the address space to the lower 3 gig so we don't
 * want to put any addresses above this.
 */
#define MAX_ADDR 0xc0000000

/*
 * These are just used in the layout info structs created in the 
 * sorted_flat_layout_info array so that next_flat_seg1addr() will step over
 * them.
 */
#define READ_ONLY_SEGMENT_NAME "<< read-only segment for split libraries >>"
#define READ_WRITE_SEGMENT_NAME "<< read-write segment for split libraries >>"

/*
 * These are the default factor (power of 2) to multiply the sizes of the
 * segments of of an image and what to round that size up to to allow the
 * image grow without overlapping the next image.
 *
 * The two 256meg regions for split libraries will overflow when the doing
 * a -relayout of the Gonzo1H libraries and a DEFAULT_FACTOR of more than
 * 1 or trying to used the SYMROOT files. So the DEFAULT_FACTOR is not used for
 * split libraries and only the DEFAULT_ROUND.
 */
#define DEFAULT_FACTOR 0x1  /* power of 2, so this is 1<<1 = 2 */
#define DEFAULT_ROUND 0x10000

/* used by error routines as the name of the program */
char *progname = NULL;

/*
 * This structure is passed to the routine max_size to accumulate the maximum
 * size of the segments a given image for the architectures being considered.
 */
struct max_sizes {
    unsigned long all;
    unsigned long read_only;
    unsigned long read_write;
};

/*
 * This structure is used when re-laying out of the table.  Dynamic libraries
 * which are laid out flat that have the same name but with differ only in
 * suffix (like no suffix, _profile, _debug, etc) are given the same address.
 * The base name must be the same to say that it is the same dynamic library.
 * That is /lib/libx.A.dylib is not the same as /usr/local/libx_profile.A.dylib.
 * For dynamic libraries which are laid out split each one is given a different
 * address.
 */
struct layout_info {
    char *install_name;
    char *image_file_name;
    struct max_sizes max_sizes;
    enum bool split;
    /* used only for flat layouts */
    enum bool assigned;
    char *short_name;
    unsigned long seg1addr;
    /* used only for split layouts */
    unsigned long segs_read_only_addr;
    unsigned long segs_read_write_addr;
    /* the current entry when calling sizes_and_addresses() */
    struct seg_addr_table *current_entry;
};

/*
 * This structure has the info needed to re-layout or update the
 * seg_addr_table.
 */
struct info {
    /* the file name of the input seg_addr_table */
    char *seg_addr_table_name;

    /* the parsed seg_addr_table */
    struct seg_addr_table *seg_addr_table;
    unsigned long table_size;

    /* the array of pointers to info for layout for each entry */
    struct layout_info **layout_info;

    /* the arrays of pointers to sorted layout info for flat/split entries */
    struct layout_info **sorted_flat_layout_info;
    unsigned long nsorted_flat;
    struct layout_info **sorted_split_read_only_layout_info;
    struct layout_info **sorted_split_read_write_layout_info;
    unsigned long nsorted_split;

    /* The first segment address for non-split images */
    unsigned long seg1addr;
    unsigned long next_flat_line;

    /* read-only and read-write segment addresses for split images*/
    unsigned long start_segs_read_only_addr;
    unsigned long start_segs_read_write_addr;
    unsigned long segs_read_only_addr;
    unsigned long segs_read_write_addr;
    unsigned long next_split_line;

    /* the specified -arch flags */
    struct arch_flag *arch_flags;
    unsigned long narch_flags;
    enum bool all_archs;

    /* the file name of the output seg_addr_table */
    char *output_file_name;

    /* the factor and amount to round applied to the sizes for layout */
    unsigned long factor;
    unsigned long round;

    /* the -release argument for translating to SYMROOT files */
    char *release_name;

    /* the -disablewarnings flag */
    enum bool disablewarnings;
};

static void usage(
    void);

static FILE * create_output_file(
    char *output_file_name);

static int qsort_flat(
    const struct layout_info **p1,
    const struct layout_info **p2);

static int qsort_split_read_only(
    const struct layout_info **p1,
    const struct layout_info **p2);

static int qsort_split_read_write(
    const struct layout_info **p1,
    const struct layout_info **p2);

static void flat_overlap_error(
    struct info *info,
    unsigned long i1,
    unsigned long i2,
    enum bool next_address);

static void split_overlap_error(
    struct info *info,
    unsigned long i1,
    unsigned long i2,
    struct layout_info **sorted_layout_info,
    char *segment,
    enum bool next_address);

static struct seg_addr_table * search_seg_addr_table_for_fixed(
    struct seg_addr_table *seg_addr_table,
    unsigned long seg1addr,
    unsigned long size);

static unsigned long next_flat_seg1addr(
    struct info *info,
    unsigned long size);

static char * get_image_file_name(
    struct info *info,
    char *install_name,
    enum bool split);

static void new_table_processor(
    struct seg_addr_table *entry,
    FILE *out_fp,
    void *cookie);

static void sizes_and_addresses(
    struct ofile *ofile,
    char *arch_name,
    void *cookie);

static void dylib_table_processor(
    struct seg_addr_table *entry,
    FILE *out_fp,
    void *cookie);

static void get_seg1addr(
    struct ofile *ofile,
    char *arch_name,
    void *cookie);


/*
 * The seg_addr_table program.  It takes a file which contains the starting
 * segment address of images and can update or re-layout the table based on
 * the images it finds on the machine it is running on.
 */
int
main(
int argc,
char **argv,
char **envp)
{
    unsigned long i, j, used, max, size;
    char *endp, *user, *dylib_table_name, *base_name, *short_name,
	 *image_file_name;
    struct dylib_table *dylib_table;
    FILE *out_fp;
    time_t tloc;
    struct info info;
    struct seg_addr_table *entry;
    struct layout_info *layout_info;
    enum bool seg1addr_specified, segs_read_only_addr_specified,
	      segs_read_write_addr_specified, relayout, update, create,
	      checkonly;
    enum bool found, is_framework, next_flat, next_split;
    enum bool operation_specified, from_dylib_table, create_dylib_table;
    char *install_name, *has_suffix;
    struct stat stat_buf;

	progname = argv[0];

	dylib_table_name = NULL;
	dylib_table = NULL;

	info.seg_addr_table = NULL;
	info.seg_addr_table_name = NULL;
	info.table_size = 0;

	info.layout_info = NULL;

	info.arch_flags = NULL;
	info.narch_flags = 0;
	info.all_archs = FALSE;
	info.disablewarnings = FALSE;

	operation_specified = FALSE;
	relayout = FALSE;
	update = FALSE;
	checkonly = FALSE;
	create = FALSE;
	from_dylib_table = FALSE;
	create_dylib_table = FALSE;

	info.output_file_name = NULL;
	out_fp = NULL;

	seg1addr_specified = FALSE;
	segs_read_only_addr_specified = FALSE;
	segs_read_write_addr_specified = FALSE;

	info.seg1addr = DEFAULT_SEG1ADDR;
	info.segs_read_only_addr = DEFAULT_READ_ONLY_ADDR;
	info.start_segs_read_only_addr = info.segs_read_only_addr;
	info.segs_read_write_addr = DEFAULT_READ_WRITE_ADDR;
	info.start_segs_read_write_addr = info.segs_read_write_addr;
	info.factor = DEFAULT_FACTOR;
	info.round = DEFAULT_ROUND;

	info.release_name = NULL;

	for(i = 1; i < argc; i++){
	    if(argv[i][0] == '-'){
		if(strcmp(argv[i], "-relayout") == 0){
		    if(operation_specified == TRUE){
			error("more than one operation specified");
			usage();
		    }
		    operation_specified = TRUE;
		    relayout = TRUE;
		}
		else if(strcmp(argv[i], "-update") == 0){
		    if(operation_specified == TRUE){
			error("more than one operation specified");
			usage();
		    }
		    operation_specified = TRUE;
		    update = TRUE;
		}
		else if(strcmp(argv[i], "-checkonly") == 0){
		    if(operation_specified == TRUE){
			error("more than one operation specified");
			usage();
		    }
		    operation_specified = TRUE;
		    checkonly = TRUE;
		}
		else if(strcmp(argv[i], "-create") == 0){
		    if(operation_specified == TRUE){
			error("more than one operation specified");
			usage();
		    }
		    operation_specified = TRUE;
		    create = TRUE;
		}
		else if(strcmp(argv[i], "-from_dylib_table") == 0){
		    if(operation_specified == TRUE){
			error("more than one operation specified");
			usage();
		    }
		    operation_specified = TRUE;
		    from_dylib_table = TRUE;
		}
		else if(strcmp(argv[i], "-create_dylib_table") == 0){
		    if(operation_specified == TRUE){
			error("more than one operation specified");
			usage();
		    }
		    operation_specified = TRUE;
		    create_dylib_table = TRUE;
		}
		else if(strcmp(argv[i], "-seg_addr_table") == 0){
		    if(i + 1 == argc){
			error("missing argument(s) to %s option", argv[i]);
			usage();
		    }
		    if(info.seg_addr_table != NULL){
			error("more than one: %s option", argv[i]);
			usage();
		    }
		    info.seg_addr_table_name = argv[i+1];
		    info.seg_addr_table = parse_seg_addr_table(argv[i+1],
					  argv[i], argv[i+1], &info.table_size);
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
		/* specify the address (in hex) of the first segment
		   -seg1addr <address> */
		else if(strcmp(argv[i], "-seg1addr") == 0){
		    if(i + 1 >= argc){
			error("%s: argument missing", argv[i]);
			usage();
		    }
		    if(seg1addr_specified == TRUE){
			error("more than one: %s option", argv[i]);
			usage();
		    }
		    info.seg1addr = strtoul(argv[i+1], &endp, 16);
		    if(*endp != '\0')
			fatal("address for %s %s not a proper "
			      "hexadecimal number", argv[i], argv[i+1]);
		    seg1addr_specified = TRUE;
		    i++;
		}
		/* specify the address (in hex) of the read-only segments
		   -segs_read_only_addr <address> */
		else if(strcmp(argv[i], "segs_read_only_addr") == 0){
		    if(i + 1 >= argc){
			error("%s: argument missing", argv[i]);
			usage();
		    }
		    if(segs_read_only_addr_specified == TRUE){
			error("more than one: %s option", argv[i]);
			usage();
		    }
		    info.segs_read_only_addr =
			strtoul(argv[i+1], &endp, 16);
		    info.start_segs_read_only_addr = info.segs_read_only_addr;
		    if(*endp != '\0')
			fatal("address for %s %s not a proper "
			      "hexadecimal number", argv[i], argv[i+1]);
		    segs_read_only_addr_specified = TRUE;
		    i++;
		}
		/* specify the address (in hex) of the read-write segments
		   -segs_read_write_addr <address> */
		else if(strcmp(argv[i], "segs_read_write_addr") == 0){
		    if(i + 1 >= argc){
			error("%s: argument missing", argv[i]);
			usage();
		    }
		    if(segs_read_write_addr_specified == TRUE){
			error("more than one: %s option", argv[i]);
			usage();
		    }
		    info.segs_read_write_addr =
			strtoul(argv[i+1], &endp, 16);
		    info.start_segs_read_write_addr = info.segs_read_write_addr;
		    if(*endp != '\0')
			fatal("address for %s %s not a proper "
			      "hexadecimal number", argv[i], argv[i+1]);
		    segs_read_write_addr_specified = TRUE;
		    i++;
		}
		else if(strcmp(argv[i], "-arch") == 0){
		    if(i + 1 == argc){
			error("missing argument(s) to %s option", argv[i]);
			usage();
		    }
		    if(strcmp("all", argv[i+1]) == 0){
			info.all_archs = TRUE;
		    }
		    else{
			info.arch_flags = reallocate(info.arch_flags,
						     (info.narch_flags + 1) *
						     sizeof(struct arch_flag));
			if(get_arch_from_flag(argv[i+1],
				      info.arch_flags + info.narch_flags) == 0){
			    error("unknown architecture specification flag: "
				  "%s %s", argv[i], argv[i+1]);
			    arch_usage();
			    usage();
			}
			info.narch_flags++;
		    }
		    i++;
		}
		else if(strcmp(argv[i], "-o") == 0){
		    if(i + 1 == argc){
			error("missing argument(s) to %s option", argv[i]);
			usage();
		    }
		    if(info.output_file_name != NULL){
			error("more than one: %s %s option", argv[i],argv[i+1]);
			usage();
		    }
		    info.output_file_name = argv[i+1];
		    i++;
		}
		else if(strcmp(argv[i], "-release") == 0){
		    if(i + 1 == argc){
			error("missing argument(s) to %s option", argv[i]);
			usage();
		    }
		    if(info.release_name != NULL){
			error("more than one: %s option", argv[i]);
			usage();
		    }
		    info.release_name = argv[i+1];
		    i++;
		}
		else if(strcmp(argv[i], "-disablewarnings") == 0){
		    info.disablewarnings = TRUE;
		}
		else{
		    error("unknown option: %s\n", argv[i]);
		    usage();
		}
	    }
	    else{
		error("unknown argument: %s\n", argv[i]);
		usage();
	    }
	}
	/* check the sizes of all architectures by default */
	if(info.narch_flags == 0)
	    info.all_archs = TRUE;

	if(operation_specified == FALSE){
	    error("no operation specified");
	    usage();
	}

	if(checkonly == FALSE && info.output_file_name == NULL){
	    error("must specify an output with the -o option");
	    usage();
	}

	if((checkonly == TRUE || relayout == TRUE) &&
	   info.disablewarnings == TRUE){
	    warning("-disablewarnings has no effect with -checkonly or "
		    "-relayout");
	    info.disablewarnings = FALSE;
	}

	if(update == TRUE){
	    if(seg1addr_specified == TRUE)
		fatal("can't specify -seg1addr option when -update is used");
	    if(segs_read_only_addr_specified == TRUE)
		fatal("can't specify -segs_read_only_addr option when -update "
		      "is used");
	    if(segs_read_write_addr_specified == TRUE)
		fatal("can't specify -segs_read_write_addr option when -update "
		      "is used");
	}

	/*
	 * If there is no seg_addr_table open the default table and use it.
	 */
	if(info.seg_addr_table == NULL && from_dylib_table == FALSE)
	    info.seg_addr_table = parse_default_seg_addr_table(
				&info.seg_addr_table_name, &info.table_size);

#ifdef DEBUG
	if(dylib_table == NULL)
	    dylib_table = parse_default_dylib_table(&dylib_table_name);

	if(dylib_table != NULL){
	    printf("dylib_table %s\n", dylib_table_name);
	    for(i = 0; dylib_table[i].name != NULL; i++){
		printf("    %d\n", i);
		printf("\tname = %s\n", dylib_table[i].name);
		printf("\tseg1addr = 0x%x\n",
			(unsigned int)dylib_table[i].seg1addr);
		install_name = guess_dylib_install_name(dylib_table[i].name);
		if(install_name != NULL)
		    printf("\tinstall_name = %s\n", install_name);
	    }
	}

	if(info.seg_addr_table != NULL){
	    printf("seg_addr_table %s\n", info.seg_addr_table_name);
	    for(i = 0; info.seg_addr_table[i].install_name != NULL; i++){
		printf("    %d\n", i);
		printf("\tinstall_name = %s\n",
		       info.seg_addr_table[i].install_name);
		printf("\tsplit = %s\n", info.seg_addr_table[i].split == TRUE ?
					 "TRUE" : "FALSE");
		if(info.seg_addr_table[i].split == TRUE){
		    printf("\tsegs_read_only_addr = 0x%x\n",
		   (unsigned int)info.seg_addr_table[i].segs_read_only_addr);
		    printf("\tsegs_read_write_addr = 0x%x\n",
		   (unsigned int)info.seg_addr_table[i].segs_read_write_addr);
		}
		else{
		    printf("\tseg1addr = 0x%x\n",
			   (unsigned int)info.seg_addr_table[i].seg1addr);
		}
		short_name = guess_short_name(
				info.seg_addr_table[i].install_name,
				&is_framework, &has_suffix);
		if(short_name != NULL){
		    printf("\tshort_name = %s\n", short_name);
		    printf("\tis_framework = %s\n", is_framework == TRUE ?
			   "TRUE" : "FALSE");
		    printf("\thas_suffix = %s\n", has_suffix == NULL ?
			   "NULL" : has_suffix);
		    if(search_dylib_table(dylib_table, short_name) == NULL)
			printf("\tNOT in dylib_table\n");
		    else
			printf("\thas entry in dylib_table\n");
		}
	    }
	}
#endif /* DEBUG */

	/*
	 * If trying to create a segment address table from a dylib table then
	 * guess at all the install names.
	 */
	if(from_dylib_table == TRUE){
	    if(dylib_table == NULL)
		dylib_table = parse_default_dylib_table(&dylib_table_name);
	    for(i = 0; dylib_table[i].name != NULL; i++){
		install_name = guess_dylib_install_name(dylib_table[i].name);
		if(install_name == NULL)
		    error("can't guess install name for: %s",
			  dylib_table[i].name);
		else{
		    if(out_fp == NULL)
			out_fp = create_output_file(info.output_file_name);
		    fprintf(out_fp, "0x%x\t%s\n",
			    (unsigned int)dylib_table[i].seg1addr,
			    install_name);
		}
	    }
	}

	/*
	 * If trying to create a dylib table from a segment address table then
	 * process the segment address table to determine the addresses and
	 * guess at the short name of the library.
	 */
	if(create_dylib_table == TRUE){
	    out_fp = create_output_file(info.output_file_name);
	    process_seg_addr_table(info.seg_addr_table_name, out_fp, progname,
				   dylib_table_processor, &info);
	}

	/*
	 * If the -update option is specified then pick up the next addresses
	 * to assign.  The addresses are assigned starting at the
	 * NEXT_FLAT_ADDRESS_TO_ASSIGN and NEXT_SPLIT_ADDRESS_TO_ASSIGN.
	 */
	if(update == TRUE || checkonly == TRUE){
	    next_flat = FALSE;
	    next_split = FALSE;
	    for(i = 0 ; i < info.table_size; i++){
		entry = info.seg_addr_table + i;
		if(strcmp(entry->install_name,
			  NEXT_FLAT_ADDRESS_TO_ASSIGN) == 0){
		    if(next_flat == TRUE)
			fatal("segment address table: %s has more than one "
			      "entry for %s", info.seg_addr_table_name,
			      NEXT_FLAT_ADDRESS_TO_ASSIGN);
		    if(entry->split == TRUE)
			fatal("segment address table: %s entry for %s is not "
			      "a single address", info.seg_addr_table_name,
			      NEXT_FLAT_ADDRESS_TO_ASSIGN);
		    next_flat = TRUE;
		    info.seg1addr = entry->seg1addr;
		    info.next_flat_line = entry->line;
		}
		else if(strcmp(entry->install_name,
			       NEXT_SPLIT_ADDRESS_TO_ASSIGN) == 0){
		    if(next_split == TRUE)
			fatal("segment address table: %s has more than one "
			      "entry for %s", info.seg_addr_table_name,
			      NEXT_SPLIT_ADDRESS_TO_ASSIGN);
		    if(entry->split == FALSE)
			fatal("segment address table: %s entry for %s is "
			      "a single address", info.seg_addr_table_name,
			      NEXT_SPLIT_ADDRESS_TO_ASSIGN);
		    next_split = TRUE;
		    info.segs_read_only_addr = entry->segs_read_only_addr;
		    info.segs_read_write_addr = entry->segs_read_write_addr;
		    info.next_split_line = entry->line;
		}
	    }
	    if(next_flat == FALSE)
		error("segment address table: %s does not have an entry for %s",
		      info.seg_addr_table_name, NEXT_FLAT_ADDRESS_TO_ASSIGN);
	    if(next_split == FALSE)
		error("segment address table: %s does not have an entry for %s",
		      info.seg_addr_table_name, NEXT_SPLIT_ADDRESS_TO_ASSIGN);
	    if(errors != 0)
		exit(EXIT_FAILURE);
	}

	/*
	 * For the -relayout, -update, -checkonly  and -create options make a
	 * pass through all the entries in the seg_addr_table and determine the
	 * short names for each of the flat entries and determine the maximum
	 * size and their seg1addr.  For the split libraries just determine
	 * their maximum size and their segs_read_only_addr and
	 * segs_read_write_addr.
	 */
	if(relayout == TRUE || update == TRUE || checkonly == TRUE ||
	   create == TRUE){
    	    layout_info = allocate(sizeof(struct layout_info) *
				   (info.table_size + 2));
	    memset(layout_info, '\0', sizeof(struct layout_info) *
					     (info.table_size + 2));
	    used = 0;

	    info.layout_info =
		allocate(sizeof(struct layout_info *) * 
				(info.table_size + 2));
	    memset(info.layout_info, '\0', sizeof(struct layout_info *) *
					   (info.table_size + 2));

	    info.sorted_flat_layout_info =
		allocate(sizeof(struct layout_info *) * (info.table_size + 2));
	    info.nsorted_flat = 0;
	    info.sorted_split_read_only_layout_info =
		allocate(sizeof(struct layout_info *) * info.table_size);
	    info.sorted_split_read_write_layout_info =
		allocate(sizeof(struct layout_info *) * info.table_size);
	    info.nsorted_split = 0;

	    /*
	     * Force in two layout info structs into the
	     * sorted_flat_layout_info array so that next_flat_seg1addr() will
	     * step over them.
	     */
	    layout_info[used].install_name = READ_ONLY_SEGMENT_NAME;
	    layout_info[used].image_file_name = READ_ONLY_SEGMENT_NAME;
	    layout_info[used].short_name = READ_ONLY_SEGMENT_NAME;
	    layout_info[used].split = FALSE;
	    layout_info[used].seg1addr = DEFAULT_READ_ONLY_ADDR;
	    layout_info[used].max_sizes.all = 0x10000000;
	    info.sorted_flat_layout_info[info.nsorted_flat++] =
		layout_info + used;
	    used++;

	    layout_info[used].install_name = READ_WRITE_SEGMENT_NAME;
	    layout_info[used].image_file_name = READ_WRITE_SEGMENT_NAME;
	    layout_info[used].short_name = READ_WRITE_SEGMENT_NAME;
	    layout_info[used].split = FALSE;
	    layout_info[used].seg1addr = DEFAULT_READ_WRITE_ADDR;
	    layout_info[used].max_sizes.all = 0x10000000;
	    info.sorted_flat_layout_info[info.nsorted_flat++] =
		layout_info + used;
	    used++;

	    for(i = 0 ; i < info.table_size; i++){
		entry = info.seg_addr_table + i;
		/*
		 * Just skip the entries for the address used for updating.
		 */
		if(strcmp(entry->install_name,
			  NEXT_FLAT_ADDRESS_TO_ASSIGN) == 0 ||
		   strcmp(entry->install_name,
			  NEXT_SPLIT_ADDRESS_TO_ASSIGN) == 0)
		    continue;
		/*
		 * Convert fixed address and size regions to flat entries using
		 * the segs_read_only_addr value as the address and the
		 * segs_read_write_addr value as the size.
		 */
		if(strcmp(entry->install_name, FIXED_ADDRESS_AND_SIZE) == 0){
		    if(entry->split != TRUE)
			error("entry in seg_addr_table: %s line: %lu for fixed "
			      "address and size not to be allocated does not "
			      "have both address and size values",
			      info.seg_addr_table_name, entry->line);
		    else{
			entry->split = FALSE;
			entry->seg1addr = entry->segs_read_only_addr;

			layout_info[used].install_name = entry->install_name;
			layout_info[used].image_file_name = entry->install_name;
			layout_info[used].short_name = entry->install_name;
			layout_info[used].split = FALSE;
			layout_info[used].seg1addr = entry->seg1addr;
			layout_info[used].max_sizes.all =
			    entry->segs_read_write_addr;

			info.layout_info[i] = layout_info + used;
			info.sorted_flat_layout_info
				[info.nsorted_flat++] = layout_info + used;
			used++;
		    }
		    continue;
		}
		/*
		 * Get the SYMROOT file or the DSTROOT file for this install
		 * name if we are given a -release option.
		 */
		image_file_name = get_image_file_name(&info,
						      entry->install_name,
						      entry->split);
		if(image_file_name == NULL){
		    if(info.disablewarnings == FALSE)
			error("from seg_addr_table: %s line: %lu can't find "
			      "file for install name: %s in -release %s",
			      info.seg_addr_table_name, entry->line,
			      entry->install_name, info.release_name);
		    continue;
		}
		if(stat(image_file_name, &stat_buf) == -1){
		    if(info.disablewarnings == FALSE)
			error("from seg_addr_table: %s line: %lu can't open "
			      "file: %s", info.seg_addr_table_name, entry->line,
			      image_file_name);
		    continue;
		}

		if(entry->split == FALSE){
		    base_name = strrchr(entry->install_name, '/');
		    if(base_name == NULL || base_name[1] == '\0')
			base_name = entry->install_name;
		    else
			base_name = base_name + 1;
		    short_name = guess_short_name(entry->install_name,
						  &is_framework,
						  &has_suffix);
		    if(short_name == NULL)
			short_name = base_name;
		    found = FALSE;
		    for(j = 0; j < used; j++){
			if(layout_info[j].short_name != NULL &&
			   strncmp(layout_info[j].install_name,
				   entry->install_name,
				   base_name - entry->install_name) == 0 &&
			   strcmp(layout_info[j].short_name, short_name) == 0){
			    found = TRUE;
			    info.layout_info[i] = layout_info + j;
			}
		    }
		    if(found == FALSE){
			layout_info[used].install_name = entry->install_name;
			layout_info[used].image_file_name = image_file_name;
			layout_info[used].short_name = short_name;
			info.layout_info[i] = layout_info + used;
			if((update == TRUE || checkonly == TRUE) &&
			   entry->seg1addr != 0)
			    info.sorted_flat_layout_info
				[info.nsorted_flat++] = layout_info + used;
			used++;
		    }
		}
		else{
		    layout_info[used].install_name = entry->install_name;
		    layout_info[used].image_file_name = image_file_name;
		    layout_info[used].short_name = NULL;
		    info.layout_info[i] = layout_info + used;
		    if((update == TRUE || checkonly == TRUE) &&
		       entry->segs_read_only_addr != 0){
			info.sorted_split_read_only_layout_info
			    [info.nsorted_split] = layout_info + used;
			info.sorted_split_read_write_layout_info
			    [info.nsorted_split] = layout_info + used;
			info.nsorted_split++;
		    }
		    used++;
		}
		info.layout_info[i]->current_entry = entry;
		ofile_process(image_file_name, info.arch_flags,
			      info.narch_flags, info.all_archs, FALSE,
			      TRUE, sizes_and_addresses,
			      info.layout_info[i]);

	    }

	    /*
	     * Now with all the sizes and addresses known create the sorted
	     * list of flat and split libraries for those things not to be
	     * reassigned addresses and check for overlaps.
	     */
	    qsort(info.sorted_flat_layout_info, info.nsorted_flat, 
		  sizeof(struct layout_info *),
		  (int (*)(const void *, const void *))qsort_flat);
#ifdef DEBUG
	    for(i = 0 ; i < info.nsorted_flat; i++){
		printf("0x%08x %s\n",
		       (unsigned int)info.sorted_flat_layout_info[i]->seg1addr,
		       info.sorted_flat_layout_info[i]->image_file_name);
	    }
#endif /* DEBUG */
	    qsort(info.sorted_split_read_only_layout_info, info.nsorted_split, 
		  sizeof(struct layout_info *),
		  (int (*)(const void *, const void *))qsort_split_read_only);
	    qsort(info.sorted_split_read_write_layout_info, info.nsorted_split, 
		  sizeof(struct layout_info *),
		  (int (*)(const void *, const void *))qsort_split_read_write);

	    /* check the sorted flat libraries for overlaps */
	    for(i = 0 ; i < info.nsorted_flat; i++){
		for(j = i + 1; j < info.nsorted_flat; j++){
		    if(info.sorted_flat_layout_info[i]->seg1addr +
		       info.sorted_flat_layout_info[i]->max_sizes.all >
		       info.sorted_flat_layout_info[j]->seg1addr){
			flat_overlap_error(&info, i, j, FALSE);
		    }
		}
		/*
		 * If the operation is update check the last assigned address
		 * (excluding fixed regions) so that it does not overlap with
		 * the next address to be assigned.
		 */
		if((update == TRUE || checkonly == TRUE) &&
		   strcmp(info.sorted_flat_layout_info[i]->install_name,
			  FIXED_ADDRESS_AND_SIZE) != 0 &&
		   info.sorted_flat_layout_info[i]->seg1addr +
		   info.sorted_flat_layout_info[i]->max_sizes.all >
		   info.seg1addr &&
		   info.sorted_flat_layout_info[i]->seg1addr < info.seg1addr){
		    flat_overlap_error(&info, i, ULONG_MAX, TRUE);
		}
	    }

	    /*
	     * Check the sorted split libraries read-only segments for
	     * overlaps.
	     */
	    for(i = 0 ; i < info.nsorted_split; i++){
		for(j = i + 1 ; j < info.nsorted_split; j++){
		    if(info.sorted_split_read_only_layout_info[i]->
			    segs_read_only_addr +
		       info.sorted_split_read_only_layout_info[i]->
			    max_sizes.read_only >
		       info.sorted_split_read_only_layout_info[j]->
			    segs_read_only_addr){
			split_overlap_error(&info, i, j,
					info.sorted_split_read_only_layout_info,
					    "read-only", FALSE);
		    }
		}
		/*
		 * If the operation is update check the last assigned address
		 * so that it does not overlap with the next address to be
		 * assigned.
		 */
		if((update == TRUE || checkonly == TRUE) &&
		   info.sorted_split_read_only_layout_info[i]->
		    segs_read_only_addr +
		   info.sorted_split_read_only_layout_info[i]->
		    max_sizes.read_only >
		   info.segs_read_only_addr){
		    split_overlap_error(&info, i, ULONG_MAX,
					info.sorted_split_read_only_layout_info,
					"read-only", TRUE);
		}
	    }

	    /*
	     * Check the sorted split libraries read-write segments for
	     * overlaps.
	     */
	    for(i = 0 ; i < info.nsorted_split; i++){
		for(j = i + 1 ; j < info.nsorted_split; j++){
		    if(info.sorted_split_read_write_layout_info[i]->
			    segs_read_write_addr +
		       info.sorted_split_read_write_layout_info[i]->
			    max_sizes.read_write >
		       info.sorted_split_read_write_layout_info[j]->
			    segs_read_write_addr){
			split_overlap_error(&info, i, j,
				    info.sorted_split_read_write_layout_info,
					"read-write", FALSE);
		    }
		}
		/*
		 * If the operation is update check the last assigned address
		 * so that it does not overlap with the next address to be
		 * assigned.
		 */
		if((update == TRUE || checkonly == TRUE) &&
		   info.sorted_split_read_write_layout_info[i]->
		    segs_read_write_addr +
		   info.sorted_split_read_write_layout_info[i]->
		    max_sizes.read_write >
		   info.segs_read_write_addr){
		    split_overlap_error(&info, i, ULONG_MAX,
				    info.sorted_split_read_write_layout_info,
					"read-write", TRUE);
		}
	    }
	}

	/*
	 * If the -create option is specified then just used the addresses
	 * picked up from the libraries them selves and write out the table.
	 */
	if(create == TRUE){
	    out_fp = create_output_file(info.output_file_name);
	    fprintf(out_fp, "#%s -create", progname);
	    user = getenv("USER");
	    if(user != NULL)
		fprintf(out_fp, " DEVELOPER:%s", user);
	    if(time(&tloc) != -1)
		fprintf(out_fp, " BUILT:%s", ctime(&tloc));
	    else
		fprintf(out_fp, "\n");

	    process_seg_addr_table(info.seg_addr_table_name, out_fp, progname,
				   new_table_processor, &info);
	}

	/*
	 * If the -relayout option is specified then re-layout all images in
	 * the seg_addr_table.
	 */
	if(relayout == TRUE){
	    /*
	     * Since we are relaying out all libraries and not just updating
	     * the table for unassigned addresses clear the assigned feild
	     * that sizes_and_addresses() set.
	     */
	    for(i = 0 ; i < info.table_size; i++){
		entry = info.seg_addr_table + i;
		if(info.layout_info[i] == NULL)
		    continue;
		if(entry->split == FALSE){
		    info.layout_info[i]->assigned = FALSE;
		}
	    }
	    /*
	     * Now with all the maximum sizes known for the libraries assign
	     * them addresses.
	     */
	    for(i = 0 ; i < info.table_size; i++){
		entry = info.seg_addr_table + i;
		if(info.layout_info[i] == NULL ||
		   strcmp(entry->install_name, FIXED_ADDRESS_AND_SIZE) == 0)
		    continue;
		if(entry->split == FALSE){
		    if(info.layout_info[i]->assigned == FALSE){
			size = round(info.layout_info[i]->max_sizes.all <<
				     info.factor, info.round);
			info.seg1addr = next_flat_seg1addr(&info, size);
			info.layout_info[i]->seg1addr = info.seg1addr;
			info.layout_info[i]->assigned = TRUE;
			info.seg1addr += size;
			if(info.seg1addr > MAX_ADDR)
			    error("address assignment: 0x%x plus size 0x%x for "
				  "%s greater maximum address 0x%x",
				  (unsigned int)info.layout_info[i]->seg1addr,
				  (unsigned int)size, 
				  entry->install_name, (unsigned int)MAX_ADDR);
		    }
		}
		else{
		    info.layout_info[i]->segs_read_only_addr =
			info.segs_read_only_addr;
		    info.layout_info[i]->segs_read_write_addr =
			info.segs_read_write_addr;
		    if(info.layout_info[i]->max_sizes.read_only > 
		       info.layout_info[i]->max_sizes.read_write)
			max = info.layout_info[i]->max_sizes.read_only;
		    else
			max = info.layout_info[i]->max_sizes.read_write;
		    size = round(max + info.round, info.round);
		    info.segs_read_only_addr += size;
		    info.segs_read_write_addr += size;
		    if((info.layout_info[i]->segs_read_only_addr &
			SPLIT_OVERFLOW_MASK) !=
		       (info.segs_read_only_addr & SPLIT_OVERFLOW_MASK))
			error("read-only address assignment: 0x%x plus size "
			      "0x%x for %s overflows area to be allocated",
			      (unsigned int)
				info.layout_info[i]->segs_read_only_addr,
			      (unsigned int)size, 
			      entry->install_name);
		    if((info.layout_info[i]->segs_read_write_addr &
			SPLIT_OVERFLOW_MASK) !=
		       (info.segs_read_write_addr & SPLIT_OVERFLOW_MASK))
			error("read-write address assignment: 0x%x plus size "
			      "0x%x for %s overflows area to be allocated",
			      (unsigned int)
				info.layout_info[i]->segs_read_write_addr,
			      (unsigned int)size, 
			      entry->install_name);
		}
	    }

	    /*
	     * Now with the addresses assigned write out the new table.
	     */
	    out_fp = create_output_file(info.output_file_name);
	    fprintf(out_fp, "#%s -relayout", progname);
	    user = getenv("USER");
	    if(user != NULL)
		fprintf(out_fp, " DEVELOPER:%s", user);
	    if(time(&tloc) != -1)
		fprintf(out_fp, " BUILT:%s", ctime(&tloc));
	    else
		fprintf(out_fp, "\n");

	    process_seg_addr_table(info.seg_addr_table_name, out_fp, progname,
				   new_table_processor, &info);

	    fprintf(out_fp, "#%s: Do not remove the following line, "
		    "it is used by the %s tool\n", progname, progname);
	    fprintf(out_fp, "0x%08x\t0x%08x\t%s\n",
		    (unsigned int)info.segs_read_only_addr,
		    (unsigned int)info.segs_read_write_addr,
		    NEXT_SPLIT_ADDRESS_TO_ASSIGN);
	    if(info.segs_read_only_addr >
	       info.start_segs_read_only_addr + 0x08000000)
		error("segs_read_only_addr over flow (more than 128meg's of "
		      "address space assigned, 0x%08x - 0x%08x)",
		      (unsigned int)info.start_segs_read_only_addr,
		      (unsigned int)info.segs_read_only_addr);
	    if(info.segs_read_write_addr >
	       info.start_segs_read_write_addr + 0x08000000)
		error("segs_read_write_addr over flow (more than 128meg's of "
		      "address space assigned, 0x%08x - 0x%08x)",
		      (unsigned int)info.start_segs_read_write_addr,
		      (unsigned int)info.segs_read_write_addr);

	    fprintf(out_fp, "#%s: Do not remove the following line, "
		    "it is used by the %s tool\n", progname, progname);
	    fprintf(out_fp, "0x%08x\t%s\n",
		    (unsigned int)info.seg1addr,
		    NEXT_FLAT_ADDRESS_TO_ASSIGN);
	}

	/*
	 * If the -update option is specified then only layout the images in
	 * the seg_addr_table which had an address of zero.  The addresses are
	 * assigned starting at the NEXT_FLAT_ADDRESS_TO_ASSIGN and
	 * NEXT_SPLIT_ADDRESS_TO_ASSIGN and were picked up above.
	 */
	if(update == TRUE){
	    for(i = 0 ; i < info.table_size; i++){
		entry = info.seg_addr_table + i;
		if(info.layout_info[i] != NULL &&
		   info.disablewarnings == FALSE){
		    if(entry->split == FALSE && entry->seg1addr != 0){
			if(info.layout_info[i]->split == TRUE)
			    error("file: %s is split layout which does not "
				  "match the table entry in seg_addr_table: %s "
				  "line: %lu", info.layout_info[i]->
				  image_file_name, info.seg_addr_table_name,
				  entry->line);
			else if(info.layout_info[i]->seg1addr !=
				entry->seg1addr)
			    error("seg1addr (0x%x) of file: %s does not "
				  "match the table entry (0x%x) in "
				  "seg_addr_table: %s line: %lu",
				  (unsigned int)info.layout_info[i]->seg1addr,
				  info.layout_info[i]->image_file_name,
				  (unsigned int)entry->seg1addr,
				  info.seg_addr_table_name,
				  entry->line);
		    }
		    else if(entry->split == TRUE &&
			    entry->segs_read_only_addr != 0){
			if(info.layout_info[i]->split == FALSE)
			    error("file: %s is flat layout which does not "
				  "match the table entry in seg_addr_table: %s "
				  "line: %lu", info.layout_info[i]->
				  image_file_name, info.seg_addr_table_name,
				  entry->line);
			else{
			    if(info.layout_info[i]->segs_read_only_addr !=
			       entry->segs_read_only_addr)
				error("segs_read_only_addr (0x%x) of file: %s "
				      "does not match the table entry (0x%x) "
				      "in seg_addr_table: %s line: %lu",
				      (unsigned int)info.layout_info[i]->
					    segs_read_only_addr,
				      info.layout_info[i]->image_file_name,
				      (unsigned int)entry->segs_read_only_addr,
				      info.seg_addr_table_name,
				      entry->line);
			    if(entry->segs_read_write_addr != 0 &&
			       info.layout_info[i]->segs_read_write_addr !=
			       entry->segs_read_write_addr)
				error("segs_read_write_addr (0x%x) of file: %s "
				      "does not match the table entry (0x%x) "
				      "in seg_addr_table: %s line: %lu",
				      (unsigned int)info.layout_info[i]->
					    segs_read_write_addr,
				      info.layout_info[i]->image_file_name,
				      (unsigned int)entry->segs_read_write_addr,
				      info.seg_addr_table_name,
				      entry->line);
			}
		    }
		}
	    }
	    if(errors != 0)
		exit(EXIT_FAILURE);
	    /*
	     * Now with all the maximum sizes known for the libraries assign
	     * them addresses.
	     */
	    for(i = 0 ; i < info.table_size; i++){
		entry = info.seg_addr_table + i;
		/*
		 * Just skip the entries for the address used for updating and
		 * fixed address and size regions.
		 */
		if(strcmp(entry->install_name,
			  NEXT_FLAT_ADDRESS_TO_ASSIGN) == 0 ||
		   strcmp(entry->install_name,
			  NEXT_SPLIT_ADDRESS_TO_ASSIGN) == 0 ||
		   strcmp(entry->install_name,
			  FIXED_ADDRESS_AND_SIZE) == 0)
		    continue;
		if(info.layout_info[i] == NULL)
		    continue;
		if(entry->split == FALSE && entry->seg1addr == 0){
		    if(info.layout_info[i]->assigned == FALSE){
			size = round(info.layout_info[i]->max_sizes.all <<
				     info.factor, info.round);
			info.seg1addr = next_flat_seg1addr(&info, size);
			info.layout_info[i]->seg1addr = info.seg1addr;
			info.layout_info[i]->assigned = TRUE;
			info.seg1addr += size;
			if(info.seg1addr > MAX_ADDR)
			    error("address assignment: 0x%x plus size 0x%x for "
				  "%s greater maximum address 0x%x",
				  (unsigned int)info.layout_info[i]->seg1addr,
				  (unsigned int)size, 
				  entry->install_name, (unsigned int)MAX_ADDR);
		    }
		}
		/*
		 * If the flat address is not zero then it is possible that
		 * the layout info will have the address the library was last
		 * built at and not the address in the table entry.  So to make
		 * sure the table is written out correctly in an -update mode
		 * copy the table entries that are non-zero into the layout
		 * info.
		 */
		if(entry->split == FALSE && entry->seg1addr != 0){
		    info.layout_info[i]->seg1addr = entry->seg1addr;
		}
		else if(entry->split == TRUE &&
			entry->segs_read_only_addr == 0){
		    info.layout_info[i]->segs_read_only_addr =
			info.segs_read_only_addr;
		    info.layout_info[i]->segs_read_write_addr =
			info.segs_read_write_addr;
		    if(info.layout_info[i]->max_sizes.read_only > 
		       info.layout_info[i]->max_sizes.read_write)
			max = info.layout_info[i]->max_sizes.read_only;
		    else
			max = info.layout_info[i]->max_sizes.read_write;
		    size = round(max, info.round);
		    info.segs_read_only_addr += size;
		    info.segs_read_write_addr += size;
		    if((info.layout_info[i]->segs_read_only_addr &
			SPLIT_OVERFLOW_MASK) !=
		       (info.segs_read_only_addr & SPLIT_OVERFLOW_MASK))
			error("read-only address assignment: 0x%x plus size "
			      "0x%x for %s overflows area to be allocated",
			      (unsigned int)
				info.layout_info[i]->segs_read_only_addr,
			      (unsigned int)size, 
			      entry->install_name);
		    if((info.layout_info[i]->segs_read_write_addr &
			SPLIT_OVERFLOW_MASK) !=
		       (info.segs_read_write_addr & SPLIT_OVERFLOW_MASK))
			error("read-write address assignment: 0x%x plus size "
			      "0x%x for %s overflows area to be allocated",
			      (unsigned int)
				info.layout_info[i]->segs_read_write_addr,
			      (unsigned int)size, 
			      entry->install_name);
		}
		/*
		 * If the split address is not zero then it is possible that
		 * the layout info will have the address the library was last
		 * built at and not the address in the table entry.  So to make
		 * sure the table is written out correctly in an -update mode
		 * copy the table entries that are non-zero into the layout
		 * info.
		 */
		else if(entry->split == TRUE &&
			entry->segs_read_only_addr != 0){
		    info.layout_info[i]->segs_read_only_addr =
			entry->segs_read_only_addr;
		    info.layout_info[i]->segs_read_write_addr =
			entry->segs_read_write_addr;
		}
	    }

	    /*
	     * Now with the addresses assigned write out the updated table.
	     */
	    out_fp = create_output_file(info.output_file_name);
	    process_seg_addr_table(info.seg_addr_table_name, out_fp, progname,
				   new_table_processor, &info);

	    fprintf(out_fp, "#%s: Do not remove the following line, "
		    "it is used by the %s tool\n", progname, progname);
	    fprintf(out_fp, "0x%08x\t0x%08x\t%s\n",
		    (unsigned int)info.segs_read_only_addr,
		    (unsigned int)info.segs_read_write_addr,
		    NEXT_SPLIT_ADDRESS_TO_ASSIGN);

	    fprintf(out_fp, "#%s: Do not remove the following line, "
		    "it is used by the %s tool\n", progname, progname);
	    fprintf(out_fp, "0x%08x\t%s\n",
		    (unsigned int)info.seg1addr,
		    NEXT_FLAT_ADDRESS_TO_ASSIGN);
	}

	if(out_fp != NULL){
	    if(fclose(out_fp) != 0)
		system_fatal("can't close output file: %s\n",
			     info.output_file_name);
	}

	if(errors != 0)
	    return(EXIT_FAILURE);
	else
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
	fprintf(stderr, "Usage: %s [[[-relayout | -update ] -o output_file] |  "
		"-checkonly ] [-seg_addr_table input_file] "
		"[-disablewarnings] [-seg1addr hex_address] "
		"[-segs_read_only_addr hex_address] "
		"[-segs_read_write_addr hex_address] "
		"[-release <release_name> ] [[-arch <arch_flag>] ...]\n",
		progname);
	exit(EXIT_FAILURE);
}

/*
 * create_output_file() creates the output file for the new table.
 */
static
FILE *
create_output_file(
char *output_file_name)
{
    FILE *out_fp;

	out_fp = fopen(output_file_name, "w");
	if(out_fp == NULL)
	    system_fatal("can't create output file: %s\n", output_file_name);
	return(out_fp);
}

/*
 * qsort_flat() is used by qsort() to sort the list of flat libraries that have
 * addresses assigned to them my their seg1addr.
 */
static
int
qsort_flat(
const struct layout_info **p1,
const struct layout_info **p2)
{
	if((*p1)->seg1addr == (*p2)->seg1addr)
	    return(0);
	if((*p1)->seg1addr < (*p2)->seg1addr)
	    return(-1);
	return(1);
}

/*
 * qsort_split_read_only() is used by qsort() to sort the list of split
 * libraries that have addresses assigned to them my their segs_read_only_addr.
 */
static
int
qsort_split_read_only(
const struct layout_info **p1,
const struct layout_info **p2)
{
	if((*p1)->segs_read_only_addr == (*p2)->segs_read_only_addr)
	    return(0);
	if((*p1)->segs_read_only_addr < (*p2)->segs_read_only_addr)
	    return(-1);
	return(1);
}

/*
 * qsort_split_read_write() is used by qsort() to sort the list of split
 * libraries that have addresses assigned to them my their segs_read_write_addr.
 */
static
int
qsort_split_read_write(
const struct layout_info **p1,
const struct layout_info **p2)
{
	if((*p1)->segs_read_write_addr == (*p2)->segs_read_write_addr)
	    return(0);
	if((*p1)->segs_read_write_addr < (*p2)->segs_read_write_addr)
	    return(-1);
	return(1);
}

/*
 * flat_overlap_error() is called to print an error message indicating that two
 * flat libraries overlap.  The libraries are indicated as indexes into the
 * array of sorted flat library layout info in the struct info.  If next_address
 * is TRUE then the overlap is with the next address to assign and not the
 * second index.
 */
static
void
flat_overlap_error(
struct info *info,
unsigned long i1,
unsigned long i2,
enum bool next_address)
{
    struct layout_info *p1, *p2;
    struct seg_addr_table *f1, *f2;

	p1 = info->sorted_flat_layout_info[i1];
	if(strcmp(p1->install_name, FIXED_ADDRESS_AND_SIZE) == 0)
	    f1 = search_seg_addr_table_for_fixed(info->seg_addr_table,
					         p1->seg1addr,
					         p1->max_sizes.all);
	else
	    f1 = search_seg_addr_table(info->seg_addr_table, p1->install_name);
	if(f1 == NULL)
	    fatal("internal error (can't find entry in info->seg_addr_table "
		  "for: %s\n", p1->install_name);

	if(next_address == FALSE){
	    p2 = info->sorted_flat_layout_info[i2];
	    if(strcmp(p2->install_name, FIXED_ADDRESS_AND_SIZE) == 0)
		f2 = search_seg_addr_table_for_fixed(info->seg_addr_table,
						     p2->seg1addr,
						     p2->max_sizes.all);
	    else
		f2 = search_seg_addr_table(info->seg_addr_table,
					   p2->install_name);
	    if(f2 == NULL)
		fatal("internal error (can't find entry in info->"
		      "seg_addr_table for: %s\n", p2->install_name);
	    if(info->disablewarnings == FALSE)
		error("address space of: %s for seg_addr_table: %s entry on "
		      "line: %lu overlaps with: %s for entry on line: %lu",
		      p1->image_file_name, info->seg_addr_table_name, f1->line,
		      p2->image_file_name, f2->line);
	}
	else{
	    if(info->disablewarnings == FALSE)
		error("address space of: %s for seg_addr_table: %s entry on "
		      "line: %lu overlaps with: %s for entry on line: %lu",
		      p1->image_file_name, info->seg_addr_table_name, f1->line,
		      NEXT_FLAT_ADDRESS_TO_ASSIGN, info->next_flat_line);
	}
}

/*
 * split_overlap_error() is called to print an error message indicating that two
 * split libraries overlap.  The string segment is either "read-only" or
 * "read-write" and sorted_layout_info is a pointer to the sorted array of
 * layout info structs into which the indexes i1 and i2 refer which are the
 * libraries that overlap.  If next_address is TRUE then the overlap is with
 * the next address to assign and not the second index.
 */
static
void
split_overlap_error(
struct info *info,
unsigned long i1,
unsigned long i2,
struct layout_info **sorted_layout_info,
char *segment,
enum bool next_address)
{
    struct layout_info *p1, *p2;
    struct seg_addr_table *f1, *f2;

	p1 = sorted_layout_info[i1];
	f1 = search_seg_addr_table(info->seg_addr_table, p1->install_name);
	if(f1 == NULL)
	    fatal("internal error (can't find entry in info->seg_addr_table "
		  "for: %s\n", p1->install_name);

	if(next_address == FALSE){
	    p2 = sorted_layout_info[i2];
	    f2 = search_seg_addr_table(info->seg_addr_table, p2->install_name);
	    if(f2 == NULL)
		fatal("internal error (can't find entry in info->"
		      "seg_addr_table for: %s\n", p2->install_name);
	    if(info->disablewarnings == FALSE)
		error("%s segments of: %s for seg_addr_table: %s entry on line:"
		  " %lu overlaps with: %s for entry on line: %lu", segment,
		  p1->image_file_name, info->seg_addr_table_name, f1->line,
		  p2->image_file_name, f2->line);
	}
	else{
	    if(info->disablewarnings == FALSE)
		error("%s segments of: %s for seg_addr_table: %s entry on line:"
		  " %lu overlaps with: %s for entry on line: %lu", segment,
		  p1->image_file_name, info->seg_addr_table_name, f1->line,
		  NEXT_SPLIT_ADDRESS_TO_ASSIGN, info->next_split_line);
	}
}

/*
 * search_seg_addr_table() searches the specified segment address table for
 * a fixed entry with the specified seg1addr and size and returns the entry to
 * it if it is found.  If it is not found NULL is returned.
 */
static
struct seg_addr_table *
search_seg_addr_table_for_fixed(
struct seg_addr_table *seg_addr_table,
unsigned long seg1addr,
unsigned long size)
{
    struct seg_addr_table *p;

	if(seg_addr_table == NULL)
	    return(NULL);

	for(p = seg_addr_table; p->install_name != NULL; p++){
	    if(strcmp(p->install_name, FIXED_ADDRESS_AND_SIZE) == 0 &&
	       p->segs_read_only_addr == seg1addr &&
	       p->segs_read_write_addr == size)
		return(p);
	}
	return(NULL);
}


/*
 * next_flat_seg1addr() uses the info->seg1addr as the starting point
 * to try to find the next address of a flat library to assign for 'size'.  It
 * goes through the sorted libraries and finds the next place big enough to put
 * 'size' if info->seg1addr of 'size' overlaps a library.
 */
static
unsigned long
next_flat_seg1addr(
struct info *info,
unsigned long size)
{
    unsigned long i, seg1addr, start, end;

	seg1addr = info->seg1addr;
	/*
	 * Go through the flat libraries sorted by address to see if
	 * seg1addr for size overlaps with anything.  If so move seg1addr
	 * past that region.  When all regions have been checked then
	 * return the seg1addr to be used.
	 */
	for(i = 0 ; i < info->nsorted_flat; i++){
	    start = info->sorted_flat_layout_info[i]->seg1addr;
	    end = start + info->sorted_flat_layout_info[i]->max_sizes.all;
	    if((seg1addr <= start && seg1addr + size > start) ||
	       (seg1addr < end && seg1addr + size > end)){
#ifdef DEBUG
printf("next_flat_seg1addr() stepping over region start = 0x%x  end = 0x%x\n",
       (unsigned int)start, (unsigned int)end);
#endif
		seg1addr = end;
	    }
	}
#ifdef DEBUG
printf("next_flat_seg1addr() returning seg1addr = 0x%x\n",
       (unsigned int)seg1addr);
#endif
	return(seg1addr);
}


/*
 * get_image_file_name() gets the SYMROOT file or the DSTROOT file for this
 * install name if we were given a -release option.  If a -release option is
 * given an the install_name can't be found in the SYMROOT or the DSTROOT this
 * routine returns NULL.  If a -release option is not given this routine simply
 * returns install_name.
 */
static
char *
get_image_file_name(
struct info *info,
char *install_name,
enum bool split)
{
    char *image_file_name;
    enum bool found_project;

	/*
	 * Get the SYMROOT file or the DSTROOT file for this install
	 * name if we are given a -release option.
	 */
	image_file_name = NULL;
	if(info->release_name != NULL){
	    /*
	     * There is not enough room in the shared regions to use the full
	     * debugging SYMROOT files.  So only look for there if the library
	     * is not split.
	     */
	    if(split == FALSE)
		image_file_name = get_symfile_for_dylib(
					 install_name,
					 info->release_name,
					 &found_project,
					 info->disablewarnings);
	    else
		found_project = TRUE;
	    if(image_file_name == NULL && found_project == TRUE){
		image_file_name = get_dstfile_for_dylib(
					 install_name,
					 info->release_name,
					 &found_project,
					 info->disablewarnings);
	    }
	}
	else{
	    image_file_name = install_name;
	}
#ifdef DEBUG
	printf("using %s for %s\n",
	       image_file_name == NULL ? "NULL" : image_file_name,
	       install_name);
#endif
	return(image_file_name);
}

/*
 * new_table_processor() is passed an entry in the seg_addr_table and writes
 * the new entry into the file pointer out_fp.  The cookie is a info struct
 * with all the info for the new entry.
 */
static
void
new_table_processor(
struct seg_addr_table *entry,
FILE *out_fp,
void *cookie)
{
    struct info *info;
    char *image_file_name;
    struct stat stat_buf;
    struct seg_addr_table *f;

	/*
	 * Just skip the entries for the address used for updating as that
	 * will be set last in the routine that create the output file.
	 */
	if(strcmp(entry->install_name, NEXT_FLAT_ADDRESS_TO_ASSIGN) == 0 ||
	   strcmp(entry->install_name, NEXT_SPLIT_ADDRESS_TO_ASSIGN) == 0)
	    return;

	if(strcmp(entry->install_name, FIXED_ADDRESS_AND_SIZE) == 0){
	    fprintf(out_fp, "0x%08x\t0x%08x\t%s\n",
		   (unsigned int)entry->segs_read_only_addr,
		   (unsigned int)entry->segs_read_write_addr,
		   entry->install_name);
	    return;
	}

	info = (struct info *)cookie;

	/*
	 * If the file exist then print out its previously assigned address.
	 */
	image_file_name = get_image_file_name(info, entry->install_name,
					      entry->split);
	if(image_file_name == NULL)
	    return;
	if(stat(image_file_name, &stat_buf) != -1){
	    /*
	     * Since new_table_processor() is called to re-walk the table so to
	     * re-layout it in the order it came in, then entries passed to it
	     * are not the same as the ones stored way in the first pass used to
	     * layout the addresses.  So match up this entry with the previous
	     * copy of the entry by its name.
	     */
	    f = search_seg_addr_table(info->seg_addr_table,
				      entry->install_name);
	    if(f == NULL)
		fatal("internal error (can't find entry in info->"
		      "seg_addr_table for: %s\n", entry->install_name);
	    if(entry->split == TRUE){
		fprintf(out_fp, "0x%08x\t0x%08x\t%s\n",
		       (unsigned int)info->layout_info[
				f - info->seg_addr_table]->segs_read_only_addr,
		       (unsigned int)info->layout_info[
				f - info->seg_addr_table]->segs_read_write_addr,
		       entry->install_name);
	    }
	    else{
		fprintf(out_fp, "0x%08x\t%s\n",
		       (unsigned int)info->layout_info[
				    f - info->seg_addr_table]->seg1addr,
		       entry->install_name);
	    }
	}
}

/*
 * sizes_and_addresses() is the routine that gets called by ofile_process() to
 * process single object files.  If sums up the vmsizes of all the segments, the
 * read_only segments and the read_write segments.  Then if any of these are
 * larger the the previous sizes in the max_sizes struct passed in cookie the
 * fields in the max_sizes struct are updated with the larger sizes.
 * It also picks up the seg1addr or the pair of segs_read_only_addr and
 * seg_read_write_addr's.
 */
static
void
sizes_and_addresses(
struct ofile *ofile,
char *arch_name,
void *cookie)
{
    unsigned long i, all, read_only, read_write,
		  segs_read_only_addr, segs_read_write_addr;
    struct load_command *lc;
    struct segment_command *sg, *first;
    struct layout_info *layout_info;
    enum bool split;

#ifdef DEBUG
	printf("In max_size() ofile->file_name = %s", ofile->file_name);
	if(arch_name != NULL)
	    printf(" arch_name = %s\n", arch_name);
	else
	    printf("\n");
#endif /* DEBUG */

	layout_info = (struct layout_info *)cookie;

	split = (ofile->mh->flags & MH_SPLIT_SEGS) == MH_SPLIT_SEGS;
	layout_info->split = split;
	lc = ofile->load_commands;
	first = NULL;
	all = 0;
	read_only = 0;
	read_write = 0;
	segs_read_only_addr = ULONG_MAX;
	segs_read_write_addr = ULONG_MAX;
	for(i = 0; i < ofile->mh->ncmds; i++){
	    if(lc->cmd == LC_SEGMENT){
		sg = (struct segment_command *)lc;
		if(first == NULL){
		    first = sg;
		    if(split == FALSE && first->vmaddr != 0){
			if(layout_info->seg1addr != 0 &&
			   layout_info->seg1addr != first->vmaddr){
			    if(layout_info->current_entry->seg1addr != 0){
				if(arch_name != NULL)
				    error("first segment address in: %s (for "
				      "architecture %s) different than "
				      "in previous architectures or files for "
				      "library: %s in file %s",ofile->file_name,
				      arch_name, layout_info->short_name, 
				      layout_info->image_file_name);
				else
				    error("first segment address in: %s "
				      "different than in previous architectures"
				      " or files for library: %s in file %s",
				      ofile->file_name, layout_info->short_name,
				      layout_info->image_file_name);
			    }
			}
			else
			    layout_info->seg1addr = first->vmaddr;
		    }
		}
		if((sg->initprot & VM_PROT_WRITE) == 0){
		    read_only += sg->vmsize;
		    if(split == TRUE && sg->vmaddr < segs_read_only_addr)
			segs_read_only_addr = sg->vmaddr;
		}
		else{
		    read_write += sg->vmsize;
		    if(split == TRUE && sg->vmaddr < segs_read_write_addr)
			segs_read_write_addr = sg->vmaddr;
		}
		all += sg->vmsize;
	    }
	    lc = (struct load_command *)((char *)lc + lc->cmdsize);
	}
	if(all > layout_info->max_sizes.all)
	    layout_info->max_sizes.all = all;
	if(read_only > layout_info->max_sizes.read_only)
	    layout_info->max_sizes.read_only = read_only;
	if(read_write > layout_info->max_sizes.read_write)
	    layout_info->max_sizes.read_write = read_write;
	if(split == TRUE){
	    layout_info->segs_read_only_addr = segs_read_only_addr;
	    layout_info->segs_read_write_addr = segs_read_write_addr;
	}
}

/*
 * dylib_table_processor() is passed an entry in the seg_addr_table and writes
 * a dylib table entry into the file pointer out_fp.  The cookie is a info
 * struct with all the info (but not yet used here).
 */
static
void
dylib_table_processor(
struct seg_addr_table *entry,
FILE *out_fp,
void *cookie)
{
    struct info *info;
    char *image_file_name, *short_name, *has_suffix;
    struct stat stat_buf;
    unsigned long seg1addr;
    enum bool is_framework;

	/*
	 * Just skip the entries for the address used for updating as that
	 * will be set last in the routine that create the output file.
	 */
	if(strcmp(entry->install_name, NEXT_FLAT_ADDRESS_TO_ASSIGN) == 0 ||
	   strcmp(entry->install_name, NEXT_SPLIT_ADDRESS_TO_ASSIGN) == 0)
	    return;

	info = (struct info *)cookie;

	/*
	 * This may change to using the DSTROOT file for this install name.
	 */
	image_file_name = entry->install_name;

	/*
	 * Determine if the file exist and if so check to see all the
	 * architectures to be considered have the same seg1addr.
	 */
	if(stat(image_file_name, &stat_buf) != -1){
	    seg1addr = 0;
	    ofile_process(image_file_name, info->arch_flags, info->narch_flags,
			  info->all_archs, FALSE, TRUE, get_seg1addr,&seg1addr);
	    short_name = guess_short_name(entry->install_name, &is_framework,
					  &has_suffix);
	    if(short_name == NULL){
		short_name = strrchr(entry->install_name, '/');
		if(short_name == NULL || short_name[1] == '\0')
		     short_name = entry->install_name;
	    }
	    fprintf(out_fp, "0x%08x\t%s\n", (unsigned int)seg1addr, short_name);
	}
	else{
	    error("from seg_addr_table: %s line: %lu can't open file: "
		  "%s \n", info->seg_addr_table_name, entry->line,
		  image_file_name);
	}
}

/*
 * get_seg1addr() is the routine that gets called by ofile_process() to process
 * single object files.  It determines the seg1addr of the ofile passed to it
 * and checks that it is the same as what is passed in the cookie (it that is
 * not zero).  Then it sets the seg1addr into the cookie is it was zero.
 */
static
void
get_seg1addr(
struct ofile *ofile,
char *arch_name,
void *cookie)
{
    unsigned long i;
    struct load_command *lc;
    struct segment_command *sg;
    unsigned long *seg1addr, first_addr;

#ifdef DEBUG
	printf("In get_seg1addr() ofile->file_name = %s", ofile->file_name);
	if(arch_name != NULL)
	    printf(" arch_name = %s\n", arch_name);
	else
	    printf("\n");
#endif /* DEBUG */

	first_addr = 0;
	seg1addr = (unsigned long *)cookie;

	lc = ofile->load_commands;
	for(i = 0; i < ofile->mh->ncmds; i++){
	    if(lc->cmd == LC_SEGMENT){
		sg = (struct segment_command *)lc;
		first_addr = sg->vmaddr;
		break;
	    }
	    lc = (struct load_command *)((char *)lc + lc->cmdsize);
	}
	if(*seg1addr != 0 && *seg1addr != first_addr)
	    error("seg1addr of: %s not the same in all architectures",
		  ofile->file_name);
	*seg1addr = first_addr;
}
