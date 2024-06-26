divert(-1)changequote(<<, >>)<<
-----------------------------------------------------------------------------

                     FEATURE(domainmap) Macro
                                       
   The existing virtusertable feature distributed with sendmail is a good
   basic approach to virtual hosting, but it is missing a few key
   features:

    1. Ability to have a different map for each domain.
    2. Ability to perform virtual hosting for domains which are not in $=w.
    3. Ability to use a centralized network-accessible database (such as
       PH) which is keyed on username alone (as opposed to the
       fully-qualified email address).

   The FEATURE(domainmap) macro neatly solves these problems.
   
   The basic syntax of the macro is:
        FEATURE(domainmap, `domain.com', `map definition ...')dnl

   To illustrate how it works, here is an example:
        FEATURE(domainmap, `foo.com', `dbm -o /etc/mail/foo-users')dnl

   In this example, mail sent to user@foo.com will be rewritten by the
   domainmap. The username will be looked up in the DBM map
   /etc/mail/foo-users, which looks like this:
        jsmith  johnsmith@mailbox.foo.com
        jdoe    janedoe@sandbox.bar.com

   So mail sent to jsmith@foo.com will be relayed to
   johnsmith@mailbox.foo.com, and mail sent to jdoe@foo.com will be
   relayed to janedoe@sandbox.bar.com.
   
   The FEATURE(domainmap) Macro supports the user+detail syntax by
   stripping off the +detail portion before the domainmap lookup and
   tacking it back on to the result. Using the example above, mail sent
   to jsmith+sometext@foo.com will be rewritten as
   johnsmith+sometext@mailbox.foo.com.
   
   If one of the elements in the $=w class (i.e., "local" delivery hosts)
   is a domain specified in a FEATURE(domainmap) entry, you need to use
   the LOCAL_USER(username) macro to specify the list of users for whom
   domainmap lookups should not be done.

   To use this macro, simply copy this file into the cf/feature directory
   in the sendmail source tree.  For more information, please see the
   following URL:

      http://www-wsg.cso.uiuc.edu/sendmail/patches/domainmap.html

   Feedback is welcome.

                                             Mark D. Roth <roth@uiuc.edu>

-----------------------------------------------------------------------------
>>changequote(`, ')undivert(-1)divert

ifdef(`_DOMAIN_MAP_',`',`dnl
LOCAL_RULE_0
# do mapping for domains where applicable
R$* $=O $* <@ $={MappedDomain} .>	$@ $>97 $1 $2 $3	Strip extraneous routing
R$+ <@ $={MappedDomain} .>		$>DomainMapLookup $1 <@ $2 .>	domain mapping

LOCAL_RULESETS
###########################################################################
###   Ruleset DomainMapLookup -- special rewriting for mapped domains   ###
###########################################################################

SDomainMapLookup
R $=L <@ $=w .>		$@ $1 <@ $2 .>		weed out local users, in case
#						Cw contains a mapped domain
R $+ <@ $+ .>		$1 <@ $2 >		strip trailing dot
R $+ <@ $+ . $+ >	$1 <@ $(dequote $2 "_" $3 $) >
#						change "." to "_"
R $+ <@ $+ >		$: $1 <@ $(dequote "domain_" $2 $) >
#						prepend "domain_"
R $+ + $+ <@ $*>	$1 <@ $3 > <+> $2	handle user+list syntax
R $+ <@ $* > $*		$( $2 $1 $: <ERROR> $) $3
#						do actual domain map lookup
R <ERROR> $*		$#error $@ 5.1.1 $: "550 email address lookup in domain map failed"
R $* <TEMP> $*		$#error $@ 4.3.0 $: "450 domain map temporarily unavailable"
R $+ @ $+ <+> $+	$1 + $3 @ $2		reset original user+list
R $+ <+> $*		$1			paranoid check - remove <+>
R $+ @ $+ .		$1 @ $2			strip trailing dot
R $+ @ $+		$@ $>97 $1 @ $2		recanonify
define(`_DOMAIN_MAP_',`1')')

LOCAL_CONFIG
C{MappedDomain} _ARG_
K `domain_'translit(_ARG_, `.', `_') _ARG2_ -T<TEMP>
