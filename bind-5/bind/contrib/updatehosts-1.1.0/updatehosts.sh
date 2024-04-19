#! @SH@
# $Id: updatehosts.sh,v 1.1.1.1 2001/01/31 03:59:15 zarzycki Exp $

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

# update the host tables and DNS files
#
# arguments are the files to edit

CONFIG_FILE=@sysconfdir@/updatehosts.env
DB_DIR=/var/named/db
EDITOR=${EDITOR-vi}
NAMED_DIR=/var/named
VERSION=8
RELOAD=1
RESTART=

# check if programs exists in user's PATH
in_path () {
	for prog do
		which $prog 2>&1 >/dev/null || return 1
	done
	return 0
}

# look for the startup file which sets global environment variables
[ -f $CONFIG_FILE ] && . $CONFIG_FILE

# read the options - command line overrides the configuration file
while getopts FVd:f:lnsv option; do
	case $option in
	F) FORCE_ZONE=-F;;
	d) DB_DIR=$OPTARG ;;
	f) CONFIG_FILE=$OPTARG
	   [ -f $CONFIG_FILE ] && . $CONFIG_FILE;;
	l) RELOAD=1 ;;
	n) RESTART=1 ;;
	r) USERCS=1 ;;
	s) USESCCS=1 ;;
	v) VERBOSE=-v ;;
	V) PR_VERSION=1 ;;
	esac
done
[ $OPTIND ] && shift `expr $OPTIND - 1`

if [ $PR_VERSION ]; then
	echo Updatehosts Version 1.1.0
	exit 0;
fi
# set the overridden variables
# unset Boolean variables set to "0" or "no"
[ "$GENBOOTP" = "0" -o "$GENBOOTP" = "no" ] && GENBOOTP=
[ "$GENDHCP" = "0" -o "$GENDHCP" = "no" ] && GENDHCP=
[ "$USERCS" = "0" -o "$USERCS" = "no" ] && USERCS=
[ "$USESCCS" = "0" -o "$USESCCS" = "no" ] && USESCCS=

list=
# check the filename arguments
for i do
	found=
	for j in $DB_FILES; do
		if [ $i = $j ]; then
			found=1
			break
		fi
	done
	if [ ! "$found" ]; then
		echo $i is not a valid filename
		list=1
	fi
done
if [ "$list" ]; then
	echo legal filenames are:
	echo $DB_FILES
	exit 1
fi

cd $NAMED_DIR || ( echo no $NAMED_DIR directory; exit 1 )

# set umask to group read and write
umask 2
# check if the files exist and are under SCCS control and are readable
#
# files are in a subdirectory - so we cd there
cd $DB_DIR || ( echo no $DB_DIR present; exit 1 )

okay=1
if [ "$USESCCS" ]; then
	# check for presence of sccs command
	if in_path sccs; then
		:
	else
		echo "cannot find sccs command"
		okay=0
	fi
	for i do
		if [ ! -f SCCS/s.$i ]; then
			echo "file $i is not under SCCS control"
			okay=0
		elif [ ! -r SCCS/s.$i ]; then
			echo "file $i is not readable by you"
			okay=0
		elif [ -f SCCS/p.$i ]; then
			echo "file $i is already checked out under SCCS"
			okay=0
		elif [ -f $i ]; then
			rm -f $i
		fi
	done
elif [ "$USERCS" ]; then
	# check for presence of rcs command
	if in_path co ci; then
		:
	else
		echo "cannot find rcs co or ci command"
		okay=0
	fi
	for i do
		if [ ! -f RCS/$i,v ]; then
			echo "file $i is not under RCS control"
			okay=0
		elif [ ! -r RCS/$i,v ]; then
			echo "file $i is not readable by you"
			okay=0
		elif [ -f $i ]; then
			rm -f $i
		fi
	done
fi

if [ $okay -eq 0 ]; then
	exit 1
fi

# checkout and edit the files
for i do
	if [ "$USESCCS" ]; then
		sccs edit $i
	elif [ "$USERCS" ]; then
		co -l $i
	fi
	$EDITOR $i
	if [ "$USESCCS" ]; then
		sccs delget $i
	elif [ "$USERCS" ]; then
		ci -u $i
	fi
done

cd $NAMED_DIR

# generate the static tables
if [ $STATICHOSTS ]; then
	echo "generating static hosts table..."
	HOSTS=$STATICHOSTS genstatic
fi

# generate the dns zone files
echo "generating the DNS database..."
[ ! "$ERROR_LEVEL" ] && ERROR_LEVEL=2
gendns -e $ERROR_LEVEL $FORCE_ZONE $VERBOSE -d $DB_DIR

# generate the bootptab file 
# but only if we are instructed to
if [ $GENBOOTP ]; then
	echo "generating the bootptab file..."
	genbootp

	# copy it to alternate machines
	for m in $SECBOOTP; do
		cp -p bootptab /net/$SECBOOTP/etc
	done
fi

# generate the dhcptab file 
# but only if we are instructed to
if [ $GENDHCP ]; then
	echo "generating the dhcpd.conf file..."
	if [ $VERBOSE ]; then
		VERBOSE=-v
	fi
	if [ $KNOWNHOSTS ]; then
		known=-k
	fi
	gendhcp $VERBOSE $known

	# copy it to alternate machines
	for m in $SECDHCP; do
		cp -p dhcpd.conf /net/$SECDHCP/etc/dhcpd.conf
	done

	# restart the server
	echo "restarting DHCP server..."
	poke_dhcp >/dev/null 2>&1
fi

# run postprocessor command
if [ "$POSTPROCESS" ]; then
	echo running $POSTPROCESS
	eval $POSTPROCESS
fi

if [ $RELOAD ]; then
	echo "reloading zone files..."
	poke_ns reload
elif [ $RESTART ]; then
	echo "restarting nameserver..."
	poke_ns restart
fi
