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
/*
 * fsm.c - {Link, IP} Control Protocol Finite State Machine.
 *
 * Copyright (c) 1989 Carnegie Mellon University.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms are permitted
 * provided that the above copyright notice and this paragraph are
 * duplicated in all such forms and that any documentation,
 * advertising materials, and other materials related to such
 * distribution and use acknowledge that the software was developed
 * by Carnegie Mellon University.  The name of the
 * University may not be used to endorse or promote products derived
 * from this software without specific prior written permission.
 * THIS SOFTWARE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
 * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 */


/*
 * TODO:
 * Randomize fsm id on link/init.
 * Deal with variable outgoing MTU.
 */

#include <string.h>
#include <stdio.h>
#include <termios.h>
#include <fcntl.h>
#include <sys/errno.h>
#include <sys/signal.h>
#include <sys/param.h>
#include <sys/socket.h>
#include <net/if.h>
#include <CoreFoundation/CoreFoundation.h>

#include "ppp_msg.h"
#include "../Family/PPP.kmodproj/ppp.h"
#include "../Family/PPP.kmodproj/ppp_domain.h"

#include "fsm.h"
#include "lcp.h"
#include "ipcp.h"
#include "chap.h"
#include "upap.h"
#include "ppp_client.h"
#include "ppp_utils.h"
#include "ppp_manager.h"



static void fsm_timeout __P((CFRunLoopTimerRef,void *));
static void fsm_rconfreq __P((fsm *, int, u_char *, int));
static void fsm_rconfack __P((fsm *, int, u_char *, int));
static void fsm_rconfnakrej __P((fsm *, int, int, u_char *, int));
static void fsm_rtermreq __P((fsm *, int, u_char *, int));
static void fsm_rtermack __P((fsm *));
static void fsm_rcoderej __P((fsm *, u_char *, int));
static void fsm_sconfreq __P((fsm *, int));

#define PROTO_NAME(f)	((f)->callbacks->proto_name)



/*
 * fsm_init - Initialize fsm.
 *
 * Initialize fsm state.
 */
void
fsm_init(f)
    fsm *f;
{
    f->state = INITIAL;
    f->flags = 0;
    f->id = 0;				/* XXX Start with random id? */
    f->timeouttime = DEFTIMEOUT;
    f->maxconfreqtransmits = DEFMAXCONFREQS;
    f->maxtermtransmits = DEFMAXTERMREQS;
    f->maxnakloops = DEFMAXNAKLOOPS;
    f->term_reason_len = 0;
    f->TORef = 0;
}


/*
 * fsm_lowerup - The lower layer is up.
 */
void
fsm_lowerup(f)
    fsm *f;
{
    switch( f->state ){
    case INITIAL:
	f->state = CLOSED;
	break;

    case STARTING:
	if( f->flags & OPT_SILENT )
	    f->state = STOPPED;
	else {
	    /* Send an initial configure-request */
	    fsm_sconfreq(f, 0);
	    f->state = REQSENT;
	}
	break;

    default:
	FSMDEBUG(("%s: Up event in state %d!", PROTO_NAME(f), f->state));
    }
}


/*
 * fsm_lowerdown - The lower layer is down.
 *
 * Cancel all timeouts and inform upper layers.
 */
void
fsm_lowerdown(f)
    fsm *f;
{
    switch( f->state ){
    case CLOSED:
	f->state = INITIAL;
	break;

    case STOPPED:
	f->state = STARTING;
	if( f->callbacks->starting )
	    (*f->callbacks->starting)(f);
	break;

    case CLOSING:
	f->state = INITIAL;
        DelTimerFromRunLoop(&(f->TORef)); /* Cancel timeout */
	break;

    case STOPPING:
    case REQSENT:
    case ACKRCVD:
    case ACKSENT:
	f->state = STARTING;
        DelTimerFromRunLoop(&(f->TORef)); /* Cancel timeout */
	break;

    case OPENED:
	if( f->callbacks->down )
	    (*f->callbacks->down)(f);
	f->state = STARTING;
	break;

    default:
	FSMDEBUG(("%s: Down event in state %d!", PROTO_NAME(f), f->state));
    }
}


/*
 * fsm_open - Link is allowed to come up.
 */
