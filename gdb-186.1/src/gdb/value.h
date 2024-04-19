/* Definitions for values of C expressions, for GDB.
   Copyright 1986, 1987, 1989, 1992-1996, 2000 Free Software Foundation, Inc.

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

#if !defined (VALUE_H)
#define VALUE_H 1

/*
 * The structure which defines the type of a value.  It should never
 * be possible for a program lval value to survive over a call to the inferior
 * (ie to be put into the history list or an internal variable).
 */

struct value
  {
    /* Type of value; either not an lval, or one of the various
       different possible kinds of lval.  */
    enum lval_type lval;
    /* Is it modifiable?  Only relevant if lval != not_lval.  */
    int modifiable;
    /* Location of value (if lval).  */
    union
      {
	/* If lval == lval_memory, this is the address in the inferior.
	   If lval == lval_register, this is the byte offset into the
	   registers structure.  */
	CORE_ADDR address;
	/* Pointer to internal variable.  */
	struct internalvar *internalvar;
	/* Number of register.  Only used with
	   lval_reg_frame_relative.  */
	int regnum;
      }
    location;
    /* Describes offset of a value within lval of a structure in bytes.
       If lval == lval_memory, this is an offset to the address.
       If lval == lval_register, this is a further offset from
          location.address within the registers structure.  
       Note also the member embedded_offset below.  */
    int offset;
    /* Only used for bitfields; number of bits contained in them.  */
    int bitsize;
    /* Only used for bitfields; position of start of field.
       For BITS_BIG_ENDIAN=0 targets, it is the position of the LSB.
       For BITS_BIG_ENDIAN=1 targets, it is the position of the MSB. */
    int bitpos;
    /* Frame value is relative to.  In practice, this address is only
       used if the value is stored in several registers in other than
       the current frame, and these registers have not all been saved
       at the same place in memory.  This will be described in the
       lval enum above as "lval_reg_frame_relative".  */
    CORE_ADDR frame_addr;
    /* Type of the value.  */
    struct type *type;
    /* Type of the enclosing object if this is an embedded subobject.
       The member embedded_offset gives the real position of the subobject
       if type is not the same as enclosing_type.

       If the type field is a pointer type, then enclosing_type is 
       a pointer type pointing to the real (enclosing) type of the target
       object. */
    struct type *enclosing_type;
    /* Values are stored in a chain, so that they can be deleted
       easily over calls to the inferior.  Values assigned to internal
       variables or put into the value history are taken off this
       list.  */
    struct value *next;

    /* ??? When is this used?  */
    union
      {
	CORE_ADDR memaddr;
	char *myaddr;
      }
    substring_addr;

    /* Register number if the value is from a register.  Is not kept
       if you take a field of a structure that is stored in a
       register.  Shouldn't it be?  */
    short regno;
    /* If zero, contents of this value are in the contents field.
       If nonzero, contents are in inferior memory at address
       in the location.address field plus the offset field
       (and the lval field should be lval_memory).

       WARNING: This field is used by the code which handles
       watchpoints (see breakpoint.c) to decide whether a particular
       value can be watched by hardware watchpoints.  If the lazy flag
       is set for some member of a value chain, it is assumed that
       this member of the chain doesn't need to be watched as part of
       watching the value itself.  This is how GDB avoids watching the
       entire struct or array when the user wants to watch a single
       struct member or array element.  If you ever change the way
       lazy flag is set and reset, be sure to consider this use as
       well!  */
    char lazy;
    /* If nonzero, this is the value of a variable which does not
       actually exist in the program.  */
    char optimized_out;
    /* If this value represents an object that is embedded inside a
       larger object (e.g., a base subobject in C++), this gives the
       offset (in bytes) from the start of the contents buffer where
       the embedded object begins. This is required because some C++
       runtime implementations lay out objects (especially virtual bases
       with possibly negative offsets to ancestors).
       Note: This may be positive or negative! Also note that this offset
       is not used when retrieving contents from target memory; the entire
       enclosing object has to be retrieved always, and the offset for
       that is given by the member offset above. */
    int embedded_offset;
    /* If this value represents a pointer to an object that is embedded
       in another object, this gives the embedded_offset of the object
       that is pointed to. */
    int pointed_to_offset;
    /* The BFD section associated with this value.  */
    asection *bfd_section;
    /* Actual contents of the value.  For use of this value; setting
       it uses the stuff above.  Not valid if lazy is nonzero.
       Target byte-order.  We force it to be aligned properly for any
       possible value.  Note that a value therefore extends beyond
       what is declared here.  */
    union
      {
	long contents[1];
	double force_double_align;
	LONGEST force_longlong_align;
	char *literal_data;
      }
    aligner;
    /* Do not add any new members here -- contents above will trash them */
  };

