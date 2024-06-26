/*
 * Copyright (c) 1998-2000 Sendmail, Inc. and its suppliers.
 *	All rights reserved.
 * Copyright (c) 1992 Eric P. Allman.  All rights reserved.
 * Copyright (c) 1992, 1993
 *	The Regents of the University of California.  All rights reserved.
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the sendmail distribution.
 *
 */

#ifndef lint
static char copyright[] =
"@(#) Copyright (c) 1998-2000 Sendmail, Inc. and its suppliers.\n\
	All rights reserved.\n\
     Copyright (c) 1992 Eric P. Allman.  All rights reserved.\n\
     Copyright (c) 1992, 1993\n\
	The Regents of the University of California.  All rights reserved.\n";
#endif /* ! lint */

#ifndef lint
static char id[] = "@(#)$Id: makemap.c,v 1.1.1.4 2000/06/10 00:40:53 wsanchez Exp $";
#endif /* ! lint */

#include <sys/types.h>
#ifndef ISC_UNIX
# include <sys/file.h>
#endif /* ! ISC_UNIX */
#include <ctype.h>
#include <stdlib.h>
#include <unistd.h>
#ifdef EX_OK
# undef EX_OK		/* unistd.h may have another use for this */
#endif /* EX_OK */
#include <sysexits.h>
#include <sendmail/sendmail.h>
#include <sendmail/pathnames.h>
#include <libsmdb/smdb.h>

uid_t	RealUid;
gid_t	RealGid;
char	*RealUserName;
uid_t	RunAsUid;
uid_t	RunAsGid;
char	*RunAsUserName;
int	Verbose = 2;
bool	DontInitGroups = FALSE;
uid_t	TrustedUid = 0;
BITMAP256 DontBlameSendmail;

#define BUFSIZE		1024
#if _FFR_DELIM
# define ISSEP(c) ((sep == '\0' && isascii(c) && isspace(c)) || (c) == sep)
#else /* _FFR_DELIM */
# define ISSEP(c) (isascii(c) && isspace(c))
#endif /* _FFR_DELIM */

static void
usage(progname)
	char *progname;
{
	fprintf(stderr,
		"Usage: %s [-C cffile] [-N] [-c cachesize] [-d] [-e] [-f] [-l] [-o] [-r] [-s] %s[-u] [-v] type mapname\n",
		progname,
#if _FFR_DELIM
		"[-t  delimiter] "
#else /* _FFR_DELIM */
		""
#endif /* _FFR_DELIM */
		);
	exit(EX_USAGE);
}

