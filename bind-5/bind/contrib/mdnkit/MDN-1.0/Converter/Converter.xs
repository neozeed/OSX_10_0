/* $Id: Converter.xs,v 1.1.1.1 2001/01/31 03:58:53 zarzycki Exp $ */
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

#include <mdn/converter.h>

typedef mdn_converter_t MDN__Converter;

#include "lasterror.h"

static int
not_here(char *s)
{
    croak("%s not implemented on this architecture", s);
    return -1;
}

static double
constant(char *name, int arg)
{
    errno = 0;
    if (strEQ(name, "DELAYEDOPEN"))
#ifdef MDN_CONVERTER_DELAYEDOPEN
	return MDN_CONVERTER_DELAYEDOPEN;
#else
	goto not_there;
#endif
    if (strEQ(name, "RTCHECK"))
#ifdef MDN_CONVERTER_RTCHECK
	return MDN_CONVERTER_RTCHECK;
#else
	goto not_there;
#endif
    errno = EINVAL;
    return 0;

not_there:
    errno = ENOENT;
    return 0;
}

/******************************************************************/
MODULE = MDN::Converter		PACKAGE = MDN::Converter

PROTOTYPES: ENABLE

double
constant(name,arg)
	char *		name
	int		arg

BOOT:
    {
	mdn_result_t result;
	set_last_error(mdn_success, NULL);
	result = mdn_converter_initialize();
	if (result != mdn_success) {
	    my_croak(result, NULL);
	}
    }
	
MDN::Converter
new_internal(package_name, encoding_name, flags)
	const char *package_name
	const char *encoding_name
	int flags
    PREINIT:
	mdn_converter_t context;
	mdn_result_t result;
	const char *comma;
	const char *flag_start;
    CODE:
	result = mdn_converter_create(encoding_name, &context, flags);
        if (result == mdn_success) {
	    RETVAL = context;
        } else if (result == mdn_invalid_name) {
	    RETVAL = NULL;
	    set_last_error(result, encoding_name);
        } else {
	    my_croak(result, encoding_name);
	}
    OUTPUT:
	RETVAL

void
DESTROY(self)
	MDN::Converter self
    CODE:
	mdn_converter_destroy(self);

SV*
localencoding(self)
	MDN::Converter self
    PREINIT:
	char *encoding;
    CODE:
	encoding = mdn_converter_localencoding(self);
	if (encoding != NULL && *encoding != '\0') {
	    RETVAL = newSVpv(encoding, 0);
	} else {
	    RETVAL = &PL_sv_undef;
	}
    OUTPUT:
	RETVAL

int
isasciicompatible(self)
	MDN::Converter self
    CODE:
	RETVAL = mdn_converter_isasciicompatible(self);
    OUTPUT:
	RETVAL

SV*
convert(self, direction, from)
	MDN::Converter self
	const char *direction
	const char *from
    PREINIT:
	mdn_result_t result;
	mdn_converter_dir_t direction_id;
	char *to;
	size_t to_length;
    CODE:
	if (strEQ(direction, "l2u") || strEQ(direction, "localtoutf8")) {
            direction_id = mdn_converter_l2u;
	} else if (strEQ(direction, "u2l")
	    || strEQ(direction, "utf8tolocal")) {
            direction_id = mdn_converter_u2l;
	} else {
	    croak("invalid direction %s", direction);
	}
	to_length = strlen(from) + 1;
	for (;;) {
	    to = (char *)safemalloc(to_length);
	    result = mdn_converter_convert(self, direction_id, from, to,
		to_length);
	    if (result == mdn_success) {
		RETVAL = newSVpv(to, 0);
		break;
	    } else if (result == mdn_invalid_encoding
		|| result == mdn_invalid_name) {
		RETVAL = &PL_sv_undef;
		set_last_error(result, NULL);
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

void
addalias(package_name, alias_name, real_name)
	const char *package_name
	const char *alias_name
	const char *real_name
    PREINIT:
	mdn_result_t result;
    CODE:
	result = mdn_converter_addalias(alias_name, real_name);
        if (result != mdn_success) {
	    my_croak(result, alias_name);
	}

int
aliasfile(package_name, file_name)
	const char *package_name
	const char *file_name
    PREINIT:
	mdn_result_t result;
    CODE:
	result = mdn_converter_aliasfile(file_name);
        if (result == mdn_success) {
	    RETVAL = 1;
	} else if (result == mdn_nofile || result == mdn_invalid_syntax) {
	    set_last_error(result, file_name);
	    RETVAL = 0;
	} else {
	    my_croak(result, file_name);
	}
    OUTPUT:
	RETVAL

void
resetalias(package_name)
	const char *package_name
    PREINIT:
	mdn_result_t result;
    CODE:
	result = mdn_converter_resetalias();
        if (result != mdn_success) {
	    my_croak(result, NULL);
	}

SV*
lasterror(package_name)
	const char *package_name
    CODE:
	RETVAL = newSVpv(last_error_message, 0);
    OUTPUT:
	RETVAL
