/*
 *  Copyright (c) 1999-2000 Sendmail, Inc. and its suppliers.
 *	All rights reserved.
 *
 * By using this file, you agree to the terms and conditions set
 * forth in the LICENSE file which can be found at the top level of
 * the sendmail distribution.
 *
 */

#ifndef lint
static char id[] = "@(#)$Id: main.c,v 1.1.1.1 2000/06/10 00:40:50 wsanchez Exp $";
#endif /* ! lint */

#if _FFR_MILTER
#define _DEFINE	1
#include "libmilter.h"
#include <fcntl.h>
#include <sys/stat.h>

static smfiDesc_ptr smfi = NULL;

/*
**  SMFI_REGISTER -- register a filter description
**
**	Parameters:
**		smfilter -- description of filter to register
**
**	Returns:
**		MI_SUCCESS/MI_FAILURE
*/

int
smfi_register(smfilter)
	smfiDesc_str smfilter;
{
	size_t len;

	if (smfi == NULL)
	{
		smfi = (smfiDesc_ptr) malloc(sizeof *smfi);
		if (smfi == NULL)
			return MI_FAILURE;
	}
	(void)memcpy(smfi, &smfilter, sizeof *smfi);
	if (smfilter.xxfi_name == NULL)
		smfilter.xxfi_name = "Unknown";

	len = strlen(smfilter.xxfi_name) + 1;
	smfi->xxfi_name = (char *) malloc(len);
	if (smfi->xxfi_name == NULL)
		return MI_FAILURE;
	(void) strlcpy(smfi->xxfi_name, smfilter.xxfi_name, len);
	return MI_SUCCESS;
}

static int dbg = 0;
static char *conn = NULL;
static int timeout = MI_TIMEOUT;

int
smfi_setdbg(odbg)
	int odbg;
{
	dbg = odbg;
	return MI_SUCCESS;
}

int
smfi_settimeout(otimeout)
	int otimeout;
{
	timeout = otimeout;
	return MI_SUCCESS;
}

int
smfi_setconn(oconn)
	char *oconn;
{
	size_t l;

	if (oconn == NULL || *oconn == '\0')
		return MI_FAILURE;
	l = strlen(oconn) + 1;
	if ((conn = (char *) malloc(l)) == NULL)
		return MI_FAILURE;
	if (strlcpy(conn, oconn, l) >= l)
		return MI_FAILURE;
	return MI_SUCCESS;
}

int
smfi_main()
{
	signal(SIGPIPE, SIG_IGN);
	if (conn == NULL)
	{
		smi_log(SMI_LOG_FATAL, "%s: missing connection information",
			smfi->xxfi_name);
		exit(EX_DATAERR);
	}

	(void) atexit(mi_clean_signals);
	if (mi_control_startup(smfi->xxfi_name) != MI_SUCCESS)
	{
		smi_log(SMI_LOG_FATAL,
			"%s: Couldn't start signal thread",
			smfi->xxfi_name);
		exit(EX_OSERR);
	}

	/* Startup the listener */
	if (mi_listener(conn, dbg, smfi, timeout) != MI_SUCCESS)
		return(MI_FAILURE);

	return(MI_SUCCESS);
}
#endif /* _FFR_MILTER */