int
main(argc, argv)
	int argc;
	char **argv;
{
	char *progname;
	char *cfile;
	bool inclnull = FALSE;
	bool notrunc = FALSE;
	bool allowreplace = FALSE;
	bool allowempty = FALSE;
	bool verbose = FALSE;
	bool foldcase = TRUE;
	bool unmake = FALSE;
#if _FFR_DELIM
	char sep = '\0';
#endif /* _FFR_DELIM */
	int exitstat;
	int opt;
	char *typename = NULL;
	char *mapname = NULL;
	int lineno;
	int st;
	int mode;
	int smode;
	int putflags = 0;
	long sff = SFF_ROOTOK|SFF_REGONLY;
	struct passwd *pw;
	SMDB_DATABASE *database;
	SMDB_CURSOR *cursor;
	SMDB_DBENT db_key, db_val;
	SMDB_DBPARAMS params;
	SMDB_USER_INFO user_info;
	char ibuf[BUFSIZE];
#if HASFCHOWN
	FILE *cfp;
	char buf[MAXLINE];
#endif /* HASFCHOWN */
	static char rnamebuf[MAXNAME];	/* holds RealUserName */
	extern char *optarg;
	extern int optind;

	memset(&params, '\0', sizeof params);
	params.smdbp_cache_size = 1024 * 1024;

	progname = strrchr(argv[0], '/');
	if (progname != NULL)
		progname++;
	else
		progname = argv[0];
	cfile = _PATH_SENDMAILCF;

	clrbitmap(DontBlameSendmail);
	RunAsUid = RealUid = getuid();
	RunAsGid = RealGid = getgid();
	pw = getpwuid(RealUid);
	if (pw != NULL)
		(void) strlcpy(rnamebuf, pw->pw_name, sizeof rnamebuf);
	else
		snprintf(rnamebuf, sizeof rnamebuf,
			"Unknown UID %d", (int) RealUid);
	RunAsUserName = RealUserName = rnamebuf;
	user_info.smdbu_id = RunAsUid;
	user_info.smdbu_group_id = RunAsGid;
	(void) strlcpy(user_info.smdbu_name, RunAsUserName,
		       SMDB_MAX_USER_NAME_LEN);


#define OPTIONS		"C:Nc:t:deflorsuv"
	while ((opt = getopt(argc, argv, OPTIONS)) != EOF)
	{
		switch (opt)
		{
		  case 'C':
			cfile = optarg;
			break;

		  case 'N':
			inclnull = TRUE;
			break;

		  case 'c':
			params.smdbp_cache_size = atol(optarg);
			break;

		  case 'd':
			params.smdbp_allow_dup = TRUE;
			break;

		  case 'e':
			allowempty = TRUE;
			break;

		  case 'f':
			foldcase = FALSE;
			break;

		  case 'l':
			smdb_print_available_types();
			exit(EX_OK);
			break;

		  case 'o':
			notrunc = TRUE;
			break;

		  case 'r':
			allowreplace = TRUE;
			break;

		  case 's':
			setbitn(DBS_MAPINUNSAFEDIRPATH, DontBlameSendmail);
			setbitn(DBS_WRITEMAPTOHARDLINK, DontBlameSendmail);
			setbitn(DBS_WRITEMAPTOSYMLINK, DontBlameSendmail);
			setbitn(DBS_LINKEDMAPINWRITABLEDIR, DontBlameSendmail);
			break;

#if _FFR_DELIM
		  case 't':
			if (optarg == NULL || *optarg == '\0')
			{
				fprintf(stderr, "Invalid separator\n");
				break;
			}
			sep = *optarg;
			break;
#endif /* _FFR_DELIM */

		  case 'u':
			unmake = TRUE;
			break;

		  case 'v':
			verbose = TRUE;
			break;

		  default:
			usage(progname);
			/* NOTREACHED */
		}
	}

	if (!bitnset(DBS_WRITEMAPTOSYMLINK, DontBlameSendmail))
		sff |= SFF_NOSLINK;
	if (!bitnset(DBS_WRITEMAPTOHARDLINK, DontBlameSendmail))
		sff |= SFF_NOHLINK;
	if (!bitnset(DBS_LINKEDMAPINWRITABLEDIR, DontBlameSendmail))
		sff |= SFF_NOWLINK;

	argc -= optind;
	argv += optind;
	if (argc != 2)
	{
		usage(progname);
		/* NOTREACHED */
	}
	else
	{
		typename = argv[0];
		mapname = argv[1];
	}

#if HASFCHOWN
	/* Find TrustedUser value in sendmail.cf */
	if ((cfp = fopen(cfile, "r")) == NULL)
	{
		fprintf(stderr, "makemap: %s: %s", cfile, errstring(errno));
		exit(EX_NOINPUT);
	}
	while (fgets(buf, sizeof(buf), cfp) != NULL)
	{
		register char *b;

		if ((b = strchr(buf, '\n')) != NULL)
			*b = '\0';

		b = buf;
		switch (*b++)
		{
		  case 'O':		/* option */
			if (strncasecmp(b, " TrustedUser", 12) == 0 &&
			    !(isascii(b[12]) && isalnum(b[12])))
			{
				b = strchr(b, '=');
				if (b == NULL)
					continue;
				while (isascii(*++b) && isspace(*b))
					continue;
				if (isascii(*b) && isdigit(*b))
					TrustedUid = atoi(b);
				else
				{
					TrustedUid = 0;
					pw = getpwnam(b);
					if (pw == NULL)
						fprintf(stderr,
							"TrustedUser: unknown user %s\n", b);
					else
						TrustedUid = pw->pw_uid;
				}

# ifdef UID_MAX
				if (TrustedUid > UID_MAX)
				{
					fprintf(stderr,
						"TrustedUser: uid value (%ld) > UID_MAX (%ld)",
						(long) TrustedUid,
						(long) UID_MAX);
					TrustedUid = 0;
				}
# endif /* UID_MAX */
				break;
			}


		  default:
			continue;
		}
	}
	(void) fclose(cfp);
#endif /* HASFCHOWN */

	if (!params.smdbp_allow_dup && !allowreplace)
		putflags = SMDBF_NO_OVERWRITE;

	if (unmake)
	{
		mode = O_RDONLY;
		smode = S_IRUSR;
	}
	else
	{
		mode = O_RDWR;
		if (!notrunc)
		{
			mode |= O_CREAT|O_TRUNC;
			sff |= SFF_CREAT;
		}
		smode = S_IWUSR;
	}

	params.smdbp_num_elements = 4096;

	errno = smdb_open_database(&database, mapname, mode, smode, sff,
				   typename, &user_info, &params);
	if (errno != SMDBE_OK)
	{
		char *hint;

		if (errno == SMDBE_UNSUPPORTED_DB_TYPE &&
		    (hint = smdb_db_definition(typename)) != NULL)
			fprintf(stderr,
				"%s: Need to recompile with -D%s for %s support\n",
				progname, hint, typename);
		else
			fprintf(stderr,
				"%s: error opening type %s map %s: %s\n",
				progname, typename, mapname, errstring(errno));
		exit(EX_CANTCREAT);
	}

	(void) database->smdb_sync(database, 0);

	if (geteuid() == 0 && TrustedUid != 0)
	{
		errno = database->smdb_set_owner(database, TrustedUid, -1);
		if (errno != SMDBE_OK)
		{
			fprintf(stderr,
				"WARNING: ownership change on %s failed %s",
				mapname, errstring(errno));
		}
	}

	/*
	**  Copy the data
	*/

	exitstat = EX_OK;
	if (unmake)
	{
		bool stop;
		errno = database->smdb_cursor(database, &cursor, 0);
		if (errno != SMDBE_OK)
		{

			fprintf(stderr,
				"%s: cannot make cursor for type %s map %s\n",
				progname, typename, mapname);
			exit(EX_SOFTWARE);
		}

		memset(&db_key, '\0', sizeof db_key);
		memset(&db_val, '\0', sizeof db_val);

		for (stop = FALSE, lineno = 0; !stop; lineno++)
		{
			errno = cursor->smdbc_get(cursor, &db_key, &db_val,
						  SMDB_CURSOR_GET_NEXT);
			if (errno != SMDBE_OK)
			{
				stop = TRUE;
			}
			if (!stop)
				printf("%.*s\t%.*s\n",
				       (int) db_key.data.size,
				       (char *) db_key.data.data,
				       (int) db_val.data.size,
				       (char *)db_val.data.data);

		}
		(void) cursor->smdbc_close(cursor);
	}
	else
	{
		lineno = 0;
		while (fgets(ibuf, sizeof ibuf, stdin) != NULL)
		{
			register char *p;

			lineno++;

			/*
			**  Parse the line.
			*/

			p = strchr(ibuf, '\n');
			if (p != NULL)
				*p = '\0';
			else if (!feof(stdin))
			{
				fprintf(stderr,
					"%s: %s: line %d: line too long (%ld bytes max)\n",
					progname, mapname, lineno, (long) sizeof ibuf);
				exitstat = EX_DATAERR;
				continue;
			}

			if (ibuf[0] == '\0' || ibuf[0] == '#')
				continue;
			if (
#if _FFR_DELIM
			    sep == '\0' &&
#endif /* _FFR_DELIM */
			    isascii(ibuf[0]) && isspace(ibuf[0]))
			{
				fprintf(stderr,
					"%s: %s: line %d: syntax error (leading space)\n",
					progname, mapname, lineno);
				exitstat = EX_DATAERR;
				continue;
			}

			memset(&db_key, '\0', sizeof db_key);
			memset(&db_val, '\0', sizeof db_val);
			db_key.data.data = ibuf;

			for (p = ibuf; *p != '\0' && !(ISSEP(*p)); p++)
			{
				if (foldcase && isascii(*p) && isupper(*p))
					*p = tolower(*p);
			}
			db_key.data.size = p - ibuf;
			if (inclnull)
				db_key.data.size++;

			if (*p != '\0')
				*p++ = '\0';
			while (ISSEP(*p))
				p++;
			if (!allowempty && *p == '\0')
			{
				fprintf(stderr,
					"%s: %s: line %d: no RHS for LHS %s\n",
					progname, mapname, lineno,
					(char *) db_key.data.data);
				exitstat = EX_DATAERR;
				continue;
			}

			db_val.data.data = p;
			db_val.data.size = strlen(p);
			if (inclnull)
				db_val.data.size++;

			/*
			**  Do the database insert.
			*/

			if (verbose)
			{
				printf("key=`%s', val=`%s'\n",
				       (char *) db_key.data.data,
				       (char *) db_val.data.data);
			}

			errno = database->smdb_put(database, &db_key, &db_val,
						   putflags);
			switch (errno)
			{
			  case SMDBE_KEY_EXIST:
				st = 1;
				break;

			  case 0:
				st = 0;
				break;

			  default:
				st = -1;
				break;
			}

			if (st < 0)
			{
				fprintf(stderr,
					"%s: %s: line %d: key %s: put error: %s\n",
					progname, mapname, lineno,
					(char *) db_key.data.data,
					errstring(errno));
				exitstat = EX_IOERR;
			}
			else if (st > 0)
			{
				fprintf(stderr,
					"%s: %s: line %d: key %s: duplicate key\n",
					progname, mapname,
					lineno, (char *) db_key.data.data);
				exitstat = EX_DATAERR;
			}
		}
	}

	/*
	**  Now close the database.
	*/

	errno = database->smdb_close(database);
	if (errno != SMDBE_OK)
	{
		fprintf(stderr, "%s: close(%s): %s\n",
			progname, mapname, errstring(errno));
		exitstat = EX_IOERR;
	}
	smdb_free_database(database);

	exit(exitstat);
	/* NOTREACHED */
	return exitstat;
}

