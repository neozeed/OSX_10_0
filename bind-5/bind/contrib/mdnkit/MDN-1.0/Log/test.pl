# Before `make install' is performed this script should be runnable with
# `make test'. After `make install' it should work as `perl test.pl'

######################### We start with some black magic to print on failure.

# Change 1..1 below to 1..last_test_to_print .
# (It may become useful if the test is moved to ./t subdirectory.)

BEGIN { $| = 1; print "1..14\n"; }
END {print "not ok 1\n" unless $loaded;}
use MDN::Normalizer;
use MDN::Log;
$loaded = 1;
print "ok 1\n";

######################### End of black magic.

# Insert your test code below (better if it prints "ok 13"
# (correspondingly "not ok 13") depending on the success of chunk 13
# of the test code):

$test_id = 2;
$log_file = 'test.log';

#####
print "# getlevel\n";
$level = MDN::Log->getlevel();
$result = ($level =~ /^(fatal|error|warning|info|trace|dump)$/);
print $result ? 'ok ' : 'not ok ', $test_id++, "\n";

#####
foreach $level ('fatal', 'error', 'warning', 'info', 'trace', 'dump') {
    print "# setlevel, $level\n";
    MDN::Log->setlevel($level);
    $result = (MDN::Log->getlevel() eq $level);
    print $result ? 'ok ' : 'not ok ', $test_id++, "\n";
}

#####
print "# output log, info\n";
open_log_file($log_file);
MDN::Log->setlevel('dump');
MDN::Normalizer->new('invalid-scheme');
close_log_file($log_file);
$result = (scan_log_file($log_file, '\\[WARNING\\]'));
print $result ? 'ok ' : 'not ok ', $test_id++, "\n";

#####
print "# output log, fatal\n";
open_log_file($log_file);
MDN::Log->setlevel('fatal');
MDN::Normalizer->new('invalid-scheme');
close_log_file($log_file);
$result = (!scan_log_file($log_file, '\\[WARNING\\]'));
print $result ? 'ok ' : 'not ok ', $test_id++, "\n";

#####
print "# setproc_none\n";
open_log_file($log_file);
MDN::Log->setlevel('dump');
MDN::Log->setproc_none();
MDN::Normalizer->new('invalid-scheme');
close_log_file($log_file);
$result = (!scan_log_file($log_file, '.'));
print $result ? 'ok ' : 'not ok ', $test_id++, "\n";

#####
print "# setproc_stderr\n";
open_log_file($log_file);
MDN::Log->setlevel('dump');
MDN::Log->setproc_none();
MDN::Log->setproc_stderr();
MDN::Normalizer->new('invalid-scheme');
close_log_file($log_file);
$result = (scan_log_file($log_file, '\\[WARNING\\]'));
print $result ? 'ok ' : 'not ok ', $test_id++, "\n";

#####
print "# setproc, empty\n";
open_log_file($log_file);
MDN::Log->setlevel('dump');
MDN::Log->setproc_none();
MDN::Log->setproc();
MDN::Normalizer->new('invalid-scheme');
close_log_file($log_file);
$result = (scan_log_file($log_file, '\\[WARNING\\]'));
print $result ? 'ok ' : 'not ok ', $test_id++, "\n";

#####
print "# setproc, my_handler\n";
open_log_file($log_file);
MDN::Log->setlevel('dump');
MDN::Log->setproc(\&my_handler);
MDN::Normalizer->new('invalid-scheme');
close_log_file($log_file);
$result = (scan_log_file($log_file, 'MY-HANDLER'));
print $result ? 'ok ' : 'not ok ', $test_id++, "\n";

#
# Open the log file.
#
sub open_log_file {
    my ($log_file) = @_;

    if (-f $log_file) {
	unlink($log_file);
    }

    open(SAVEERR, ">&STDERR");
    if (!open(STDERR, ">$log_file")) {
	print SAVEERR "cannot open the file $log_file, $!\n";
	exit 1;
    }
}

#
# Close the log file.
#
sub close_log_file {
    open(STDERR, ">&SAVEERR");
    close(SAVEERR);
}

sub scan_log_file {
    my ($log_file, $pattern) = @_;
    my ($line, $found);

    open(LOGFILE, $log_file) or die "cannot open the file $log_file, $!\n";

    study $pattern;
    $found = 0;
    while ($line = <LOGFILE>) {
	chomp($line);
	if ($line =~ /$pattern/) {
	    $found = 1;
	    last;
	}
    }

    close(LOGFILE);
    return $found;
}

sub my_handler {
    my ($level, $message) = @_;
    warn "MY-HANDLER: [$level] $message";
}
