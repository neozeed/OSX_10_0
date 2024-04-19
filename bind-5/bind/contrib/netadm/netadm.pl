#!/usr/local/bin/perl -w
#
#	$Id: netadm.pl,v 1.1.1.1 2001/01/31 03:59:06 zarzycki Exp $
#

print STDERR 'netadm V 1.3 (C) 2000 by ron@simcb.ru

';

%CmdHash = (
# Option Code(1 - no value; 2 - must be value; 3 - enable multiple value)
	'a',		1,
	'c',		1,
	'd',		2,
	's',		1,
	'-help',	1,
	'-version',	1
);
%CmdOpt = ();
@CmdArg = ();
$_ = CmdParse();
$_ and Usage($_);
$CmdOpt{'-help'} and Help();
$CmdOpt{'-version'} and exit 0;
$#CmdArg > 0 and Usage();

#
# Command line parameters
#
$Report		= $CmdOpt{'a'};	# Report about all result files
$Create		= $CmdOpt{'c'};	# Not create result files
$Stdout		= $CmdOpt{'s'};	# Output result files to STDOUT

#
# Global Constants
#
$MaxErr		= 20;
$Config		= 'netadm.conf';
$Named_cfg	= 'named.conf';
$Named_cache	= 'root.cache';
$Named_root	= 'root.zone';
$TmpFile	= '/tmp/netadm.tmp';
$SerialLine	= 4;
#
# Global Variables
#
$E		= 0;	# Error counter
$IsChange	= 0;	# Result file[s] is changed
$HostOut	= 0;	# Flag for output
$Serial		= 0;	# Serial for zone files
$Text		= '';	# Continue of multiline codewords
$Level		= '';	# Level in use
$Domain		= '';	# Current domain
%Html		= ();	# Hash filename for rezult tables
%File		= ();	# Hash file/directory names in use
$Directory	= undef;	# Base directory in config file
$Root		= undef;	# Id of Root Zone
$IN		= 'fh00';	# File handler
$IP127		= setarpa('127.0.0.1');
# Named parameters
$NamedDir	= '';
$NamedCache	= '';
@NamedCfg	= ();
@NamedOpt	= ();
%NamedFor	= ();
@DB_Host	= ([(
  'localhost',		# domain
  '',			# port
  '?',			# line
  undef,		# vlan
  [($IP127)]		# @ip
)]);
%Host		= (
  'localhost'=>{'' =>0,	# domain => port => id_host
		'?'=>1}	# domain => '?' => count
);
%HostIP		= (
  'localhost'=> [($IP127)]	# domain => @ip
);
%IP		= (
  $IP127 =>0		# ip => id_host
);
@DB_Zone	= ([(
  '.Primary',		# type
  '',			# directory
  [{'localhost' => 1}],	# %nameserver
  [()],	 		# @option
  'localhost',		# bind_host
  'root.localhost',	# bind_master
  3600,			# refresh
  1800,			# retry
  604800,		# expire
  3600			# ttl
)]);
%Zone		= (
  'localhost'=>{'' =>0 },	# network => mask => id_zone
  $IP127 - 1 =>{24 =>0 }
);
%ZoneD		= (
  'localhost'=>[()]	# d_network => @d_network
);
%ZoneDHost	= (
  'localhost'=>[()] 	# d_network => @domain
);
%ZoneIP		= (
  $IP127 - 1 =>{24=>[()]}	# ip_network => mask => @(ip_network,mask)
);
%ZoneIPHost	= (
  $IP127 - 1 =>{24=>[()]}	# ip_network => mask => @ip
);
%RootD		= ();	# domain => 1
%RootIP		= ();	# ip_network => mask
@DB_Mail	= ();	# mailserver
%Mail		= ();	# mailserver => priority
%MX		= ();	# domain => id_mailserver => priority
@DB_Vlan	= ();	# vlan
%Vlan		= ();	# vlan => id_vlan
%Vnet		= ();	# network => mask => id_vlan
%Vhost		= ();	# ip => id_vlan
%Line		= ();	# line => @id_host
%CNAME		= ();	# domain => domain
%MailOnly	= ();	# domain => ''
%IPzone		= ();	# mask => @ip
%IPvlan		= ();	# mask => @ip

checkDir($CmdOpt{'d'});
$E and die "Error[s] in command line. Terminated.\n";

$_ = `pwd`;
chomp;
$Pwd = $_;

