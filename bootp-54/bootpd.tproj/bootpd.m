/*
 * Copyright (c) 1999 Apple Computer, Inc. All rights reserved.
 *
 * @APPLE_LICENSE_HEADER_START@
 * 
 * The contents of this file constitute Original Code as defined in and
 * are subject to the Apple Public Source License Version 1.1 (the
 * "License").  You may not use this file except in compliance with the
 * License.  Please obtain a copy of the License at
 * http://www.apple.com/publicsource and read it before using this file.
 * 
 * This Original Code and all software distributed under the License are
 * distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY KIND, EITHER
 * EXPRESS OR IMPLIED, AND APPLE HEREBY DISCLAIMS ALL SUCH WARRANTIES,
 * INCLUDING WITHOUT LIMITATION, ANY WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE OR NON-INFRINGEMENT.  Please see the
 * License for the specific language governing rights and limitations
 * under the License.
 * 
 * @APPLE_LICENSE_HEADER_END@
 */
/*
 * bootpd.m
 * - BOOTP/DHCP server main
 * - see RFC951, RFC2131, RFC2132 for details on the BOOTP protocol, 
 *   BOOTP extensions/DHCP options, and the DHCP protocol
 */

/*
 * Modification History
 * 01/22/86	Croft	created.
 *
 * 03/19/86	Lougheed  Converted to run under 4.3 BSD inetd.
 *
 * 09/06/88	King	Added NeXT interrim support.
 *
 * 02/23/98	Dieter Siegmund (dieter@apple.com)
 *		- complete overhaul
 *		- added specialized Mac NC support
 *		- removed the NeXT "Sexy Net Init" code that performed
 *		  a proprietary form of dynamic BOOTP, since this
 *		  functionality is replaced by DHCP
 *		- added ability to respond to requests originating from 
 *		  a specific set of interfaces
 *		- added rfc2132 option handling
 *
 * June 5, 1998 	Dieter Siegmund (dieter@apple.com)
 * - do lookups using netinfo calls directly to be able to read/write
 *   entries and get subnet-specific bindings
 *
 * Oct 19, 1998		Dieter Siegmund (dieter@apple.com)
 * - provide domain name servers for this server if not explicitly 
 *   configured otherwise
 * Mar 29, 1999		Dieter Siegmund (dieter@apple.com)
 * - added code to do ethernet lookups with or without leading zeroes
 * April 27, 2000	Dieter Siegmund (dieter@apple.com)
 * - added netinfo host caching to avoid denial of service
 *   attacks and handle any valid format for the ethernet address
 *   i.e. leading zeroes, capitalization, etc.
 * - eliminated practice of supplying a default bootfile
 * - eliminated ability to read host entries from a file
 */

#import <unistd.h>
#import <stdlib.h>
#import <sys/stat.h>
#import <sys/socket.h>
#import <sys/ioctl.h>
#import <sys/file.h>
#import <sys/time.h>
#import <sys/types.h>
#import <net/if.h>
#import <netinet/in.h>
#import <netinet/in_systm.h>
#import <netinet/ip.h>
#import <netinet/udp.h>
#import <netinet/bootp.h>
#import <netinet/if_ether.h>
#import <net/if_arp.h>
#import <mach/boolean.h>
#import <signal.h>
#import <stdio.h>
#import <string.h>
#import <errno.h>
#import <ctype.h>
#import <netdb.h>
#import <setjmp.h>
#import <syslog.h>
#import <arpa/inet.h>
#import <arpa/nameser.h>
#import <sys/uio.h>
#import <resolv.h>

#import "arp.h"
#import "netinfo.h"
#import "interfaces.h"
#import "inetroute.h"
#import "subnetDescr.h"
#import "dhcp_options.h"
#import "rfc_options.h"
#import "macNC.h"
#import "bsdpd.h"
#import "NICache.h"
#import "host_identifier.h"
#import "dhcpd.h"
#import "ts_log.h"
#import "bootpd.h"
#import "bsdp.h"
#import "bootp_transmit.h"

#define CFGPROP_BOOTP_ENABLED		"bootp_enabled"
#define CFGPROP_DHCP_ENABLED		"dhcp_enabled"
#define CFGPROP_OLD_NETBOOT_ENABLED	"old_netboot_enabled"
#define CFGPROP_NETBOOT_ENABLED		"netboot_enabled"

/* external functions */
extern char *  			ether_ntoa(struct ether_addr *e);
extern struct ether_addr *	ether_aton(char *);

/* local defines */
#define	MAXIDLE			(5*60)	/* we hang around for five minutes */
#define DOMAIN_HIERARCHY	"..."	/* ... means open the hierarchy */

#define SERVICE_BOOTP		0x00000001
#define SERVICE_DHCP		0x00000002
#define SERVICE_OLD_NETBOOT	0x00000004
#define SERVICE_NETBOOT		0x00000008

/* global variables: */
char		boot_tftp_dir[128] = "/private/tftpboot";
int		bootp_socket = -1;
NICache_t	cache;
int		debug = 0;
NIDomain_t *	ni_local = NULL; /* local netinfo domain */
NIDomainList_t	niSearchDomains;
int		quiet = 0;
unsigned char	rfc_magic[4] = RFC_OPTIONS_MAGIC;
unsigned short	server_priority = BSDP_PRIORITY_BASE;
char *		testing_control = "";
char		server_name[MAXHOSTNAMELEN + 1];
id		subnets = nil;
char 		transmit_buffer[2048];
int		verbose = 0;

/* local types */

/* local variables */
static boolean_t		S_bootfile_noexist_reply = TRUE;
static unsigned long		S_cache_check_interval = 30; /* seconds */
static PropList_t		S_config_dhcp;
static boolean_t		S_do_bootp;
static boolean_t		S_do_netboot;
static boolean_t		S_do_dhcp;
static boolean_t		S_do_old_netboot;
static boolean_t		S_netinfo_host = FALSE;
static ptrlist_t		S_domain_list;
static struct in_addr *		S_dns_servers = NULL;
static int			S_dns_servers_count = 0;
static char *			S_domain_name = NULL;
static ptrlist_t		S_if_list;
static interface_list_t *	S_interfaces;
static inetroute_list_t *	S_inetroutes = NULL;
static u_short			S_ipport_client = IPPORT_BOOTPC;
static u_short			S_ipport_server = IPPORT_BOOTPS;
static struct timeval		S_lastmsgtime;
static u_char 			S_rxpkt[2048];/* receive packet buffer */
#ifndef SYSTEM_ARP
static int			S_rtsockfd = -1;
#endif SYSTEM_ARP
static boolean_t		S_sighup = TRUE; /* fake the 1st sighup */
static u_int32_t		S_which_services = 0;

/*
 * PropList_t routines
 */

int
PropList_instance(PropList_t * pl_p)
{
    return (pl_p->instance);
}

void
PropList_free(PropList_t * pl_p)
{
    if (pl_p == NULL)
	return;
    ni_name_free(&pl_p->path);
    ni_proplist_free(&pl_p->pl);
    bzero(pl_p, sizeof(*pl_p));
    return;
}

void 
PropList_init(PropList_t * pl_p, ni_name path)
{
    bzero(pl_p, sizeof(*pl_p));
    NI_INIT(&pl_p->pl);
    pl_p->path = ni_name_dup(path);
}

boolean_t
PropList_read(PropList_t * pl_p)
{
    ni_status	status;
    ni_id	dir_id = {0,0};
    
    status = ni_pathsearch(NIDomain_handle(ni_local), &dir_id, pl_p->path);
    if (status != NI_OK) {
	syslog(LOG_INFO, "ni_pathsearch '%s' failed: %s",
	       pl_p->path, ni_error(status));
	ni_proplist_free(&pl_p->pl);
	bzero(&pl_p->dir_id, sizeof(pl_p->dir_id));
	pl_p->instance++;
	return (FALSE);
    }
    if (bcmp(&dir_id, &pl_p->dir_id, sizeof(dir_id))) {
	/* directory was modified - re-read */
	ni_proplist_free(&pl_p->pl);
	bzero(&pl_p->dir_id, sizeof(pl_p->dir_id));
	pl_p->instance++;
	status = ni_read(NIDomain_handle(ni_local), &dir_id, &pl_p->pl);
	if (status != NI_OK) {
	    syslog(LOG_INFO, "ni_read '%s' failed: %s",
		   pl_p->path, ni_error(status));
	    return (FALSE);
	}
	pl_p->dir_id = dir_id;
    }
    return (TRUE);
}

