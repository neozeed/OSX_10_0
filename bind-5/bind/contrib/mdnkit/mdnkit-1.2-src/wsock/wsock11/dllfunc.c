/*
 * dllfunc.c - wrapper functions
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
#include <process.h>

#include "dlldef.h"

int 
PASCAL FAR wrapGethostname(char FAR * name, int namelen)
{
    int     ret ;
    
    TRACE("ENTER gethostname\n") ;
    ret = (*fpGethostname) (name, namelen) ;
    TRACE("LEAVE gethostname %d <%s>\n", ret, name) ;

    return ret ;
}

struct hostent FAR * 
PASCAL FAR wrapGethostbyname(const char FAR * name)
{
    struct hostent FAR *ret ;
    char    nbuff[64]  ;
    char    hbuff[256] ;
    BOOL    stat ;
    
    TRACE("ENTER gethostbyname <%s>\n", (name != NULL ? name : "NULL")) ;

    if (encodeHere != TRUE) {
        ret = (*fpGethostbyname) (name) ;
    } else if (name == NULL) {
        ret = (*fpGethostbyname) (name) ;
    } else {
        stat = mdnConvReq(encodeCtx, name, nbuff, sizeof(nbuff)) ;
	if (stat == FALSE) {
	    TRACE("mdnConvReq failed\n") ;
	    ret = NULL ;
	} else {
	    TRACE("Converted Name <%s>\n", dumpName(nbuff, hbuff)) ;
            ret = (*fpGethostbyname) (nbuff) ;
        }
    }

    if (ret != NULL && encodeHere) {
        TRACE("Resulting Name <%s>\n", dumpName(ret->h_name, hbuff)) ;
        stat = mdnConvRsp(encodeCtx, ret->h_name, nbuff, sizeof(nbuff)) ;
	if (stat == FALSE) {
	    TRACE("mdnConvRsp failed\n") ;
	    ret = NULL ;
	} else {
            TRACE("Converted Back <%s>\n", dumpName(nbuff, hbuff)) ;
            strcpy(ret->h_name, nbuff) ;
        }
    }

    if (ret == NULL) {
        TRACE("LEAVE gethostbyname NULL\n") ;
    } else {
        TRACE("LEAVE gethostbyname <%s>\n", dumpHost(ret, hbuff)) ;
    }
    return ret ;
}

struct hostent FAR * 
PASCAL FAR wrapGethostbyaddr(const char FAR * addr, int len, int type)
{
    struct hostent FAR *ret ;
    char    nbuff[64]  ;
    char    abuff[256] ;
    char    hbuff[256] ;
    BOOL    stat ;
    
    TRACE("ENTER gethostbyaddr <%s>\n", dumpAddr(addr, len, abuff)) ;

    ret = (*fpGethostbyaddr) (addr, len, type) ;

    if (ret != NULL && encodeHere) {
        TRACE("Resulting Name <%s>\n", dumpName(ret->h_name, hbuff)) ;
        stat = mdnConvRsp(encodeCtx, ret->h_name, nbuff, sizeof(nbuff)) ;
	if (stat == FALSE) {
	    TRACE("mdnConvRsp failed\n") ;
	    ret = NULL ;
	} else {
            TRACE("Converted Back <%s>\n", dumpName(nbuff, hbuff)) ;
	    strcpy(ret->h_name, nbuff) ;
	}
    }

    if (ret == NULL) {
        TRACE("LEAVE gethostbyaddr NULL\n") ;
    } else {
        TRACE("LEAVE gethostbyaddr <%s>\n", dumpHost(ret, hbuff)) ;
    }    
    return ret ;
}

HANDLE
PASCAL FAR wrapWSAAsyncGetHostByName(HWND hWnd, u_int wMsg, 
        const char FAR * name, char FAR * buf, int buflen)
{
    HANDLE  ret ;
    char    nbuff[64]  ;
    char    hbuff[256] ;
    
    TRACE("ENTER WSAAsyncGetHostByName <%s>\n", name) ;

    if (encodeHere != TRUE) {
        ret = (*fpWSAAsyncGetHostByName) (hWnd, wMsg, name, buf, buflen) ;
    } else if (name == NULL) {
        ret = (*fpWSAAsyncGetHostByName) (hWnd, wMsg, name, buf, buflen) ;
    } else {
        mdnHook(hWnd, wMsg, buf, encodeCtx) ;
	mdnConvReq(encodeCtx, name, nbuff, sizeof(nbuff)) ;
	TRACE("Converted Name <%s>\n", dumpName(nbuff, hbuff)) ;
        ret = (*fpWSAAsyncGetHostByName) (hWnd, wMsg, nbuff, buf, buflen) ;
    }

    TRACE("LEAVE WSAAsyncGetHostByName HANDLE %08x\n", ret) ;

    return ret ;
}

HANDLE 
PASCAL FAR wrapWSAAsyncGetHostByAddr(HWND hWnd, u_int wMsg,
        const char FAR * addr, int len, int type, char FAR * buf, int buflen)
{
    HANDLE  ret ;
    char    abuff[256] ;
    
    if (encodeHere) {
        mdnHook(hWnd, wMsg, buf, encodeCtx) ;
    }

    TRACE("ENTER WSAAsyncGetHostByAddr <%s>\n", dumpAddr(addr, len, abuff)) ;
    ret = (*fpWSAAsyncGetHostByAddr) (hWnd, wMsg, addr, len, type, buf, buflen) ;
    TRACE("LEAVE WSAAsyncGetHostByAddr HANDLE %08x\n", ret) ;

    return ret ;
}


