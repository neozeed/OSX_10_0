/*
 * Copyright (c) 1999-2004, Apple Computer, Inc. All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1.  Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer. 
 * 2.  Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution. 
 * 3.  Neither the name of Apple Computer, Inc. ("Apple") nor the names of
 *     its contributors may be used to endorse or promote products derived
 *     from this software without specific prior written permission. 
 * 
 * THIS SOFTWARE IS PROVIDED BY APPLE AND ITS CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL APPLE OR ITS CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */
/* bytesex.h */

#if defined(__MWERKS__) && !defined(__private_extern__)
#define __private_extern__ __declspec(private_extern)
#endif

#import <mach-o/fat.h>
#import <mach-o/loader.h>
#import <mach/m68k/thread_status.h>
#import <mach/ppc/thread_status.h>
#import <mach/m88k/thread_status.h>
#import <mach/i860/thread_status.h>
#import <mach/i386/thread_status.h>
#import <mach/hppa/thread_status.h>
#import <mach/sparc/thread_status.h>
#import <mach-o/nlist.h>
#import <mach-o/reloc.h>
#import <mach-o/ranlib.h>
#import "stuff/bool.h"

enum byte_sex {
    UNKNOWN_BYTE_SEX,
    BIG_ENDIAN_BYTE_SEX,
    LITTLE_ENDIAN_BYTE_SEX
};

#define SWAP_SHORT(a) ( ((a & 0xff) << 8) | ((unsigned short)(a) >> 8) )

#define SWAP_LONG(a) ( ((a) << 24) | \
		      (((a) << 8) & 0x00ff0000) | \
		      (((a) >> 8) & 0x0000ff00) | \
	((unsigned long)(a) >> 24) )

__private_extern__ float SWAP_FLOAT(
    float f);

__private_extern__ double SWAP_DOUBLE(
    double d);

__private_extern__ enum byte_sex get_host_byte_sex(
    void);

__private_extern__ void swap_fat_header(
    struct fat_header *fat_header,
    enum byte_sex target_byte_sex);

__private_extern__ void swap_fat_arch(
    struct fat_arch *fat_archs,
    unsigned long nfat_arch,
    enum byte_sex target_byte_sex);

__private_extern__ void swap_mach_header(
    struct mach_header *mh,
    enum byte_sex target_byte_sex);

__private_extern__ void swap_load_command(
    struct load_command *lc,
    enum byte_sex target_byte_sex);

__private_extern__ void swap_segment_command(
    struct segment_command *sg,
    enum byte_sex target_byte_sex);

__private_extern__ void swap_section(
    struct section *s,
    unsigned long nsects,
    enum byte_sex target_byte_sex);

__private_extern__ void swap_symtab_command(
    struct symtab_command *st,
    enum byte_sex target_byte_sex);

__private_extern__ void swap_dysymtab_command(
    struct dysymtab_command *dyst,
    enum byte_sex target_byte_sex);

__private_extern__ void swap_symseg_command(
    struct symseg_command *ss,
    enum byte_sex target_byte_sex);

__private_extern__ void swap_fvmlib_command(
    struct fvmlib_command *fl,
    enum byte_sex target_byte_sex);

__private_extern__ void swap_dylib_command(
    struct dylib_command *dl,
    enum byte_sex target_byte_sex);

__private_extern__ void swap_sub_framework_command(
    struct sub_framework_command *sub,
    enum byte_sex target_byte_sex);

__private_extern__ void swap_sub_umbrella_command(
    struct sub_umbrella_command *usub,
    enum byte_sex target_byte_sex);

__private_extern__ void swap_sub_client_command(
    struct sub_client_command *csub,
    enum byte_sex target_byte_sex);

__private_extern__ void swap_prebound_dylib_command(
    struct prebound_dylib_command *pbdylib,
    enum byte_sex target_byte_sex);

__private_extern__ void swap_dylinker_command(
    struct dylinker_command *dyld,
    enum byte_sex target_byte_sex);

__private_extern__ void swap_fvmfile_command(
    struct fvmfile_command *ff,
    enum byte_sex target_byte_sex);

__private_extern__ void swap_thread_command(
    struct thread_command *ut,
    enum byte_sex target_byte_sex);

__private_extern__ void swap_m68k_thread_state_regs(
    struct m68k_thread_state_regs *cpu,
    enum byte_sex target_byte_sex);

__private_extern__ void swap_m68k_thread_state_68882(
    struct m68k_thread_state_68882 *fpu,
    enum byte_sex target_byte_sex);

__private_extern__ void swap_m68k_thread_state_user_reg(
    struct m68k_thread_state_user_reg *user_reg,
    enum byte_sex target_byte_sex);

