/*
 * encoding.c - get DNS/Local encodings
 *
 *      Software\\JPNIC\\MDN\\Where
 *                          \\Encoding
 *                          \\Normalize
 *                          \\LogFile
 *                          \\PerProg\\<name>\\Where
 *                          \\PerProg\\<name>\\Encoding
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
#include <ctype.h>

#include "jpnicmdn.h"

#ifdef  DEBUG
#define TRACE   mdnPrintf("encoding : ") ; mdnPrintf
#define FATAL   mdnPrintf("encoding : ") ; mdnPrintf
#else
#define TRACE
#define FATAL   mdnPrintf("encoding : ") ; mdnPrintf
#endif

/*
 * Registry of Encodings
 */

static  UCHAR   MDNKEY_WRAPPER[] = "Software\\JPNIC\\MDN"          ;
static  UCHAR   MDNKEY_PERPROG[] = "Software\\JPNIC\\MDN\\PerProg" ;
static  UCHAR   MDNVAL_WHERE[]   = "Where" ;
static  UCHAR   MDNVAL_ENCODE[]  = "Encoding" ;
static  UCHAR   MDNVAL_NORM[]    = "Normalize" ;
static  UCHAR   MDNVAL_LOGLVL[]  = "LogLevel" ;
static  UCHAR   MDNVAL_LOGFILE[] = "LogFile" ;

/*
 * mdnEncodeWhere - which module should convert domain name
 */

int     mdnEncodeWhere(void)
{
    HKEY    hk   ;
    LONG    stat ;
    DWORD   dw, type, leng ;
    DWORD   param ;
    PUCHAR  p, last ;
    UCHAR   exename[256] ;
    UCHAR   prgname[256] ;
    UCHAR   keyname[256] ;
    int     where = MDN_ENCODE_ALWAYS ;  /* as normal default */

    /*
     * first try on HKEY_LOCAL_MACHINE
     */
     
    stat = RegCreateKeyEx(HKEY_LOCAL_MACHINE,
                          MDNKEY_WRAPPER, 
			  0,
			  REG_NONE,
			  REG_OPTION_NON_VOLATILE,
			  KEY_READ,
			  NULL,
			  &hk,
			  &dw) ;

    if (stat == ERROR_SUCCESS) {
        type = REG_DWORD ;
	leng = sizeof(param) ;
        stat = RegQueryValueEx(hk, 
	                       MDNVAL_WHERE, 
			       NULL,
			       &type,
			       (LPBYTE) &param,
			       &leng) ;
        RegCloseKey(hk) ;
	
	if (stat == ERROR_SUCCESS) {
            where = (int) param ;
	}
    }
    
    /*
     * May be overwritten HKEY_CURRENT_USER
     */
     
    stat = RegCreateKeyEx(HKEY_CURRENT_USER,
                          MDNKEY_WRAPPER, 
			  0,
			  REG_NONE,
			  REG_OPTION_NON_VOLATILE,
			  KEY_READ,
			  NULL,
			  &hk,
			  &dw) ;

    if (stat == ERROR_SUCCESS) {
        type = REG_DWORD ;
	leng = sizeof(param) ;
        stat = RegQueryValueEx(hk, 
	                       MDNVAL_WHERE, 
			       NULL,
			       &type,
			       (LPBYTE) &param,
			       &leng) ;
        RegCloseKey(hk) ;

	if (stat == ERROR_SUCCESS) {
            where = (int) param ;
	}
    }

    /*
     * also override with program specific setting
     */

    GetModuleFileName(NULL, exename, 256) ;     
     
    for (p = exename, last = NULL ; *p ; p++) {
        if (*p == '/' || *p == '\\') {
            last = p ;
        }
    }
    if (last == NULL) {
        strcpy(prgname, exename) ;
    } else {
        strcpy(prgname, &last[1]) ;
    }
    if ((p = strrchr(prgname, '.')) != NULL) {
        *p = '\0' ;
    }
    sprintf(keyname, "%s\\%s", MDNKEY_PERPROG, prgname) ;

    /*
     * first try on HKEY_LOCAL_MACHINE
     */
     
    stat = RegCreateKeyEx(HKEY_LOCAL_MACHINE,
                          keyname, 
			  0,
			  REG_NONE,
			  REG_OPTION_NON_VOLATILE,
			  KEY_READ,
			  NULL,
			  &hk,
			  &dw) ;

    if (stat == ERROR_SUCCESS) {
        type = REG_DWORD ;
	leng = sizeof(param) ;
        stat = RegQueryValueEx(hk, 
	                       MDNVAL_WHERE,
			       NULL,
			       &type,
			       (LPBYTE) &param,
			       &leng) ;
        RegCloseKey(hk) ;

	if (stat == ERROR_SUCCESS) {
	    where = (int) param ;
	}
    }
    
    /*
     * May be overwritten HKEY_CURRENT_USER
     */
     
    stat = RegCreateKeyEx(HKEY_CURRENT_USER,
                          keyname, 
			  0,
			  REG_NONE,
			  REG_OPTION_NON_VOLATILE,
			  KEY_READ,
			  NULL,
			  &hk,
			  &dw) ;

    if (stat == ERROR_SUCCESS) {
        type = REG_DWORD ;
	leng = sizeof(param) ;
        stat = RegQueryValueEx(hk, 
	                       MDNVAL_WHERE,
			       NULL,
			       &type,
			       (LPBYTE) &param,
			       &leng) ;
        RegCloseKey(hk) ;

	if (stat == ERROR_SUCCESS) {
	    where = (int) param ;
	}
    }

    return where ;
}

