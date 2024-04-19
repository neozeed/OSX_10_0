/*
 * Copyright (c) 2000 Apple Computer, Inc. All rights reserved.
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
/* Copyright (c) 1997-2000 Apple Computer, Inc. All Rights Reserved */
/*
 * The Darwin (Apple Public Source) license specifies the terms
 * and conditions for redistribution.
 *
 * Support for networking from Classic:
 *  AppleTalk: OT and X have separate stacks and addresses
 *  IP: OT and X have separate stacks, but share the same IP address(es)
 * This is the AppleTalk support module
 *
 * Justin Walker/Laurent Dumont, 991112
 */

#include <sys/kdebug.h>
#if KDEBUG

#define DBG_SPLT_BFCHK  DRVDBG_CODE(DBG_DRVSPLT, 0)
#define DBG_SPLT_APPND  DRVDBG_CODE(DBG_DRVSPLT, 1)
#define DBG_SPLT_MBUF   DRVDBG_CODE(DBG_DRVSPLT, 2)
#define DBG_SPLT_DUP    DRVDBG_CODE(DBG_DRVSPLT, 3)
#define DBG_SPLT_PAD    DRVDBG_CODE(DBG_DRVSPLT, 4)

#endif

#include <sys/param.h>
#include <sys/systm.h>
#include <sys/socket.h>
#include <sys/protosw.h>
#include <sys/socketvar.h>
#include <sys/fcntl.h>
#include <sys/malloc.h>
#include <sys/queue.h>
#include <sys/domain.h>
#include <sys/mbuf.h>
#include <net/route.h>
#include <net/if.h>
#include <net/if_var.h>
#include <net/if_types.h>
#include <net/if_dl.h>
#include <net/ndrv.h>
#include <net/kext_net.h>
#include <net/dlil.h>
#include <net/ethernet.h>
#include <netinet/in.h>
#include <netinet/in_var.h>
#include <net/if_arp.h>

#include "SharedIP.h"
#include "sip.h"

#include <netat/sysglue.h>
#include <netat/at_pcb.h>
#include <netat/at_var.h>
#include <machine/spl.h>

#include <sys/syslog.h>

/*
 * Setup the filter (passed in) for the requested AppleTalk values.
 *  Verifying that it's OK to do this is done by the caller once
 *  we're done.  Nothing here changes real state.
 */
int
enable_atalk(struct BlueFilter *bf, void *data, struct blueCtlBlock *ifb)
{	int retval, s;
        struct sockaddr_at atalk_sockaddr, *sap;
	struct ifaddr *ifa = NULL;
	struct ifnet *ifp = NULL;

	retval = copyin(data, &atalk_sockaddr,
			sizeof (struct sockaddr_at));
	if (retval) {
#if SIP_DEBUG_ERR
		log(LOG_WARNING, "enable_atalk: copyin for data=%x to atalk_sockaddr failed ret=%x\n",
                        data, retval);
#endif
                return (-retval);
	}
	/* Record client's address */
	bf->BF_address       = atalk_sockaddr.sat_addr.s_net;
	bf->BF_node          = atalk_sockaddr.sat_addr.s_node;
#if SIP_DEBUG
	log(LOG_WARNING, "enable_atalk: SIP_PROTO_ATALK  Flags: %x, AT address %x.%x\n",
		bf->BF_flags, atalk_sockaddr.sat_addr.s_net,
		atalk_sockaddr.sat_addr.s_node);
#endif
	/*
	 * Record X-side address, if any, for this interface
	 * NB: for this to be really correct, we need change of
	 *  address notification, so we can update this value.
	 *  Can't just hang on to the 'ifa' or sockaddr pointer,
	 *  because that might get thrown away in an update.
	 */
	ifp = sotondrvcb(ifb->ifb_so)->nd_if;
	bzero((caddr_t)&ifb->XAtalkAddr, sizeof (ifb->XAtalkAddr));
	/* Assume the first one is it, since AppleTalk only uses one */
	TAILQ_FOREACH(ifa, &ifp->if_addrhead, ifa_link)
	{	if ((sap = (struct sockaddr_at *)ifa->ifa_addr) &&
		    (sap->sat_family == AF_APPLETALK)) 
			*(&ifb->XAtalkAddr) = *sap;
	}

        /* Invoke protocol filter attachment */

	s = splnet();
        if ((retval = atalk_attach_protofltr(ifp, ifb)) != 0) {
#if SIP_DEBUG_ERR
		log(LOG_WARNING, "enable_atalk: failed, ifp=%d retval=%d\n",
			ifp, retval);
#endif
		splx(s);
		return(retval);
        }
	splx(s);
	return(BFS_ATALK);
}