void
fsm_open(f)
    fsm *f;
{

    switch( f->state ){
    case INITIAL:
	f->state = STARTING;
	if( f->callbacks->starting )
	    (*f->callbacks->starting)(f);
	break;

    case CLOSED:
	if( f->flags & OPT_SILENT )
	    f->state = STOPPED;
	else {
	    /* Send an initial configure-request */
	    fsm_sconfreq(f, 0);
	    f->state = REQSENT;
	}
	break;

    case CLOSING:
	f->state = STOPPING;
	/* fall through */
    case STOPPED:
    case OPENED:
	if( f->flags & OPT_RESTART ){
	    fsm_lowerdown(f);
	    fsm_lowerup(f);
	}
	break;
    }
}


/*
 * fsm_close - Start closing connection.
 *
 * Cancel timeouts and either initiate close or possibly go directly to
 * the CLOSED state.
 */
void
fsm_close(f, reason)
    fsm *f;
    char *reason;
{
    f->term_reason = reason;
    f->term_reason_len = (reason == NULL? 0: strlen(reason));
    switch( f->state ){
    case STARTING:
	f->state = INITIAL;
	break;
    case STOPPED:
	f->state = CLOSED;
	break;
    case STOPPING:
	f->state = CLOSING;
	break;

    case REQSENT:
    case ACKRCVD:
    case ACKSENT:
    case OPENED:
	if( f->state != OPENED )
            DelTimerFromRunLoop(&(f->TORef)); /* Cancel timeout */
	else if( f->callbacks->down )
	    (*f->callbacks->down)(f);	/* Inform upper layers we're down */

	/* Init restart counter, send Terminate-Request */
	f->retransmits = f->maxtermtransmits;
	fsm_sdata(f, TERMREQ, f->reqid = ++f->id,
		  (u_char *) f->term_reason, f->term_reason_len);
	f->TORef = AddTimerToRunLoop(fsm_timeout, f, f->timeouttime);
	--f->retransmits;

	f->state = CLOSING;
	break;
    }
}


/*
 * fsm_timeout - Timeout expired.
 */
static void
fsm_timeout(CFRunLoopTimerRef timer, void *arg)
{
    fsm *f = (fsm *) arg;

    switch (f->state) {
    case CLOSING:
    case STOPPING:
	if( f->retransmits <= 0 ){
	    /*
	     * We've waited for an ack long enough.  Peer probably heard us.
	     */
	    f->state = (f->state == CLOSING)? CLOSED: STOPPED;
	    if( f->callbacks->finished )
		(*f->callbacks->finished)(f);
	} else {
	    /* Send Terminate-Request */
	    fsm_sdata(f, TERMREQ, f->reqid = ++f->id,
		      (u_char *) f->term_reason, f->term_reason_len);
            f->TORef = AddTimerToRunLoop(fsm_timeout, f, f->timeouttime);
	    --f->retransmits;
	}
	break;

    case REQSENT:
    case ACKRCVD:
    case ACKSENT:
	if (f->retransmits <= 0) {
            warn(f->ppp, "%s: timeout sending Config-Requests\n", PROTO_NAME(f));
	    f->state = STOPPED;
	    if( (f->flags & OPT_PASSIVE) == 0 && f->callbacks->finished )
		(*f->callbacks->finished)(f);

	} else {
	    /* Retransmit the configure-request */
	    if (f->callbacks->retransmit)
		(*f->callbacks->retransmit)(f);
	    fsm_sconfreq(f, 1);		/* Re-send Configure-Request */
	    if( f->state == ACKRCVD )
		f->state = REQSENT;
	}
	break;

    default:
	FSMDEBUG(("%s: Timeout event in state %d!", PROTO_NAME(f), f->state));
    }
}


/*
 * fsm_input - Input packet.
 */
