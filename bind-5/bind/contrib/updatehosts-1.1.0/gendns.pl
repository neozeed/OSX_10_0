#! @PERL@
# $Id: gendns.pl,v 1.1.1.1 2001/01/31 03:59:15 zarzycki Exp $

# updatehosts DNS maintenance package
# Copyright (C) 1998-2000  Smoot Carl-Mitchell
# 
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License
# as published by the Free Software Foundation; either version 2
# of the License, or (at your option) any later version.
# 
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
# 
# smoot@tic.com

# generate the DNS database files from generic database files

# files used
#	<dir>/soa
#	<dir>/main
#	<dir>/cname
#	<dir>/mx
#	<dir>/ns
#	<dir>/txt
#	<dir>/secondary
#	<dir>/global
#
# this script assumes that a subdomain is always delegated to
# another nameserver and hosts for the subdomain, except for the
# glue records, do not exist in the base host database files

undefine(`shift')

require "getopts.pl";

# common variables for readinfo format data
include(`fields.pl')

# determine a zone filename
sub zonefile {
	local($domain) = shift;

	if ($domain !~ /.*\.in-addr\.arpa/i) {
		$Rev = 0;
		return $domain;
	}
	else {
		$Rev = 1;
		$Unreverse = unrev($domain);
		# change subnet / to a - if present
		$Unreverse =~ s/\//-/;
		return "f.$Unreverse";
	}
}

# check for CNAME records at populated nodes and remove them
sub check_cname {
	# build a list of all the domain names in the database
	local(%d) = ();
	local($i, $d, $e, @rest);
	local($soa, $main, $ns, $mx);
	for $soa (@Soa) {
		($d, @rest) = split("\t", $soa);
		$d{"\L$d\E"} = 1;
	}
	for $main (@Main) {
		($d, @rest) = split("\t", $main);
		$d{"\L$d\E"} = 1;
	}
	for $ns (@Ns) {
		($d, @rest) = split("\t", $ns);
		$d{"\L$d\E"} = 1;
	}
	for $mx (@Mx) {
		($d, @rest) = split("\t", $mx);
		$d{"\L$d\E"} = 1;
	}
	# see if the domain of the CNAME records match
	local($host);
	local($invalidcname) = 0;
	for ($i=0; $i<=$#Cname; $i++) {
		($d, $host) = split("\t", $Cname[$i]);
		$e = "\L$d\E";
		if ($d{$e}) {
			$invalidcname++;
			print STDERR "ERROR - $d CNAME $host - record invalid - deleted\n" if $Error_Level > 0;
			$Cname[$i] = "";
		}
	}
	# close the holes in the cname list
	if ($invalidcname > 0) {
		local($n) = $#Cname;
		for ($i=0; $i<$n; $i++) {
			if ($Cname[$i] eq "") {
				$Cname[$i] = $Cname[$i+1];
				$Cname[$i+1] = "";
			}
		}
	}
	return;
}

# check for bad ip addresses
sub check_ip {

	local(@ip) = ();
	local(%ip) = ();
	local($main) = "";
	local($invalida) = 0;
	local($err) = 0;
	for $main (@Main) {
		local MAIN_FIELDS_P = split(/\t/, $main);

		# skip dynamic addresses
		next if $ip =~ /dynamic/;

		# build the ip address list
		# don't care about duplicate IPs if no PTR record generated
		$ip{$ip}++ if $ptr ne "no";

		@octets = split(/\./, $ip);
		$err = 0;
		if ($#octets != 3) {
			$err++;
		}
		else {
			for $octet (@octets) { 
				$err++ if $octet > 255 || $octet < 0;
			}
		}
		if ($err) {
			$invalida += $err;
			print STDERR "ERROR $host A $ip - bad IP address - record deleted\n" if $Error_Level > 0;
			$main = "";
		}
	}

	# warn about duplicate IP addresses
	local($key, $value);
	while (($key, $value) = each(%ip)) {
		print STDERR "WARNING - duplicate IP address - $key\n" if $value > 1 && $Error_Level > 1;
	}

	# close the holes in the main list if there are invalid A records
	if ($invalida > 0) {
		local($n) = $#Main;
		local($i);
		for ($i=0; $i<$n; $i++) {
			if ($Main[$i] eq "") {
				$Main[$i] = $Main[$i+1];
				$Main[$i+1] = "";
			}
		}
	}
}

# check for duplicate hostnames
sub check_duphosts {

	local($err);
	local(@ip);
	local($main);
	local(%host) = ();
	for $main (@Main) {
		local MAIN_FIELDS_P = split(/\t/, $main);

		# skip dynamic addresses
		next if $ip =~ /dynamic/;
		
		# build the host list
		$host{$host}++;
	}

	local($key, $value);
	while (($key, $value) = each(%host)) { 
		print STDERR "WARNING - duplicate hostname - $key\n" if $value > 1 && $Error_Level > 1;
	}
}

# check for non-numeric ttl value
sub check_ttl {
	local($domain) = shift;
	local($file) = shift;
	local($ttl) = shift;

	# must be a number if it exists at all
	if ($ttl && $ttl !~ /^\d\d*$/) {
		print STDERR "ERROR - ttl field $ttl for $domain in file $file non-numeric - using default value\n" if $Error_Level > 0;
		return 0;
	}
	return 1;
}

# see if an IP address is in a reverse domain

sub reverse_domain {
	local($ip) = shift;

	local($i, $ip1);
	local($domain);
	local($value);

	# exact match
	if ($Unreverse{$ip}) {
		$partsindomain = 4;
		return $domain[$Unreverse{$ip}];
	}

	@ipparts = split(/\./, $ip);
	# one down match
	pop(@ipparts);
	$ip1 = join(".", @ipparts);
	if ($Unreverse{$ip1}) {
		$partsindomain = 3;
		return $domain[$Unreverse{$ip1}];
	}

	# two down match
	pop(@ipparts);
	$ip1 = join(".", @ipparts);
	if ($Unreverse{$ip1}) {
		$partsindomain = 2;
		return $domain[$Unreverse{$ip1}];
	}

	# three down match
	pop(@ipparts);
	$ip1 = join(".", @ipparts);
	if ($Unreverse{$ip1}) {
		$partsindomain = 1;
		return $domain[$Unreverse{$ip1}];
	}

	# must not be a match
	return "";
}

# see if a domain name is a part of a domain in the SOA list
sub forward_domain {
	local($host) = shift;

	local($domain);
	local($rest);
	local(@domainparts);

	# make the name all lowercase
	$host = "\L$host\E";

	# first an exact match
	return $host if $domain{$host};

	# return null if the domain is in the secondary list
	return "" if $Secondary{$host}; 

	@domainparts = split(/\./, $host);

	# now up the tree
	for ($i=0; $i<=$#domainparts; $i++) {
		shift(@domainparts);
		$domain = join(".", @domainparts);
		return $domain if $domain{$domain};
	}

	return "";
}

# make the secondary zone bootstrap entries
sub boot_sec {
	local($filename);
	for (local($i)=0; $i<=$#Secondary; $i++) {
		local SECONDARY_FIELDS_P = split("\t", $Secondary[$i]);

		local($zone_type) = "slave";
		$zone_type = "stub" if $stub;
		# skip if domain in soa file
		next if $Secondary{"\L$domain\E"} == 0;

		if ($domain !~ /.*\.in-addr\.arpa/i) {
			$filename = $domain;
		}
		else {
			$filename = unrev($domain);
			$filename = "f.$filename";
		}
		# version 8
		print BOOT8 "zone \"$domain\" {\n";
		print BOOT8 "\ttype $zone_type;\n";
		# replace "," with ";"
		$ip =~ s/,/;/;
		print BOOT8 "\tmasters { $ip; };\n";
		print BOOT8 "\tfile \"$filename\";\n";
		print BOOT8 "\tcheck-names $checknames;\n" if $checknames;
		print BOOT8 "};\n";

		# version 4
		print BOOT4 "secondary $domain $ip $filename\n";
	}
}

sub lock {
	if ( -f "$Dir/LOCK") {
		open(LCK, "$Dir/LOCK") || die "cannot open $Dir/LOCK\n";
		$pid = <LCK>;
		close(LCK);
		chomp($pid);
		return 0 if kill(0, $pid);
	}
	open(LCK, ">$Dir/LOCK") || die "cannot open $Dir/LOCK\n";
	print LCK "$$\n";
	close(LCK);
	return 1;
}

sub unlock {
	unlink("$Dir/LOCK");
}

# generate unreverse IP address from in-addr.arpa domain
sub unrev {
	local($d) = shift;

	# unreverse the domain name for matching IP addresses
	local(@p) = split(/\./, $d);
	# delete the in-addr.arpa
	$#p -= 2;

	return join(".", reverse(@p));
}

# get a dynamic IP address from the DHCP lease file
%Ether = ();

sub getdynamicip {
	local($Ether)= shift;

	local(*IN);
	local($dum);
	local($ip);
	local($ether1);
	# read in the leases if we have not already done so
	if (! %Ether) {
		print STDERR "reading the $Leasefile -" if $Verbose;
		print STDERR " found dynamic address for " if $Verbose;
		open(IN, "<$Leasefile") || return "";
		while (<IN>) {
			chomp;
			if (/^lease/) {
				($dum, $ip, $dum) = split;
				next;
			}
			if (/\thardware/) {
				local($dum, $dum, $ether1) = split;
				chomp($ether1);

				# convert hardware address to canonical format
				$ether1 = cvttohex($ether1);
				$Ether{$ether1} = $ip;
				print STDERR "$ether1 " if $Verbose;
			}
		}
		close(IN);
		print STDERR "\n" if $Verbose;
	}

	# see if there is a mapping
	return $Ether{$ether};
}

# convert an Ethernet address in xx:xx:xx:xx:xx:xx format to hex
sub cvttohex {
	local($hard) = shift;

	local(@byte) = split(/:/, $hard);

	local($byte);
	local($out) = "";
	for $byte (@byte) {
		$out .= sprintf("%2.2X", hex($byte));
	}
	return $out;
}

# check the FIELDS line for the correct fields names in each database files
sub check_db_file_syntax {
	local($dir) = shift;

	local($file);
	local($fields);
	local(@fields);
	local($ret) = 1;

	for $file (keys(%FIELDS)) {

		# first check if file is readable
		next if ! -r "$dir/$file";

		$fields = $FIELDS{$file};

		# just do a readinfo and check the STDERR output for errors
		open(IN, "readinfo $fields <$dir/$file 2>&1 1>/dev/null |");
		local($line) = <IN>;
		if ($line =~ /\*\*\*ERROR\*\*\*/) {
			chomp($line);
			local($fieldname) = $line;
			$fieldname =~ s/.* //;
			print STDERR "ERROR - field $fieldname not in file $file\n" if $Error_Level > 0;
			$ret = 0;
		}
		close(IN);
	}
	return $ret;
}

# strip domain name off fully qualified names
sub strip_domain {
	local ($domain) = shift;
	local ($origin_domain) = shift;

	# add a dot to signify name is fully qualified, since readinfo strips
	# dots from fully qualified names (i.e., removes "no=" character)
	$domain .= ".";

	# do case-insensitive match
	$domain =~ s/\.$origin_domain\.$//i;

	return $domain;
}

sub boot_header {

	local($i);
	
	# process all the options
	# fold the global file options into the Options array
	local($directory_option) = 0;
	local($checknames_option) = 0;
	local($forwarders_option) = 0;

	for ($i=0; $i<$#Options; $i++) {
		OPTIONS_FIELDS_P = split(/\t/, $Options[$i]);
		$directory_option++ if $option eq "directory";
		$checknames_option++ if $option eq "check-names";
		$forwarders_option++ if $option eq "forwarders";
	}
	push(@Options, "directory\t$directory") if ! $directory_option && $directory;
	push(@Options, "check_names\t$checknames") if ! $checknames_option && $checknames;
	push(@Options, "forwarders\t$forwarders") if ! $forwarders_option && $forwarders;
	if ($#Options >= 0) {
		print BOOT8 "options {\n";
		for ($i=0; $i<=$#Options; $i++) {
			OPTIONS_FIELDS_P = split(/\t/, $Options[$i]);
			# reformat a few options
			if ($option eq "allow-query" ||
			    $option eq "forwarders" ||
			    $option eq "allow-recursion" ||
			    $option eq "allow-transfer" ||
			    $option eq "blackhole" ||
			    $option eq "topology" ||
			    $option eq "also-notify" ||
			    $option eq "sortlist") {
	 			$opt_value =~ s/,/;/g;
				print BOOT8 "\t$option { $opt_value; };\n";
			}
			elsif ($option eq "rrset-order") {
				$opt_value =~ s/(.*\s+)(name)(\s+)(\S+)(.*)/$1$2$3"$4"$5/;
				$opt_value =~ s/,/;/g;
				print BOOT8 "\t$option { $opt_value; };\n";
			}
			elsif ($option eq "listen-on") {
				print BOOT8 "\t$option ";
				if ($opt_value =~ /\s*port\s+/) {
					$opt_value =~ s/\s*port\s+//;
					print BOOT8 "port ";
					$opt_value =~ /([0-9]+)(\s*)(.*)/;
					print BOOT8 "$1 ";
					$opt_value = $3;
				}
				if ($opt_value) {
					$opt_value =~ s/,/;/g;
					print BOOT8 "{ $opt_value;};";
				}
				print BOOT8 "\n";
			}
			# quoted strings
			elsif ($option eq "directory" ||
			       $option eq "named-xfer" ||
                	       $option eq "dump-file" ||
                	       $option eq "memstatistics-file" ||
                	       $option eq "pid-file" ||
			       $option eq "statistics-file") {
				print BOOT8 "\t$option \"$opt_value\";\n";
			}
			elsif ($option eq "check-names") {
				$opt_value =~ s/^/(/;
				$opt_value =~ s/,/)(/;
				$opt_value =~ s/$/)/;
				print BOOT8 "\t$option $opt_value;\n";
			}
			else {
				print BOOT8 "\t$option $opt_value;\n";
			}
		}
		print BOOT8 "};\n";
	}

	# version 4 bootstrap file
	print BOOT4 "directory $directory\n" if $directory;
	print BOOT4 "check-names primary $checknames\n" if $checknames;
	print BOOT4 "forwarders $forwarders\n" if $forwarders;

}

sub boot_trailer {

	# slave server
	# XXX is there a version 8 command for this?
	print BOOT4 "slave\n" if $slave eq "yes";

	# and the cache directive
	print BOOT8 "zone \".\" {\n";
	print BOOT8 "\ttype hint;\n";
	print BOOT8 "\tfile \"$cache\";\n";
	print BOOT8 "};\n";

	print BOOT4 "cache . $cache\n";
}

# bootstrap file entry for a zone
sub boot_entry {

	print BOOT8 "zone \"$domain\" {\n";
	print BOOT8 "\ttype master;\n";
	print BOOT8 "\tfile \"$filename{$domain}\";\n";
	print BOOT8 "\tcheck-names $checknames;\n" if $checknames;
	print BOOT8 "\tnotify $notify;\n" if $notify;
	# add IP addresses of other secondary servers
	if ($notify eq "yes" && $also_notify) {
		# IP addresses are separated with commas
		local(@ipaddr) = split(/,/, $also_notify);
		print BOOT8 "\talso-notify {";
		local($ipaddr);
		for $ipaddr (@ipaddr) {
			print BOOT8 "$ipaddr;";
		}
		print BOOT8 "};\n";
	}
	print BOOT8 "};\n";

	# version 4 bootstrap
	print BOOT4 "primary $domain $filename{$domain}\n";
}

# main program
# get commmand line options
Getopts("Fd:e:l:v");
$Dir = "db";
$Dir = $opt_d if $opt_d;
$Leasefile = "/etc/dhcpd.leases";
$Leasefile = $opt_l if $opt_l;
$Verbose = $opt_v;
$Force_Zones = $opt_F; # forces a new zone file with updated serial field

# set error level - default is 2
# 0 - suppress all error messages
# 1 -  show only errors
# 2 -  show errors and warnings
$Error_Level = $opt_e;
$Error_Level = 1 unless $Error_Level;

# sanity checking
if (! -d "$Dir") {
	print STDERR "directory $Dir does not exist\n";
	exit 1;
}

# check for a running gendns
if (! lock) {
	print STDERR "another gendns is running\n";
	exit;
}

# if the syntax of the FIELDS line in each file is wrong just abort
check_db_file_syntax($Dir) || exit 1;

# serial number generation
($sec, $min, $hour, $mday, $mon, $year, $wday, $yday, $isdst) = localtime;
$mon++;
# last two digits are 0-99 which divides the day into about 15 min intervals
$hour = ($hour*60+$min)/15;
# the year gets returned as yy instead of yyyy
# Y2K soft bug: Serial number is arbitrary, so it won't break anything;
# but it will look funny.
# Actually, slave servers may not know to update if serial number on master
# is suddenly lower.
$year += 1900;
$Dateserial = sprintf("%4.4d%2.2d%2.2d%2.2d", $year, $mon, $mday, $hour);

# preload all the database files
print STDERR "preloading the database files..." if $Verbose;
open(IN , "readinfo SOA_FIELDS_R <$Dir/soa|"); @Soa = <IN>;chomp(@Soa); close(IN); print STDERR "soa " if $Verbose;
open(IN , "readinfo MAIN_FIELDS_R <$Dir/main|"); @Main = <IN>;chomp(@Main); close(IN); print STDERR "main " if $Verbose;
open(IN , "readinfo CNAME_FIELDS_R <$Dir/cname|"); @Cname = <IN>;chomp(@Cname); close(IN); print STDERR "cname " if $Verbose;
open(IN , "readinfo MX_FIELDS_R <$Dir/mx|"); @Mx = <IN>;chomp(@Mx); close(IN); print STDERR "mx " if $Verbose;
open(IN , "readinfo NS_FIELDS_R <$Dir/ns|"); @Ns = <IN>;chomp(@Ns); close(IN); print STDERR "ns " if $Verbose;
open(IN , "readinfo TXT_FIELDS_R <$Dir/txt|"); @Txt = <IN>;chomp(@Txt); close(IN); print STDERR "txt " if $Verbose;
open(IN , "readinfo SECONDARY_FIELDS_R <$Dir/secondary|"); @Secondary = <IN>;chomp(@Secondary); close(IN); print STDERR "secondary " if $Verbose;
open(IN , "readinfo OPTIONS_FIELDS_R <$Dir/options|"); @Options = <IN>;chomp(@Options); close(IN); print STDERR "options " if $Verbose;
open(IN , "readinfo GLOBAL_FIELDS_R <$Dir/global|"); @Global = <IN>;chomp(@Global); close(IN); print STDERR "global " if $Verbose;

# split out the global file if it existed
if ($#Global > -1) {
# check to be sure it is exactly one line long 
	if ($#Global != 0) {
		print STDERR "global is not exactly one line long\n";
		exit 1;
	}
	GLOBAL_FIELDS_P = split(/\t/, $Global[0]);
}
print STDERR "\n" if $Verbose;

# build  an associative array for the secondary domains
for $secondary (@Secondary) {
	local SECONDARY_FIELDS_P = split(/\t/, $secondary);
	$domain = "\L$domain\E";
	$Secondary{$domain} = 1;
}

unlink("named.boot");
unlink("named.conf");
open(BOOT8, "> named.conf") || die "cannot open named.conf\n";
open(BOOT4, "> named.boot") || die "cannot open named.boot\n";
boot_header(BOOT8, BOOT4);

# check for invalid IP addresses and remove them
print STDERR "checking for invalid IP addresses...\n" if $Verbose;
check_ip;

# check for duplicate hostnames
print STDERR "checking for duplicate hostnames...\n" if $Verbose;
check_duphosts;

# check for CNAME records at populated nodes and remove them
print STDERR "checking for invalid CNAME records...\n" if $Verbose;
check_cname;

$n_zones = $#Soa+1;
# if we have a list of domains to update, just use them

print STDERR "generating files for $n_zones zones\n" if $Verbose;

print STDERR "reading SOA information\n" if $Verbose;
# read SOA info for each domain
for ($i=0; $i<=$#Soa; $i++) {
	local SOA_FIELDS_P = split(/\t/, $Soa[$i]);

	# save the domain case for the $ORIGIN line
	$origin_domain = $domain;
	# lowercase the domain
	$domain = "\L$domain\E";


	# check if it is in the secondary list
	if ($Secondary{$domain}) {
		print STDERR "WARNING domain $domain is also in the soa file - deleted from secondary domain list\n" if $Error_Level > 1;
		$Secondary{$domain} = 0;
	}

	# get the filename for this zone
	$filename{$domain} = zonefile($domain);
	$Unreverse[$i] = "";
	if ($Rev == 1) {
		# if there is a "-" in the name then it is a Class C on non-byte subnet boundaries
		# set the unreverse to the Class C network address
		if ($Unreverse =~ /-/) {
			$Unreverse =~ s/([0-9][0-9]*\.[0-9][0-9]*\.[0-9][0-9]*)(\..*)/$1/;
		}
		$Unreverse[$i] = $Unreverse;
		$Unreverse{$Unreverse} = $i;
	}

	# write the bootstrap file entry for the domain
	boot_entry;

	$domain[$i] = $domain;
	$domain{$domain} = $i;

	# save the header and SOA record
	$domain{$domain} = "\$ORIGIN $origin_domain.\n";
	$domain{$domain} .= "@\tIN SOA\t$server. $contact. ( $Dateserial $refresh $retry $expire $min )\n";
}

# get servers for this domain
# also gets servers for any delegated subdomains

print STDERR "reading NS information\n" if $Verbose;
for $ns (@Ns) {
	NS_FIELDS_P = split(/\t/, $ns);
	if (($pdomain = forward_domain($domain))) {
		# check ttl value
		$ttl = "" if !check_ttl($domain, "ns", $ttl);
		$domain = strip_domain($domain, $pdomain);
		$server = strip_domain($server, $pdomain);
		$domain{$pdomain} .= "$domain\t$ttl IN NS\t$server\n";
	}
}
# scan the input file and extract info depending on whether this
# is a forward or reverse domain file

print STDERR "scanning input file main\n" if $Verbose;
for $main (@Main) {
	MAIN_FIELDS_P = split(/\t/, $main);
	# forward domain
	if ((! $ptr || $ptr ne "only") && ($domain = forward_domain($host))) {

		# dynamic address
		# get IP address from DHCP lease file, if available
		next if $ip =~ /dynamic/ && ! ($ip = getdynamicip($ether));

		# double quote $hard and $os if blanks in value
		$hard = "\"$hard\"" if ($hard =~ / /);
		$os = "\"$os\"" if ($os =~ / /);

		# check ttl value
		$ttl = "" if !check_ttl($host, "main", $ttl);
		$shost = strip_domain($host, $domain);
 		$domain{$domain} .= "$shost\t$ttl IN A\t$ip\n";
		$domain{$domain} .= "$shost\t$ttl IN HINFO\t$hard\t$os\n" if $hard && $os;
	}

	# reverse domain
	if ($ptr ne "no" && ($domain = reverse_domain($ip))) {
		$out = "";
		@ipparts = split(/\./, $ip);
		# Ignore as many octets as there are in the reverse domain name
		# ($partsindomain), take the rest of the IP address, reverse it.
		@partip = reverse(splice(@ipparts, $partsindomain));
		$partip = join(".", @partip);

		# check ttl value
		$ttl = "" if !check_ttl($domain, "main", $ttl);

		$out .= "$partip\t$ttl IN PTR\t$host.\n";
		$domain{$domain} .= $out;
	}
}

print STDERR "scanning input file cname\n" if $Verbose;
for $cname (@Cname) {
	CNAME_FIELDS_P = split(/\t/, $cname);
	# skip if we do not serve the domain
	if (($domain = forward_domain($alias))) {
		# check ttl value
		$ttl = "" if !check_ttl($alias, "alias", $ttl);
		$alias = strip_domain($alias, $domain);
		$host = strip_domain($host, $domain);
		$domain{$domain} .= "$alias\t$ttl IN CNAME\t$host\n";
	}
}

print STDERR "scanning input file mx\n" if $Verbose;
for $mx (@Mx) {
	MX_FIELDS_P = split(/\t/, $mx);
	# skip if we do not serve the domain
	if (($pdomain = forward_domain($domain))) {
		# check ttl value
		$ttl = "" if !check_ttl($domain, "mx", $ttl);
		$domain = strip_domain($domain, $pdomain);
		$host = strip_domain($host, $pdomain);
		$domain{$pdomain} .= "$domain\t$ttl IN MX\t$priority $host\n";
	}
}

print STDERR "scanning input file txt\n" if $Verbose;
for $txt (@Txt) {
	TXT_FIELDS_P = split(/\t/, $txt);
	# skip if we do not serve the domain
	if (($pdomain = forward_domain($domain))) {
		# check ttl value
		$ttl = "" if !check_ttl($domain, "txt", $ttl);
		$domain = strip_domain($domain, $pdomain);
		$host = strip_domain($host, $pdomain);
		$domain{$pdomain} .= "$domain\t$ttl IN TXT\t\"$txt\"\n";
	}
}

# produce the output zone files
# this checks if the new zone file has changed from the old zone file
# unless $Force_zones option is set
print STDERR "generating output files...\n" if $Verbose;
for ($i=0; $i<=$#Soa; $i++) {
	$domain = $domain[$i];

	# replace origin domain by "@" shorthand
	# treat string as multiline, of course
     	$domain{$domain} =~ s/^$domain\./@/gm;

	# check the zone against the old zone file
	$filename = $filename{$domain};
	if (! $Force_Zones) {
		open(OLD, $filename);
		@old_domain = <OLD>;
		close(OLD);
		$old_domain = join("", @old_domain);
		# skip the serial number record in the comparison
		$old_domain =~  s/(^\@\tIN SOA.*\( )([0-9]+)/$1/m;
		$new_domain = $domain{$domain};
		$new_domain =~  s/(^\@\tIN SOA.*\( )([0-9]+)/$1/m;
		next if $old_domain eq $new_domain;
	}
	# unlink the output file in case of permission problems
	unlink("$filename");
	open(OUT, ">$filename");
	print OUT $domain{$domain};
	close(OUT);
	print STDERR "$domain " if $Verbose;
}
print STDERR "\n" if $Verbose;
close(IN);

# create secondary zone directives
boot_sec;

boot_trailer;

#finally unlock the database files
unlock;