ni_namelist *
PropList_lookup(PropList_t * pl_p, ni_name propname)
{
    int i;

    for (i = 0; i < pl_p->pl.nipl_len; i++) {
	ni_property * p = &(pl_p->pl.nipl_val[i]);
	if (strcmp(propname, p->nip_name) == 0) {
	    return (&p->nip_val);
	}
    }
    return (NULL);
}

/* forward function declarations */
static int 		issock(int fd);
static void		on_alarm(int sigraised);
static void		on_sighup(int sigraised);
static void		bootp_request(interface_t *, void * bp, int len,
				      struct timeval *);
static void		setarp(struct in_addr * ia, u_char * ha, int len);
static void		S_server_loop();
static void		S_relay_loop(struct in_addr * relay, int max_hops);


#define PID_FILE "/var/run/bootpd.pid"
static void
writepid(void)
{
    FILE *fp;
    
    fp = fopen(PID_FILE, "w");
    if (fp != NULL) {
	fprintf(fp, "%d\n", getpid());
	(void) fclose(fp);
    }
}

/*
 * Function: background
 *
 * Purpose:
 *   Daemon-ize ourselves.
 */
static void
background()
{
    if (fork())
	exit(0);
    { 
	int s;
	for (s = 0; s < 10; s++)
	    (void) close(s);
    }
    (void) open("/", O_RDONLY);
    (void) dup2(0, 1);
    (void) dup2(0, 2);
    {
	int tt = open("/dev/tty", O_RDWR);
	if (tt > 0) {
	    ioctl(tt, TIOCNOTTY, 0);
	    close(tt);
	}
    }
    writepid();
}

static __inline__ boolean_t
is_this_our_name(interface_list_t * list, char * name)
{
    struct hostent * h;

    h = gethostbyname(name);
    if (h && h->h_addr_list && *h->h_addr_list) {
	struct in_addr * * a = (struct in_addr * *)h->h_addr_list;
	while (*a) {
	    if (ifl_find_ip(list, **a)) {
		return (TRUE);
	    }
	    a++;
	}
    }
    return (FALSE);
}

#define NIPROP_MASTER		"master"

void
setMaster(NIDomain_t * domain, interface_list_t * list)
{
    ni_id		dir;
    ni_namelist		nl;
    u_char *		slash;
    ni_status		status;

    if (domain == NULL || list == NULL)
	return;

    status = ni_pathsearch(NIDomain_handle(domain), &dir, "/");
    if (status != NI_OK)
	return;
    
    NI_INIT(&nl);
    status = ni_lookupprop(NIDomain_handle(domain), &dir, NIPROP_MASTER, &nl);
    if (status != NI_OK)
	return;

    if (nl.ninl_len && (slash = strchr(nl.ninl_val[0], '/'))) {
	int		len;
	u_char		tmp[256];

	len = slash - (u_char *) nl.ninl_val[0];
	strncpy(tmp, nl.ninl_val[0], len);
	tmp[len] = '\0';
	NIDomain_set_master(domain, is_this_our_name(list, tmp));
    }
    ni_namelist_free(&nl);
    return;
}

/*
 * Function: S_ni_domains_init
 *
 * Purpose:
 *   Given the list of domain paths in S_domain_list,
 *   open a connection to it, and store the NIDomain object
 *   in the niSearchDomain list.
 *   The code makes sure it only opens each domain once by
 *   checking for uniqueness of the host/tag combination.
 *   The code also pays attention to the the special path "...",
 *   which means open the hierarchy starting from the local domain
 *   on up.
 */
static boolean_t
S_ni_domains_init()
{
    boolean_t	hierarchy_done = FALSE;
    int 	i;

    NICache_init(&cache, S_cache_check_interval);
    NIDomainList_init(&niSearchDomains);
    ni_local = NULL;

    for (i = 0; i < ptrlist_count(&S_domain_list); i++) {
	NIDomain_t * 	domain;
	u_char *   	dstr = (u_char *)ptrlist_element(&S_domain_list, i);

	if (strcmp(dstr, DOMAIN_HIERARCHY) == 0) {
	    NIDomain_t * domain;

	    if (hierarchy_done)
		continue;
	    hierarchy_done = TRUE;
	    if (verbose)
		syslog(LOG_INFO, 
		       "opening hierarchy starting at " NI_DOMAIN_LOCAL);
	    domain = NIDomain_init(NI_DOMAIN_LOCAL);
	    setMaster(domain, S_interfaces);
	    while (TRUE) {
		NIDomain_t * obj;

		if (domain == NULL)
		    break; /* we're done */
		obj = NIDomainList_find(&niSearchDomains, domain);
		if (obj != NULL) {
		    if (debug)
			printf("%s/%s already in the list: %s\n",
			       NIDomain_tag(obj), inet_ntoa(NIDomain_ip(obj)),
			       NIDomain_name(obj));
		    NIDomain_free(obj);
		    domain = obj;
		}
		else {
		    if (verbose)
			syslog(LOG_INFO, "opened domain %s/%s", 
			       inet_ntoa(NIDomain_ip(domain)),
			       NIDomain_tag(domain));
		    NIDomainList_add(&niSearchDomains, domain);
		    NICache_add_domain(&cache, domain);
		}
		domain = NIDomain_parent(domain);
		setMaster(domain, S_interfaces);
	    }
	}
	else {
	    if (verbose) {
		syslog(LOG_INFO, "opening domain %s", dstr);
	    }
	    domain = NIDomain_init(dstr);

	    if (domain != NULL) {
		if (NIDomainList_find(&niSearchDomains, domain)
		    != NULL) {
		    /* already in the list */
		    if (debug) {
			printf("%s/%s already in the list\n",
			       inet_ntoa(NIDomain_ip(domain)),
			       NIDomain_tag(domain)); 
		    }
		    NIDomain_free(domain);
		    continue;
		}
		setMaster(domain, S_interfaces);
		NIDomainList_add(&niSearchDomains, domain);
		NICache_add_domain(&cache, domain);
		if (verbose)
		    syslog(LOG_INFO, "opened domain %s/%s", 
			   inet_ntoa(NIDomain_ip(domain)),
			   NIDomain_tag(domain));
	    }
	    else {
		syslog(LOG_INFO, "unable to open domain '%s'", dstr);
	    }
	}
    }
    { /* find the "local" netinfo domain */
	int i;

	for (i = 0; i < NIDomainList_count(&niSearchDomains); i++) {
	    NIDomain_t * domain = NIDomainList_element(&niSearchDomains, i);
	    if (ifl_find_ip(S_interfaces, NIDomain_ip(domain))
		&& strcmp(NIDomain_tag(domain), "local") == 0) {
		ni_local = domain;
		break;
	    }
	}
	if (ni_local == NULL) {
	    ni_local = NIDomain_init(NI_DOMAIN_LOCAL);
	    setMaster(ni_local, S_interfaces);

	    if (ni_local == NULL)
		exit(1);
	    syslog(LOG_INFO, 
		   "opened local netinfo domain");
	}
    }
    
    return (TRUE);
}

static void
S_get_dns()
{
    int i;

    res_init(); /* figure out the default dns servers */

    if (S_dns_servers) {
	free(S_dns_servers);
    }
    S_dns_servers_count = _res.nscount;
    if (S_dns_servers_count == 1) {
      if (_res.nsaddr_list[0].sin_addr.s_addr == 0)
	S_dns_servers_count = 0;	/* if not set, DNS is 0.0.0.0 */
    }
    if (S_dns_servers_count) {
	S_dns_servers = (struct in_addr *)malloc(sizeof(*S_dns_servers) 
						 * S_dns_servers_count);
	if (_res.defdname[0]) {
	    S_domain_name = _res.defdname;
	    if (debug)
		printf("%s\n", S_domain_name);
	}
	for (i = 0; i < S_dns_servers_count; i++) {
	    S_dns_servers[i] = _res.nsaddr_list[i].sin_addr;
	    if (debug)
		printf("DNS %s\n", inet_ntoa(S_dns_servers[i]));
	}
    }
    return;
}

/*
 * Function: S_string_in_list
 *
 * Purpose:
 *   Given a List object, return boolean whether the C string is
 *   in the list.
 */
static __inline__ boolean_t
S_string_in_list(ptrlist_t * list, u_char * str)
{
    int i;

    for (i = 0; i < ptrlist_count(list); i++) {
	u_char * lstr = (u_char *)ptrlist_element(list, i);
	if (strcmp(str, lstr) == 0)
	    return (TRUE);
    }
    return (FALSE);
}

/*
 * Function: S_log_interfaces
 *
 * Purpose:
 *   Log which interfaces we will respond on.
 */