/*
 * This filter function intercept incoming packets being delivered to
 * AppleTalk (either from the interface or when a packet is sent by
 * MacOS X) and decide if they need to be sent up to the BlueBox.
 * Note: right now, handles both AT and AARP
 */

static
int  atalk_infltr(caddr_t cookie,
		     struct mbuf   **m_orig,
		     char          **etherheader,
		     struct ifnet  **ifnet_ptr)

{

     register struct blueCtlBlock *ifb = (struct blueCtlBlock *)cookie;
     register struct BlueFilter *bf;
     register unsigned char *p;
     register unsigned short *s;
     struct mbuf * m0, *m = *m_orig;

    bf = &ifb->filter[BFS_ATALK];  /* find AppleTalk filter */

    MDATA_ETHER_START(m);

    s = (unsigned short *)*etherheader;
    p = (unsigned char *)*etherheader;

    /* Check SNAP header for EtherTalk packets */

#if SIP_DEBUG_FLOW
    if (!bf->BF_flags)
       	log(LOG_WARNING, "atalk_infltr: p0=%x %x, %x, ... %x, net/node %x.%x\n",
		 p[0],s[6], s[7], s[10], s[13], p[30]);
#endif

    if (bf->BF_flags & SIP_PROTO_ENABLE) {      /* Filtering Appletalk. We already know it's an AppleTalk packet */
        if (((p[0] & 0x01) == 0) && (s[10] == 0x809b)) {
            if (bf->BF_flags & SIP_PROTO_RCV_FILT) {
		    if ((s[13] == bf->BF_address &&
                     	  p[30] == bf->BF_node)) {
#if SIP_DEBUG_FLOW
                          log(LOG_WARNING, "atalk_infltr: Filter match %x.%x ! ifb=%x so=%x m=%x m_flags=%x\n",
                                 s[13], p[30], ifb, ifb->ifb_so, m, m->m_flags);
#endif
			  blue_inject(ifb, m);
			  return(EJUSTRETURN); /* packet swallowed by bbox*/
		    }
		    else  {
			  /* puts the packet back together as expected by X*/
                          MDATA_ETHER_END(m);
#if SIP_DEBUG_FLOW
			  log(LOG_WARNING, "atalk_infltr: direct packet not matched: ifb=%x m=%x %x.%x\n",
                                         ifb, m, s[13], p[30]);
#endif
			  return(0); /* filtering and no match, let MacOS X have it */
		    }
		}
#if SIP_DEBUG
            log(LOG_WARNING, "atalk_infltr: SIP_PROTO_RCV_FILT == FALSE\n");
#endif
	}
    } 
    /*
     * Either a multicast, or AARP or we don't filter on node address.
     * duplicate so both X and Classic receive the packet.
     * NOTE: when no filter is set (ie startup) Blue will receive all
     *       AppleTalk traffic. Fixes problem when Blue AT is starting
     *	 up and has no node/net hint.
     */

