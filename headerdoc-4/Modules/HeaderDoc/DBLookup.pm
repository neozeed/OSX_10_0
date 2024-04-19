#! /usr/bin/perl -w
#
# Class name: DBLookup
# Synopsis: Encapsulates the look-up tables and routines to get info 
#           from database output. Since we need only one of these DBLookups
#           we implement only class methods
#
# Author: Matt Morse (matt@apple.com)
# Last Updated: 12/9/99
# 
# Copyright (c) 1999 Apple Computer, Inc.  All Rights Reserved.
# The contents of this file constitute Original Code as defined in and are
# subject to the Apple Public Source License Version 1.1 (the "License").
# You may not use this file except in compliance with the License.  Please
# obtain a copy of the License at http://www.apple.com/publicsource and
# read it before using this file.
#
# This Original Code and all software distributed under the License are
# distributed on an TAS ISU basis, WITHOUT WARRANTY OF ANY KIND, EITHER
# EXPRESS OR IMPLIED, AND APPLE HEREBY DISCLAIMS ALL SUCH WARRANTIES,
# INCLUDING WITHOUT LIMITATION, ANY WARRANTIES OF MERCHANTABILITY, FITNESS
# FOR A PARTICULAR PURPOSE OR NON-INFRINGEMENT.  Please see the License for
# the specific language governing rights and limitations under the
# License.
#
######################################################################
package HeaderDoc::DBLookup;

use strict;
use vars qw($VERSION @ISA);
$VERSION = '1.20';

################ Portability ###################################
my $isMacOS;
my $pathSeparator;
if ($^O =~ /MacOS/i) {
	$pathSeparator = ":";
	$isMacOS = 1;
} else {
	$pathSeparator = "/";
	$isMacOS = 0;
}

################ General Constants ###################################
my $debugging = 0;

my $theTime = time();
my ($sec, $min, $hour, $dom, $moy, $year, @rest);
($sec, $min, $hour, $dom, $moy, $year, @rest) = localtime($theTime);
$moy++;
$year += 1900;
my $dateStamp = "$moy/$dom/$year";
######################################################################

################ Lookup Hashes ###################################
my %datatypeNameToIDHash;
my %functionNameToIDHash;
######################################################################


sub loadUsingFolderAndFiles {
    my($class) = shift;
    my $folder = shift;
	my $functionFilename = shift;
	my $typesFilename = shift;

    if (ref $class)  { die "Class method called as object method" };
    
	###################### Read in lookup table of functionID to name ######################
	my $functionTable = $folder.$pathSeparator.$functionFilename;
	open(FUNCIDS, "<$functionTable") || die "Can't open $functionTable.\n";
	my @funcIDLines = <FUNCIDS>;
	close FUNCIDS;
	foreach my $line (@funcIDLines) {
	    if ($line =~/^#/) {next;};
	    chomp $line;
	    my ($funcID, $funcName);
	    ($funcID, $funcName) = split (/\t/, $line);
	    if (length($funcID)) {
	        $functionNameToIDHash{$funcName} = $funcID;
	    }
	}
	undef @funcIDLines;
	###################### Read in lookup table of typeID to name ######################
	my $typeTable = $folder.$pathSeparator.$typesFilename;
	open(TYPEIDS, "<$typeTable") || die "Can't open $typeTable.\n";
	my @typeIDLines = <TYPEIDS>;
	close TYPEIDS;
	foreach my $line (@typeIDLines) {
	    if ($line =~/^#/) {next;};
	    chomp $line;
	    my ($typeID, $typeName);
	    ($typeID, $typeName) = split (/\t/, $line);
	    if (length($typeID)) {
	        $datatypeNameToIDHash{$typeName} = $typeID;
	    }
	}
	undef @typeIDLines;
}

sub dataTypeNameToIDHash {
    return %datatypeNameToIDHash;
}

sub functionNameToIDHash {
    return %functionNameToIDHash;
}

sub functionIDForName {
    my $class = shift;
    my $name = shift;
    
    if (exists ($functionNameToIDHash{$name})) {
		return $functionNameToIDHash{$name};
    } else {
        return "UNKNOWN_ID";
    }
}

sub typeIDForName {
    my $class = shift;
    my $name = shift;
    
    if (exists ($datatypeNameToIDHash{$name})) {
		return $datatypeNameToIDHash{$name};
    } else {
        return "UNKNOWN_ID";
    }
}

1;
