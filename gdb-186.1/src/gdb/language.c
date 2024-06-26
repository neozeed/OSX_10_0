/* Multiple source language support for GDB.
   Copyright 1991, 1992, 2000 Free Software Foundation, Inc.
   Contributed by the Department of Computer Science at the State University
   of New York at Buffalo.

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

/* This file contains functions that return things that are specific
   to languages.  Each function should examine current_language if necessary,
   and return the appropriate result. */

/* FIXME:  Most of these would be better organized as macros which
   return data out of a "language-specific" struct pointer that is set
   whenever the working language changes.  That would be a lot faster.  */

#include "defs.h"
#include <ctype.h>
#include "gdb_string.h"

#include "symtab.h"
#include "gdbtypes.h"
#include "value.h"
#include "gdbcmd.h"
#include "frame.h"
#include "expression.h"
#include "language.h"
#include "target.h"
#include "parser-defs.h"
#include "jv-lang.h"

extern void _initialize_language (void);

static void show_language_command (char *, int);

static void set_language_command (char *, int);

static void show_type_command (char *, int);

static void set_type_command (char *, int);

static void show_range_command (char *, int);

static void set_range_command (char *, int);

static void show_case_command (char *, int);

static void set_case_command (char *, int);

static void set_case_str (void);

static void set_range_str (void);

static void set_type_str (void);

static void set_lang_str (void);

static void unk_lang_error (char *);

static int unk_lang_parser (void);

static void show_check (char *, int);

static void set_check (char *, int);

static void set_type_range_case (void);

static void unk_lang_emit_char (int c, struct ui_file *stream, int quoter);

static void unk_lang_printchar (int c, struct ui_file *stream);

static void unk_lang_printstr (struct ui_file * stream, char *string,
			       unsigned int length, int width,
			       int force_ellipses);

static struct type *unk_lang_create_fundamental_type (struct objfile *, int);

static void unk_lang_print_type (struct type *, char *, struct ui_file *,
				 int, int);

static int unk_lang_val_print (struct type *, char *, int, CORE_ADDR,
			       struct ui_file *, int, int, int,
			       enum val_prettyprint);

static int unk_lang_value_print (value_ptr, struct ui_file *, int, enum val_prettyprint);

/* Forward declaration */
extern const struct language_defn unknown_language_defn;
extern char *warning_pre_print;

/* The current (default at startup) state of type and range checking.
   (If the modes are set to "auto", though, these are changed based
   on the default language at startup, and then again based on the
   language of the first source file.  */

enum range_mode range_mode = range_mode_auto;
enum range_check range_check = range_check_off;
enum type_mode type_mode = type_mode_auto;
enum type_check type_check = type_check_off;
enum case_mode case_mode = case_mode_auto;
enum case_sensitivity case_sensitivity = case_sensitive_on;

/* The current language and language_mode (see language.h) */

const struct language_defn *current_language = &unknown_language_defn;
enum language_mode language_mode = language_mode_auto;

/* The language that the user expects to be typing in (the language
   of main(), or the last language we notified them about, or C).  */

const struct language_defn *expected_language;

/* The list of supported languages.  The list itself is malloc'd.  */

static const struct language_defn **languages;
static unsigned languages_size;
static unsigned languages_allocsize;
#define	DEFAULT_ALLOCSIZE 4

/* The "set language/type/range" commands all put stuff in these
   buffers.  This is to make them work as set/show commands.  The
   user's string is copied here, then the set_* commands look at
   them and update them to something that looks nice when it is
   printed out. */

static char *language;
static char *type;
static char *range;
static char *case_sensitive;

/* Warning issued when current_language and the language of the current
   frame do not match. */
char lang_frame_mismatch_warn[] =
"Warning: the current language does not match this frame.";


/* This page contains the functions corresponding to GDB commands
   and their helpers. */

/* Show command.  Display a warning if the language set
   does not match the frame. */
static void
show_language_command (char *ignore, int from_tty)
{
  enum language flang;		/* The language of the current frame */

  flang = get_frame_language ();
  if (flang != language_unknown &&
      language_mode == language_mode_manual &&
      current_language->la_language != flang)
    printf_filtered ("%s\n", lang_frame_mismatch_warn);
}

/* Set command.  Change the current working language. */
static void
set_language_command (char *ignore, int from_tty)
{
  int i;
  enum language flang;
  char *err_lang;

  if (!language || !language[0])
    {
      printf_unfiltered ("The currently understood settings are:\n\n");
      printf_unfiltered ("local or auto    Automatic setting based on source file\n");

      for (i = 0; i < languages_size; ++i)
	{
	  /* Already dealt with these above.  */
	  if (languages[i]->la_language == language_unknown
	      || languages[i]->la_language == language_auto)
	    continue;

	  /* FIXME for now assume that the human-readable name is just
	     a capitalization of the internal name.  */
	  printf_unfiltered ("%-16s Use the %c%s language\n",
			     languages[i]->la_name,
	  /* Capitalize first letter of language
	     name.  */
			     toupper (languages[i]->la_name[0]),
			     languages[i]->la_name + 1);
	}
      /* Restore the silly string. */
      set_language (current_language->la_language);
      return;
    }

  /* Search the list of languages for a match.  */
  for (i = 0; i < languages_size; i++)
    {
      if (STREQ (languages[i]->la_name, language))
	{
	  /* Found it!  Go into manual mode, and use this language.  */
	  if (languages[i]->la_language == language_auto)
	    {
	      /* Enter auto mode.  Set to the current frame's language, if known.  */
	      language_mode = language_mode_auto;
	      flang = get_frame_language ();
	      if (flang != language_unknown)
		set_language (flang);
	      expected_language = current_language;
	      return;
	    }
	  else
	    {
	      /* Enter manual mode.  Set the specified language.  */
	      language_mode = language_mode_manual;
	      current_language = languages[i];
	      set_type_range_case ();
	      set_lang_str ();
	      expected_language = current_language;
	      return;
	    }
	}
    }

  /* Reset the language (esp. the global string "language") to the 
     correct values. */
  err_lang = savestring (language, strlen (language));
  make_cleanup (free, err_lang);	/* Free it after error */
  set_language (current_language->la_language);
  error ("Unknown language `%s'.", err_lang);
}

