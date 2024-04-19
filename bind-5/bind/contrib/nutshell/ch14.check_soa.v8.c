/****************************************************************
 * check_soa -- Retrieve the SOA record from each name server   *
 *     for a given domain and print out the serial number.      *
 *                                                              *
 * usage: check_soa domain                                      *
 *                                                              *
 * The following errors are reported:                           *
 *     o There is no address for a server.                      *
 *     o There is no server running on this host.               *
 *     o There was no response from a server.                   *
 *     o The server is not authoritative for the domain.        *
 *     o The response had an error response code.               *
 *     o The response had more than one answer.                 *
 *     o The response answer did not contain an SOA record.     *
 *     o The expansion of a compressed domain name failed.      *
 ****************************************************************/

/* Various header files */
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <errno.h>
#include <arpa/nameser.h>
#include <resolv.h>

/* Error variables */
extern int h_errno;  /* for resolver errors */
extern int errno;    /* general system errors */

/* Our own routines; code included later in this chapter */
void nsError();            /* report resolver errors */
void findNameServers();    /* find a domain's name servers */
void addNameServers();     /* add name servers to our list */
void queryNameServers();   /* grab SOA records from servers */
void returnCodeError();    /* report response packet errors */

/* Maximum number of name servers we will check */
#define MAX_NS 20

main(argc, argv)
int argc;
char *argv[];
{
    char *nsList[MAX_NS];  /* list of name servers */
    int  nsNum = 0;        /* number of name servers in list */

    /* sanity check: one (and only one) argument? */
    if(argc != 2){
        (void) fprintf(stderr, "usage: %s domain\n", argv[0]);
        exit(1);
    }

    (void) res_init();

    /* 
     * Find the name servers for the domain.
     * The name servers are written into nsList.
     */
    findNameServers(argv[1], nsList, &nsNum);

    /* 
     * Query each name server for the domain's SOA record.
     * The name servers are read from nsList.
     */
    queryNameServers(argv[1], nsList, nsNum);

    exit(0);
}

/****************************************************************
 * findNameServers -- find all of the name servers for the      *
 *     given domain and store their names in nsList.  nsNum is  *
 *     the number of servers in the nsList array.               *
 ****************************************************************/
void
findNameServers(domain, nsList, nsNum)
char *domain;
char *nsList[];
int  *nsNum;
{
    union {
        HEADER hdr;              /* defined in resolv.h */
        u_char buf[NS_PACKETSZ]; /* defined in arpa/nameser.h */
    } response;                  /* response buffers */
    int responseLen;             /* buffer length */

    ns_msg handle;  /* handle for response packet */

    /* 
     * Look up the NS records for the given domain name.
     * We expect the domain to be a fully qualified name, so
     * we use res_query().  If we wanted the resolver search 
     * algorithm, we would have used res_search() instead.
     */
    if((responseLen = 
           res_query(domain,      /* the domain we care about   */
                     ns_c_in,     /* Internet class records     */
                     ns_t_ns,     /* Look up name server records*/
                     (u_char *)&response,      /*response buffer*/
                     sizeof(response)))        /*buffer size    */
                                        < 0){  /*If negative    */
        nsError(h_errno, domain); /* report the error           */
        exit(1);                  /* and quit                   */
    }

    /*
     * Initialize a handle to this response.  The handle will
     * be used later to extract information from the response.
     */
    if (ns_initparse(response.buf, responseLen, &handle) < 0) {
        fprintf(stderr, "ns_initparse: %s\n", strerror(errno));
        return;
    }

    /*
     * Create a list of name servers from the response.
     * NS records may be in the answer section and/or in the
     * authority section depending on the DNS implementation.  
     * Walk through both.  The name server addresses may be in
     * the additional records section, but we will ignore them
     * since it is much easier to call gethostbyname() later
     * than to parse and store the addresses here.
     */

    /*
     * Add the name servers from the answer section.
     */
    addNameServers(nsList, nsNum, handle, ns_s_an);

    /*
     * Add the name servers from the authority section.
     */
    addNameServers(nsList, nsNum, handle, ns_s_ns);
}