typedef struct value *value_ptr;

#define VALUE_TYPE(val) (val)->type
#define VALUE_ENCLOSING_TYPE(val) (val)->enclosing_type
#define VALUE_LAZY(val) (val)->lazy
/* VALUE_CONTENTS and VALUE_CONTENTS_RAW both return the address of
   the gdb buffer used to hold a copy of the contents of the lval.  
   VALUE_CONTENTS is used when the contents of the buffer are needed --
   it uses value_fetch_lazy() to load the buffer from the process being 
   debugged if it hasn't already been loaded.  VALUE_CONTENTS_RAW is 
   used when data is being stored into the buffer, or when it is 
   certain that the contents of the buffer are valid.
   Note: The contents pointer is adjusted by the offset required to
   get to the real subobject, if the value happens to represent
   something embedded in a larger run-time object. */

#define VALUE_CONTENTS_RAW(val) ((char *) (val)->aligner.contents + (val)->embedded_offset)
#define VALUE_CONTENTS(val) ((void)(VALUE_LAZY(val) && value_fetch_lazy(val)),\
			     VALUE_CONTENTS_RAW(val))

/* The ALL variants of the above two macros do not adjust the returned
   pointer by the embedded_offset value. */

#define VALUE_CONTENTS_ALL_RAW(val) ((char *) (val)->aligner.contents)
#define VALUE_CONTENTS_ALL(val) ((void) (VALUE_LAZY(val) && value_fetch_lazy(val)),\
                                 VALUE_CONTENTS_ALL_RAW(val))


extern int value_fetch_lazy (value_ptr val);

#define VALUE_LVAL(val) (val)->lval
#define VALUE_ADDRESS(val) (val)->location.address
#define VALUE_INTERNALVAR(val) (val)->location.internalvar
#define VALUE_FRAME_REGNUM(val) ((val)->location.regnum)
#define VALUE_FRAME(val) ((val)->frame_addr)
#define VALUE_OFFSET(val) (val)->offset
#define VALUE_BITSIZE(val) (val)->bitsize
#define VALUE_BITPOS(val) (val)->bitpos
#define VALUE_NEXT(val) (val)->next
#define VALUE_REGNO(val) (val)->regno
#define VALUE_OPTIMIZED_OUT(val) ((val)->optimized_out)
#define VALUE_EMBEDDED_OFFSET(val) ((val)->embedded_offset)
#define VALUE_POINTED_TO_OFFSET(val) ((val)->pointed_to_offset)
#define VALUE_BFD_SECTION(val) ((val)->bfd_section)

/* Convert a REF to the object referenced. */

#define COERCE_REF(arg)    \
do { struct type *value_type_arg_tmp = check_typedef (VALUE_TYPE (arg));\
     if (TYPE_CODE (value_type_arg_tmp) == TYPE_CODE_REF)		\
	 arg = value_at_lazy (TYPE_TARGET_TYPE (value_type_arg_tmp),	\
			      unpack_pointer (VALUE_TYPE (arg),		\
					      VALUE_CONTENTS (arg)),    \
			      VALUE_BFD_SECTION (arg));			\
    } while (0)

/* If ARG is an array, convert it to a pointer.
   If ARG is an enum, convert it to an integer.
   If ARG is a function, convert it to a function pointer.

   References are dereferenced.  */

#define COERCE_ARRAY(arg)    \
do { COERCE_REF(arg);							\
  if (current_language->c_style_arrays					\
      && TYPE_CODE (VALUE_TYPE (arg)) == TYPE_CODE_ARRAY)		\
    arg = value_coerce_array (arg);					\
  if (TYPE_CODE (VALUE_TYPE (arg)) == TYPE_CODE_FUNC)                   \
    arg = value_coerce_function (arg);                                  \
} while (0)

