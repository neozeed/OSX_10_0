/*
 * Copyright (c) 1999 Apple Computer, Inc. All rights reserved.
 *
 * @APPLE_LICENSE_HEADER_START@
 * 
 * "Portions Copyright (c) 1999 Apple Computer, Inc.  All Rights
 * Reserved.  This file contains Original Code and/or Modifications of
 * Original Code as defined in and that are subject to the Apple Public
 * Source License Version 1.0 (the 'License').  You may not use this file
 * except in compliance with the License.  Please obtain a copy of the
 * License at http://www.apple.com/publicsource and read it before using
 * this file.
 * 
 * The Original Code and all software distributed under the License are
 * distributed on an 'AS IS' basis, WITHOUT WARRANTY OF ANY KIND, EITHER
 * EXPRESS OR IMPLIED, AND APPLE HEREBY DISCLAIMS ALL SUCH WARRANTIES,
 * INCLUDING WITHOUT LIMITATION, ANY WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE OR NON-INFRINGEMENT.  Please see the
 * License for the specific language governing rights and limitations
 * under the License."
 * 
 * @APPLE_LICENSE_HEADER_END@
 */

/*
 * Server-side procedure implementation
 * Copyright (C) 1989 by NeXT, Inc.
 */
#include <NetInfo/config.h>
#include <NetInfo/project_version.h>
#include "ni_server.h"
#include <NetInfo/mm.h>
#include <NetInfo/system.h>
#include <NetInfo/system_log.h>
#include <NetInfo/network.h>
#include <NetInfo/systhread.h>
#include "ni_globals.h"
#include "notify.h"
#include "ni_dir.h"
#include <NetInfo/socket_lock.h>
#include "alert.h"
#include "getstuff.h"
#include "proxy_pids.h"
#include <sys/socket.h>
#include "sanitycheck.h"
#include "multi_call.h"
#include <sys/time.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>
#include <arpa/inet.h>
#include <sys/param.h>
#include <sys/signal.h>
#include <sys/stat.h>
#include <rpc/rpc.h>
#include <rpc/pmap_clnt.h>
#include "bootparam_prot.h"
#ifdef _DHCP_BINDING_
#ifdef _OS_VERSION_MACOS_X_
#define MOSX
#endif
#include <mach/mach.h>
#include <mach/mach_error.h>
#include "ipconfig.h"
#include "ipconfig_ext.h"
#endif

#ifdef _OS_NEXT_
/* Support the old rpcgen, which doesn't append the _svc suffix
 * to server side stubs.
 */
#define	_ni_ping_2_svc		_ni_ping_2
#define _ni_statistics_2_svc	_ni_statistics_2
#define _ni_root_2_svc		_ni_root_2
#define _ni_self_2_svc		_ni_self_2
#define _ni_parent_2_svc	_ni_parent_2
#define _ni_children_2_svc	_ni_children_2
#define _ni_create_2_svc	_ni_create_2
#define _ni_destroy_2_svc	_ni_destroy_2
#define _ni_read_2_svc		_ni_read_2
#define _ni_write_2_svc		_ni_write_2
#define _ni_lookup_2_svc	_ni_lookup_2
#define _ni_lookupread_2_svc	_ni_lookupread_2
#define _ni_list_2_svc		_ni_list_2
#define _ni_listall_2_svc	_ni_listall_2
#define _ni_readprop_2_svc	_ni_readprop_2
#define _ni_writeprop_2_svc	_ni_writeprop_2
#define _ni_listprops_2_svc	_ni_listprops_2
#define _ni_createprop_2_svc	_ni_createprop_2
#define _ni_destroyprop_2_svc	_ni_destroyprop_2
#define _ni_renameprop_2_svc	_ni_renameprop_2
#define _ni_createname_2_svc	_ni_createname_2
#define _ni_writename_2_svc	_ni_writename_2
#define _ni_readname_2_svc	_ni_readname_2
#define _ni_destroyname_2_svc	_ni_destroyname_2
#define _ni_bind_2_svc		_ni_bind_2
#define _ni_rparent_2_svc	_ni_rparent_2
#define _ni_crashed_2_svc	_ni_crashed_2
#define _ni_readall_2_svc	_ni_readall_2
#define _ni_resync_2_svc	_ni_resync_2
#endif _OS_NEXT_

CLIENT *svctcp_getclnt(SVCXPRT *xprt);

#define MAXINTSTRSIZE sizeof("4294967296") /* size of largest integer */

#define NI_SEPARATOR '/'		/* separator for netinfo values */

/*
 * These definitions are used when doing the search for the parent. When
 * a timeout occurs because we are unable to find the parent server, we
 * sleep for 10 seconds, checking once a second to see if the user decided
 * to abort the search. After 30 seconds, the search is continued again.
 */
#ifdef _OS_APPLE_
#define PARENT_NINTERVALS 0
#else
#define PARENT_NINTERVALS 10
#endif
#define PARENT_SLEEPTIME 1	

/*
 * How long to sleep after catching an rparent reply.
 * This reduces a flood of ICMP port unreachable messages.
 */
#define RPARENT_CATCH_SLEEP_SECONDS 2

/*
 * Ping timeout is 1/2 second per try, with 10 tries.  Total timeout is 5 seconds.
 */
 
#define PING_TIMEOUT_SEC  0     
#define PING_TIMEOUT_USEC 500000
#define PING_CALL_SEC  5     
#define PING_CALL_USEC 0

/* Number of minutes to force between attempts to see if we are still root */

#define NETROOT_TIMEOUT_MINS	10

extern bool_t readall();
static int ni_ping(u_long, ni_name);

extern void setproctitle(char *fmt, ...);

extern int have_notifications_pending(void);

/* for getting language */
#define NAME_LOCALCONFIG "localconfig"
#define NAME_LANGUAGE "language"

#define KEY_PARENT_SERVER_ADDR "ServerAddresses"
#define KEY_PARENT_SERVER_TAG "ServerTags"

#define BOOTPARAM_FILEID (bp_fileid_t)"netinfo_parent"
#define BOOTPARAM_TAG "-BOOTPARAMS-"

#define DHCP_TAG "-DHCP-"

#ifdef _DHCP_BINDING_
#define DHCP_NETINFO_SERVER_ADDRESS 	112
#define DHCP_NETINFO_SERVER_TAG 		113
#endif

static struct in_addr bootparam_addr;
static ni_name bootparam_tag;

#define forever for(;;)

typedef enum
{
	TokenNULL = 0,
	TokenWord = 1,
	TokenXMLArrayStart = 2,
	TokenXMLArrayEnd = 3,
	TokenXMLDictStart = 4,
	TokenXMLDictEnd = 5,
	TokenXMLKeyStart = 6,
	TokenXMLKeyEnd = 7
} token_type_t;

typedef struct
{
	token_type_t type;
	char *value;
} token_t;

/*
 * Is this call an update from the master?
 * XXX: the method used is to look for a privileged port from the master
 * server. There is no way to distinguish this from a call from a root
 * user on the master (versus the master server process).
 */
static int
isupdate(struct svc_req *req)
{
	struct sockaddr_in *sin = svc_getcaller(req->rq_xprt);

	/*
	 * XXX: Do not allow the client library as root to look
	 * like an update. Since the client library always sets AUTH_UNIX,
	 * and updates never do, we can safely test for AUTH_UNIX.
	 */
	if (req->rq_cred.oa_flavor == AUTH_UNIX) {
		return (FALSE);
	}
	/*
	 * XXX: Master could have multiple addresses: should check all of
	 * them.
	 */
	return (sin->sin_addr.s_addr == master_addr && 
		ntohs(sin->sin_port) < IPPORT_RESERVED);
}

/*
 * Authenticate a NetInfo call. Only required for write operations.
 * NetInfo uses passwords for authentications, but does not send them
 * in the clear. Instead, a trivial authentication system is used to
 * defeat packet browsers.
 *
 * UNIX-style RPC authentication is used, with a gross hack that the encrypted
 * password is placed in the machine-name field.
 *
 * XXX: design a better authentication system (non-trivial!)
 */
static ni_status
authenticate(void *ni, struct svc_req *req)
{
	struct sockaddr_in *sin = svc_getcaller(req->rq_xprt);
	struct authunix_parms *aup;
	char *p;
	ni_namelist nl;
	ni_status status;
	ni_id id;
	ni_idlist idl;
	char uidstr[MAXINTSTRSIZE];
	int u, found;

	/*
	 * Root on the local machine can do anything
	 */
	if (sys_is_my_address(&(sin->sin_addr)) && 
	    ntohs(sin->sin_port) < IPPORT_RESERVED) {
		ni_setuser(ni, ACCESS_USER_SUPER);
		return (NI_OK);
	}
	if (req->rq_cred.oa_flavor != AUTH_UNIX) {
		ni_setuser(ni, NULL);
		return (NI_OK);
	}
	aup = (struct authunix_parms *)req->rq_clntcred;
	/*
	 * Pull user-supplied password out of RPC message.
	 * Our trivial encryption scheme just inverts the bits
	 */
	for (p = aup->aup_machname; *p; p++) *p = ~(*p);

	status = ni_root(ni, &id);
	if (status != NI_OK) {
		return (status);
	}

	/*
	 * Get /users directory
	 */
	NI_INIT(&idl);
	status = ni_lookup(ni, &id, NAME_NAME, NAME_USERS, &idl);
	if (status != NI_OK) {
		system_log(LOG_ERR,
		       "Cannot authenticate user %d from %s:%hu - no /%s "
		       "directory: %s", aup->aup_uid,
		       inet_ntoa(sin->sin_addr), ntohs(sin->sin_port),
		       NAME_USERS,
		       ni_error(status));
		auth_count[BAD]++;
		return (status == NI_NODIR ? NI_NOUSER : status);
	}

	id.nii_object = idl.ni_idlist_val[0];
	ni_idlist_free(&idl);

	/*
	 * Find all users with this uid
	 */
	sprintf(uidstr, "%d", aup->aup_uid);
	NI_INIT(&idl);
	status = ni_lookup(ni, &id, NAME_UID, uidstr, &idl);
	if (status != NI_OK) {
		system_log(LOG_ERR, "Cannot find user %d from %s:%hu: %s",
		       aup->aup_uid,
		       inet_ntoa(sin->sin_addr), ntohs(sin->sin_port),
		       ni_error(status));
		auth_count[BAD]++;
		return (status == NI_NODIR ? NI_NOUSER : status);
	}

	/*
	 * Check each user for a password match
	 */
	found = 0;
	for (u = 0; u < idl.ni_idlist_len; u++) {
		id.nii_object = idl.ni_idlist_val[u];
		NI_INIT(&nl);
		status = ni_lookupprop(ni, &id, NAME_PASSWD, &nl);
		if (status == NI_OK) {
			if ((nl.ni_namelist_len == 0) || (nl.ni_namelist_val[0][0] == '\0'))
			{
				/*
				 * Free Parking: user has no password
				 */
				found = 1;
				ni_namelist_free(&nl);
				break;
			}

			if (!strcmp(nl.ni_namelist_val[0],
				crypt(aup->aup_machname, nl.ni_namelist_val[0])) != 0)
			{
				/*
				 * Password match
				 */
				found = 1;
				ni_namelist_free(&nl);
				break;
			}
		}
		ni_namelist_free(&nl);
	}

	ni_idlist_free(&idl);
	if (!found) {
		/*
		 * No user with this uid with no password or a matching password
		 */
		system_log(LOG_ERR, "Authentication error for user "
		       "%d from %s:%hu",
		       aup->aup_uid,
		       inet_ntoa(sin->sin_addr), ntohs(sin->sin_port),
		       ni_error(status));
		auth_count[BAD]++;
		return (NI_AUTHERROR);
	}

	NI_INIT(&nl);
	status = ni_lookupprop(ni, &id, NAME_NAME, &nl);
	if (status != NI_OK) {
		system_log(LOG_ERR,
		       "User %d from %s:%hu - name prop not found during "
		       "authentication",
		       aup->aup_uid,
		       inet_ntoa(sin->sin_addr), ntohs(sin->sin_port),
		       ni_error(status));
		auth_count[BAD]++;
		return (status == NI_NOPROP ? NI_NOUSER : status);
	}
	if (nl.ni_namelist_len == 0) {
		system_log(LOG_ERR,
		       "User %d from %s:%hu - name value not found during "
		       "authentication",
		       aup->aup_uid,
		       inet_ntoa(sin->sin_addr), ntohs(sin->sin_port));
		auth_count[BAD]++;
		return (NI_NOUSER);
	}

	/* If the user has uid 0, allow root access */
	if (aup->aup_uid == 0) ni_setuser(ni, ACCESS_USER_SUPER); 
	else ni_setuser(ni, nl.ni_namelist_val[0]);

	auth_count[GOOD]++;
	system_log(LOG_NOTICE,
		"Authenticated user %s [%d] from %s:%hu", 
		nl.ni_namelist_val[0], aup->aup_uid,
		inet_ntoa(sin->sin_addr), ntohs(sin->sin_port));

	ni_namelist_free(&nl);
	return (NI_OK);
}

