/* $Id: log.h,v 1.1.1.1 2001/01/31 03:58:58 zarzycki Exp $ */
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

#ifndef MDN_LOG_H
#define MDN_LOG_H 1

/*
 * MDN library logging facility.
 */

/*
 * Log level definition.
 */
enum {
	mdn_log_level_fatal = 0,
	mdn_log_level_error = 1,
	mdn_log_level_warning = 2,
	mdn_log_level_info = 3,
	mdn_log_level_trace = 4,
	mdn_log_level_dump = 5
};

/*
 * Log handler type.
 */
typedef void	(*mdn_log_proc_t)(int level, const char *msg);

/*
 * Log routines.
 */
extern void	mdn_log_fatal(const char *fmt, ...);
extern void	mdn_log_error(const char *fmt, ...);
extern void	mdn_log_warning(const char *fmt, ...);
extern void	mdn_log_info(const char *fmt, ...);
extern void	mdn_log_trace(const char *fmt, ...);
extern void	mdn_log_dump(const char *fmt, ...);

/*
 * Set/get log level.
 *
 * If log level has not been explicitly defined by 'mdn_log_setlevel',
 * the default level is determined by the value of enrironment
 * variable 'MDN_LOG_LEVEL'.
 */
extern void	mdn_log_setlevel(int level);
extern int	mdn_log_getlevel(void);

/*
 * Set log handler.
 *
 * If no log handler is set, log goes to stderr by default.
 * You can reset the handler to the default one by specifying
 * NULL.
 */
extern void	mdn_log_setproc(mdn_log_proc_t proc);

#endif /* MDN_LOG_H */
