#!@PERL@

# $Id: udh_stats.pl,v 1.1.1.1 2001/01/31 03:59:15 zarzycki Exp $
# DNS statistics generator

# use defaults from configuration file if found and not overridden
$CONFIG_FILE = "@sysconfdir@/updatehosts.env";

require "getopts.pl";

Getopts("f:");

if (-f $CONFIG_FILE) {
	$db_dir = "";
	open(CONF, "$CONFIG_FILE") || die "cannot open $CONFIG_FILE\n";
	while (<CONF>) {
		chomp;

		next unless /DB_DIR=/;
		($env, $db_dir) = split(/=/);
	}
	die "cannot find DB_DIR=<dir_path> in $CONFIG_FILE\n" unless $db_dir;
}
die "cannot chdir to $db_dir\n" unless chdir $db_dir; 

print "DNS Statistics\n\n";
open(IN, "readinfo domain <soa|") || die "cannot open soa";
@soa = <IN>;
chomp(@soa);
close(IN);
$forward = 0;
$reverse = 0;
for $soa (@soa) {
	if ($soa =~ /in-addr.arpa$/i) {
		$reverse++;
	}
	else {
		$forward++;
	}
}
$n = $#soa+1;
print "Total Zones - $n\n";
print " Forward Zones - $forward\n";
print " Reverse Zones - $reverse\n";

open(IN, "readinfo domain stub <secondary|") || die "cannot open secondary\n";
@secondary = <IN>;
chomp(@secondary);
close(IN);
$slave = 0;
$reverse_slave = 0;
$forward_slave = 0;
$stub = 0;
$forward_stub = 0;
$reverse_stub = 0;
for $secondary (@secondary) {
	($domain, $stubby) = split(/\t/, $secondary);
	if ($stubby ne "") {
		$stub++;
		if ($domain =~ /in-addr.arpa$/i) {
			$reverse_stub++;
		}
		else {
			$forward_stub++;
		}
	}
	else {
		$slave++;
		if ($domain =~ /in-addr.arpa$/i) {
			$reverse_slave++;
		}
		else {
			$forward_slave++;
		}
			
	}
}
print "Slave Zones - $slave\n";
print " Forward Slave Zones - $forward_slave\n";
print " Reverse Slave Zones - $reverse_slave\n";
print "Stub Zones - $stub\n";
print " Forward Stub Zones - $forward_stub\n";
print " Reverse Stub Zones - $reverse_stub\n";

open(IN, "readinfo domain <ns|") || die "cannot open ns";
@ns = <IN>;
close(IN);
$n = $#ns+1;
print "NS records - $n\n";

open(IN, "readinfo ip <main|") || die "cannot open main";
@main = <IN>;
close(IN);
$n = $#main+1;
print "A records - $n\n";

open(IN, "readinfo host <cname|") || die "cannot open cname\n";
@cname = <IN>;
close(IN);
$n = $#cname+1;
print "CNAME records - $n\n";

open(IN, "readinfo domain <mx|") || die "cannot open mx\n";
@mx = <IN>;
close(IN);
$n = $#mx+1;
print "MX records - $n\n";

open(IN, "readinfo domain <txt|") || die "cannot open txt\n";
@txt = <IN>;
close(IN);
$n = $#txt+1;
print "TXT records - $n\n";