/*
 * Validate that a write-call is allowed. A write call is allowed
 * to the master if the user is correctly authenticated and permission
 * is allowed. To the clone, the write is only allowed if it comes
 * from the master (an update).
 */
static ni_status
validate_write(struct svc_req *req)
{
	ni_status status;

	status = NI_OK;
	ni_setuser(db_ni, NULL);
	if (i_am_clone) {
		if (!isupdate(req)) {
			status = NI_RDONLY;
		} else {
			ni_setuser(db_ni, ACCESS_USER_SUPER);
		}
	} else {
		status = authenticate(db_ni, req);
	}
	/*
	 * Do master side of readall in separate process.
	 * We need to lock out modifications during a readall, to
	 * avoid just needing another one far too soon.  Since all
	 * the modifcations come through here, this is a good place
	 * to enforce things.
	 */
	if ((sending_all > 0) || db_lockup) {
	    status = NI_MASTERBUSY;
	}
	return (status);
}

/*
 * The NetInfo PING procedure
 */
void *
_ni_ping_2_svc(
	   void *arg,
	   struct svc_req *req
	   )
{
	return ((void *)~0);
}

/*
 * The NetInfo statistics procedure
 */
ni_proplist *
_ni_statistics_2_svc(
		 void *arg,
		 struct svc_req *req
		 )
{
	/*
	 * Statistics are updated in ni_prog_2_svc() in ni_prot_svc.c,
	 * which lives in SYMS/netinfod_syms.
	 *
	 * XXX This definition really belongs in ni.x, but we're not
	 * going to muck with the protocol definition file at this time.
	 * This definition must be shared with ni_prot_svc.c
	 */
	extern struct ni_stats {
		unsigned long ncalls;
		unsigned long time;
	    } netinfod_stats[];
	#define STATS_PROCNUM	0
	#define STATS_NCALLS	1
	#define STATS_TIME	2
	#define N_STATS_VALS	(STATS_TIME+1)
	int total_calls = 0;		/* We'll total things here */
	interface_list_t *ifaces;
	struct timeval now;
	extern char *procname(int);

	/*
	 * Sizes of values, excluding call stats
	 * Note: for properties with multiple values the size should reflect the
	 *       max space required for any one value.
	 */
	static int props_sizes[] = {
	    MAXINTSTRSIZE,	/* checksum: */
	    BUFSIZ,		/* server_version: */
	    NI_NAME_MAXLEN+1,	/* tag: max(NI_NAME_MAXLEN+1, 7, MAXINTSTRSIZE) */
	    16,			/* ip_address: "xxx.yyy.zzz.www" */
	    MAXHOSTNAMELEN + 1,	/* hostname: */
	    MAXINTSTRSIZE + 17,	/* write_locked: strlen(SENDING_ALLn_STG)+MAXINTSTRSIZE */
	    MAXINTSTRSIZE,	/* notify_threads: 3 of MAXINTSTRSIZE */
	    MAXINTSTRSIZE,	/* notifications_pending: */
	    MAXINTSTRSIZE,	/* authentications: 4 of MAXINTRSTRSIZE */
	    MAXINTSTRSIZE,	/* readall_proxies: max(MAXINTSTRSIZE, "strict"|"loose") */
	    MAXINTSTRSIZE,	/* cleanup_wait: */
	    MAXINTSTRSIZE,	/* total_calls: */
	    NI_NAME_MAXLEN+1+16	/* binding: "xxx.yyy.zzz.www/tag" */
	};

#define wProps(i, fmt, val) \
	wPropsN(i,0, fmt, val)
#define wPropsN(i, j, fmt, val) \
	(void)sprintf(props[i].nip_val.ni_namelist_val[j], fmt, val)

	static ni_property props[] = {
	    {"checksum", {1, NULL}},
#define P_CHECKSUM 0
	    {"server_version", {1, NULL}},
#define P_VERSION 1
	    /*
	     * tag can have two or three values.  If master, there'll
	     * be 3; if clone, 2.  We'll allocate space for 3, to
	     * simplify the code.
	     */
	    {"tag", {3, NULL}},	/* tag; master & #clones, or clone */
#define P_TAG 2
#define STATS_TAG 0
#define STATS_MASTER 1
#define STATS_NCLONES 2
	    {"ip_address", {1, NULL}},
#define P_ADDR 3
	    {"hostname", {1, NULL}},
#define P_HOST 4
	    {"write_locked", {1, NULL}},
#define P_LOCKED 5
#define SENDING_ALL1_STG "Yes (a readall)"
#define SENDING_ALLn_STG "Yes (%u readalls)"
#define READING_ALL_STG "clone%s"
	    {"notify_threads", {3, NULL}},	/* max, current, latency */
#define P_THREADS 6
#define STATS_THREADS_USED 0
#define STATS_THREADS_MAX 1
#define STATS_THREADS_LATENCY 2
	    {"notifications_pending", {1, NULL}},
#define P_PENDING 7
	    {"authentications", {4, NULL}},	/* {user,dir}:{good,bad} */
#define P_AUTHS 8
	    {"readall_proxies", {2, NULL}},	/* max, strict */
#define P_PROXIES 9
#define STATS_PROXIES 0
#define STATS_PROXIES_STRICT 1
	    {"cleanup_wait", {2, NULL}},	/* minutes (!), remaining */
#define P_CLEANUP 10
#define STATS_CLEANUPTIME 0
#define STATS_CLEANUP_TOGO 1
	    {"total_calls", {1, NULL}},
#define P_CALLS 11
	    {"binding", {1, NULL}},		/* unknown, notResponding, addr/tag, root, forcedRoot */
#define P_BINDING 12
	    /*
	     * 3 values of following properties:
	     * procnum, ncalls, time (usec)
	     */
#define PROC_STATS_START	13
	    {NULL, {N_STATS_VALS, NULL}},	/* 0: ping */
	    {NULL, {N_STATS_VALS, NULL}},
	    {NULL, {N_STATS_VALS, NULL}},
	    {NULL, {N_STATS_VALS, NULL}},
	    {NULL, {N_STATS_VALS, NULL}},
	    {NULL, {N_STATS_VALS, NULL}},
	    {NULL, {N_STATS_VALS, NULL}},
	    {NULL, {N_STATS_VALS, NULL}},
	    {NULL, {N_STATS_VALS, NULL}},
	    {NULL, {N_STATS_VALS, NULL}},
	    {NULL, {N_STATS_VALS, NULL}},	/* 10: lookup */
	    {NULL, {N_STATS_VALS, NULL}},
	    {NULL, {N_STATS_VALS, NULL}},
	    {NULL, {N_STATS_VALS, NULL}},
	    {NULL, {N_STATS_VALS, NULL}},
	    {NULL, {N_STATS_VALS, NULL}},
	    {NULL, {N_STATS_VALS, NULL}},
	    {NULL, {N_STATS_VALS, NULL}},
	    {NULL, {N_STATS_VALS, NULL}},
	    {NULL, {N_STATS_VALS, NULL}},
	    {NULL, {N_STATS_VALS, NULL}},	/* 20: readname */
	    {NULL, {N_STATS_VALS, NULL}},
	    {NULL, {N_STATS_VALS, NULL}},
	    {NULL, {N_STATS_VALS, NULL}},
	    {NULL, {N_STATS_VALS, NULL}},
	    {NULL, {N_STATS_VALS, NULL}},
	    {NULL, {N_STATS_VALS, NULL}},
	    {NULL, {N_STATS_VALS, NULL}},
	    {NULL, {N_STATS_VALS, NULL}}	/* 28: lookupread */
	};

	int i, j;
	static unsigned char first_time = TRUE;
	static ni_proplist res;

	if (req == NULL) {
		return (NULL);
	}

	if (first_time) {
	    /* Save myself some typing (and static memory)... */
	    first_time = FALSE;
	    for (i = 0; i < PROC_STATS_START; i++) {
		/* Initialize the props other than call stats */
		if (NULL == (props[i].nip_val.ni_namelist_val =
			(ni_name *)malloc(props[i].nip_val.ni_namelist_len *
					  sizeof(ni_name)))) {
			system_log(LOG_ALERT, 
				"Couldn't allocate memory for statistics");
                	system_log(LOG_ALERT, "Aborting!");
                	abort();
		}
		for (j = 0; j < props[i].nip_val.ni_namelist_len; j++) {
		    if (NULL == (props[i].nip_val.ni_namelist_val[j] =
				    (ni_name)malloc(props_sizes[i]))) {
				system_log(LOG_ALERT, 
					"Couldn't allocate memory for statistics");
                		system_log(LOG_ALERT, "Aborting!");
                		abort();
		    }
		}
	    }
	    for (i = PROC_STATS_START;
		 i <= PROC_STATS_START+_NI_LOOKUPREAD;
		 i++) {
		/* Initialize the call stats */
		props[i].nip_name = procname(i - PROC_STATS_START);
		if (NULL == (props[i].nip_val.ni_namelist_val =
				(ni_name *)malloc(sizeof(ni_name) *
						    N_STATS_VALS))) {
				system_log(LOG_ALERT, 
					"Couldn't allocate memory for statistics");
                		system_log(LOG_ALERT, "Aborting!");
                		abort();
		}
		for (j = 0; j < N_STATS_VALS; j++) {
		    if (NULL == (props[i].nip_val.ni_namelist_val[j] = 
				    (ni_name)malloc(MAXINTSTRSIZE+1))) {
				system_log(LOG_ALERT, 
					"Couldn't allocate memory for statistics");
                		system_log(LOG_ALERT, "Aborting!");
                		abort();
		    }
		}
		/* Set up the (static) procedure number */
		wPropsN(i, STATS_PROCNUM, "%u", i - PROC_STATS_START);
	    }

	    /* generate static information */

	    /* server_version */
	    wProps(P_VERSION, "%s", _PROJECT_VERSION_);

	    /* tag: tag, master (w/# clones) or clone */
	    wPropsN(P_TAG, STATS_TAG, "%s", db_tag);
	    if (!i_am_clone) {
		wPropsN(P_TAG, STATS_MASTER, "%s", "master");
		props[P_TAG].nip_val.ni_namelist_len = 3;
	    } else {
		wPropsN(P_TAG, STATS_MASTER, "%s", "clone");
		props[P_TAG].nip_val.ni_namelist_len = 2;
	    }

	    /* ip_address */
		ifaces = sys_interfaces();
		if (ifaces == NULL)
		{
			wProps(P_ADDR, "%s", "0.0.0.0");
		}
		else
		{
			for (i = 0; i < ifaces->count; i++)
			{
				if ((ifaces->interface[i].flags & IFF_UP) == 0) continue;
				if (ifaces->interface[i].flags & IFF_LOOPBACK) continue;
				
				wProps(P_ADDR, "%s", inet_ntoa(ifaces->interface[i].addr));
			}
		}

	    /* hostname */
	    wProps(P_HOST, "%s", sys_hostname());

	}

	/* checksum */
	wProps(P_CHECKSUM, "%u", ni_getchecksum(db_ni));

	/* identify whether the extended statistics should be returned */
	if (req->rq_cred.oa_flavor == AUTH_UNIX) {
		struct authunix_parms *aup;
		char *p;
		
		aup = (struct authunix_parms *)req->rq_clntcred;
		/*
		 * Pull the user-supplied password out of RPC message
		 * and undo the (trivial) encryption scheme.
		 */
		for (p = aup->aup_machname; *p; p++) *p = ~(*p);

		if (strcmp(aup->aup_machname, "checksum") == 0) {
			res.ni_proplist_len =  1;	/* assumes "checksum" is first property */
			res.ni_proplist_val = props;
			return (&res);
		}
	}

	/* tag: tag, master (w/# clones) or clone */
	if (!i_am_clone) {
	    wPropsN(P_TAG, STATS_NCLONES, "%d", count_clones());
	}

	/* write_locked */
	if (i_am_clone) {
	    wProps(P_LOCKED, READING_ALL_STG,
		   reading_all ? " (reading all)" : "");
	} else if (sending_all > 0) {
	    wProps(P_LOCKED,
		   1 == sending_all ? SENDING_ALL1_STG : SENDING_ALLn_STG,
		   sending_all);
	} else if (db_lockup) {
	    /* If this is longer than SENDING_ALLn_STG, change props_sizes */
	    wProps(P_LOCKED, "%s", "Yes (due to SIGINT)");
	} else {
	    wProps(P_LOCKED, "%s", "No");
	}

	/* notify_threads: current, max, latency */
	wPropsN(P_THREADS, STATS_THREADS_USED, "%u",
		count_notify_subthreads());
	wPropsN(P_THREADS, STATS_THREADS_MAX, "%u", max_subthreads);
	wPropsN(P_THREADS, STATS_THREADS_LATENCY, "%u", update_latency_secs);

	/* notifications_pending */
	wProps(P_PENDING, "%u", notifications_pending());

	/* Authentications, good and bad */
	wPropsN(P_AUTHS, GOOD, "%u", auth_count[GOOD]);
	wPropsN(P_AUTHS, BAD, "%u", auth_count[BAD]);
	wPropsN(P_AUTHS, WGOOD, "%u", auth_count[WGOOD]);
	wPropsN(P_AUTHS, WBAD, "%u", auth_count[WBAD]);

	/* readall_proxies: max, strict */
	wPropsN(P_PROXIES, STATS_PROXIES, "%d", max_readall_proxies);
	wPropsN(P_PROXIES, STATS_PROXIES_STRICT, "%s",
		      strict_proxies ? "strict" : "loose");

	/* cleanup_wait (in seconds internally, in minutes externally!) */
	gettimeofday(&now, NULL);
	wPropsN(P_CLEANUP, STATS_CLEANUPTIME, "%d", cleanupwait/60);
#ifdef _OS_NEXT_
	wPropsN(P_CLEANUP, STATS_CLEANUP_TOGO, "%ld",
	       cleanupwait < 0 ? -1 : (cleanuptime - now.tv_sec)/60);
#else
	wPropsN(P_CLEANUP, STATS_CLEANUP_TOGO, "%d",
	       cleanupwait < 0 ? -1 : (cleanuptime - now.tv_sec)/60);
#endif

	/* current binding status */
	if (forcedIsRoot)
	{
		wProps(P_BINDING, "%s", "forcedRoot");
	}
	else
	{
		switch ((ni_status) latestParentStatus)
		{
			case NI_NORESPONSE:
				wProps(P_BINDING,  "%s", "notResponding");
				break;
			case NI_NETROOT:
				wProps(P_BINDING,  "%s", "root");
				break;
			case NI_OK:
				wProps(P_BINDING, "%s", latestParentInfo);
				break;
			case NI_NOTMASTER:
			default:
				wProps(P_BINDING,  "%s", "unknown");
				break;
		}
	}

	/* We'll return to total_calls later */

	/* Loop through the call stats. */
	for (i = PROC_STATS_START;
	     i <= PROC_STATS_START + _NI_LOOKUPREAD;
	     i++) {
	    wPropsN(i, STATS_NCALLS, "%lu",
		    netinfod_stats[i - PROC_STATS_START].ncalls);
	    wPropsN(i, STATS_TIME, "%lu",
		    netinfod_stats[i - PROC_STATS_START].time);
	    total_calls += netinfod_stats[i - PROC_STATS_START].ncalls;
	}
	wProps(P_CALLS, "%u", total_calls);

	res.ni_proplist_len = PROC_STATS_START+(_NI_LOOKUPREAD+1);
	res.ni_proplist_val = props;

	return (&res);
}

