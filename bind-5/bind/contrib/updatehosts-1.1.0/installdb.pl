#! @PERL@
# $Id: installdb.pl,v 1.1.1.1 2001/01/31 03:59:15 zarzycki Exp $

# updatehosts DNS maintenance package
# Copyright (C) 1998  Smoot Carl-Mitchell
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

# install template db files in the install directory

require "getopts.pl";

@db_files = ("cname", "global", "main", "mx", "ns", "secondary", "soa", "subnet", "txt", "options");

Getopts("d:g:hn:D:");

if ($opt_h || ! $opt_D) {
	print STDERR <<EOF;
usage: installdb -D directory [-h] [-d domain suffix] [-n network prefix]
	-D - directory to install files (e.g. /var/named)
	-g - database group
	-h - print this help message
	-d - domain suffix (e.g. tic.com)
	-n - network prefix (e.g. 198.252.134)
EOF
	exit 1;
}

# sanity checking
if (! -d "db") {
	print STDERR "no db subdirectory present\n";
	exit 1;
}

# set the umask for the group access
umask(022);
if ($opt_g) {
	umask(002);
}

# get the numeric group id
if ($opt_g) {
	$group_id = $opt_g;
	if ($opt_g !~ /^[0-9][0-9]*$/) {
		($group, $dum, $group_id, $gr_members) = getgrnam($opt_g);
	}
}

if (! -d $opt_D) {
	print STDERR "creating database directory...\n";
	$dbpath =~ $opt_D;
	$dbpath =~ s/(.*\/)(.*)/$1/;

	# if the parent path does not exist then it is an error
	if ($dbpath && ! -d $dbpath) {
		print STDERR "parent path of $opt_D does not exist\n";
		exit 1;
	}
	# make the directory and the "db" subdirectory
	mkdir($opt_D, 0777);
}

mkdir("$opt_D/db", 0777) if ! -d "$opt_D/db";

# set the group and permissions
if ($opt_g) {
	system("/bin/chgrp $group_id $opt_D");
	system("/bin/chmod g+s $opt_D");
	system("/bin/chgrp $group_id $opt_D/db");
	system("/bin/chmod g+s $opt_D/db");
}

# install the template db files and set the defaults

$domain = $opt_d;
$ip = $opt_n;
for $file (@db_files) {
	open(IN, "db/$file") || die "cannot read db/$file\n";
	open(OUT, ">$opt_D/db/$file") || die "cannot write $opt_D/$file\n";

	while (<IN>) {
		# do nothing if domain suffix nor IP prefix set
		if (!$domain && !$ip) {
			print OUT;
			next;
		}

		if (! /^#FIELDS/) {
			print OUT;
			next;
		}

		if ($file eq "main") {
			s/host/host suffix=.$domain no=./ if $domain;
			s/ip/ip prefix=$ip. no=./ if $ip;
		}
		elsif ($file eq "cname") {
			s/host/host suffix=.$domain no=./ if $domain;
			s/alias`/alias suffix=.$domain no=./ if $domain;
		}
		elsif ($file eq "soa") {
			s/contact/contact suffix=.$domain no=./ if $domain;
		}
		elsif ($file eq "mx") {
			s/host/host suffix=.$domain no=./ if $domain;;
			s/domain/domain suffix=.$domain no=./ if $domain;
		}
		elsif ($file eq "txt") {
			s/domain/domain suffix=.$domain no=./ if $domain;
		}
		elsif ($file eq "subnet") {
			;
		}
		print OUT;
	}
	close IN;
	close OUT;
}
