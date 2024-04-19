/* $Id: Log.xs,v 1.1.1.1 2001/01/31 03:58:53 zarzycki Exp $ */
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

#include <mdn/log.h>

typedef struct {
    int id;
    const char *string;
} MDN_Log_Level;

static const MDN_Log_Level log_level_table[] = {
   {mdn_log_level_fatal,   "fatal"},
   {mdn_log_level_error,   "error"},
   {mdn_log_level_warning, "warning"},
   {mdn_log_level_info,    "info"},
   {mdn_log_level_trace,   "trace"},
   {mdn_log_level_dump,    "dump"},
   {0,                     NULL}
};

static SV *message_handler = (SV *)NULL;
 
static const char *
log_level_id_to_string(id)
    int id;
{
    int i;

    for (i = 0; log_level_table[i].string != NULL; i++) {
	if (id == log_level_table[i].id)
	    return log_level_table[i].string;
    }

    croak("invalid log level %d", id);
}

static int
log_level_string_to_id(string)
    const char *string;
{
    int i;

    for (i = 0; log_level_table[i].string != NULL; i++) {
	if (strEQ(string, log_level_table[i].string))
	    return log_level_table[i].id;
    }

    croak("invalid log level %d", string);
}

static void
log_to_nowhere(int level, const char *buf)
{
}

static void
call_handler(int level, const char *message)
{
    dSP;

    ENTER;
    SAVETMPS;

    PUSHMARK(SP);
    XPUSHs(sv_2mortal(newSVpv((char *)log_level_id_to_string(level), 0)));
    XPUSHs(sv_2mortal(newSVpv((char *)message, 0)));
    PUTBACK;

    perl_call_sv(message_handler, G_DISCARD);

    FREETMPS;
    LEAVE;
}

/******************************************************************/

MODULE = MDN::Log		PACKAGE = MDN::Log		

PROTOTYPES: ENABLE

void
setlevel(package_name, level)
	const char *package_name
	const char *level
    PREINIT:
	int level_id;
    CODE:
	mdn_log_setlevel(log_level_string_to_id(level));

SV*
getlevel(package_name)
	const char *package_name
    PREINIT:
	char *string;
	int level;
    CODE:
	RETVAL = newSVpv((char *)log_level_id_to_string(mdn_log_getlevel()),
	    0);
    OUTPUT:
	RETVAL

void
setproc_stderr(package_name)
	const char *package_name
    CODE:
	mdn_log_setproc(NULL);

void
setproc_none(package_name)
	const char *package_name
    CODE:
	mdn_log_setproc((mdn_log_proc_t)log_to_nowhere);

void
setproc(package_name, handler = &PL_sv_undef)
	const char *package_name
	SV *handler
    CODE:
	if (!SvOK(handler)) {
	    mdn_log_setproc(NULL);
	} else {
	    if (message_handler == (SV *)NULL)
		message_handler = newSVsv(handler);
	    else
		SvSetSV(message_handler, handler);
	    mdn_log_setproc(call_handler);
	}