/*
 * The NetInfo ROOT procedure
 */
ni_id_res *
_ni_root_2_svc(
	   void *arg,
	   struct svc_req *req
	   )
{
	static ni_id_res res;

	if (req == NULL) {
		return (NULL);
	}
	res.status = ni_root(db_ni, &res.ni_id_res_u.id);
	return (&res);
}

/*
 * The NetInfo SELF procedure
 */
ni_id_res *
_ni_self_2_svc(
	   ni_id *arg,
	   struct svc_req *req
	   )
{
	static ni_id_res res;

	if (req == NULL) {
		return (NULL);
	}
	res.ni_id_res_u.id = *arg;
	res.status = ni_self(db_ni, &res.ni_id_res_u.id);
	return (&res);
}

/*
 * The NetInfo PARENT procedure
 */
ni_parent_res * 
_ni_parent_2_svc(
	     ni_id *arg,
	     struct svc_req *req
	     )
{
	static ni_parent_res res;

	if (req == NULL) {
		return (NULL);
	}
	res.ni_parent_res_u.stuff.self_id = *arg;
	res.status = ni_parent(db_ni, &res.ni_parent_res_u.stuff.self_id,
			       &res.ni_parent_res_u.stuff.object_id);
	return (&res);
}

/*
 * The NetInfo CHILDREN procedure
 */
ni_children_res *
_ni_children_2_svc(
	       ni_id *arg,
	       struct svc_req *req
	       )
{
	static ni_children_res res;

	ni_idlist_free(&res.ni_children_res_u.stuff.children);
	if (req == NULL) {
		return (NULL);
	}
	res.ni_children_res_u.stuff.self_id = *arg;
	res.status = ni_children(db_ni, &res.ni_children_res_u.stuff.self_id, 
				 &res.ni_children_res_u.stuff.children);
	return (&res);
}

/*
 * The NetInfo CREATE procedure
 */
ni_create_res *
_ni_create_2_svc(
	     ni_create_args *arg,
	     struct svc_req *req
	     )
{
	static ni_create_res res;

	if (req == NULL) {
		return (NULL);
	}
	res.status = validate_write(req);
	if (res.status != NI_OK) {
		return (&res);
	}
	if (arg->target_id != NULL) {
		res.ni_create_res_u.stuff.id = *arg->target_id;
	} else {
		res.ni_create_res_u.stuff.id.nii_object = NI_INDEX_NULL;
	}
	res.ni_create_res_u.stuff.self_id = arg->id;
	res.status = ni_create(db_ni, &res.ni_create_res_u.stuff.self_id, 
			       arg->props, &res.ni_create_res_u.stuff.id,
			       arg->where);
	if (res.status == NI_OK) {
		if (!i_am_clone) {
			if (arg->target_id == NULL) {
				MM_ALLOC(arg->target_id);
				*arg->target_id = res.ni_create_res_u.stuff.id;
			}
			notify_clients(_NI_CREATE, arg);
		}
	} else if (i_am_clone) {
		dir_clonecheck();
	}
	return (&res);
}

/*
 * The NetInfo DESTROY procedure
 */
ni_id_res *
_ni_destroy_2_svc(
	      ni_destroy_args *arg,
	      struct svc_req *req
	      )
{
	static ni_id_res res;

	if (req == NULL) {
		return (NULL);
	}
	res.status = validate_write(req);
	if (res.status != NI_OK) {
		return (&res);
	}
	res.ni_id_res_u.id = arg->parent_id;
	res.status = ni_destroy(db_ni, &res.ni_id_res_u.id, arg->self_id);
	if (res.status == NI_OK) {
		if (!i_am_clone) {
			notify_clients(_NI_DESTROY, arg);
		}
	} else if (i_am_clone) {
		dir_clonecheck();
	}
	return (&res);
}

/*
 * The NetInfo READ procedure
 */
ni_proplist_res *
_ni_read_2_svc(
	   ni_id *arg,
	   struct svc_req *req
	   )
{
	static ni_proplist_res res;

	ni_proplist_free(&res.ni_proplist_res_u.stuff.props);

	if (req == NULL) {
		return (NULL);
	}
	res.ni_proplist_res_u.stuff.id = *arg;
	res.status = ni_read(db_ni, &res.ni_proplist_res_u.stuff.id, 
			     &res.ni_proplist_res_u.stuff.props);
	return (&res);
}

/*
 * The NetInfo WRITE procedure
 */
ni_id_res *
_ni_write_2_svc(
	    ni_proplist_stuff *arg,
	    struct svc_req *req
	    )
{
	static ni_id_res res;

	if (req == NULL) {
		return (NULL);
	}
	res.status = validate_write(req);
	if (res.status != NI_OK) {
		return (&res);
	}
	res.ni_id_res_u.id = arg->id;
	res.status = ni_write(db_ni, &res.ni_id_res_u.id, arg->props);
	if (res.status == NI_OK) {
		if (!i_am_clone) {
			notify_clients(_NI_WRITE, arg);
		}
	} else if (i_am_clone) {
		dir_clonecheck();
	}
	return (&res);
}

/*
 * The NetInfo LOOKUP procedure
 */
ni_lookup_res *
_ni_lookup_2_svc(
	     ni_lookup_args *arg,
	     struct svc_req *req
	     )
{
	static ni_lookup_res res;

	ni_idlist_free(&res.ni_lookup_res_u.stuff.idlist);
	if (req == NULL) {
		return (NULL);
	}
	res.ni_lookup_res_u.stuff.self_id = arg->id;
	NI_INIT(&res.ni_lookup_res_u.stuff.idlist);
	res.status = ni_lookup(db_ni, &res.ni_lookup_res_u.stuff.self_id,
			       arg->key, arg->value, 
			       &res.ni_lookup_res_u.stuff.idlist);
	return (&res);
}

/*
 * The NetInfo LOOKUPREAD procedure
 */
ni_proplist_res *
_ni_lookupread_2_svc(
		 ni_lookup_args *arg,
		 struct svc_req *req
		 )
{
	static ni_proplist_res res;

	ni_proplist_free(&res.ni_proplist_res_u.stuff.props);

	if (req == NULL) {
		return (NULL);
	}
	res.ni_proplist_res_u.stuff.id = arg->id;
	res.status = ni_lookupread(db_ni, 
				   &res.ni_proplist_res_u.stuff.id,
				   arg->key, arg->value, 
				   &res.ni_proplist_res_u.stuff.props);
	return (&res);
}

/*
 * The NetInfo LIST procedure
 */
ni_list_res *
_ni_list_2_svc(
	   ni_name_args *arg,
	   struct svc_req *req
	   )
{
	static ni_list_res res;

	if (req == NULL) {
		ni_list_const_free(db_ni);
		return (NULL);
	}
	res.ni_list_res_u.stuff.self_id = arg->id;
	res.status = ni_list_const(db_ni, &res.ni_list_res_u.stuff.self_id,
				   arg->name, 
				   &res.ni_list_res_u.stuff.entries);
	return (&res);
}

/*
 * WARNING: this function is dangerous and may be removed in future
 * implementations of the protocol.
 * While it is easier on the network, it eats up too much time on the
 * server and interrupts service for others.
 * 
 * PLEASE DO NOT CALL IT!!!
 */
ni_listall_res *
_ni_listall_2_svc(
	      ni_id *id,
	      struct svc_req *req
	      )
{
	static ni_listall_res res;

	ni_proplist_list_free(&res.ni_listall_res_u.stuff.entries);
	if (req == NULL) {
		return (NULL);
	}

	res.ni_listall_res_u.stuff.self_id = *id;
	res.status = ni_listall(db_ni, &res.ni_listall_res_u.stuff.self_id,
			     &res.ni_listall_res_u.stuff.entries);
	return (&res);
}

/*
 * The NetInfo READPROP procedure
 */
