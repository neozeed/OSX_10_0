/*
** Program to verify subdomain delegation
** (for use with BIND 8.1.1)
** 
** Paul Albitz
** Hewlett Packard
**
** usage: check_del [-v] [-F] [-o origin] -f dns_file [-o origin] \
**                                                      [-f dns_file ...]
**
**    -v            print out successes as well as failures.  This argument
**                  must appear before the -f argument.  If -v is used twice, 
**                  check_del prints out the address used and the response packet.
**    -F            fast mode - cuts down the retransmission wait time.  This
**                  argument must appear before the -f argument.
**    -o origin     same use as origin in BIND's boot file
**    -f dns_file   file in RFC 1035 format
**
**  Build in the version 8 named directory with this command:
**       cc -Ae -o check_del -I../../include -I../../port/hpux/include \
**          ch09.check_del.c db_load.c db_glue.c ns_glue.c ../../lib/libbind.a
**
**  On SYSV or SYSV-derived operating systems, define SYSV at compile
**  time (-DSYSV).
**
*/

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <signal.h>
#include <stdio.h>
#include <errno.h>
#include <signal.h>
#include <syslog.h>
#include <ctype.h>
#include <netdb.h>
#include <arpa/nameser.h>
#include <resolv.h>

#include <isc/eventlib.h>
#include <isc/logging.h>

#include "named.h"

/*
** These variables are needed by db_load.c.
*/
int errs = 0;
struct __res_state res_save;
struct zoneinfo zone, *zones = &zone;	/* zone information */
struct hostent *hp;
int debug = 0;
struct	timeval tt;
FILE *ddt = NULL;
struct hashbuf *hashtab;	/* root hash table */
struct hashbuf *fcachetab;	/* hash table of cache read from file */
struct sockaddr_in sin;
int lineno;
struct in_addr inaddr_any;
u_int max_cache_ttl = 7*24*60*60;
int nzones;
evContext ev;
int getnum_error = 0;
log_context log_ctx;
int log_ctx_valid = 0;

/*
** These variables are used by this program.
*/
u_char query[PACKETSZ];
u_char response[PACKETSZ];
int verbose = 0;
extern int errno;
char *string_resp[200];
char *string_maybe[200];
char *string_run[200];
int noresponse = 0;
int notrunning = 0;
int mayberunning = 0;
int goodresp = 0;
int badresp = 0;
int servfail = 0;


void dostats(sig)
int sig;
{
	int j;

	printf("\n");
	if(goodresp > 0)
	    printf("%d proper domain delegations\n", goodresp);
	if(badresp > 0)
	    printf("%d improper domain delegations\n", badresp);
	if(servfail > 0)
	    printf("%d SERVFAIL answers\n", servfail);
	if(notrunning > 0)
	    printf("%d servers not running\n", notrunning);
	if(noresponse > 0)
	    printf("%d servers not responding\n", noresponse);
	if(notrunning > 0){
	    printf("\nServers not running:\n");
	    for(j = 0; j < notrunning; j++)
		printf("\t%s\n", string_run[j]);
	}
	if(noresponse > 0){
	    printf("\nServers not responding:\n");
	    for(j = 0; j < noresponse; j++)
		printf("\t%s\n", string_resp[j]);
	}
	exit(errs);
}

usage()
{
	fprintf(stderr, "check_del [-v] [-F] -o origin -f db_file [-o origin] [-f db_file]\n");
	exit(1);
}

main(argc, argv)
int argc;
char *argv[];
{
	extern char *optarg;            /* for getopt */
	extern int optind;              /* for getopt */
	char c;			        /* for getopt */
        int got_dash_o_option;
        int file_count;

	if(argc <=1)
		usage();
	/* 
	** print out a line at at time 
	*/
#ifdef SYSV
	setvbuf(stdout, NULL, _IOLBF, BUFSIZ);
#else
	setlinebuf(stdout);
#endif
	signal(SIGINT, dostats);
	res_init();
	/* 
	** the name server names will be fully qualified 
	*/
	_res.options &= ~(RES_DNSRCH|RES_DEFNAMES);
	/* 
	** keep down the retransmissions 
	*/
	_res.retry = 2;
	/* 
	** send packet; wait 20 seconds; send packet wait 40 seconds 
	** (this can be tuned down on a local network)
	*/
	_res.retrans = 20;
	res_save = _res;
	buildservicelist();
	buildprotolist();
        evCreate(&ev);

        zone.z_serial = 0;
        zone.z_origin = "";
        zone.z_type = Z_PRIMARY;
        zone.z_class = C_IN;

        got_dash_o_option = 0;
        file_count = 0;
	while ((c = getopt(argc, argv, "Ff:o:v")) != EOF)
		switch(c){
			case 'F':
					_res.retrans = 5;
					res_save = _res;
					break;
			case 'f':
                                        if(!got_dash_o_option)
                                        {
                                             fprintf(stderr, "need -o <origin>\n");
                                             exit(1);
                                        }
					errs +=db_load(optarg,
                                                       zone.z_origin,
                                                       &zone,
                                                       NULL);
                                        file_count++;
					break;
			case 'o':
                                        got_dash_o_option = 1;
			                zone.z_origin = optarg;
					break;
			case 'v':
					verbose++;
					break;
			case '?':
					usage();
					break;
		}
        if(file_count == 0)
        {
            fprintf(stderr, "need -f <file>\n");
            exit(1);
        }
	dostats();
}