    m0 = m_dup(m, M_NOWAIT);
    if (m0 == NULL) {
#if SIP_DEBUG_FLOW
	log(LOG_WARNING, "atalk_infltr: m_dup failed\n");
#endif
	ifb->no_bufs1++;
	/* puts the packet back together as expected */
	MDATA_ETHER_END(m);
	return(0); /* MacOS X will still get the packet if it needs to*/
    }
#if SIP_DEBUG_FLOW
    log(LOG_WARNING, "atalk_infltr: inject for bluebox p0=%x m0=%x m0->m_flags=%x so=%x \n",
	p[0], m0,  m0->m_flags, ifb->ifb_so);
#endif
    blue_inject(ifb, m0);

    /* this is for MacOS X, DLIL will hand the mbuf to AppleTalk */
    /* puts the packet back together as expected */

    MDATA_ETHER_END(m);
    return (0);
}

static
int  atalk_outfltr(caddr_t cookie,
                   struct mbuf   **m_orig,
                   struct ifnet  **ifnet_ptr,
                   struct sockaddr **dest,
                   char *dest_linkaddr,
                   char *frame_type)

{   register struct blueCtlBlock *ifb = (struct blueCtlBlock *)cookie;
    register struct BlueFilter *bf;
    register unsigned char *p;
    register unsigned short *s;
    struct mbuf * m0, *m = *m_orig;
    int total;

    /* do what splitter used to do */
    MDATA_ETHER_START(m);
    /* the following is needed if the packet proto headers
     * are constructed using several mbufs (can happen with AppleTalk)
     */
    if (FILTER_LEN > m->m_pkthdr.len)
                 return(1);

    while ((FILTER_LEN > m->m_len) && m->m_next) {
                 total = m->m_len + (m->m_next)->m_len;
                 if ((m = m_pullup(m, min(FILTER_LEN, total))) == 0)
                         return(-1);
    }

    bf = &ifb->filter[BFS_ATALK];  /* find AppleTalk filter */

    s = (unsigned short *)m->m_data;
    p = (unsigned char *)m->m_data;

    /* Check SNAP header for EtherTalk packets */

#if SIP_DEBUG_FLOW
    if (!bf->BF_flags)
       	log(LOG_WARNING, "atalk_outfltr: p0=%x %x, %x, ... %x, net/node %x.%x\n",
                 p[0],s[6], s[7], s[10], s[13], p[30]);
#endif

    /* See if we're filtering Appletalk.
     * We already know it's an AppleTalk packet
     */
    if (bf->BF_flags & SIP_PROTO_ENABLE) {
        if (((p[0] & 0x01) == 0) && (s[10] == 0x809b)) {
            if (bf->BF_flags & SIP_PROTO_RCV_FILT) {
                    if ((s[13] == bf->BF_address &&
                          p[30] == bf->BF_node)) {
#if SIP_DEBUG_FLOW
                          log(LOG_WARNING, "atalk_outfltr: Filter match %x.%x ! p[0]=%x m=%x m_flags=%x\n",
                                s[13], p[30], p[0], m, m->m_flags);
#endif
                          MDATA_ETHER_END(m);
                          if (!my_frameout(&m, *ifnet_ptr, dest_linkaddr, frame_type))
                              blue_inject(ifb, m);
			  *m_orig = 0;
                          return(EJUSTRETURN); /* packet swallowed by bbox*/
                    }
                    else  {
                          /* puts the packet back together as expected by X*/
                          MDATA_ETHER_END(m);
#if SIP_DEBUG_FLOW
 			  log(LOG_WARNING, "atalk_outfltr: direct packet not matched: p[0]=%x m=%x %x.%x\n",
			      p[0], m, s[13], p[30]);
#endif
                          *m_orig = m;
                          return(0); /* filtering and no match, let MacOS X have it */
                    }
                }
#if SIP_DEBUG
            log(LOG_WARNING, "atalk_outfltr: SIP_PROTO_RCV_FILT == FALSE\n");
#endif
        }

	/*
	 * Either a multicast, or AARP or we don't filter on node address.
	 * duplicate so both X and Classic receive the packet.
	 */
	m0 = m_dup(m, M_NOWAIT);
	if (m0 == NULL)
	{
#if SIP_DEBUG_FLOW
		log(LOG_WARNING, "atalk_outfltr: m_dup failed\n");
#endif
		ifb->no_bufs1++;
		/* puts the packet back together as expected */
		MDATA_ETHER_END(m);
		*m_orig = m;
		/* MacOS X will still get the packet if it needs to*/
		return(0);
	}

#if SIP_DEBUG_FLOW
	log(LOG_WARNING, "atalk_outfilter: inject for bluebox p0=%x m0=%x type=%x so=%x \n",
		p[0], m0,  s[10], ifb->ifb_so);
#endif
	MDATA_ETHER_END(m0);
	if (!my_frameout(&m0, *ifnet_ptr, dest_linkaddr, frame_type))
		blue_inject(ifb, m0);

    }

    /* this is for MacOS X, DLIL will hand the mbuf to AppleTalk */
    /* puts the packet back together as expected */

    MDATA_ETHER_END(m);
    *m_orig = m;
    return (0);
}