ni_namelist_res *
_ni_readprop_2_svc(
	       ni_prop_args *arg,
	       struct svc_req *req
	       )
{
	static ni_namelist_res res;

	ni_namelist_free(&res.ni_namelist_res_u.stuff.values);
	if (req == NULL) {
		return (NULL);
	}
	res.ni_namelist_res_u.stuff.self_id = arg->id;
	res.status = ni_readprop(db_ni, &res.ni_namelist_res_u.stuff.self_id,
				 arg->prop_index,
				 &res.ni_namelist_res_u.stuff.values);
	return (&res);
}

/*
 * The NetInfo WRITEPROP procedure
 */
ni_id_res *
_ni_writeprop_2_svc(
		ni_writeprop_args *arg,
		struct svc_req *req
		)
{
	static ni_id_res res;

	if (req == NULL) {
		return (NULL);
	}
	res.status = validate_write(req);
	if (res.status != NI_OK) {
		return (&res);
	}
	res.ni_id_res_u.id = arg->id;
	res.status = ni_writeprop(db_ni, &res.ni_id_res_u.id,
				  arg->prop_index,
				  arg->values);
	if (res.status == NI_OK) {
		if (!i_am_clone) {
			notify_clients(_NI_WRITEPROP, arg);
		}
	} else if (i_am_clone) {
		dir_clonecheck();
	}
	return (&res);
}

/*
 * The NetInfo LISTPROPS procedure
 */
ni_namelist_res *
_ni_listprops_2_svc(
		ni_id *arg,
		struct svc_req *req
		)
{
	static ni_namelist_res res;

	ni_namelist_free(&res.ni_namelist_res_u.stuff.values);
	if (req == NULL) {
		return (NULL);
	}
	res.ni_namelist_res_u.stuff.self_id = *arg;
	res.status = ni_listprops(db_ni, &res.ni_namelist_res_u.stuff.self_id, 
				  &res.ni_namelist_res_u.stuff.values);
	return (&res);
}
	

/*
 * The NetInfo CREATEPROP procedure
 */	
ni_id_res *
_ni_createprop_2_svc(
		 ni_createprop_args *arg,
		 struct svc_req *req
		 )
{
	static ni_id_res res;

	if (req == NULL) {
		return (NULL);
	}
	res.status = validate_write(req);
	if (res.status != NI_OK) {
		return (&res);
	}
	res.ni_id_res_u.id = arg->id;
	res.status = ni_createprop(db_ni, &res.ni_id_res_u.id, arg->prop, arg->where);
	if (res.status == NI_OK) {
		if (!i_am_clone) {
			notify_clients(_NI_CREATEPROP, arg);
		}
	} else if (i_am_clone) {
		dir_clonecheck();
	}
	return (&res);
}

/*
 * The NetInfo DESTROYPROP procedure
 */
ni_id_res *
_ni_destroyprop_2_svc(
		  ni_prop_args *arg,
		  struct svc_req *req
		  )
{
	static ni_id_res res;

	if (req == NULL) {
		return (NULL);
	}
	res.status = validate_write(req);
	if (res.status != NI_OK) {
		return (&res);
	}
	res.ni_id_res_u.id = arg->id;
	res.status = ni_destroyprop(db_ni, &res.ni_id_res_u.id, arg->prop_index);
	if (res.status == NI_OK) {
		if (!i_am_clone) {
			notify_clients(_NI_DESTROYPROP, arg);
		}
	} else if (i_am_clone) {
		dir_clonecheck();
	}
	return (&res);
}

/*
 * The NetInfo RENAMEPROP procedure
 */
ni_id_res *
_ni_renameprop_2_svc(
		 ni_propname_args *arg,
		 struct svc_req *req
		 )
{
	static ni_id_res res;

	if (req == NULL) {
		return (NULL);
	}
	res.status = validate_write(req);
	if (res.status != NI_OK) {
		return (&res);
	}
	res.ni_id_res_u.id = arg->id;
	res.status = ni_renameprop(db_ni, &res.ni_id_res_u.id, 
				   arg->prop_index, arg->name);
	if (res.status == NI_OK) {
		if (!i_am_clone) {
			notify_clients(_NI_RENAMEPROP, arg);
		}
		
	} else if (i_am_clone) {
		dir_clonecheck();
	}
	return (&res);
}

/*
 * The NetInfo CREATENAME procedure
 */
ni_id_res *
_ni_createname_2_svc(
		 ni_createname_args *arg,
		 struct svc_req *req
		 )
{
	static ni_id_res res;

	if (req == NULL) {
		return (NULL);
	}
	res.status = validate_write(req);
	if (res.status != NI_OK) {
		return (&res);
	}
	res.ni_id_res_u.id = arg->id;
	res.status = ni_createname(db_ni, &res.ni_id_res_u.id, arg->prop_index, arg->name,
				   arg->where);
	if (res.status == NI_OK) {
		if (!i_am_clone) {
			notify_clients(_NI_CREATENAME, arg);
		}
	} else if (i_am_clone) {
		dir_clonecheck();
	}
	return (&res);
}

/*
 * The NetInfo WRITENAME procedure
 */
ni_id_res *
_ni_writename_2_svc(
		ni_writename_args *arg,
		struct svc_req *req
		)
{
	static ni_id_res res;

	if (req == NULL) {
		return (NULL);
	}
	res.status = validate_write(req);
	if (res.status != NI_OK) {
		return (&res);
	}
	res.ni_id_res_u.id = arg->id;
	res.status = ni_writename(db_ni, &res.ni_id_res_u.id, 
				    arg->prop_index, arg->name_index,
				    arg->name);
	if (res.status == NI_OK) {
		if (!i_am_clone) {
			notify_clients(_NI_WRITENAME, arg);
		}
	} else if (i_am_clone) {
		dir_clonecheck();
	}
	return (&res);
}

/*
 * The NetInfo READNAME procedure 
 */
ni_readname_res *
_ni_readname_2_svc(
	       ni_nameindex_args *arg,
	       struct svc_req *req
		)
{
	static ni_readname_res res;

	ni_name_free(&res.ni_readname_res_u.stuff.name);
	if (req == NULL) {
		return (NULL);
	}
	res.ni_readname_res_u.stuff.id = arg->id;
	res.status = ni_readname(db_ni, &res.ni_readname_res_u.stuff.id, 
				 arg->prop_index, arg->name_index,
				 &res.ni_readname_res_u.stuff.name);
	return (&res);
}

/*
 * The NetInfo DESTROYNAME procedure
 */
ni_id_res *
_ni_destroyname_2_svc(
		  ni_nameindex_args *arg,
		  struct svc_req *req
		  )
{
	static ni_id_res res;

	if (req == NULL) {
		return (NULL);
	}
	res.status = validate_write(req);
	if (res.status != NI_OK) {
		return (&res);
	}
	res.ni_id_res_u.id = arg->id;
	res.status = ni_destroyname(db_ni, &res.ni_id_res_u.id, 
				    arg->prop_index, arg->name_index);
	if (res.status == NI_OK) {
		if (!i_am_clone) {
			notify_clients(_NI_DESTROYNAME, arg);
		}
	} else if (i_am_clone) {
		dir_clonecheck();
	}
	return (&res);
}

/*
 * Given a NetInfo "serves" value (contains a slash character), does
 * the given tag match the field on the right of the value?
 */
static int
tag_match(
	  ni_name slashtag,
	  ni_name tag
	  )
{
	ni_name sep;
	int len;

	sep = index(slashtag, NI_SEPARATOR);
	if (sep == NULL) {
		return (0);
	}
	if (!ni_name_match(sep + 1, tag)) {
		return (0);
	}

	/*
	 * Ignore special values "." and ".."
	 */
	len = sep - slashtag;
	if (ni_name_match_seg(NAME_DOT, slashtag, len) ||
	    ni_name_match_seg(NAME_DOTDOT, slashtag, len)) {
		return (0);
	}

	return (1);
}

/*
 * The NetInfo BIND procedure.
 *
 * Only reply if served to avoid creating unnecessary net traffic. This
 * is implemented by returned ~NULL if served, NULL otherwise (an rpcgen
 * convention).
 */
void *
_ni_bind_2_svc(
	   ni_binding *binding,
	   struct svc_req *req
	   )
{
	ni_id id;
	ni_idlist ids;
	ni_namelist nl;
	ni_index i;
	ni_name address;
	struct in_addr inaddr;
	ni_status status;

	if (req == NULL) return (NULL);

	if (db_ni == NULL)
	{
		/* Report an RPC error so the caller doesn't need to time out */
		svcerr_systemerr(req->rq_xprt);
		return (NULL);
	}

	if (ni_root(db_ni, &id) != NI_OK)
	{
		svcerr_systemerr(req->rq_xprt);
		return (NULL);
	}

	NI_INIT(&ids);
	status = ni_lookup(db_ni, &id, NAME_NAME, NAME_MACHINES, &ids);
	if (status!= NI_OK)
	{
		svcerr_systemerr(req->rq_xprt);
		return (NULL);
	}

	id.nii_object = ids.ni_idlist_val[0];
	ni_idlist_free(&ids);
	inaddr.s_addr = binding->addr;
	address = inet_ntoa(inaddr);

	NI_INIT(&ids);
	status = ni_lookup(db_ni, &id, NAME_IP_ADDRESS, address, &ids);
	if (status != NI_OK)
	{
		svcerr_systemerr(req->rq_xprt);
		return (NULL);
	}

	id.nii_object = ids.ni_idlist_val[0];
	ni_idlist_free(&ids);

	NI_INIT(&nl);
	status = ni_lookupprop(db_ni, &id, NAME_SERVES, &nl);
	if (status != NI_OK)
	{
		svcerr_systemerr(req->rq_xprt);
		return (NULL);
	}

	for (i = 0; i < nl.ni_namelist_len; i++)
	{
		if (tag_match(nl.ni_namelist_val[i], binding->tag))
		{
			ni_namelist_free(&nl);
			return ((void *)~0);
		}
	}

	ni_namelist_free(&nl);
	svcerr_systemerr(req->rq_xprt);
	return (NULL);
}

/*
 * Data structure used to hold arguments and results needed for calling
 * the BIND procedure which is broadcast and receives many replies.
 */
typedef struct ni_rparent_stuff {
	nibind_bind_args *bindings; /* arguments to BIND */
	ni_rparent_res *res;	     /* result from BIND  */
} ni_rparent_stuff;

/*
 * Catches a BIND reply
 */
static bool_t
catch(void *vstuff, struct sockaddr_in *raddr, int which)
{
	ni_rparent_stuff *stuff = (ni_rparent_stuff *)vstuff;

	ni_name_free(&stuff->res->ni_rparent_res_u.binding.tag);
	stuff->res->ni_rparent_res_u.binding.tag = ni_name_dup(stuff->bindings[which].server_tag);
	stuff->res->ni_rparent_res_u.binding.addr = raddr->sin_addr.s_addr;
	stuff->res->status = NI_OK;

	/*
	 * Wait just a moment here to decrease the number of ICMP
	 * Port Unreachable messages that flood the network.
	 */
	sleep(RPARENT_CATCH_SLEEP_SECONDS);

	return (TRUE);
}

static bool_t
getfile_catch(struct bp_getfile_res *res, struct sockaddr_in *from)
{
	union
	{
		char c[4];
		unsigned int i;
	} ci;

	if (res == NULL) return FALSE;

	ci.c[0] = res->server_address.bp_address_u.ip_addr.net;
	ci.c[1] = res->server_address.bp_address_u.ip_addr.host;
	ci.c[2] = res->server_address.bp_address_u.ip_addr.lh;
	ci.c[3] = res->server_address.bp_address_u.ip_addr.impno;

	bootparam_addr.s_addr = ci.i;
	bootparam_tag = ni_name_dup(res->server_path);

	return TRUE;
}

/*
 * Determine if this entry serves ".." (i.e. it has a serves property of
 * which one of the values looks like ../SOMETAG.
 */       
