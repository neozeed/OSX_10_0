divert(-1)
#
# Copyright (c) 1998, 1999 Sendmail, Inc. and its suppliers.
#	All rights reserved.
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the sendmail distribution.
#
#

divert(0)
VERSIONID(`$Id: access_db.m4,v 1.1.1.3 2000/06/10 00:40:33 wsanchez Exp $')
divert(-1)

define(`_ACCESS_TABLE_', `')
define(`_TAG_DELIM_', `:')dnl should be in OperatorChars

LOCAL_CONFIG
# Access list database (for spam stomping)
Kaccess ifelse(defn(`_ARG_'), `',
	       DATABASE_MAP_TYPE MAIL_SETTINGS_DIR`access',
	       `_ARG_')