/*
 * This function attaches a protocol filter between the AppleTalk stack
 *  and the specified interface;
 * Note: use the blueCtlBlock pointer as the "cookie" passed when handling
 *  packets.
 */
int
atalk_attach_protofltr(struct ifnet *ifp, struct blueCtlBlock *ifb)
{
    u_long at_dltag, aarp_dltag;
    int retval=0;
    struct dlil_pr_flt_str atalk_pfilter = { (caddr_t)ifb, atalk_infltr, atalk_outfltr, 0,0,0};


    /* Note: this assume the folowing here:
	- if AppleTalk X is already up and running, get it's home port and
  	  register on it.
	- if not, we register ourselves.
        - AT and AARP share the same DLIL tag (temporary)
     */

    if (ifb->atalk_proto_filter_id) /* make sure to deregister the current filter first */
		retval= atalk_stop(ifb);

    if (retval)
	return (retval);

    if ((retval = dlil_find_dltag(ifp->if_family, ifp->if_unit, PF_APPLETALK, &at_dltag)) != 0) {
        if (retval == EPROTONOSUPPORT) {
              /* nobody registered AppleTalk on this i/f yet? */
		ether_attach_at(ifp, &at_dltag, &aarp_dltag);
                retval = 0;
        }
    }   
    aarp_dltag = at_dltag;

    if (retval == 0)
   	retval= dlil_attach_protocol_filter(at_dltag, &atalk_pfilter,
		&ifb->atalk_proto_filter_id, DLIL_LAST_FILTER);
#if SIP_DEBUG
   log(LOG_WARNING, "atalk_attach_protofilter: dltag=%d filter_id=%d retval=%d\n",
	   at_dltag, ifb->atalk_proto_filter_id, retval);
#endif
   return (retval);

}

/*
 * TEMP! Disable the guts of this function; need another, non-global
 *  way of dealing with this interface's AppleTalk address.
 *
 * We use a global from X-side AppleTalk; if it's not running, flags
 *  are NULL.  It's only read, not written, so there's little chance
 *  of MP problems.  Exception: need to protect its use during this
 *  chunk of code, to avoid mid-operation surprises.
 */