static unsigned
servesdotdot(ni_entry entry, ni_name *tag)
{
	ni_name name;
	ni_name sep;
	unsigned addr;
	ni_namelist nl;
	ni_index i;
	ni_id id;
	
	if (entry.names == NULL || forcedIsRoot) return (0);
	
	id.nii_object = entry.id;
	for (i = 0; i < entry.names->ni_namelist_len; i++)
	{
		name = entry.names->ni_namelist_val[i];
		sep = index(name, NI_SEPARATOR);
		if (sep == NULL) continue;

		if (!ni_name_match_seg(NAME_DOTDOT, name, sep - name)) continue;

		NI_INIT(&nl);
		if (ni_lookupprop(db_ni, &id, NAME_IP_ADDRESS, &nl) != NI_OK) continue;
		if (nl.ni_namelist_len == 0) continue;

		addr = inet_addr(nl.ni_namelist_val[0]);
		ni_namelist_free(&nl);
		*tag = ni_name_dup(sep + 1);
		return (addr);
	}

	return (0);
}

static void
add_binding_entry(struct in_addr sa, ni_name st, struct in_addr ca, ni_name ct,
	struct in_addr **addrs, ni_rparent_stuff *stuff, unsigned int *naddrs)
{
	char astr[32], cstr[32];
	
	sprintf(astr, "%s", inet_ntoa(sa));
	sprintf(cstr, "%s", inet_ntoa(ca));
	
	system_log(LOG_DEBUG, "binding list added   %s/%s   (%s/%s)", astr, st, cstr, ct);

	MM_GROW_ARRAY(*addrs, *naddrs);
	(*addrs)[*naddrs].s_addr = sa.s_addr;
	MM_GROW_ARRAY(stuff->bindings, *naddrs);
	stuff->bindings[*naddrs].client_tag = ni_name_dup(ct);
	/*
	 * Byte swap client address, since this gets sent out in an RPC packet,
	 * and XDR treats it as an unsigned long.
	 */
	stuff->bindings[*naddrs].client_addr = ntohl(ca.s_addr);
	stuff->bindings[*naddrs].server_tag = ni_name_dup(st);
	*naddrs = *naddrs + 1;
}

#ifdef _DHCP_BINDING_
#ifdef _IPC_UNTYPED_
static void
sys_port_free(mach_port_t p)
{
	mach_port_deallocate(mach_task_self(), p);
}
#else
static void
sys_port_free(port_t p)
{
	port_deallocate(task_self(), p);
}
#endif
#endif

static void
add_binding_from_dhcp(struct in_addr server_addr, ni_name client_tag,
	struct in_addr **addrs, ni_rparent_stuff *stuff, unsigned int *naddrs)
#ifdef _DHCP_BINDING_
{
	boolean_t active = FALSE;
	int i, j;
	interface_list_t *l;
	unsigned int len;
	struct in_addr dhcp_ni_addr[128];
	char dhcp_ni_tag[256];
	port_t server;
	kern_return_t status;

	status = ipconfig_server_port(&server, &active);
	if (status != KERN_SUCCESS) return;
	if (active == FALSE)
	{
		sys_port_free(server);
		return;   
	}

	l = sys_interfaces();
	if (l == NULL)
	{
		sys_port_free(server);
		return;   
	}

	for (i = 0; i < l->count; i++)
	{
		/* get tag, allow for null termination */
		len = sizeof(dhcp_ni_tag) - 1; 
		status = ipconfig_get_option(server, l->interface[i].name, DHCP_NETINFO_SERVER_TAG, (void *)&dhcp_ni_tag, &len);
	
		if (status != KERN_SUCCESS)
		{
			system_log(LOG_DEBUG,
				"-DHCP- ipconfig get netinfo server tag"
				" option failed for \"%s\", %s",
				l->interface[i].name, mach_error_string(status));
			continue;
		}

		dhcp_ni_tag[len] = '\0';

		/* get address(es) */
		len = sizeof(dhcp_ni_addr);
		status = ipconfig_get_option(server, l->interface[i].name, DHCP_NETINFO_SERVER_ADDRESS, (void *)&dhcp_ni_addr[0], &len);

		if (status != KERN_SUCCESS)
		{
			system_log(LOG_DEBUG,
				"-DHCP- ipconfig get netinfo server address"
				" option failed for \"%s\", %s",
				l->interface[i].name, mach_error_string(status));
			continue;
		}

		len /= 4;
		for (j = 0; j < len; j++) 
		{
			/* add a binding for each server in the array */
			add_binding_entry(dhcp_ni_addr[j], ni_name_dup(dhcp_ni_tag), l->interface[i].addr, client_tag, addrs, stuff, naddrs);
			system_log(LOG_INFO,
				"-DHCP- bind lookup returned %s/%s",
				inet_ntoa(dhcp_ni_addr[j]), dhcp_ni_tag);
		}
	}

	sys_port_free(server);
}
#else
{
}
#endif _DHCP_BINDING_

static void
add_binding_from_bootparams(struct in_addr server_addr, ni_name client_tag,
	struct in_addr **addrs, ni_rparent_stuff *stuff, unsigned int *naddrs)
{
	enum clnt_stat stat;
	struct bp_getfile_arg getfile_arg;
	struct bp_getfile_res getfile_res;
	interface_list_t *l;
	int i;

	l = sys_interfaces();
	if (l == NULL) return;

	getfile_arg.client_name = (bp_machine_name_t)sys_hostname();
	getfile_arg.file_id = BOOTPARAM_FILEID;
	memset(&getfile_res, 0, sizeof(struct bp_getfile_res));

	stat = clnt_broadcast(BOOTPARAMPROG, BOOTPARAMVERS, BOOTPARAMPROC_GETFILE,
		xdr_bp_getfile_arg, (char *)&getfile_arg,
		xdr_bp_getfile_res, (char *)&getfile_res,
		getfile_catch);

	if (stat != RPC_SUCCESS) return;

	system_log(LOG_INFO, "-BOOTPARAMS- bind lookup returned %s/%s",
		inet_ntoa(bootparam_addr), bootparam_tag);

	for (i = 0; i < l->count; i++)
	{
		if ((l->interface[i].flags & IFF_UP) == 0) continue;
		if ((l->interface[i].flags & IFF_LOOPBACK) && (l->count > 1)) continue;

		add_binding_entry(bootparam_addr, bootparam_tag, l->interface[i].addr, client_tag, addrs, stuff, naddrs);
	}
}

static void
add_broadcast_binding(ni_name server_tag, ni_name client_tag,
	struct in_addr **addrs, ni_rparent_stuff *stuff, unsigned int *naddrs)
{
	interface_list_t *l;
	int i;

	l = sys_interfaces();
	if (l == NULL) return;

	for (i = 0; i < l->count; i++)
	{
		if ((l->interface[i].flags & IFF_UP) == 0) continue;
		if (l->interface[i].flags & IFF_LOOPBACK)
		{
			if (l->count == 1)
			{
				/* Special case: include loopback if it is the only interface */
				add_binding_entry(l->interface[i].addr, server_tag, l->interface[i].addr, client_tag, addrs, stuff, naddrs);
			}
		}
		else
		{
			add_binding_entry(l->interface[i].bcast, server_tag, l->interface[i].addr, client_tag, addrs, stuff, naddrs);
		}
	}
}

static void
add_hardwired_binding(struct in_addr server_addr, ni_name server_tag, ni_name client_tag, struct in_addr **addrs, ni_rparent_stuff *stuff, unsigned int *naddrs)
{
	interface_list_t *l;
	int i;

	l = sys_interfaces();
	if (l == NULL) return;

	for (i = 0; i < l->count; i++)
	{
		if ((l->interface[i].flags & IFF_UP) == 0) continue;
		if ((l->interface[i].flags & IFF_LOOPBACK) && (l->count > 1)) continue;
		add_binding_entry(server_addr, server_tag, l->interface[i].addr, client_tag, addrs, stuff, naddrs);
	}
}

static void
freeToken(token_t *t)
{
	if (t == NULL) return;
	if (t->value != NULL) free(t->value);
	t->value = NULL;
	t->type = TokenNULL;
	free(t);
}

/*
 * XML tokens are any run enclosed by < and >.
 * White space includes spaces, tabs, and newlines.
 */
static token_t *
get_token_1(FILE *fp, int xword)
{
	token_t *t, *s;
	char c;
	static char x = EOF;
	int i, run, len, xml;

	t = (token_t *)malloc(sizeof(token_t));
	t->type = TokenNULL;
	t->value = NULL;
	len = 0;

	if (xword == 1)
	{

		run = 1;
		while (run == 1)
		{
			c = x;
			x = EOF;
			if (c == EOF) c = getc(fp);
			if (c == EOF) return t;

			if (c == '<')
			{
				x = c;
				break;
			}

			if (len == 0) t->value = malloc(1);

			t->value[len++] = c;
			t->value = realloc(t->value, len + 1);
			t->value[len] = '\0';
		}

		s = (token_t *)malloc(sizeof(token_t));
		s->type = TokenWord;
		s->value = NULL;

		len = 0;
		for (i = 0; t->value[i] != '\0'; i++)
		{
			c = t->value[i];
			if (c == '&')
			{
				if (!strncmp(t->value + i, "&amp;", 5))
				{
					c = '&';
					i += 4;
				}
				else if (!strncmp(t->value + i, "&lt;", 4))
				{
					c = '<';
					i += 3;
				}
				else if (!strncmp(t->value + i, "&gt;", 4))
				{
					c = '>';
					i += 3;
				}
			}

			if (len == 0) s->value = malloc(1);
			s->value[len++] = c;
			s->value = realloc(s->value, len + 1);
			s->value[len] = '\0';
		}

		freeToken(t);
		return s;
	}

	/* Skip white space */
	run = 1;
	while (run == 1)
	{
		c = x;
		x = EOF;
		if (c == EOF) c = getc(fp);
		if (c == EOF) return t;
		if (c == ' ') continue;
		if (c == '\t') continue;
		if (c == '\n') continue;
		run = 0;
	}

	xml = 0;
	if (c == '<') xml = 1;

	len = 1;
	t->value = malloc(len + 1);
	t->value[0] = c;
	t->value[len] = '\0';
	t->type = TokenWord;

	run = 1;
	while (run == 1)
	{
		c = getc(fp);
		if (c == EOF) return t;

		if (c == '<')
		{
			x = c;
			return t;
		}

		if ((xml == 1) && (c == '>')) run = 0;

		t->value[len++] = c;
		t->value = realloc(t->value, len + 1);
		t->value[len] = '\0';
	}

	return t;
}	

static token_t *
pop_token(FILE *fp)
{
	token_t *t, *s;
		
	t = get_token_1(fp, 0);
	if (t->type != TokenWord) return t;

	if (!strcmp(t->value, "<string>"))
	{
		freeToken(t);
		t = get_token_1(fp, 1);
		s = get_token_1(fp, 0);
		if (strcmp(s->value, "</string>"))
		{
			freeToken(t);
			freeToken(s);
			return NULL;
		}
		freeToken(s);
		return t;
	}

	if (t->value[0] == '<')
	{
		if (!strcmp(t->value, "<array>")) t->type = TokenXMLArrayStart;
		if (!strcmp(t->value, "</array>")) t->type = TokenXMLArrayEnd;
		if (!strcmp(t->value, "<dict>")) t->type = TokenXMLDictStart;
		if (!strcmp(t->value, "</dict>")) t->type = TokenXMLDictEnd;
		if (!strcmp(t->value, "<key>")) t->type = TokenXMLKeyStart;
		if (!strcmp(t->value, "</key>")) t->type = TokenXMLKeyEnd;
		return t;
	}

	return t;
}

static ni_property *
pl_createprop(ni_proplist *l, char *n)
{
	ni_property *p;
	ni_index where;

	p = (ni_property *)malloc(sizeof(ni_property));
	NI_INIT(p);
	p->nip_name = ni_name_dup(n);
	p->nip_val.ni_namelist_len = 0;
	p->nip_val.ni_namelist_val = NULL;
	ni_proplist_insert(l, *p, NI_INDEX_NULL);
	ni_prop_free(p);

	where = ni_proplist_match(*l, n, NULL);
	return &(l->nipl_val[where]);
}