/* Show command.  Display a warning if the type setting does
   not match the current language. */
static void
show_type_command (char *ignore, int from_tty)
{
  if (type_check != current_language->la_type_check)
    printf_unfiltered (
			"Warning: the current type check setting does not match the language.\n");
}

/* Set command.  Change the setting for type checking. */
static void
set_type_command (char *ignore, int from_tty)
{
  if (STREQ (type, "on"))
    {
      type_check = type_check_on;
      type_mode = type_mode_manual;
    }
  else if (STREQ (type, "warn"))
    {
      type_check = type_check_warn;
      type_mode = type_mode_manual;
    }
  else if (STREQ (type, "off"))
    {
      type_check = type_check_off;
      type_mode = type_mode_manual;
    }
  else if (STREQ (type, "auto"))
    {
      type_mode = type_mode_auto;
      set_type_range_case ();
      /* Avoid hitting the set_type_str call below.  We
         did it in set_type_range_case. */
      return;
    }
  else
    {
      warning ("Unrecognized type check setting: \"%s\"", type);
    }
  set_type_str ();
  show_type_command ((char *) NULL, from_tty);
}

/* Show command.  Display a warning if the range setting does
   not match the current language. */
static void
show_range_command (char *ignore, int from_tty)
{

  if (range_check != current_language->la_range_check)
    printf_unfiltered (
			"Warning: the current range check setting does not match the language.\n");
}

/* Set command.  Change the setting for range checking. */
static void
set_range_command (char *ignore, int from_tty)
{
  if (STREQ (range, "on"))
    {
      range_check = range_check_on;
      range_mode = range_mode_manual;
    }
  else if (STREQ (range, "warn"))
    {
      range_check = range_check_warn;
      range_mode = range_mode_manual;
    }
  else if (STREQ (range, "off"))
    {
      range_check = range_check_off;
      range_mode = range_mode_manual;
    }
  else if (STREQ (range, "auto"))
    {
      range_mode = range_mode_auto;
      set_type_range_case ();
      /* Avoid hitting the set_range_str call below.  We
         did it in set_type_range_case. */
      return;
    }
  else
    {
      warning ("Unrecognized range check setting: \"%s\"", range);
    }
  set_range_str ();
  show_range_command ((char *) 0, from_tty);
}

/* Show command.  Display a warning if the case sensitivity setting does
   not match the current language. */
static void
show_case_command (char *ignore, int from_tty)
{
   if (case_sensitivity != current_language->la_case_sensitivity)
      printf_unfiltered(
"Warning: the current case sensitivity setting does not match the language.\n");
}

/* Set command.  Change the setting for case sensitivity. */
static void
set_case_command (char *ignore, int from_tty)
{
   if (STREQ (case_sensitive, "on"))
   {
      case_sensitivity = case_sensitive_on;
      case_mode = case_mode_manual;
   }
   else if (STREQ (case_sensitive, "off"))
   {
      case_sensitivity = case_sensitive_off;
      case_mode = case_mode_manual;
   }
   else if (STREQ (case_sensitive, "auto"))
   {
      case_mode = case_mode_auto;
      set_type_range_case ();
      /* Avoid hitting the set_case_str call below.  We
         did it in set_type_range_case. */
      return;
   }
   else
   {
      warning ("Unrecognized case-sensitive setting: \"%s\"", case_sensitive);
   }
   set_case_str();
   show_case_command ((char *) NULL, from_tty);
}

/* Set the status of range and type checking and case sensitivity based on
   the current modes and the current language.
   If SHOW is non-zero, then print out the current language,
   type and range checking status. */
static void
set_type_range_case (void)
{

  if (range_mode == range_mode_auto)
    range_check = current_language->la_range_check;

  if (type_mode == type_mode_auto)
    type_check = current_language->la_type_check;

  if (case_mode == case_mode_auto)
    case_sensitivity = current_language->la_case_sensitivity;

  set_type_str ();
  set_range_str ();
  set_case_str ();
}

/* Set current language to (enum language) LANG.  Returns previous language. */

enum language
set_language (enum language lang)
{
  int i;
  enum language prev_language;

  prev_language = current_language->la_language;

  for (i = 0; i < languages_size; i++)
    {
      if (languages[i]->la_language == lang)
	{
	  current_language = languages[i];
	  set_type_range_case ();
	  set_lang_str ();
	  break;
	}
    }

  return prev_language;
}

/* This page contains functions that update the global vars
   language, type and range. */
static void
set_lang_str (void)
{
  char *prefix = "";

  if (language)
    free (language);
  if (language_mode == language_mode_auto)
    prefix = "auto; currently ";

  language = concat (prefix, current_language->la_name, NULL);
}

static void
set_type_str (void)
{
  char *tmp = NULL, *prefix = "";

  if (type)
    free (type);
  if (type_mode == type_mode_auto)
    prefix = "auto; currently ";

  switch (type_check)
    {
    case type_check_on:
      tmp = "on";
      break;
    case type_check_off:
      tmp = "off";
      break;
    case type_check_warn:
      tmp = "warn";
      break;
    default:
      error ("Unrecognized type check setting.");
    }

  type = concat (prefix, tmp, NULL);
}

static void
set_range_str (void)
{
  char *tmp, *pref = "";

  if (range_mode == range_mode_auto)
    pref = "auto; currently ";

  switch (range_check)
    {
    case range_check_on:
      tmp = "on";
      break;
    case range_check_off:
      tmp = "off";
      break;
    case range_check_warn:
      tmp = "warn";
      break;
    default:
      error ("Unrecognized range check setting.");
    }

  if (range)
    free (range);
  range = concat (pref, tmp, NULL);
}

