divert(-1)
#
# Copyright (c) 1998, 1999 Sendmail, Inc. and its suppliers.
#	All rights reserved.
# Copyright (c) 1983 Eric P. Allman.  All rights reserved.
# Copyright (c) 1988, 1993
#	The Regents of the University of California.  All rights reserved.
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the sendmail distribution.
#
#

divert(0)
VERSIONID(`$Id: mailertable.m4,v 1.1.1.3 2000/06/10 00:40:34 wsanchez Exp $')
divert(-1)

define(`_MAILER_TABLE_', `')

LOCAL_CONFIG
# Mailer table (overriding domains)
Kmailertable ifelse(defn(`_ARG_'), `',
		    DATABASE_MAP_TYPE MAIL_SETTINGS_DIR`mailertable',
		    `_ARG_')
