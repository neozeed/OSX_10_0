#! awk

BEGIN { host_count = 0 ;
  tmp = 1 ;
  bits = 0 ;
  for (bits=32;bits>=1;bits--) {
    bit_divisor[bits] = tmp ;
    tmp *= 2 ;
  }

  cleanup_list = "rm -f ";
  XFER = "rcp" ;
}

init == 0 {
  init++ ;
  if (soa_file == "")
    soa_file = "soa_file" ;
  if (serialnum == "")
   serialnum = "1" ;
  while (getline tmp <soa_file) {
    gsub(/\\t/, "\t", tmp) ;
    gsub(/SERIALNUM/, serialnum, tmp) ;
    soa_format = soa_format tmp "\n" ;
  }
}

$0 ~ /^#/ { next ; }

$1 == "127.0.0.1" { next ; }

NF < 2 { next ; }

# domain <domain name> <name server> <domain contact> [private]

$1 == "domain" {
  if ( NF >= 4) {
    domains[$2]++ ;
    domain_ns[$2] = $3 ;
    domain_who[$2] = $4 ;
    if (NF == 5) {
      if ($NF == "private")
        domain_private[$2]++ ;
      else
        syntax_error("domain <domain name> <server name> " \
		     "<domain e-mail contact> [private]") ;
    }
  } else
    syntax_error("domain <domain name> <server name> "\
		 "<domain e-mail contact> [private]") ;
  next ;
}

$1 == "network" {
  if ( NF == 3 ) {
    networks[$2]++ ;
    network_part[$2]  = cider_to_ip($2) ;
    network_bits[$2] = cider_to_bits($2) ;
    network_domain[$2] = $3 ;
  } else
    syntax_error("network <network/bits> <domain>");
  next ;
}