static void
set_case_str()
{
   char *tmp = NULL, *prefix = "";

   if (case_mode==case_mode_auto)
      prefix = "auto; currently ";

   switch (case_sensitivity)
   {
   case case_sensitive_on:
     tmp = "on";
     break;
   case case_sensitive_off:
     tmp = "off";
     break;
   default:
     error ("Unrecognized case-sensitive setting.");
   }

   free (case_sensitive);
   case_sensitive = concat (prefix, tmp, NULL);
}

/* Print out the current language settings: language, range and
   type checking.  If QUIETLY, print only what has changed.  */

void
language_info (int quietly)
{
  if (quietly && expected_language == current_language)
    return;

  expected_language = current_language;
  printf_unfiltered ("Current language:  %s\n", language);
  show_language_command ((char *) 0, 1);

  if (!quietly)
    {
      printf_unfiltered ("Type checking:     %s\n", type);
      show_type_command ((char *) 0, 1);
      printf_unfiltered ("Range checking:    %s\n", range);
      show_range_command ((char *) 0, 1);
      printf_unfiltered ("Case sensitivity:  %s\n", case_sensitive);
      show_case_command ((char *) 0, 1);
    }
}

/* Return the result of a binary operation. */

#if 0				/* Currently unused */

struct type *
binop_result_type (value_ptr v1, value_ptr v2)
{
  int size, uns;
  struct type *t1 = check_typedef (VALUE_TYPE (v1));
  struct type *t2 = check_typedef (VALUE_TYPE (v2));

  int l1 = TYPE_LENGTH (t1);
  int l2 = TYPE_LENGTH (t2);

  switch (current_language->la_language)
    {
    case language_c:
    case language_cplus:
    case language_objc:
      if (TYPE_CODE (t1) == TYPE_CODE_FLT)
	return TYPE_CODE (t2) == TYPE_CODE_FLT && l2 > l1 ?
	  VALUE_TYPE (v2) : VALUE_TYPE (v1);
      else if (TYPE_CODE (t2) == TYPE_CODE_FLT)
	return TYPE_CODE (t1) == TYPE_CODE_FLT && l1 > l2 ?
	  VALUE_TYPE (v1) : VALUE_TYPE (v2);
      else if (TYPE_UNSIGNED (t1) && l1 > l2)
	return VALUE_TYPE (v1);
      else if (TYPE_UNSIGNED (t2) && l2 > l1)
	return VALUE_TYPE (v2);
      else			/* Both are signed.  Result is the longer type */
	return l1 > l2 ? VALUE_TYPE (v1) : VALUE_TYPE (v2);
      break;
    case language_m2:
      /* If we are doing type-checking, l1 should equal l2, so this is
         not needed. */
      return l1 > l2 ? VALUE_TYPE (v1) : VALUE_TYPE (v2);
      break;
    case language_chill:
      error ("Missing Chill support in function binop_result_check.");	/*FIXME */
    }
  abort ();
  return (struct type *) 0;	/* For lint */
}

#endif /* 0 */


/* This page contains functions that return format strings for
   printf for printing out numbers in different formats */

/* Returns the appropriate printf format for hexadecimal
   numbers. */
char *
local_hex_format_custom (char *pre)
{
  static char form[50];

  strcpy (form, local_hex_format_prefix ());
  strcat (form, "%");
  strcat (form, pre);
  strcat (form, local_hex_format_specifier ());
  strcat (form, local_hex_format_suffix ());
  return form;
}

#if 0
/* FIXME: cagney/2000-03-04: This function does not appear to be used.
   It can be deleted once 5.0 has been released. */
/* FIXME: cagney/2000-03-04: This code assumes that the compiler
   supports ``long long''. */
/* Converts a number to hexadecimal (without leading "0x") and stores it in a
   static string.  Returns a pointer to this string. */

char *
longest_raw_hex_string (LONGEST num)
{
  static char res_longest_raw_hex_string[50];
  long long ll = num;		/* MERGEBUG ?? see below */
  res_longest_raw_hex_string[0] = 0;
  /* MERGEBUG ?? As a quick fix I am replacing this with sprintf 
     strcat_address_numeric (num, 0, res_longest_raw_hex_string, 50); 
  */

  sprintf (res_longest_raw_hex_string, "%llx", ll);
  return res_longest_raw_hex_string;
}
#endif

/* Converts a number to hexadecimal and stores it in a static
   string.  Returns a pointer to this string. */
char *
local_hex_string (unsigned long num)
{
  static char res[50];

  sprintf (res, local_hex_format (), num);
  return res;
}

/* Converts a LONGEST number to hexadecimal and stores it in a static
   string.  Returns a pointer to this string. */
char *
longest_local_hex_string (LONGEST num)
{
  return longest_local_hex_string_custom (num, "l");
}

/* Converts a number to custom hexadecimal and stores it in a static
   string.  Returns a pointer to this string. */
char *
local_hex_string_custom (unsigned long num, char *pre)
{
  static char res[50];

  sprintf (res, local_hex_format_custom (pre), num);
  return res;
}

/* Converts a LONGEST number to custom hexadecimal and stores it in a static
   string.  Returns a pointer to this string. Note that the width parameter
   should end with "l", e.g. "08l" as with calls to local_hex_string_custom */

