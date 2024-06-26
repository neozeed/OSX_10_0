/* OBSOLETE /* Parameters for targeting on a Gould NP1, for GDB, the GNU debugger. */
/* OBSOLETE    Copyright 1986, 1987, 1989, 1991, 1993 Free Software Foundation, Inc. */
/* OBSOLETE  */
/* OBSOLETE This file is part of GDB. */
/* OBSOLETE  */
/* OBSOLETE This program is free software; you can redistribute it and/or modify */
/* OBSOLETE it under the terms of the GNU General Public License as published by */
/* OBSOLETE the Free Software Foundation; either version 2 of the License, or */
/* OBSOLETE (at your option) any later version. */
/* OBSOLETE  */
/* OBSOLETE This program is distributed in the hope that it will be useful, */
/* OBSOLETE but WITHOUT ANY WARRANTY; without even the implied warranty of */
/* OBSOLETE MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the */
/* OBSOLETE GNU General Public License for more details. */
/* OBSOLETE  */
/* OBSOLETE You should have received a copy of the GNU General Public License */
/* OBSOLETE along with this program; if not, write to the Free Software */
/* OBSOLETE Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.  *x/ */
/* OBSOLETE  */
/* OBSOLETE #define GOULD_NPL */
/* OBSOLETE  */
/* OBSOLETE #define TARGET_BYTE_ORDER BIG_ENDIAN */
/* OBSOLETE  */
/* OBSOLETE /* N_ENTRY appears in libraries on Gould machines. */
/* OBSOLETE    Don't know what 0xa4 is; it's mentioned in stab.h */
/* OBSOLETE    but only in the sdb symbol list.  *x/ */
/* OBSOLETE #define IGNORE_SYMBOL(type) (type == N_ENTRY || type == 0xa4) */
/* OBSOLETE  */
/* OBSOLETE /* We don't want the extra gnu symbols on the machine; */
/* OBSOLETE    they will interfere with the shared segment symbols.  *x/ */
/* OBSOLETE #define NO_GNU_STABS */
/* OBSOLETE  */
/* OBSOLETE /* Macro for text-offset and data info (in NPL a.out format).  *x/ */
/* OBSOLETE #define     TEXTINFO                                                \ */
/* OBSOLETE         text_offset = N_TXTOFF (exec_coffhdr, exec_aouthdr);        \ */
/* OBSOLETE         exec_data_offset = N_TXTOFF (exec_coffhdr, exec_aouthdr)\ */
/* OBSOLETE                 + exec_aouthdr.a_text */
/* OBSOLETE  */
/* OBSOLETE /* Macro for number of symbol table entries *x/ */
/* OBSOLETE #define NUMBER_OF_SYMBOLS                                   \ */
/* OBSOLETE     (coffhdr.f_nsyms) */
/* OBSOLETE  */
/* OBSOLETE /* Macro for file-offset of symbol table (in NPL a.out format).  *x/ */
/* OBSOLETE #define SYMBOL_TABLE_OFFSET                                 \ */
/* OBSOLETE     N_SYMOFF (coffhdr) */
/* OBSOLETE  */
/* OBSOLETE /* Macro for file-offset of string table (in NPL a.out format).  *x/ */
/* OBSOLETE #define STRING_TABLE_OFFSET                                 \ */
/* OBSOLETE     (N_STROFF (coffhdr)) */
/* OBSOLETE  */
/* OBSOLETE /* Macro to store the length of the string table data in INTO.  *x/ */
/* OBSOLETE #define READ_STRING_TABLE_SIZE(INTO)                                \ */
/* OBSOLETE     { INTO = hdr.a_stsize; } */
/* OBSOLETE  */
/* OBSOLETE /* Macro to declare variables to hold the file's header data.  *x/ */
/* OBSOLETE #define DECLARE_FILE_HEADERS  struct exec hdr;                      \ */
/* OBSOLETE                           FILHDR coffhdr */
/* OBSOLETE  */
/* OBSOLETE /* Macro to read the header data from descriptor DESC and validate it. */
/* OBSOLETE    NAME is the file name, for error messages.  *x/ */
/* OBSOLETE #define READ_FILE_HEADERS(DESC, NAME)                               \ */
/* OBSOLETE { val = myread (DESC, &coffhdr, sizeof coffhdr);            \ */
/* OBSOLETE   if (val < 0)                                                      \ */
/* OBSOLETE     perror_with_name (NAME);                                        \ */
/* OBSOLETE   val = myread (DESC, &hdr, sizeof hdr);                    \ */
/* OBSOLETE   if (val < 0)                                                      \ */
/* OBSOLETE     perror_with_name (NAME);                                        \ */
/* OBSOLETE   if (coffhdr.f_magic != GNP1MAGIC)                         \ */
/* OBSOLETE     error ("File \"%s\" not in coff executable format.", NAME);     \ */
/* OBSOLETE   if (N_BADMAG (hdr))                                               \ */
/* OBSOLETE     error ("File \"%s\" not in executable format.", NAME); } */
/* OBSOLETE  */
/* OBSOLETE /* Define COFF and other symbolic names needed on NP1 *x/ */
/* OBSOLETE #define     NS32GMAGIC      GNP1MAGIC */
/* OBSOLETE #define     NS32SMAGIC      GPNMAGIC */
/* OBSOLETE  */
/* OBSOLETE /* Address of blocks in N_LBRAC and N_RBRAC symbols are absolute addresses, */
/* OBSOLETE    not relative to start of source address.  *x/ */
/* OBSOLETE #define BLOCK_ADDRESS_ABSOLUTE */
/* OBSOLETE  */
/* OBSOLETE /* Offset from address of function to start of its code. */
/* OBSOLETE    Zero on most machines.  *x/ */
/* OBSOLETE #define FUNCTION_START_OFFSET       8 */
/* OBSOLETE  */
/* OBSOLETE /* Advance PC across any function entry prologue instructions */
/* OBSOLETE    to reach some "real" code.  One NPL we can have one two startup */
/* OBSOLETE    sequences depending on the size of the local stack: */
/* OBSOLETE  */
/* OBSOLETE    Either: */
/* OBSOLETE       "suabr b2, #" */
/* OBSOLETE    of */
/* OBSOLETE       "lil r4, #", "suabr b2, #(r4)" */
/* OBSOLETE  */
/* OBSOLETE    "lwbr b6, #", "stw r1, 8(b2)" */
/* OBSOLETE    Optional "stwbr b3, c(b2)" */
/* OBSOLETE    Optional "trr r2,r7"      (Gould first argument register passing) */
/* OBSOLETE      or */
/* OBSOLETE    Optional "stw r2,8(b3)"   (Gould first argument register passing) */
/* OBSOLETE  *x/ */
/* OBSOLETE #define SKIP_PROLOGUE(pc) {                                                 \ */
/* OBSOLETE     register int op = read_memory_integer ((pc), 4);                \ */
/* OBSOLETE     if ((op & 0xffff0000) == 0xFA0B0000) {                          \ */
/* OBSOLETE         pc += 4;                                                    \ */
/* OBSOLETE         op = read_memory_integer ((pc), 4);                         \ */
/* OBSOLETE         if ((op & 0xffff0000) == 0x59400000) {                      \ */
/* OBSOLETE             pc += 4;                                                \ */
/* OBSOLETE             op = read_memory_integer ((pc), 4);                     \ */
/* OBSOLETE             if ((op & 0xffff0000) == 0x5F000000) {                  \ */
/* OBSOLETE                 pc += 4;                                            \ */
/* OBSOLETE                 op = read_memory_integer ((pc), 4);                 \ */
/* OBSOLETE                 if (op == 0xD4820008) {                             \ */
/* OBSOLETE                     pc += 4;                                        \ */
/* OBSOLETE                     op = read_memory_integer ((pc), 4);             \ */
/* OBSOLETE                     if (op == 0x5582000C) {                         \ */
/* OBSOLETE                         pc += 4;                                    \ */
/* OBSOLETE                         op = read_memory_integer ((pc), 2);         \ */
/* OBSOLETE                         if (op == 0x2fa0) {                         \ */
/* OBSOLETE                             pc += 2;                                \ */
/* OBSOLETE                         } else {                                    \ */
/* OBSOLETE                             op = read_memory_integer ((pc), 4);     \ */
/* OBSOLETE                             if (op == 0xd5030008) {                 \ */
/* OBSOLETE                                 pc += 4;                            \ */
/* OBSOLETE                             }                                       \ */
/* OBSOLETE                         }                                           \ */
/* OBSOLETE                     } else {                                        \ */
/* OBSOLETE                         op = read_memory_integer ((pc), 2);         \ */
/* OBSOLETE                         if (op == 0x2fa0) {                         \ */
/* OBSOLETE                             pc += 2;                                \ */
/* OBSOLETE                         }                                           \ */
/* OBSOLETE                     }                                               \ */
/* OBSOLETE                 }                                                   \ */
/* OBSOLETE             }                                                       \ */
/* OBSOLETE         }                                                           \ */
/* OBSOLETE     }                                                               \ */
/* OBSOLETE     if ((op & 0xffff0000) == 0x59000000) {                          \ */
/* OBSOLETE         pc += 4;                                                    \ */
/* OBSOLETE         op = read_memory_integer ((pc), 4);                         \ */
/* OBSOLETE         if ((op & 0xffff0000) == 0x5F000000) {                      \ */
/* OBSOLETE             pc += 4;                                                \ */
/* OBSOLETE             op = read_memory_integer ((pc), 4);                     \ */
/* OBSOLETE             if (op == 0xD4820008) {                                 \ */
/* OBSOLETE                 pc += 4;                                            \ */
/* OBSOLETE                 op = read_memory_integer ((pc), 4);                 \ */
/* OBSOLETE                 if (op == 0x5582000C) {                             \ */
/* OBSOLETE                     pc += 4;                                        \ */
/* OBSOLETE                     op = read_memory_integer ((pc), 2);             \ */
/* OBSOLETE                     if (op == 0x2fa0) {                             \ */
/* OBSOLETE                         pc += 2;                                    \ */
/* OBSOLETE                     } else {                                        \ */
/* OBSOLETE                         op = read_memory_integer ((pc), 4);         \ */
/* OBSOLETE                         if (op == 0xd5030008) {                     \ */
/* OBSOLETE                             pc += 4;                                \ */
/* OBSOLETE                         }                                           \ */
/* OBSOLETE                     }                                               \ */
/* OBSOLETE                 } else {                                            \ */
/* OBSOLETE                     op = read_memory_integer ((pc), 2);             \ */
/* OBSOLETE                     if (op == 0x2fa0) {                             \ */
/* OBSOLETE                         pc += 2;                                    \ */
/* OBSOLETE                     }                                               \ */
/* OBSOLETE                 }                                                   \ */
/* OBSOLETE             }                                                       \ */
/* OBSOLETE         }                                                           \ */
/* OBSOLETE     }                                                               \ */
/* OBSOLETE } */
/* OBSOLETE  */
/* OBSOLETE /* Immediately after a function call, return the saved pc. */
/* OBSOLETE    Can't go through the frames for this because on some machines */
/* OBSOLETE    the new frame is not set up until the new function executes */
/* OBSOLETE    some instructions.  True on NPL! Return address is in R1. */
/* OBSOLETE    The true return address is REALLY 4 past that location! *x/ */
/* OBSOLETE #define SAVED_PC_AFTER_CALL(frame) \ */
/* OBSOLETE     (read_register(R1_REGNUM) + 4) */
/* OBSOLETE  */
/* OBSOLETE /* Address of end of stack space.  *x/ */
/* OBSOLETE #define STACK_END_ADDR              0x7fffc000 */
/* OBSOLETE  */
/* OBSOLETE /* Stack grows downward.  *x/ */
/* OBSOLETE #define INNER_THAN(lhs,rhs) ((lhs) < (rhs)) */
/* OBSOLETE  */
/* OBSOLETE /* Sequence of bytes for breakpoint instruction. */
/* OBSOLETE    This is padded out to the size of a machine word.  When it was just */
/* OBSOLETE    {0x28, 0x09} it gave problems if hit breakpoint on returning from a */
/* OBSOLETE    function call.  *x/ */
/* OBSOLETE #define BREAKPOINT          {0x28, 0x09, 0x0, 0x0} */
/* OBSOLETE  */
/* OBSOLETE /* Amount PC must be decremented by after a breakpoint. */
/* OBSOLETE    This is often the number of bytes in BREAKPOINT */
/* OBSOLETE    but not always.  *x/ */
/* OBSOLETE #define DECR_PC_AFTER_BREAK 2 */
/* OBSOLETE  */
/* OBSOLETE /* Return 1 if P points to an invalid floating point value.  *x/ */
/* OBSOLETE #define INVALID_FLOAT(p, len)       ((*(short *)p & 0xff80) == 0x8000) */
/* OBSOLETE  */
/* OBSOLETE /* Say how long (ordinary) registers are.  This is a piece of bogosity */
/* OBSOLETE    used in push_word and a few other places; REGISTER_RAW_SIZE is the */
/* OBSOLETE    real way to know how big a register is.  *x/ */
/* OBSOLETE  */
/* OBSOLETE #define REGISTER_SIZE 4 */
/* OBSOLETE  */
/* OBSOLETE /* Size of bytes of vector register (NP1 only), 32 elements * sizeof(int) *x/ */
/* OBSOLETE #define VR_SIZE                     128 */
/* OBSOLETE  */
/* OBSOLETE /* Number of machine registers *x/ */
/* OBSOLETE #define NUM_REGS            27 */
/* OBSOLETE #define NUM_GEN_REGS                16 */
/* OBSOLETE #define NUM_CPU_REGS                4 */
/* OBSOLETE #define NUM_VECTOR_REGS             7 */
/* OBSOLETE  */
/* OBSOLETE /* Initializer for an array of names of registers. */
/* OBSOLETE    There should be NUM_REGS strings in this initializer.  *x/ */
/* OBSOLETE #define REGISTER_NAMES { \ */
/* OBSOLETE   "r0", "r1", "r2", "r3", "r4", "r5", "r6", "r7", \ */
/* OBSOLETE   "b0", "b1", "b2", "b3", "b4", "b5", "b6", "b7", \ */
/* OBSOLETE   "sp", "ps", "pc", "ve", \ */
/* OBSOLETE   "v1", "v2", "v3", "v4", "v5", "v6", "v7", \ */
/* OBSOLETE } */
/* OBSOLETE  */
/* OBSOLETE /* Register numbers of various important registers. */
/* OBSOLETE    Note that some of these values are "real" register numbers, */
/* OBSOLETE    and correspond to the general registers of the machine, */
/* OBSOLETE    and some are "phony" register numbers which are too large */
/* OBSOLETE    to be actual register numbers as far as the user is concerned */
/* OBSOLETE    but do serve to get the desired values when passed to read_register.  *x/ */
/* OBSOLETE #define R1_REGNUM   1       /* Gr1 => return address of caller *x/ */
/* OBSOLETE #define R2_REGNUM   2       /* Gr2 => return value from function *x/ */
/* OBSOLETE #define R4_REGNUM   4       /* Gr4 => register save area *x/ */
/* OBSOLETE #define R5_REGNUM   5       /* Gr5 => register save area *x/ */
/* OBSOLETE #define R6_REGNUM   6       /* Gr6 => register save area *x/ */
/* OBSOLETE #define R7_REGNUM   7       /* Gr7 => register save area *x/ */
/* OBSOLETE #define B1_REGNUM   9       /* Br1 => start of this code routine *x/ */
/* OBSOLETE #define SP_REGNUM   10      /* Br2 == (sp) *x/ */
/* OBSOLETE #define AP_REGNUM   11      /* Br3 == (ap) *x/ */
/* OBSOLETE #define FP_REGNUM   16      /* A copy of Br2 saved in trap *x/ */
/* OBSOLETE #define PS_REGNUM   17      /* Contains processor status *x/ */
/* OBSOLETE #define PC_REGNUM   18      /* Contains program counter *x/ */
/* OBSOLETE #define VE_REGNUM   19      /* Vector end (user setup) register *x/ */
/* OBSOLETE #define V1_REGNUM   20      /* First vector register *x/ */
/* OBSOLETE #define V7_REGNUM   26      /* First vector register *x/ */
/* OBSOLETE  */
/* OBSOLETE /* Total amount of space needed to store our copies of the machine's */
/* OBSOLETE    register state, the array `registers'.  *x/ */
/* OBSOLETE #define REGISTER_BYTES \ */
/* OBSOLETE     (NUM_GEN_REGS*4 + NUM_VECTOR_REGS*VR_SIZE + NUM_CPU_REGS*4) */
/* OBSOLETE  */
/* OBSOLETE /* Index within `registers' of the first byte of the space for */
/* OBSOLETE    register N.  *x/ */
/* OBSOLETE #define REGISTER_BYTE(N)  \ */
/* OBSOLETE     (((N) < V1_REGNUM) ? ((N) * 4) : (((N) - V1_REGNUM) * VR_SIZE) + 80) */
/* OBSOLETE  */
/* OBSOLETE /* Number of bytes of storage in the actual machine representation */
/* OBSOLETE    for register N.  On the NP1, all normal regs are 4 bytes, but */
/* OBSOLETE    the vector registers are VR_SIZE*4 bytes long. *x/ */
/* OBSOLETE #define REGISTER_RAW_SIZE(N) \ */
/* OBSOLETE     (((N) < V1_REGNUM) ? 4 : VR_SIZE) */
/* OBSOLETE  */
/* OBSOLETE /* Number of bytes of storage in the program's representation */
/* OBSOLETE    for register N.  On the NP1, all regs are 4 bytes. *x/ */
/* OBSOLETE #define REGISTER_VIRTUAL_SIZE(N) \ */
/* OBSOLETE     (((N) < V1_REGNUM) ? 4 : VR_SIZE) */
/* OBSOLETE  */
/* OBSOLETE /* Largest value REGISTER_RAW_SIZE can have.  *x/ */
/* OBSOLETE #define MAX_REGISTER_RAW_SIZE               VR_SIZE */
/* OBSOLETE  */
/* OBSOLETE /* Largest value REGISTER_VIRTUAL_SIZE can have.  *x/ */
/* OBSOLETE #define MAX_REGISTER_VIRTUAL_SIZE   VR_SIZE */
/* OBSOLETE  */
/* OBSOLETE /* Return the GDB type object for the "standard" data type */
/* OBSOLETE    of data in register N.  *x/ */
/* OBSOLETE #define REGISTER_VIRTUAL_TYPE(N)    \ */
/* OBSOLETE   ((N) > VE_REGNUM ? builtin_type_np1_vector : builtin_type_int) */
/* OBSOLETE extern struct type *builtin_type_np1_vector; */
/* OBSOLETE  */
/* OBSOLETE /* Store the address of the place in which to copy the structure the */
/* OBSOLETE    subroutine will return.  This is called from call_function. */
/* OBSOLETE  */
/* OBSOLETE    On this machine this is a no-op, because gcc isn't used on it */
/* OBSOLETE    yet.  So this calling convention is not used. *x/ */
/* OBSOLETE  */
/* OBSOLETE #define STORE_STRUCT_RETURN(ADDR, SP) push_word(SP + 8, ADDR) */
/* OBSOLETE  */
/* OBSOLETE /* Extract from an arrary REGBUF containing the (raw) register state */
/* OBSOLETE    a function return value of type TYPE, and copy that, in virtual format, */
/* OBSOLETE    into VALBUF. *x/ */
/* OBSOLETE  */
/* OBSOLETE #define EXTRACT_RETURN_VALUE(TYPE,REGBUF,VALBUF) \ */
/* OBSOLETE     memcpy (VALBUF, ((int *)(REGBUF)) + 2, TYPE_LENGTH (TYPE)) */
/* OBSOLETE  */
/* OBSOLETE /* Write into appropriate registers a function return value */
/* OBSOLETE    of type TYPE, given in virtual format.  *x/ */
/* OBSOLETE  */
/* OBSOLETE #define STORE_RETURN_VALUE(TYPE,VALBUF) \ */
/* OBSOLETE     write_register_bytes (REGISTER_BYTE (R2_REGNUM), VALBUF,      \ */
/* OBSOLETE                           TYPE_LENGTH (TYPE)) */
/* OBSOLETE  */
/* OBSOLETE /* Extract from an array REGBUF containing the (raw) register state */
/* OBSOLETE    the address in which a function should return its structure value, */
/* OBSOLETE    as a CORE_ADDR (or an expression that can be used as one).  *x/ */
/* OBSOLETE  */
/* OBSOLETE #define EXTRACT_STRUCT_VALUE_ADDRESS(REGBUF) (*((int *)(REGBUF) + 2)) */
/* OBSOLETE  */
/* OBSOLETE /* Both gcc and cc return small structs in registers (i.e. in GDB */
/* OBSOLETE    terminology, small structs don't use the struct return convention).  *x/ */
/* OBSOLETE extern use_struct_convention_fn gould_use_struct_convention; */
/* OBSOLETE #define USE_STRUCT_CONVENTION(gcc_p, type) gould_use_struct_convention (gcc_p, type) */
/* OBSOLETE  */
/* OBSOLETE /* Describe the pointer in each stack frame to the previous stack frame */
/* OBSOLETE    (its caller).  *x/ */
/* OBSOLETE  */
/* OBSOLETE /* FRAME_CHAIN takes a frame's nominal address */
/* OBSOLETE    and produces the frame's chain-pointer. */
/* OBSOLETE  */
/* OBSOLETE    However, if FRAME_CHAIN_VALID returns zero, */
/* OBSOLETE    it means the given frame is the outermost one and has no caller.  *x/ */
/* OBSOLETE  */
/* OBSOLETE /* In the case of the NPL, the frame's norminal address is Br2 and the  */
/* OBSOLETE    previous routines frame is up the stack X bytes, where X is the */
/* OBSOLETE    value stored in the code function header xA(Br1). *x/ */
/* OBSOLETE #define FRAME_CHAIN(thisframe)              (findframe(thisframe)) */
/* OBSOLETE  */
/* OBSOLETE extern int gould_frame_chain_valid  (CORE_ADDR, struct frame_info *);  */
/* OBSOLETE #define FRAME_CHAIN_VALID(chain, thisframe) gould_frame_chain_valid (chain, thisframe) */
/* OBSOLETE  */
/* OBSOLETE /* Define other aspects of the stack frame on NPL.  *x/ */
/* OBSOLETE #define FRAME_SAVED_PC(FRAME) \ */
/* OBSOLETE     (read_memory_integer ((FRAME)->frame + 8, 4)) */
/* OBSOLETE  */
/* OBSOLETE #define FRAME_ARGS_ADDRESS(fi) \ */
/* OBSOLETE     ((fi)->next ? \ */
/* OBSOLETE      read_memory_integer ((fi)->frame + 12, 4) : \ */
/* OBSOLETE      read_register (AP_REGNUM)) */
/* OBSOLETE  */
/* OBSOLETE #define FRAME_LOCALS_ADDRESS(fi)    ((fi)->frame) */
/* OBSOLETE  */
/* OBSOLETE /* Set VAL to the number of args passed to frame described by FI. */
/* OBSOLETE    Can set VAL to -1, meaning no way to tell.  *x/ */
/* OBSOLETE  */
/* OBSOLETE /* We can check the stab info to see how */
/* OBSOLETE    many arg we have.  No info in stack will tell us *x/ */
/* OBSOLETE #define FRAME_NUM_ARGS(fi)          (findarg(fi)) */
/* OBSOLETE  */
/* OBSOLETE /* Return number of bytes at start of arglist that are not really args.  *x/ */
/* OBSOLETE #define FRAME_ARGS_SKIP                     8 */
/* OBSOLETE  */
/* OBSOLETE /* Put here the code to store, into a struct frame_saved_regs, */
/* OBSOLETE    the addresses of the saved registers of frame described by FRAME_INFO. */
/* OBSOLETE    This includes special registers such as pc and fp saved in special */
/* OBSOLETE    ways in the stack frame.  sp is even more special: */
/* OBSOLETE    the address we return for it IS the sp for the next frame.  *x/ */
/* OBSOLETE  */
/* OBSOLETE #define FRAME_FIND_SAVED_REGS(frame_info, frame_saved_regs)         \ */
/* OBSOLETE {                                                                       \ */
/* OBSOLETE   memset (&frame_saved_regs, '\0', sizeof frame_saved_regs);                        \ */
/* OBSOLETE   (frame_saved_regs).regs[SP_REGNUM] = framechain (frame_info);         \ */
/* OBSOLETE   (frame_saved_regs).regs[PC_REGNUM] = (frame_info)->frame + 8;             \ */
/* OBSOLETE   (frame_saved_regs).regs[R4_REGNUM] = (frame_info)->frame + 0x30;  \ */
/* OBSOLETE   (frame_saved_regs).regs[R5_REGNUM] = (frame_info)->frame + 0x34;  \ */
/* OBSOLETE   (frame_saved_regs).regs[R6_REGNUM] = (frame_info)->frame + 0x38;  \ */
/* OBSOLETE   (frame_saved_regs).regs[R7_REGNUM] = (frame_info)->frame + 0x3C;  \ */
/* OBSOLETE } */
/* OBSOLETE  */
/* OBSOLETE /* Things needed for making the inferior call functions.  *x/ */
/* OBSOLETE  */
/* OBSOLETE #define CALL_DUMMY_LOCATION BEFORE_TEXT_END */
/* OBSOLETE #define     NEED_TEXT_START_END 1 */
/* OBSOLETE  */
/* OBSOLETE /* Push an empty stack frame, to record the current PC, etc.  *x/ */
/* OBSOLETE  */
/* OBSOLETE #define PUSH_DUMMY_FRAME \ */
/* OBSOLETE { register CORE_ADDR sp = read_register (SP_REGNUM); \ */
/* OBSOLETE   register int regnum;                               \ */
/* OBSOLETE   for (regnum = 0; regnum < FP_REGNUM; regnum++)     \ */
/* OBSOLETE     sp = push_word (sp, read_register (regnum));     \ */
/* OBSOLETE   sp = push_word (sp, read_register (PS_REGNUM));    \ */
/* OBSOLETE   sp = push_word (sp, read_register (PC_REGNUM));    \ */
/* OBSOLETE   write_register (SP_REGNUM, sp);} */
/* OBSOLETE  */
/* OBSOLETE /* Discard from the stack the innermost frame,  */
/* OBSOLETE    restoring all saved registers.  *x/ */
/* OBSOLETE /* FIXME: Should be using {store,extract}_unsigned_integer.  *x/ */
/* OBSOLETE  */
/* OBSOLETE #define POP_FRAME  \ */
/* OBSOLETE { CORE_ADDR sp = read_register(SP_REGNUM);             \ */
/* OBSOLETE   ULONGEST reg;                                   \ */
/* OBSOLETE   int regnum;                                          \ */
/* OBSOLETE   for(regnum = 0;regnum < FP_REGNUM;regnum++){         \ */
/* OBSOLETE     sp-=REGISTER_SIZE;                         \ */
/* OBSOLETE     read_memory(sp,&reg,REGISTER_SIZE);        \ */
/* OBSOLETE     write_register(regnum,reg);}                       \ */
/* OBSOLETE   sp-=REGISTER_SIZE;                           \ */
/* OBSOLETE   read_memory(sp,&reg,REGISTER_SIZE);          \ */
/* OBSOLETE   write_register(PS_REGNUM,reg);                       \ */
/* OBSOLETE   sp-=REGISTER_SIZE;                           \ */
/* OBSOLETE   read_memory(sp,&reg,REGISTER_SIZE);          \ */
/* OBSOLETE   write_register(PC_REGNUM,reg);} */
/* OBSOLETE  */
/* OBSOLETE /* MJD - Size of dummy frame pushed onto stack by PUSH_DUMMY_FRAME *x/ */
/* OBSOLETE  */
/* OBSOLETE #define DUMMY_FRAME_SIZE (0x48) */
/* OBSOLETE  */
/* OBSOLETE /* MJD - The sequence of words in the instructions is */
/* OBSOLETE    halt */
/* OBSOLETE    halt */
/* OBSOLETE    halt */
/* OBSOLETE    halt */
/* OBSOLETE    subr    b2,stack size,0             grab stack space for dummy call */
/* OBSOLETE    labr    b3,x0(b2),0                 set AP_REGNUM to point at arguments */
/* OBSOLETE    lw      r2,x8(b3),0                 load r2 with first argument     */
/* OBSOLETE    lwbr    b1,arguments size(b2),0     load address of function to be called */
/* OBSOLETE    brlnk   r1,x8(b1),0                 call function */
/* OBSOLETE    halt */
/* OBSOLETE    halt */
/* OBSOLETE    labr    b2,stack size(b2),0         give back stack */
/* OBSOLETE    break                               break */
/* OBSOLETE    *x/ */
/* OBSOLETE  */
/* OBSOLETE #define CALL_DUMMY {0x00000000,  \ */
/* OBSOLETE                 0x00000000,  \ */
/* OBSOLETE                 0x59000000,  \ */
/* OBSOLETE                 0x598a0000,  \ */
/* OBSOLETE                 0xb5030008,  \ */
/* OBSOLETE                 0x5c820000,  \ */
/* OBSOLETE                 0x44810008,  \ */
/* OBSOLETE                 0x00000000,  \ */
/* OBSOLETE                 0x590a0000,  \ */
/* OBSOLETE                 0x28090000 } */
/* OBSOLETE  */
/* OBSOLETE #define CALL_DUMMY_LENGTH 40 */
/* OBSOLETE  */
/* OBSOLETE #define CALL_DUMMY_START_OFFSET 8 */
/* OBSOLETE  */
/* OBSOLETE #define CALL_DUMMY_STACK_ADJUST 8 */
/* OBSOLETE  */
/* OBSOLETE /* MJD - Fixup CALL_DUMMY for the specific function call. */
/* OBSOLETE    OK heres the problems */
/* OBSOLETE    1) On a trap there are two copies of the stack pointer, one in SP_REGNUM */
/* OBSOLETE       which is read/write and one in FP_REGNUM which is only read. It seems */
/* OBSOLETE       that when restarting the GOULD NP1 uses FP_REGNUM's value. */
/* OBSOLETE    2) Loading function address into b1 looks a bit difficult if bigger than */
/* OBSOLETE       0x0000fffc, infact from what I can tell the compiler sets up table of */
/* OBSOLETE       function address in base3 through which function calls are referenced. */
/* OBSOLETE  */
/* OBSOLETE    OK my solutions */
/* OBSOLETE      Calculate the size of the dummy stack frame and do adjustments of */
/* OBSOLETE      SP_REGNUM in the dummy call. */
/* OBSOLETE      Push function address onto the stack and load it in the dummy call */
/* OBSOLETE  *x/ */
/* OBSOLETE  */
/* OBSOLETE #define FIX_CALL_DUMMY(dummyname, sp, fun, nargs, args, type, gcc_p) \ */
/* OBSOLETE   {   int i;\ */
/* OBSOLETE       int arg_len = 0, total_len;\ */
/* OBSOLETE       old_sp = push_word(old_sp,fun);\ */
/* OBSOLETE       for(i = nargs - 1;i >= 0;i--)\ */
/* OBSOLETE     arg_len += TYPE_LENGTH (VALUE_TYPE (value_arg_coerce (args[i])));\ */
/* OBSOLETE       if(struct_return)\ */
/* OBSOLETE     arg_len += TYPE_LENGTH(value_type);\ */
/* OBSOLETE       total_len = DUMMY_FRAME_SIZE+CALL_DUMMY_STACK_ADJUST+4+arg_len;\ */
/* OBSOLETE       dummyname[0] += total_len;\ */
/* OBSOLETE       dummyname[2] += total_len;\ */
/* OBSOLETE       dummyname[5] += arg_len+CALL_DUMMY_STACK_ADJUST;\ */
/* OBSOLETE       dummyname[8] += total_len;} */
/* OBSOLETE  */
/* OBSOLETE /* MJD - So the stack should end up looking like this */
/* OBSOLETE  */
/* OBSOLETE                    | Normal stack frame  | */
/* OBSOLETE                    | from normal program | */
/* OBSOLETE                    | flow                | */
/* OBSOLETE                    +---------------------+ <- Final sp - 0x08 - argument size */
/* OBSOLETE                    |                     |    - 0x4 - dummy_frame_size */
/* OBSOLETE                    | Pushed dummy frame  | */
/* OBSOLETE                    |  b0-b7, r0-r7       | */
/* OBSOLETE                    |  pc and ps          | */
/* OBSOLETE                    |                     | */
/* OBSOLETE                    +---------------------+ */
/* OBSOLETE                    | Function address    | */
/* OBSOLETE                    +---------------------+ <- Final sp - 0x8 - arguments size */
/* OBSOLETE                    |                     | */
/* OBSOLETE                    |                     | */
/* OBSOLETE                    |                     | */
/* OBSOLETE                    |  Arguments to       | */
/* OBSOLETE                    |       Function      | */
/* OBSOLETE                    |                     | */
/* OBSOLETE                    |                     | */
/* OBSOLETE                    |                     | */
/* OBSOLETE                    +---------------------+ <- Final sp - 0x8 */
/* OBSOLETE                    | Dummy_stack_adjust  | */
/* OBSOLETE                    +---------------------+ <- Final sp */
/* OBSOLETE                    |                     | */
/* OBSOLETE                    | where call will     | */
/* OBSOLETE                    |   build frame       | */
/* OBSOLETE *x/ */