/*
 * mdnGetDnsEncoding - refer to DNS Encoding
 */

BOOL    mdnGetDnsEncoding(PUCHAR enc)
{
    HKEY    hk   ;
    LONG    stat ;
    DWORD   dw, type, leng ;
    UCHAR   param[256] ;

    /*
     * first try on HKEY_LOCAL_MACHINE
     */
     
    stat = RegCreateKeyEx(HKEY_LOCAL_MACHINE,
                          MDNKEY_WRAPPER, 
			  0,
			  REG_NONE,
			  REG_OPTION_NON_VOLATILE,
			  KEY_READ,
			  NULL,
			  &hk,
			  &dw) ;

    if (stat == ERROR_SUCCESS) {
        type = REG_SZ ;
	leng = sizeof(param) ;
        stat = RegQueryValueEx(hk, 
	                       MDNVAL_ENCODE,
			       NULL,
			       &type,
			       param,
			       &leng) ;
        RegCloseKey(hk) ;
	
	if (stat == ERROR_SUCCESS) {
	    strcpy(enc, param) ;
	}
    }
    
    /*
     * May be overwritten HKEY_CURRENT_USER
     */
     
    stat = RegCreateKeyEx(HKEY_CURRENT_USER,
                          MDNKEY_WRAPPER, 
			  0,
			  REG_NONE,
			  REG_OPTION_NON_VOLATILE,
			  KEY_READ,
			  NULL,
			  &hk,
			  &dw) ;

    if (stat == ERROR_SUCCESS) {
        type = REG_SZ ;
	leng = sizeof(param) ;
        stat = RegQueryValueEx(hk, 
	                       MDNVAL_ENCODE,
			       NULL,
			       &type,
			       param,
			       &leng) ;
        RegCloseKey(hk) ;

	if (stat == ERROR_SUCCESS) {
	    strcpy(enc, param) ;
	}
    }

    if (stat == ERROR_SUCCESS) {
        return TRUE ;
    } else {
        return FALSE ;
    }
}

/*
 * mdnGetNormalize - refer to normalization schemes
 */

BOOL    mdnGetNormalize(PUCHAR norm)
{
    HKEY    hk   ;
    LONG    stat ;
    DWORD   dw, type, leng ;
    UCHAR   param[256]  ;

    /*
     * first try on HKEY_LOCAL_MACHINE
     */
     
    stat = RegCreateKeyEx(HKEY_LOCAL_MACHINE,
                          MDNKEY_WRAPPER, 
			  0,
			  REG_NONE,
			  REG_OPTION_NON_VOLATILE,
			  KEY_READ,
			  NULL,
			  &hk,
			  &dw) ;

    if (stat == ERROR_SUCCESS) {
        type = REG_SZ ;
	leng = sizeof(param) ;
        stat = RegQueryValueEx(hk, 
	                       MDNVAL_NORM, 
			       NULL,
			       &type,
			       param,
			       &leng) ;
        RegCloseKey(hk) ;
	
	if (stat == ERROR_SUCCESS) {
	    strcpy(norm, param) ;
	}
    }
    
    /*
     * May be overwritten HKEY_CURRENT_USER
     */
     
    stat = RegCreateKeyEx(HKEY_CURRENT_USER,
                          MDNKEY_WRAPPER, 
			  0,
			  REG_NONE,
			  REG_OPTION_NON_VOLATILE,
			  KEY_READ,
			  NULL,
			  &hk,
			  &dw) ;

    if (stat == ERROR_SUCCESS) {
        type = REG_SZ ;
	leng = sizeof(param) ;
        stat = RegQueryValueEx(hk, 
	                       MDNVAL_NORM, 
			       NULL,
			       &type,
			       param,
			       &leng) ;
        RegCloseKey(hk) ;

	if (stat == ERROR_SUCCESS) {
	    strcpy(norm, param) ;
	}
    }

    if (stat == ERROR_SUCCESS) {
        return TRUE ;
    } else {
        return FALSE ;
    }
}

