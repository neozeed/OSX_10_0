/*
 * dllmain.c - entry for DLL
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

/*
 * Control Variables
 */

int     procPid      =   0   ;
UCHAR   procExe[256] = { 0 } ;

UCHAR   paramLocal[256]    ;
UCHAR   paramServer[256]   ;
UCHAR   paramNorm[256]     ;
BOOL    encodeHere = FALSE ;
CONVPTR encodeCtx  = NULL  ;

/*
 * DLL Entry
 */
 
BOOL    APIENTRY    DllMain(HMODULE hmod, DWORD reason, LPVOID *resv)
{
    switch (reason) {
    case DLL_PROCESS_ATTACH :
        procPid = getpid() ;
        GetModuleFileName(NULL, procExe, 256) ;
	
	TRACE("Attached to Process <%s>\n", procExe) ;

        if (linkLoad() != TRUE) {
	    FATAL("cannot link to original DLL\n") ;
	    return FALSE ;
	}
	
        mdnLogInit() ;	                
	mdnHookInit() ;

        switch (mdnEncodeWhere()) {
        case MDN_ENCODE_ALWAYS :
	case MDN_ENCODE_ONLY20 :
	case MDN_ENCODE_CHECK  :
	    encodeHere = TRUE ;
	    break ;
	default :
	    encodeHere = FALSE ;
	    break ;
        }

	if (encodeHere == FALSE) {
	    TRACE("not encode here\n") ;
	} else {
            mdnGetPrgEncoding(paramLocal)  ;
            mdnGetDnsEncoding(paramServer) ;
            mdnGetNormalize(paramNorm)     ;
            TRACE("Encoding PRG <%s> DNS <%s>\n", paramLocal, paramServer) ;
            TRACE("Normalize <%s>\n", paramNorm) ;
            encodeCtx = mdnConvInit(paramLocal, paramServer, paramNorm) ;
            TRACE("Context %08x\n", encodeCtx) ;
        }
        break ;

    case DLL_PROCESS_DETACH :
        if (encodeCtx != NULL) {
	    mdnConvDone(encodeCtx) ;
	}
	mdnHookDone() ;
        linkFree() ;
	TRACE("Deatched from Process\n") ;
        break ;

    case DLL_THREAD_ATTACH  :
        break ;
    case DLL_THREAD_DETACH  :
        break ;
    }
    return TRUE ;
}