void
fsm_input(f, inpacket, l)
    fsm *f;
    u_char *inpacket;
    int l;
{
    u_char *inp;
    u_char code, id;
    int len;

    /*
     * Parse header (code, id and length).
     * If packet too short, drop it.
     */
    inp = inpacket;
    if (l < HEADERLEN) {
	FSMDEBUG(("fsm_input(%x): Rcvd short header.", f->protocol));
	return;
    }
    GETCHAR(code, inp);
    GETCHAR(id, inp);
    GETSHORT(len, inp);
    if (len < HEADERLEN) {
	FSMDEBUG(("fsm_input(%x): Rcvd illegal length.", f->protocol));
	return;
    }
    if (len > l) {
	FSMDEBUG(("fsm_input(%x): Rcvd short packet.", f->protocol));
	return;
    }
    len -= HEADERLEN;		/* subtract header length */

    if( f->state == INITIAL || f->state == STARTING ){
	FSMDEBUG(("fsm_input(%x): Rcvd packet in state %d.",
		  f->protocol, f->state));
	return;
    }

    /*
     * Action depends on code.
     */
    switch (code) {
    case CONFREQ:
	fsm_rconfreq(f, id, inp, len);
	break;
    
    case CONFACK:
	fsm_rconfack(f, id, inp, len);
	break;
    
    case CONFNAK:
    case CONFREJ:
	fsm_rconfnakrej(f, code, id, inp, len);
	break;
    
    case TERMREQ:
	fsm_rtermreq(f, id, inp, len);
	break;
    
    case TERMACK:
	fsm_rtermack(f);
	break;
    
    case CODEREJ:
	fsm_rcoderej(f, inp, len);
	break;
    
    default:
	if( !f->callbacks->extcode
	   || !(*f->callbacks->extcode)(f, code, id, inp, len) )
	    fsm_sdata(f, CODEREJ, ++f->id, inpacket, len + HEADERLEN);
	break;
    }
}


/*
 * fsm_rconfreq - Receive Configure-Request.
 */
static void
fsm_rconfreq(f, id, inp, len)
    fsm *f;
    u_char id;
    u_char *inp;
    int len;
{
    int code, reject_if_disagree;

    switch( f->state ){
    case CLOSED:
	/* Go away, we're closed */
	fsm_sdata(f, TERMACK, id, NULL, 0);
	return;
    case CLOSING:
    case STOPPING:
	return;

    case OPENED:
	/* Go down and restart negotiation */
	if( f->callbacks->down )
	    (*f->callbacks->down)(f);	/* Inform upper layers */
	fsm_sconfreq(f, 0);		/* Send initial Configure-Request */
	break;

    case STOPPED:
	/* Negotiation started by our peer */
	fsm_sconfreq(f, 0);		/* Send initial Configure-Request */
	f->state = REQSENT;
	break;
    }

    /*
     * Pass the requested configuration options
     * to protocol-specific code for checking.
     */
    if (f->callbacks->reqci){		/* Check CI */
	reject_if_disagree = (f->nakloops >= f->maxnakloops);
	code = (*f->callbacks->reqci)(f, inp, &len, reject_if_disagree);
    } else if (len)
	code = CONFREJ;			/* Reject all CI */
    else
	code = CONFACK;

    /* send the Ack, Nak or Rej to the peer */
    fsm_sdata(f, code, id, inp, len);

    if (code == CONFACK) {
	if (f->state == ACKRCVD) {
            DelTimerFromRunLoop(&(f->TORef)); /* Cancel timeout */
	    f->state = OPENED;
	    if (f->callbacks->up)
		(*f->callbacks->up)(f);	/* Inform upper layers */
	} else
	    f->state = ACKSENT;
	f->nakloops = 0;

    } else {
	/* we sent CONFACK or CONFREJ */
	if (f->state != ACKRCVD)
	    f->state = REQSENT;
	if( code == CONFNAK )
	    ++f->nakloops;
    }
}


/*
 * fsm_rconfack - Receive Configure-Ack.
 */
static void
fsm_rconfack(f, id, inp, len)
    fsm *f;
    int id;
    u_char *inp;
    int len;
{
    if (id != f->reqid || f->seen_ack)		/* Expected id? */
	return;					/* Nope, toss... */
    if( !(f->callbacks->ackci? (*f->callbacks->ackci)(f, inp, len):
	  (len == 0)) ){
	/* Ack is bad - ignore it */
	//error(ppp, "Received bad configure-ack: %P", inp, len);
	return;
    }
    f->seen_ack = 1;

    switch (f->state) {
    case CLOSED:
    case STOPPED:
	fsm_sdata(f, TERMACK, id, NULL, 0);
	break;

    case REQSENT:
	f->state = ACKRCVD;
	f->retransmits = f->maxconfreqtransmits;
	break;

    case ACKRCVD:
	/* Huh? an extra valid Ack? oh well... */
        DelTimerFromRunLoop(&(f->TORef)); /* Cancel timeout */
	fsm_sconfreq(f, 0);
	f->state = REQSENT;
	break;

    case ACKSENT:
        DelTimerFromRunLoop(&(f->TORef)); /* Cancel timeout */
	f->state = OPENED;
	f->retransmits = f->maxconfreqtransmits;
	if (f->callbacks->up)
	    (*f->callbacks->up)(f);	/* Inform upper layers */
	break;

    case OPENED:
	/* Go down and restart negotiation */
	if (f->callbacks->down)
	    (*f->callbacks->down)(f);	/* Inform upper layers */
	fsm_sconfreq(f, 0);		/* Send initial Configure-Request */
	f->state = REQSENT;
	break;
    }
}