/****************************************************************
 * addNameServers -- Look at the resource records from a        *
 *     section.  Save the names of all name servers.            *
 ****************************************************************/
 
void
addNameServers(nsList, nsNum, handle, section)
char *nsList[];
int  *nsNum;
ns_msg handle;
ns_sect section;
{
    int rrnum;  /* resource record number */
    ns_rr rr;   /* expanded resource record */

    int i, dup; /* misc variables */

    /*
     * Look at all the resource records in this section.
     */
    for(rrnum = 0; rrnum < ns_msg_count(handle, section); rrnum++)
    {
        /*
         * Expand the resource record number rrnum into rr.
         */
        if (ns_parserr(&handle, section, rrnum, &rr)) {
            fprintf(stderr, "ns_parserr: %s\n", strerror(errno));
        }

        /*
         * If the record type is NS, save the name of the
         * name server.
         */
        if (ns_rr_type(rr) == ns_t_ns) {

            /*
             * Allocate storage for the name.  Like any good 
             * programmer should, we test malloc's return value, 
             * and quit if it fails.
             */
            nsList[*nsNum] = (char *) malloc (MAXDNAME);
            if(nsList[*nsNum] == NULL){
                (void) fprintf(stderr, "malloc failed\n");
                exit(1);
            }

            /* Expand the name server's name */
            if (ns_name_uncompress(
                          ns_msg_base(handle),/* Start of the packet   */
                          ns_msg_end(handle), /* End of the packet     */
                          ns_rr_rdata(rr),    /* Position in the packet*/
                          nsList[*nsNum],     /* Result                */
                          MAXDNAME)           /* Size of nsList buffer */
                                    < 0) {    /* Negative: error       */
                (void) fprintf(stderr, "ns_name_uncompress failed\n");
                exit(1);
            }

            /*
             * Check the name we've just unpacked and add it to 
             * the list of servers if it is not a duplicate.
             * If it is a duplicate, just ignore it.
             */
            for(i = 0, dup=0; (i < *nsNum) && !dup; i++)
                dup = !strcasecmp(nsList[i], nsList[*nsNum]);
            if(dup) 
                free(nsList[*nsNum]);
            else
                (*nsNum)++;
        }
    }
}

/****************************************************************
 * queryNameServers -- Query each of the name servers in nsList *
 *     for the SOA record of the given domain.  Report any      *
 *     errors encountered.  (e.g., a name server not running or *
 *     the response not being an authoritative response.)  If   *
 *     there are no errors, print out the serial number for the *
 *     domain.                                                  *
 ****************************************************************/
