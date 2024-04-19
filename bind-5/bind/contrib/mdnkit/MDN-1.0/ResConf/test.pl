# Before `make install' is performed this script should be runnable with
# `make test'. After `make install' it should work as `perl test.pl'

######################### We start with some black magic to print on failure.

# Change 1..1 below to 1..last_test_to_print .
# (It may become useful if the test is moved to ./t subdirectory.)

BEGIN { $| = 1; print "1..25\n"; }
END {print "not ok 1\n" unless $loaded;}
use MDN::ResConf;
$loaded = 1;
print "ok 1\n";

######################### End of black magic.

# Insert your test code below (better if it prints "ok 13"
# (correspondingly "not ok 13") depending on the success of chunk 13
# of the test code):

$test_id = 2;

#####
print "# new, mdnres.conf\n";
$conf = MDN::ResConf->new('./mdnres.conf');
$result = (ref($conf));
print $result ? 'ok ' : 'not ok ', $test_id++, "\n";

#####
print "# new, mdnres-error.conf\n";
$conf = MDN::ResConf->new('./invalid-syntax.conf');
$result = (!ref($conf) && MDN::ResConf->lasterror() =~ /invalid syntax/);
print $result ? 'ok ' : 'not ok ', $test_id++, "\n";

#####
print "# new, no-file.conf\n";
$conf = MDN::ResConf->new('./no-file.conf');
$result = (!ref($conf));
print $result ? 'ok ' : 'not ok ', $test_id++, "\n";

#####
print "# localtoucs\n";
$ENV{'MDN_LOCAL_CODESET'} = 'euc-jp';
$conf = MDN::ResConf->new('./mdnres.conf');
$name = $conf->localtoucs("\xbb\xee\xb8\xb3");
$result = ($name eq "\xe8\xa9\xa6\xe9\xa8\x93");
print $result ? 'ok ' : 'not ok ', $test_id++, "\n";

#####
print "# localtoucs, invalid-encoding\n";
$ENV{'MDN_LOCAL_CODESET'} = 'euc-jp';
$conf = MDN::ResConf->new('./mdnres.conf');
$name = $conf->localtoucs("\xa1");
$result = (!defined($name) && $conf->lasterror() =~ /invalid encoding/);
print $result ? 'ok ' : 'not ok ', $test_id++, "\n";

#####
print "# ucstolocal\n";
$ENV{'MDN_LOCAL_CODESET'} = 'euc-jp';
$conf = MDN::ResConf->new('./mdnres.conf');
$name = $conf->ucstolocal("\xe8\xa9\xa6\xe9\xa8\x93");
$result = ($name eq "\xbb\xee\xb8\xb3");
print $result ? 'ok ' : 'not ok ', $test_id++, "\n";

#####
print "# ucstolocal, invalid-encoding\n";
$ENV{'MDN_LOCAL_CODESET'} = 'euc-jp';
$conf = MDN::ResConf->new('./mdnres.conf');
$name = $conf->ucstolocal("\xc3");
$result = (!defined($name) && $conf->lasterror() =~ /invalid encoding/);
print $result ? 'ok ' : 'not ok ', $test_id++, "\n";

#####
print "# normalize\n";
$ENV{'MDN_LOCAL_CODESET'} = 'iso-8859-1';
$conf = MDN::ResConf->new('./mdnres.conf');
$name = $conf->normalize("\xc3\x86\xc3\xa6");
$result = ($name eq "\xc3\x86\xc3\x86");
print $result ? 'ok ' : 'not ok ', $test_id++, "\n";

#####
print "# normalize, invalid-encoding\n";
$ENV{'MDN_LOCAL_CODESET'} = 'iso-8859-1';
$conf = MDN::ResConf->new('./mdnres.conf');
$name = $conf->normalize("\xc3");
$result = (!defined($name) && $conf->lasterror() =~ /invalid encoding/);
print $result ? 'ok ' : 'not ok ', $test_id++, "\n";

#####
print "# ucstodns\n";
$conf = MDN::ResConf->new('./mdnres.conf');
$name = $conf->ucstodns("\xe8\xa9\xa6\xe9\xa8\x93");
$result = ($name eq "bq--3cfgngqt");
print $result ? 'ok ' : 'not ok ', $test_id++, "\n";

#####
print "# ucstodns, invalid-encoding\n";
$ENV{'MDN_LOCAL_CODESET'} = 'euc-jp';
$conf = MDN::ResConf->new('./mdnres.conf');
$name = $conf->ucstodns("\xc3");
$result = (!defined($name) && $conf->lasterror() =~ /invalid encoding/);
print $result ? 'ok ' : 'not ok ', $test_id++, "\n";