#define COERCE_NUMBER(arg)  \
  do { COERCE_ARRAY(arg);  COERCE_ENUM(arg); } while (0)

#define COERCE_VARYING_ARRAY(arg, real_arg_type)	\
{ if (chill_varying_type (real_arg_type))  \
    arg = varying_to_slice (arg), real_arg_type = VALUE_TYPE (arg); }

/* If ARG is an enum, convert it to an integer.  */

#define COERCE_ENUM(arg)   { \
  if (TYPE_CODE (check_typedef (VALUE_TYPE (arg))) == TYPE_CODE_ENUM)	\
    arg = value_cast (builtin_type_unsigned_int, arg);			\
}

/* Internal variables (variables for convenience of use of debugger)
   are recorded as a chain of these structures.  */

struct internalvar
  {
    struct internalvar *next;
    char *name;
    value_ptr value;
  };

/* Pointer to member function.  Depends on compiler implementation. */

#define METHOD_PTR_IS_VIRTUAL(ADDR)  ((ADDR) & 0x80000000)
#define METHOD_PTR_FROM_VOFFSET(OFFSET) (0x80000000 + (OFFSET))
#define METHOD_PTR_TO_VOFFSET(ADDR) (~0x80000000 & (ADDR))


#include "symtab.h"
#include "gdbtypes.h"
#include "expression.h"

struct frame_info;
struct fn_field;

extern void print_address_demangle (CORE_ADDR, struct ui_file *, int);

extern LONGEST value_as_long (value_ptr val);

extern DOUBLEST value_as_double (value_ptr val);

extern CORE_ADDR value_as_pointer (value_ptr val);

extern LONGEST unpack_long (struct type *type, char *valaddr);

extern DOUBLEST unpack_double (struct type *type, char *valaddr, int *invp);

extern CORE_ADDR unpack_pointer (struct type *type, char *valaddr);

extern LONGEST unpack_field_as_long (struct type *type, char *valaddr,
				     int fieldno);

extern value_ptr value_from_longest (struct type *type, LONGEST num);

extern value_ptr value_from_pointer (struct type *type, CORE_ADDR addr);

extern value_ptr value_from_double (struct type *type, DOUBLEST num);

extern value_ptr value_from_string (char *string);

extern value_ptr value_at (struct type *type, CORE_ADDR addr,
			   asection * sect);

extern value_ptr value_at_lazy (struct type *type, CORE_ADDR addr,
				asection * sect);

extern value_ptr value_from_register (struct type *type, int regnum,
				      struct frame_info *frame);

extern value_ptr value_of_variable (struct symbol *var, struct block *b);

extern value_ptr value_of_register (int regnum);

extern int symbol_read_needs_frame (struct symbol *);

extern value_ptr read_var_value (struct symbol *var,
				 struct frame_info *frame);

extern value_ptr locate_var_value (struct symbol *var,
				   struct frame_info *frame);

extern value_ptr allocate_value (struct type *type);

extern value_ptr allocate_repeat_value (struct type *type, int count);

extern value_ptr value_change_enclosing_type (value_ptr val, struct type *new_type);

extern value_ptr value_mark (void);

extern void value_free_to_mark (value_ptr mark);

extern value_ptr value_string (char *ptr, int len);
extern value_ptr value_bitstring (char *ptr, int len);

extern value_ptr value_array (int lowbound, int highbound,
			      value_ptr * elemvec);

extern value_ptr value_concat (value_ptr arg1, value_ptr arg2);

extern value_ptr value_binop (value_ptr arg1, value_ptr arg2,
			      enum exp_opcode op);

extern value_ptr value_add (value_ptr arg1, value_ptr arg2);

extern value_ptr value_sub (value_ptr arg1, value_ptr arg2);

extern value_ptr value_coerce_array (value_ptr arg1);

extern value_ptr value_coerce_function (value_ptr arg1);

extern value_ptr value_ind (value_ptr arg1);

extern value_ptr value_addr (value_ptr arg1);

extern value_ptr value_assign (value_ptr toval, value_ptr fromval);

