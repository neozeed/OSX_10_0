#!/usr/local/bin/perl5 -w

use Net::DNS;

#------------------------------------------------------------------------------
# Get the domain from the command line.
#------------------------------------------------------------------------------

die "Usage:  check_soa domain\n" unless @ARGV == 1;
$domain = $ARGV[0];

#------------------------------------------------------------------------------
# Find all the nameservers for the domain.
#------------------------------------------------------------------------------

$res = new Net::DNS::Resolver;

$res->defnames(0);
$res->retry(2);

$ns_req = $res->query($domain, "NS");
die "No nameservers found for $domain: ", $res->errorstring, "\n"
	unless defined($ns_req) and ($ns_req->header->ancount > 0);

@nameservers = grep { $_->type eq "NS" } $ns_req->answer;

#------------------------------------------------------------------------------
# Check the SOA record on each nameserver.
#------------------------------------------------------------------------------

$| = 1;
$res->recurse(0);

foreach $nsrr (@nameservers) {

	#----------------------------------------------------------------------
	# Set the resolver to query this nameserver.
	#----------------------------------------------------------------------

	$ns = $nsrr->nsdname;
	print "$ns ";

	unless ($res->nameservers($ns)) {
		warn ": can't find address: ", $res->errorstring, "\n";
		next;
	}

	#----------------------------------------------------------------------
	# Get the SOA record.
	#----------------------------------------------------------------------

	$soa_req = $res->send($domain, "SOA");
	unless (defined($soa_req)) {
		warn ": ", $res->errorstring, "\n";
		next;
	}

	#----------------------------------------------------------------------
	# Is this nameserver authoritative for the domain?
	#----------------------------------------------------------------------

	unless ($soa_req->header->aa) {
		warn "is not authoritative for $domain\n";
		next;
	}

	#----------------------------------------------------------------------
	# We should have received exactly one answer.
	#----------------------------------------------------------------------

	unless ($soa_req->header->ancount == 1) {
		warn ": expected 1 answer, got ",
		      $soa_req->header->ancount, "\n";
		next;
	}
		
	#----------------------------------------------------------------------
	# Did we receive an SOA record?
	#----------------------------------------------------------------------

	unless (($soa_req->answer)[0]->type eq "SOA") {
		warn ": expected SOA, got ",
		     ($soa_req->answer)[0]->type, "\n";
		next;
	}
		
	#----------------------------------------------------------------------
	# Print the serial number.
	#----------------------------------------------------------------------

	print "has serial number ", ($soa_req->answer)[0]->serial, "\n";
}