void
queryNameServers(domain, nsList, nsNum)
char *domain;
char *nsList[];
int nsNum;
{
    union {
        HEADER hdr;               /* defined in resolv.h */
        u_char buf[NS_PACKETSZ];  /* defined in arpa/nameser.h */
    } query, response;            /* query and response buffers */
    int responseLen, queryLen;    /* buffer lengths */

    u_char     *cp;       /* character pointer to parse DNS packet */

    struct in_addr saveNsAddr[MAXNS];  /* addrs saved from _res */
    int nsCount;          /* count of addresses saved from _res */
    struct hostent *host; /* structure for looking up ns addr */
    int i;                /* counter variable */

    ns_msg handle;  /* handle for response packet */
    ns_rr rr;       /* expanded resource record */

    /*
     * Save the _res name server list since 
     * we will need to restore it later.
     */
    nsCount = _res.nscount;
    for(i = 0; i < nsCount; i++)
      saveNsAddr[i] = _res.nsaddr_list[i].sin_addr;

    /*
     * Turn off the search algorithm and turn off appending 
     * the default domain before we call gethostbyname(); the 
     * name server names will be fully qualified.
     */
    _res.options &= ~(RES_DNSRCH | RES_DEFNAMES);

    /*
     * Query each name server for an SOA record.
     */
    for(nsNum-- ; nsNum >= 0; nsNum--){

        /* 
         * First, we have to get the IP address of every server.
         * So far, all we have are names.  We use gethostbyname
         * to get the addresses, rather than anything fancy.
         * But first, we have to restore certain values in _res 
         * because _res affects gethostbyname().  (We altered
         * _res in the previous iteration through the loop.)
         *
         * We can't just call res_init() again to restore
         * these values since some of the _res fields are 
         * initialized when the variable is declared, not when 
         * res_init() is called.
         */
        _res.options |= RES_RECURSE;  /* recursion on (default) */
        _res.retry = 4;               /* 4 retries (default)    */
        _res.nscount = nsCount;       /* original name servers  */
        for(i = 0; i < nsCount; i++)
            _res.nsaddr_list[i].sin_addr = saveNsAddr[i];

        /* Look up the name server's address */
        host = gethostbyname(nsList[nsNum]);
        if (host == NULL) {
            (void) fprintf(stderr,"There is no address for %s\n",
                                              nsList[nsNum]);
            continue; /* nsNum for-loop */
        }

        /*
         * Now get ready for the real fun.  host contains IP 
         * addresses for the name server we're testing.
         * Store the first address for host in the _res 
         * structure.  Soon, we'll look up the SOA record...
         */
        (void) memcpy((void *)&_res.nsaddr_list[0].sin_addr,
           (void *)host->h_addr_list[0], (size_t)host->h_length);
        _res.nscount = 1;

        /*
         * Turn off recursion.  We don't want the name server
         * querying another server for the SOA record; this name 
         * server ought to be authoritative for this data.
         */
        _res.options &= ~RES_RECURSE;

        /*
         * Reduce the number of retries.  We may be checking
         * several name servers, so we don't want to wait too
         * long for any one server.  With two retries and only
         * one address to query, we'll wait at most 15 seconds.
         */
        _res.retry = 2;

        /*
         * We want to see the response code in the next
         * response, so we must make the query packet and 
         * send it ourselves instead of having res_query()
         * do it for us.  If res_query() returned -1, there
         * might not be a response to look at.  
         *
         * There is no need to check for res_mkquery() 
         * returning -1.  If the compression was going to 
         * fail, it would have failed when we called 
         * res_query() earlier with this domain name.
         */
        queryLen = res_mkquery(
                     ns_o_query,      /* regular query         */
                     domain,          /* the domain to look up */
                     ns_c_in,         /* Internet type         */
                     ns_t_soa,        /* Look up an SOA record */
                     (u_char *)NULL,  /* always NULL       */
                     0,               /* length of NULL        */
                     (u_char *)NULL,  /* always NULL       */
                     (u_char *)&query,/* buffer for the query  */
                     sizeof(query));  /* size of the buffer    */

        /*
         * Send the query packet.  If there is no name server
         * running on the target host, res_send() returns -1
         * and errno is ECONNREFUSED.  First, clear out errno.
         */
        errno = 0;
        if((responseLen = res_send((u_char *)&query,/* the query  */
                                   queryLen,        /* true length*/
                                   (u_char *)&response,/*buffer  */
                                   sizeof(response)))  /*buf size*/
                                        < 0){          /* error  */
            if(errno == ECONNREFUSED) { /* no server on the host */
                (void) fprintf(stderr, 
                    "There is no name server running on %s\n",
                    nsList[nsNum]);
            } else {                    /* anything else: no response */
                (void) fprintf(stderr, 
                    "There was no response from %s\n", 
                    nsList[nsNum]);
            }
            continue; /* nsNum for-loop */
        }

        /*
         * Initialize a handle to this response.  The handle will
         * be used later to extract information from the response.
         */
        if (ns_initparse(response.buf, responseLen, &handle) < 0) {
            fprintf(stderr, "ns_initparse: %s\n", strerror(errno));
            return;
        }

        /*
         * If the response reports an error, issue a message
         * and proceed to the next server in the list.
         */
        if(ns_msg_getflag(handle, ns_f_rcode) != ns_r_noerror){
            returnCodeError(ns_msg_getflag(handle, ns_f_rcode), 
                                                  nsList[nsNum]);
            continue; /* nsNum for-loop */
        }

        /*
         * Did we receive an authoritative response?  Check the 
         * authoritative answer bit.  If the server isn't
         * authoritative, report it, and go on to the next server.
         */
        if(!ns_msg_getflag(handle, ns_f_aa)){
            (void) fprintf(stderr, 
                "%s is not authoritative for %s\n",
                nsList[nsNum], domain);
            continue; /* nsNum for-loop */
        }

        /* 
         * The response should only contain one answer; if more,
         * report the error, and proceed to the next server.
         */
        if(ns_msg_count(handle, ns_s_an) != 1){
            (void) fprintf(stderr, 
                "%s: expected 1 answer, got %d\n",
                nsList[nsNum], ns_msg_count(handle, ns_s_an));
            continue; /* nsNum for-loop */
        }

        /*
         * Expand the answer section record number 0 into rr.
         */
        if (ns_parserr(&handle, ns_s_an, 0, &rr)) {
                if (errno != ENODEV){
                        fprintf(stderr, "ns_parserr: %s\n", strerror(errno));
                }
        }

        /* 
         * We asked for an SOA record; if we got something else,
         * report the error and proceed to the next server.
         */
        if (ns_rr_type(rr) != ns_t_soa) {
            (void) fprintf(stderr, 
                "%s: expected answer type %d, got %d\n",
                nsList[nsNum], ns_t_soa, ns_rr_type(rr));
            continue; /* nsNum for-loop */
        }

        /*
         * Set cp to point the the SOA record.
         */ 
        cp = (u_char *)ns_rr_rdata(rr);

        /*
         * Skip the SOA origin and mail address, which we don't
         * care about.  Both are standard "compressed names."
         */
        ns_name_skip(&cp, ns_msg_end(handle));
        ns_name_skip(&cp, ns_msg_end(handle));

        /* cp now points to the serial number; print it. */
        (void) printf("%s has serial number %d\n", 
            nsList[nsNum], ns_get32(cp));

    } /* end of nsNum for-loop */
}

