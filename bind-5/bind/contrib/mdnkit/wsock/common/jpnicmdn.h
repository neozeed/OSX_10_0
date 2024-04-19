/*
 * jpnicmdn.h
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

#ifndef _JPNICMDN_H
#define _JPNICMDN_H

void    mdnPrintf(PUCHAR fmt, ...) ;
void    mdnLogProc(int level, const char *msg) ;
void    mdnLogInit(void) ;

PUCHAR  dumpAddr(const char FAR *addr, int len, PUCHAR buff) ;
PUCHAR  dumpHost(struct hostent FAR *hp, PUCHAR buff) ;
PUCHAR  dumpName(PUCHAR name, PUCHAR buff) ;

int     mdnEncodeWhere(void) ;

#define MDN_ENCODE_ALWAYS   0
#define MDN_ENCODE_CHECK    1
#define MDN_ENCODE_ONLY11   2
#define MDN_ENCODE_ONLY20   3

BOOL    mdnGetDnsEncoding(PUCHAR enc) ;
BOOL    mdnGetPrgEncoding(PUCHAR enc) ;
BOOL    mdnGetNormalize(PUCHAR norm) ;
BOOL    mdnGetLogFile(PUCHAR file) ;

BOOL    mdnCheckDll(PUCHAR name) ;

int     mdnGetLogLevel(void) ;      /* 0 : fatal        */
                                    /* 1 : error        */
				    /* 2 : warning      */
				    /* 3 : info         */
				    /* 4 : trace        */
				    /* 5 : dump         */

/*
 * Converter I/F, wrapper for libmdn
 */

#include <mdn/result.h>
#include <mdn/log.h>
#include <mdn/converter.h>
#include <mdn/normalizer.h>
#include <mdn/translator.h>
#include <mdn/zldrule.h>

/*
 * Conversion Context, determined when attached to process
 */
 
typedef struct _CONV {
    mdn_converter_t     local_converter  ;
    mdn_converter_t     local_alternate  ;
    char                *local_zld       ;
    mdn_converter_t     server_converter ;
    mdn_converter_t     server_alternate ;
    char                *server_zld      ;
    mdn_normalizer_t    normalizer       ;
} CONVREC, *CONVPTR ;

CONVPTR mdnConvInit(char *local, char *server, char *norm) ;
void    mdnConvDone(CONVPTR converter) ;

/*
 * Converting Request/Response
 */

BOOL    mdnConvReq(CONVPTR converter, const char FAR *from, char FAR *to, size_t tolen) ;
BOOL    mdnConvRsp(CONVPTR converter, const char FAR *from, char FAR *to, size_t tolen) ;

/*
 * Hook for Asynchronouse Query
 */

void    mdnHookInit(void) ;
void    mdnHookDone(void) ;
BOOL    mdnHook(HWND hWnd, u_int wMsg, char FAR *buf, CONVPTR pConv) ;

#endif  /* _JPNICMDN_H */