void
S_log_interfaces() 
{
    int i;
    int count = 0;
    
    for (i = 0; i < S_interfaces->count; i++) {
	interface_t * 	if_p = S_interfaces->list + i;
	
	if ((ptrlist_count(&S_if_list) == 0
	     || S_string_in_list(&S_if_list, if_name(if_p)))
	    && if_inet_valid(if_p) && !(if_flags(if_p) & IFF_LOOPBACK)) {
	    char 	ip[32];

	    strcpy(ip, inet_ntoa(if_inet_addr(if_p)));
	    syslog(LOG_INFO, "interface %s: ip %s mask %s", 
		   if_name(if_p), ip, inet_ntoa(if_inet_netmask(if_p)));
	    count++;
	}
    }
    if (count == 0) {
	syslog(LOG_INFO, "no available interfaces");
	exit(2);
    }
    return;
}

/*
 * Function: S_get_interfaces
 * 
 * Purpose:
 *   Get the list of interfaces we will use.
 */
void
S_get_interfaces()
{
    interface_list_t *	new_list;
    
    new_list = ifl_init();
    if (new_list == NULL) {
	syslog(LOG_INFO, "interface list initialization failed");
	exit(1);
    }
    ifl_free(&S_interfaces);
    S_interfaces = new_list;
    S_log_interfaces();
    return;
}

/*
 * Function: S_get_network_routes
 *
 * Purpose:
 *   Get the list of network routes.
 */
void
S_get_network_routes()
{
    inetroute_list_t * new_list;
    
    new_list = inetroute_list_init();
    if (new_list == NULL) {
	syslog(LOG_INFO, "can't get inetroutes list");
	exit(1);
    }
    
    inetroute_list_free(&S_inetroutes);
    S_inetroutes = new_list;
    if (debug)
	inetroute_list_print(S_inetroutes);
}

static void
S_service_enable(char * * list, int count, u_int32_t which)
{
    int i;

    for (i = 0; i < count; i++) {
	interface_t * 	if_p;
	char *		ifname = list[i];

	if (ifname == NULL || *ifname == '\0')
	    continue;
	if_p = ifl_find_name(S_interfaces, ifname);
	if (if_p == NULL)
	    continue;
	if_p->user_defined |= which;
    }
    return;
}

static void
S_service_disable(u_int32_t service)
{
    int i;

    S_which_services &= ~service;
    
    for (i = 0; i < S_interfaces->count; i++) {
	interface_t * 	if_p = S_interfaces->list + i;
	if_p->user_defined &= ~service;
    }
    return;
}

static boolean_t
S_service_is_enabled(u_int32_t service)
{
    int i;

    if (S_which_services & service)
	return (TRUE);

    for (i = 0; i < S_interfaces->count; i++) {
	interface_t * 	if_p = S_interfaces->list + i;
	if (if_p->user_defined & service)
	    return (TRUE);
    }
    return (FALSE);
}

static __inline__ void
S_disable_netboot()
{
    S_do_netboot = FALSE;
    S_do_old_netboot = FALSE;
    S_service_disable(SERVICE_NETBOOT | SERVICE_OLD_NETBOOT);
    return;
}

static void
S_update_services()
{
    ni_namelist *	nl_p;

    S_which_services = 0;
    PropList_read(&S_config_dhcp);

    /* BOOTP */
    nl_p = PropList_lookup(&S_config_dhcp, CFGPROP_BOOTP_ENABLED);
    if (nl_p == NULL || nl_p->ninl_len == 0) {
	S_which_services |= SERVICE_BOOTP;
    }
    else {
	S_service_enable(nl_p->ninl_val, nl_p->ninl_len, SERVICE_BOOTP);
    }

    /* DHCP */
    nl_p = PropList_lookup(&S_config_dhcp, CFGPROP_DHCP_ENABLED);
    if (nl_p) {
	if (nl_p->ninl_len == 0) {
	    S_which_services |= SERVICE_DHCP;
	}
	else {
	    S_service_enable(nl_p->ninl_val, nl_p->ninl_len, SERVICE_DHCP);
	}
    }

    /* NetBoot (2.0) */
    nl_p = PropList_lookup(&S_config_dhcp, CFGPROP_NETBOOT_ENABLED);
    if (nl_p) {
	if (nl_p->ninl_len == 0) {
	    S_which_services |= SERVICE_NETBOOT;
	}
	else {
	    S_service_enable(nl_p->ninl_val, nl_p->ninl_len, SERVICE_NETBOOT);
	}
    }

    /* NetBoot (old, pre 2.0) */
    nl_p = PropList_lookup(&S_config_dhcp, CFGPROP_OLD_NETBOOT_ENABLED);
    if (nl_p) {
	if (nl_p->ninl_len == 0) {
	    S_which_services |= SERVICE_OLD_NETBOOT;
	}
	else {
	    S_service_enable(nl_p->ninl_val, nl_p->ninl_len, 
			     SERVICE_OLD_NETBOOT);
	}
    }
    return;
}

static __inline__ boolean_t
bootp_enabled(interface_t * if_p)
{
    u_int32_t 	which = (S_which_services | if_p->user_defined);

    return (S_do_bootp || (which & SERVICE_BOOTP) != 0);
}

static __inline__ boolean_t
dhcp_enabled(interface_t * if_p)
{
    u_int32_t 	which = (S_which_services | if_p->user_defined);

    return (S_do_dhcp || (which & SERVICE_DHCP) != 0);
}

static __inline__ boolean_t
netboot_enabled(interface_t * if_p)
{
    u_int32_t 	which = (S_which_services | if_p->user_defined);

    return (S_do_netboot || (which & SERVICE_NETBOOT) != 0);
}

static __inline__ boolean_t
old_netboot_enabled(interface_t * if_p)
{
    u_int32_t 	which = (S_which_services | if_p->user_defined);

    return (S_do_old_netboot || (which & SERVICE_OLD_NETBOOT) != 0);
}

void
usage()
{
    fprintf(stderr, "useage: bootpd <options>\n"
	    "<options> are:\n"
	    "[ -a ] 	support anonymous binding for BOOTP clients\n"
	    "[ -D ]	be a DHCP server\n"
	    "[ -b ] 	bootfile must exist or we don't respond\n"
	    "[ -c <cache check interval in seconds> ]\n"
	    "[ -d ]	debug mode, stay in foreground, extra printf's\n"
	    "[ -i <interface> [ -i <interface> ... ] ]\n"
	    "[ -n <domain> [ -n <domain> [...] ] ]\n"
	    "[ -N ]	be a NetBoot server\n"
	    "[ -q ]	be quiet as possible\n"
	    "[ -r <BOOTP/DHCP server ip> [ -o <max hops> ] ]\n"
	    "[ -v ] 	verbose mode, extra information\n"
	    );
    exit(1);
}