int
si_send_eth_atalk(register struct mbuf **m_orig, struct blueCtlBlock *ifb)
{	struct mbuf *m1, *m;
	unsigned char *p, *p1;
	register unsigned short *s;
	register unsigned long *l;
        struct sockaddr_at *sap;

	m = *m_orig;		/* Assumption: 'm' is not changed below! */
	p = mtod(m, unsigned char *);   /* Point to destination media addr */
	s = (unsigned short *)p;


	/*
	 * flags entry is non-null if X's Appletalk is up (???)
	 * Otherwise, it's null, and we just exit.
	 */
	sap = &ifb->XAtalkAddr;
        if (sap->sat_family) {	/* Zero if not set */
	    if (p[0] & 0x01) {  /* Multicast/broadcast, send to both */
		m1 = m_dup(m, M_NOWAIT);
#if SIP_DEBUG_FLOW
		log(LOG_WARNING, "si_send: broadcast! inject m=%x m1=%x..\n", m, m1);
#endif
		if (m1) {
		   p1 = mtod(m1, unsigned char *);   /* Point to destination media addr */
                   MDATA_ETHER_END(m1);
		   m1->m_pkthdr.rcvif = sotondrvcb(ifb->ifb_so)->nd_if;
		   dlil_inject_pr_input(m1, (char *)p1, ifb->atalk_proto_filter_id);
		} else
		  ifb->no_bufs2++;
		return(0);
	    }
	    l = (unsigned long *)&s[8];
#if SIP_DEBUG_FLOW
	    log(LOG_WARNING, "si_send packet: p0=%x len=%x, m_len=%x snap? %x %x:%x, Add:%x.%x\n",
		    p[0], m->m_len, s[6], s[7], *l, s[10], s[13], p[30]);
#endif
	    if (s[6] <= ETHERMTU)
	    {       if (s[7] == 0xaaaa) /* Could be Atalk */
		     {       /* Verify SNAP header */
			     if (*l == 0x03080007 && s[10] == 0x809b)
			     {       if (s[13] == sap->sat_addr.s_net &&
					 p[30] == sap->sat_addr.s_node) {
#if SIP_DEBUG_FLOW
					     log(LOG_WARNING, "si_send: packet is for X side %x.%x m=%x\n",
						     s[13], p[30], m);
#endif
                                             MDATA_ETHER_END(m);
					     m->m_pkthdr.rcvif = sotondrvcb(ifb->ifb_so)->nd_if;
					     dlil_inject_pr_input(m, p, ifb->atalk_proto_filter_id);                               								     return(EJUSTRETURN);
			       }
			       else
				   return(0);
			     } else if (*l == 0x03000000 && s[10] == 0x80f3) {
				     /* AARP pkts aren't net-addressed */
				     m1 = m_dup(m, M_NOWAIT);
#if SIP_DEBUG_FLOW
				     log(LOG_WARNING, "si_send:  AARP m=%x m1=%x send to X\n", m, m1);
#endif
				     if (m1) {
					 p1 = mtod(m1, unsigned char *);   /* Point to destination media addr */
                                         MDATA_ETHER_END(m1);
					 m1->m_pkthdr.rcvif = sotondrvcb(ifb->ifb_so)->nd_if;
					 dlil_inject_pr_input(m1, p1, ifb->atalk_proto_filter_id);
				     } else
				       ifb->no_bufs2++;
				    return(0);
			     }
#if SIP_DEBUG_FLOW
			     log(LOG_WARNING, "si_send: not an Atalk nor AARP...\n");
#endif
			     return(0);
		     } else { /* Not for us? */
#if SIP_DEBUG_FLOW
			     log(LOG_WARNING, "si_send: not a SNAP...s[7]=%x\n", s[7]);
#endif
			     return(0);
		     }
	     } /* Fall through */
	}
	return(0);
}

int
atalk_stop(struct blueCtlBlock *ifb)
{
    int retval = 0;

    if (ifb == NULL)
	  return(0);
    if (ifb->atalk_proto_filter_id) {
#if SIP_DEBUG
	log(LOG_WARNING, "atalk_stop: deregister AppleTalk proto filter tag=%d\n",
		ifb->atalk_proto_filter_id);
#endif
        retval = dlil_detach_filter(ifb->atalk_proto_filter_id);
        if (retval == 0)
            ifb->atalk_proto_filter_id = 0; /* make sure we can't deregister twice */

     }
     return(retval);
}