static ni_proplist *
fread_plist(FILE *fp)
{
	ni_proplist *pl;
	ni_property *p;
	token_t *t;

	t = pop_token(fp);

	/* Ignore XML "headers" until we hit "<dict>" */
	while (t->type != TokenXMLDictStart)
	{
		if (t->type == TokenNULL)
		{
			freeToken(t);
			return NULL;
		}

		freeToken(t);
		t = pop_token(fp);
	}

	freeToken(t);

	pl = (ni_proplist *)malloc(sizeof(ni_proplist));
	NI_INIT(pl);

	forever
	{
		t = pop_token(fp);

		if (t->type == TokenXMLDictEnd)
		{
			freeToken(t);
			return pl;
		}

		if (t->type != TokenXMLKeyStart)
		{
			freeToken(t);
			ni_proplist_free(pl);
			return NULL;
		}
		freeToken(t);

		t = pop_token(fp);
		if (t->type != TokenWord)
		{
			freeToken(t);
			ni_proplist_free(pl);
			return NULL;
		}
		
		p = pl_createprop(pl, t->value);
		freeToken(t);

		t = pop_token(fp);
		if (t->type != TokenXMLKeyEnd)
		{
			freeToken(t);
			ni_proplist_free(pl);
			return NULL;
		}
		freeToken(t);

		t = pop_token(fp);
		if (t->type != TokenXMLArrayStart)
		{
			freeToken(t);
			ni_proplist_free(pl);
			return NULL;
		}
		freeToken(t);

		forever
		{
			t = pop_token(fp);
			if (t->type == TokenXMLArrayEnd)
			{
				freeToken(t);
				break;
			}

			if (t->type != TokenWord)
			{
				freeToken(t);
				ni_proplist_free(pl);
				return NULL;
			}

			ni_namelist_insert(&(p->nip_val), t->value, NI_INDEX_NULL);
			freeToken(t);
		}
	}
}

static void
add_parent_server(struct in_addr server_addr, ni_name server_tag, ni_name client_tag, struct in_addr **addrs, ni_rparent_stuff *stuff, unsigned int *naddrs)
{
	/*
	 * If the address is a general broadcast address (0xffffffff)
	 * we substitute the network-specific broadcast addresss for
	 * each configured non-loopback interface.
	 *
	 * If the tag is "-BOOTPARAMS-", we broadcast (on all interfaces)
	 * to get a parent server address and tag from the BOOTPARMS 
	 * service.  To support this, the bootparams server must have
	 * an entry "netinfo_parent=server_name:tag".  If we get a
	 * address and tag from BOOTPARAMS, we add it to the list.
	 * 
	 * If the tag is "-DHCP-", we ask the DHCP client for the
	 * values of the netinfo_server_address (112) and
	 * netinfo_server_tag (113) options.  If we get an address and
	 * tag from DHCP, we add it to the list.
	 *
	 * If the address is not a broadcast address, we ping the 
	 * NetInfo server with the given address and tag.  If the server
	 * is up, we cut short the search and "bind" to that server.
	 * This does not require the server to reply to a bind request,
	 * and even allows us to "bind" to a parent that doesn't know
	 * this server.  This is called "Anonymous Binding".
	 */
	if (server_addr.s_addr == 0) return;
	
	if (!strcmp(server_tag, DHCP_TAG))
	{
		/*
		 * Special case for DHCP binding when the tag is "-DHCP-".
		 * We ask DHCP for a server address and tag.
		 * Thanks to Dieter Siegmund for DHCP client support!
		 */
		add_binding_from_dhcp(server_addr, client_tag, addrs, stuff, naddrs);
	}
	else if (!strcmp(server_tag, BOOTPARAM_TAG))
	{
		/*
		 * Special case for BOOTPARM binding when the tag is "-BOOTPARAMS-".
		 * We do a bootparams broadcast for a server address and tag.
		 */
		add_binding_from_bootparams(server_addr, client_tag, addrs, stuff, naddrs);
	}
	else if (sys_is_general_broadcast(&server_addr))
	{
		/*
		 * Add network_specific broadcast for each
		 * interface (other than loopback).
		 */
		add_broadcast_binding(server_tag, client_tag, addrs, stuff, naddrs);
	}
	else
	{
		add_hardwired_binding(server_addr, server_tag, client_tag, addrs, stuff, naddrs);
	}
}

static ni_status
get_parents_from_file(void *ni, struct in_addr **addrs, ni_rparent_stuff *stuff, unsigned int *naddrs)
{
	FILE *fp;
	char str[MAXPATHLEN + 1], tag[MAXPATHLEN + 1];
	ni_name client_tag = NULL;
	ni_proplist *pl;
	ni_index where_addr, where_tag;
	struct in_addr server_addr;
	ni_name server_tag;
	int i, i_tag;

	client_tag = ni_tagname(ni);
	strcpy(tag, client_tag);
	ni_name_free(&client_tag);

	sprintf(str, CONFIG_FILE_NAME, tag);

	/* If the config file doesn't exist or is unreadable, we fail. */
	fp = fopen(str, "r");
	if (fp == NULL) return NI_FAILED;

	pl = fread_plist(fp);
	fclose(fp);

	/* No proplist == do not bind */
	if (pl == NULL) return NI_OK;

	where_addr = ni_proplist_match(*pl, KEY_PARENT_SERVER_ADDR, NULL);
	where_tag = ni_proplist_match(*pl, KEY_PARENT_SERVER_TAG, NULL);

	if ((where_addr == NI_INDEX_NULL)
		|| (where_tag == NI_INDEX_NULL)
		|| (pl->ni_proplist_val[where_addr].nip_val.ni_namelist_len == 0)
		|| (pl->ni_proplist_val[where_tag].nip_val.ni_namelist_len == 0))
	{
		ni_proplist_free(pl);
		return NI_OK;
	}


	i_tag = 0;
	for (i = 0; i < pl->ni_proplist_val[where_addr].nip_val.ni_namelist_len; i++)
	{
		server_addr.s_addr = inet_addr(pl->ni_proplist_val[where_addr].nip_val.ni_namelist_val[i]);
		server_tag = pl->ni_proplist_val[where_tag].nip_val.ni_namelist_val[i_tag];
		add_parent_server(server_addr, server_tag, tag, addrs, stuff, naddrs);
		
		i_tag++;

		/* If we run out of tags, stick on the last one */
		if (i_tag == pl->ni_proplist_val[where_tag].nip_val.ni_namelist_len) i_tag--;
	}

	ni_proplist_free(pl);
	return NI_OK;
}

static ni_status
get_parents_from_db(void *ni, struct in_addr **addrs, ni_rparent_stuff *stuff, unsigned int *naddrs)
{
	ni_name client_tag = NULL;
	ni_name server_tag = NULL;
	struct in_addr server_addr;
	ni_id id;
	ni_idlist ids;
	ni_entrylist entries;
	int i;
	ni_status status;

	status = ni_root(ni, &id);
	if (status != NI_OK) return status;

	NI_INIT(&ids);
	status = ni_lookup(ni, &id, NAME_NAME, NAME_MACHINES, &ids);
	if (status != NI_OK) return status;

	/* list "serves" properties in /machines */
	id.nii_object = ids.ni_idlist_val[0];
	ni_idlist_free(&ids);

	NI_INIT(&entries);
	status = ni_list_const(ni, &id, NAME_SERVES, &entries);
	if (status != NI_OK) return status;

	client_tag = ni_tagname(ni);

	for (i = 0; i < entries.ni_entrylist_len; i++)
	{
		/*
		 * We walk through the list of "serves" values.  If a value
		 * is "../tag", we add the host's address and the tag to a list
		 * of potential parent servers.
		 */
		server_addr.s_addr = servesdotdot(entries.ni_entrylist_val[i], &server_tag);
		add_parent_server(server_addr, server_tag, client_tag, addrs, stuff, naddrs);
		ni_name_free(&server_tag);	
	}

	ni_name_free(&client_tag);
	ni_list_const_free(ni);

	return NI_OK;
}

static ni_status
bind_to_parent(struct in_addr *addrs, ni_rparent_stuff *stuff, unsigned int naddrs, ni_rparent_res *res)
{
	int i, j, r;
	ni_name temptag;
	unsigned int nlocal;
	unsigned int nnetwork;
	unsigned long tempaddr;
	enum clnt_stat stat;
	static ni_rparent_res old_res = { NI_NOTMASTER };
	static struct in_addr old_binding;
	static struct in_addr new_binding;
	int *shuffle;

	if (naddrs == 0) return NI_NETROOT;

	/*
	 * Majka - 1994.04.27
	 * re-order the servers so that:
	 * servers on the local host are first, then
	 * servers on the local network are next, then
	 * all other servers are next
	 */

	/*
	 * move local servers to the head of the list
	 */
	nlocal = 0;
	for (i = nlocal; i < naddrs; i++)
	{
		if (sys_is_my_address(&(addrs[i])))
		{
			tempaddr = addrs[nlocal].s_addr;
			addrs[nlocal].s_addr = addrs[i].s_addr;
			addrs[i].s_addr = tempaddr;

			temptag = stuff->bindings[nlocal].server_tag;
			stuff->bindings[nlocal].server_tag = stuff->bindings[i].server_tag;
			stuff->bindings[i].server_tag = temptag;

			tempaddr = stuff->bindings[nlocal].client_addr;
			stuff->bindings[nlocal].client_addr = stuff->bindings[i].client_addr;
			stuff->bindings[i].client_addr = tempaddr;

			temptag = stuff->bindings[nlocal].client_tag;
			stuff->bindings[nlocal].client_tag = stuff->bindings[i].client_tag;
			stuff->bindings[i].client_tag = temptag;

			nlocal++;
		}
	}

	/*
	 * move servers on this network to follow local servers
	 */
	nnetwork = nlocal;
	for (i = nnetwork; i < naddrs; i++)
	{
		if (sys_is_my_network(&(addrs[i])))
		{
			tempaddr = addrs[nnetwork].s_addr;
			addrs[nnetwork].s_addr = addrs[i].s_addr;
			addrs[i].s_addr = tempaddr;

			temptag = stuff->bindings[nnetwork].server_tag;
			stuff->bindings[nnetwork].server_tag = stuff->bindings[i].server_tag;
			stuff->bindings[i].server_tag = temptag;

			tempaddr = stuff->bindings[nnetwork].client_addr;
			stuff->bindings[nnetwork].client_addr = stuff->bindings[i].client_addr;
			stuff->bindings[i].client_addr = tempaddr;

			temptag = stuff->bindings[nnetwork].client_tag;
			stuff->bindings[nnetwork].client_tag = stuff->bindings[i].client_tag;
			stuff->bindings[i].client_tag = temptag;

			nnetwork++;
		}
	}

	/*
	 * Anonymous Binding.
	 * Majka - 1999.08.25
	 * We shuffle the servers to distribute client load.
	 */
	 
	shuffle = (int *)malloc(naddrs * sizeof(int));

	for (i = 0; i < naddrs; i++) shuffle[i] = i;	
	for (i = 0; i < naddrs; i++)
	{
		r = random() % naddrs;
		j = shuffle[i];
		shuffle[i] = shuffle[r];
		shuffle[r] = j;
	}

	for (i = 0; i < naddrs; i++)
	{
		j = shuffle[i];

		if (sys_is_my_broadcast(&(addrs[j]))) continue;

		if (ni_ping(addrs[j].s_addr, stuff->bindings[j].server_tag))
		{
			ni_name_free(&(res->ni_rparent_res_u.binding.tag));
			res->ni_rparent_res_u.binding.tag = ni_name_dup(stuff->bindings[j].server_tag);
			res->ni_rparent_res_u.binding.addr = addrs[j].s_addr;

			system_log(LOG_INFO, "Anonymous binding to %s/%s",
				inet_ntoa(addrs[j]), stuff->bindings[j].server_tag);

			res->status = NI_OK;
			stat = RPC_SUCCESS;
			free(shuffle);
			goto binding_done;
		}
	}
	free(shuffle);

	/*
	 * Found the addresses and committed to multicalling now
	 */
	stuff->res = res;

	/* 
	 * Try binding to a server on this host,
	 * if that fails,
	 *		try binding to a server on this host or on an attached subnet,
	 * if that fails,
	 *		try all servers.
	 */
	stat = RPC_TIMEDOUT; /* start with stat != RPC_SUCCESS */

	if ((stat != RPC_SUCCESS) && (nlocal > 0))
	{
		for (i = 0; i < nlocal; i++)
		{
			system_log(LOG_INFO, "local bind try %d: %s/%s",
			       i + 1, inet_ntoa(addrs[i]),
			       stuff->bindings[i].server_tag);
		}

		stat = ni_multi_call(nlocal, addrs,
			NIBIND_PROG, NIBIND_VERS, NIBIND_BIND,
			xdr_nibind_bind_args, stuff->bindings, 
			sizeof(nibind_bind_args),
			xdr_void, stuff, 
			catch, -1);
	}

	if (stat != RPC_SUCCESS && nnetwork > 0)
	{
		for (i = 0; i < nnetwork; i++)
		{
			system_log(LOG_INFO, "network bind try %d: %s/%s",
			       i + 1, inet_ntoa(addrs[i]),
			       stuff->bindings[i].server_tag);
		}

		stat = ni_multi_call(nnetwork, addrs,
			NIBIND_PROG, NIBIND_VERS, NIBIND_BIND,
			xdr_nibind_bind_args, stuff->bindings, 
			sizeof(nibind_bind_args),
			xdr_void, stuff, 
			catch, -1);
	}

	if (stat != RPC_SUCCESS)
	{
		for (i = 0; i < naddrs; i++)
		{
			system_log(LOG_INFO, "world bind try %d: %s/%s",
			       i + 1, inet_ntoa(addrs[i]),
			       stuff->bindings[i].server_tag);
		}

		stat = ni_multi_call(naddrs, addrs,
			NIBIND_PROG, NIBIND_VERS, NIBIND_BIND,
			xdr_nibind_bind_args, stuff->bindings, 
			sizeof(nibind_bind_args),
			xdr_void, stuff, 
			catch, -1);
	}

binding_done:

	if (stat != RPC_SUCCESS)
	{
		alert_open("English");
		res->status = NI_NORESPONSE;
	}
	else
	{
		alert_close();
		res->status = NI_OK;
	}

	old_binding.s_addr = old_res.ni_rparent_res_u.binding.addr;
	
	if (NI_OK == res->status)
	{
		/* Successfully bound */
		new_binding.s_addr = res->ni_rparent_res_u.binding.addr;
		switch (old_res.status)
		{
			case NI_NOTMASTER:
				/* Initial binding */
				system_log(LOG_NOTICE, "bound to %s/%s",
					inet_ntoa(new_binding), res->ni_rparent_res_u.binding.tag);
				old_res.ni_rparent_res_u.binding.tag =
					(ni_name)malloc(NI_NAME_MAXLEN+1);
				break;
			case NI_NETROOT:
				/* New ".." serves property, new binding */
				system_log(LOG_NOTICE, "bound to (new parent) %s/%s",
					inet_ntoa(new_binding), res->ni_rparent_res_u.binding.tag);
				old_res.ni_rparent_res_u.binding.tag =
					(ni_name)malloc(NI_NAME_MAXLEN+1);
				break;
			default:
				/* We just rebound */
				system_log(LOG_WARNING, "rebound to %s/%s (was to %s/%s)",
					inet_ntoa(new_binding), res->ni_rparent_res_u.binding.tag,
					inet_ntoa(old_binding), old_res.ni_rparent_res_u.binding.tag);
				break;
		}

		old_res.status = res->status;
		old_res.ni_rparent_res_u.binding.addr =
			res->ni_rparent_res_u.binding.addr;
		(void)strcpy(old_res.ni_rparent_res_u.binding.tag,
			res->ni_rparent_res_u.binding.tag);

		/* keep track of latest parent for statistics */
		latestParentStatus = res->status;
		if (latestParentInfo == NULL)
			latestParentInfo = malloc(sizeof("xxx.xxx.xxx.xxx") + 1 + NI_NAME_MAXLEN + 1);
		sprintf(latestParentInfo, "%s/%s",
			inet_ntoa(new_binding), res->ni_rparent_res_u.binding.tag);
	}
	else
	{
		/* Binding failed (!) */
		switch (old_res.status)\
		{
			case NI_NOTMASTER:
				system_log(LOG_ERR, "unable to bind to parent - %s",
					clnt_sperrno(stat));
				break;
			default:
				system_log(LOG_ERR, "unable to rebind from %s/%s - %s",
					inet_ntoa(old_binding), old_res.ni_rparent_res_u.binding.tag,
					clnt_sperrno(stat));
				latestParentStatus = res->status;
				break;
		}
	}

	return res->status;
}

