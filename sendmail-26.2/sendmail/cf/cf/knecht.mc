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

#
#  This is specific to Eric's home machine.
#

divert(0)dnl
VERSIONID(`$Id: knecht.mc,v 1.1.1.3 2000/06/10 00:40:30 wsanchez Exp $')
OSTYPE(bsdi)dnl
DOMAIN(generic)dnl
define(`confFORWARD_PATH', `$z/.forward.$w:$z/.forward+$h:$z/.forward')dnl
define(`confDEF_USER_ID', `mailnull')dnl
define(`confHOST_STATUS_DIRECTORY', `.hoststat')dnl
define(`confTO_ICONNECT', `10s')dnl
define(`confCOPY_ERRORS_TO', `Postmaster')dnl
define(`confTO_QUEUEWARN', `8h')dnl
define(`confTRUSTED_USERS', `www')dnl
define(`confPRIVACY_FLAGS', ``authwarnings,noexpn,novrfy'')dnl
FEATURE(virtusertable)dnl
FEATURE(access_db)dnl
FEATURE(local_lmtp)dnl
MODIFY_MAILER_FLAGS(`LOCAL', `+P')dnl
MAILER(local)dnl
MAILER(smtp)dnl

LOCAL_CONFIG
#
#  Regular expression to reject:
#    * numeric-only localparts from aol.com and msn.com
#    * localparts starting with a digit from juno.com
#
Kcheckaddress regex -a@MATCH
   ^([0-9]+<@(aol|msn)\.com|[0-9][^<]*<@juno\.com)\.?>

#
#  Names that won't be allowed in a To: line (local-part and domains)
#
C{RejectToLocalparts}	friend you
C{RejectToDomains}	public.com

LOCAL_RULESETS
HTo: $>CheckTo

SCheckTo
R$={RejectToLocalparts}@$*	$#error $: "553 Header error"
R$*@$={RejectToDomains}		$#error $: "553 Header error"

HMessage-Id: $>CheckMessageId

SCheckMessageId
R< $+ @ $+ >			$@ OK
R$*				$#error $: "553 Header error"

LOCAL_RULESETS
SLocal_check_mail
# check address against various regex checks
R$*				$: $>Parse0 $>3 $1
R$+				$: $(checkaddress $1 $)
R@MATCH				$#error $: "553 Header error"