extern value_ptr value_neg (value_ptr arg1);

extern value_ptr value_complement (value_ptr arg1);

extern value_ptr value_struct_elt (value_ptr * argp, value_ptr * args,
				   char *name,
				   int *static_memfuncp, char *err);

extern value_ptr value_struct_elt_for_reference (struct type *domain,
						 int offset,
						 struct type *curtype,
						 char *name,
						 struct type *intype);

extern value_ptr value_static_field (struct type *type, int fieldno);

extern struct fn_field *value_find_oload_method_list (value_ptr *, char *,
						      int, int *, int *,
						      struct type **, int *);

extern int find_overload_match (struct type **arg_types, int nargs,
				char *name, int method, int lax,
				value_ptr obj, struct symbol *fsym,
				value_ptr * valp, struct symbol **symp,
				int *staticp);

extern value_ptr value_field (value_ptr arg1, int fieldno);

extern value_ptr value_primitive_field (value_ptr arg1, int offset,
					int fieldno, struct type *arg_type);

extern struct type *value_rtti_type (value_ptr, int *, int *, int *);

extern struct type *value_rtti_target_type (value_ptr, int *, int *, int *);

extern value_ptr value_full_object (value_ptr, struct type *, int, int, int);

extern value_ptr value_cast (struct type *type, value_ptr arg2);

extern value_ptr value_zero (struct type *type, enum lval_type lv);

extern value_ptr value_repeat (value_ptr arg1, int count);

extern value_ptr value_subscript (value_ptr array, value_ptr idx);

extern value_ptr value_from_vtable_info (value_ptr arg, struct type *type);

extern value_ptr value_being_returned (struct type *valtype,
				       char *retbuf, int struct_return);

extern value_ptr value_in (value_ptr element, value_ptr set);

extern int value_bit_index (struct type *type, char *addr, int index);

extern int using_struct_return (value_ptr function, CORE_ADDR funcaddr,
				struct type *value_type, int gcc_p);

extern void set_return_value (value_ptr val);

extern value_ptr evaluate_expression (struct expression *exp);

extern value_ptr evaluate_type (struct expression *exp);

extern value_ptr evaluate_subexp_with_coercion (struct expression *,
						int *, enum noside);

extern value_ptr parse_and_eval (char *exp);

extern value_ptr parse_to_comma_and_eval (char **expp);

extern struct type *parse_and_eval_type (char *p, int length);

extern CORE_ADDR parse_and_eval_address (char *exp);

extern CORE_ADDR parse_and_eval_address_1 (char **expptr);

extern LONGEST parse_and_eval_long (char *exp);

extern value_ptr access_value_history (int num);

extern value_ptr value_of_internalvar (struct internalvar *var);

extern void set_internalvar (struct internalvar *var, value_ptr val);

extern void set_internalvar_component (struct internalvar *var,
				       int offset,
				       int bitpos, int bitsize,
				       value_ptr newvalue);

extern struct internalvar *lookup_internalvar (char *name);

extern int value_equal (value_ptr arg1, value_ptr arg2);

extern int value_less (value_ptr arg1, value_ptr arg2);

extern int value_logical_not (value_ptr arg1);

extern value_ptr value_of_local (char *name, int complain);

/* C++ */

extern value_ptr value_of_this (int complain);

extern value_ptr value_x_binop (value_ptr arg1, value_ptr arg2,
				enum exp_opcode op,
				enum exp_opcode otherop, enum noside noside);

extern value_ptr value_x_unop (value_ptr arg1, enum exp_opcode op,
			       enum noside noside);

extern value_ptr value_fn_field (value_ptr * arg1p, struct fn_field *f,
				 int j, struct type *type, int offset);

extern value_ptr value_virtual_fn_field (value_ptr * arg1p,
					 struct fn_field *f, int j,
					 struct type *type, int offset);

extern int binop_user_defined_p (enum exp_opcode op,
				 value_ptr arg1, value_ptr arg2);

extern int unop_user_defined_p (enum exp_opcode op, value_ptr arg1);

extern int destructor_name_p (const char *name, const struct type *type);

#define value_free(val) free ((PTR)val)

extern void free_all_values (void);

