/* Pascal language support routines for GDB, the GNU debugger.
   Copyright 2000 Free Software Foundation, Inc.

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
   Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.  */

/* This file is derived from p-lang.c */

#include "defs.h"
#include "symtab.h"
#include "gdbtypes.h"
#include "expression.h"
#include "parser-defs.h"
#include "language.h"
#include "p-lang.h"
#include "valprint.h"

extern void _initialize_pascal_language (void);
static void pascal_one_char (int, struct ui_file *, int *);

/* Print the character C on STREAM as part of the contents of a literal
   string.
   In_quotes is reset to 0 if a char is written with #4 notation */

static void
pascal_one_char (register int c, struct ui_file *stream, int *in_quotes)
{

  c &= 0xFF;			/* Avoid sign bit follies */

  if ((c == '\'') || (PRINT_LITERAL_FORM (c)))
    {
      if (!(*in_quotes))
	fputs_filtered ("'", stream);
      *in_quotes = 1;
      if (c == '\'')
	{
	  fputs_filtered ("''", stream);
	}
      else
	fprintf_filtered (stream, "%c", c);
    }
  else
    {
      if (*in_quotes)
	fputs_filtered ("'", stream);
      *in_quotes = 0;
      fprintf_filtered (stream, "#%d", (unsigned int) c);
    }
}

static void pascal_emit_char (int c, struct ui_file *stream, int quoter);

/* Print the character C on STREAM as part of the contents of a literal
   string whose delimiter is QUOTER.  Note that that format for printing
   characters and strings is language specific. */

static void
pascal_emit_char (register int c, struct ui_file *stream, int quoter)
{
  int in_quotes = 0;
  pascal_one_char (c, stream, &in_quotes);
  if (in_quotes)
    fputs_filtered ("'", stream);
}

void
pascal_printchar (int c, struct ui_file *stream)
{
  int in_quotes = 0;
  pascal_one_char (c, stream, &in_quotes);
  if (in_quotes)
    fputs_filtered ("'", stream);
}

/* Print the character string STRING, printing at most LENGTH characters.
   Printing stops early if the number hits print_max; repeat counts
   are printed as appropriate.  Print ellipses at the end if we
   had to stop before printing LENGTH characters, or if FORCE_ELLIPSES.  */

void
pascal_printstr (struct ui_file *stream, char *string, unsigned int length,
		 int width, int force_ellipses)
{
  register unsigned int i;
  unsigned int things_printed = 0;
  int in_quotes = 0;
  int need_comma = 0;
  extern int inspect_it;

  /* If the string was not truncated due to `set print elements', and
     the last byte of it is a null, we don't print that, in traditional C
     style.  */
  if ((!force_ellipses) && length > 0 && string[length - 1] == '\0')
    length--;

  if (length == 0)
    {
      fputs_filtered ("''", stream);
      return;
    }

  for (i = 0; i < length && things_printed < print_max; ++i)
    {
      /* Position of the character we are examining
         to see whether it is repeated.  */
      unsigned int rep1;
      /* Number of repetitions we have detected so far.  */
      unsigned int reps;

      QUIT;

      if (need_comma)
	{
	  fputs_filtered (", ", stream);
	  need_comma = 0;
	}

      rep1 = i + 1;
      reps = 1;
      while (rep1 < length && string[rep1] == string[i])
	{
	  ++rep1;
	  ++reps;
	}

      if (reps > repeat_count_threshold)
	{
	  if (in_quotes)
	    {
	      if (inspect_it)
		fputs_filtered ("\\', ", stream);
	      else
		fputs_filtered ("', ", stream);
	      in_quotes = 0;
	    }
	  pascal_printchar (string[i], stream);
	  fprintf_filtered (stream, " <repeats %u times>", reps);
	  i = rep1 - 1;
	  things_printed += repeat_count_threshold;
	  need_comma = 1;
	}
      else
	{
	  int c = string[i];
	  if ((!in_quotes) && (PRINT_LITERAL_FORM (c)))
	    {
	      if (inspect_it)
		fputs_filtered ("\\'", stream);
	      else
		fputs_filtered ("'", stream);
	      in_quotes = 1;
	    }
	  pascal_one_char (c, stream, &in_quotes);
	  ++things_printed;
	}
    }

  /* Terminate the quotes if necessary.  */
  if (in_quotes)
    {
      if (inspect_it)
	fputs_filtered ("\\'", stream);
      else
	fputs_filtered ("'", stream);
    }

  if (force_ellipses || i < length)
    fputs_filtered ("...", stream);
}

