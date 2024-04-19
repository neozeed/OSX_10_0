# Before `make install' is performed this script should be runnable with
# `make test'. After `make install' it should work as `perl test.pl'

######################### We start with some black magic to print on failure.

# Change 1..1 below to 1..last_test_to_print .
# (It may become useful if the test is moved to ./t subdirectory.)

BEGIN { $| = 1; print "1..11\n"; }
END {print "not ok 1\n" unless $loaded;}
use MDN::UTF8;
$loaded = 1;
print "ok 1\n";

######################### End of black magic.

# Insert your test code below (better if it prints "ok 13"
# (correspondingly "not ok 13") depending on the success of chunk 13
# of the test code):

$test_id = 2;
$utf8_sample = "\xe8\xa9\xa6\xe9\xa8\x93";
$utf8_sample_wc = 0x8a66;
$utf8_sample_length = 3;

@ucs4_sample = (0x8a66, 0x9a13);

$utf8_invalid = "\xff";
@ucs4_invalid = ();

#####
print "# mblen\n";
$length = MDN::UTF8->mblen($utf8_sample);
$result = ($length == 3);
print $result ? 'ok ' : 'not ok ', $test_id++, "\n";

#####
print "# mblen, invalid UTF-8 string\n";
$length = MDN::UTF8->mblen($utf8_invalid);
$result = ($length == 0);
print $result ? 'ok ' : 'not ok ', $test_id++, "\n";

#####
print "# getwc\n";
($wc, $length) = MDN::UTF8->getwc($utf8_sample);
$result = ($wc == $utf8_sample_wc && $utf8_sample_length == 3);
print $result ? 'ok ' : 'not ok ', $test_id++, "\n";

#####
print "# getwc, invalid UTF-8 string\n";
$result = !MDN::UTF8->getwc($utf8_invalid);
print $result ? 'ok ' : 'not ok ', $test_id++, "\n";

#####
print "# isvalid\n";
$result = MDN::UTF8->isvalid($utf8_sample);
print $result ? 'ok ' : 'not ok ', $test_id++, "\n";

#####
print "# isvalid, invalid UTF-8 string\n";
$result = !MDN::UTF8->getwc($utf8_invalid);
print $result ? 'ok ' : 'not ok ', $test_id++, "\n";

#####
print "# unpack\n";
@ucs4_list = MDN::UTF8->unpack($utf8_sample);
$result = (compare_list(\@ucs4_list, \@ucs4_sample) == 0);
print $result ? 'ok ' : 'not ok ', $test_id++, "\n";

#####
print "# unpack, invalid UTF-8 string\n";
@ucs4_list = MDN::UTF8->unpack($utf8_invalid);
$result = (@ucs4_list == 0);
print $result ? 'ok ' : 'not ok ', $test_id++, "\n";

#####
print "# pack\n";
$utf8_string = MDN::UTF8->pack(@ucs4_sample);
$result = ($utf8_string eq $utf8_sample);
print $result ? 'ok ' : 'not ok ', $test_id++, "\n";

#####
print "# pack, invalid UTF-8 string\n";
$utf8_string = MDN::UTF8->pack(@ucs4_invalid);
$result = (!defined($utf8_string));
print $result ? 'ok ' : 'not ok ', $test_id++, "\n";

#
# Compare references to list1 and list2.
#
sub compare_list {
    my (@list1) = @{$_[0]};
    my (@list2) = @{$_[1]};
    my ($i);

    return (@list2 - @list1) if (@list1 != @list2);

    for ($i = 0; $i < @list1; $i++) {
	return ($list2[$i] - $list1[$i]) if ($list1[$i] != $list2[$i]);
    }

    return 0;
}
