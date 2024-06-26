/* Top level stuff for GDB, the GNU debugger.
   Copyright 1986-1995, 1999-2000 Free Software Foundation, Inc.

   This file is part of GDB.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.  */

#include "defs.h"
#include "top.h"
#include "target.h"
#include "inferior.h"
#include "call-cmds.h"

#include "getopt.h"

#include <sys/types.h>
#include "gdb_stat.h"
#include <ctype.h>

#include "gdb_string.h"
#include "event-loop.h"
#include "ui-out.h"
#if defined (TUI) || defined (GDBTK)
/* FIXME: cagney/2000-01-31: This #include is to allow older code such
   as that found in the TUI to continue to build. */
#include "tui/tui-file.h"
#endif

/* If nonzero, display time usage both at startup and for each command.  */

int display_time;

/* If nonzero, display space usage both at startup and for each command.  */

int display_space;

/* Whether this is the async version or not.  The async version is
   invoked on the command line with the -nw --async options.  In this
   version, the usual command_loop is substituted by and event loop which
   processes UI events asynchronously. */
int event_loop_p = 1;

#ifdef UI_OUT
/* Has an interpreter been specified and if so, which. 
   This will be used as a set command variable, so it should
   always be malloc'ed - since do_setshow_command will free it. */
char *interpreter_p;
#endif

/* Whether this is the command line version or not */
int tui_version = 0;

/* Whether xdb commands will be handled */
int xdb_commands = 0;

/* Whether dbx commands will be handled */
int dbx_commands = 0;

struct ui_file *gdb_stdout;
struct ui_file *gdb_stderr;
struct ui_file *gdb_stdlog;
struct ui_file *gdb_stdtarg;
struct ui_file *gdb_null;

/* Used to initialize error() - defined in utils.c */

extern void error_init (void);

/* Whether to enable writing into executable and core files */
extern int write_files;

static void print_gdb_help (struct ui_file *);

/* These two are used to set the external editor commands when gdb is farming
   out files to be edited by another program. */

extern int enable_external_editor;
extern char *external_editor_command;

#ifdef __CYGWIN__
#include <windows.h>		/* for MAX_PATH */
#include <sys/cygwin.h>		/* for cygwin32_conv_to_posix_path */
#endif

static char *
quote_string (char *s)
{
  char *ret = xmalloc (strlen (s) + 2 + 1);
  sprintf (ret, "\"%s\"", s);
  return ret;
}

/* Call command_loop.  If it happens to return, pass that through as a
   non-zero return status. */

static int
captured_command_loop (void *data)
{
  if (command_loop_hook == NULL)
    command_loop ();
  else
    command_loop_hook ();
  /* FIXME: cagney/1999-11-05: A correct command_loop() implementaton
     would clean things up (restoring the cleanup chain) to the state
     they were just prior to the call.  Technically, this means that
     the do_cleanups() below is redundant.  Unfortunately, many FUNCs
     are not that well behaved.  do_cleanups should either be replaced
     with a do_cleanups call (to cover the problem) or an assertion
     check to detect bad FUNCs code. */
  do_cleanups (ALL_CLEANUPS);
  /* If the command_loop returned, normally (rather than threw an
     error) we try to quit. If the quit is aborted, catch_errors()
     which called this catch the signal and restart the command
     loop. */
  quit_command (NULL, instream == stdin);
  return 1;
}

struct captured_main_args
  {
    int argc;
    char **argv;
  };