char *
longest_local_hex_string_custom (LONGEST num, char *width)
{
#define RESULT_BUF_LEN 50
  static char res2[RESULT_BUF_LEN];
  char format[RESULT_BUF_LEN];
#if !defined (PRINTF_HAS_LONG_LONG)
  int field_width;
  int num_len;
  int num_pad_chars;
  char *pad_char;		/* string with one character */
  int pad_on_left;
  char *parse_ptr;
  char temp_nbr_buf[RESULT_BUF_LEN];
#endif

#ifndef CC_HAS_LONG_LONG
  /* If there is no long long, then LONGEST should be just long and we
     can use local_hex_string_custom 
   */
  return local_hex_string_custom ((unsigned long) num, width);
#elif defined (PRINTF_HAS_LONG_LONG)
  /* Just use printf.  */
  strcpy (format, local_hex_format_prefix ());	/* 0x */
  strcat (format, "%");
  strcat (format, width);	/* e.g. "08l" */
  strcat (format, "l");		/* need "ll" for long long */
  strcat (format, local_hex_format_specifier ());	/* "x" */
  strcat (format, local_hex_format_suffix ());	/* "" */
  sprintf (res2, format, num);
  return res2;
#else /* !defined (PRINTF_HAS_LONG_LONG) */
  /* Use strcat_address_numeric to print the number into a string, then
     build the result string from local_hex_format_prefix, padding and 
     the hex representation as indicated by "width".  */

  temp_nbr_buf[0] = 0;
  /* With use_local == 0, we don't get the leading "0x" prefix. */
  /* MERGEBUG ?? As a quick fix I am replacing this call to
     strcat_address_numeric with sprintf
     strcat_address_numeric(num, 0, temp_nbr_buf, RESULT_BUF_LEN);
   */

  {
    long long ll = num;
    sprintf (temp_nbr_buf, "%llx", ll);
  }
  /* parse width */
  parse_ptr = width;
  pad_on_left = 1;
  pad_char = " ";
  if (*parse_ptr == '-')
    {
      parse_ptr++;
      pad_on_left = 0;
    }
  if (*parse_ptr == '0')
    {
      parse_ptr++;
      if (pad_on_left)
	pad_char = "0";		/* If padding is on the right, it is blank */
    }
  field_width = atoi (parse_ptr);
  num_len = strlen (temp_nbr_buf);
  num_pad_chars = field_width - strlen (temp_nbr_buf);	/* possibly negative */

  if (strlen (local_hex_format_prefix ()) + num_len + num_pad_chars
      < RESULT_BUF_LEN)		/* paranoia */
    internal_error ("longest_local_hex_string_custom: insufficient space to store result");

  strcpy (res2, local_hex_format_prefix ());
  if (pad_on_left)
    {
      while (num_pad_chars > 0)
	{
	  strcat (res2, pad_char);
	  num_pad_chars--;
	}
    }
  strcat (res2, temp_nbr_buf);
  if (!pad_on_left)
    {
      while (num_pad_chars > 0)
	{
	  strcat (res2, pad_char);
	  num_pad_chars--;
	}
    }
  return res2;
#endif

}				/* longest_local_hex_string_custom */

/* Returns the appropriate printf format for octal
   numbers. */
char *
local_octal_format_custom (char *pre)
{
  static char form[50];

  strcpy (form, local_octal_format_prefix ());
  strcat (form, "%");
  strcat (form, pre);
  strcat (form, local_octal_format_specifier ());
  strcat (form, local_octal_format_suffix ());
  return form;
}

/* Returns the appropriate printf format for decimal numbers. */
char *
local_decimal_format_custom (char *pre)
{
  static char form[50];

  strcpy (form, local_decimal_format_prefix ());
  strcat (form, "%");
  strcat (form, pre);
  strcat (form, local_decimal_format_specifier ());
  strcat (form, local_decimal_format_suffix ());
  return form;
}

#if 0
/* This page contains functions that are used in type/range checking.
   They all return zero if the type/range check fails.

   It is hoped that these will make extending GDB to parse different
   languages a little easier.  These are primarily used in eval.c when
   evaluating expressions and making sure that their types are correct.
   Instead of having a mess of conjucted/disjuncted expressions in an "if",
   the ideas of type can be wrapped up in the following functions.

   Note that some of them are not currently dependent upon which language
   is currently being parsed.  For example, floats are the same in
   C and Modula-2 (ie. the only floating point type has TYPE_CODE of
   TYPE_CODE_FLT), while booleans are different. */

/* Returns non-zero if its argument is a simple type.  This is the same for
   both Modula-2 and for C.  In the C case, TYPE_CODE_CHAR will never occur,
   and thus will never cause the failure of the test. */
int
simple_type (struct type *type)
{
  CHECK_TYPEDEF (type);
  switch (TYPE_CODE (type))
    {
    case TYPE_CODE_INT:
    case TYPE_CODE_CHAR:
    case TYPE_CODE_ENUM:
    case TYPE_CODE_FLT:
    case TYPE_CODE_RANGE:
    case TYPE_CODE_BOOL:
      return 1;

    default:
      return 0;
    }
}

/* Returns non-zero if its argument is of an ordered type.
   An ordered type is one in which the elements can be tested for the
   properties of "greater than", "less than", etc, or for which the
   operations "increment" or "decrement" make sense. */
int
ordered_type (struct type *type)
{
  CHECK_TYPEDEF (type);
  switch (TYPE_CODE (type))
    {
    case TYPE_CODE_INT:
    case TYPE_CODE_CHAR:
    case TYPE_CODE_ENUM:
    case TYPE_CODE_FLT:
    case TYPE_CODE_RANGE:
      return 1;

    default:
      return 0;
    }
}

/* Returns non-zero if the two types are the same */
int
same_type (struct type *arg1, struct type *arg2)
{
  CHECK_TYPEDEF (type);
  if (structured_type (arg1) ? !structured_type (arg2) : structured_type (arg2))
    /* One is structured and one isn't */
    return 0;
  else if (structured_type (arg1) && structured_type (arg2))
    return arg1 == arg2;
  else if (numeric_type (arg1) && numeric_type (arg2))
    return (TYPE_CODE (arg2) == TYPE_CODE (arg1)) &&
      (TYPE_UNSIGNED (arg1) == TYPE_UNSIGNED (arg2))
      ? 1 : 0;
  else
    return arg1 == arg2;
}

