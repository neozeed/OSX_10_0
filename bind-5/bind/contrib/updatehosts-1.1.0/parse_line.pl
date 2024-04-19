# $Id: parse_line.pl,v 1.1.1.1 2001/01/31 03:59:15 zarzycki Exp $

# updatehosts DNS maintenance package
# Copyright (C) 1998  Smoot Carl-Mitchell
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

sub parse_line {
	local($_) = shift;

	return split(' ') if ! /["\\]/; 

	my(@chars) = split(//);
	$chars[$#chars+1] = " ";
	my($l) = $#chars;
	my($quoted) = 0;
	my($escaped) = 0;
	my($space) = 0;
	my($out) = "";
	my($nfields) = 0;
	my($i);
	my(@out);
	for ($i=0; $i<=$l; $i++) {
		if ($chars[$i] eq "\"") {
			if ($quoted) {
				$quoted = 0;
			}
			else {
				$quoted = 1;
			}
			$space = 0;
		}
		elsif ($chars[$i] eq "\\") {
			$escaped = 1;
			$space = 0;
		}
		elsif ($quoted) {
			$out .= $chars[$i];
			$space = 0;
		}
		elsif ($escaped) {
			$out .= $chars[$i];
			$escaped = 0;
			$space = 0;
		}
		elsif ($chars[$i] eq " " || $chars[$i] eq "	") {
			if ($space == 0) {
				$out[$nfields++] = $out;
				$out = "";
			}
			$space = 1;
		}
		else {
			$out .= $chars[$i];
			$space = 0;
		}
	}
	return @out;
}
