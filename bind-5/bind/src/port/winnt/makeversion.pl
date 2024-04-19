# makeversion.pl
# This script takes the version information from the Version file located at the
# root of the source tree and replaces the the %VERSION% placeholder along with the
# other placeholders in the version.c files that the build process uses to build the
# executable code.
# This program was written by PDM. mayer@gis.net 15-Sep-2000.
#
# List of directories with version files to be modified
#
#@dirlist = ("named", "irpd");
@dirlist = ("named");		# Only named for now
#
# First get the version information
#
open (VERSIONFILE, "../../Version");
$version = <VERSIONFILE>;
chomp($version);
close(VERSIONFILE);

# Get User Name

$user = $ENV{"USERNAME"};
#$user = "mayer";

# Get machine name

$machine = $ENV{"COMPUTERNAME"};
#$machine = "tecotoo";
$dir = "src/port/winnt";

# Set up the Patterns

$Patterns{"%VERSION%"} = $version;
$Patterns{"%WHEN%"} = scalar localtime();
$Patterns{"%WHOANDWHERE%"} = "$user\@$machine:$dir";
$Patterns{"%WHOANDWHERE%"} =~ tr|A-Z|a-z|;	# make sure everything's lower case


print "BIND Version is $Patterns{'%VERSION%'}\n";
print "Date: $Patterns{'%WHEN%'}\n";
print "Who and Where: $Patterns{'%WHOANDWHERE%'}\n";

# Run the changes for each directory in the directory list 

foreach $dir (@dirlist) {
   createversionfile($dir);
}

# This is the routine that applies the changes

sub createversionfile {
my($infile, $outfile);

$infile = "../../bin/$_[0]/version.c";
$outfile = "$_[0]/version.c";

# get the input file
open (CVERSIONFILE, $infile) || die "Can't open file $infile: $!";
@lines = <CVERSIONFILE>;

# replace the placeholders

foreach $line (@lines) {
  foreach $old (keys %Patterns) {
    $line =~ s|$old|$Patterns{$old}|g;
  }
}

# output the result to the platform specific directory.

open (OUTVERSIONFILE, ">$outfile") || die "Can't open output file $outfile: $!";
print OUTVERSIONFILE @lines;
close(OUTVERSIONFILE);
}