/****************************************************************
 * nsError -- Print an error message from h_errno for a failure *
 *     looking up NS records.  res_query() converts the DNS     *
 *     packet return code to a smaller list of errors and       *
 *     places the error value in h_errno.  There is a routine   *
 *     called herror() for printing out strings from h_errno    *
 *     like perror() does for errno.  Unfortunately, the        *
 *     herror() messages assume you are looking up address      *
 *     records for hosts.  In this program, we are looking up   *
 *     NS records for domains, so we need our own list of error *
 *     strings.                                                 *
 ****************************************************************/
void
nsError(error, domain)
int error;
char *domain;
{
    switch(error){
        case HOST_NOT_FOUND:
          (void) fprintf(stderr, "Unknown domain: %s\n", domain);
          break;
        case NO_DATA:
          (void) fprintf(stderr, "No NS records for %s\n", domain); 
          break;
        case TRY_AGAIN:
          (void) fprintf(stderr, "No response for NS query\n");
          break;
        default:
          (void) fprintf(stderr, "Unexpected error\n");
          break;
    }
}

/****************************************************************
 * returnCodeError -- print out an error message from a DNS     *
 *     response return code.                                    *
 ****************************************************************/
void
returnCodeError(rcode, nameserver)
ns_rcode rcode;
char *nameserver;
{
    (void) fprintf(stderr, "%s: ", nameserver);
    switch(rcode){
        case ns_r_formerr:
          (void) fprintf(stderr, "FORMERR response\n");
          break;
        case ns_r_servfail:
          (void) fprintf(stderr, "SERVFAIL response\n");
          break;
        case ns_r_nxdomain:
          (void) fprintf(stderr, "NXDOMAIN response\n");
          break;
        case ns_r_notimpl:
          (void) fprintf(stderr, "NOTIMP response\n");
          break;
        case ns_r_refused:
          (void) fprintf(stderr, "REFUSED response\n");
          break;
        default:
          (void) fprintf(stderr, "unexpected return code\n");
          break;
    }
}