/*
 * mdnGetLogFile - refer to log file
 */

BOOL    mdnGetLogFile(PUCHAR file)
{
    HKEY    hk   ;
    LONG    stat ;
    DWORD   dw, type, leng ;
    UCHAR   param[128] ;
    BOOL    ok = FALSE ;

    /*
     * first try on HKEY_LOCAL_MACHINE
     */
     
    stat = RegCreateKeyEx(HKEY_LOCAL_MACHINE,
                          MDNKEY_WRAPPER, 
			  0,
			  REG_NONE,
			  REG_OPTION_NON_VOLATILE,
			  KEY_READ,
			  NULL,
			  &hk,
			  &dw) ;

    if (stat == ERROR_SUCCESS) {
        type = REG_SZ ;
	leng = sizeof(param) ;
        stat = RegQueryValueEx(hk, 
	                       MDNVAL_LOGFILE, 
			       NULL,
			       &type,
			       param,
			       &leng) ;
        RegCloseKey(hk) ;
	
	if (stat == ERROR_SUCCESS) {
	    strcpy(file, param) ;
	    ok = TRUE;
	}
    }
    
    /*
     * May be overwritten HKEY_CURRENT_USER
     */
     
    stat = RegCreateKeyEx(HKEY_CURRENT_USER,
                          MDNKEY_WRAPPER, 
			  0,
			  REG_NONE,
			  REG_OPTION_NON_VOLATILE,
			  KEY_READ,
			  NULL,
			  &hk,
			  &dw) ;

    if (stat == ERROR_SUCCESS) {
        type = REG_SZ ;
	leng = sizeof(param) ;
        stat = RegQueryValueEx(hk, 
	                       MDNVAL_LOGFILE, 
			       NULL,
			       &type,
			       param,
			       &leng) ;
        RegCloseKey(hk) ;

	if (stat == ERROR_SUCCESS) {
	    strcpy(file, param) ;
	    ok = TRUE;
	}
    }

    return ok ;
}

/*
 * mdnGetPrgEncoding - refer to Program's Local Encoding
 *
 *      use program name as registry key
 */
 
BOOL    mdnGetPrgEncoding(PUCHAR enc)
{
    HKEY    hk   ;
    LONG    stat ;
    DWORD   dw, type, leng ;
    UCHAR   param[256] ;
    PUCHAR  p, last ;
    BOOL    found = FALSE ;
    UCHAR   exename[256] ;
    UCHAR   prgname[256] ;
    UCHAR   keyname[256] ;

    /*
     * use program name as value name
     */

    GetModuleFileName(NULL, exename, 256) ;     
     
    for (p = exename, last = NULL ; *p ; p++) {
        if (*p == '/' || *p == '\\') {
            last = p ;
        }
    }
    if (last == NULL) {
        strcpy(prgname, exename) ;
    } else {
        strcpy(prgname, &last[1]) ;
    }
    if ((p = strrchr(prgname, '.')) != NULL) {
        *p = '\0' ;
    }
    sprintf(keyname, "%s\\%s", MDNKEY_PERPROG, prgname) ;
    mdnPrintf("mdnGetPrgEncoding %s\n", keyname) ;

    /*
     * first try on HKEY_LOCAL_MACHINE
     */
     
    stat = RegCreateKeyEx(HKEY_LOCAL_MACHINE,
                          keyname, 
			  0,
			  REG_NONE,
			  REG_OPTION_NON_VOLATILE,
			  KEY_READ,
			  NULL,
			  &hk,
			  &dw) ;

    if (stat == ERROR_SUCCESS) {
        type = REG_SZ ;
	leng = sizeof(param) ;
        stat = RegQueryValueEx(hk, 
	                       MDNVAL_ENCODE,
			       NULL,
			       &type,
			       param,
			       &leng) ;
        RegCloseKey(hk) ;

	if (stat == ERROR_SUCCESS && strlen(param) > 0) {
	    strcpy(enc, param) ;
	    found = TRUE ;
	}
    }
    
    /*
     * May be overwritten HKEY_CURRENT_USER
     */
     
    stat = RegCreateKeyEx(HKEY_CURRENT_USER,
                          keyname, 
			  0,
			  REG_NONE,
			  REG_OPTION_NON_VOLATILE,
			  KEY_READ,
			  NULL,
			  &hk,
			  &dw) ;

    if (stat == ERROR_SUCCESS) {
        type = REG_SZ ;
	leng = sizeof(param) ;
        stat = RegQueryValueEx(hk, 
	                       MDNVAL_ENCODE,
			       NULL,
			       &type,
			       param,
			       &leng) ;
        RegCloseKey(hk) ;

	if (stat == ERROR_SUCCESS && strlen(param) > 0) {
	    strcpy(enc, param) ;
	    found = TRUE ;
	}
    }
    
    if (found) {
        return TRUE ;
    }

    /*
     * not found in registry, use current codepage
     */

    sprintf(enc, "CP%d", GetACP())  ;
    
    return TRUE ;
}