/*
** Replacement routine to check NS records
** instead of actually adding the data to
** the hash tables.
*/
int db_update(const char *name,
	 struct databuf *odp,
	 struct databuf *newdp,
	 struct databuf **savedpp,
	 int flags,
	 struct hashbuf *htp,
	 struct sockaddr_in from)
{
	int i, j, n, failure;
	HEADER *header;

	/* 
	** only look at NS records 
	*/
	if(newdp->d_type == T_NS){

		/* 
		** Reset _res structure for gethostbyname
		** in case it is later modified
		*/
		_res = res_save;
		failure = 0;

		/*
		** Skip over servers that aren't running
		** or haven't responded.  
		*/
		for(j = 0; j < noresponse; j++)
			if(strcasecmp(newdp->d_data, string_resp[j]) == 0){
				if(verbose > 0)
				    printf("Skipping %s\n", newdp->d_data);
				goto skip;
			}
		for(j = 0; j < notrunning; j++)
			if(strcasecmp(newdp->d_data, string_run[j]) == 0){
				if(verbose > 0)
				    printf("Skipping %s\n", newdp->d_data);
				goto skip;
			}
		hp = gethostbyname((const char *)newdp->d_data);
		if(hp == NULL){
			printf("No address for %s\n", newdp->d_data);
		} else {
			/* 
			**try each address 
			*/
			for(i = 0; hp->h_addr_list[i]; i++){
			  	memcpy((caddr_t)&sin.sin_addr, 
				    hp->h_addr_list[i],
				    hp->h_length);
				if(verbose > 1)
				    printf("address %s\n", 
					inet_ntoa(sin.sin_addr));

				/* 
				** make SOA query 
				*/
				n = res_mkquery(QUERY, name, C_IN, T_SOA, 
					NULL, 0, NULL, query, PACKETSZ);
				if(n < 0){
					fprintf(stderr, "res_mkquery failed\n");
					continue;
				}
				header = (HEADER *) query;

				/* 
				** turn off recursion desired bit so
				** the server does not go out and
				** find the SOA data
				*/
				header->rd = 0;

				/* 
				** use only 1 address to get proper errno 
				** to determine if server was or wasn't
				** running
				*/
    				_res.nsaddr_list[0].sin_addr = 
							sin.sin_addr;
    				_res.nscount = 1;
				errno = 0;
				n = res_send(query, n, response, PACKETSZ);

				if(verbose > 1)
				    fp_query(response,stdout);
				if(n > 0) {
			    	    for(j = 0; j < mayberunning; j++)
			                if((string_maybe[j] != NULL) &&
                                           !strcasecmp(newdp->d_data, string_maybe[j])){
					    if(verbose > 0)
					        printf("\tServer %s moved off of maybe list\n",
					        string_maybe[j]);
					    free(string_maybe[j]);
					    string_maybe[j] = NULL;
					}
				    header = (HEADER *) response;
				    if (header->rcode == SERVFAIL){
					servfail++;
					printf("SERVFAIL response from %s (domain %s)\n",
					    newdp->d_data, name);
				    /*
				    ** authoritative server will have aa bit
				    ** on and something in the answer section
				    */
				    } else if ((header->aa != 1) ||
					(ntohs(header->ancount) == 0)){
					badresp++;
					printf("Server %s is not authoritative for %s\n",
					    newdp->d_data, name);
				    } else {
					goodresp++;
					if(verbose > 0)
					    printf("Server %s is authoritative for %s\n",
					    newdp->d_data, name);
				    }
				    break;
				} else {
				    failure = errno;
				    /* 
				    ** server not running, don't
				    ** bother trying other addresses
				    */
				    if(errno == ECONNREFUSED)
					break;
				}
			}
			if((verbose > 0) && (failure == ECONNREFUSED)){
			    printf("No name server running on %s (domain %s)\n",
					    newdp->d_data, name);
			} else {
			    if((verbose > 0) && (failure == ETIMEDOUT))
			        printf("No response from %s (domain %s)\n",
					    newdp->d_data, name);
			}
			/* 
			** keep track of servers not running or not responding 
			*/
			if((failure == ECONNREFUSED) && (notrunning < 200))
				string_run[notrunning++] = 
						savestr((const char *)newdp->d_data);
			if((failure == ETIMEDOUT) && (mayberunning < 200)){
			    int found;
			    found = 0;
			    for(j = 0; j < mayberunning; j++){
				if(string_maybe[j] == NULL)
				    continue;
			        if(!strcasecmp(newdp->d_data, string_maybe[j])){
				    if(noresponse < 200){
					if(verbose > 0)
					    printf("\tServer %s moved to not responding list\n",
					        string_maybe[j]);
				        string_resp[noresponse++] =
				    	    string_maybe[j];
				    }
				    found = 1;
				    break;
				}
			    }
			    if(found == 0){
				if(verbose > 0)
				    printf("\tServer %s put on maybe list\n",
				        newdp->d_data);
				string_maybe[mayberunning++] = 
						savestr((const char *)newdp->d_data);
			    }
			}

		}
	} 
    skip:
	free(newdp);
	return(0);
}

