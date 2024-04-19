static char rcsid[] = "$Id: bindv8conf_lister.l,v 1.1.1.1 2001/01/31 03:59:15 zarzycki Exp $";
%{
/* generate a list of domain names from a V8 BIND configuration file */

#include <unistd.h>
#include <string.h>
#include <sys/param.h>
#define MAX_INCLUDE_DEPTH 32
YY_BUFFER_STATE include_stack[MAX_INCLUDE_DEPTH];
int include_stack_ptr = 0;
int options_brackets = 0;
char filename[MAXPATHLEN];
char *original_path = NULL;
char *cwd = NULL;
int first;
%}
%x incl
%s options
%x ccomment
COMMENT	(#.*\n)|("//".*\n)
WS [ \t\n]
ZONE_TYPE master|slave|stub|hint
IDENTIFIER [A-Za-z0-9\."/-]+
LBRACK \{
RBRACK \}
%%
options	BEGIN(options);
<options>{WS}*{LBRACK} options_brackets++;
<options>{WS}*{RBRACK} {
	options_brackets--;
	if (options_brackets == 0)
		BEGIN(INITIAL);
}
<options>[^*\n]*
"/*"			BEGIN(ccomment);
<ccomment>[^*\n]*
<ccomment>"*"+[^*/\n]*
<ccomment>\n
<ccomment>"*"+"/"	BEGIN(INITIAL);

include	BEGIN(incl);
zone{WS}+{IDENTIFIER} {
	if (first) first = 0; else printf("\n");
	cleanup(yytext);
	printf("%s", yytext+5);
}
type{WS}+{ZONE_TYPE} {
	cleanup(yytext);
	printf(" %s", yytext+5);
}
file{WS}+{IDENTIFIER} {
	cleanup(yytext);
	printf(" %s", yytext+5);
}
masters{WS}+{LBRACK}{WS}*({IDENTIFIER}{WS}*;{WS}*)+{RBRACK}{WS}* {
	cleanup_masters(yytext);
	printf(" %s", yytext+8);
}
[ \t\n]+
{COMMENT}
{IDENTIFIER}
\{|\}|\;|\/|\.
<incl>[ \t\n]+
<incl>{IDENTIFIER} {	/* include file processing */
	FILE *yytemp;

	if (include_stack_ptr >= MAX_INCLUDE_DEPTH) {
		fprintf (stderr, "Include nesting too deep\n");
		exit(1);
	}
	(void)strcpy(filename, yytext+1); /* get rid of leading quote */
	filename[strlen(filename)-1] = '\0'; /* get rid of trailing quote */

	/* check what the original pathname for the current directory was
	   and adjust absolute filenames accordingly */
	if (filename[0] == '/') {
		if (original_path) {
			int slash_count = 0;
			char *p;
			int i, n, shift;

			p = filename;
			while (*p != '\0') {
				if (*p++ == '/') slash_count++;
			}
			/* shift the filename down by slash_count*3 */
			n =strlen(filename);
			shift = slash_count*3-1;
			for (i=n; i>=0; i--) {
				filename[i+shift] = filename[i];
			}
			for (i=0; i<slash_count; i++) {
				filename[3*i] = '.';
				filename[3*i+1] = '.';
				filename[3*i+2] = '/';
			}
		}
	}
	include_stack[include_stack_ptr++] = YY_CURRENT_BUFFER;
	if ((yytemp = fopen(filename, "r")) == NULL) {
		fprintf(stderr, "cannot open include file %s\n", filename);
		include_stack_ptr--;
	}
	else {
		yyin = yytemp;
		yy_switch_to_buffer(yy_create_buffer(yyin, YY_BUF_SIZE));
	}
	BEGIN(INITIAL);
}
<<EOF>> {
	if (--include_stack_ptr < 0) {
		yyterminate();
	}
	else {
		yy_delete_buffer(YY_CURRENT_BUFFER);
		yy_switch_to_buffer(include_stack[include_stack_ptr]);
	}

}
%%
extern char *optarg;
extern int optind, opterr, optopt;
main(int argc, char **argv)
{
	int opt;

	cwd = getcwd(NULL, 0);
	while ((opt = getopt(argc, argv, "P:")) != -1) {
		switch (opt) {
		case 'P':
			original_path = optarg;
		}
	}

	if (argv[optind] != NULL) {
		if ((yyin = fopen(argv[optind], "r")) == NULL) {
			fprintf(stderr, "cannot open %s\n", argv[optind]);
		}
	}
	else {
		yyin = stdin;
	}
	first = 1;
	yylex();
	printf("\n");
}

int
cleanup(char buffer[])
{
	char *p = buffer;

	while (*p != '\0') {
		if (*p == '\t') *p = ' ';
		if (*p == '"') *p = ' ';
		p++;
	}
	/* compress whitespace */
	p = buffer;
	while (*p != '\0') {
		char *q;

		if (*p == ' ') {
			char *q;

			q = p+1;
			while (*q == ' ') q++;
			if (*q == '\0') {
				*p = '\0';
			}
			else if (q > (p+1)) {
				char *r;

				r = (p+1);
				while (*r != '\0') {
					*r++ = *q++;
				}
			}
		}
		p++;
	}
	return 1;
}

cleanup_masters(char buffer[])
{
	char *p = buffer;
	char *first_ws;

	while (*p != '\0') {
		if (*p == '\t') *p = ' ';
		if (*p == '\n') *p = ' ';
		if (*p == ';') *p = ' ';
		if (*p == '{') *p = ' ';
		if (*p == '}') *p = ' ';
		p++;
	}
	cleanup(buffer);
	first_ws = strchr(buffer, ' ');
	p = strrchr(buffer, ' ');
	while (p != first_ws) {
		*p = ',';
		p = strrchr(buffer, ' ');
	}
}