/*VARARGS1*/
void
#ifdef __STDC__
message(const char *msg, ...)
#else /* __STDC__ */
message(msg, va_alist)
	const char *msg;
	va_dcl
#endif /* __STDC__ */
{
	const char *m;
	VA_LOCAL_DECL

	m = msg;
	if (isascii(m[0]) && isdigit(m[0]) &&
	    isascii(m[1]) && isdigit(m[1]) &&
	    isascii(m[2]) && isdigit(m[2]) && m[3] == ' ')
		m += 4;
	VA_START(msg);
	(void) vfprintf(stderr, m, ap);
	VA_END;
	(void) fprintf(stderr, "\n");
}

/*VARARGS1*/
void
#ifdef __STDC__
syserr(const char *msg, ...)
#else /* __STDC__ */
syserr(msg, va_alist)
	const char *msg;
	va_dcl
#endif /* __STDC__ */
{
	const char *m;
	VA_LOCAL_DECL

	m = msg;
	if (isascii(m[0]) && isdigit(m[0]) &&
	    isascii(m[1]) && isdigit(m[1]) &&
	    isascii(m[2]) && isdigit(m[2]) && m[3] == ' ')
		m += 4;
	VA_START(msg);
	(void) vfprintf(stderr, m, ap);
	VA_END;
	(void) fprintf(stderr, "\n");
}