$1 == "allowed-hosts" {
  for (i=2;i<=NF;i++) {
    if ($i ~ /^#/)
      break ;
    allowed_hosts = allowed_hosts " " $i " ;" ;
  }
  next ;
}

# mx <domain name> <mail server> [precedence ( == 0) ]
$1 == "mx" {
  domain = $2 ;
  fname = "named." domain ;
  mail_host = $3 ;
  mx[fname] = mx[fname] sprintf("%s.\tIN\tMX\t%d\t%s.\n", domain, \
  	$4 == "" ? "0" : $4, mail_host) ;
  next ;
}

$1 ~ /^([0-9]{1,3}\.){3}[0-9]{1,3}\/[1-9][0-9]*$/{
  host_count++ ;
  host_ip[$2] = cider_to_ip($1) ;
  domain = network_domain[cider_to_network($1) "/" cider_to_bits($1)] ;
  fname = "named." domain ;

  text[fname] = text[fname] sprintf("%s\tIN\tA\t%s\n", $2, cider_to_ip($1)) ;

  reverse_fname = "named." reverse_octets_network(cider_to_network($1), \
						  cider_to_bits($1)) ;
  text[reverse_fname] = text[reverse_fname] \
  sprintf("%s\tIN\tPTR\t%s." domain ".\n", \
	  reverse_octets_host(cider_to_host_part($1), cider_to_bits($1)), $2 );

  for (i=3;i<=NF;i++) {
    if ($i ~ /^#/)
      break;
    text[fname] = text[fname] sprintf( "%s\tIN\tCNAME\t%s." domain ".\n", \
       $i, $2 ) ;
  }

  rhost_fixups = rhost_fixups \
    sprintf("$1 ~ /(^%s$)|(^%s\.[a-z0-9\._]+$)/ { host[\"%s-\" $2] = \"%s\" ;" \
	" user[\"%s-\" $2] = $2 ;" \
	" domain[\"%s-\" $2] = \"%s\" ; next ; }\n", \
	$2, $2, $2, $2, $2, $2, domain ) ;
  next ;
}

# otherwise syntax error
{ syntax_error("bad keyword or pattern") ; }

END {
  if (host_count == 0)
    exit 0 ;
  if (errors > 0) {
    print errors " Errors - aborting" ;
    exit 1;
  }

  # figure out which name server is the top level one for this domain
  # and make all the rest 'forward only'
  order_name_servers() ;

  for (x in domains) {
    who = domain_who[x] "." domain_ns[x] "." x ;
    write_soa(x, domain_ns[x] "." x, who, "named." x) ;
    print "localhost\tIN A\t127.0.0.1" >"named." x ;

    write_soa("0.0.127", x, who, "named.local-" domain_ns[x]) ;
    print "1.0.0.127.in-addr.arpa.\tIN PTR\tlocalhost." \
      >"named.local-" domain_ns[x] ;
  }

  for (net in networks) {
    reverse_net = reverse_octets_network(network_part[net], network_bits[net]);
    if (domain_private[network_domain[net]] > 0) {
      domain_private[reverse_net ".in-addr.arpa"]++ ;
    }
    write_soa(reverse_net, \
	      domain_ns[network_domain[net]] "." network_domain[net], \
	      domain_who[network_domain[net]] "." \
	      domain_ns[network_domain[net]] "." network_domain[net], \
	      "named." reverse_net) ;
  }

  for (x in text) {
    print "creating " x ;
    print text[x] >x ;
  }

  for (x in mx) {
    print "creating mx records for " x ;
    printf ";\n;\tMail Exchange Records\n;\n" >x ;
    print mx[x] >x ;
  }

  # write named.conf files
  named_conf() ;

  # write transfer scripts
  write_transfer_scripts() ;

  # write .rhosts fixup script
  write_rhosts_fixup_script() ;

  # write cleanup script
  print cleanup_list >"make-clean.sh" ;

  print "Run the transfer scripts and the .rhosts fixups" ;
}

# utility functions
function insert_sort(x, n, i, j) {
  for (i=1;i<=n;i++) {
    if ( x >= host_ar[i] )
      continue ;
    for (j=n;j>=i;j--)
      host_ar[j+1] = host_ar[j];
    break;
  }
  n++;
  host_ar[i] = x;
  return n;
}

#conversion functions
function octets_to_x(octets, i, tmp) {
  split(octets, tmp_ar, ".") ;
  tmp = tmp_ar[1] ;
  for (i=2;i<=4;i++)
    tmp = 256 * tmp + tmp_ar[i] ;
  return tmp ;
}

function x_to_octets(x, tmp, i) {
  tmp = x % 256 ;
  for (i=2;i<=4;i++) {
    x = int( x / 256 ) ;
    tmp = ( x % 256 ) "." tmp ;
  }
  return tmp;
}

# entire ip address
function cider_to_ip(cider_addr) {
  return substr(cider_addr, 1, index(cider_addr, "/") - 1) ;
}

# number of bits in network part
function cider_to_bits(cider_addr) {
  split(cider_addr, tmp_ar, "/") ;
  return tmp_ar[2] ;
}

# returns network address given ip/bits
function cider_to_network(cider_addr, tmp) {
  tmp = bit_divisor[cider_to_bits(cider_addr)] ;
  return x_to_octets(tmp * int( octets_to_x(cider_to_ip(cider_addr)) / tmp )) ;
}

# returns host only part - network part deleted
function cider_to_host_part(cider_addr, tmp, tmp2) {
  tmp = bit_divisor[cider_to_bits(cider_addr)] ;
  tmp2 = octets_to_x(cider_to_ip(cider_addr)) ;
  tmp2 -= tmp * int( tmp2 / tmp ) ;

  return x_to_octets(tmp2);
  if (shift_bits <= 8)
    return tmp_ar[2] "." tmp_ar[3] "." tmp_ar[4] ;
  if (shift_bits <= 16)
    return tmp_ar[3] "." tmp_ar[4] ;
  if (shift_bits <= 24)
    return tmp_ar[4] ;
}

# returns reverse of network part of address in dotted decimal

function reverse_octets_network(octet, bits, n, i, tmp) {
  n = split(octet, tmp_ar, ".") ;
  if (bits <= 8)
    n = 1;
  else if (bits <= 16)
    n = 2 ;
  else if (bits <= 24)
    n = 3 ;
  tmp = tmp_ar[n] ;
  for (i=n-1;i>=1;i--)
    tmp = tmp "." tmp_ar[i] ;
  return tmp ;
}

# returns reverse of host part of address in dotted decimal
function reverse_octets_host(octet, bits, n, i, tmp) {
  split(octet, tmp_ar, ".") ;
  if (bits < 8)
    n = 1;
  else if (bits < 16)
    n = 2 ;
  else if (bits < 24)
    n = 3 ;
  else
    n = 4 ;
  tmp = tmp_ar[4] ;
  for (i=3;i>=n;i--) {
    tmp = tmp "." tmp_ar[i] ;
  }
  return tmp ;
}

# x.y.z yields z.y.x
function reverse_domain(domain, tmp, n) {
  n = split(domain, tmp_ar, ".") ;
  tmp = tmp_ar[n] ;
  for (i=n-1;i>=1;i--)
    tmp = tmp "." tmp_ar[i] ;
  return tmp;
}

# writes start of authourity record and name servers part
# of a zone file
function write_soa(domain, ns, who, fname, x) {
  printf soa_format, ns, who >fname ;
  print ";\tName Servers" > fname ;
  for (x in domain_ns)
     printf "\tIN NS  %s.%s.\n", domain_ns[x], x > fname;
  print ";" > fname ;
  print ";\tAddresses for names" > fname ;

  cleanup_list = cleanup_list " " fname ;
}

function syntax_error(msg) {
  printf "syntax error in %s [%d]: %s\n'%s'\n", FILENAME, FNR, msg, $0
  errors++ ;
}


# named.conf file

function named_conf(tmp) {
  for (x in networks) {
    domain_network[network_domain[x]] = x ;
  }
  for (x in domains) {
    print "creating named.conf for " x " - called named.conf-" x ;
    write_named_conf(x, "named.conf-" domain_ns[x], domain_ns[x]) ;
    cleanup_list = cleanup_list " named.conf-" domain_ns[x] ;
  }
}

function write_named_conf(domain, fname, server, tmp, x) {
  print "# named.conf created automatically by hosts2named\n\n" >fname ;

  if (options[domain] != "")
    printf "options {\n%s };\n\n", options[domain] >fname ;

  write_zone(domain, "master", "file \"/etc/named.d/named." domain "\" ;", fname );
  for (x in domains) {
    if (x == domain)
      continue ;

    write_zone(x, "slave", "masters { " host_ip[domain_ns[x]] " ; } ;", fname );
    
  }

  for (x in networks) {
    reverse_zone = reverse_octets_network(network_part[x], network_bits[x])
    if (network_domain[x] == domain)
     write_zone(reverse_zone ".in-addr.arpa", "master", \
     "file \"/etc/named.d/named." reverse_zone "\" ;", fname);
    else
     write_zone(reverse_zone ".in-addr.arpa", "slave", \
     "masters { " host_ip[domain_ns[network_domain[x]]] " ; } ;",  fname ) ; 
  }

  write_zone(".", "hint", "file \"/etc/named.d/named.ca\" ;", fname );
  write_zone("0.0.127.in-addr.arpa", "master", "file \"/etc/named.d/named.local\" ;", fname);
}

function write_zone(zone, type, text, fname) {
  printf "zone \"%s\" {\n\ttype %s;\n\t%s\n", zone, type, text >fname ;
  if (domain_private[zone] > 0 && allowed_hosts != "") {
    print "\tallow-query { " allowed_hosts " } ;" >fname ;
    print "\tallow-transfer { " allowed_hosts " } ;" >fname ;
  }
  print "};\n\n" >fname ;
}

# transfer scripts

function write_transfer_scripts(x) {
  for (x in domains) {
    server = domain_ns[x] ;
    fname = "xfer." server ".sh" ;
    if (XFER == "ftp") {
      print "ftp -v -n -i ${1:-" server "} <<XXX" >fname ;
      print "user root" >fname ;
#    print "passwd $1" >fname ;
      printf "put named.conf-%s /etc/named.conf\n", server >fname;
      for ( x in text)
	printf "put %s /etc/named.d/%s\n", x, x >fname;
      printf "put %s /etc/named.d/%s\n", "named.ca", "named.ca" >fname;
      printf "put %s /etc/named.d/%s\n", "named.local-" server, "named.local" \
	>fname;
      print "XXX" >fname ;
    } else {
      rcmd_transfer_func("named.conf-" server, "named.conf", server, fname) ;
      for ( x in text)
	rcmd_transfer_func(x, "/etc/named.d/" x, server, fname) ;
      rcmd_transfer_func("name.ca", "/etc/named.d/named.ca", server, fname) ;
      rcmd_transfer_func("named.local-" server, "/etc/named.d/named.local", \
		    server, fname) ;
    }
    cleanup_list = cleanup_list " " fname ;

    print "/bin/sh xfer." server ".sh <root-password for " server ">" ;
  }
}

function rcmd_transfer_func(src, trgt, server, fname) {
  printf "rsh -l root %s rm -f %s %s-\n", server, trgt, trgt >fname ;
  printf "rsh -l root %s mv %s %s-\n", server, trgt, trgt >fname ;
  printf "rcp %s root@%s:%s\n", src, server, trgt >fname ;
}

# .rhost file fixups
function write_rhosts_fixup_script(fname) {
  fname = "rhosts-fixup.sh" ;
  print "#! /bin/sh" >fname ;
  print "fixup() {" >fname ;
  print "cd $HOMEDIR" >fname ;
  print "[ ! -s .rhosts ] && return" >fname ;
  print "rm -f .rhosts-bak" >fname ;
  print "mv .rhosts .rhosts-bak\n" >fname ;
  print "awk '" >fname
  print rhost_fixups >fname ;
  print " # default rules\nNF == 0 { print \"\" ; next }\n" >fname ;
  print " $1 ~ /^#/ { print ; next }\n{ print \"# \" $0 }" >fname ;
  print "END { for (x in user) { printf \"%s\\t%s\\n%s.%s\\t%s\\n%s.%s\\t%s\\n\", " \
    "host[x], user[x],  host[x], domain[x], user[x], host[x], \"" domain_root "\", user[x] } }" >fname ;
  print "' .rhosts-bak >.rhosts" >fname ;
  print "chown $USER .rhosts" >fname ;
  print "chmod 400 .rhosts" >fname ;
  print "}" >fname ;

  print "" >fname ;
  print "awk -F: '{ print $1, $6 }' /etc/passwd |" >fname ;
  print "while read USER HOMEDIR ; do" >fname ;
  print "echo \"repairing .rhosts for $USER in $HOMEDIR\"" >fname ;
  print "# this is run in a subshell to nullify the cd $HOMEDIR" >fname ;
  print "( fixup $USER $HOMEDIR )" >fname ;
  print "done" >fname ;
  cleanup_list = cleanup_list " " fname ;

  print "on the home master machine, run: /bin/sh " fname ;
}

# name server heirarchy
function order_name_servers(x, len) {
  len = 1024 ;
  domain_root = "" ;
  for (x in domain_ns) {
    if (length(x) < len) {
      domain_root = x ;
      len = length(x) ;
    }
  }

  for (x in domain_ns) {
    if (x != domain_root)
      options[x] = sprintf("\tforward only;\n\tforwarders { %s ; } ;\n", \
			   host_ip[domain_ns[domain_root]] );
  }
}