#####
print "# dnstoucs\n";
$conf = MDN::ResConf->new('./mdnres.conf');
$name = $conf->dnstoucs("bq--3cfgngqt");
$result = ($name eq "\xe8\xa9\xa6\xe9\xa8\x93");
print $result ? 'ok ' : 'not ok ', $test_id++, "\n";

#####
print "# dnstoucs, invalid-encoding\n";
$ENV{'MDN_LOCAL_CODESET'} = 'euc-jp';
$conf = MDN::ResConf->new('./mdnres.conf');
$name = $conf->dnstoucs("\xff");
$result = (!defined($name) && $conf->lasterror() =~ /invalid encoding/);
print $result ? 'ok ' : 'not ok ', $test_id++, "\n";

#####
print "# localtodns\n";
$ENV{'MDN_LOCAL_CODESET'} = 'euc-jp';
$conf = MDN::ResConf->new('./mdnres.conf');
$name = $conf->localtodns("\xbb\xee\xb8\xb3");
$result = ($name eq "bq--3cfgngqt");
print $result ? 'ok ' : 'not ok ', $test_id++, "\n";

#####
print "# localtodns, invalid-encoding\n";
$ENV{'MDN_LOCAL_CODESET'} = 'euc-jp';
$conf = MDN::ResConf->new('./mdnres.conf');
$name = $conf->localtodns("\xa1");
$result = (!defined($name) && $conf->lasterror() =~ /invalid encoding/);
print $result ? 'ok ' : 'not ok ', $test_id++, "\n";

#####
print "# dnstolocal\n";
$ENV{'MDN_LOCAL_CODESET'} = 'euc-jp';
$conf = MDN::ResConf->new('./mdnres.conf');
$name = $conf->dnstolocal("bq--3cfgngqt");
$result = ($name eq "\xbb\xee\xb8\xb3");
print $result ? 'ok ' : 'not ok ', $test_id++, "\n";

#####
print "# dnstolocal, invalid-encoding\n";
$ENV{'MDN_LOCAL_CODESET'} = 'euc-jp';
$conf = MDN::ResConf->new('./mdnres.conf');
$name = $conf->dnstolocal("\xff");
$result = (!defined($name) && $conf->lasterror() =~ /invalid encoding/);
print $result ? 'ok ' : 'not ok ', $test_id++, "\n";

#####
print "# localconverter\n";
$ENV{'MDN_LOCAL_CODESET'} = 'iso-2022-jp';
$conf = MDN::ResConf->new('./mdnres.conf');
$converter = $conf->localconverter();
$result = (ref($converter) eq 'MDN::ResConf::Converter'
	   && $converter->localencoding() eq 'iso-2022-jp');
print $result ? 'ok ' : 'not ok ', $test_id++, "\n";

#####
print "# serverconverter\n";
$ENV{'MDN_LOCAL_CODESET'} = 'iso-2022-jp';
$conf = MDN::ResConf->new('./mdnres.conf');
$converter = $conf->serverconverter();
$result = (ref($converter) eq 'MDN::ResConf::Converter'
	   && $converter->localencoding() eq 'RACE');
print $result ? 'ok ' : 'not ok ', $test_id++, "\n";

#####
print "# alternateconverter\n";
$ENV{'MDN_LOCAL_CODESET'} = 'iso-2022-jp';
$conf = MDN::ResConf->new('./mdnres.conf');
$converter = $conf->alternateconverter();
$result = (ref($converter) eq 'MDN::ResConf::Converter'
	   && $converter->localencoding() eq 'UTF-5');
print $result ? 'ok ' : 'not ok ', $test_id++, "\n";

#####
print "# normalizer\n";
$ENV{'MDN_LOCAL_CODESET'} = 'iso-2022-jp';
$conf = MDN::ResConf->new('./mdnres.conf');
$normalizer = $conf->normalizer();
$result = (ref($normalizer) eq 'MDN::ResConf::Normalizer'
	   && $normalizer->normalize("\xc3\xa6") eq "\xc3\x86");
print $result ? 'ok ' : 'not ok ', $test_id++, "\n";

#####
print "# serverconverter, null\n";
$conf = MDN::ResConf->new('./null.conf');
$converter = $conf->serverconverter();
$result = (!ref($converter));
print $result ? 'ok ' : 'not ok ', $test_id++, "\n";

#####
print "# alternateconverter, null\n";
$conf = MDN::ResConf->new('./null.conf');
$converter = $conf->alternateconverter();
$result = (!ref($converter));
print $result ? 'ok ' : 'not ok ', $test_id++, "\n";

#####
print "# normalizer, null\n";
$conf = MDN::ResConf->new('./null.conf');
$normalizer = $conf->normalizer();
$result = (!ref($normalizer));
print $result ? 'ok ' : 'not ok ', $test_id++, "\n";

