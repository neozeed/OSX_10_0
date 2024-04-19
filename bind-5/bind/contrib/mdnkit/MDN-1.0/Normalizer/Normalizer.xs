/* $Id: Normalizer.xs,v 1.1.1.1 2001/01/31 03:58:53 zarzycki Exp $ */
/*
 * Copyright (c) 2000 Japan Network Information Center.  All rights reserved.
 *  
 * By using this file, you agree to the terms and conditions set forth bellow.
 * 
 *                      LICENSE TERMS AND CONDITIONS 
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

#include "EXTERN.h"
#include "perl.h"
#include "XSUB.h"

#include <mdn/normalizer.h>

typedef mdn_normalizer_t MDN__Normalizer;

#include "lasterror.h"

/******************************************************************/
MODULE = MDN::Normalizer		PACKAGE = MDN::Normalizer

PROTOTYPES: ENABLE

BOOT:
    {
	mdn_result_t result;
	set_last_error(mdn_success, NULL);
	result = mdn_normalizer_initialize();
        if (result != mdn_success) {
	    my_croak(result, NULL);
	}
    }	

MDN::Normalizer
new_internal(package_name)
	const char *package_name
    PREINIT:
	mdn_normalizer_t context;
	mdn_result_t result;
    CODE:
	result = mdn_normalizer_create(&context);
        if (result == mdn_success) {
	    RETVAL = context;
	} else {
	    my_croak(result, NULL);
	}
    OUTPUT:
	RETVAL

void
DESTROY(self)
	MDN::Normalizer self
    CODE:
	mdn_normalizer_destroy(self);

int
add(self, ...)
	MDN::Normalizer self
    PROTOTYPE: $@
    PREINIT:
	const char *scheme_name;
	mdn_result_t result;
	STRLEN scheme_name_length;
	I32 i;
    CODE:
	RETVAL = 1;
	for (i = 1; i < items; i++) {
	    scheme_name = SvPV(ST(i), scheme_name_length);
	    result = mdn_normalizer_add(self, scheme_name);
	    if (result == mdn_success) {
		;
	    } else if (result == mdn_invalid_name) {
		RETVAL = 0;
		set_last_error(result, scheme_name);
		break;
	    } else {
		my_croak(result, scheme_name);
	    }
	}
    OUTPUT:
	RETVAL

SV*
normalize(self, from)
	MDN::Normalizer self
	const char *from
    PREINIT:
	mdn_result_t result;
	char *to;
	size_t to_length;
    CODE:
	to_length = strlen(from) + 1;
	for (;;) {
	    to = (char *)safemalloc(to_length);
	    result = mdn_normalizer_normalize(self, from, to, to_length);
	    if (result == mdn_success) {
		RETVAL = newSVpv(to, 0);
		break;
	    } else if (result == mdn_invalid_encoding) {
		RETVAL = &PL_sv_undef;
		set_last_error(result, from);
		break;
	    } else if (result != mdn_buffer_overflow) {
		my_croak(result, from);
	    }
	    safefree(to);
	    to_length += to_length;
	}
	safefree(to);
    OUTPUT:
	RETVAL

SV*
lasterror(package_name)
	const char *package_name
    CODE:
	RETVAL = newSVpv(last_error_message, 0);
    OUTPUT:
	RETVAL
