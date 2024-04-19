/* $Id: ResConf.xs,v 1.1.1.1 2001/01/31 03:58:53 zarzycki Exp $ */
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

#include <mdn/res.h>

typedef mdn_resconf_t MDN__ResConf;

#include "lasterror.h"

static mdn_resconf_t
get_resconf(SV *sv)
{
    IV tmp;

    if (!sv_derived_from(sv, "MDN::ResConf"))
	croak("self is not of type MDN::ResConf");
    tmp = SvIV((SV *)SvRV(sv));
    return (mdn_resconf_t)tmp;
}

static SV *
new_magic_sv(SV *self, char *class, void *ptr)
{
    SV *ret = sv_newmortal();
    sv_setref_pv(ret, class, (void *)ptr);
    sv_magic(SvRV(ret), SvRV(self), '~', "hack", 4);
    return ret;
}

/******************************************************************/
MODULE = MDN::ResConf		PACKAGE = MDN::ResConf

PROTOTYPES: ENABLE

BOOT:
    {
	mdn_result_t result;
	set_last_error(mdn_success, NULL);
	result = mdn_resconf_initialize();
	if (result != mdn_success) {
	    my_croak(result, NULL);
	}
    }


MDN::ResConf
new(package_name, file_name = NULL)
	const char *package_name
	const char *file_name
    PREINIT:
	mdn_resconf_t context;
	mdn_result_t result;
    CODE:
	result = mdn_resconf_create(&context);
        if (result == mdn_success) {
	    ;
	} else {
	    my_croak(result, NULL);
	}
	result = mdn_resconf_loadfile(context, file_name);
        if (result == mdn_success) {
	    RETVAL = context;
        } else if (result == mdn_nofile || result == mdn_invalid_syntax) {
	    RETVAL = NULL;
	    set_last_error(result, file_name);
	} else if (result == mdn_invalid_name) {
	    RETVAL = NULL;
	    set_last_error(result, NULL);
        } else {
	    my_croak(result, file_name);
	}
    OUTPUT:
	RETVAL

void
DESTROY(self)
	MDN::ResConf self
    CODE:
	mdn_resconf_destroy(self);

SV*
localtoucs(self, local_name)
	MDN::ResConf self
	const char *local_name
    PREINIT:
	mdn_result_t result;
	char *ucs_name;
	size_t ucs_name_length;
	size_t local_name_length;
    CODE:
	ucs_name_length = strlen(local_name) + 1;
	for (;;) {
	    ucs_name = (char *)safemalloc(ucs_name_length);
	    result = mdn_res_localtoucs(self, local_name, ucs_name,
		ucs_name_length);
	    if (result == mdn_success) {
		RETVAL = newSVpv(ucs_name, 0);
		break;
	    } else if (result == mdn_invalid_encoding) {
		RETVAL = &PL_sv_undef;
		set_last_error(result, local_name);
		break;
	    } else if (result != mdn_buffer_overflow) {
		my_croak(result, local_name);
	    }
	    safefree(ucs_name);
	    ucs_name_length += ucs_name_length;
	}
	safefree(ucs_name);
    OUTPUT:
	RETVAL

SV*
ucstolocal(self, ucs_name)
	MDN::ResConf self
	const char *ucs_name
    PREINIT:
	mdn_result_t result;
	char *local_name;
	size_t local_name_length;
	size_t ucs_name_length;
    CODE:
	local_name_length = strlen(ucs_name) + 1;
	for (;;) {
	    local_name = (char *)safemalloc(local_name_length);
	    result = mdn_res_ucstolocal(self, ucs_name, local_name,
		local_name_length);
	    if (result == mdn_success) {
		RETVAL = newSVpv((char *)local_name, 0);
		break;
	    } else if (result == mdn_invalid_encoding) {
		RETVAL = &PL_sv_undef;
		set_last_error(result, ucs_name);
		break;
	    } else if (result != mdn_buffer_overflow) {
		my_croak(result, ucs_name);
	    }
	    safefree((char *)ucs_name);
	    local_name_length += local_name_length;
	}
	safefree(local_name);
    OUTPUT:
	RETVAL

SV*
normalize(self, name)
	MDN::ResConf self
	const char *name
    PREINIT:
	mdn_result_t result;
	char *normalized_name;
	size_t name_length;
	size_t normalized_name_length;
    CODE:
	normalized_name_length = strlen(name) + 1;
	for (;;) {
	    normalized_name = (char *)safemalloc(normalized_name_length);
	    result = mdn_res_normalize(self, name, normalized_name,
		normalized_name_length);
	    if (result == mdn_success) {
		RETVAL = newSVpv(normalized_name, 0);
		break;
	    } else if (result == mdn_invalid_encoding) {
		RETVAL = &PL_sv_undef;
		set_last_error(result, name);
		break;
	    } else if (result != mdn_buffer_overflow) {
		my_croak(result, name);
	    }
	    safefree(normalized_name);
	    normalized_name_length += normalized_name_length;
	}
	safefree(normalized_name);
    OUTPUT:
	RETVAL

