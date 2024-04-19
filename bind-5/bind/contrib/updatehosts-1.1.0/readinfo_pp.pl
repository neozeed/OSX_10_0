#! @PERL@

# $Id: readinfo_pp.pl,v 1.1.1.1 2001/01/31 03:59:15 zarzycki Exp $

undefine(`shift')
undefine(`format')
include(`parse_line.pl')

# pretty print a readinfo input file

@line = <>;
chomp(@line);

# sentinel at the end
push(@line, "#FIELDS");
$start = -1;
$line_num = -1;
for $line (@line) {
	$line_num++;
	if ($line =~ /^#FIELDS/) {
		$end = $line_num - 1;
		# beginning of file
		if ($start == -1) {
			$start = 0;
			next;
		}
		for ($i=$start; $i<=$end; $i++) {
			if ($line[$i] =~ /^\s*#/ || $line[$i] =~ /^\s*$/) {
				print "$line[$i]\n";
				next;
			}
			$n = 0;
			@field = parse_line($line[$i]);
			for $field (@field) {
				$l = $length[$n];
				$format = "%-$l"."s ";
				# XXX add quote checking
				printf $format, $field;
				$n++;
			}
			print "\n";
		}
		@field = ();
		@length = ();
		$start = $line_num;
		next;
	}
	next if $line =~ /^\s*#/;
	@field = parse_line($line);
	$n = 0;
	for $field (@field) {
		$l = length($field);
		$l += 2 if $field =~ /^"/ || $field =~ / /;
		$length[$n] = $l if $l > $length[$n];
		$n++
	}
}
