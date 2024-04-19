#include "port_before.h"
#include "port_after.h"

/* 
 * global externs for getopt()
 */
int		optind = 0;
int		optopt = 0;
char	*optarg;

/*
 * local for getopt()
 */
static char	*prog = NULL;
static char	*scan = NULL;

/*
 * getopt() - Parse an argv array
 */
int getopt(int argc, char *const *argv, const char *optstring)
{
	char c;
	const char *place;

	prog = argv[0];
	optarg = NULL;

	if (optind == 0) {
		scan = NULL;
		optind++;
	}
	
	if (scan == NULL || *scan == '\0') {
		if (optind >= argc
		    || argv[optind][0] != '-' && argv[optind][0] != '/'
		    || argv[optind][1] == '\0') {
			return (EOF);
		}
		if (argv[optind][1] == '-' || argv[optind][0] == '/'
		    && argv[optind][2] == '\0') {
			optind++;
			return (EOF);
		}
	
		scan = argv[optind++]+1;
	}

	c = *scan++;
	optopt = c & 0377;
	for (place = optstring; place != NULL && *place != '\0'; ++place)
		if (*place == c)
			break;

	if (place == NULL || *place == '\0' || c == ':' || c == '?') {
		fprintf(stderr, "%s: unknown option - %c\n", prog, c);
		return('?');
	}

	place++;
	if (*place == ':') {
		if (*scan != '\0') {
			optarg = scan;
			scan = NULL;
		} else if (optind >= argc) {
			fprintf(stderr, "%s: option requires argument - %c\n", prog, c);
			return('?');
		} else {
			optarg = argv[optind++];
		}
	}

	return (c & 0377);
}
