divert(-1)
#
# Copyright (c) 1998 Sendmail, Inc.  All rights reserved.
#
# By using this file, you agree to the terms and conditions set
# forth in the LICENSE file which can be found at the top level of
# the sendmail distribution.
#
#

divert(0)
VERSIONID(`$Id: rbl.m4,v 1.1.1.3 2000/06/10 00:40:35 wsanchez Exp $')
divert(-1)

define(`_RBL_', ifelse(defn(`_ARG_'), `', `rbl.maps.vix.com', `_ARG_'))dnl
ifelse(defn(`_ARG_'), `', `', `
errprint(`Warning: FEATURE(`rbl') is deprecated, use FEATURE(`dnsbl') instead
')')dnl