static int
captured_main (void *data)
{
  struct captured_main_args *context = data;
  int argc = context->argc;
  char **argv = context->argv;
  int count;
  static int quiet = 0;
  static int batch = 0;

  /* Pointers to various arguments from command line.  */
  char *symarg = NULL;
  char *execarg = NULL;
  char *corearg = NULL;
  char *cdarg = NULL;
  char *ttyarg = NULL;

  /* These are static so that we can take their address in an initializer.  */
  static int print_help;
  static int print_version;

  /* Pointers to all arguments of --command option.  */
  char **cmdarg;
  /* Allocated size of cmdarg.  */
  int cmdsize;
  /* Number of elements of cmdarg used.  */
  int ncmd;

  /* Indices of all arguments of --directory option.  */
  char **dirarg;
  /* Allocated size.  */
  int dirsize;
  /* Number of elements used.  */
  int ndir;

  struct stat homebuf, cwdbuf, globalbuf;
  char *homedir, *homeinit;

  register int i;

  long time_at_startup = get_run_time ();

  START_PROGRESS (argv[0], 0);

#ifdef MPW
  /* Do all Mac-specific setup. */
  mac_init ();
#endif /* MPW */

#if USE_MMALLOC
  init_mmalloc_default_pool ((PTR) NULL);
#endif
 
  /* This needs to happen before the first use of malloc.  */
  init_malloc ((PTR) NULL);

#if defined (ALIGN_STACK_ON_STARTUP)
  i = (int) &count & 0x3;
  if (i != 0)
    alloca (4 - i);
#endif

  cmdsize = 1;
  cmdarg = (char **) xmalloc (cmdsize * sizeof (*cmdarg));
  ncmd = 0;
  dirsize = 1;
  dirarg = (char **) xmalloc (dirsize * sizeof (*dirarg));
  ndir = 0;

  quit_flag = 0;
  line = (char *) xmalloc (linesize);
  line[0] = '\0';		/* Terminate saved (now empty) cmd line */
  instream = stdin;

  getcwd (gdb_dirbuf, sizeof (gdb_dirbuf));
  current_directory = gdb_dirbuf;

  gdb_null = ui_file_new ();

#if defined (TUI) || defined (GDBTK)
  /* Older code uses the tui_file and fputs_unfiltered_hook().  It
     should be using a customized UI_FILE object and re-initializing
     within its own _initialize function.  */
  gdb_stdout = tui_fileopen (stdout);
  gdb_stderr = tui_fileopen (stderr);
  gdb_stdlog = gdb_stdout;	/* for moment */
  gdb_stdtarg = gdb_stderr;	/* for moment */
#else
  gdb_stdout = stdio_fileopen (stdout);
  gdb_stderr = stdio_fileopen (stderr);
  gdb_stdlog = gdb_stderr;	/* for moment */
  gdb_stdtarg = gdb_stderr;	/* for moment */
#endif

  /* initialize error() */
  error_init ();

  /* Parse arguments and options.  */
  {
    int c;
    /* When var field is 0, use flag field to record the equivalent
       short option (or arbitrary numbers starting at 10 for those
       with no equivalent).  */
    static struct option long_options[] =
    {
      {"async", no_argument, &event_loop_p, 1},
      {"noasync", no_argument, &event_loop_p, 0},
#if defined(TUI)
      {"tui", no_argument, &tui_version, 1},
#endif
      {"xdb", no_argument, &xdb_commands, 1},
      {"dbx", no_argument, &dbx_commands, 1},
      {"readnow", no_argument, &readnow_symbol_files, 1},
      {"r", no_argument, &readnow_symbol_files, 1},
      {"mapped", no_argument, &mapped_symbol_files, 1},
      {"m", no_argument, &mapped_symbol_files, 1},
      {"quiet", no_argument, &quiet, 1},
      {"q", no_argument, &quiet, 1},
      {"silent", no_argument, &quiet, 1},
      {"nx", no_argument, &inhibit_gdbinit, 1},
      {"n", no_argument, &inhibit_gdbinit, 1},
      {"batch", no_argument, &batch, 1},
      {"epoch", no_argument, &epoch_interface, 1},

    /* This is a synonym for "--annotate=1".  --annotate is now preferred,
       but keep this here for a long time because people will be running
       emacses which use --fullname.  */
      {"fullname", no_argument, 0, 'f'},
      {"f", no_argument, 0, 'f'},

      {"annotate", required_argument, 0, 12},
      {"help", no_argument, &print_help, 1},
      {"se", required_argument, 0, 10},
      {"symbols", required_argument, 0, 's'},
      {"s", required_argument, 0, 's'},
      {"exec", required_argument, 0, 'e'},
      {"e", required_argument, 0, 'e'},
      {"core", required_argument, 0, 'c'},
      {"c", required_argument, 0, 'c'},
      {"command", required_argument, 0, 'x'},
      {"version", no_argument, &print_version, 1},
      {"x", required_argument, 0, 'x'},
#ifdef GDBTK
      {"tclcommand", required_argument, 0, 'z'},
      {"enable-external-editor", no_argument, 0, 'y'},
      {"editor-command", required_argument, 0, 'w'},
#endif
#ifdef UI_OUT
      {"ui", required_argument, 0, 'i'},
      {"interpreter", required_argument, 0, 'i'},
      {"i", required_argument, 0, 'i'},
#endif
      {"directory", required_argument, 0, 'd'},
      {"d", required_argument, 0, 'd'},
      {"cd", required_argument, 0, 11},
      {"tty", required_argument, 0, 't'},
      {"baud", required_argument, 0, 'b'},
      {"b", required_argument, 0, 'b'},
      {"nw", no_argument, &use_windows, 0},
      {"nowindows", no_argument, &use_windows, 0},
      {"w", no_argument, &use_windows, 1},
      {"windows", no_argument, &use_windows, 1},
      {"statistics", no_argument, 0, 13},
      {"write", no_argument, &write_files, 1},
/* Allow machine descriptions to add more options... */
#ifdef ADDITIONAL_OPTIONS
      ADDITIONAL_OPTIONS
#endif
      {0, no_argument, 0, 0}
    };

    while (1)
      {
	int option_index;

	c = getopt_long_only (argc, argv, "",
			      long_options, &option_index);
	if (c == EOF)
	  break;

	/* Long option that takes an argument.  */
	if (c == 0 && long_options[option_index].flag == 0)
	  c = long_options[option_index].val;

	switch (c)
	  {
	  case 0:
	    /* Long option that just sets a flag.  */
	    break;
	  case 10:
	    symarg = optarg;
	    execarg = optarg;
	    break;
	  case 11:
	    cdarg = optarg;
	    break;
	  case 12:
	    /* FIXME: what if the syntax is wrong (e.g. not digits)?  */
	    annotation_level = atoi (optarg);
	    break;
	  case 13:
	    /* Enable the display of both time and space usage.  */
	    display_time = 1;
	    display_space = 1;
	    break;
	  case 'f':
	    annotation_level = 1;
/* We have probably been invoked from emacs.  Disable window interface.  */
	    use_windows = 0;
	    break;
	  case 's':
	    symarg = optarg;
	    break;
	  case 'e':
	    execarg = optarg;
	    break;
	  case 'c':
	    corearg = optarg;
	    break;
	  case 'x':
	    cmdarg[ncmd++] = optarg;
	    if (ncmd >= cmdsize)
	      {
		cmdsize *= 2;
		cmdarg = (char **) xrealloc ((char *) cmdarg,
					     cmdsize * sizeof (*cmdarg));
	      }
	    break;
#ifdef GDBTK
	  case 'z':
	    {
extern int gdbtk_test (char *);
	      if (!gdbtk_test (optarg))
		{
		  fprintf_unfiltered (gdb_stderr, "%s: unable to load tclcommand file \"%s\"",
				      argv[0], optarg);
		  exit (1);
		}
	      break;
	    }
	  case 'y':
	    {
	      /*
	       * This enables the edit/button in the main window, even
	       * when IDE_ENABLED is set to false. In this case you must
	       * use --tclcommand to specify a tcl/script to be called,
	       * Tcl/Variable to store the edit/command is:
	       * external_editor
	       */
	      enable_external_editor = 1;
	      break;
	    }
	  case 'w':
	    {
	      /*
	       * if editor command is enabled, both flags are set
	       */
	      enable_external_editor = 1;
	      external_editor_command = xstrdup (optarg);
	      break;
	    }
#endif /* GDBTK */
#ifdef UI_OUT
	  case 'i':
	    interpreter_p = strsave(optarg);
	    break;
#endif
	  case 'd':
	    dirarg[ndir++] = optarg;
	    if (ndir >= dirsize)
	      {
		dirsize *= 2;
		dirarg = (char **) xrealloc ((char *) dirarg,
					     dirsize * sizeof (*dirarg));
	      }
	    break;
	  case 't':
	    ttyarg = optarg;
	    break;
	  case 'q':
	    quiet = 1;
	    break;
	  case 'b':
	    {
	      int i;
	      char *p;

	      i = strtol (optarg, &p, 0);
	      if (i == 0 && p == optarg)

		/* Don't use *_filtered or warning() (which relies on
		   current_target) until after initialize_all_files(). */

		fprintf_unfiltered
		  (gdb_stderr,
		   "warning: could not set baud rate to `%s'.\n", optarg);
	      else
		baud_rate = i;
	    }
	  case 'l':
	    {
	      int i;
	      char *p;

	      i = strtol (optarg, &p, 0);
	      if (i == 0 && p == optarg)

		/* Don't use *_filtered or warning() (which relies on
		   current_target) until after initialize_all_files(). */

		fprintf_unfiltered
		  (gdb_stderr,
		 "warning: could not set timeout limit to `%s'.\n", optarg);
	      else
		remote_timeout = i;
	    }
	    break;

#ifdef ADDITIONAL_OPTION_CASES
	    ADDITIONAL_OPTION_CASES
#endif
	  case '?':
	    fprintf_unfiltered (gdb_stderr,
			"Use `%s --help' for a complete list of options.\n",
				argv[0]);
	    exit (1);
	  }
      }

    /* If --help or --version, disable window interface.  */
    if (print_help || print_version)
      {
	use_windows = 0;
#ifdef TUI
	/* Disable the TUI as well.  */
	tui_version = 0;
#endif
      }

#ifdef TUI
    /* An explicit --tui flag overrides the default UI, which is the
       window system.  */
    if (tui_version)
      use_windows = 0;
#endif

    /* OK, that's all the options.  The other arguments are filenames.  */
    count = 0;
    for (; optind < argc; optind++)
      switch (++count)
	{
	case 1:
	  symarg = argv[optind];
	  execarg = argv[optind];
	  break;
	case 2:
	  corearg = argv[optind];
	  break;
	case 3:
	  fprintf_unfiltered (gdb_stderr,
			  "Excess command line arguments ignored. (%s%s)\n",
			  argv[optind], (optind == argc - 1) ? "" : " ...");
	  break;
	}
    if (batch)
      quiet = 1;
    if (symarg)
      symarg = quote_string (symarg);
    if (execarg)
      execarg = quote_string (execarg);
    if (corearg)
      corearg = quote_string (corearg);
  }

#if defined(TUI)
  /* Should this be moved to tui-top.c:_initialize_tui()? */
  if (tui_version)
    init_ui_hook = tuiInit;
#endif

  /* Initialize all files.  Give the interpreter a chance to take
     control of the console via the init_ui_hook()) */
  gdb_init (argv[0]);

  /* Do these (and anything which might call wrap_here or *_filtered)
     after initialize_all_files.  */
  if (print_version)
    {
      print_gdb_version (gdb_stdout);
      wrap_here ("");
      printf_filtered ("\n");
      exit (0);
    }

  if (print_help)
    {
      print_gdb_help (gdb_stdout);
      fputs_unfiltered ("\n", gdb_stdout);
      exit (0);
    }

  if (!quiet)
    {
      /* Print all the junk at the top, with trailing "..." if we are about
         to read a symbol file (possibly slowly).  */
      print_gdb_version (gdb_stdout);
#if 0
      if (symarg)
	printf_filtered ("..");
#else
      printf_filtered ("\n");
#endif
      wrap_here ("");
      gdb_flush (gdb_stdout);	/* Force to screen during slow operations */
    }

  if (state_change_hook)
    {
      state_change_hook (STATE_ACTIVE);
    }
  
  error_pre_print = "\n\n";
  quit_pre_print = error_pre_print;

  /* We may get more than one warning, don't double space all of them... */
  warning_pre_print = "\nwarning: ";

  /* Make sure that they are zero in case one of them fails (this
     guarantees that they won't match if either exists).  */
  
  memset (&globalbuf, 0, sizeof (struct stat));
  memset (&homebuf, 0, sizeof (struct stat));
  memset (&cwdbuf, 0, sizeof (struct stat));
  
  stat (gdbinit_global, &globalbuf);
  if (!inhibit_gdbinit)
    {
      /* if (!SET_TOP_LEVEL ()) */
	 source_command (gdbinit_global, 0);
    }
  do_cleanups (ALL_CLEANUPS);
 

  /* Read and execute $HOME/.gdbinit file, if it exists.  This is done
     *before* all the command line arguments are processed; it sets
     global parameters, which are independent of what file you are
     debugging or what directory you are in.  */
#ifdef __CYGWIN32__
  {
    char *tmp = getenv ("HOME");

    if (tmp != NULL)
      {
	homedir = (char *) alloca (MAX_PATH + 1);
	cygwin32_conv_to_posix_path (tmp, homedir);
      }
    else
      homedir = NULL;
  }
#else
  homedir = getenv ("HOME");
#endif
  if (homedir)
    {
      homeinit = (char *) alloca (strlen (homedir) +
				  strlen (gdbinit) + 10);
      strcpy (homeinit, homedir);
      strcat (homeinit, "/");
      strcat (homeinit, gdbinit);

      stat (homeinit, &homebuf);
      if (!inhibit_gdbinit)
	if ((globalbuf.st_dev != homebuf.st_dev) || (globalbuf.st_ino != homebuf.st_ino))
	  {
	    catch_command_errors (source_command, homeinit, 0, RETURN_MASK_ALL);
	  }
    }

  /* Now perform all the actions indicated by the arguments.  */
  if (cdarg != NULL)
    {
      catch_command_errors (cd_command, cdarg, 0, RETURN_MASK_ALL);
    }

  for (i = 0; i < ndir; i++)
    catch_command_errors (directory_command, dirarg[i], 0, RETURN_MASK_ALL);
  free ((PTR) dirarg);

  if (execarg != NULL
      && symarg != NULL
      && STREQ (execarg, symarg))
    {
      /* The exec file and the symbol-file are the same.  If we can't
         open it, better only print one error message.
         catch_command_errors returns non-zero on success! */
      if (catch_command_errors (exec_file_command, execarg, !batch, RETURN_MASK_ALL))
	catch_command_errors (symbol_file_command, symarg, 0, RETURN_MASK_ALL);
    }
  else
    {
      if (execarg != NULL)
	catch_command_errors (exec_file_command, execarg, !batch, RETURN_MASK_ALL);
      if (symarg != NULL)
	catch_command_errors (symbol_file_command, symarg, 0, RETURN_MASK_ALL);
    }

  if (state_change_hook && (symarg != NULL))
    {
      state_change_hook (STATE_INFERIOR_LOADED);
    }
  
  /* After the symbol file has been read, print a newline to get us
     beyond the copyright line...  But errors should still set off
     the error message with a (single) blank line.  */
#if 0
  if (!quiet)
    printf_filtered ("\n");
#endif
  error_pre_print = "\n";
  quit_pre_print = error_pre_print;
  warning_pre_print = "\nwarning: ";

  if (corearg != NULL)
    {
      if (catch_command_errors (core_file_command, corearg, !batch, RETURN_MASK_ALL) == 0)
	{
	  /* See if the core file is really a PID. */
	  /* Be careful, we have quoted the corearg above... */
	  if (corearg[0] == '"')
	    {
	      int len = strlen (corearg);
	      if (corearg[len - 1] == '"')
		{
		  corearg[len - 1] = '\0';
		  corearg += 1;
		}
	    }

	  if (isdigit (corearg[0]))
	    catch_command_errors (attach_command, corearg, !batch, RETURN_MASK_ALL);
	}
    }

  if (ttyarg != NULL)
    catch_command_errors (tty_command, ttyarg, !batch, RETURN_MASK_ALL);

#ifdef ADDITIONAL_OPTION_HANDLER
  ADDITIONAL_OPTION_HANDLER;
#endif

  /* Error messages should no longer be distinguished with extra output. */
  error_pre_print = NULL;
  quit_pre_print = NULL;
  warning_pre_print = "warning: ";

  /* Read the .gdbinit file in the current directory, *if* it isn't
     the same as the $HOME/.gdbinit file (it should exist, also).  */

  stat (gdbinit, &cwdbuf);
  if (!inhibit_gdbinit)
    if (((globalbuf.st_dev != cwdbuf.st_dev) || (globalbuf.st_ino != cwdbuf.st_ino))
	&& ((homebuf.st_dev != cwdbuf.st_dev) || (homebuf.st_ino != cwdbuf.st_ino)))
      {
	catch_command_errors (source_command, gdbinit, 0, RETURN_MASK_ALL);
      }

  for (i = 0; i < ncmd; i++)
    {
#if 0
      /* NOTE: cagney/1999-11-03: SET_TOP_LEVEL() was a macro that
         expanded into a call to setjmp().  */
      if (!SET_TOP_LEVEL ()) /* NB: This is #if 0'd out */
	{
	  /* NOTE: I am commenting this out, because it is not clear
	     where this feature is used. It is very old and
	     undocumented. ezannoni: 1999-05-04 */
#if 0
	  if (cmdarg[i][0] == '-' && cmdarg[i][1] == '\0')
	    read_command_file (stdin);
	  else
#endif
	    source_command (cmdarg[i], !batch);
	  do_cleanups (ALL_CLEANUPS);
	}
#endif
      catch_command_errors (source_command, cmdarg[i], !batch, RETURN_MASK_ALL);
    }
  free ((PTR) cmdarg);

  /* Read in the old history after all the command files have been read. */
  init_history ();

  if (batch)
    {
      /* We have hit the end of the batch file.  */
      exit (0);
    }

  /* Do any host- or target-specific hacks.  This is used for i960 targets
     to force the user to set a nindy target and spec its parameters.  */

#ifdef BEFORE_MAIN_LOOP_HOOK
  BEFORE_MAIN_LOOP_HOOK;
#endif

  END_PROGRESS (argv[0]);

  /* Show time and/or space usage.  */

  if (display_time)
    {
      long init_time = get_run_time () - time_at_startup;

      printf_unfiltered ("Startup time: %ld.%06ld\n",
			 init_time / 1000000, init_time % 1000000);
    }

  if (display_space)
    {
#ifdef HAVE_SBRK
      extern char **environ;
      char *lim = (char *) sbrk (0);

      printf_unfiltered ("Startup size: data size %ld\n",
			 (long) (lim - (char *) &environ));
#endif
    }

  /* The default command loop. 
     The WIN32 Gui calls this main to set up gdb's state, and 
     has its own command loop. */
#if !defined _WIN32 || defined __GNUC__
  /* GUIs generally have their own command loop, mainloop, or
     whatever.  This is a good place to gain control because many
     error conditions will end up here via longjmp(). */
#if 0
  /* FIXME: cagney/1999-11-06: The original main loop was like: */
  while (1)
    {
      if (!SET_TOP_LEVEL ())
	{
	  do_cleanups (ALL_CLEANUPS);	/* Do complete cleanup */
	  /* GUIs generally have their own command loop, mainloop, or whatever.
	     This is a good place to gain control because many error
	     conditions will end up here via longjmp(). */
	  if (command_loop_hook)
	    command_loop_hook ();
	  else
	    command_loop ();
	  quit_command ((char *) 0, instream == stdin);
	}
    }
  /* NOTE: If the command_loop() returned normally, the loop would
     attempt to exit by calling the function quit_command().  That
     function would either call exit() or throw an error returning
     control to SET_TOP_LEVEL. */
  /* NOTE: The function do_cleanups() was called once each time round
     the loop.  The usefulness of the call isn't clear.  If an error
     was thrown, everything would have already been cleaned up.  If
     command_loop() returned normally and quit_command() was called,
     either exit() or error() (again cleaning up) would be called. */
#endif
  /* NOTE: cagney/1999-11-07: There is probably no reason for not
     moving this loop and the code found in captured_command_loop()
     into the command_loop() proper.  The main thing holding back that
     change - SET_TOP_LEVEL() - has been eliminated. */
  while (1)
    {
      catch_errors (captured_command_loop, 0, "", RETURN_MASK_ALL);
    }
#endif
  /* No exit -- exit is through quit_command.  */
}