/*
 * fsm_rconfnakrej - Receive Configure-Nak or Configure-Reject.
 */
static void
fsm_rconfnakrej(f, code, id, inp, len)
    fsm *f;
    int code, id;
    u_char *inp;
    int len;
{
    int (*proc) __P((fsm *, u_char *, int));
    int ret;

    if (id != f->reqid || f->seen_ack)	/* Expected id? */
	return;				/* Nope, toss... */
    proc = (code == CONFNAK)? f->callbacks->nakci: f->callbacks->rejci;
    if (!proc || !(ret = proc(f, inp, len))) {
	/* Nak/reject is bad - ignore it */
	//error(ppp, "Received bad configure-nak/rej: %P", inp, len);
	return;
    }
    f->seen_ack = 1;

    switch (f->state) {
    case CLOSED:
    case STOPPED:
	fsm_sdata(f, TERMACK, id, NULL, 0);
	break;

    case REQSENT:
    case ACKSENT:
	/* They didn't agree to what we wanted - try another request */
        DelTimerFromRunLoop(&(f->TORef)); /* Cancel timeout */
	if (ret < 0)
	    f->state = STOPPED;		/* kludge for stopping CCP */
	else
	    fsm_sconfreq(f, 0);		/* Send Configure-Request */
	break;

    case ACKRCVD:
	/* Got a Nak/reject when we had already had an Ack?? oh well... */
        DelTimerFromRunLoop(&(f->TORef)); /* Cancel timeout */
	fsm_sconfreq(f, 0);
	f->state = REQSENT;
	break;

    case OPENED:
	/* Go down and restart negotiation */
	if (f->callbacks->down)
	    (*f->callbacks->down)(f);	/* Inform upper layers */
	fsm_sconfreq(f, 0);		/* Send initial Configure-Request */
	f->state = REQSENT;
	break;
    }
}


/*
 * fsm_rtermreq - Receive Terminate-Req.
 */
static void
fsm_rtermreq(f, id, p, len)
    fsm *f;
    int id;
    u_char *p;
    int len;
{
    switch (f->state) {
    case ACKRCVD:
    case ACKSENT:
	f->state = REQSENT;		/* Start over but keep trying */
	break;

    case OPENED:
	if (len > 0) {
            ;//info("%s terminated by peer (%0.*v)", PROTO_NAME(f), len, p);
	} else
            ;//info("%s terminated by peer", PROTO_NAME(f));
	if (f->callbacks->down)
	    (*f->callbacks->down)(f);	/* Inform upper layers */
	f->retransmits = 0;
	f->state = STOPPING;
        f->TORef = AddTimerToRunLoop(fsm_timeout, f, f->timeouttime);
	break;
    }

    fsm_sdata(f, TERMACK, id, NULL, 0);
}


/*
 * fsm_rtermack - Receive Terminate-Ack.
 */
static void
fsm_rtermack(f)
    fsm *f;
{
    switch (f->state) {
    case CLOSING:
        DelTimerFromRunLoop(&(f->TORef)); /* Cancel timeout */
	f->state = CLOSED;
	if( f->callbacks->finished )
	    (*f->callbacks->finished)(f);
	break;
    case STOPPING:
        DelTimerFromRunLoop(&(f->TORef)); /* Cancel timeout */
	f->state = STOPPED;
	if( f->callbacks->finished )
	    (*f->callbacks->finished)(f);
	break;

    case ACKRCVD:
	f->state = REQSENT;
	break;

    case OPENED:
	if (f->callbacks->down)
	    (*f->callbacks->down)(f);	/* Inform upper layers */
	fsm_sconfreq(f, 0);
	break;
    }
}


/*
 * fsm_rcoderej - Receive an Code-Reject.
 */
