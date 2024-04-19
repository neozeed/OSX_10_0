/* $Id: normalizer.h,v 1.1.1.1 2001/01/31 03:58:58 zarzycki Exp $ */
/*
 * Copyright (c) 2000 Japan Network Information Center.  All rights reserved.
 *  
 * By using this file, you agree to the terms and conditions set forth bellow.
 * 
 * 			LICENSE TERMS AND CONDITIONS 
 * 
 * The following License Terms and Conditions apply, unless a different
 * license is obtained from Japan Network Information Center ("JPNIC"),
 * a Japanese association, Fuundo Bldg., 1-2 Kanda Ogawamachi, Chiyoda-ku,
 * Tokyo, Japan.
 * 
 * 1. Use, Modification and Redistribution (including distribution of any
 *    modified or derived work) in source and/or binary forms is permitted
 *    under this License Terms and Conditions.
 * 
 * 2. Redistribution of source code must retain the copyright notices as they
 *    appear in each source code file, this License Terms and Conditions.
 * 
 * 3. Redistribution in binary form must reproduce the Copyright Notice,
 *    this License Terms and Conditions, in the documentation and/or other
 *    materials provided with the distribution.  For the purposes of binary
 *    distribution the "Copyright Notice" refers to the following language:
 *    "Copyright (c) Japan Network Information Center.  All rights reserved."
 * 
 * 4. Neither the name of JPNIC may be used to endorse or promote products
 *    derived from this Software without specific prior written approval of
 *    JPNIC.
 * 
 * 5. Disclaimer/Limitation of Liability: THIS SOFTWARE IS PROVIDED BY JPNIC
 *    "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *    LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 *    PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL JPNIC BE LIABLE
 *    FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 *    CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 *    SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
 *    BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 *    WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 *    OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 *    ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
 * 
 * 6. Indemnification by Licensee
 *    Any person or entities using and/or redistributing this Software under
 *    this License Terms and Conditions shall defend indemnify and hold
 *    harmless JPNIC from and against any and all judgements damages,
 *    expenses, settlement liabilities, cost and other liabilities of any
 *    kind as a result of use and redistribution of this Software or any
 *    claim, suite, action, litigation or proceeding by any third party
 *    arising out of or relates to this License Terms and Conditions.
 * 
 * 7. Governing Law, Jurisdiction and Venue
 *    This License Terms and Conditions shall be governed by and and
 *    construed in accordance with the law of Japan. Any person or entities
 *    using and/or redistributing this Software under this License Terms and
 *    Conditions hereby agrees and consent to the personal and exclusive
 *    jurisdiction and venue of Tokyo District Court of Japan.
 */

#ifndef MDN_NORMALIZER_H
#define MDN_NORMALIZER_H 1

/*
 * Domain name normalizer.
 *
 * Perform normalization on the specified strings.  String must be
 * in UTF-8 encoding.
 */

#include <mdn/result.h>

/*
 * Normalizer type (opaque).
 */
typedef struct mdn_normalizer *mdn_normalizer_t;

/*
 * Normalizer procedure type.
 */
typedef mdn_result_t (*mdn_normalizer_proc_t)(const char *from,
					      char *to, size_t tolen);

/*
 * Initialize this module.
 *
 * Returns:
 *	mdn_success		-- ok.
 *	mdn_nomemory		-- malloc failed.
 */
extern mdn_result_t
mdn_normalizer_initialize(void);

/*
 * Create a empty normalizer.
 *
 * Returns:
 *	mdn_success		-- ok.
 *	mdn_nomemory		-- malloc failed.
 */
extern mdn_result_t
mdn_normalizer_create(mdn_normalizer_t *ctxp);

/*
 * Release all the memory allocated for a normalizer created by
 * 'mdn_normalizer_create'.
 */
extern void
mdn_normalizer_destroy(mdn_normalizer_t ctx);

/*
 * Add a normalization scheme to a normalizer.
 *
 * Multiple shemes can be added to a normalizer, and they will be
 * applied in order.
 *
 * Returns:
 *	mdn_success		-- ok.
 *	mdn_invalid_name	-- unknown scheme was specified.
 *	mdn_nomemory		-- malloc failed.
 */
extern mdn_result_t
mdn_normalizer_add(mdn_normalizer_t ctx, const char *scheme_name);

/*
 * Perform normalization(s) defined by a normalizer to the specified string, 
 * If the normalizer has two or more normalization schemes, they are
 * applied in order.
 *
 * Returns:
 *	mdn_success		-- ok.
 *	mdn_buffer_overflow	-- output buffer is too small.
 *	mdn_invalid_encoding	-- input is not a valid UTF-8 string.
 *	mdn_nomemory		-- malloc failed.
 */
extern mdn_result_t
mdn_normalizer_normalize(mdn_normalizer_t ctx, const char *from,
			 char *to, size_t tolen);

/*
 * Register a new normalization scheme.
 *
 * You can override the default normalization schemes, if you want.
 *
 * Returns:
 *	mdn_success		-- ok.
 *	mdn_nomemory		-- malloc failed.
 */
extern mdn_result_t
mdn_normalizer_register(const char *scheme_name, mdn_normalizer_proc_t proc);

#endif /* MDN_NORMALIZER_H */