/*
 * mdnGetLogLevel
 */

int     mdnGetLogLevel(void)
{
    HKEY    hk   ;
    LONG    stat ;
    DWORD   dw, type, leng ;
    DWORD   param ;
    PUCHAR  p, last ;
    UCHAR   exename[256] ;
    UCHAR   prgname[256] ;
    UCHAR   keyname[256] ;
    int     level = 0 ;

    /*
     * first try on HKEY_LOCAL_MACHINE
     */
     
    stat = RegCreateKeyEx(HKEY_LOCAL_MACHINE,
                          MDNKEY_WRAPPER, 
			  0,
			  REG_NONE,
			  REG_OPTION_NON_VOLATILE,
			  KEY_READ,
			  NULL,
			  &hk,
			  &dw) ;

    if (stat == ERROR_SUCCESS) {
        type = REG_DWORD ;
	leng = sizeof(param) ;
        stat = RegQueryValueEx(hk, 
	                       MDNVAL_LOGLVL, 
			       NULL,
			       &type,
			       (LPBYTE) &param,
			       &leng) ;
        RegCloseKey(hk) ;
	
	if (stat == ERROR_SUCCESS) {
            level = (int) param ;
	}
    }
    
    /*
     * May be overwritten HKEY_CURRENT_USER
     */
     
    stat = RegCreateKeyEx(HKEY_CURRENT_USER,
                          MDNKEY_WRAPPER, 
			  0,
			  REG_NONE,
			  REG_OPTION_NON_VOLATILE,
			  KEY_READ,
			  NULL,
			  &hk,
			  &dw) ;

    if (stat == ERROR_SUCCESS) {
        type = REG_DWORD ;
	leng = sizeof(param) ;
        stat = RegQueryValueEx(hk, 
	                       MDNVAL_LOGLVL, 
			       NULL,
			       &type,
			       (LPBYTE) &param,
			       &leng) ;
        RegCloseKey(hk) ;

	if (stat == ERROR_SUCCESS) {
            level = (int) param ;
	}
    }

    /*
     * also override with program specific setting
     */

    GetModuleFileName(NULL, exename, 256) ;     
     
    for (p = exename, last = NULL ; *p ; p++) {
        if (*p == '/' || *p == '\\') {
            last = p ;
        }
    }
    if (last == NULL) {
        strcpy(prgname, exename) ;
    } else {
        strcpy(prgname, &last[1]) ;
    }
    if ((p = strrchr(prgname, '.')) != NULL) {
        *p = '\0' ;
    }
    sprintf(keyname, "%s\\%s", MDNKEY_PERPROG, prgname) ;

    /*
     * first try on HKEY_LOCAL_MACHINE
     */
     
    stat = RegCreateKeyEx(HKEY_LOCAL_MACHINE,
                          keyname, 
			  0,
			  REG_NONE,
			  REG_OPTION_NON_VOLATILE,
			  KEY_READ,
			  NULL,
			  &hk,
			  &dw) ;

    if (stat == ERROR_SUCCESS) {
        type = REG_DWORD ;
	leng = sizeof(param) ;
        stat = RegQueryValueEx(hk, 
	                       MDNVAL_LOGLVL,
			       NULL,
			       &type,
			       (LPBYTE) &param,
			       &leng) ;
        RegCloseKey(hk) ;

	if (stat == ERROR_SUCCESS) {
	    level = (int) param ;
	}
    }
    
    /*
     * May be overwritten HKEY_CURRENT_USER
     */
     
    stat = RegCreateKeyEx(HKEY_CURRENT_USER,
                          keyname, 
			  0,
			  REG_NONE,
			  REG_OPTION_NON_VOLATILE,
			  KEY_READ,
			  NULL,
			  &hk,
			  &dw) ;

    if (stat == ERROR_SUCCESS) {
        type = REG_DWORD ;
	leng = sizeof(param) ;
        stat = RegQueryValueEx(hk, 
	                       MDNVAL_LOGLVL,
			       NULL,
			       &type,
			       (LPBYTE) &param,
			       &leng) ;
        RegCloseKey(hk) ;

	if (stat == ERROR_SUCCESS) {
	    level = (int) param ;
	}
    }

    return level ;
}