int
main (int argc, char **argv)
{
  struct captured_main_args args;
  args.argc = argc;
  args.argv = argv;
  catch_errors (captured_main, &args, "", RETURN_MASK_ALL);
  return 0;
}


/* Don't use *_filtered for printing help.  We don't want to prompt
   for continue no matter how small the screen or how much we're going
   to print.  */

static void
print_gdb_help (struct ui_file *stream)
{
  fputs_unfiltered ("\
This is the GNU debugger.  Usage:\n\n\
    gdb [options] [executable-file [core-file or process-id]]\n\n\
Options:\n\n\
", stream);
  fputs_unfiltered ("\
  --[no]async        Enable (disable) asynchronous version of CLI\n\
", stream);
  fputs_unfiltered ("\
  -b BAUDRATE        Set serial port baud rate used for remote debugging.\n\
  --batch            Exit after processing options.\n\
  --cd=DIR           Change current directory to DIR.\n\
  --command=FILE     Execute GDB commands from FILE.\n\
  --core=COREFILE    Analyze the core dump COREFILE.\n\
", stream);
  fputs_unfiltered ("\
  --dbx              DBX compatibility mode.\n\
  --directory=DIR    Search for source files in DIR.\n\
  --epoch            Output information used by epoch emacs-GDB interface.\n\
  --exec=EXECFILE    Use EXECFILE as the executable.\n\
  --fullname         Output information used by emacs-GDB interface.\n\
  --help             Print this message.\n\
", stream);
  fputs_unfiltered ("\
  --interpreter=INTERP\n\
                     Select a specific interpreter / user interface\n\
", stream);
  fputs_unfiltered ("\
  --mapped           Use mapped symbol files if supported on this system.\n\
  --nw		     Do not use a window interface.\n\
  --nx               Do not read ", stream);
  fputs_unfiltered (gdbinit, stream);
  fputs_unfiltered (" file.\n\
  --quiet            Do not print version number on startup.\n\
  --readnow          Fully read symbol files on first access.\n\
", stream);
  fputs_unfiltered ("\
  --se=FILE          Use FILE as symbol file and executable file.\n\
  --symbols=SYMFILE  Read symbols from SYMFILE.\n\
  --tty=TTY          Use TTY for input/output by the program being debugged.\n\
", stream);
#if defined(TUI)
  fputs_unfiltered ("\
  --tui              Use a terminal user interface.\n\
", stream);
#endif
  fputs_unfiltered ("\
  --version          Print version information and then exit.\n\
  -w                 Use a window interface.\n\
  --write            Set writing into executable and core files.\n\
  --xdb              XDB compatibility mode.\n\
", stream);
#ifdef ADDITIONAL_OPTION_HELP
  fputs_unfiltered (ADDITIONAL_OPTION_HELP, stream);
#endif
  fputs_unfiltered ("\n\
For more information, type \"help\" from within GDB, or consult the\n\
GDB manual (available as on-line info or a printed manual).\n\
Report bugs to \"bug-gdb@gnu.org\".\
", stream);
}