extern void release_value (value_ptr val);

extern int record_latest_value (value_ptr val);

extern void registers_changed (void);

extern void read_register_bytes (int regbyte, char *myaddr, int len);

extern void write_register_bytes (int regbyte, char *myaddr, int len);

extern void read_register_gen (int regno, char *myaddr);

extern void write_register_gen (int regno, char *myaddr);

/* Rename to read_unsigned_register()? */
extern ULONGEST read_register (int regno);

extern LONGEST read_signed_register (int regno);

/* Rename to read_unsigned_register_pid()? */
extern ULONGEST read_register_pid (int regno, int pid);

extern LONGEST read_signed_register_pid (int regno, int pid);

extern void write_register (int regno, LONGEST val);

extern void write_register_pid (int regno, CORE_ADDR val, int pid);

extern void supply_register (int regno, char *val);

extern int register_cached (int regno);

extern void register_changed (int regnum);

extern void get_saved_register (char *raw_buffer, int *optimized,
				CORE_ADDR * addrp,
				struct frame_info *frame,
				int regnum, enum lval_type *lval);

extern void
modify_field (char *addr, LONGEST fieldval, int bitpos, int bitsize);

extern void type_print (struct type * type, char *varstring,
			struct ui_file * stream, int show);

extern char *baseclass_addr (struct type *type, int index,
			     char *valaddr, value_ptr * valuep, int *errp);

extern void print_longest (struct ui_file * stream, int format,
			   int use_local, LONGEST val);

extern void print_floating (char *valaddr, struct type * type,
			    struct ui_file * stream);

extern int value_print (value_ptr val, struct ui_file *stream, int format,
			enum val_prettyprint pretty);

extern void value_print_array_elements (value_ptr val,
					struct ui_file *stream,
					int format,
					enum val_prettyprint pretty);

extern value_ptr value_release_to_mark (value_ptr mark);

extern int val_print (struct type * type, char *valaddr,
		      int embedded_offset, CORE_ADDR address,
		      struct ui_file * stream, int format,
		      int deref_ref, int recurse,
		      enum val_prettyprint pretty);

extern int val_print_string (CORE_ADDR addr, int len, int width, struct ui_file *stream);

extern void print_variable_value (struct symbol * var,
				  struct frame_info * frame,
				  struct ui_file *stream);
extern void
print_variable_value (struct symbol *var, struct frame_info *frame,
		      struct ui_file *stream);

extern int check_field (value_ptr, const char *);

extern void typedef_print (struct type * type, struct symbol * news,
			     struct ui_file * stream);

extern char *internalvar_name (struct internalvar *var);

extern void clear_value_history (void);

extern void clear_internalvars (void);

/* From values.c */

extern value_ptr value_copy (value_ptr);

extern int baseclass_offset (struct type *, int, char *, CORE_ADDR);

/* From valops.c */

extern value_ptr varying_to_slice (value_ptr);

extern value_ptr value_slice (value_ptr, int, int);

extern value_ptr call_function_by_hand (value_ptr, int, value_ptr *);

extern value_ptr call_function_by_hand_expecting_type (value_ptr,
						       struct type *, int,
						       value_ptr *, int);

extern int default_coerce_float_to_double (struct type *, struct type *);

extern int standard_coerce_float_to_double (struct type *, struct type *);

extern value_ptr value_literal_complex (value_ptr, value_ptr, struct type *);

extern void find_rt_vbase_offset (struct type *, struct type *, char *, int,
				  int *, int *);

struct cached_value
{
  char *name;
  struct type *type;
  struct value val;
  unsigned int generation;
};
typedef struct cached_value *cached_value_ptr;

extern cached_value_ptr create_cached_function (char *, struct type *);

extern value_ptr lookup_cached_function (cached_value_ptr cval);

extern value_ptr find_function_in_inferior (char *, struct type *);

extern value_ptr value_allocate_space_in_inferior (int);

extern CORE_ADDR default_push_arguments (int nargs, value_ptr * args,
					 CORE_ADDR sp,
					 int struct_return,
					 CORE_ADDR struct_addr);

extern CORE_ADDR find_function_addr (value_ptr, struct type **);

#endif /* !defined (VALUE_H) */