/*
 * The NetInfo RPARENT procedure
 */
ni_rparent_res *
_ni_rparent_2_svc(void *arg, struct svc_req *req)
{
	static ni_rparent_res res = { NI_FAILED };
	static long root_time = -1;
	static unsigned long paddr = -1;
	long now;
	struct in_addr *addrs;
	unsigned int i, naddrs;
	ni_rparent_stuff stuff;
	ni_status status;

	if (req == NULL) return NULL;

	/*
	 * If standalone (i.e. no network attached), then stop here
	 */
	if (alert_aborted())
	{
		res.status = NI_NETROOT;
		latestParentStatus = NI_NETROOT;
		return &res;
	}

	if (sys_is_standalone())
	{
		res.status = NI_NETROOT;
		latestParentStatus = NI_NETROOT;
		return &res;
	}

	/*
	 * If already have the result, return it.
	 *
	 * Note: As long as the parent NetInfo server which we were
	 *       previously bound to is still up and running there
	 *       will be no re-binding calls which might detect that
	 *       we are now the root domain.
	 */
	if (res.status == NI_OK)
	{
		if (ni_ping(paddr, res.ni_rparent_res_u.binding.tag))
		{
			/*
			 * The address is stuffed into an unsigned long, 
			 * so XDR will byte swap it.  Thus we must pass the
			 * result in host byte order.
			 */
			res.ni_rparent_res_u.binding.addr = ntohl(paddr);
			return (&res);
		}
	}
	
	/*
	 * If we were the network root before, we probably still are.
	 * Throttle back attempts to find the parent domain.
	 */
	
	if ((res.status == NI_NETROOT) && (root_time != -1))
	{
		time(&now);
		if ((now - root_time) < (NETROOT_TIMEOUT_MINS * 60)) return (&res);
	}

	addrs = NULL;
	naddrs = 0;
	stuff.bindings = NULL;

	/*
	 * If there is a binding file, get potential parent servers from it.
	 * Otherwise, look in the database.
	 */
	status = get_parents_from_file(db_ni, &addrs, &stuff, &naddrs);
	if (status != NI_OK)
		status = get_parents_from_db(db_ni, &addrs, &stuff, &naddrs);

	if (status != NI_OK)
	{
		/* Can't get any potential parents! */
		res.status = NI_NETROOT;
		latestParentStatus = NI_NETROOT;
		return &res;
	}

	system_log(LOG_DEBUG, "Rebinding");

	/*
	 * Attempt to bind.
	 */
	if (bind_to_parent(addrs, &stuff, naddrs, &res) == NI_OK)
	{
		paddr = res.ni_rparent_res_u.binding.addr;
		/* Byte-swap the result so XDR does the "right thing" */
		res.ni_rparent_res_u.binding.addr = ntohl(paddr);
	}
	else
	{
		res.status = NI_NETROOT;
		time(&root_time);
	}

	MM_FREE_ARRAY(addrs, naddrs);

	for (i = 0; i < naddrs; i++)
	{
		ni_name_free(&stuff.bindings[i].client_tag);
		ni_name_free(&stuff.bindings[i].server_tag);
	}

	MM_FREE_ARRAY(stuff.bindings, naddrs);

	return (&res);
}

/*
 * Called at startup: try to locate a parent server, allowing for user
 * to abort search if timeouts occur.
 */
void
waitforparent(void)
{
	ni_rparent_res *res;
	int i;
	unsigned int a, n;

	n = get_localbindattempts(db_ni);
	if (n == 0) return;

	a = 0;

	alert_enable(1);
	for (;;)
	{
		a++;
		system_log(LOG_DEBUG, "waitforparent attempt %d", a);
		res = _ni_rparent_2_svc(NULL, (struct svc_req *)(~0));
		if (res->status != NI_NORESPONSE)
		{
			system_log(LOG_DEBUG, "waitforparent exiting for status %d", res->status);
			break;
		}

		if (a >= n)
		{
			system_log(LOG_DEBUG, "waitforparent aborted after %d attempt%s", a, (a == 1) ? "" : "s");
			break;
		}

		for (i = 0; i < PARENT_NINTERVALS; i++)
		{
			if (alert_aborted())
			{
				system_log(LOG_DEBUG, "waitforparent aborted (alert_aborted)");
				alert_enable(0);
				return;
			}
			sleep(PARENT_SLEEPTIME);
		}
	}

	alert_enable(0);
	return;
}		   

/*
 * The NetInfo CRASHED procedure
 * If master, do nothing. If clone, check that our database is up to date.
 */
void *
_ni_crashed_2_svc(unsigned *checksum, struct svc_req *req)
{
	if (req == NULL) return NULL;

	if (i_am_clone)
	{
		if (*checksum != ni_getchecksum(db_ni))
		{
			/*
			 * If we get a crashed, it means the master's coming
			 * up.  If our database is out-of-sync, we should
			 * sync up, regardless of when we last got a new
			 * database.
			 */
			have_transferred = 0;
			dir_clonecheck();
		}
	}

	return ((void *)~0);
}

/*
 * The NetInfo READALL procedure
 *
 * XXX: doing a readall takes a long time. We should really
 * fork a thread to do this and disable writes until it is done.
 * Since there are bunches of thread-safety issues (especially
 * with the RPC libraries) we will do the master side in a
 * separate process.
 */