/* Create a fundamental Pascal type using default reasonable for the current
   target machine.

   Some object/debugging file formats (DWARF version 1, COFF, etc) do not
   define fundamental types such as "int" or "double".  Others (stabs or
   DWARF version 2, etc) do define fundamental types.  For the formats which
   don't provide fundamental types, gdb can create such types using this
   function.

   FIXME:  Some compilers distinguish explicitly signed integral types
   (signed short, signed int, signed long) from "regular" integral types
   (short, int, long) in the debugging information.  There is some dis-
   agreement as to how useful this feature is.  In particular, gcc does
   not support this.  Also, only some debugging formats allow the
   distinction to be passed on to a debugger.  For now, we always just
   use "short", "int", or "long" as the type name, for both the implicit
   and explicitly signed types.  This also makes life easier for the
   gdb test suite since we don't have to account for the differences
   in output depending upon what the compiler and debugging format
   support.  We will probably have to re-examine the issue when gdb
   starts taking it's fundamental type information directly from the
   debugging information supplied by the compiler.  fnf@cygnus.com */

/* Note there might be some discussion about the choosen correspondance
   because it mainly reflects Free Pascal Compiler setup for now PM */


struct type *
pascal_create_fundamental_type (struct objfile *objfile, int typeid)
{
  register struct type *type = NULL;

  switch (typeid)
    {
    default:
      /* FIXME:  For now, if we are asked to produce a type not in this
         language, create the equivalent of a C integer type with the
         name "<?type?>".  When all the dust settles from the type
         reconstruction work, this should probably become an error. */
      type = init_type (TYPE_CODE_INT,
			TARGET_INT_BIT / TARGET_CHAR_BIT,
			0, "<?type?>", objfile);
      warning ("internal error: no Pascal fundamental type %d", typeid);
      break;
    case FT_VOID:
      type = init_type (TYPE_CODE_VOID,
			TARGET_CHAR_BIT / TARGET_CHAR_BIT,
			0, "void", objfile);
      break;
    case FT_CHAR:
      type = init_type (TYPE_CODE_INT,
			TARGET_CHAR_BIT / TARGET_CHAR_BIT,
			0, "char", objfile);
      break;
    case FT_SIGNED_CHAR:
      type = init_type (TYPE_CODE_INT,
			TARGET_CHAR_BIT / TARGET_CHAR_BIT,
			0, "shortint", objfile);
      break;
    case FT_UNSIGNED_CHAR:
      type = init_type (TYPE_CODE_INT,
			TARGET_CHAR_BIT / TARGET_CHAR_BIT,
			TYPE_FLAG_UNSIGNED, "byte", objfile);
      break;
    case FT_SHORT:
      type = init_type (TYPE_CODE_INT,
			TARGET_SHORT_BIT / TARGET_CHAR_BIT,
			0, "integer", objfile);
      break;
    case FT_SIGNED_SHORT:
      type = init_type (TYPE_CODE_INT,
			TARGET_SHORT_BIT / TARGET_CHAR_BIT,
			0, "integer", objfile);		/* FIXME-fnf */
      break;
    case FT_UNSIGNED_SHORT:
      type = init_type (TYPE_CODE_INT,
			TARGET_SHORT_BIT / TARGET_CHAR_BIT,
			TYPE_FLAG_UNSIGNED, "word", objfile);
      break;
    case FT_INTEGER:
      type = init_type (TYPE_CODE_INT,
			TARGET_INT_BIT / TARGET_CHAR_BIT,
			0, "longint", objfile);
      break;
    case FT_SIGNED_INTEGER:
      type = init_type (TYPE_CODE_INT,
			TARGET_INT_BIT / TARGET_CHAR_BIT,
			0, "longint", objfile);		/* FIXME -fnf */
      break;
    case FT_UNSIGNED_INTEGER:
      type = init_type (TYPE_CODE_INT,
			TARGET_INT_BIT / TARGET_CHAR_BIT,
			TYPE_FLAG_UNSIGNED, "cardinal", objfile);
      break;
    case FT_LONG:
      type = init_type (TYPE_CODE_INT,
			TARGET_LONG_BIT / TARGET_CHAR_BIT,
			0, "long", objfile);
      break;
    case FT_SIGNED_LONG:
      type = init_type (TYPE_CODE_INT,
			TARGET_LONG_BIT / TARGET_CHAR_BIT,
			0, "long", objfile);	/* FIXME -fnf */
      break;
    case FT_UNSIGNED_LONG:
      type = init_type (TYPE_CODE_INT,
			TARGET_LONG_BIT / TARGET_CHAR_BIT,
			TYPE_FLAG_UNSIGNED, "unsigned long", objfile);
      break;
    case FT_LONG_LONG:
      type = init_type (TYPE_CODE_INT,
			TARGET_LONG_LONG_BIT / TARGET_CHAR_BIT,
			0, "long long", objfile);
      break;
    case FT_SIGNED_LONG_LONG:
      type = init_type (TYPE_CODE_INT,
			TARGET_LONG_LONG_BIT / TARGET_CHAR_BIT,
			0, "signed long long", objfile);
      break;
    case FT_UNSIGNED_LONG_LONG:
      type = init_type (TYPE_CODE_INT,
			TARGET_LONG_LONG_BIT / TARGET_CHAR_BIT,
			TYPE_FLAG_UNSIGNED, "unsigned long long", objfile);
      break;
    case FT_FLOAT:
      type = init_type (TYPE_CODE_FLT,
			TARGET_FLOAT_BIT / TARGET_CHAR_BIT,
			0, "float", objfile);
      break;
    case FT_DBL_PREC_FLOAT:
      type = init_type (TYPE_CODE_FLT,
			TARGET_DOUBLE_BIT / TARGET_CHAR_BIT,
			0, "double", objfile);
      break;
    case FT_EXT_PREC_FLOAT:
      type = init_type (TYPE_CODE_FLT,
			TARGET_LONG_DOUBLE_BIT / TARGET_CHAR_BIT,
			0, "extended", objfile);
      break;
    }
  return (type);
}


