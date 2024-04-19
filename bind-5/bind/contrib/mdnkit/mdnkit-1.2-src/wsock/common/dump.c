/*
 * dump.c - dump data
 */

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

#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "jpnicmdn.h"

PUCHAR  dumpAddr(const char FAR *addr, int len, PUCHAR buff)
{
    int     i ;
    PUCHAR  p ;
    
    for (i = 0, p = buff ; i < len ; i++, p += strlen(p)) {
        sprintf(p, "%d", (addr[i] & 0xff)) ;
	if (i + 1 < len) {
	    strcat(p, ".") ;
	}
    }
    return buff ;
}

PUCHAR  dumpHost(struct hostent FAR *hp, PUCHAR buff)
{
    int     i ;
    PUCHAR  p, a ;
    
    sprintf(buff, "%s ", hp->h_name) ;
    p = buff + strlen(buff) ;
    
    for (i = 0, a = hp->h_addr_list[0] ; i < hp->h_length ; i++, p += strlen(p)) {
        sprintf(p, "%d", (a[i] & 0xff)) ;
	if (i + 1 < hp->h_length) {
	    strcat(p, ".") ;
	}
    }
    return buff ;
}

PUCHAR  dumpName(PUCHAR name, PUCHAR buff)
{
    PUCHAR  sp, dp ;
    
    for (sp = name, dp = buff ; *sp != '\0' ; sp++) {
        if (*sp >= 0x21 && *sp <= 0x7e) {
	    *dp++ = *sp ;
	} else {
	    *dp++ = '\\' ;
	    *dp++ = 'x'  ;
            sprintf(dp, "%02x", (*sp & 0xff)) ;
	    dp += 2 ;
	}
    }
    *dp = '\0' ;
    
    return buff ;
}

