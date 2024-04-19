#! @PERL@
# $Id: readinfo.pl,v 1.1.1.1 2001/01/31 03:59:15 zarzycki Exp $

# updatehosts DNS maintenance package
# Copyright (C) 1998-2000  Smoot Carl-Mitchell
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

# reads a field oriented text file
#
# input from stdin
# output to stdout
#
# Arguments are names of fields in the file.
#
# fields in a file are defined by a comment line of the form
# #FIELDS <field_description> ...
#
# or
#
# $FIELDS <field_description>
#
# where <field_description> is the name of the field followed by optional
# keyword parameters of the form parameter=<value>
# 4 paramaters are supported in this script
#	prefix - prefix added to value of the field
#	suffix - suffix added to value of field
#	no - character to prevent adding a prefix or suffix to a field
#	null - null field value (default is X)
#
# It is also possible to define global values for the parameters with the
# GLOBAL field value followed by the usual keywords
#
# Fields in the file are separated with white space.  Embedded blanks in fields
# are either escaped with a '\' or the entire field is quoted with double quotes
# The prefix or suffix can be overridden by prepending or following the value
# with the ``no='' character.

# Fields may be continued over more than 1 line by ending the line with a \

# Other directives include
# #INCLUDE or $INCLUDE <filename> - include an alternate file
# #GENERATOR or $GENERATOR <generator_name> <generator> -  define a generator
# e.g. $GENERATOR STUFF 1,4,1
# #END - end of a generator

use POSIX;
use FileHandle;