print STDERR "Read configfile[s] ...\n";
getConfig($#CmdArg == 0 ? $CmdArg[0] : $Config);
$E and die "Error[s] in configfile[s]. Terminated.\n";

print STDERR "Analyse geting data ...\n";
mkHash();
$E and die "Error[s] in geting data. Terminated.\n";

print STDERR "Build result files ...\n";
mkResult();
$E and die "Error[s] when build result files.\n";

print STDERR "Done.\n";
$IsChange and print STDERR "*** Please, check changed files before reload or restart BIND! ***\n\n";
exit 0;

#
#	Subroutines
#
sub checkDir {
	my($d) = @_;
	defined $d or return;
	my($e,$s,$r) = mkPath($d);
	$e == -1 and return Error("Parameter '$d' not pathname");
	$e == 4 and return Error("Empty path");
	($e & 2) == 0 and return Error("'$d' not absolute pathname");
	$Directory = $r;
}
sub getConfig {
	$_ = doInclude('',$_[0]) and Error($_);
	$_ = doEnd(-1) and Error($_);
}
sub mkHash {
	my($d,$p,$l,$v,$m,$i,@a,%t);
	foreach $m (reverse sort keys %IPzone) {
		foreach $d (@{$IPzone{$m}}) {
			%t = ($d,'');
			$p = $Zone{$d}{$m};
			$l = $DB_Zone[$p][1];
			if($m != 24 && $m != 16 && $m != 8) {
				delete $Zone{$d}{$m};
				if($m > 16) {
					$v = 24;
					$_ = 1 << 8;
				} else {
					$v = 16;
					$_ = 1 << 16;
				}
				$i = -1 << (32 - $m);
				for(;;) {
					$d += $_;
					($d & $i) == $d and last;
					$t{$d} = '';
				}
				foreach $_ (keys %t) {
					if(defined $Zone{$_}{$v}) {
						delete $t{$_};
					} else {
						$Zone{$_}{$v} = $p;
					}
				}
			} else {
				$v = $m;
			}
			foreach $i (keys %t) {
				$ZoneIP{$i}{$v} = [()];
				$ZoneIPHost{$i}{$v} = [()];
				$_ = RegFileAll($l . arpaf($i,$v));
				$_ and do {
					Error($_);
					$E > $MaxErr and return;
				};
			}
		}
	}
	foreach $m (reverse sort keys %IPvlan) {
		foreach $d (@{$IPvlan{$m}}) {
			if($m != 24 && $m != 16 && $m != 8) {
				%t = ($d,'');
				$p = $Vnet{$d}{$m};
				delete $Vnet{$d}{$m};
				if($m > 16) {
					$v = 24;
					$_ = 1 << 8;
				} else {
					$v = 16;
					$_ = 1 << 16;
				}
				$i = -1 << $m;
				for(;;) {
					$d += $_;
					($d & $i) == $d and last;
					$t{$d} = '';
				}
				foreach $_ (keys %t) {
					if(defined $Vnet{$_}{$v}) {
						delete $t{$_};
					} else {
						$Vnet{$_}{$v} = $p;
					}
				}
			}
		}
	}
	%IPzone = %IPvlan = ();
	foreach $d (keys %ZoneD) {
		$l = undef;
		for($_ = $d; s/^[^\.]+\.?//;) {
			$_ or last;
			$l = $ZoneD{$_};
			defined $l and do {
				push @{$ZoneD{$_}},$d;
				last;
			};
		}
		not defined $l and $RootD{$d} = 1;
	}
	@a = (24,(-1 << 8),16,(-1 << 16),8,(-1 << 24));
	foreach $d (keys %ZoneIP) {
		foreach $m (keys %{$ZoneIP{$d}}) {
			$l = undef;
			$m != 8 and do {
				foreach $i (2,4) {
					$m == 16 and $i == 2 and next;
					$_ = $d & $a[$i+1];
					defined $ZoneIP{$_} or next;
					$l = $ZoneIP{$_}{$a[$i]};
					defined $l or next;
					push @{$ZoneIP{$_}{$a[$i]}},$d,$m;
				}
			};
			not defined $l and $RootIP{$d} = $m;
		}
	}
	foreach $d (keys %IP) {
		$v = $m = undef;
		foreach $i (0,2,4) {
			$_ = $d & $a[$i+1];
			defined $m or do {
				defined $ZoneIPHost{$_} and do {
					$m = $ZoneIPHost{$_}{$a[$i]};
					defined $m and do {
						push @{$ZoneIPHost{$_}{$a[$i]}},$d;
						defined $v and last;
					};
				};
			};
			defined $v or do {
				defined $Vnet{$_} and do {
					$v = $Vnet{$_}{$a[$i]};
					defined $v and do {
						$Vhost{$d} = $v;
						defined $m and last;
					};
				};
			};
		}
	}
	foreach $d (keys %HostIP, keys %CNAME, keys %MailOnly) {
		for($_ = $d; s/^[^\.]+\.?//;) {
			$_ or last;
			defined $ZoneDHost{$_} and do {
				push @{$ZoneDHost{$_}},$d;
				last;
			};
		}
	}
	foreach $d (keys %Host) {
		@a = %{$Host{$d}};
		$Host{$d}{'?'} = $#a;
	}
	for($i = 0; $i <= $#DB_Host; $i++) {
		($d,$p,$l,$v) = @{$DB_Host[$i]}[0..3];
		@a = ();
		$l ne '?' and do {
			foreach $_ (@{$Line{$l}}) {
				push @a,$DB_Host[$_][3];
			}
		};
		foreach $_ (@{$DB_Host[$i][4]}) {
			$_ or last;
			/\D/ and last;
			push @a,$Vhost{$_};
		}
		foreach $_ (@a) {
			defined $_ or next;
			if(defined $v) {
				$v != $_ and do {
					Error("'$d$p' in vlan '$DB_Vlan[$_]' and '$DB_Vlan[$v]'");
					$E > $MaxErr and return;
				};
			} else {
				$v = $_;
			}
		}
		defined $v or next;
		if($l ne '?') {
			foreach $_ (@{$Line{$l}}) {
				$DB_Host[$_][3] = $v;
			}
		} else {
			$DB_Host[$i][3] = $v;
		}
	}
	for($i = 0; $i <= $#DB_Zone; $i++) {
		if($DB_Zone[$i][0] eq '.Primary' or $DB_Zone[$i][0] eq '.Root') {
			$DB_Zone[$i][5] or $DB_Zone[$i][5] = 'root.' . $DB_Zone[$i][4];
			$DB_Zone[$i][6] or @{$DB_Zone[$i]}[6..9] = @{$DB_Zone[0]}[6..9];
			%{$DB_Zone[$i][2]} or $DB_Zone[$i][2]{$DB_Zone[$i][4]} = 0;
		}
		foreach $_ (keys %{$DB_Zone[$i][2]}) {
			defined $HostIP{$_} or do {
				Error("No IP address for NameServer '$_'");
				$E > $MaxErr and return;
				$HostIP{$_} = [()];
			};
		}
	}
	foreach $_ (keys %NamedFor) {
		defined $HostIP{$_} or do {
			Error("No IP address for NameServer '$_'");
			$E > $MaxErr and return;
			$HostIP{$_} = [()];
		};
	}
	foreach $_ (@DB_Mail) {
		defined $HostIP{$_} or do {
			Error("No IP address for MailServer '$_'");
			$E > $MaxErr and return;
			$HostIP{$_} = [()];
		};
	}
	foreach $d (keys %MailOnly) {
		defined $MX{$d} or do {
			$_ = $d;
			s/^[^\.]*/*/;
			defined $MX{$_} or do {
				Error("No MailServer for mail only domain '$d'");
				$E > $MaxErr and return;
			};
		};
	}
	while(($d,$v) = each %CNAME) {
		defined $CNAME{$v} and do {
			Error("Domain '$d' is CNAME for CNAME '$v'");
			$E > $MaxErr and return;
		};
	}
}
sub mkResult {
	my(@s) = localtime(time);
	$Serial = (((1900 + $s[5]) * 100 + $s[4] + 1) * 100 + $s[3]) * 100;
	my(%f);
	foreach $_ (keys %File) {
		s/[^\/]*$//;
		$f{$_} = 1;
	}
	foreach $_ (sort keys %f) {
		CreateDir("$_.");
	}
	$E and return;
	ResultCfg() and return;
	ResultRoot() and return;
	ResultRZone() and return;
	ResultDZone() and return;
	$_ = $Html{'.Host'};
	defined $_ and ResultHost($_) and return;
	$_ = $Html{'.Line'};
	defined $_ and ResultLine($_) and return;
	$E and return;
	$_ = $Html{'.Net'};
	defined $_ and ResultNet($_);
}
sub ResultCfg {
	my($f,$z,$m,$i);
	$NamedCache and do {
		open IN, $NamedCache or do {
			Error("Can't open source hint file '$NamedCache'. $!");
			return 1;
		};
		$f = $NamedDir . $Named_cache;
		CreateFile($f) and return;
		while(<IN>) {
			print OUT;
		}
		close OUT or do { Error("$!"); return 1; };
		checkFiles($f,0);
	};
	$f = $NamedDir . $Named_cfg;
	CreateFile($f) and return 1;
	$_ = defined $Directory ? $Directory : $NamedDir;
	s/\/$//;
	$_ or $_ = "/";
	print OUT "options {\n\tdirectory \"$_\";\n";
	foreach $_ (@NamedOpt) {
		s/^\s//;
		print OUT "\t$_\n";
	}
	%NamedFor and do {
		print OUT "\tforwarders {\n";
		foreach $i (keys %NamedFor) {
			foreach $_ (sort @{$HostIP{$i}}) {
				print OUT "\t\t" . getarpa($_) . ";\n";
			}
		}
		print OUT "\t};\n";
	};
	print OUT "};\n\n";
	foreach $_ (@NamedCfg) {
		s/^\s//;
		print OUT "$_\n";
	}
	print OUT "\n";
	$NamedCache and do {
		print OUT
		"zone \".\" {\n\ttype hint;\n\tfile \"$Named_cache\";\n};\n\n";
	};
	defined $Root and PrintZone($Named_root,'.',$Root);
	foreach $z (sort by_domain keys %ZoneD) {
		PrintZone($z,$z,$Zone{$z}{''});
	}
	foreach $z (sort keys %ZoneIP) {
		foreach $m (sort keys %{$ZoneIP{$z}}) {
			PrintZone(arpaf($z,$m),revarpa($z,$m),$Zone{$z}{$m});
		}
	}
	close OUT or do { Error("$!"); return 1; };
	return checkFiles($f,0);
}
sub PrintZone {
	my($f,$z,$i,$j,$t) = @_;
	$t = $DB_Zone[$i][0];
	if($t eq '.Secondary') {
		$t = 'slave';
	} elsif($t eq '.Stub') {
		$t = 'stub';
	} else {
		$t = 'master';
	}
	$_ = $DB_Zone[$i][1] . $f;
	s/^$NamedDir//;
	print OUT "zone \"$z\" {\n\ttype $t;\n\tfile \"$_\";\n";
	$t ne 'master' and do {
		print OUT "\tmasters {\n";
		foreach $j (sort keys %{$DB_Zone[$i][2]}) {
			foreach $_ (sort @{$HostIP{$j}}) {
                                print OUT "\t\t" . getarpa($_) . ";\n";
			}
		}
		print OUT "\t};\n";
	};
	foreach $_ (@{$DB_Zone[$i][3]}) {
		s/^\s//;
		print OUT "\t$_\n";
	}
	print OUT "};\n\n";
}
sub ResultRoot {
	defined $Root or return 0;
	my($f,$i,$s,%n) = ($NamedDir . $Named_root,$Root);
	$s = $Serial;
	$_ = GetSerial($f);
	$s > $_ or $s = $_ +1;
	CreateFile($f) and return 1;
	PrintOUT(";\n;\n.");
	PrintOut("IN\tSOA\t" .
		$DB_Zone[$i][4] . '. ' . $DB_Zone[$i][5] .
		". (\n\t\t\t\t$s\n\t\t\t\t@{$DB_Zone[$i]}[6..9] )");
	foreach $_ (sort keys %{$DB_Zone[$i][2]}) {
		PrintOut("IN\tNS\t$_.");
		$n{$_} = 1;
	}
	delete $RootD{'localhost'};
	delete $RootIP{$IP127-1};
	foreach $i (sort keys %RootD) {
		PrintOUT("$i.");
		$_ = $Zone{$i}{''};
		foreach $_ (sort keys %{$DB_Zone[$_][2]}) {
			PrintOut("IN\tNS\t$_.");
			$n{$_} = 1;
		}
	}
	foreach $i (sort keys %RootIP) {
		$s = $RootIP{$i};
		PrintOUT(revarpa($i,$s) . '.');
		$_ = $Zone{$i}{$s};
		foreach $_ (sort keys %{$DB_Zone[$_][2]}) {
			PrintOut("IN\tNS\t$_.");
			$n{$_} = 1;
		}
	}
	foreach $i (sort keys %n) {
		PrintOUT("$i.");
		foreach $_ (sort @{$HostIP{$i}}) {
			PrintOut("IN\tA\t" . getarpa($_));
		}
	}
	close OUT or do { Error("$!"); return 1; };
	return checkFiles($f,$SerialLine);
}
sub ResultDZone {
	my($z,$i,$f,$s,$h,$k,%n,%m);
	foreach $z (sort keys %ZoneD) {
		$i = $Zone{$z}{''};
		$DB_Zone[$i][0] ne '.Primary' and next;
		$f = $DB_Zone[$i][1] . $z;
		$s = $Serial;
		$_ = GetSerial($f);
		$s > $_ or $s = $_ +1;
		CreateFile($f) and return 1;
		%m = %n = ();
		PrintOUT('$ORIGIN');
		PrintOut("$z.");
		PrintOUT('$TTL');
		PrintOut("$DB_Zone[$i][9]\n\@\t\tIN\tSOA\t" .
			stripzone($DB_Zone[$i][4],$z) . ' ' .
			stripzone($DB_Zone[$i][5],$z) .
			" (\n\t\t\t\t$s\n" .
			"\t\t\t\t@{$DB_Zone[$i]}[6..9] )");
		foreach $_ (sort keys %{$DB_Zone[$i][2]}) {
			PrintOut("IN\tNS\t" . stripzone($_,$z));
			/(.+)\.$z$/ and do {
				$n{$_} = $1;
			};
		}
		ResultZoneHost($z,$z);
		foreach $h (sort @{$ZoneD{$z}}) {
			$_ = $h;
			s/\.$z$//;
			PrintOUT($_);
			$_ = $Zone{$h}{''};
			foreach $_ (sort keys %{$DB_Zone[$_][2]}) {
				PrintOut("IN\tNS\t" . stripzone($_,$z));
				/(.+)\.$z$/ and do {
					$n{$_} = $1;
				};
			}
		}
		foreach $_ (@{$ZoneDHost{$z}}) {
			$m{$_} = 1;
		};
		%n and do {
			foreach $_ (keys %n) {
				$m{$_} and delete $n{$_};
			}
			while(($h,$s) = each %n) {
				PrintOUT($s);
				foreach $_ (sort @{$HostIP{$h}}) {
					PrintOut("IN\tA\t" . getarpa($_));
				}
			}
		};
		foreach $h (sort keys %m) {
			$_ = $h;
			s/\.$z$//;
			PrintOUT($_);
			ResultZoneHost($h,$z);
		}
		close OUT or do { Error("$!"); return 1; };
		checkFiles($f,$SerialLine) and return 1;
	}
	return 0;
}
sub ResultRZone {
	my($z,$m,$i,$f,$h,$s,$k,%n);
	foreach $z (sort keys %ZoneIP) {
		foreach $m (sort keys %{$ZoneIP{$z}}) {
			$i = $Zone{$z}{$m};
			$DB_Zone[$i][0] ne '.Primary' and next;
			$f = $DB_Zone[$i][1] . arpaf($z,$m);
			$h = revarpa($z,$m);
			$s = $Serial;
			$_ = GetSerial($f);
			$s > $_ or $s = $_ +1;
			CreateFile($f) and return 1;
			PrintOUT('$ORIGIN');
			PrintOut("$h.");
			PrintOUT('$TTL');
			PrintOut("$DB_Zone[$i][9]\n\@\t\tIN\tSOA\t" .
				$DB_Zone[$i][4] . '. ' .
				$DB_Zone[$i][5] .
				". (\n\t\t\t\t$s\n" .
				"\t\t\t\t@{$DB_Zone[$i]}[6..9] )");
			foreach $_ (sort keys %{$DB_Zone[$i][2]}) {
				PrintOut("IN\tNS\t$_.");
			}
			%n = @{$ZoneIP{$z}{$m}};
			foreach $h (sort keys %n) {
				$s = $n{$h};
				$k = $Zone{$h}{$s};
				PrintOUT('$ORIGIN');
				PrintOut(revarpa($h,$s) . '.');
				PrintOUT('@');
				foreach $_ (sort keys %{$DB_Zone[$k][2]}) {
					PrintOut("IN\tNS\t$_.");
				}
			}
			$k = 0;
			foreach $h (sort @{$ZoneIPHost{$z}{$m}}) {
				$s = $h & ~255;
				$s != $k and do {
					$k = $s;
					PrintOUT('$ORIGIN');
					PrintOut(revarpa($k,24) . '.');
				};
				PrintOUT($h & 255);
				$_ = $IP{$h};
				$_ = $DB_Host[$_][0];
				PrintOut("IN\tPTR\t$_.");
			}
			close OUT or do { Error("$!"); return 1; };
			checkFiles($f,$SerialLine) and return 1;
		}
	}
	return 0;
}
sub stripzone {
	my($d,$z) = @_;
	$d =~ s/\.$z$// and return $d;
	return "$d.";
}
sub ResultZoneHost {
	my($d,$z,$t,%s) = @_;
	if(defined $HostIP{$d}) {
		foreach $_ (sort @{$HostIP{$d}}) {
			PrintOut("IN\tA\t" . getarpa($_));
		}
	} elsif(defined $Host{$d}) {
		$_ = $DB_Host[$Host{$d}{''}][4][0];
		$_ ne '' and do {
			PrintOut("IN\tCNAME\t" . stripzone($_,$z));
			return;
		};
	}
	$t = $d;
	$t =~ s/^[^\.]+/*/;
	foreach $_ (keys %{$MX{$t}}) {
		$s{$_} = $MX{$t}{$_};
	}
	foreach $_ (keys %{$MX{$d}}) {
		$s{$_} = $MX{$d}{$_};
	}
	foreach $_ (sort { $s{$a} <=> $s{$b} or $a cmp $b } keys %s) {
		PrintOut("IN\tMX $s{$_}\t" . stripzone($DB_Mail[$_],$z));
	}
}
sub ResultNet {
	my($f,$v,$l,@s) = @_;
	CreateFile($f) and return 1;
	print OUT '<table border=1><th>Lan<th>Host<th>Line
';
	@s = ();
	foreach $l (keys %Line) {
		foreach $_ (@{$Line{$l}}) {
			$v = $DB_Host[$_][3];
			$v = defined $v ? $DB_Vlan[$v]: '?';
			push @s,"$v<td>$DB_Host[$_][0]$DB_Host[$_][1]<td>$l";
		}
	}
	foreach $_ (sort @s) {
		print OUT "<tr><td>$_\n";
	}
	print OUT '</table>';
	close OUT or do { Error("$!"); return 1; };
	return checkFiles($f,0);
}
sub ResultLine {
	my($f,$l,$s,$t,$v) = @_;
	CreateFile($f) and return 1;
	print OUT '<table border=1><th>Line<th>Source Host<th>Target Host<th>Lan
';
	foreach $l (sort keys %Line) {
		($s,$t) = @{$Line{$l}};
		if(defined $t) {
			$Host{$DB_Host[$s][0]}{'?'} <
			 $Host{$DB_Host[$t][0]}{'?'} and ($s,$t) = ($t,$s);
			$t = $DB_Host[$t][0] . $DB_Host[$t][1];
		} else {
			$t = '';
		}
		$v = $DB_Host[$s][3];
		$v = defined $v ? $DB_Vlan[$v]: '?';
		$s = $DB_Host[$s][0] . $DB_Host[$s][1];
		print OUT "<tr><td>$l<td>$s<td>$t<td>$v\n";
	}
	print OUT '</table>';
	close OUT or do { Error("$!"); return 1; };
	return checkFiles($f,0);
}
sub ResultHost {
	my($f,$d,$p,$l,$v,$i,$t) = @_;
	CreateFile($f) and return 1;
	print OUT '<table border=1><th>Source Host<th>Target Host<th>Line<th>Lan
';
	foreach $d (sort keys %Host) {
		foreach $p (sort keys %{$Host{$d}}) {
			$p eq '?' and next;
			$i = $Host{$d}{$p};
			$l = $DB_Host[$i][2];
			$t = '?';
			if($l eq '?') {
				$p eq '' and next;
			} else {
				foreach $_ (@{$Line{$l}}) {
					$_ == $i and next;
					$t = $DB_Host[$_][0] . $DB_Host[$_][1];
					last;
				}
			}
			$v = $DB_Host[$i][3];
			$v = defined $v ? $DB_Vlan[$v]: '?';
			print OUT "<tr><td>$d$p<td>$t<td>$l<td>$v\n";
		}
	}
	print OUT '</table>';
	close OUT or do { Error("$!"); return 1; };
	return checkFiles($f,0);
}
sub GetSerial {
	my($r) = 0;
	open IN,$_[0] and do {
		while(<IN>) {
			if($. == $SerialLine) {
				/^\s*(\d+)/ and $r = $1;
				last;
			}
		}
		close IN;
	};
	return $r;
}
sub CreateDir {
	my($j,$f,@s) = @_;
	$Stdout and return 0;
	$Create and return 0;
	@s = split /\/+/,$j;
	$#s > 0 and do {
		unless($s[0]) { $f = '/'; }
		else { $f = $s[0]; }
		shift @s;
		do {
			not -d $f and do {
				mkdir $f,0755 or do {
					Error("$!. Can't create directory $f");
					return 1;
				};
				print STDERR "Create directory '$f'.\n";
			};
			$f eq '/' and $f = '';
			$f = $f . '/' . shift @s;
		} while($#s >= 0);
	};
	return 0;
}
sub CreateFile {
	my($j) = @_;
	$Stdout and do {
		print "*** Result file: $j\n";
		open OUT,'>-';
		return 0;
	};
	$Create and do {
		open OUT,">$TmpFile" or do {
			Error("$!. Can't create temporary file '$TmpFile'");
			return 1;
		};
		return 0;
	};
	$j .= '.tmp';
	#CreateDir($j) and return 1;
	open OUT,">$j" or do {
		Error("$!. Can't create file '$j'");
		return 1;
	};
	return 0;
}
sub checkFiles {
	my($of,$l,$nf,$o,$n) = @_;
	$Stdout and return 0;
	if($Create) { $nf = $TmpFile; }
	else { $nf = $of . '.tmp'; }
	if(-e $of) {
		if(open OLD,$of) {
			open NEW,$nf or do {
				close OLD;
				Error("$!. Can't open file '$nf'");
				return 1;
			};
			for($_ = 0;;) {
				$o = <OLD>;
				$n = <NEW>;
				not defined $o and not defined $n and last;
				defined $o and defined $n and do {
					$o eq $n and next;
					$. == $l and next;
				};
				$_ = 1;
				last;
			}
			close NEW;
			close OLD;
		} else {
			Error("$!.");
			unlink $nf;
			return 1;
		}
	} else {
		$_ = 1;
	}
	if($_ == 1) {
		if($Create) {
			unlink $nf;
		} else {
			rename $of, $of . '.old';
			rename $nf, $of;
			chmod 0444, $of;
		}
		print STDERR "File '$of' changed.\n";
		$IsChange = 1;
	} elsif($_ == 0) {
		unlink $nf;
		$Report and print STDERR "File '$of' not changed.\n";
	}
	return 0;
}
sub doHost {
	$_[0] =~ /^([^\/]+)(.*)$/;
	my($d,$p,$l,$v) = ($1,$2,'?',undef);
	checkdomain($d) or return "'$d' not domain";
	$d = setdomain($d);
	$p = '' if not defined $p;
	defined $Host{$d}{$p} and return "'$d$p' already registered";
	push @DB_Host,[($d,$p,$l,$v,[()])];
	$Host{$d}{$p} = $#DB_Host;
	not defined $_[1] and return 0;
	$_[1] eq '*' and do {
		$p and return "Port must be empty for this type of record";
		$#_ > 2 and return 'To many parameters';
		if(defined $_[2]) {
			checkdomain($_[2]) or return "'$_[2]' not domain";
			$v = setdomain($_[2]);
			push @{$DB_Host[-1][4]},$v;
			$CNAME{$d} = $v;
		} else {
			push @{$DB_Host[-1][4]},'';
			$MailOnly{$d} = '';
		}
		return 0;
	};
	my(@s,@a,%T) = @_;
	@a = ();
	shift @s;
	$l = 2;
	foreach $v (@s) {
		checkarpa($v) or do {
			$l or return "'$v' not ip address";
			if($l == 2) {
				$v ne '?' and do {
					$DB_Host[-1][2] = $v;
					defined $Line{$v} or $Line{$v} = [()];
					defined $Line{$v}[1] and return "Multiple definition of line '$v'";
					push @{$Line{$v}},$#DB_Host;
				};
			} else {
				$v ne '?' and do {
					$_ = $Vlan{$v};
					defined $_ or do {
						push @DB_Vlan,$v;
						$_ = $Vlan{$v} = $#DB_Vlan;
					};
					$DB_Host[-1][3] = $_;
				};
			}
			$l--;
			next;
		};
		defined $T{$v} and return "Duble ip address '$v'";
		$T{$v} = $l = 0;
		$v = setarpa($v);
		push @a,$v;
		defined $IP{$v} or $IP{$v} = $#DB_Host;
	}
        if($#a >= 0) {
		push @{$DB_Host[-1][4]},@a;
		defined $HostIP{$d} or $HostIP{$d} = [()];
		push @{$HostIP{$d}},@a;
	}
	return 0;
}
sub doMailHost {
	$#_ > 1 and return 'To many parameters';
	my($d,$p) = @_;
	$d ne '*' and do {
		$_ = ($d =~ /^\*\.(.*)$/) ? $1 : $d;
		checkdomain($_) or return "'$d' not domain";
	};
	$d = setdomain($d);
	$p = defined $p ? $p : $Mail{$DB_Mail[-1]};
	$p =~ /\D/ and return "Priority '$p' not number";
	defined $MX{$d}{$#DB_Mail} and return
		"Mailserver '$DB_Mail[-1]' for '$d' already registered";
	$MX{$d}{$#DB_Mail} = $p;
	return 0;
}
sub RegNetwork {
	my($n,$d,$m) = @_;
	for(;;) {
		if($Level eq '.Vlan') {
			defined $Vnet{$d}{$m} and last;
			$Vnet{$d}{$m} = $#DB_Vlan;
			defined $IPvlan{$m} or $IPvlan{$m} = [()];
			push @{$IPvlan{$m}},$d;
		} else {
			defined $Zone{$d}{$m} and last;
			$Zone{$d}{$m} = $#DB_Zone;
			if($m eq '') {
				$ZoneD{$d} = [()];
				$ZoneDHost{$d} = [()];
				$_ = RegFileAll($DB_Zone[-1][1] . $d);
				$_ and return $_;
			} else {
				defined $IPzone{$m} or $IPzone{$m} = [()];
				push @{$IPzone{$m}},$d;
			}
		}
		return 0;
	}
	return "Network '$n' already registered";
}
sub doNetwork {
	my($n,$m,$r);
	foreach $r (@_) {
		$_ = $r;
                /^([^\/]+)(.*)/;
		($n,$m) = ($1,$2);
		if(checkdomain($n)) {
			$Level eq '.Vlan' and
				return "Domain network not enable for '.Vlan'";
			$m and return "Bitmask not enable for domain network";
			$m = '';
			$n = setdomain($n);
		} elsif(checkarpa($n)) {
			$n = setarpa($n);
			$m = '/24' if $m eq '';
			$_ = $m;
			/^\/(.*)$/;
			$_ = $1;
			$_ eq '' and return "No value for bitmask";
			/^[1-9]+\d?$/ or return "Bitmask '$m' incorrect";
			if($_ < 8 or $_ > 24) {
				return "Bitmask must be in range from 8 to 24";
			}
			$n << $_ and return
				 "Bitmask too big for network '$r'";
			$m = $_;
		} else {
			return "'$r' not network name";
		}
		$_ = RegNetwork($r,$n,$m) and return $_;
	}
	return 0;
}
sub doInclude {
	$#_ == 1 or return "Incorrect number parameters for '$_[0]'";
	my($in,$f,@e,@s) = @_;
	$in = $IN++;
	open $in,$f or return "'$f'. $!";
	CIRCLE: while(<$in>) {
		s/\s*(#.*)?\n//g;
		$_ or $Text or next;
		if(/^\s*(\.\w+)/) {
			@s = split;
			if($s[0] eq '.Domain') {
				push @e,doDomain(@s);
			} elsif($s[0] eq '.Include') {
				push @e,doInclude(@s);
			} elsif($s[0] eq '.End') {
				push @e,doEnd('');
			} elsif($s[0] eq '.MailServer') {
				push @e,doEnd($s[0]),doMailServer(@s);
			} elsif($s[0] eq '.NameServer') {
				push @e,doNameServer(@s);
			} elsif($s[0] eq '.Option') {
				push @e,doOption($_);
			} elsif($s[0] eq '.Vlan') {
				push @e,doEnd($s[0]),doVlan(@s);
			} elsif($s[0] eq '.Primary') {
				push @e,doEnd($s[0]),doZone(@s);
			} elsif($s[0] eq '.Secondary') {
				push @e,doEnd($s[0]),doZone(@s);
			} elsif($s[0] eq '.Stub') {
				push @e,doEnd($s[0]),doZone(@s);
			} elsif($s[0] eq '.Bind') {
				push @e,doBind(@s);
			} elsif($s[0] eq '.Time') {
				push @e,doTime(@s);
			} elsif($s[0] eq '.Named') {
				push @e,doEnd($s[0]),doNamed(@s);
			} elsif($s[0] eq '.Config') {
				push @e,doConfig($_);
			} elsif($s[0] eq '.Cache') {
				push @e,doCache(@s);
			} elsif($s[0] eq '.Forwarders') {
				push @e,doForwarders(@s);
			} elsif($s[0] eq '.Html') {
				push @e,doEnd($s[0]),doHtml(@s);
			} elsif($s[0] eq '.Line') {
				push @e,doTable(@s);
			} elsif($s[0] eq '.Host') {
				push @e,doTable(@s);
			} elsif($s[0] eq '.Net') {
				push @e,doTable(@s);
			} elsif($s[0] eq '.Root') {
				push @e,doEnd($s[0]),doRoot(@s);
			} else {
				push @e,"Unknown key '$s[0]'";
			}
			unless($s[0] eq '.Option' or $s[0] eq '.Config') {
				$Text = 0;
			}
		} else {
			$Text and do {
				push @$Text,$_;
				next;
			};
			$Text = 0;
			@s = split;
			if($Level) {
				if($Level eq '.MailServer') {
					push @e,doMailHost(@s);
				} elsif($Level eq '.Named' or
					$Level eq '.Html' or
					$Level eq '.Root') {
					push @e,doEnd('');
					push @e,doHost(@s);
				} else {
					push @e,doNetwork(@s);
				}
			} else {
				push @e,doHost(@s);
			}
		}
		for $_ (@e) {
			$_ or next;
			print STDERR "File $f: Line $.. $_.\n";
			++$E < $MaxErr or last CIRCLE;
		}
		@e = ();
	}
	close $in;
	$IN--;
	return 0;
}
sub doNamed {
	$NamedDir and return "'$_[0]' already defined";
	$#_ == 1 or $#_ == 0 or
		return "Incorrect number parameters for '$_[0]'";
	my($n) = $_[1] ? "$_[1]/" : '';
	my(@s) = mkPath($n,"$Pwd/");
	$s[0] == -1 and return "Parameter '$n' not pathname";
	$_ = RegFile($s[2]) and return $_;
	$DB_Zone[0][1] = $NamedDir = $s[2];
	@s = mkPath($Named_cfg,$NamedDir);
	return RegFileAll($s[2]);
}
sub doRoot {
	$NamedDir or return "'.Named' must be defined before '$_[0]";
	$#_ == 0 or return "Incorrect number parameters for '$_[0]'";
	push @DB_Zone,[$_[0],$NamedDir,{},[()],'','',0,0,0,0];
	$Root = $#DB_Zone;
	return "Defined '.Cache' or '.Forwarders' before '.Root'" if
		$NamedCache or %NamedFor;
	return RegFileAll($NamedDir . $Named_root);
}
sub doZone {
	$NamedDir or return "'.Named' must be defined before '$_[0]";
	$#_ == 1 or $#_ == 0 or
		return "Incorrect number parameters for '$_[0]'";
	my($n) = $_[1] ? "$_[1]/" : '';
	my(@s) = mkPath($n,$NamedDir);
	$s[0] == -1 and return "Parameter '$n' not pathname";
	($s[0] & 2) == 1 and return "'$n' is absolute pathname";
	push @DB_Zone,[$_[0],$s[2],{},[()],'','',0,0,0,0];
	return 0;
}
sub doMailServer {
	$#_ == 1 or $#_ == 2 or
		return "Incorrect number parameters for '$_[0]'";
	checkdomain($_[1]) or return "'$_[1]' not domain";
	my($m) = setdomain($_[1]);
	defined $Mail{$m} and return "'$_[0] $m' already defined";
	my($p) = $_[2];
	defined $p or $p = 0;
	$p =~ /\D/ and return "Priority must be digital";
	push @DB_Mail,$m;
	$Mail{$m} = $p;
	$MX{$m}{$#DB_Mail} = 0;
	return 0;
}
sub doNameServer {
	$Level eq '.Primary' or $Level eq '.Secondary' or
		$Level eq '.Stub' or $Level eq '.Root' or
		return "'$_[0]' not enable in current statement";
	$#_ >= 1 or return "Must be parameter[s] for '$_[0]'";
	my($k,@s,$i) = @_;
	foreach $i (@s) {
		checkdomain($i) or return "'$i' not domain";
		$i = setdomain($i);
		defined $DB_Zone[-1][2]{$i} and
			return "Duplicate '$i' as '$k'";
		$DB_Zone[-1][2]{$i} = 1;
	};
	return 0;
}
sub doForwarders {
	$Level eq '.Named' or return "'$_[0]' not enable in current statement";
	$#_ >= 1 or return "Must be parameter[s] for '$_[0]'";
	my($k,@s,$i) = @_;
	foreach $i (@s) {
		checkdomain($i) or return "'$i' not domain";
		$i = setdomain($i);
		defined $NamedFor{$i} and return "Duplicate '$i' as '$k'";
		$NamedFor{$i} = 1;
	};
	return 0;
}
sub doCache {
	$Level eq '.Named' or return "'$_[0]' not enable in current statement";
	$#_ == 1 or return "Must be parameter for '$_[0]'";
	$NamedCache and return "'$_[0]' already registered";
	my(@s) = mkPath($_[1],"$Pwd/");
	$s[0] == -1 and return "Parameter '$_[1]' not pathname";
	($s[0] & 1) != 0 and return "Parameter '$_[1]' is directory";
	$NamedCache = $s[2];
	-r $NamedCache or return "Can't read source hint file '$NamedCache'";
	@s = mkPath($Named_cache,$NamedDir);
	return RegFileAll($s[2])
}
sub doBind {
	$Level eq '.Primary' or $Level eq '.Root' or
		return "'$_[0]' not enable in current statement";
	$#_ == 1 or $#_ == 2 or
                return "Incorrect number parameters for '$_[0]'";
	$DB_Zone[-1][4] and return "'$_[0]' already registered";
	checkdomain($_[1]) or return "'$_[1]' not domain";
	$DB_Zone[-1][4] = setdomain($_[1]);
	my($n) = $_[2];
	defined $n or $n = 'root.' . $DB_Zone[-1][4];
	checkdomain($n) or return "'$n' not domain";
	$DB_Zone[-1][5] = setdomain($n);
	return 0;
}
sub doTime {
	$Level eq '.Primary' or $Level eq '.Root' or
		return "'$_[0]' not enable in current statement";
	$#_ == 4 or return "Incorrect number parameters for '$_[0]'";
	$DB_Zone[-1][6] and return "'$_[0]' already registered";
	my($i,@s) = @_;
	foreach $_ (@s) {
		/\D/ and return "Value '$_' not number";
		$_ > 0 or return "Value '$_' too big";
		$_ > 60 or return "Value '$_' too small";
	}
	$s[1] <= $s[0] / 3 or return "Refresh time less them retry time * 3";
	@{$DB_Zone[-1]}[6..9] = @s;
	return 0;
}
sub doVlan {
	my(@s) = @_;
	$#s == 0 and return "Incorrect number parameters for '$s[0]'";
	$s[1] eq '?' and return "'?' as parameter '$s[0]' not enabled";
	defined $Vlan{$s[1]} and return "'@s[0..1]' already registered";
	push @DB_Vlan,$s[1];
	$Vlan{$s[1]} = $#DB_Vlan;
	shift @s;
	shift @s;
	$#s >= 0 and return doNetwork(@s);
	return 0;
}
sub doEnd {
	my($e) = '';
	$Text and return $e;
	$_[0] or $Level or
		return "'$_[0]' not enable in current statement";
	$Level = $DB_Zone[-1][0];
	if($Level eq '.Primary' or $Level eq '.Root') {
		$DB_Zone[-1][4] or
			$e = "Not defined '.Bind' for '$Level' zone";
	} elsif($Level eq '.Secondary' or $Level eq '.Stub') {
		$DB_Zone[-1][2] or
			$e = "Not defined '.NameServer' for '$Level' zone";
	}
	$Level = $_[0];
	return $e;
}
sub doConfig {
	$Level eq '.Named' or
		return "'.Config' not enable in current statement";
	$Text = \@NamedCfg;
	s/^\s*\.Config\s*//;
	$_ and push @$Text, $_;
	return 0;
}
sub doOption {
	$Level eq '.Primary' or $Level eq '.Secondary' or $Level eq '.Stub' or
		$Level eq '.Named' or $Level eq '.Root' or
		return "'.Option' not enable in current statement";
	$Text = ($Level eq '.Named') ? \@NamedOpt : \@{$DB_Zone[-1][3]};
	s/^\s*\.Option\s*//;
	$_ and push @$Text, $_;
	return 0;
}
sub doDomain {
	$#_ == 0 or $#_ == 1 or
                return "Incorrect number parameters for '$_[0]'";
	$Domain = '';
	$Domain = setdomain($_[1]);
	return 0;
}
sub doHtml {
	$Level = $_[0];
	$#_ == 1 or $#_ == 0 or
                return "Incorrect number parameters for '$_[0]'";
	my($n) = $_[1] ? "$_[1]/" : '';
        my(@s) = mkPath($n,"$Pwd/");
        $s[0] == -1 and return "Parameter '$n' not pathname";
	$Html{$_[0]} = $s[2];
        return RegFile($s[2]);
}
sub doTable {
	$Level eq '.Html' or return "'$_[0]' not enable in current statement";
	defined $_[1] or return "Must be parameter for '$_[0]'";
	defined $Html{$_[0]} and return "'$_[0]' already registered";
	my(@s) = mkPath($_[1],$Html{'.Html'});
	$s[0] == -1 and return "Parameter '$_[1]' not pathname";
	($s[0] & 1) != 0 and return "Parameter '$_[1]' is directory";
	$Html{$_[0]} = $s[2];
        return RegFileAll($s[2]);
}
sub by_domain {
	my($i,$j,@A,@B) = 0;
	@A = split /\./,$a;
	@B = split /\./,$b;
	for(;;) {
		$j = pop @A;
		$i = pop @B;
		not defined $j and not defined $i and last;
		defined $j and not defined $i and return 1;
		not defined $j and defined $i and return -1;
		$i = ($j cmp $i) or next;
		return $i;
	}
	return 0;
}
sub checkarpa {
	if($_[0] =~ /^[1-9]\d{0,2}(\.(0|[1-9]\d{0,2})){3}$/) {
		foreach $_ (split /\./,$_[0]) {
			$_ < 256 or return 0;
		}
		return 1;
	}
	return 0;
}
sub setarpa {
	my(@s) = split /\./, $_[0];
	return ((($s[0] << 24) | ($s[1] << 16)) | ($s[2] << 8)) | $s[3];
}
sub getarpa {
	return join '.',($_[0] >> 24) & 255,($_[0] >> 16) & 255,
			($_[0] >> 8) & 255,$_[0] & 255;
}
sub arpaf {
	my($d,$m,@r) = @_;
	@r = (($d >> 24) & 255,($d >> 16) & 255,($d >> 8) & 255);
	$m == 24 and return join '.',@r;
	$m == 16 and return join '.',@r[0..1];
	return $r[0];
}
sub revarpa {
	my($d,$m,@r) = @_;
	@r = (($d >> 8) & 255,($d >> 16) & 255,($d >> 24) & 255);
	if($m == 24) {
		$d = join '.',@r;
	} elsif($m == 16) {
		$d = join '.',@r[1..2];
	} else {
		$d = $r[2];
	}
	return $d . '.in-addr.arpa';
}
sub checkdomain {
	$_[0] and $_[0] =~ /^([a-zA-Z](-?[a-zA-Z\d])*\.*)+$/ and return 1;
	return 0;
}
sub setdomain {
	$_ = $_[0] or return '';
	tr/A-Z/a-z/;
	if(/(.+)\.$/) { $_ = $1; }
	else { $Domain and $_ = $_ . '.' . $Domain; }
	return $_;
}
sub RegFileAll {
        return RegFile($_[0],$_[0] . '.old',$_[0] . '.tmp');
}
sub RegFile {
	while(@_) {
		$_ = shift @_;
		if(s/\/$//) {
			defined $File{$_} and
				return "Directory $_ can't be use";
			$File{"$_/"} = '_';
		} else {
			if(defined $File{$_} or defined $File{"$_/"}) {
				return "Filename $_ can't be use";
			}
			$File{$_} = '_';
		}
	}
	return 0;
}
# mkPath path prefix_if_need postfix_if_need
# Code:
# -1	Error
# 0	"*"
# 1	"*/"
# 2	"/*"
# 3	"/*/"
# 4	""
# Return: Code_of_input_string Code_of_result_string result_string
sub mkPath {
	my($a,$b,$c,$e,@s,@d) = @_;
	if($a) {
		$_ = $a;
		#if(/[^\da-zA-Z\.\-\/]/) { return (-1,4,''); }
		$e = 0;
		if(/\/$/ or /\/\.\.$/ or /\/\.$/) {
			 $e |= 1;
			 if($c) { $_ = $_ . '/' . $c; }
		}
		if(/^\//) { $e |= 2; }
		if($e < 2 and $b) { $_ = $b . '/' . $_; }
	} else {
		$_ = '';
		if($c) { $_ = $c; }
		if($b) { $_ = $b . '/' . $_; }
		unless($_) { return (4,4,''); }
		$e = 4;
	}
	$c = 0;
	if(/\/$/ or /\/\.\.$/ or /\/\.$/) { $c |= 1; }
	if(/^\//) { $c |= 2; }
	@s = split /\/+/;
	while(@s) {
		$_ = shift @s;
		if($_ eq '.' or $_ eq '') { next; }
		if($_ eq '..' and @d) {
			$b = pop @d;
			if($b ne '..') { next; }
			push @d, $b;
		}
		push @d, $_;
	}
	$_ = join '/', @d;
	if(($c & 1) != 0) { $_ = $_ . '/'; }
	if(($c & 2) != 0) { $_ = '/' . $_; }
	s/^(\/\.\.)+//;
	if($_ eq '//') { $_ = '/'; }
	if($_ eq '/' and ($c & 2) == 0) { $_ = './'; }
	return ($e,$c,$_);
}
sub PrintOUT {
	if(length $_[0] > 7) {
		print OUT "$_[0]\t";
	} else {
		print OUT "$_[0]\t\t";
	}
	$HostOut = 1;
}
sub PrintOut {
	if($HostOut) {
		print OUT "$_[0]\n";
		$HostOut = 0;
	} else {
		print OUT "\t\t$_[0]\n";
	}
}
sub Error {
	print STDERR "Error: $_[0].\n";
	++$E;
	return 0;
}
sub Help {
	print STDERR 'Usage: netadm [options] [configfile]
Options:
  --help	Display this information
  --version	Display the version of the programm
  -a		Report about all result files
  -c		Do not create result files, check only difference
  -d DIR	Use DIR as base directory in named.conf
  -s		Output all result files to STDOUT

Option -s cancel options -c and -a.
Default name of configfile is "netadm.conf".

	Report bugs to <ron@simcb.ru>
';
	exit 0;
}
sub Usage {
	$_[0] and print STDERR "netadm: $_[0]\n";
	print STDERR 'Usage: netadm [-a] [-c] [-d dir] [-s] [configfile]
';
	exit 1;
}
sub CmdParse {
	my($f,$i,$k,$v,$o,@s);
	for($f = $i = 0; $i <= $#ARGV;) {
		$_ = $ARGV[$i++];
		not $f and s/^-// and do {
			$_ eq '-' and $f = 1 and next;
			if(/^-/) {
				/^([^=]+)(=?)(.*)/;
				@s = ($1);
				if($2) { $v = $3; } else { undef $v; }
			} else {
				@s = split // or @s = ('');
				$v = $ARGV[$i];
			}
			foreach $_ (@s) {
				$k = $CmdHash{$_} or
					return "Unknown option '-$_'";
				$o = $CmdOpt{$_};
				defined $o and $k != 3 and
					return "Multiple use option '-$_'";
				if($k == 1) {
					/^-/ and defined $v and return "Option ' -$_' doesn't allow an argument";
					$CmdOpt{$_} = 1;
				} else {
					defined $v or return "Option '-$_' requires an argument";
					$k == 3 and defined $o and $v = join "\n", $o, $v;
					$CmdOpt{$_} = $v;
					/^-/ or $i++;
				}
				$v = $ARGV[$i];
			}
			next;
		};
		push @CmdArg, $_;
	}
	return '';
}
