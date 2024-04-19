/* $Id: lasterror.h,v 1.1.1.1 2001/01/31 03:58:53 zarzycki Exp $ */
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

#include <sys/types.h>
#include <stdio.h>
#include <string.h>

typedef struct {
    mdn_result_t result;
    const char *message;
    int sub_message_flag;
} mdn_result_info_t;

#define NO_SUB_MESSAGE   0
#define WITH_SUB_MESSAGE 1

static const mdn_result_info_t result_info_table[] = {
    {mdn_success,          "success",                      NO_SUB_MESSAGE},
    {mdn_notfound,         "not found",                    NO_SUB_MESSAGE},
    {mdn_invalid_encoding, "invalid encoding",             WITH_SUB_MESSAGE},
    {mdn_invalid_syntax,   "invalid syntax",               WITH_SUB_MESSAGE},
    {mdn_invalid_name,     "invalid encoding name",        WITH_SUB_MESSAGE},
    {mdn_invalid_message,  "invalid message in",           WITH_SUB_MESSAGE},
    {mdn_buffer_overflow,  "buffer overflow",              NO_SUB_MESSAGE},
    {mdn_noentry,          "no such entry",                WITH_SUB_MESSAGE},
    {mdn_nomemory,         "memory exhausted",             NO_SUB_MESSAGE},
    {mdn_nofile,           "no such file",                 WITH_SUB_MESSAGE},
    {mdn_nomapping,        "no such converter",            WITH_SUB_MESSAGE},
    {mdn_context_required, "context information required", NO_SUB_MESSAGE},
    /* The following must be the last element. */
    {mdn_failure,          "error",                        NO_SUB_MESSAGE}
};

static char *last_error_message;

static void
set_last_error(result, sub_message)
    mdn_result_t result;
    const char *sub_message;
{
    const mdn_result_info_t *info;

    if (last_error_message != NULL)
	safefree(last_error_message);

    for (info = result_info_table; info->result != mdn_failure; info++) {
	if (result == info->result) {
	    break;
	}
    }

    if (sub_message == NULL
	|| info->sub_message_flag == NO_SUB_MESSAGE) {
	last_error_message = (char *)safemalloc(strlen(info->message) + 1);
	strcpy(last_error_message, info->message);
    } else {
	last_error_message = (char *)safemalloc(strlen(info->message)
	    + strlen(sub_message) + 3);
	sprintf(last_error_message, "%s, %s", info->message, sub_message);
    }
}

static void
my_croak(result, sub_message)
    mdn_result_t result;
    const char *sub_message;
{
    set_last_error(result, sub_message);
    croak(last_error_message);
}