__private_extern__ void swap_ppc_thread_state_t(
    ppc_thread_state_t *cpu,
    enum byte_sex target_byte_sex);

__private_extern__ void swap_ppc_float_state_t(
    ppc_float_state_t *fpu,
    enum byte_sex target_byte_sex);

__private_extern__ void swap_ppc_exception_state_t(
    ppc_exception_state_t *state,
    enum byte_sex target_byte_sex);

__private_extern__ void swap_m88k_thread_state_grf_t(
    m88k_thread_state_grf_t *cpu,
    enum byte_sex target_byte_sex);

__private_extern__ void swap_m88k_thread_state_xrf_t(
    m88k_thread_state_xrf_t *fpu,
    enum byte_sex target_byte_sex);

__private_extern__ void swap_m88k_thread_state_user_t(
    m88k_thread_state_user_t *user,
    enum byte_sex target_byte_sex);

__private_extern__ void swap_m88110_thread_state_impl_t(
    m88110_thread_state_impl_t *spu,
    enum byte_sex target_byte_sex);

__private_extern__ void swap_i860_thread_state_regs(
    struct i860_thread_state_regs *cpu,
    enum byte_sex target_byte_sex);

__private_extern__ void swap_i386_thread_state(
    i386_thread_state_t *cpu,
    enum byte_sex target_byte_sex);

__private_extern__ void swap_i386_thread_fpstate(
    i386_thread_fpstate_t *fpu,
    enum byte_sex target_byte_sex);

__private_extern__ void swap_i386_thread_exceptstate(
    i386_thread_exceptstate_t *exc,
    enum byte_sex target_byte_sex);

__private_extern__ void swap_i386_thread_cthreadstate(
    i386_thread_cthreadstate_t *user,
    enum byte_sex target_byte_sex);

__private_extern__ void swap_hppa_integer_thread_state(
    struct hp_pa_integer_thread_state *regs,
    enum byte_sex target_byte_order);

__private_extern__ void swap_hppa_frame_thread_state(
  struct hp_pa_frame_thread_state *frame,
  enum byte_sex target_byte_order);

__private_extern__ void swap_hppa_fp_thread_state(
  struct hp_pa_fp_thread_state *fp,
  enum byte_sex target_byte_order);

__private_extern__ void swap_sparc_thread_state_regs(
  struct sparc_thread_state_regs *cpu,
  enum byte_sex target_byte_order);

__private_extern__ void swap_sparc_thread_state_fpu(
  struct sparc_thread_state_fpu *fpu,
  enum byte_sex target_byte_order);

__private_extern__ void swap_ident_command(
    struct ident_command *id_cmd,
    enum byte_sex target_byte_sex);

__private_extern__ void swap_routines_command(
    struct routines_command *r_cmd,
    enum byte_sex target_byte_sex);

__private_extern__ void swap_nlist(
    struct nlist *symbols,
    unsigned long nsymbols,
    enum byte_sex target_byte_sex);

__private_extern__ void swap_ranlib(
    struct ranlib *ranlibs,
    unsigned long nranlibs,
    enum byte_sex target_byte_sex);

__private_extern__ void swap_relocation_info(
    struct relocation_info *relocs,
    unsigned long nrelocs,
    enum byte_sex target_byte_sex);

__private_extern__ void swap_indirect_symbols(
    unsigned long *indirect_symbols,
    unsigned long nindirect_symbols,
    enum byte_sex target_byte_sex);

__private_extern__ void swap_dylib_reference(
    struct dylib_reference *refs,
    unsigned long nrefs,
    enum byte_sex target_byte_sex);

__private_extern__ void swap_dylib_module(
    struct dylib_module *mods,
    unsigned long nmods,
    enum byte_sex target_byte_sex);

__private_extern__ void swap_dylib_table_of_contents(
    struct dylib_table_of_contents *tocs,
    unsigned long ntocs,
    enum byte_sex target_byte_sex);

/*
 * swap_object_headers() swaps the object file headers from the host byte sex
 * into the non-host byte sex.  It returns TRUE if it can and did swap the
 * headers else returns FALSE and does not touch the headers and prints an error
 * using the error() routine.
 */
__private_extern__ enum bool swap_object_headers(
    struct mach_header *mh,
    struct load_command *load_commands);

/*
 * get_toc_byte_sex() guesses the byte sex of the table of contents of the
 * library mapped in at the address, addr, of size, size based on the first
 * object file's bytesex.  If it can't figure it out, because the library has
 * no object file members or is malformed it will return UNKNOWN_BYTE_SEX.
 */
__private_extern__ enum byte_sex get_toc_byte_sex(
    char *addr,
    unsigned long size);