SV*
dnstoucs(self, dns_name)
	MDN::ResConf self
	const char *dns_name
    PREINIT:
	mdn_result_t result;
	char *ucs_name;
	size_t ucs_name_length;
	size_t local_name_length;
    CODE:
	ucs_name_length = strlen(dns_name) + 1;
	for (;;) {
	    ucs_name = (char *)safemalloc(ucs_name_length);
	    result = mdn_res_dnstoucs(self, dns_name, ucs_name,
		ucs_name_length);
	    if (result == mdn_success) {
		RETVAL = newSVpv(ucs_name, 0);
		break;
	    } else if (result == mdn_invalid_encoding) {
		RETVAL = &PL_sv_undef;
		set_last_error(result, dns_name);
		break;
	    } else if (result != mdn_buffer_overflow) {
		my_croak(result, dns_name);
	    }
	    safefree(ucs_name);
	    ucs_name_length += ucs_name_length;
	}
	safefree(ucs_name);
    OUTPUT:
	RETVAL

SV*
ucstodns(self, ucs_name)
	MDN::ResConf self
	const char *ucs_name
    PREINIT:
	mdn_result_t result;
	char *dns_name;
	size_t dns_name_length;
	size_t ucs_name_length;
    CODE:
	dns_name_length = strlen(ucs_name) + 1;
	for (;;) {
	    dns_name = (char *)safemalloc(dns_name_length);
	    result = mdn_res_ucstodns(self, ucs_name, dns_name,
		dns_name_length);
	    if (result == mdn_success) {
		RETVAL = newSVpv(dns_name, 0);
		break;
	    } else if (result == mdn_invalid_encoding) {
		RETVAL = &PL_sv_undef;
		set_last_error(result, ucs_name);
		break;
	    } else if (result != mdn_buffer_overflow) {
		my_croak(result, ucs_name);
	    }
	    safefree(dns_name);
	    dns_name_length += dns_name_length;
	}
	safefree(dns_name);
    OUTPUT:
	RETVAL

SV*
lasterror(package_name)
	const char *package_name
    CODE:
	RETVAL = newSVpv(last_error_message, 0);
    OUTPUT:
	RETVAL

void
localconverter(self)
	SV * self
    INIT:
	SV *ret;
	mdn_resconf_t resconf;
	mdn_converter_t conv;
    PPCODE:
	resconf = get_resconf(self);
	conv = mdn_resconf_localconverter(resconf);
	if (conv == NULL) {
	    XPUSHs(&PL_sv_undef);
	} else {
	    ret = new_magic_sv(self, "MDN::ResConf::Converter", (void *)conv);
	    XPUSHs(ret);
	}

void
serverconverter(self)
	SV * self
    INIT:
	SV *ret;
	mdn_resconf_t resconf;
	mdn_converter_t conv;
    PPCODE:
	resconf = get_resconf(self);
	conv = mdn_resconf_serverconverter(resconf);
	if (conv == NULL) {
	    XPUSHs(&PL_sv_undef);
	} else {
	    ret = new_magic_sv(self, "MDN::ResConf::Converter", (void *)conv);
	    XPUSHs(ret);
	}

void
alternateconverter(self)
	SV * self
    INIT:
	SV *ret;
	mdn_resconf_t resconf;
	mdn_converter_t conv;
    PPCODE:
	resconf = get_resconf(self);
	conv = mdn_resconf_alternateconverter(resconf);
	if (conv == NULL) {
	    XPUSHs(&PL_sv_undef);
	} else {
	    ret = new_magic_sv(self, "MDN::ResConf::Converter", (void *)conv);
	    XPUSHs(ret);
	}

void
normalizer(self)
	SV * self
    INIT:
	SV *ret;
	mdn_resconf_t resconf;
	mdn_normalizer_t norm;
    PPCODE:
	resconf = get_resconf(self);
	norm = mdn_resconf_normalizer(resconf);
	if (norm == NULL) {
	    XPUSHs(&PL_sv_undef);
	} else {
	    ret = new_magic_sv(self, "MDN::ResConf::Normalizer", (void *)norm);
	    XPUSHs(ret);
	}

SV*
zld(self)
	MDN::ResConf self
    PREINIT:
	const char *zld;
    CODE:
	zld = mdn_resconf_zld(self);
	if (zld == NULL) {
	    RETVAL = newSVpv("", 0);
	} else {
	    RETVAL = newSVpv((char *)zld, 0);
	}
    OUTPUT:
	RETVAL