static void
fsm_rcoderej(f, inp, len)
    fsm *f;
    u_char *inp;
    int len;
{
    u_char code, id;

    if (len < HEADERLEN) {
	FSMDEBUG(("fsm_rcoderej: Rcvd short Code-Reject packet!"));
	return;
    }
    GETCHAR(code, inp);
    GETCHAR(id, inp);
    warn(f->ppp, "%s: Rcvd Code-Reject for code %d, id %d", PROTO_NAME(f), code, id);

    if( f->state == ACKRCVD )
	f->state = REQSENT;
}


/*
 * fsm_protreject - Peer doesn't speak this protocol.
 *
 * Treat this as a catastrophic error (RXJ-).
 */
void
fsm_protreject(f)
    fsm *f;
{
    switch( f->state ){
    case CLOSING:
        DelTimerFromRunLoop(&(f->TORef)); /* Cancel timeout */
	/* fall through */
    case CLOSED:
	f->state = CLOSED;
	if( f->callbacks->finished )
	    (*f->callbacks->finished)(f);
	break;

    case STOPPING:
    case REQSENT:
    case ACKRCVD:
    case ACKSENT:
        DelTimerFromRunLoop(&(f->TORef)); /* Cancel timeout */
	/* fall through */
    case STOPPED:
	f->state = STOPPED;
	if( f->callbacks->finished )
	    (*f->callbacks->finished)(f);
	break;

    case OPENED:
	if( f->callbacks->down )
	    (*f->callbacks->down)(f);

	/* Init restart counter, send Terminate-Request */
	f->retransmits = f->maxtermtransmits;
	fsm_sdata(f, TERMREQ, f->reqid = ++f->id,
		  (u_char *) f->term_reason, f->term_reason_len);
        f->TORef = AddTimerToRunLoop(fsm_timeout, f, f->timeouttime);
	--f->retransmits;

	f->state = STOPPING;
	break;

    default:
	FSMDEBUG(("%s: Protocol-reject event in state %d!",
		  PROTO_NAME(f), f->state));
    }
}


/*
 * fsm_sconfreq - Send a Configure-Request.
 */
static void
fsm_sconfreq(f, retransmit)
    fsm *f;
    int retransmit;
{
    u_char *outp;
    int cilen;

   if( f->state != REQSENT && f->state != ACKRCVD && f->state != ACKSENT ){
	/* Not currently negotiating - reset options */
	if( f->callbacks->resetci )
	    (*f->callbacks->resetci)(f);
	f->nakloops = 0;
    }

   if( !retransmit ){
	/* New request - reset retransmission counter, use new ID */
	f->retransmits = f->maxconfreqtransmits;
	f->reqid = ++f->id;
    }

    f->seen_ack = 0;

    /*
     * Make up the request packet
     */
    outp = f->ppp->outpacket_buf + PPP_HDRLEN + HEADERLEN;
    if( f->callbacks->cilen && f->callbacks->addci ){
	cilen = (*f->callbacks->cilen)(f);
        if( cilen > f->ppp->lcp_peer_mru - HEADERLEN )
            cilen = f->ppp->lcp_peer_mru - HEADERLEN;
	if (f->callbacks->addci)
	    (*f->callbacks->addci)(f, outp, &cilen);
    } else
	cilen = 0;

    /* send the request to our peer */
    fsm_sdata(f, CONFREQ, f->reqid, outp, cilen);

   /* start the retransmit timer */
    --f->retransmits;
    f->TORef = AddTimerToRunLoop(fsm_timeout, f, f->timeouttime);
}


/*
 * fsm_sdata - Send some data.
 *
 * Used for all packets sent to our peer by this module.
 */
void
fsm_sdata(f, code, id, data, datalen)
    fsm *f;
    u_char code, id;
    u_char *data;
    int datalen;
{
    u_char *outp;
    int outlen;

    /* Adjust length to be smaller than MTU */
    outp = f->ppp->outpacket_buf;
    if (datalen > f->ppp->lcp_peer_mru - HEADERLEN)
        datalen = f->ppp->lcp_peer_mru - HEADERLEN;
    if (datalen && data != outp + PPP_HDRLEN + HEADERLEN)
	BCOPY(data, outp + PPP_HDRLEN + HEADERLEN, datalen);
    outlen = datalen + HEADERLEN;
    MAKEHEADER(outp, f->protocol);
    PUTCHAR(code, outp);
    PUTCHAR(id, outp);
    PUTSHORT(outlen, outp);
    ppp_output(f->ppp, f->ppp->outpacket_buf, outlen + PPP_HDRLEN);
}