int
main(int argc, char * argv[])
{
    int			ch;
    int			logopt = LOG_CONS;
    int			max_hops = 4;
    boolean_t		netinfo_lookups = TRUE;
    boolean_t		relay = FALSE;
    struct in_addr	relay_server = { 0 };

    debug = 0;			/* no debugging ie. go into the background */
    verbose = 0;		/* don't print extra information */

    ptrlist_init(&S_domain_list);
    ptrlist_init(&S_if_list);
    while ((ch =  getopt(argc, argv, "aBbc:DdhHi:mNn:o:p:qr:t:v")) != EOF) {
	switch ((char)ch) {
	case 'a':
	    /* enable anonymous binding for BOOTP clients */
	    S_netinfo_host = TRUE;
	    break;
	case 'B':
	    S_do_bootp = FALSE;
	    break;
	case 'b':
	    S_bootfile_noexist_reply = FALSE; 
	    /* reply only if bootfile exists */
	    break;
	case 'c':		    /* cache check interval - seconds */
	    S_cache_check_interval = strtoul(optarg, NULL, NULL);
	    printf("Using cache check interval %ld seconds\n",
		   S_cache_check_interval);
	    break;
	case 'D':		/* answer DHCP requests as a DHCP server */
	    S_do_dhcp = TRUE;
	    S_do_bootp = TRUE;
	    break;
	case 'd':		/* stay in the foreground, extra printf's */
	    debug = 1;
	    break;
	case 'h':
	case 'H':
	    usage();
	    exit(1);
	    break;
	case 'i':	/* user specified interface(s) to use */
	    if (S_string_in_list(&S_if_list, optarg) == FALSE) {
		ptrlist_add(&S_if_list, optarg);
	    }
	    else {
		syslog(LOG_INFO, "interface %s already specified",
		       optarg);
	    }
	    break;
	case 'm':
	    S_do_old_netboot = TRUE;
	    S_do_dhcp = TRUE;
	    break;
	case 'n':	/* specify netinfo domain search hierarchy */
	    if (optarg && *optarg == '\0') {
		netinfo_lookups = FALSE;
	    }
	    else {
		if (S_string_in_list(&S_domain_list, optarg) == FALSE) {
		    ptrlist_add(&S_domain_list, optarg);
		}
	    }
	    break;
	case 'N':
	    S_do_netboot = TRUE;
	    break;
	case 'o': {
	    int h;
	    h = atoi(optarg);
	    if (h > 16 || h < 1) {
		printf("max hops value %s must be in the range 1..16\n",
		       optarg);
		exit(1);
	    }
	    max_hops = h;
	    break;
	}
	case 'p': {
	    server_priority = strtoul(optarg, NULL, NULL);
	    printf("Priority set to %d\n", server_priority);
	    break;
	}
	case 'q':
	    quiet = 1;
	    break;
	case 'r':
	    relay = TRUE;
	    if (inet_aton(optarg, &relay_server) == 0
		|| relay_server.s_addr == 0 
		|| relay_server.s_addr == INADDR_BROADCAST) {
		printf("Invalid relay server ip address %s\n", optarg);
		exit(1);
	    }
	    break;
	case 't': {
	    testing_control = optarg;
	    break;
	}
	case 'v':	/* extra info to syslog */
	    verbose++;
	    break;
	default:
	    break;
	}
    }
    if (debug)
	quiet = 0;
    else if (quiet)
	verbose = 0;
    if (!issock(0)) { /* started by user */
	struct sockaddr_in Sin = { sizeof(Sin), AF_INET };
	int i;
	
	if (!debug)
	    background();
	
	if ((bootp_socket = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
	    syslog(LOG_INFO, "socket call failed");
	    exit(1);
	}
	Sin.sin_port = htons(S_ipport_server);
	Sin.sin_addr.s_addr = htonl(INADDR_ANY);
	i = 0;
	while (bind(bootp_socket, (struct sockaddr *)&Sin, sizeof(Sin)) < 0) {
	    syslog(LOG_INFO, "bind call failed: %s", strerror(errno));
	    if (errno != EADDRINUSE)
		exit(1);
	    i++;
	    if (i == 10) {
		syslog(LOG_INFO, "exiting");
		exit(1);
	    }
	    sleep(10);
	}
    } 
    else { /* started by inetd */
	bootp_socket = 0;
	signal(SIGALRM, on_alarm);
	gettimeofday(&S_lastmsgtime, 0);
	alarm(15);
    }

    if (debug)
	logopt = LOG_PERROR;

    if (relay)
	(void) openlog("bootp_relay", logopt | LOG_PID, LOG_DAEMON);
    else
	(void) openlog("bootpd", logopt | LOG_PID, LOG_DAEMON);
	
    syslog(LOG_DEBUG, "server starting");

    { 
	int opt = 1;

#if defined(IP_RECVIF)
	if (setsockopt(bootp_socket, IPPROTO_IP, IP_RECVIF, (caddr_t)&opt,
		       sizeof(opt)) < 0) {
	    syslog(LOG_INFO, "setsockopt(IP_RECVIF) failed: %s", 
		   strerror(errno));
	    exit(1);
	}
#endif
	
	if (setsockopt(bootp_socket, SOL_SOCKET, SO_BROADCAST, (caddr_t)&opt,
		       sizeof(opt)) < 0) {
	    syslog(LOG_INFO, "setsockopt(SO_BROADCAST) failed");
	    exit(1);
	}
	if (setsockopt(bootp_socket, IPPROTO_IP, IP_RECVDSTADDR, (caddr_t)&opt,
		       sizeof(opt)) < 0) {
	    syslog(LOG_INFO, "setsockopt(IPPROTO_IP, IP_RECVDSTADDR) failed");
	    exit(1);
	}
	if (setsockopt(bootp_socket, SOL_SOCKET, SO_REUSEADDR, (caddr_t)&opt,
		       sizeof(opt)) < 0) {
	    syslog(LOG_INFO, "setsockopt(SO_REUSEADDR) failed");
	    exit(1);
	}
    }
    
    ts_log_init(verbose);

    /* install our sighup handler */
    signal(SIGHUP, on_sighup);

    S_get_interfaces();
    S_get_network_routes();

    if (relay == FALSE) {
	PropList_init(&S_config_dhcp, "/config/dhcp");

	/* initialize our netinfo search domains */
	if (ptrlist_count(&S_domain_list) == 0 && netinfo_lookups) {
	    ptrlist_add(&S_domain_list, DOMAIN_HIERARCHY);
	}
	if (S_ni_domains_init() == FALSE) {
	    syslog(LOG_INFO, "domain initialization failed");
	    exit (1);
	}
	S_update_services();
    }

#ifndef SYSTEM_ARP
    S_rtsockfd = arp_get_routing_socket();
    if (S_rtsockfd < 0) {
	syslog(LOG_INFO, "couldn't get routing socket: %s",
	       strerror(errno));
	exit(1);
    }
#endif SYSTEM_ARP
	
    if (relay) {
	S_relay_loop(&relay_server, max_hops);
    }
    else {
	S_server_loop();
    }
    exit (0);
}

/*
 * Function: subnetAddressAndMask
 *
 * Purpose:
 *   Given the gateway address field from the request and the 
 *   interface the packet was received on, determine the subnet
 *   address and mask.
 * Note:
 *   This currently does not support "super-netting", in which
 *   more than one proper subnet shares the same physical subnet.
 */
boolean_t
subnetAddressAndMask(struct in_addr giaddr, interface_t * if_p,
		     struct in_addr * addr, struct in_addr * mask)
{
    /* gateway specified, find a subnet description on the same subnet */
    if (giaddr.s_addr) {
	id subnet;
	/* find a subnet entry on the same subnet as the gateway */
	if (subnets == nil 
	    || (subnet = [subnets entrySameSubnet:giaddr]) == nil)
	    return (FALSE);
	*addr = giaddr;
	*mask = [subnet mask];
    }
    else {
	*addr = if_inet_netaddr(if_p);
	*mask = if_inet_netmask(if_p);
    }
    return (TRUE);
}

/*
 * Function: issock
 *
 * Purpose:
 *   Determine if a descriptor belongs to a socket or not
 */
static int
issock(fd)
     int fd;
{
    struct stat st;
    
    if (fstat(fd, &st) < 0) {
	return (0);
    } 
    /*	
     * SunOS returns S_IFIFO for sockets, while 4.3 returns 0 and
     * does not even have an S_IFIFO mode.  Since there is confusion 
     * about what the mode is, we check for what it is not instead of 
     * what it is.
     */
    switch (st.st_mode & S_IFMT) {
      case S_IFCHR:
      case S_IFREG:
      case S_IFLNK:
      case S_IFDIR:
      case S_IFBLK:
	return (0);
      default:	
	return (1);
    }
}


/*
 * Function: on_sighup
 *
 * Purpose:
 *   If we get a sighup, re-read the subnet descriptions.
 */
static void
on_sighup(int sigraised)
{
    if (sigraised == SIGHUP)
	S_sighup = TRUE;
    return;
}

/*
 * Function: on_alarm
 *
 * Purpose:
 *   If we were started by inetd, we kill ourselves during periods of
 *   inactivity.  If we've been idle for MAXIDLE, exit.
 */
static void
on_alarm(int sigraised)
{
    struct timeval tv;
    
    gettimeofday(&tv, 0);
    
    if ((tv.tv_sec - S_lastmsgtime.tv_sec) >= MAXIDLE)
	exit(0);
    alarm(15);
    return;
}

/*
 * Function: bootp_add_bootfile
 *
 * Purpose:
 *   Verify that the specified bootfile exists, and add it to the given
 *   packet.  Handle <bootfile>.<hostname> to allow a specific host to
 *   get its own version of the bootfile.
 */
boolean_t
bootp_add_bootfile(char * request_file, char * hostname,
		   char * bootfile, char * reply_file)
{
    boolean_t 	dothost = FALSE;	/* file.host was found */
    char 	file[PATH_MAX];
    char 	path[PATH_MAX];

    if (request_file && request_file[0])
	strcpy(file, request_file);
    else if (bootfile && bootfile[0])
	strcpy(file, bootfile);
    else {
	if (verbose)
	    syslog(LOG_INFO, "no replyfile", path);
	return (TRUE);
    }

    if (file[0] == '/')	/* if absolute pathname */
	strcpy(path, file);
    else {
	strcpy(path, boot_tftp_dir);
	strcat(path, "/");
	strcat(path, file);
    }

    /* see if file exists with a ".host" suffix */
    if (hostname) {
	int 	n;

	n = strlen(path);
	strcat(path, ".");
	strcat(path, hostname);
	if (access(path, R_OK) >= 0)
	    dothost = TRUE;
	else
	    path[n] = 0;	/* try it without the suffix */
    }
    
    if (dothost == FALSE) {
	if (access(path, R_OK) < 0) {
	    if (S_bootfile_noexist_reply == FALSE) {
		syslog(LOG_INFO, 
		       "boot file %s* missing - not replying", path);
		return (FALSE);
	    }
	    if (verbose)
		syslog(LOG_INFO, "boot file %s* missing", path);
	}
    }

    if (verbose)
	syslog(LOG_INFO,"replyfile %s", path);
    strcpy(reply_file, path);
    return (TRUE);
}

void
host_parms_from_proplist(ni_proplist * pl_p, int index, struct in_addr * ip, 
			 u_char * * name, u_char * * bootfile)
{
    ni_name ipstr;

    if (index != -1) {
	/* retrieve the ip address */
	if (ip) { /* return the ip address */
	    ipstr = (ni_nlforprop(pl_p, NIPROP_IP_ADDRESS))->ninl_val[index];
	    ip->s_addr = inet_addr(ipstr);
	}
    }

    /* retrieve the host name */
    if (name) {
	ni_name 	str;
	
	*name = NULL;
	str = ni_valforprop(pl_p, NIPROP_NAME);
	if (str)
	    *name = ni_name_dup(str);
    }
    
    /* retrieve the bootfile */
    if (bootfile) {
	ni_name str;
	
	*bootfile = NULL;
	str = ni_valforprop(pl_p, NIPROP_BOOTFILE);
	if (str)
	    *bootfile = ni_name_dup(str);
    }
    return;
}

/*
 * Function: ip_address_reachable
 *
 * Purpose:
 *   Determine whether the given ip address is directly reachable from
 *   the given interface or gateway.
 *
 *   Directly reachable means without using a router ie. share the same wire.
 */
boolean_t
ip_address_reachable(struct in_addr ip, struct in_addr giaddr, 
		    interface_t * if_p)
{
    int i;

    if (giaddr.s_addr) { /* gateway'd */
	/* find a subnet entry on the same subnet as the gateway */
	if (subnets == nil 
	    || [subnets ip:ip SameSupernet:giaddr] == FALSE)
	    return (FALSE);
	return (TRUE);
    }

    for (i = 0; i < S_inetroutes->count; i++) {
	inetroute_t * inr_p = S_inetroutes->list + i;

	if (inr_p->gateway.link.sdl_family == AF_LINK
	    && (ifl_find_link(S_interfaces, inr_p->gateway.link.sdl_index) 
		== if_p)) {
	    /* reachable? */
	    if (in_subnet(inr_p->dest, inr_p->mask, ip))
		return (TRUE);
	}
    }
    return (FALSE);
}

boolean_t 
subnet_match(void * arg, struct in_addr iaddr)
{
    subnet_match_args_t *	s = (subnet_match_args_t *)arg;

    /* the binding may be invalid for this subnet, but it has one */
    s->has_binding = TRUE;
    if (iaddr.s_addr == s->ciaddr.s_addr
	|| ip_address_reachable(iaddr, s->giaddr, s->if_p)) {
	return (TRUE);
    }
    return (FALSE);
}

/*
 * Function: bootp_request
 *
 * Purpose:
 *   Process BOOTREQUEST packet.
 *
 */
static void
bootp_request(interface_t * if_p, void * rxpkt, int rxpkt_len,
	      struct timeval * time_in_p)
{
    u_char *		bootfile = NULL;
    NIDomain_t *	domain = NULL;
    u_char *		hostname = NULL;
    struct in_addr	iaddr;
    boolean_t		netinfo_host = FALSE;
    static u_char	netinfo_options[] = {
	dhcptag_subnet_mask_e, 
	dhcptag_router_e, 
	dhcptag_host_name_e,
	dhcptag_netinfo_server_address_e,
	dhcptag_netinfo_server_tag_e,
	dhcptag_domain_name_server_e,
	dhcptag_domain_name_e,
    };
    static int		n_netinfo_options 
	= sizeof(netinfo_options) / sizeof(netinfo_options[0]);
    struct bootp 	rp;
    struct bootp *	rq = (struct bootp *)rxpkt;

    if (rxpkt_len < sizeof(struct bootp))
	return;

    rp = *rq;	/* copy request into reply */
    rp.bp_op = BOOTREPLY;

    if (rq->bp_ciaddr.s_addr == 0) { /* client doesn't specify ip */
	subnet_match_args_t	match;
	PLCacheEntry_t * 	entry;

	bzero(&match, sizeof(match));
	match.if_p = if_p;
	match.giaddr = rq->bp_giaddr;
	entry = NICache_lookup_hw(&cache, time_in_p, 
				  rq->bp_htype, rq->bp_chaddr, rq->bp_hlen,
				  subnet_match, &match, &domain, &iaddr);
	if (entry == NULL) {
	    return;
	}
	host_parms_from_proplist(&entry->pl, 0, NULL, &hostname, &bootfile);

	rp.bp_yiaddr = iaddr;
	if (S_netinfo_host) {
	    if (ni_valforprop(&entry->pl, NIPROP_SERVES))
		netinfo_host = TRUE;
	}
    }
    else { /* client specified ip address */
	PLCacheEntry_t * entry;
	
	iaddr = rq->bp_ciaddr;

	entry = NICache_lookup_ip(&cache, time_in_p, iaddr, &domain);
	if (entry == NULL)
	    return;
	host_parms_from_proplist(&entry->pl, 0, NULL, &hostname, &bootfile);
    }
    if (!quiet)
	syslog(LOG_INFO,"BOOTP request [%s]: %s requested file '%s'",
	       if_name(if_p), hostname ? hostname : (u_char *)inet_ntoa(iaddr),
	       rq->bp_file);
    if (bootp_add_bootfile(rq->bp_file, hostname, bootfile,
			   rp.bp_file) == FALSE)
	/* client specified a bootfile but it did not exist */
	goto no_reply;
    
    if (bcmp(rq->bp_vend, rfc_magic, sizeof(rfc_magic)) == 0) {
	/* insert the usual set of options/extensions if possible */
	dhcpoa_t	options;

	dhcpoa_init(&options, rp.bp_vend + sizeof(rfc_magic),
		    sizeof(rp.bp_vend) - sizeof(rfc_magic));

	if (netinfo_host) {
	    if (verbose)
		syslog(LOG_INFO, "netinfo client");
	    add_subnet_options(domain, hostname, iaddr, if_p, &options, 
			       netinfo_options, n_netinfo_options);
	}
	else {
	    add_subnet_options(domain, hostname, iaddr, if_p, &options, 
			       NULL, 0);
	}

	if (verbose) 
	    syslog(LOG_INFO, "added vendor extensions");
	if (dhcpoa_add(&options, dhcptag_end_e, 0, NULL)
	    != dhcpoa_success_e) {
	    syslog(LOG_INFO, "couldn't add end tag");
	}
	else
	    bcopy(rfc_magic, rp.bp_vend, sizeof(rfc_magic));
    } /* if RFC magic number */

    rp.bp_siaddr = if_inet_addr(if_p);
    strcpy(rp.bp_sname, server_name);
    if (sendreply(if_p, &rp, sizeof(rp), FALSE, NULL)) {
	if (!quiet)
	    syslog(LOG_INFO, "reply sent %s %s pktsize %d",
		   hostname, inet_ntoa(iaddr), sizeof(rp));
    }

  no_reply:
    if (hostname)
	free(hostname);
    if (bootfile)
	free(bootfile);
    return;
}


/*
 * Function: sendreply
 *
 * Purpose:
 *   Send a reply packet to the client.
 */
boolean_t
sendreply(interface_t * if_p, struct bootp * bp, int n, 
	  boolean_t broadcast, struct in_addr * dest_p)
{
    struct in_addr 		dst;
    u_short			dest_port = S_ipport_client;
    void *			hwaddr = NULL;
    u_short			src_port = S_ipport_server;

    /*
     * If the client IP address is specified, use that
     * else if gateway IP address is specified, use that
     * else make a temporary arp cache entry for the client's NEW 
     * IP/hardware address and use that.
     */
    if (bp->bp_ciaddr.s_addr) {
	dst = bp->bp_ciaddr;
	if (verbose) 
	    syslog(LOG_DEBUG, "reply ciaddr %s", inet_ntoa(dst));
    }
    else if (bp->bp_giaddr.s_addr) {
	dst = bp->bp_giaddr;
	dest_port = S_ipport_server;
	src_port = S_ipport_client;
	if (verbose) 
	    syslog(LOG_INFO, "reply giaddr %s", inet_ntoa(dst));
	if (broadcast) /* tell the gateway to broadcast */
	    bp->bp_unused = htons(ntohs(bp->bp_unused | DHCP_FLAGS_BROADCAST));
    } 
    else { /* local net request */
	if (broadcast || (ntohs(bp->bp_unused) & DHCP_FLAGS_BROADCAST)) {
	    if (verbose)
		syslog(LOG_INFO, "replying using broadcast IP address");
	    dst.s_addr = htonl(INADDR_BROADCAST);
	}
	else {
	    if (dest_p)
		dst = *dest_p;
	    else
		dst = bp->bp_yiaddr;
	    if (dst.s_addr)
		setarp(&dst, bp->bp_chaddr, bp->bp_hlen);
	    hwaddr = bp->bp_chaddr;
	}
	if (verbose) 
	    syslog(LOG_INFO, "replying to %s", inet_ntoa(dst));
    }
    if (bootp_transmit(bootp_socket, transmit_buffer, if_name(if_p),
		       bp->bp_htype,
		       hwaddr,
		       bp->bp_hlen,
		       dst, if_inet_addr(if_p),
		       dest_port, src_port,
		       bp, n) < 0) {
	syslog(LOG_INFO, "transmit failed, %m");
	return (FALSE);
    }
    if (debug && verbose) {
	printf("\n=================== Server Reply ===="
	       "=================\n");
	dhcp_print_packet((struct dhcp *)bp, n);
    }
    return (TRUE);
}

/*
 * Function: get_dhcp_option
 *
 * Purpose:
 *   Get a dhcp option from subnet description.
 */
boolean_t
get_dhcp_option(id subnet, int tag, void * buf, int * len_p)
{
    unsigned char	err[256];
    unsigned char	propname[128];
    ni_namelist	*	nl_p;
    unsigned char *	tag_name;

    tag_name = dhcptag_name(tag);
    if (tag_name == NULL)
	return (FALSE);

    if (dhcptag_subnet_mask_e == tag)
	strcpy(propname, NIPROP_NET_MASK);
    else
	sprintf(propname, "%s%s", NI_DHCP_OPTION_PREFIX, tag_name);

    nl_p = [subnet lookup:propname];
    if (nl_p == NULL) {
	if (verbose)
	    syslog(LOG_INFO, "subnet entry %s is missing option %s",
		   [subnet name:err], propname);
	return (FALSE);
    }

    if (dhcptag_from_strlist((unsigned char * *)nl_p->ninl_val,
			     nl_p->ninl_len, tag, buf, len_p, err) == FALSE) {
	if (verbose)
	    syslog(LOG_INFO, "couldn't add option '%s': %s",
		   propname, err);
	return (FALSE);
    }
    return (TRUE);
}

/*
 * Function: add_subnet_options
 *
 * Purpose:
 *   Given a list of tags, retrieve them from the subnet entry and
 *   insert them into the message options.
 */
int
add_subnet_options(NIDomain_t * domain, u_char * hostname, 
		   struct in_addr iaddr, interface_t * if_p,
		   dhcpoa_t * options, u_char * tags, int n)
{
    inet_addrinfo_t *	info = if_inet_addr_at(if_p, 0);
    char		buf[DHCP_OPTION_SIZE_MAX];
    int			len;
    static u_char 	default_tags[] = { 
	dhcptag_subnet_mask_e, 
	dhcptag_router_e, 
	dhcptag_domain_name_server_e,
	dhcptag_domain_name_e,
	dhcptag_host_name_e,
    };
    boolean_t		netinfo_done = FALSE;
    static int		n_default_tags 
	= sizeof(default_tags) / sizeof(default_tags[0]);
    int			number_before = dhcpoa_count(options);
    int			i;
    id			subnet = [subnets entry:iaddr];

    if (tags == NULL) {
	tags = default_tags;
	n = n_default_tags;
    }
			
    for (i = 0; i < n; i++ ) {
	len = sizeof(buf);
	switch (tags[i]) {
	  case dhcptag_end_e:
	  case dhcptag_pad_e:
	  case dhcptag_requested_ip_address_e:
	  case dhcptag_lease_time_e:
	  case dhcptag_option_overload_e:
	  case dhcptag_dhcp_message_type_e:
	  case dhcptag_server_identifier_e:
	  case dhcptag_parameter_request_list_e:
	  case dhcptag_message_e:
	  case dhcptag_max_dhcp_message_size_e:
	  case dhcptag_renewal_t1_time_value_e:
	  case dhcptag_rebinding_t2_time_value_e:
	  case dhcptag_vendor_class_identifier_e:
	  case dhcptag_client_identifier_e:
	      continue; /* ignore these */
	  default:
	      break;
	}
	if (tags[i] == dhcptag_host_name_e) {
	    if (hostname) {
		if (dhcpoa_add(options, dhcptag_host_name_e,
			       strlen(hostname), hostname)
		    != dhcpoa_success_e) {
		    syslog(LOG_INFO, "couldn't add hostname: %s",
			   dhcpoa_err(options));
		}
	    }
	}
	else if (subnet != nil 
	    && get_dhcp_option(subnet, tags[i], buf, &len)) {
	    if (dhcpoa_add(options, tags[i], len, buf) 
		!= dhcpoa_success_e) {
		if (!quiet)
		    syslog(LOG_INFO, "couldn't add option %d: %s",
			   tags[i], dhcpoa_err(options));
	    }
	}
	else { /* try to use defaults if no explicit configuration */
	    struct in_addr * def_route;

	    switch (tags[i]) {
	      case dhcptag_netinfo_server_tag_e:
	      case dhcptag_netinfo_server_address_e: {
		  struct sockaddr_in 	ip;
		  ni_status		status;
		  ni_name 		tag;

		  if (netinfo_done)
		      continue;

		  netinfo_done = TRUE;
		  if (domain == NULL)
		      continue;
		  status = ni_addrtag(NIDomain_handle(domain), &ip, &tag);
		  if (status != NI_OK)
		      continue;
#define LOCAL_NETINFO_TAG	"local"
		  if (strcmp(tag, LOCAL_NETINFO_TAG) == 0) {
		      goto netinfo_failed;
		      /* can't bind to a server's local domain */
		  }

		  if (dhcpoa_add(options, dhcptag_netinfo_server_address_e,
				 sizeof(ip.sin_addr), &ip.sin_addr) 
		      != dhcpoa_success_e) {
		      if (!quiet)
			  syslog(LOG_INFO, 
				 "couldn't add netinfo server address: %s",
				 dhcpoa_err(options));
		      goto netinfo_failed;
		  }
		  if (dhcpoa_add(options, dhcptag_netinfo_server_tag_e,
				 strlen(tag), tag) != dhcpoa_success_e) {
		      if (!quiet)
			  syslog(LOG_INFO, 
				 "couldn't add netinfo server tag: %s",
				 dhcpoa_err(options));
		      goto netinfo_failed;
		  }
	      netinfo_failed:
		  ni_name_free(&tag);
		  break;
	      }
	      case dhcptag_subnet_mask_e: {
		if (ifl_find_subnet(S_interfaces, iaddr) != if_p)
		    continue;
		if (dhcpoa_add(options, dhcptag_subnet_mask_e, 
			       sizeof(info->mask), &info->mask) 
		    != dhcpoa_success_e) {
		    if (!quiet)
			syslog(LOG_INFO, "couldn't add subnet_mask: %s",
			       dhcpoa_err(options));
		    continue;
		}
		if (verbose)
		    syslog(LOG_INFO, "subnet mask %s derived from %s",
			   inet_ntoa(info->mask), if_name(if_p));
		break;
	      }
	      case dhcptag_router_e:
		def_route = inetroute_default(S_inetroutes);
		if (def_route == NULL
		    || in_subnet(info->netaddr, info->mask,
				 *def_route) == FALSE
		    || in_subnet(info->netaddr, info->mask,
				 iaddr) == FALSE)
		    /* don't respond if default route not on same subnet */
		    continue;
		if (dhcpoa_add(options, dhcptag_router_e, sizeof(*def_route),
			       def_route) != dhcpoa_success_e) {
		    if (!quiet)
			syslog(LOG_INFO, "couldn't add router: %s",
			       dhcpoa_err(options));
		    continue;
		}
		if (verbose)
		    syslog(LOG_INFO, "default route added as router");
		break;
	      case dhcptag_domain_name_server_e:
		if (S_dns_servers_count == 0)
		    continue;
		if (dhcpoa_add(options, dhcptag_domain_name_server_e,
			       S_dns_servers_count * sizeof(*S_dns_servers),
			       S_dns_servers) != dhcpoa_success_e) {
		    if (!quiet)
			syslog(LOG_INFO, "couldn't add dns servers: %s",
			       dhcpoa_err(options));
		    continue;
		}
		if (verbose)
		    syslog(LOG_INFO, "default dns servers added");
		break;
	      case dhcptag_domain_name_e:
		if (S_domain_name) {
		    if (dhcpoa_add(options, dhcptag_domain_name_e,
				   strlen(S_domain_name), S_domain_name)
			!= dhcpoa_success_e) {
			if (!quiet)
			    syslog(LOG_INFO, "couldn't add domain name: %s",
				   dhcpoa_err(options));
			continue;
		    }
		    if (verbose)
			syslog(LOG_INFO, "default domain name added");
		}
		break;
	      default:
		break;
	    }
	}
    }
    return (dhcpoa_count(options) - number_before);
}


#ifndef SYSTEM_ARP
/*
 * Function: setarp
 *
 * Purpose:
 *   Temporarily bind IP address 'ia'  to hardware address 'ha' of 
 *   length 'len'.  Uses the arp_set/arp_delete routines.
 */
void
setarp(struct in_addr * ia, u_char * ha, int len)
{
    int arp_ret;
    route_msg msg;

    if (S_rtsockfd == -1) {
	syslog(LOG_ERR, "setarp: routing socket not initialized");
	exit(1);
    }
    arp_ret = arp_get(S_rtsockfd, &msg, ia);
    if (arp_ret == 0) {
   	struct sockaddr_inarp *	sin;
    	struct sockaddr_dl *	sdl;

	sin = (struct sockaddr_inarp *)msg.m_space;
	sdl = (struct sockaddr_dl *)(sin->sin_len + (char *)sin);
	if (sdl->sdl_alen == len
	    && bcmp(ha, sdl->sdl_data + sdl->sdl_nlen, len) == 0) {
	    if (msg.m_rtm.rtm_rmx.rmx_expire == 0) {
		if (debug)
		    printf("permanent ARP entry for %s - leaving it alone\n",
			   inet_ntoa(*ia));
		return;
	    }
	}
    }
    else if (verbose)
	syslog(LOG_INFO, "arp_get(%s) failed, %d", inet_ntoa(*ia), arp_ret);
    arp_ret = arp_delete(S_rtsockfd, ia, FALSE);
    if (arp_ret != 0 && verbose)
	syslog(LOG_INFO, "arp_delete(%s) failed, %d", inet_ntoa(*ia), arp_ret);
    arp_ret = arp_set(S_rtsockfd, ia, (void *)ha, len, TRUE, FALSE);
    if (verbose) {
	if (arp_ret == 0)
	    syslog(LOG_INFO, "arp_set(%s, %s) succeeded", inet_ntoa(*ia), 
		   ether_ntoa((struct ether_addr *)ha));
	else
	    syslog(LOG_INFO, "arp_set(%s, %s) failed: %s", inet_ntoa(*ia), 
		   ether_ntoa((struct ether_addr *)ha),
		   arp_strerror(arp_ret));
    }
    return;
}
#else SYSTEM_ARP
/* 
 * SYSTEM_ARP: use system("arp") to set the arp entry
 */
/*
 * Setup the arp cache so that IP address 'ia' will be temporarily
 * bound to hardware address 'ha' of length 'len'.
 */
static void
setarp(struct in_addr * ia, u_char * ha, int len)
{
    char buf[256];
    int status;
    
    sprintf(buf, "/usr/sbin/arp -d %s", inet_ntoa(*ia));
    if (verbose) 
	syslog(LOG_INFO, buf);
    status = system(buf);
    if (status && verbose)
	syslog(LOG_INFO, "arp -d failed, exit code=0x%x", status);
    sprintf(buf, "/usr/sbin/arp -s %s %s temp",
	    inet_ntoa(*ia), ether_ntoa((struct ether_addr *)ha));;
    if (verbose) syslog(LOG_INFO, buf);
    status = system(buf);
    if (status && verbose)
	syslog(LOG_INFO, "arp failed, exit code=0x%x", status);
    return;
}
#endif SYSTEM_ARP

/**
 ** Server Main Loop
 **/
static char 		control[1024];
static struct iovec  	iov;
static struct msghdr 	msg;

static void
S_init_msg()
{
    msg.msg_name = 0;
    msg.msg_namelen = 0;
    msg.msg_iov = &iov;
    msg.msg_iovlen = 1;
    msg.msg_control = control;
    msg.msg_controllen = sizeof(control);
    msg.msg_flags = 0;
    iov.iov_base = (caddr_t)S_rxpkt;
    iov.iov_len = sizeof(S_rxpkt);
    return;
}

static void
S_relay_packet(struct in_addr * relay, int max_hops, struct bootp * bp, int n, 
	       interface_t * if_p)
{
    if (n < sizeof(struct bootp))
	return;

    switch (bp->bp_op) {
    case BOOTREQUEST:
	if (bp->bp_hops >= max_hops)
	    return;
	if (bp->bp_giaddr.s_addr == 0) {
	    /* fill it in with our interface address */
	    bp->bp_giaddr = if_inet_addr(if_p);
	}
	bp->bp_hops++;
	if (relay->s_addr == if_inet_broadcast(if_p).s_addr)
	    return; /* don't rebroadcast */
	if (bootp_transmit(bootp_socket, transmit_buffer, if_name(if_p),
			   bp->bp_htype, NULL, 0, 
			   *relay, if_inet_addr(if_p),
			   S_ipport_server, S_ipport_client,
			   bp, n) < 0) {
	    syslog(LOG_INFO, "send failed, %m");
	    return;
	}
	break;
    case BOOTREPLY: {
	interface_t * 	if_p;
	struct in_addr	dst;

	if (bp->bp_giaddr.s_addr == 0)
	    return;
	if_p = ifl_find_ip(S_interfaces, bp->bp_giaddr);
	if (if_p == NULL) { /* we aren't the gateway - discard */
	    return;
	}
	
	if ((ntohs(bp->bp_unused) & DHCP_FLAGS_BROADCAST)) {
	    if (verbose)
		syslog(LOG_INFO, "replying using broadcast IP address");
	    dst.s_addr = htonl(INADDR_BROADCAST);
	}
	else {
	    dst = bp->bp_yiaddr;
	    if (dst.s_addr != 0)
		setarp(&dst, bp->bp_chaddr, bp->bp_hlen);
	}
	if (verbose) {
	    syslog(LOG_INFO, "relaying from server '%s' to %s", 
		   bp->bp_sname, inet_ntoa(dst));
	}

	if (bootp_transmit(bootp_socket, transmit_buffer, if_name(if_p),
			   bp->bp_htype, bp->bp_chaddr, bp->bp_hlen,
			   dst, if_inet_addr(if_p),
			   S_ipport_client, S_ipport_server,
			   bp, n) < 0) {
	    syslog(LOG_INFO, "send failed, %m");
	    return;
	}
	break;
    }

    default:
	break;
    }
    if (verbose) {
	struct timeval now;
	struct timeval result;

	gettimeofday(&now, 0);
	timeval_subtract(now, S_lastmsgtime, &result);
	if (!quiet)
	    syslog(LOG_INFO, "relay time %d.%06d seconds",
		   result.tv_sec, result.tv_usec);
    }
    return;
}

static void
S_dispatch_packet(struct bootp * bp, int n, interface_t * if_p,
		  struct in_addr * dstaddr_p)
{
    dhcpol_t		options;
    dhcpol_t *		options_p = NULL;
    boolean_t		dhcp_pkt = FALSE;
    dhcp_msgtype_t	dhcp_msgtype = dhcp_msgtype_none_e;
    
    switch (bp->bp_op) {

      case BOOTREQUEST: {
	boolean_t 	handled = FALSE;

	/* get the packet options, check for dhcp */
	dhcpol_init(&options);
	if (dhcpol_parse_packet(&options, (struct dhcp *)bp, n, NULL)) {
	    options_p = &options;
	    dhcp_pkt = is_dhcp_packet(&options, &dhcp_msgtype);
	}
	
	if (debug && verbose) {
	    printf("\n---------------- Client Request --------------------\n");
	    dhcp_print_packet((struct dhcp *)bp, n);
	}

	if (bp->bp_sname[0] != '\0' 
	    && strcmp(bp->bp_sname, server_name) != 0)
	    break;

	if (bp->bp_siaddr.s_addr != 0
	    && ntohl(bp->bp_siaddr.s_addr) != ntohl(if_inet_addr(if_p).s_addr))
	    break;

	if (dhcp_pkt) { /* this is a DHCP packet */
	    if (dhcp_enabled(if_p) || old_netboot_enabled(if_p)) {
		handled = TRUE;
		dhcp_request(dhcp_msgtype, dhcp_enabled(if_p),
			     if_p, S_rxpkt, n, options_p, 
			     dstaddr_p, &S_lastmsgtime);
	    }
	    if (netboot_enabled(if_p)) {
		bsdp_request(dhcp_msgtype, if_p, S_rxpkt, n, options_p, 
			     dstaddr_p, &S_lastmsgtime);
	    }
	}
	if (handled == FALSE && old_netboot_enabled(if_p)) {
	    handled = old_netboot_request(if_p, S_rxpkt, n, options_p,
					  dstaddr_p, &S_lastmsgtime);
	}
	if (handled == FALSE && bootp_enabled(if_p)) {
	    bootp_request(if_p, S_rxpkt, n, &S_lastmsgtime);
	}
	break;
      }

      case BOOTREPLY:
	/* we're not a relay, sorry */
	break;

      default:
	break;
    }
    dhcpol_free(&options);
    if (verbose || debug) {
	struct timeval now;
	struct timeval result;

	gettimeofday(&now, 0);
	timeval_subtract(now, S_lastmsgtime, &result);
	if (!quiet)
	    syslog(LOG_INFO, "service time %d.%06d seconds",
		   result.tv_sec, result.tv_usec);
    }
    return;
}

static __inline__ void *
S_parse_control(int level, int type, int * len)
{
    struct cmsghdr *	cmsg;

    *len = 0;
    for (cmsg = CMSG_FIRSTHDR(&msg); cmsg; cmsg = CMSG_NXTHDR(&msg, cmsg)) {
	if (cmsg->cmsg_level == level 
	    && cmsg->cmsg_type == type) {
	    if (cmsg->cmsg_len < sizeof(*cmsg))
		return (NULL);
	    *len = cmsg->cmsg_len - sizeof(*cmsg);
	    return (CMSG_DATA(cmsg));
	}
    }
    return (NULL);
}

#if defined(IP_RECVIF)
static interface_t *
S_which_interface()
{
    struct sockaddr_dl *dl_p;
    char		ifname[IFNAMSIZ + 1];
    interface_t *	if_p = NULL;
    int 		len = 0;

    dl_p = (struct sockaddr_dl *)S_parse_control(IPPROTO_IP, IP_RECVIF, &len);
    if (dl_p == NULL || len == 0 || dl_p->sdl_nlen >= sizeof(ifname)) {
	return (NULL);
    }
    
    bcopy(dl_p->sdl_data, ifname, dl_p->sdl_nlen);
    ifname[dl_p->sdl_nlen] = '\0';
    if_p = ifl_find_name(S_interfaces, ifname);
    if (if_p == NULL) {
	if (verbose)
	    syslog(LOG_INFO, "unknown interface %s\n", ifname);
	return (NULL);
    }
    if (if_inet_valid(if_p) == FALSE)
	return (NULL);
    if (ptrlist_count(&S_if_list) > 0
	&& S_string_in_list(&S_if_list, ifname) == FALSE) {
	if (verbose)
	    syslog(LOG_INFO, "ignoring request on %s", ifname);
	return (NULL);
    }
    return (if_p);
}
#endif

static __inline__ struct in_addr *
S_which_dstaddr()
{
    void *	data;
    int		len = 0;
    
    data = S_parse_control(IPPROTO_IP, IP_RECVDSTADDR, &len);
    if (data && len == sizeof(struct in_addr))
	return ((struct in_addr *)data);
    return (NULL);
}

/*
 * Function: S_server_loop
 *
 * Purpose:
 *   This is the main loop that dispatches a request according to
 *   whether it is BOOTP, DHCP, or NetBoot.
 */
static void
S_server_loop()
{
    struct in_addr * 	dstaddr_p = NULL;
    struct sockaddr_in 	from = { sizeof(from), AF_INET };
    interface_t *	if_p = NULL;
    int 		mask;
    int			n;

    for (;;) {
	S_init_msg();
	msg.msg_name = (caddr_t)&from;
	msg.msg_namelen = sizeof(from);
	n = recvmsg(bootp_socket, &msg, 0);
	if (n < 0) {
	    if (verbose)
		syslog(LOG_DEBUG, "recvmsg failed, %m");
	    errno = 0;
	    continue;
	}

	if (S_sighup) {
	    static boolean_t first = TRUE;

	    if (gethostname(server_name, sizeof(server_name) - 1)) {
		server_name[0] = '\0';
		syslog(LOG_INFO, "gethostname() failed, %m");
	    }
	    else {
		syslog(LOG_INFO, "server name %s", server_name);
	    }

	    if (first == FALSE) {
		S_get_interfaces();
		S_get_network_routes();
		S_update_services();
	    }
	    first = FALSE;

	    S_get_dns();

	    { /* get the new subnet descriptions */
		u_char		err[256];
		int		i;
		subnetListNI *	new_subnets = nil;

		if (NIDomainList_count(&niSearchDomains) == 0) {
		    err[0] = '\0';
		    if (ni_local != NULL)
			new_subnets = [[subnetListNI alloc] 
					  initFromDomain:ni_local Err:err];
		}
		else for (i = 0; i < NIDomainList_count(&niSearchDomains); 
			  i++) {
		    NIDomain_t * domain;

		    domain = NIDomainList_element(&niSearchDomains, i);
		    err[0] = '\0';
		    new_subnets = [[subnetListNI alloc] 
				      initFromDomain:domain Err:err];
		    if (new_subnets != nil)
			break;
		}
		if (new_subnets == nil) {
		    syslog(LOG_INFO, "subnets init failed: %s", err);
		}
		else {
		    [subnets free];
		    subnets = new_subnets;
		    if (debug)
			[subnets print];
		}
	    }
	    dhcp_init();
	    if (S_do_netboot || S_do_old_netboot
		|| S_service_is_enabled(SERVICE_NETBOOT 
					| SERVICE_OLD_NETBOOT)) {
		if (macNC_init() == FALSE
		    || bsdp_init() == FALSE) {
		    syslog(LOG_INFO, "NetBoot service disabled");
		    S_disable_netboot();
		}
	    }
            S_sighup = FALSE;
	}

	dstaddr_p = S_which_dstaddr();
	if (debug) {
	    if (dstaddr_p == NULL)
		printf("no destination address\n");
	    else
		printf("destination address %s\n", inet_ntoa(*dstaddr_p));
	}

#if defined(IP_RECVIF)
	if_p = S_which_interface();
	if (if_p == NULL) {
	    continue;
	}
#else 
	if_p = if_first_broadcast_inet(S_interfaces);
#endif

	gettimeofday(&S_lastmsgtime, 0);
        mask = sigblock(sigmask(SIGALRM));
	S_dispatch_packet((struct bootp *)S_rxpkt, n, if_p, dstaddr_p);
	sigsetmask(mask);
    }
    return;
}

/*
 * Function: S_relay_loop
 *
 * Purpose:
 *   Relay appropriate packets.
 */
static void
S_relay_loop(struct in_addr * relay, int max_hops)
{
    struct in_addr * 	dstaddr_p = NULL;
    struct sockaddr_in 	from = { sizeof(from), AF_INET };
    interface_t *	if_p;
    int 		mask;
    int			n;

    for (;;) {
	S_init_msg();
	msg.msg_name = (caddr_t)&from;
	msg.msg_namelen = sizeof(from);
	n = recvmsg(bootp_socket, &msg, 0);
	if (n < 0) {
	    if (verbose)
		syslog(LOG_DEBUG, "recvmsg failed, %m");
	    errno = 0;
	    continue;
	}
	dstaddr_p = S_which_dstaddr();
	if (debug) {
	    if (dstaddr_p == NULL)
		printf("no destination address\n");
	    else
		printf("destination address %s\n", inet_ntoa(*dstaddr_p));
	}

#if defined(IP_RECVIF)
	if_p = S_which_interface();
	if (if_p == NULL) {
	    continue;
	}
#else 
	if_p = if_first_broadcast_inet(S_interfaces);
#endif

	gettimeofday(&S_lastmsgtime, 0);
        mask = sigblock(sigmask(SIGALRM));
	S_relay_packet(relay, max_hops, (struct bootp *)S_rxpkt, n, if_p);
	sigsetmask(mask);
    }
    exit (0); /* not reached */
}