/*
** Misc routines necessary to compile
*/

struct databuf *
savedata(class, type, ttl, data, size)
	int class, type;
	u_int32_t ttl;
	u_char *data;
	int size;
{
	register struct databuf *dp;
	int bytes = (type == T_NS) ? DATASIZE(size)+INT32SZ : DATASIZE(size);

	dp = (struct databuf *) malloc(bytes);
	if (dp == NULL)
        {
		fprintf(stderr, "malloc failed\n");
                exit(1);
        }
	bzero((char*)dp, bytes);
	dp->d_next = NULL;
	dp->d_type = type;
	dp->d_class = class;
	dp->d_ttl = ttl;
	dp->d_size = size;
	dp->d_mark = 0;
	dp->d_flags = 0;
	dp->d_cred = 0;
	dp->d_clev = 0;
#ifdef NCACHE
	dp->d_rcode = NOERROR;
#endif
#ifdef STATS
	dp->d_ns = NULL;
#endif
	dp->d_nstime = 0;
	bcopy(data, dp->d_data, dp->d_size);
	return (dp);
}

const char *
zoneTypeString(zp)
	const struct zoneinfo *zp;
{
	static char ret[sizeof "(4294967296?)"];	/* 2^32 */

	switch (zp->z_type) {
	case Z_PRIMARY:		return ("primary");
	case Z_SECONDARY:	return ("secondary");
#ifdef STUBS
	case Z_STUB:		return ("stub");
#endif
	case Z_CACHE:		return ("cache");
	default:
		sprintf(ret, "(%lu?)", (u_long)zp->z_type);
		return (ret);
	}
}

struct in_addr
net_mask(struct in_addr ina) {
	u_long hl = ina_hlong(ina);
	struct in_addr ret;

	if (IN_CLASSA(hl))
		hl = IN_CLASSA_NET;
	else if (IN_CLASSB(hl))
		hl = IN_CLASSB_NET;
	else if (IN_CLASSC(hl))
		hl = IN_CLASSC_NET;
	else
		hl = INADDR_BROADCAST;
	ina_ulong(ret) = htonl(hl);
	return (ret);
}

/*
** Stubs to satisfy the compiler
*/

int
ns_nameok(const char *name, int class, struct zoneinfo *zp,
	  enum transport transport,
	  enum context context,
	  const char *owner,
	  struct in_addr source)
{
    return(1);
}

void
sched_zone_maint(zp) 
struct zoneinfo *zp;
{
}

extern char *sys_errlist[];
extern int errno;

struct netinfo *
findnetinfo(addr)
	struct in_addr addr;
{
	return((struct netinfo *) NULL);
}

printnetinfo(ntp)
	register struct netinfo *ntp;
{
    return(1);
}

int
build_secure_netlist(zp)
	struct zoneinfo *zp;
{
}

void
ns_refreshtime(zp, timebase)
	struct zoneinfo	*zp;
	time_t		timebase;
{
}

void
ns_need(need)
int need;
{
}

enum context
ns_ownercontext(type, transport)
int type;
enum transport transport;
{
}


void
sysnotify(dname, class, type) 
const char *dname; 
int class; 
int type;
{
}

enum context
ns_ptrcontext(owner)
const char *owner;
{
}