# undefine definitions which conflict with m4
undefine(`shift')

# the common line parsing code - shared with readinfo_pp
include(`parse_line.pl')
undefine(`include')
undefine(`substr')

sub field_def {
	local($_) = shift;

	my($nfields) = 0;
	my($global) = -1;
	my($i);
	for ($i=1; $i<=$#out; $i++) {
		# field is a parameter
		if ($out[$i] =~ /\=/) {
			($keyword, $value) = split("=", $out[$i]);
			# prefix
			if ($keyword eq "prefix") {
				$prefix[$nfields-1] = $value;
			}
			# suffix
			elsif ($keyword eq "suffix") {
				$suffix[$nfields-1] = $value;
			}
			elsif ($keyword eq "no") {
				$no[$nfields-1] = $value;
			}
			elsif ($keyword eq "null") {
				$null[$nfields-1] = $value;
			}
		}
		#field name
		else {
			$fields[$nfields] = $out[$i];
			$prefix[$nfields] = "";
			$suffix[$nfields] = "";
			$no[$nfields] = "";
			$null[$nfields] = "";
			
			# check for GLOBAL special field
			$global = $nfields if $fields[$nfields] eq "GLOBAL";
			$nfields++;
		}
	}

	# copy in GLOBAL parameters if set
	if ($global != -1) {
		for ($i=0; $i<$nfields; $i++) {
			$prefix[$i] = $prefix[$global] if !$prefix[$i];
			$suffix[$i] = $suffix[$global] if !$suffix[$i];
			$no[$i] = $no[$global] if !$no[$i];
			$null[$i] = $null[$global] if !$null[$i];
		}

		# now shift out the GLOBAL pseudo field
		for ($i=$global+1; $i<$nfields; $i++) {
			$fields[$i-1] = $fields[$i];
			$prefix[$i-1] = $prefix[$i];
			$suffix[$i-1] = $suffix[$i];
			$no[$i-1] = $no[$i];
			$null[$i-1] = $null[$i];
		}
		$nfields--;
	}

	# quote all special characters in a "no" value
	# this allows any characters in the string
	
	for ($i=0; $i<$nfields; $i++) {
		$no[$i] =~ s/(\W)/\\$1/g;
	}

	# check to see all the extract list of fields are in the fields list
	$error = 0;
	for ($j=0; $j<=$#extract; $j++) {
		$match = 0;
		for ($i=0; $i<$nfields; $i++) {
			if ($fields[$i] eq $extract[$j]) {
				$match = 1;
				last;
			}
		}
		if (! $match) {
			print STDERR "ERROR - field $extract[$j] not found\n";
			$error = 1;
		}
	}
	exit if $error;
}

sub process_line {

	# see if there is a generator in the line
	# there can only be one

	my($generator) = "";
	my($start) = 1;
	my($end) = 1;
	my($increment) = 1;
	my($i, $j, $k);
	if ($Gen_name) {
		$start = $Gen_start;
		$end = $Gen_end;
		$increment = $Gen_increment;
	}
	for ($k=$start; $k<=$end; $k += $increment) {
		for ($i=0; $i<=$#extract; $i++) {
			for ($j=0; $j<=$#fields; $j++) {
				last if $fields[$j] eq $extract[$i];
			}
			my($null) = $null[$j];
			my($no) = $no[$j];
			my($prefix) = $prefix[$j];
			my($suffix) = $suffix[$j];
			my($out) = $out[$j];
			if ($i > 0) {
				print OUT "\t";
			}
			elsif ($Line_Num) {
				print OUT "$Line\t";
			}
			# check for null value
			if ($out eq $null) {
				$out = "";
				$prefix = "";
				$suffix = "";
			}
			# if override character exists, remove override character from
			# field and don't use appropriate affix
			elsif ($no) {
				if ($out =~ /^$no/) {
					$out = substr($out, 1);
					$prefix = "";
				}
				if ($out =~ /$no$/) {
					chop($out);
					$suffix = "";
				}
			}

			# add the generator value
			$out =~ s/\{$Gen_name\}/$k/;
			print OUT $prefix, $out, $suffix;
		}
		print OUT "\n";
	}
}

sub process_include {
	local($out) = shift;
	my(@out) = split(/\s+/, $out);
	my($filename) = $out[1];

	# open up an input descriptor to read the output
	# if the filename starts with a | then execute the command
	# with any arguments following and pipe into readinfo
	if ($filename =~ s/^\|//) {
		$args = join(" ", @out[2..$#out]);
		open(INCLUDE, "$filename $args | ./readinfo $Readinfoargs|") || die "$INCLUDE $out failed\n";
	}
	# just execute readinfo with the same argument list
	else {
		open(INCLUDE, "./readinfo $Readinfoargs <$filename|") || die "#INCLUDE $out failed\n";
	}
	while ($include = <INCLUDE>) {
		print OUT $include;
	}
}

sub define_generator {
	local($_) = shift;

	my(@args) = split;

	$Gen_name = $args[1];
	($Gen_start, $Gen_end, $Gen_increment) = split(/,/, $args[2]);

	return 0 if $Gen_name !~ /^\w+$/;
	return 0 if $Gen_start !~ /^\d+$/;
	return 0 if $Gen_end !~ /^\d+$/;
	return 0 if $Gen_increment !~ /^\d+$/;

	return 1;
}

# get the fields from the command line

# command line arguments
require "getopts.pl";
Getopts("ai:o:FL");
$Input = $opt_i;
$Output = $opt_o;
$Append = $opt_a;
$Output_Fields_Line = $opt_F;
$Line_Num = $opt_L;

@extract=@ARGV;
$Readinfoargs = join(" ", @extract);

# check for i/o from/to files
if ($Input) {
	open(IN, $Input) || die "cannot read $Input\n";
}
else {
	open(IN, "<&STDIN") || die "cannot open STDIN\n";
}

if ($Output) {
	if ($Append) {
		open(OUT, ">>$Output") || die "cannot open $Output\n";
	}
	else {
		open(OUT, ">$Output") || die "cannot open $Output\n";
	}
}
else {
	open(OUT, ">&STDOUT") || die "cannot open STDOUT\n";
}

# change whitespace to tabs and handle quoted fields and escaped blanks
# also remove comment lines, except the #FIELDS line
$saved_line = "";
$Line = 0;

# generator definition
$Generator = "";
$Gen_start = 1;
$Gen_end = 1;
$Gen_increment = 1;

while (<IN>) {
	chomp;

	$Line++;
	if ($saved_line) {
		$_ = "$saved_line$_" if $saved_line;
		$saved_line = "";
		$Line--;
	}

	# line continues over next line?
	if (/\\$/) {
		$saved_line = $_;
		next;
	}

	# just do special stuff on non-comments
	next if /^#/ && ! (/#FIELDS/ ||
			   /#INCLUDE/ ||
			   /#GENERATOR/ ||
			   /^#END/);

	# also skip blank lines
	next if ! /\S/;

	@out = parse_line($_);

	# change #command to $command
	$out[0] =~ s/^#/\$/;

	# extract field definition info from file
	if ($out[0] eq "\$FIELDS") {
		print OUT "$_\n" if $Output_Fields_Line;
		field_def($_);
		next;
	}

	# $INCLUDE reads another file
	if ($out[0] eq "\$INCLUDE") {
		process_include($_);
		next;
	}

	# $GENERATOR defines a generator
	if ($out[0] eq "\$GENERATOR") {
		define_generator($_);
		next;
	}

	# $END terminates a generator
	if ($out[0] eq "\$END") {
		$Gen_name = "";
		next;
	}
	# process a record
	# scan the list of fields to extract and compare them with the
	# list in the file and print as they are encountered
	process_line;
}
close IN;
close OUT;