/* Returns non-zero if the type is integral */
int
integral_type (struct type *type)
{
  CHECK_TYPEDEF (type);
  switch (current_language->la_language)
    {
    case language_c:
    case language_cplus:
    case language_objc:
      return (TYPE_CODE (type) != TYPE_CODE_INT) &&
	(TYPE_CODE (type) != TYPE_CODE_ENUM) ? 0 : 1;
    case language_m2:
    case language_pascal:
      return TYPE_CODE (type) != TYPE_CODE_INT ? 0 : 1;
    case language_chill:
      error ("Missing Chill support in function integral_type.");	/*FIXME */
    default:
      error ("Language not supported.");
    }
}

/* Returns non-zero if the value is numeric */
int
numeric_type (struct type *type)
{
  CHECK_TYPEDEF (type);
  switch (TYPE_CODE (type))
    {
    case TYPE_CODE_INT:
    case TYPE_CODE_FLT:
      return 1;

    default:
      return 0;
    }
}

/* Returns non-zero if the value is a character type */
int
character_type (struct type *type)
{
  CHECK_TYPEDEF (type);
  switch (current_language->la_language)
    {
    case language_chill:
    case language_m2:
    case language_pascal:
      return TYPE_CODE (type) != TYPE_CODE_CHAR ? 0 : 1;

    case language_c:
    case language_cplus:
    case language_objc:
      return (TYPE_CODE (type) == TYPE_CODE_INT) &&
	TYPE_LENGTH (type) == sizeof (char)
      ? 1 : 0;
    default:
      return (0);
    }
}

/* Returns non-zero if the value is a string type */
int
string_type (struct type *type)
{
  CHECK_TYPEDEF (type);
  switch (current_language->la_language)
    {
    case language_chill:
    case language_m2:
    case language_pascal:
      return TYPE_CODE (type) != TYPE_CODE_STRING ? 0 : 1;

    case language_c:
    case language_cplus:
    case language_objc:
      /* C does not have distinct string type. */
      return (0);
    default:
      return (0);
    }
}

/* Returns non-zero if the value is a boolean type */
int
boolean_type (struct type *type)
{
  CHECK_TYPEDEF (type);
  if (TYPE_CODE (type) == TYPE_CODE_BOOL)
    return 1;
  switch (current_language->la_language)
    {
    case language_c:
    case language_cplus:
    case language_objc:
      /* Might be more cleanly handled by having a TYPE_CODE_INT_NOT_BOOL
         for CHILL and such languages, or a TYPE_CODE_INT_OR_BOOL for C.  */
      if (TYPE_CODE (type) == TYPE_CODE_INT)
	return 1;
    default:
      break;
    }
  return 0;
}

/* Returns non-zero if the value is a floating-point type */
int
float_type (struct type *type)
{
  CHECK_TYPEDEF (type);
  return TYPE_CODE (type) == TYPE_CODE_FLT;
}

/* Returns non-zero if the value is a pointer type */
int
pointer_type (struct type *type)
{
  return TYPE_CODE (type) == TYPE_CODE_PTR ||
    TYPE_CODE (type) == TYPE_CODE_REF;
}

/* Returns non-zero if the value is a structured type */
int
structured_type (struct type *type)
{
  CHECK_TYPEDEF (type);
  switch (current_language->la_language)
    {
    case language_c:
    case language_cplus:
    case language_objc:
      return (TYPE_CODE (type) == TYPE_CODE_STRUCT) ||
	(TYPE_CODE (type) == TYPE_CODE_UNION) ||
	(TYPE_CODE (type) == TYPE_CODE_ARRAY);
   case language_pascal:
      return (TYPE_CODE(type) == TYPE_CODE_STRUCT) ||
	 (TYPE_CODE(type) == TYPE_CODE_UNION) ||
	 (TYPE_CODE(type) == TYPE_CODE_SET) ||
	    (TYPE_CODE(type) == TYPE_CODE_ARRAY);
    case language_m2:
      return (TYPE_CODE (type) == TYPE_CODE_STRUCT) ||
	(TYPE_CODE (type) == TYPE_CODE_SET) ||
	(TYPE_CODE (type) == TYPE_CODE_ARRAY);
    case language_chill:
      error ("Missing Chill support in function structured_type.");	/*FIXME */
    default:
      return (0);
    }
}
#endif

struct type *
lang_bool_type (void)
{
  struct symbol *sym;
  struct type *type;
  switch (current_language->la_language)
    {
    case language_chill:
      return builtin_type_chill_bool;
    case language_fortran:
      sym = lookup_symbol ("logical", NULL, VAR_NAMESPACE, NULL, NULL);
      if (sym)
	{
	  type = SYMBOL_TYPE (sym);
	  if (type && TYPE_CODE (type) == TYPE_CODE_BOOL)
	    return type;
	}
      return builtin_type_f_logical_s2;
    case language_cplus:
    case language_pascal:
      if (current_language->la_language==language_cplus)
        {sym = lookup_symbol ("bool", NULL, VAR_NAMESPACE, NULL, NULL);}
      else
        {sym = lookup_symbol ("boolean", NULL, VAR_NAMESPACE, NULL, NULL);}
      if (sym)
	{
	  type = SYMBOL_TYPE (sym);
	  if (type && TYPE_CODE (type) == TYPE_CODE_BOOL)
	    return type;
	}
      return builtin_type_bool;
    case language_java:
      sym = lookup_symbol ("boolean", NULL, VAR_NAMESPACE, NULL, NULL);
      if (sym)
	{
	  type = SYMBOL_TYPE (sym);
	  if (type && TYPE_CODE (type) == TYPE_CODE_BOOL)
	    return type;
	}
      return java_boolean_type;
    default:
      return builtin_type_int;
    }
}

/* This page contains functions that return info about
   (struct value) values used in GDB. */

