use ExtUtils::MakeMaker;
# See lib/ExtUtils/MakeMaker.pm for details of how to influence
# the contents of the Makefile that is written.

# Include file directories.
$INC  = '-I/usr/local/include';

# Alternative library specifications.
$LIBS = ['-L/usr/local/lib -lmdn -liconv'];

# Subdirectories.
$DIR  = ['Converter', 'Normalizer', 'Log', 'UTF8', 'ResConf'];

######################################################################
make_sub_Makefile_PL();

WriteMakefile(
    'NAME'	=> 'MDN',
    'DIR'       => $DIR,
    'VERSION_FROM' => 'Converter/Converter.pm',
);

sub make_sub_Makefile_PL () {
    my ($dir, $lib, $makefile_pl);

    foreach $dir (@{$DIR}) {
	$makefile_pl = "$dir/Makefile.PL";
	unlink($makefile_pl) if (-f $makefile_pl);
	open(FILE, "> $makefile_pl")
	or die "$0: failed to open the file, $!: $makefile_pl\n";
	print FILE "use ExtUtils::MakeMaker;\n\n";
	print FILE "WriteMakefile(\n";
	print FILE "    'NAME'      => 'MDN::$dir',\n";
	print FILE "    'VERSION_FROM' => '$dir.pm',\n";
	print FILE "    'LIBS'      => [";
	foreach $lib (@{$LIBS}) {
	    print FILE "'$lib', ";
	}
	print FILE "],\n";
	print FILE "    'DEFINE'    => '',\n";
	print FILE "    'INC'       => '$INC -I../common',\n";
	print FILE "    'TYPEMAPS'  => ['../common/typemap'],\n";
	print FILE ");\n";
	close(FILE);
    }
}
