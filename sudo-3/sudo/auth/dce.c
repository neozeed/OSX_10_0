/*
 * Copyright (c) 1996, 1998, 1999 Todd C. Miller <Todd.Miller@courtesan.com>
 * All rights reserved.
 *
 * This code is derived from software contributed by Jeff Earickson
 * of Colby College, Waterville, ME <jaearick@colby.edu>
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * 4. Products derived from this software may not be called "Sudo" nor
 *    may "Sudo" appear in their names without specific prior written
 *    permission from the author.
 *
 * THIS SOFTWARE IS PROVIDED ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES,
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY
 * AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL
 * THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
/*
 *  The code below basically comes from the examples supplied on
 *  the OSF DCE 1.0.3 manpages for the sec_login routines, with
 *  enough additional polishing to make the routine work with the
 *  rest of sudo.
 *
 *  This code is known to work on HP 700 and 800 series systems
 *  running HP-UX 9.X and 10.X, with either HP's version 1.2.1 of DCE.
 *  (aka, OSF DCE 1.0.3) or with HP's version 1.4 of DCE (aka, OSF
 *  DCE 1.1).
 */

#include "config.h"

#include <stdio.h>
#ifdef STDC_HEADERS
#include <stdlib.h>
#endif /* STDC_HEADERS */
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif /* HAVE_UNISTD_H */
#ifdef HAVE_STRING_H
#include <string.h>
#endif /* HAVE_STRING_H */
#ifdef HAVE_STRINGS_H
#include <strings.h>
#endif /* HAVE_STRINGS_H */
#include <sys/param.h>
#include <sys/types.h>
#include <pwd.h>

#include <dce/rpc.h>
#include <dce/sec_login.h>
#include <dce/dce_error.h> /* required to call dce_error_inq_text routine */

#include "sudo.h"
#include "sudo_auth.h"

#ifndef lint
static const char rcsid[] = "$Sudo: dce.c,v 1.7 1999/08/31 09:39:17 millert Exp $";
#endif /* lint */

static int check_dce_status __P((error_status_t, char *));

int
dce_verify(pw, plain_pw, auth)
    struct passwd *pw;
    char *plain_pw;
    sudo_auth *auth;
{
    struct passwd		temp_pw;
    sec_passwd_rec_t		password_rec;
    sec_login_handle_t		login_context;
    boolean32			reset_passwd;
    sec_login_auth_src_t	auth_src;
    error_status_t		status;

    /*
     * Create the local context of the DCE principal necessary
     * to perform authenticated network operations.  The network
     * identity set up by this operation cannot be used until it
     * is validated via sec_login_validate_identity().
     */
    if (sec_login_setup_identity((unsigned_char_p_t) pw->pw_name,
	sec_login_no_flags, &login_context, &status)) {

	if (check_dce_status(status, "sec_login_setup_identity(1):"))
	    return(AUTH_FAILURE);

	password_rec.key.key_type = sec_passwd_plain;
	password_rec.key.tagged_union.plain = (idl_char *) plain_pw;
	password_rec.pepper = NULL;
	password_rec.version_number = sec_passwd_c_version_none;

	/* Validate the login context with the password */
	if (sec_login_validate_identity(login_context, &password_rec,
	    &reset_passwd, &auth_src, &status)) {

	    if (check_dce_status(status, "sec_login_validate_identity(1):"))
		return(AUTH_FAILURE);

	    /*
	     * Certify that the DCE Security Server used to set
	     * up and validate a login context is legitimate.  Makes
	     * sure that we didn't get spoofed by another DCE server.
	     */
	    if (!sec_login_certify_identity(login_context, &status)) {
		(void) fprintf(stderr, "Whoa! Bogus authentication server!\n");
		(void) check_dce_status(status,"sec_login_certify_identity(1):");
		return(AUTH_FAILURE);
	    }
	    if (check_dce_status(status, "sec_login_certify_identity(2):"))
		return(AUTH_FAILURE);

	    /*
	     * Sets the network credentials to those specified
	     * by the now validated login context.
	     */
	    sec_login_set_context(login_context, &status);
	    if (check_dce_status(status, "sec_login_set_context:"))
		return(AUTH_FAILURE);

	    /*
	     * Oops, your credentials were no good. Possibly
	     * caused by clock times out of adjustment between
	     * DCE client and DCE security server...
	     */
	    if (auth_src != sec_login_auth_src_network) {
		    (void) fprintf(stderr,
			"You have no network credentials.\n");
		    return(AUTH_FAILURE);
	    }
	    /* Check if the password has aged and is thus no good */
	    if (reset_passwd) {
		    (void) fprintf(stderr,
			"Your DCE password needs resetting.\n");
		    return(AUTH_FAILURE);
	    }

	    /*
	     * We should be a valid user by this point.  Pull the
	     * user's password structure from the DCE security
	     * server just to make sure.  If we get it with no
	     * problems, then we really are legitimate...
	     */
	    sec_login_get_pwent(login_context, (sec_login_passwd_t) &temp_pw,
		&status);
	    if (check_dce_status(status, "sec_login_get_pwent:"))
		return(AUTH_FAILURE);

	    /*
	     * If we get to here, then the pwent above properly fetched
	     * the password structure from the DCE registry, so the user
	     * must be valid.  We don't really care what the user's
	     * registry password is, just that the user could be
	     * validated.  In fact, if we tried to compare the local
	     * password to the DCE entry at this point, the operation
	     * would fail if the hidden password feature is turned on,
	     * because the password field would contain an asterisk.
	     * Also go ahead and destroy the user's DCE login context
	     * before we leave here (and don't bother checking the
	     * status), in order to clean up credentials files in
	     * /opt/dcelocal/var/security/creds.  By doing this, we are
	     * assuming that the user will not need DCE authentication
	     * later in the program, only local authentication.  If this
	     * is not true, then the login_context will have to be
	     * returned to the calling program, and the context purged
	     * somewhere later in the program.
	     */
	    sec_login_purge_context(&login_context, &status);
	    return(AUTH_SUCCESS);
	} else {
	    if(check_dce_status(status, "sec_login_validate_identity(2):"))
		return(AUTH_FAILURE);
	    sec_login_purge_context(&login_context, &status);
	    if(check_dce_status(status, "sec_login_purge_context:"))
		return(AUTH_FAILURE);
	}
    }
    (void) check_dce_status(status, "sec_login_setup_identity(2):");
    return(AUTH_FAILURE);
}

/* Returns 0 for DCE "ok" status, 1 otherwise */
static int
check_dce_status(input_status, comment)
    error_status_t input_status;
    char *comment;
{
    int error_stat;
    unsigned char error_string[dce_c_error_string_len];

    if (input_status == rpc_s_ok)
	return(0);
    dce_error_inq_text(input_status, error_string, &error_stat);
    (void) fprintf(stderr, "%s %s\n", comment, error_string);
    return(1);
}