/* Returns non-zero if the value VAL represents a true value. */
int
value_true (value_ptr val)
{
  /* It is possible that we should have some sort of error if a non-boolean
     value is used in this context.  Possibly dependent on some kind of
     "boolean-checking" option like range checking.  But it should probably
     not depend on the language except insofar as is necessary to identify
     a "boolean" value (i.e. in C using a float, pointer, etc., as a boolean
     should be an error, probably).  */
  return !value_logical_not (val);
}

/* Returns non-zero if the operator OP is defined on
   the values ARG1 and ARG2. */

#if 0				/* Currently unused */

void
binop_type_check (value_ptr arg1, value_ptr arg2, int op)
{
  struct type *t1, *t2;

  /* If we're not checking types, always return success. */
  if (!STRICT_TYPE)
    return;

  t1 = VALUE_TYPE (arg1);
  if (arg2 != NULL)
    t2 = VALUE_TYPE (arg2);
  else
    t2 = NULL;

  switch (op)
    {
    case BINOP_ADD:
    case BINOP_SUB:
      if ((numeric_type (t1) && pointer_type (t2)) ||
	  (pointer_type (t1) && numeric_type (t2)))
	{
	  warning ("combining pointer and integer.\n");
	  break;
	}
    case BINOP_MUL:
    case BINOP_LSH:
    case BINOP_RSH:
      if (!numeric_type (t1) || !numeric_type (t2))
	type_op_error ("Arguments to %s must be numbers.", op);
      else if (!same_type (t1, t2))
	type_op_error ("Arguments to %s must be of the same type.", op);
      break;

    case BINOP_LOGICAL_AND:
    case BINOP_LOGICAL_OR:
      if (!boolean_type (t1) || !boolean_type (t2))
	type_op_error ("Arguments to %s must be of boolean type.", op);
      break;

    case BINOP_EQUAL:
      if ((pointer_type (t1) && !(pointer_type (t2) || integral_type (t2))) ||
	  (pointer_type (t2) && !(pointer_type (t1) || integral_type (t1))))
	type_op_error ("A pointer can only be compared to an integer or pointer.", op);
      else if ((pointer_type (t1) && integral_type (t2)) ||
	       (integral_type (t1) && pointer_type (t2)))
	{
	  warning ("combining integer and pointer.\n");
	  break;
	}
      else if (!simple_type (t1) || !simple_type (t2))
	type_op_error ("Arguments to %s must be of simple type.", op);
      else if (!same_type (t1, t2))
	type_op_error ("Arguments to %s must be of the same type.", op);
      break;

    case BINOP_REM:
    case BINOP_MOD:
      if (!integral_type (t1) || !integral_type (t2))
	type_op_error ("Arguments to %s must be of integral type.", op);
      break;

    case BINOP_LESS:
    case BINOP_GTR:
    case BINOP_LEQ:
    case BINOP_GEQ:
      if (!ordered_type (t1) || !ordered_type (t2))
	type_op_error ("Arguments to %s must be of ordered type.", op);
      else if (!same_type (t1, t2))
	type_op_error ("Arguments to %s must be of the same type.", op);
      break;

    case BINOP_ASSIGN:
      if (pointer_type (t1) && !integral_type (t2))
	type_op_error ("A pointer can only be assigned an integer.", op);
      else if (pointer_type (t1) && integral_type (t2))
	{
	  warning ("combining integer and pointer.");
	  break;
	}
      else if (!simple_type (t1) || !simple_type (t2))
	type_op_error ("Arguments to %s must be of simple type.", op);
      else if (!same_type (t1, t2))
	type_op_error ("Arguments to %s must be of the same type.", op);
      break;

    case BINOP_CONCAT:
      /* FIXME:  Needs to handle bitstrings as well. */
      if (!(string_type (t1) || character_type (t1) || integral_type (t1))
	|| !(string_type (t2) || character_type (t2) || integral_type (t2)))
	type_op_error ("Arguments to %s must be strings or characters.", op);
      break;

      /* Unary checks -- arg2 is null */

    case UNOP_LOGICAL_NOT:
      if (!boolean_type (t1))
	type_op_error ("Argument to %s must be of boolean type.", op);
      break;

    case UNOP_PLUS:
    case UNOP_NEG:
      if (!numeric_type (t1))
	type_op_error ("Argument to %s must be of numeric type.", op);
      break;

    case UNOP_IND:
      if (integral_type (t1))
	{
	  warning ("combining pointer and integer.\n");
	  break;
	}
      else if (!pointer_type (t1))
	type_op_error ("Argument to %s must be a pointer.", op);
      break;

    case UNOP_PREINCREMENT:
    case UNOP_POSTINCREMENT:
    case UNOP_PREDECREMENT:
    case UNOP_POSTDECREMENT:
      if (!ordered_type (t1))
	type_op_error ("Argument to %s must be of an ordered type.", op);
      break;

    default:
      /* Ok.  The following operators have different meanings in
         different languages. */
      switch (current_language->la_language)
	{
#ifdef _LANG_c
	case language_c:
	case language_cplus:
	case language_objc:
	  switch (op)
	    {
	    case BINOP_DIV:
	      if (!numeric_type (t1) || !numeric_type (t2))
		type_op_error ("Arguments to %s must be numbers.", op);
	      break;
	    }
	  break;
#endif

#ifdef _LANG_m2
	case language_m2:
	  switch (op)
	    {
	    case BINOP_DIV:
	      if (!float_type (t1) || !float_type (t2))
		type_op_error ("Arguments to %s must be floating point numbers.", op);
	      break;
	    case BINOP_INTDIV:
	      if (!integral_type (t1) || !integral_type (t2))
		type_op_error ("Arguments to %s must be of integral type.", op);
	      break;
	    }
#endif

#ifdef _LANG_pascal
      case language_pascal:
	 switch(op)
	 {
	 case BINOP_DIV:
	    if (!float_type(t1) && !float_type(t2))
	       type_op_error ("Arguments to %s must be floating point numbers.",op);
	    break;
	 case BINOP_INTDIV:
	    if (!integral_type(t1) || !integral_type(t2))
	       type_op_error ("Arguments to %s must be of integral type.",op);
	    break;
	 }
#endif

#ifdef _LANG_chill
	case language_chill:
	  error ("Missing Chill support in function binop_type_check.");	/*FIXME */
#endif

	}
    }
}

