/*
 * convert.c - convert domain name
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

/*
 * parseNorm - parse norliazer parameter string
 */

static  PUCHAR  parseNorm(PUCHAR param, PUCHAR norm)
{
    BOOL    found = FALSE ;
    
    for ( ; *param != '\0' ; param++) {
        if (isascii(*param)) {
	    break ;
	}
    }
    for ( ; *param != '\0' ; param++) {
        if (isspace(*param)) {
	    break ;
	}
	*norm++ = *param ;
	found = TRUE ;
    }
    *norm = '\0' ;
    
    if (found) {
        return param ;
    } else {
        return NULL ;
    }
}

/*
 * parseEncode - parse encode parameter string "name opt ..."
 */

static  void    parseEncode(PUCHAR param, PUCHAR enc, PUCHAR opt)
{
    for ( ; *param != '\0' ; param++) {
        if (isascii(*param)) {
	    break ;
	}
    }
    for ( ; *param != '\0' ; param++) {
        if (isspace(*param)) {
	    break ;
	}
        *enc++= *param ;
    }
    for ( ; *param != '\0' ; param++) {
        if (isascii(*param)) {
	    break ;
	}
    }
    for ( ; *param != '\0' ; param++) {
        *opt++= *param ;
    }
    *enc = '\0' ;
    *opt = '\0' ;
}

/*
 * prepare/dispose conversion context
 */
 
void    mdnConvDone(CONVPTR converter)
{
    if (converter == NULL) {
        return ;
    }
    if (converter->normalizer != NULL) {
        mdn_normalizer_destroy(converter->normalizer) ;
	converter->normalizer = NULL ;
    }
    if (converter->local_converter != NULL) {
        mdn_converter_destroy(converter->local_converter) ;
	converter->local_converter = NULL ;
    }
    if (converter->local_alternate != NULL) {
        mdn_converter_destroy(converter->local_alternate) ;
	converter->local_alternate = NULL ;
    }
    if (converter->server_converter != NULL) {
        mdn_converter_destroy(converter->server_converter) ;
	converter->server_converter = NULL ;
    }
    if (converter->server_alternate != NULL) {
        mdn_converter_destroy(converter->server_alternate) ;
	converter->server_alternate = NULL ;
    }
    if (converter->local_zld != NULL) {
        free(converter->local_zld) ;
	converter->local_zld = NULL ;
    }
    if (converter->server_zld != NULL) {
        free(converter->server_zld) ;
	converter->server_zld = NULL ;
    }
    free(converter) ;
    
    return ;
}

CONVPTR mdnConvInit(char *local, char *server, char *norm)
{
    CONVPTR conv ;
    PUCHAR  p    ;
    UCHAR   name[64] ;
    UCHAR   enc[64]  ;
    UCHAR   opt[64]  ;
    
    if (server == NULL || strlen(server) == 0) {
        mdnPrintf("mdnConvInit: no server param\n") ;
        return NULL ;
    }
    if ((conv = malloc(sizeof(CONVREC))) == NULL) {
        mdnPrintf("mdnConvInit: allocation failed\n") ;
        return NULL ;
    }
    memset(conv, 0, sizeof(CONVREC)) ;

    /*
     * register normalizers
     */
     
    mdn_normalizer_initialize() ;

    if (mdn_normalizer_create(&conv->normalizer) != mdn_success) {
        mdnPrintf("mdnConvInit: cannot create normalizer\n") ;
        mdnConvDone(conv) ;
	return NULL ;
    }
    for (p = norm ; (p = parseNorm(p, name)) != NULL ;  ) {
        if (mdn_normalizer_add(conv->normalizer, name) != mdn_success) {
	    mdnPrintf("mdnConvInit: cannot add normalizer scheme %s\n", name) ;
	    mdnConvDone(conv) ;
	    return NULL ;
	}
    }
    
    /*
     * create converters, local & server
     */
     
    mdn_converter_initialize()  ;
    
    parseEncode(local, enc, opt) ;
    
    if (mdn_converter_create(enc, &conv->local_converter, 1) != mdn_success) {
        mdnPrintf("mdnConvInit: cannot create local converter\n") ;
        mdnConvDone(conv) ;
	return NULL ;
    }
    if (mdn_converter_create("RACE", &conv->local_alternate, 1) != mdn_success) {
        mdnPrintf("mdnConvInit: cannot create local alternate converter\n") ;
        mdnConvDone(conv) ;
	return NULL ;
    }
    if (mdn_translator_canonicalzld(opt, &conv->local_zld) != mdn_success) {
        mdnPrintf("mdnConvInit: failed to canonicalize local ZLD %s\n", opt) ;
        mdnConvDone(conv) ;
	return NULL ;
    }
    
    parseEncode(server, enc, opt) ;
    
    if (mdn_converter_create(enc, &conv->server_converter, 1) != mdn_success) {
        mdnPrintf("mdnConvInit: cannot create server converter\n") ;
        mdnConvDone(conv) ;
	return NULL ;
    }
    if (mdn_converter_create("RACE", &conv->server_alternate, 1) != mdn_success) {
        mdnPrintf("mdnConvInit: cannot create server alternate converter\n") ;
        mdnConvDone(conv) ;
	return NULL ;
    }
    if (mdn_translator_canonicalzld(opt, &conv->server_zld) != mdn_success) {
        mdnPrintf("mdnConvInit: failed to canonicalize server ZLD %s\n", opt) ;
        mdnConvDone(conv) ;
	return NULL ;
    }

#if 0
    mdnPrintf("Local  Encoding  %s\n", mdn_converter_localencoding(conv->local_converter)) ;
    mdnPrintf("Local  Alternate %s\n", mdn_converter_localencoding(conv->local_alternate)) ;
    mdnPrintf("Server Encoding  %s\n", mdn_converter_localencoding(conv->server_converter)) ;
    mdnPrintf("Server Alternate %s\n", mdn_converter_localencoding(conv->server_alternate)) ;
#endif    
    return conv ;
}

/*
 * mdnConvReq - convert domain name in a DNS request
 *
 *      convert local encoding to DNS encoding
 */
 
BOOL    mdnConvReq(CONVPTR converter, const char FAR *from, char FAR *to, size_t tolen)
{
    mdn_result_t    r ;
    
    if (converter == NULL) {
        if (strlen(from) >= tolen) {
	    return FALSE ;
	}
        strcpy(to, from) ;
	return TRUE ;
    }
        
    r = mdn_translator_translate(
            converter->local_converter, 
	    converter->local_alternate,
	    converter->local_zld,
	    converter->normalizer,
	    converter->server_converter,
	    NULL,
	    converter->server_zld,
	    from, to, tolen) ;

    if (r == mdn_success) {
        return TRUE ;
    } else {
        return FALSE ;
    }
}

/*
 * mdnConvRsp - convert domain name in a DNS response
 *
 *      convert DNS encoding to local encoding
 */

BOOL    mdnConvRsp(CONVPTR converter, const char FAR *from, char FAR *to, size_t tolen)
{
    mdn_result_t    r ;
    
    if (converter == NULL) {
        if (strlen(from) >= tolen) {
	    return FALSE ;
	}
        strcpy(to, from) ;
	return TRUE ;
    }
        
    r = mdn_translator_translate(
	    converter->server_converter,
	    NULL,
	    converter->server_zld,
	    converter->normalizer,
            converter->local_converter, 
            converter->local_alternate, 
	    converter->local_zld,
	    from, to, tolen) ;

    if (r == mdn_success) {
        return TRUE ;
    } else {
        return FALSE ;
    }
}

