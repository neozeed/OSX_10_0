#! @PERL@
#
# $Id: tail_log.pl,v 1.1.1.1 2001/01/31 03:59:15 zarzycki Exp $

# tail a logfile and check for named errors
# this program assumes the logfile tailing starts up before
# the nameserver is restarted or reloaded

die "usage: tail_log logfile daemon_name end_string time_out\n" unless $#ARGV == 3;
$LOGFILE = $ARGV[0];
$DAEMON = $ARGV[1];
$END_STRING = $ARGV[2];
$TIME_OUT = $ARGV[3];

open(LOGFILE, $LOGFILE) || die "cannot open $LOGFILE\n";
$ret = seek(LOGFILE, 0, 2);
$time_start = time();
for (;;) {
	$time = time();
	last if ($time-$time_start) >= $TIME_OUT;
	if (($_ = <LOGFILE>)) {
		next unless /$DAEMON/;
		print STDERR;
		last if /$END_STRING/;
		$time_start = time();
	}
	else {
		sleep 1;
		seek(LOGFILE, 0, 1);
	}
}
exit 0;