#endif /* 0 */


/* This page contains functions for the printing out of
   error messages that occur during type- and range-
   checking. */

/* Prints the format string FMT with the operator as a string
   corresponding to the opcode OP.  If FATAL is non-zero, then
   this is an error and error () is called.  Otherwise, it is
   a warning and printf() is called. */
void
op_error (char *fmt, enum exp_opcode op, int fatal)
{
  if (fatal)
    error (fmt, op_string (op));
  else
    {
      warning (fmt, op_string (op));
    }
}

/* These are called when a language fails a type- or range-check.
   The first argument should be a printf()-style format string, and
   the rest of the arguments should be its arguments.  If
   [type|range]_check is [type|range]_check_on, then return_to_top_level()
   is called in the style of error ().  Otherwise, the message is prefixed
   by the value of warning_pre_print and we do not return to the top level. */

void
type_error (char *string,...)
{
  va_list args;
  va_start (args, string);

  if (type_check == type_check_warn)
    fprintf_filtered (gdb_stderr, warning_pre_print);
  else
    error_begin ();

  vfprintf_filtered (gdb_stderr, string, args);
  fprintf_filtered (gdb_stderr, "\n");
  va_end (args);
  if (type_check == type_check_on)
    return_to_top_level (RETURN_ERROR);
}

void
range_error (char *string,...)
{
  va_list args;
  va_start (args, string);

  if (range_check == range_check_warn)
    fprintf_filtered (gdb_stderr, warning_pre_print);
  else
    error_begin ();

  vfprintf_filtered (gdb_stderr, string, args);
  fprintf_filtered (gdb_stderr, "\n");
  va_end (args);
  if (range_check == range_check_on)
    return_to_top_level (RETURN_ERROR);
}


/* This page contains miscellaneous functions */

/* Return the language enum for a given language string. */

enum language
language_enum (char *str)
{
  int i;

  for (i = 0; i < languages_size; i++)
    if (STREQ (languages[i]->la_name, str))
      return languages[i]->la_language;

  return language_unknown;
}

/* Return the language struct for a given language enum. */

const struct language_defn *
language_def (enum language lang)
{
  int i;

  for (i = 0; i < languages_size; i++)
    {
      if (languages[i]->la_language == lang)
	{
	  return languages[i];
	}
    }
  return NULL;
}

/* Return the language as a string */
char *
language_str (enum language lang)
{
  int i;

  for (i = 0; i < languages_size; i++)
    {
      if (languages[i]->la_language == lang)
	{
	  return languages[i]->la_name;
	}
    }
  return "Unknown";
}

static void
set_check (char *ignore, int from_tty)
{
  printf_unfiltered (
     "\"set check\" must be followed by the name of a check subcommand.\n");
  help_list (setchecklist, "set check ", -1, gdb_stdout);
}

static void
show_check (char *ignore, int from_tty)
{
  cmd_show_list (showchecklist, from_tty, "");
}

/* Add a language to the set of known languages.  */

void
add_language (const struct language_defn *lang)
{
  if (lang->la_magic != LANG_MAGIC)
    {
      fprintf_unfiltered (gdb_stderr, "Magic number of %s language struct wrong\n",
			  lang->la_name);
      abort ();
    }

  if (!languages)
    {
      languages_allocsize = DEFAULT_ALLOCSIZE;
      languages = (const struct language_defn **) xmalloc
	(languages_allocsize * sizeof (*languages));
    }
  if (languages_size >= languages_allocsize)
    {
      languages_allocsize *= 2;
      languages = (const struct language_defn **) xrealloc ((char *) languages,
				 languages_allocsize * sizeof (*languages));
    }
  languages[languages_size++] = lang;
}

/* Define the language that is no language.  */

static int
unk_lang_parser (void)
{
  return 1;
}

static void
unk_lang_error (char *msg)
{
  error ("Attempted to parse an expression with unknown language");
}

static void
unk_lang_emit_char (register int c, struct ui_file *stream, int quoter)
{
  error ("internal error - unimplemented function unk_lang_emit_char called.");
}

static void
unk_lang_printchar (register int c, struct ui_file *stream)
{
  error ("internal error - unimplemented function unk_lang_printchar called.");
}

static void
unk_lang_printstr (struct ui_file *stream, char *string, unsigned int length,
		   int width, int force_ellipses)
{
  error ("internal error - unimplemented function unk_lang_printstr called.");
}

static struct type *
unk_lang_create_fundamental_type (struct objfile *objfile, int typeid)
{
  error ("internal error - unimplemented function unk_lang_create_fundamental_type called.");
}

static void
unk_lang_print_type (struct type *type, char *varstring, struct ui_file *stream,
		     int show, int level)
{
  error ("internal error - unimplemented function unk_lang_print_type called.");
}

static int
unk_lang_val_print (struct type *type, char *valaddr, int embedded_offset,
		    CORE_ADDR address, struct ui_file *stream, int format,
		    int deref_ref, int recurse, enum val_prettyprint pretty)
{
  error ("internal error - unimplemented function unk_lang_val_print called.");
}

static int
unk_lang_value_print (value_ptr val, struct ui_file *stream, int format,
		      enum val_prettyprint pretty)
{
  error ("internal error - unimplemented function unk_lang_value_print called.");
}

static struct type **CONST_PTR (unknown_builtin_types[]) =
{
  0
};
static const struct op_print unk_op_print_tab[] =
{
  {NULL, OP_NULL, PREC_NULL, 0}
};