void proxy_term(void);
ni_readall_res *
_ni_readall_2_svc(
	      unsigned *checksum,
	      struct svc_req *req
	      )
{
	static ni_readall_res res;
	unsigned db_checksum;
	int didit;
	struct stat stat_buf;
	int i;
	static int kpid;

	if (req == NULL) return (NULL);

	/*
	 * Allow a clone to answer a readall. This means that a
	 * program can be written which does a readall but can
	 * request the information from a clone (rather than
	 * always having to go to the master).
	 */
	if (i_am_clone && !cloneReadallResponseOK)
	{
		res.status = NI_NOTMASTER;
		return (&res);
	}

	db_checksum = ni_getchecksum(db_ni);
	if (*checksum == db_checksum)
	{
		res.status = NI_OK;
		res.ni_readall_res_u.stuff.checksum = 0;
		res.ni_readall_res_u.stuff.list = NULL;
		return (&res);
	}

	/*  if notifications are pending, then fail transfer. */
	if (have_notifications_pending())
	{
		res.status = NI_MASTERBUSY;
		return (&res);
	}

	system_log(LOG_WARNING, "readall %s {%u} to %s:%hu {%u}",
		db_tag, db_checksum,
		inet_ntoa(svc_getcaller(req->rq_xprt)->sin_addr),
		ntohs(svc_getcaller(req->rq_xprt)->sin_port), *checksum);

	/*
	 * Do master side of readall in separate process.
	 * We'll use a separate process to ensure we don't run
	 * into any RPC thread-safe problems.  This will be controlled
	 * by a property in the domain's root directory, readall_proxies
	 * if > 0, it denotes the maximum number of readall subprocesses
	 * we'll have forked at once.
	 */

	syslock_lock(readall_syslock);

	sending_all++;	/* Turn off writing for duration of readall */

	if (max_readall_proxies != 0)
	{
		/* Perhaps use a proxy */
		if ((max_readall_proxies == -1) || 
			(max_readall_proxies > readall_proxies))
		{
			/* We'll fork a new process to do this readall */
			readall_proxies++;	/* Assume things work; elim races? */

			kpid = fork();
			if (kpid == 0)
			{
				/* Child */

				/* all readall proxies go in the same process group */
				if (readall_proxies == 1)
					process_group = getpid();
				if (setpgrp(0, process_group) == -1) {
					system_log(LOG_ERR, "proxy can't setpgrp - %m");
				}

				syslock_unlock(readall_syslock);

				/* Right now, if we get a SIGUSR1, we'll just set the
				 * shutdown flag, which is examined at the top of the
				 * run loop in svc_run().  We really ought to clean up
				 * and disappear when we get a SIGUSR1 as a readall
				 * proxy.
				 */
				signal(SIGUSR1, (void *)proxy_term);

				/*
				 * XXX FIX THIS! XXX
				 * sleeping does not fix race conditions!!!
				 *
				 * Possible race condition: It's possible for the
				 * readall proxy to finish sending the database before
				 * the parent process updates its list of proxy PIDs.
				 * Sleep 1 sec to decrease the chances of this.
				 */
				sleep(1);

				i_am_proxy = TRUE;

				/*
				 * Identify that we're responding to a readall in argv.
				 *
				 * N.B. This needs to be shorter than invocation string!
				 * As long as we're invoked with our full pathname as
				 * argv[0], that's fine; if we're invoked with a relative
				 * pathname, this will likely be truncated.  Note that
				 * if we've a maximum size clone address (xxx.xxx.xxx.xxx),
				 * this string is as long as it can be!
				 */
				setproctitle("netinfod %s->%s", db_tag,
					inet_ntoa(svc_getcaller(req->rq_xprt)->sin_addr));

				/*
				 * To ensure we don't inappropriately keep a connection
				 * open that our parent wants to close, let's close all
				 * the FDs that are sockets, except the one we'll use to
				 * answer the clone.
				 */
				for (i = getdtablesize() - 1; i >= 0; i--)
				{
					/* Might not have std* */
					if (i != req->rq_xprt->xp_sock)
					{
						/* Do we need to socket_lock()? We're child... */
						/*
						 * According to the BUGS section of the fstat()
						 * man page, and according to the code in the
						 * kernel [bsd/netinet/tcp_usrreq.c, the PRU_SENSE
						 * case of tcp_usrreq()], the whole fstat buffer
						 * will be zero, except for st_blksize (which
						 * will be the socket's high water mark).  So,
						 * in case this bug is ever fixed and the mode is
						 * really set properly, we check for either IFSOCK
						 * set in the mode, or the whole mode being zero
						 * (which it can't otherwise be, since ALL files
						 * have a type set in the high-order bits of the
						 * mode field, and type of 0 isn't defined).
						 */

						if (fstat(i, &stat_buf) == 0)
						{
							/* Successful; check it */
							if ((S_IFSOCK == (S_IFMT & stat_buf.st_mode)) ||
								!stat_buf.st_mode)
							close(i);
						}
						else
						{
							switch(errno)
							{
								/* Error; report it */
								case 0:			/* Whoops; no error? */
									system_log(LOG_WARNING,
										"proxy: fstat(%d) return of 0, and errno of 0");
									break;

								case EBADF:		/* Not an open fd; ignore */
									break;

								default:		/* Real error: report it */
									system_log(LOG_ERR, "proxy: fstat(%d) - %m", i);
									break;
							}
						}
					}
				}

				/*
				 * Normally, we'd surround this with a socket_lock() and
				 * socket_unlock() pair.  But, we KNOW there weren't any
				 * notifications pending in the parent, and we've no
				 * other threads.  In addition, there's a slim race
				 * condition found during testing which leaves the
				 * socket_syslock locked; with no other thread running
				 * to unlock it, we just sit forever, eating up bits
				 * of CPU time.
				 */
				didit = svc_sendreply(req->rq_xprt, readall, db_ni);
				if (!didit)
				{
					system_log(LOG_ERR, "readall %s to %s:%hu failed",
						db_tag, inet_ntoa(svc_getcaller(req->rq_xprt)->sin_addr),
						ntohs(svc_getcaller(req->rq_xprt)->sin_port));
					exit(NI_SYSTEMERR);	/* Tell parent we failed */
				}
				else
				{
					system_log(LOG_NOTICE, "readall %s to %s:%hu complete",
						db_tag, inet_ntoa(svc_getcaller(req->rq_xprt)->sin_addr),
						ntohs(svc_getcaller(req->rq_xprt)->sin_port));
				}

				exit(0);	/* Tell parent all's ok */
			}
			else if (kpid == -1)
			{
				/* Error in fork()*/
				readall_proxies--;
				syslock_unlock(readall_syslock);
				system_log(LOG_ERR, "Can't fork for readall [%p], retaining");
				/* Fall through so we do readall anyway, in this process */
			}
			else
			{
				/* Parent */
				system_log(LOG_DEBUG, "readall proxy pid %d", kpid);

				/* all readall proxies go in the same process group */
				if (readall_proxies == 1)
					process_group = kpid;
				if (setpgrp(kpid, process_group) == -1) {
					system_log(LOG_ERR, "proxy can't setpgrp - %m");
				}

				/* Retain the proxy's pid to kill it if we shutdown */
		   		add_proxy(kpid, svc_getcaller(req->rq_xprt)-> sin_addr.s_addr);

				syslock_unlock(readall_syslock);

				/*
				 * Just in case (?!), close this here; child has it open.
				 * But, it's really destruction of the RPC transport. But,
				 * we surely can't do this, because ni_prot_svc does a
				 * svc_freeargs() right after we return even return(NULL),
				 * and this will just fail, and so we'll exit(1). Maybe
				 * we need to do what svctcp_destroy() does, but that
				 * grossly violates encapsulation.
				 * XXX Do we need to do something to the LRU cache, or the
				 * FDs interesting to svc_run's select()?...
				 */

				return(NULL);	/* Just keep listening */
			}
		}
		else
		{
			/* Out of proxies */
			system_log(LOG_WARNING, "readall proxy limit (%d) reached; %s "
				"request from %s:%hu", max_readall_proxies,
				strict_proxies ? "proroguing" : "retaining",
				inet_ntoa(svc_getcaller(req->rq_xprt)->sin_addr),
				ntohs(svc_getcaller(req->rq_xprt)->sin_port));

			if (strict_proxies)
			{
				/* Strict proxies says we can't do it */
				sending_all--; /* We're not doing anything; decrement */
				syslock_unlock(readall_syslock);
				res.status = NI_MASTERBUSY;
				return (&res);
			}
			syslock_unlock(readall_syslock);
		}
	    /* If not strict proxies, retain and run in this process */
	} else {
		syslock_unlock(readall_syslock);
	}

	/*
	 * Either proxies disabled, or we're out and loose, or the fork()
	 * of the proxy failed.  Do it here, regardless.
	 */
	socket_lock();
	didit = svc_sendreply(req->rq_xprt, readall, db_ni);
	socket_unlock();
	syslock_lock(readall_syslock);
	sending_all--;	/* Let writing resume (well, once we're at 0) */
	syslock_unlock(readall_syslock);
	if (!didit)
	{
		system_log(LOG_ERR, "readall %s to %s:%hu failed", db_tag,
			inet_ntoa(svc_getcaller(req->rq_xprt)->sin_addr),
			ntohs(svc_getcaller(req->rq_xprt)->sin_port));
	}
	else
	{
		system_log(LOG_NOTICE, "readall %s to %s:%hu complete", db_tag,
			inet_ntoa(svc_getcaller(req->rq_xprt)->sin_addr),
			ntohs(svc_getcaller(req->rq_xprt)->sin_port));
	}

	return (NULL);
}

void
readall_catcher(void)
{
    /*
     * We can't just clean up the proxy's pid: remove_proxy() calls
     * [HashTable removeKey:] which calls free(), which waits on the
     * (global) malloc lock.  So, just post that we need to clean up.
     * And, make the posting as atomic as possible, so we don't ever
     * lose anything, and we avoid possible race conditions.
     */
    readall_done = TRUE;
}

void
readall_cleanup(void)
{
	int p;
	union wait wait_stat;
	unsigned int addr;

    /*
     * A readall fork finished doing its job.  Note the **ASSUMPTION**
     * that the only child processes we fork are for readalls.
     */
    syslock_lock(readall_syslock);
    while (TRUE)
	{
		p = wait4(-1, (_WAIT_TYPE_ *)&wait_stat, WNOHANG, NULL);
		switch (p)
		{
			case 0:		/* All dead children reaped */
				syslock_unlock(readall_syslock);
				return;
				break;

			case -1:	/* Error */
				syslock_unlock(readall_syslock);
				if (ECHILD != errno)
					system_log(LOG_WARNING, "problem reaping proxy: %m");
				return;
				break;

			default:	/* Someone's done */
 				/* transfer was successful */
				addr = get_proxy(p);
				if (addr == 0)
				{
					system_log(LOG_WARNING,
						"child process %d not a readall proxy", p);
				}
				else
				{
					if ((wait_stat.w_retcode == 0) && (wait_stat.w_termsig == 0))
					{
						notify_mark_clone(addr);
						remove_proxy(p);
					}
					else
					{
						system_log(LOG_ERR, "readall proxy terminated with status %u, "
							"signal %u", wait_stat.w_retcode, wait_stat.w_termsig);
					}

					if (sending_all > 0) sending_all--;
					if ((readall_proxies > 0) && (0 == --readall_proxies))
						readall_done = FALSE;	/* No more readalls can be pending */
				}
				break;
		}
	}

	syslock_unlock(readall_syslock);
	return;
}

/*
 * Allow SIGINT to turn off updates
 */
void
dblock_catcher(void)
{
    if (i_am_clone)
	{
		system_log(LOG_ERR, "SIGINT to clone ignored");
		return;
    }
	else if (i_am_proxy)
	{
		system_log(LOG_ERR, "SIGINT to readall proxy ignored");
		return;
    }

    syslock_lock(lockup_syslock);
    db_lockup = ! db_lockup;
    system_log(LOG_WARNING, "Master database is now %s", 
	   db_lockup ? "locked" : "unlocked");
    syslock_unlock(lockup_syslock);
    return;
}

/*
 * The NetInfo RESYNC procedure
 */
ni_status *
_ni_resync_2_svc(
	     void *arg,
	     struct svc_req *req
	     )
{
	static ni_status status;

	if (req == NULL) return (NULL);

	system_log(LOG_NOTICE, "got a resync from %s:%hu",
	       inet_ntoa(svc_getcaller(req->rq_xprt)->sin_addr),
	       ntohs(svc_getcaller(req->rq_xprt)->sin_port));

	cleanupwait = get_cleanupwait(db_ni);
	forcedIsRoot = get_forced_root(db_ni);
	cloneReadallResponseOK = get_clone_readall(db_ni);

	if (i_am_clone)
	{
		if (get_sanitycheck(db_ni)) sanitycheck(db_tag);
		dir_clonecheck();
	}
	else
	{
		notify_resync();
	} 

	status = NI_OK;
	return (&status);
}

/*
 * Ping the server at the given address/tag
 */
static int
ni_ping(u_long address, ni_name tag)
{
	struct sockaddr_in sin;
	struct timeval tv, total;
	enum clnt_stat stat;
	int sock;
	nibind_getregister_res res;
	CLIENT *cl;

	sin.sin_family = AF_INET;
	sin.sin_port = 0;
	sin.sin_addr.s_addr = address;
	bzero(sin.sin_zero, sizeof(sin.sin_zero));
	sock = socket_open(&sin, NIBIND_PROG, NIBIND_VERS);
	if (sock < 0) return 0;

	tv.tv_sec = PING_TIMEOUT_SEC;
	tv.tv_usec = PING_TIMEOUT_USEC;

	total.tv_sec = PING_CALL_SEC;
	total.tv_usec = PING_CALL_USEC;

	cl = clntudp_create(&sin, NIBIND_PROG, NIBIND_VERS, tv, &sock);
	if (cl == NULL)
	{
		socket_close(sock);
		return 0;
	}

	stat = clnt_call(cl, NIBIND_GETREGISTER, xdr_ni_name, &tag, xdr_nibind_getregister_res, &res, total);
	clnt_destroy(cl);
	socket_close(sock);

	if (stat != RPC_SUCCESS) return 0;
	if (res.status != NI_OK) return 0;

	/*
	 * Actually talk to parent during ni_ping
	 */
	sin.sin_port = htons(res.nibind_getregister_res_u.addrs.udp_port);
	sock = socket_open(&sin, NI_PROG, NI_VERS);
	if (sock < 0) return 0;

	cl = clntudp_create(&sin, NI_PROG, NI_VERS, tv, &sock);
	if (cl == NULL)
	{
		socket_close(sock);
		return 0;
	}

	stat = clnt_call(cl, _NI_PING, xdr_void, (void *)NULL, xdr_void, (void *)NULL, total);
	clnt_destroy(cl);
	socket_close(sock);

	return (stat == RPC_SUCCESS);
}
