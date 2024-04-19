# Before `make install' is performed this script should be runnable with
# `make test'. After `make install' it should work as `perl test.pl'

######################### We start with some black magic to print on failure.

# Change 1..1 below to 1..last_test_to_print .
# (It may become useful if the test is moved to ./t subdirectory.)

BEGIN { $| = 1; print "1..27\n"; print "# load the module.\n"; }

END {print "not ok 1\n" unless $loaded;}
use MDN::Converter;
$loaded = 1;
print "ok 1\n";

######################### End of black magic.

# Insert your test code below (better if it prints "ok 13"
# (correspondingly "not ok 13") depending on the success of chunk 13
# of the test code):

$test_id = 2;

#####
print "# addalias()\n";
MDN::Converter->addalias('PRODUCE-RACE', 'RACE');
$context = MDN::Converter->new('PRODUCE-RACE'); 
$result = (ref($context) && $context->localencoding() eq 'RACE');
print $result ? 'ok ' : 'not ok ', $test_id++, "\n";

#####
print "# aliasfile()\n";
MDN::Converter->aliasfile('aliasfile');
$context = MDN::Converter->new('STAKE-RACE'); 
$result = (ref($context) && $context->localencoding() eq 'RACE');
print $result ? 'ok ' : 'not ok ', $test_id++, "\n";

#####
print "# resetalias()\n";
MDN::Converter->resetalias();
$context = MDN::Converter->new('PRODUCE-RACE'); 
$context2 = MDN::Converter->new('STAKE-RACE'); 
$result = (!ref($context) && !ref($context2));
print $result ? 'ok ' : 'not ok ', $test_id++, "\n";

#####
print "# create a converter\n";
$context = MDN::Converter->new('RACE');
$result = (ref($context) eq 'MDN::Converter');
print $result ? 'ok ' : 'not ok ', $test_id++, "\n";

#####
print "# create a converter, delayedopen option\n";
$context = MDN::Converter->new('RACE', 'delayedopen');
$result = (ref($context) eq 'MDN::Converter');
print $result ? 'ok ' : 'not ok ', $test_id++, "\n";

#####
print "# create a converter, rtcheck option\n";
$context = MDN::Converter->new('RACE', 'rtcheck');
$result = (ref($context) eq 'MDN::Converter');
print $result ? 'ok ' : 'not ok ', $test_id++, "\n";

#####
print "# create a converter, delayedopen+rtcheck options\n";
$context = MDN::Converter->new('RACE', 'delayedopen', 'rtcheck');
$result = (ref($context) eq 'MDN::Converter');
print $result ? 'ok ' : 'not ok ', $test_id++, "\n";

#####
print "# create a converter, invalid encoding name\n";
$context = MDN::Converter->new('Invalid-Encoding-Name');
$result = (!ref($context)
	   && MDN::Converter->lasterror() =~ /invalid encoding name/);
print $result ? 'ok ' : 'not ok ', $test_id++, "\n";

#####
print "# asciicompatible UTF-8\n";
$context = MDN::Converter->new('UTF-8'); 
$result = !$context->isasciicompatible();
print $result ? 'ok ' : 'not ok ', $test_id++, "\n";

#####
print "# asciicompatible RACE\n";
$context = MDN::Converter->new('RACE'); 
$result = $context->isasciicompatible();
print $result ? 'ok ' : 'not ok ', $test_id++, "\n";

#####
print "# localencoding UTF-8\n";
$context = MDN::Converter->new('UTF-8'); 
$result = ($context->localencoding() eq 'UTF-8');
print $result ? 'ok ' : 'not ok ', $test_id++, "\n";

#####
print "# localencoding RACE\n";
$context = MDN::Converter->new('RACE'); 
$result = ($context->localencoding() eq 'RACE');
print $result ? 'ok ' : 'not ok ', $test_id++, "\n";

#####
$utf8_sample = '@test@';
$race_sample = 'bq--abahizltoraa';
foreach $option ('', 'delayedopen', 'rtcheck', 'delayedopen rtcheck') {
    print "# convert RACE to UTF-8, $option\n";
    $context = MDN::Converter->new('RACE', split(/ +/, $option)); 
    $result = ($context->convert('localtoutf8', $race_sample)
	       eq $utf8_sample);
    print $result ? 'ok ' : 'not ok ', $test_id++, "\n";

    print "# convert UTF-8 to RACE, $option\n";
    $result = ($context->convert('utf8tolocal', $utf8_sample)
	       eq $race_sample);
    print $result ? 'ok ' : 'not ok ', $test_id++, "\n";

    print "# convert invalid UTF-8 to RACE, $option\n";
    $result = ($context->convert('utf8tolocal', '%') == undef
	       && MDN::Converter->lasterror() =~ /invalid encoding/);
    print $result ? 'ok ' : 'not ok ', $test_id++, "\n";
}

#####
print "# convert_localtoutf8\n";
$context = MDN::Converter->new('RACE', split(/ +/, $option)); 
$result = ($context->convert('localtoutf8', $race_sample)
	   eq $utf8_sample);
print $result ? 'ok ' : 'not ok ', $test_id++, "\n";

print "# convert utf8tolocal\n";
$context = MDN::Converter->new('RACE'); 
$result = ($context->convert_utf8tolocal($utf8_sample)
	   eq $race_sample);
print $result ? 'ok ' : 'not ok ', $test_id++, "\n";
