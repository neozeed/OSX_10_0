#! @PERL@

# $Id: domain_list.pl,v 1.1.1.1 2001/01/31 03:59:15 zarzycki Exp $
# list all the domain zone filenames
# for slave and stub domains we list the master servers

# uses the bindv8conf_lister lexical scanner to create a list of domains
# and their associated files

require "getopts.pl";

Getopts("hi:r:sS");

$input_file = $opt_i if $opt_i;
$named_root = $opt_r if $opt_r;
$domain_type = "master";
$domain_type = "slave" if $opt_s;
$domain_type = "stub" if $opt_S;

# a little sanity checking
if ($opt_h) {
	print STDERR <<EOF;
usage: domain_list [-i named_conf_file] [-r named_root] [-s|-S]
EOF
	exit 1;
}
if ($opt_s && $opt_S) {
	print STDERR "only -s or -S allowed\n";
	exit 1;
}
if ($named_root && ! -d $named_root) {
	print STDERR "$named_root does not exist\n";
	exit 1;
}
if ($input_file) {
	if (! -f $input_file) {
		print STDERR "$input_file does not exist\n";
		exit 1;
	}
	$input_file = "<$input_file";
}

# must be in the root named directory for the lister to handle includes
# properly
if ($named_root) {
	# get rid of the traling "/" if present
	chop $named_root if $named_root =~ /\/$/;
	chdir($named_root) || die "cannot chdir to $named_root\n";
}

open(IN, "bindv8conf_lister $input_file |") || die "cannot run bindv8conf_lister\n";
while (<IN>) {
	chomp;

	($domain, $type, $filename, $masters) = split;
	next unless $domain_type eq $type;
	if ($type eq "master") {
		print "$domain $named_root/$filename\n";
	}
	elsif ($type eq "slave" || $type eq "stub") {
		print "$domain $named_root/$masters\n";
	}
}
