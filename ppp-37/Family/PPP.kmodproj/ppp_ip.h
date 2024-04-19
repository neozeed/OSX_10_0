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


#ifndef __PPP_IP_H__
#define __PPP_IP_H__


int ppp_ip_init(int init_arg);
int ppp_ip_dispose(int term_arg);

int ppp_ip_attach(struct ifnet *ifp, u_char *data);
int ppp_ip_detach(struct ifnet *ifp);

// temporary direct call for vj compression/decompression 
int ppp_ip_processvj(struct mbuf *m, struct ifnet *ifp, u_long dl_tag);
int ppp_ip_processvj_out(struct mbuf **m0, struct ifnet *ifp, char *type);

#endif