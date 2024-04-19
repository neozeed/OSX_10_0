# $Id: fields.pl,v 1.1.1.1 2001/01/31 03:59:15 zarzycki Exp $
# fields required in each file read with readinfo
define(SOA_FIELDS_R, `domain server contact refresh retry expire min checknames notify also_notify')
define(MAIN_FIELDS_R, `host ip ether hard os ptr ttl')
define(CNAME_FIELDS_R, `alias host ttl')
define(MX_FIELDS_R, `domain priority host ttl')
define(NS_FIELDS_R, `domain server ttl')
define(TXT_FIELDS_R, `domain txt ttl')
define(SECONDARY_FIELDS_R, `domain ip checknames stub')
define(OPTIONS_FIELDS_R, `option opt_value')
define(GLOBAL_FIELDS_R, `directory cache forwarders checknames slave')

# and the corresponding perl list
define(MAIN_FIELDS_P, `($host, $ip, $ether, $hard, $os, $ptr, $ttl)')
define(CNAME_FIELDS_P, `($alias, $host, $ttl)')
define(MX_FIELDS_P, `($domain, $priority, $host, $ttl)')
define(NS_FIELDS_P, `($domain, $server, $ttl)')
define(TXT_FIELDS_P, `($domain, $txt, $ttl)')
define(SOA_FIELDS_P, `($domain, $server, $contact, $refresh, $retry, $expire, $min, $checknames, $notify, $also_notify)')
define(SECONDARY_FIELDS_P, `($domain, $ip, $checknames, $stub)')
define(OPTIONS_FIELDS_P, `($option, $opt_value)')
define(GLOBAL_FIELDS_P, `($directory, $cache, $forwarders, $checknames, $slave)')

%FIELDS = ( "main", "MAIN_FIELDS_R",
	"cname", "CNAME_FIELDS_R",
	"mx", "MX_FIELDS_R",
	"ns", "NS_FIELDS_R",
	"txt", "TXT_FIELDS_R",
	"soa", "SOA_FIELDS_R",
	"secondary", "SECONDARY_FIELDS_R",
	"options", "OPTIONS_FIELDS_R",
	"global", "GLOBAL_FIELDS_R"
);
