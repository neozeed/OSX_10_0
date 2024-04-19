#
# A simple awk script to make sure SIG records contain the label count in
# text format 
# This program handles:
#     $ORIGIN lines.
#     lines starting with a name
#     lines starting with @  
#     special rule for label count for wild card records
#
# Not handled: 
#     names where . is not a label separator 
#
# Author: Olafur Gudmundsson ogud@tislabs.com
# Date:  1999-Sept
#

BEGIN{ 
	tail = substr(".",1,1);
	name = 0;  # name not seen
}

# processing of $ORIGIN directive
/^\$ORIGIN/ {
	olab = split($2,list,"."); 
	if (substr($2,length($2),1) == tail)
		olab--;
}

#
# all new names start at beginning of a line and must start with an
# alpanumeric character or the special two characters @ and *
#
/^[a-zA-Z0-9\*@]/ {
	name = $1;
	if (name == "@") # set name to last origin
		lab = olab;  
	else {  # normal name ? 
		lab = split(name,list,".");
		lastc = substr(name,length(name),1);
		if (lastc != tail)  # this is not FQDN add origin to it
		  lab += olab;
		else 	# FQDN 
		  lab--;
		if (substr(name, 1, 1) == "*")  # wildcard name decrement
		  lab--;
	}
}

# 
# now process the SIG records there are following fields on the line
# [name] <TTL> IN SIG  <alg> [<labels>] <OTTL> <expiry> <start> <keyid> <signer> (
# (assume this file was created by dnssigner that puts class and type in file
#
/ IN /&&/ SIG /{
	if ((name == 0) && (NF == 11)) { # no name on line labels missing 
		$5 = $5  " " lab;
		printf "\t\t\t%s\n", $0; # awk forgets leading spaces if 
		                         # one of $n is modified
		next;
	}
	else if((name != 0) && (NF==12)) { # name on line label missing 
		$6 = $6  " " lab;
	}
# else record is in new format. nothing to do
}

{ # print out current line
	print $0;
	name = 0;
}
