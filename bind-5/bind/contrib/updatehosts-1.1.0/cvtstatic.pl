#! @PERL@
#
# $Id: cvtstatic.pl,v 1.1.1.1 2001/01/31 03:59:15 zarzycki Exp $

# updatehosts DNS maintenance package
# Copyright (C) 1998-1999  Smoot Carl-Mitchell
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

# convert static host table to updathost file format
#

require "getopts.pl";

Getopts("acd:hi:o:");
if ($opt_h || ! $opt_d || ! $opt_o) {
	print STDERR <<EOF;
usage cvtstatic [-a] [-c] -d domain [-i IPprefix] -o directory
	-a - append to output files instead of overwriting
	-c - include comments from zone file
	-d - default domain for output
	-h - this help message
	-i - IP address prefix
	-o - directory to write output
EOF
	exit;
}
$append = $opt_a;
$app = ">";
$app = ">>" if $append;
$comments = $opt_c;
$dbdir = $opt_o;
$default_domain = $opt_d;
$default_domain .= "." if $default_domain !~ /\.$/;
$domain_suffix = $default_domain; chop($domain_suffix);	# Remove trailing dot.
$domain_suffix = ".$domain_suffix" if $domain_suffix !~ /^\./;
$ip_prefix = $opt_i;
$ip_prefix .= "." if $ip_prefix && $ip_prefix !~ /\.$/;

# open up the database files
open(MAIN, "$app$dbdir/main") || die "cannot open $dbdir/main";
open(CNAME, "$app$dbdir/cname") || die "cannot open $dbdir/cname";

if ($domain_suffix || $ip_prefix) {
	print MAIN "#FIELDS GLOBAL null=X host";
	print MAIN " suffix=$domain_suffix" if $domain_suffix;
	print MAIN " no=. ip";
	print MAIN " no=. prefix=$ip_prefix" if $ip_prefix;
	print MAIN " ether hard os contact norev\n";
	print CNAME "#FIELDS GLOBAL null=X host no=.";
	print CNAME " suffix=$domain_suffix" if $domain_suffix;
	print CNAME " alias no=.";
	print CNAME " suffix=$domain_suffix" if $domain_suffix;
	print CNAME "\n";
}
$ippat = $ip_prefix;
$domainpat = $domain_suffix;
$ippat =~ s/(\W)/\\$1/g;
$domainpat =~ s/(\W)/\\$1/g;
while (<>) {
	chop;

	next if /^\s*$/;

	# single line comments
	if (/^#/) {
		print MAIN "$_\n";
		next;
	}
	# print trailing comments before they are stripped
	if (/#/) {
		s/#.*$//;
		$comment = $1;
		print MAIN "$comment\n";
	}
		
	($ip, $canonical, @cname) = split;
	
	if ($ippat && $ip =~ /^$ippat/) {
		$ip =~ s/^$ippat//;
	}
	else {
		$ip = ".$ip" if $ip_prefix;
	}
	if ($canonical =~ /\./) {
		if ($domainpat && $canonical =~ /$domainpat$/) {
			$canonical =~ s/$domainpat$//;
		}
		else {
			$canonical = "$canonical.";
		}
	}
	print MAIN "$canonical\t$ip\n";
	foreach $cname (@cname) {
		if ($canonical =~ /\./) {
			if ($domainpat && $cname =~ /$domainpat$/) {
				$cname =~ s/$domainpat$//;
			}
			else {
				$cname = "$cname.";
			}
		}
		print CNAME "$canonical\t$cname\n";
	}
}