/* Table mapping opcodes into strings for printing operators
   and precedences of the operators.  */

const struct op_print pascal_op_print_tab[] =
{
  {",", BINOP_COMMA, PREC_COMMA, 0},
  {":=", BINOP_ASSIGN, PREC_ASSIGN, 1},
  {"or", BINOP_BITWISE_IOR, PREC_BITWISE_IOR, 0},
  {"xor", BINOP_BITWISE_XOR, PREC_BITWISE_XOR, 0},
  {"and", BINOP_BITWISE_AND, PREC_BITWISE_AND, 0},
  {"=", BINOP_EQUAL, PREC_EQUAL, 0},
  {"<>", BINOP_NOTEQUAL, PREC_EQUAL, 0},
  {"<=", BINOP_LEQ, PREC_ORDER, 0},
  {">=", BINOP_GEQ, PREC_ORDER, 0},
  {">", BINOP_GTR, PREC_ORDER, 0},
  {"<", BINOP_LESS, PREC_ORDER, 0},
  {"shr", BINOP_RSH, PREC_SHIFT, 0},
  {"shl", BINOP_LSH, PREC_SHIFT, 0},
  {"+", BINOP_ADD, PREC_ADD, 0},
  {"-", BINOP_SUB, PREC_ADD, 0},
  {"*", BINOP_MUL, PREC_MUL, 0},
  {"/", BINOP_DIV, PREC_MUL, 0},
  {"div", BINOP_INTDIV, PREC_MUL, 0},
  {"mod", BINOP_REM, PREC_MUL, 0},
  {"@", BINOP_REPEAT, PREC_REPEAT, 0},
  {"-", UNOP_NEG, PREC_PREFIX, 0},
  {"not", UNOP_LOGICAL_NOT, PREC_PREFIX, 0},
  {"^", UNOP_IND, PREC_SUFFIX, 1},
  {"@", UNOP_ADDR, PREC_PREFIX, 0},
  {"sizeof", UNOP_SIZEOF, PREC_PREFIX, 0},
  {NULL, 0, 0, 0}
};

struct type **const /* CONST_PTR v 4.17 */ (pascal_builtin_types[]) =
{
  &builtin_type_int,
    &builtin_type_long,
    &builtin_type_short,
    &builtin_type_char,
    &builtin_type_float,
    &builtin_type_double,
    &builtin_type_void,
    &builtin_type_long_long,
    &builtin_type_signed_char,
    &builtin_type_unsigned_char,
    &builtin_type_unsigned_short,
    &builtin_type_unsigned_int,
    &builtin_type_unsigned_long,
    &builtin_type_unsigned_long_long,
    &builtin_type_long_double,
    &builtin_type_complex,
    &builtin_type_double_complex,
    0
};

const struct language_defn pascal_language_defn =
{
  "pascal",			/* Language name */
  language_pascal,
  pascal_builtin_types,
  range_check_on,
  type_check_on,
  case_sensitive_on,
  pascal_parse,
  pascal_error,
  evaluate_subexp_standard,
  pascal_printchar,		/* Print a character constant */
  pascal_printstr,		/* Function to print string constant */
  pascal_emit_char,		/* Print a single char */
  pascal_create_fundamental_type,	/* Create fundamental type in this language */
  pascal_print_type,		/* Print a type using appropriate syntax */
  pascal_val_print,		/* Print a value using appropriate syntax */
  pascal_value_print,		/* Print a top-level value */
  {"", "%", "b", ""},		/* Binary format info */
  {"0%lo", "0", "o", ""},	/* Octal format info */
  {"%ld", "", "d", ""},		/* Decimal format info */
  {"$%lx", "$", "x", ""},	/* Hex format info */
  pascal_op_print_tab,		/* expression operators for printing */
  1,				/* c-style arrays */
  0,				/* String lower bound */
  &builtin_type_char,		/* Type of string elements */
  LANG_MAGIC
};

void
_initialize_pascal_language (void)
{
  add_language (&pascal_language_defn);
}