const struct language_defn unknown_language_defn =
{
  "unknown",
  language_unknown,
  &unknown_builtin_types[0],
  range_check_off,
  type_check_off,
  case_sensitive_on,
  unk_lang_parser,
  unk_lang_error,
  evaluate_subexp_standard,
  unk_lang_printchar,		/* Print character constant */
  unk_lang_printstr,
  unk_lang_emit_char,
  unk_lang_create_fundamental_type,
  unk_lang_print_type,		/* Print a type using appropriate syntax */
  unk_lang_val_print,		/* Print a value using appropriate syntax */
  unk_lang_value_print,		/* Print a top-level value */
  {"", "", "", ""},		/* Binary format info */
  {"0%lo", "0", "o", ""},	/* Octal format info */
  {"%ld", "", "d", ""},		/* Decimal format info */
  {"0x%lx", "0x", "x", ""},	/* Hex format info */
  unk_op_print_tab,		/* expression operators for printing */
  1,				/* c-style arrays */
  0,				/* String lower bound */
  &builtin_type_char,		/* Type of string elements */
  LANG_MAGIC
};

/* These two structs define fake entries for the "local" and "auto" options. */
const struct language_defn auto_language_defn =
{
  "auto",
  language_auto,
  &unknown_builtin_types[0],
  range_check_off,
  type_check_off,
  case_sensitive_on,
  unk_lang_parser,
  unk_lang_error,
  evaluate_subexp_standard,
  unk_lang_printchar,		/* Print character constant */
  unk_lang_printstr,
  unk_lang_emit_char,
  unk_lang_create_fundamental_type,
  unk_lang_print_type,		/* Print a type using appropriate syntax */
  unk_lang_val_print,		/* Print a value using appropriate syntax */
  unk_lang_value_print,		/* Print a top-level value */
  {"", "", "", ""},		/* Binary format info */
  {"0%lo", "0", "o", ""},	/* Octal format info */
  {"%ld", "", "d", ""},		/* Decimal format info */
  {"0x%lx", "0x", "x", ""},	/* Hex format info */
  unk_op_print_tab,		/* expression operators for printing */
  1,				/* c-style arrays */
  0,				/* String lower bound */
  &builtin_type_char,		/* Type of string elements */
  LANG_MAGIC
};

const struct language_defn local_language_defn =
{
  "local",
  language_auto,
  &unknown_builtin_types[0],
  range_check_off,
  type_check_off,
  case_sensitive_on,
  unk_lang_parser,
  unk_lang_error,
  evaluate_subexp_standard,
  unk_lang_printchar,		/* Print character constant */
  unk_lang_printstr,
  unk_lang_emit_char,
  unk_lang_create_fundamental_type,
  unk_lang_print_type,		/* Print a type using appropriate syntax */
  unk_lang_val_print,		/* Print a value using appropriate syntax */
  unk_lang_value_print,		/* Print a top-level value */
  {"", "", "", ""},		/* Binary format info */
  {"0%lo", "0", "o", ""},	/* Octal format info */
  {"%ld", "", "d", ""},		/* Decimal format info */
  {"0x%lx", "0x", "x", ""},	/* Hex format info */
  unk_op_print_tab,		/* expression operators for printing */
  1,				/* c-style arrays */
  0,				/* String lower bound */
  &builtin_type_char,		/* Type of string elements */
  LANG_MAGIC
};

/* Initialize the language routines */

void
_initialize_language (void)
{
  struct cmd_list_element *set, *show;

  /* GDB commands for language specific stuff */

  set = add_set_cmd ("language", class_support, var_string_noescape,
		     (char *) &language,
		     "Set the current source language.",
		     &setlist);
  show = add_show_from_set (set, &showlist);
  set->function.cfunc = set_language_command;
  show->function.cfunc = show_language_command;

  add_prefix_cmd ("check", no_class, set_check,
		  "Set the status of the type/range checker",
		  &setchecklist, "set check ", 0, &setlist);
  add_alias_cmd ("c", "check", no_class, 1, &setlist);
  add_alias_cmd ("ch", "check", no_class, 1, &setlist);

  add_prefix_cmd ("check", no_class, show_check,
		  "Show the status of the type/range checker",
		  &showchecklist, "show check ", 0, &showlist);
  add_alias_cmd ("c", "check", no_class, 1, &showlist);
  add_alias_cmd ("ch", "check", no_class, 1, &showlist);

  set = add_set_cmd ("type", class_support, var_string_noescape,
		     (char *) &type,
		     "Set type checking.  (on/warn/off/auto)",
		     &setchecklist);
  show = add_show_from_set (set, &showchecklist);
  set->function.cfunc = set_type_command;
  show->function.cfunc = show_type_command;

  set = add_set_cmd ("range", class_support, var_string_noescape,
		     (char *) &range,
		     "Set range checking.  (on/warn/off/auto)",
		     &setchecklist);
  show = add_show_from_set (set, &showchecklist);
  set->function.cfunc = set_range_command;
  show->function.cfunc = show_range_command;

  set = add_set_cmd ("case-sensitive", class_support, var_string_noescape,
                     (char *) &case_sensitive,
                     "Set case sensitivity in name search.  (on/off/auto)\n\
For Fortran the default is off; for other languages the default is on.",
                     &setlist);
  show = add_show_from_set (set, &showlist);
  set->function.cfunc = set_case_command;
  show->function.cfunc = show_case_command;

  add_language (&unknown_language_defn);
  add_language (&local_language_defn);
  add_language (&auto_language_defn);

  language = savestring ("auto", strlen ("auto"));
  type = savestring ("auto", strlen ("auto"));
  range = savestring ("auto", strlen ("auto"));
  case_sensitive = savestring ("auto",strlen ("auto"));

  /* Have the above take effect */
  set_language (language_auto);
}
