/*
 * Copyright (c) 1996, 1998, 1999 Todd C. Miller <Todd.Miller@courtesan.com>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * 4. Products derived from this software may not be called "Sudo" nor
 *    may "Sudo" appear in their names without specific prior written
 *    permission from the author.
 *
 * THIS SOFTWARE IS PROVIDED ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES,
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY
 * AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL
 * THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/*
 * Supress a warning w/ gcc on Digital UN*X.
 * The system headers should really do this....
 */
#if defined(__osf__) && !defined(__cplusplus)
struct mbuf;
struct rtentry;
#endif

#include "config.h"

#include <stdio.h>
#ifdef STDC_HEADERS
#include <stdlib.h>
#endif /* STDC_HEADERS */
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif /* HAVE_UNISTD_H */
#ifdef HAVE_STRING_H
#include <string.h>
#endif /* HAVE_STRING_H */
#ifdef HAVE_STRINGS_H
#include <strings.h>
#endif /* HAVE_STRINGS_H */
#include <netdb.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/param.h>
#include <sys/time.h>
#include <sys/ioctl.h>
#if defined(HAVE_SYS_SOCKIO_H) && !defined(SIOCGIFCONF)
#include <sys/sockio.h>
#endif
#ifdef _ISC
#include <sys/stream.h>
#include <sys/sioctl.h>
#include <sys/stropts.h>
#include <net/errno.h>
#define STRSET(cmd, param, len)	{strioctl.ic_cmd=(cmd);\
				 strioctl.ic_dp=(param);\
				 strioctl.ic_timout=0;\
				 strioctl.ic_len=(len);}
#endif /* _ISC */
#ifdef _MIPS
#include <net/soioctl.h>
#endif /* _MIPS */
#include <netinet/in.h>
#include <arpa/inet.h>
#include <net/if.h>

#include "sudo.h"
#include "interfaces.h"

#ifndef lint
static const char rcsid[] = "$Sudo: interfaces.c,v 1.59 1999/08/12 16:24:09 millert Exp $";
#endif /* lint */


#if defined(SIOCGIFCONF) && !defined(STUB_LOAD_INTERFACES)
/*
 * Allocate and fill in the interfaces global variable with the
 * machine's ip addresses and netmasks.
 */
void
load_interfaces()
{
    struct ifconf *ifconf;
    struct ifreq *ifr, ifr_tmp;
    struct sockaddr_in *sin;
    int sock, n, i;
    size_t len = sizeof(struct ifconf) + BUFSIZ;
    char *previfname = "", *ifconf_buf = NULL;
#ifdef _ISC
    struct strioctl strioctl;
#endif /* _ISC */

    sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0) {
	(void) fprintf(stderr, "%s: cannot open socket: %s\n",
	    Argv[0], strerror(errno));
	exit(1);
    }

    /*
     * Get interface configuration or return (leaving num_interfaces 0)
     */
    for (;;) {
	ifconf_buf = erealloc(ifconf_buf, len);
	ifconf = (struct ifconf *) ifconf_buf;
	ifconf->ifc_len = len - sizeof(struct ifconf);
	ifconf->ifc_buf = (caddr_t) (ifconf_buf + sizeof(struct ifconf));

	/* Networking may not be installed in kernel... */
#ifdef _ISC
	STRSET(SIOCGIFCONF, (caddr_t) ifconf, len);
	if (ioctl(sock, I_STR, (caddr_t) &strioctl) < 0) {
#else
	if (ioctl(sock, SIOCGIFCONF, (caddr_t) ifconf) < 0) {
#endif /* _ISC */
	    free(ifconf_buf);
	    (void) close(sock);
	    return;
	}

	/* Break out of loop if we have a big enough buffer. */
	if (ifconf->ifc_len + sizeof(struct ifreq) < len)
	    break;
	len += BUFSIZ;
    }

    /* Allocate space for the maximum number of interfaces that could exist. */
    n = ifconf->ifc_len / sizeof(struct ifreq);
    interfaces = (struct interface *) emalloc(sizeof(struct interface) * n);

    /* For each interface, store the ip address and netmask. */
    for (i = 0; i < ifconf->ifc_len; ) {
	/* Get a pointer to the current interface. */
	ifr = (struct ifreq *) &ifconf->ifc_buf[i];

	/* Set i to the subscript of the next interface. */
	i += sizeof(struct ifreq);
#ifdef HAVE_SA_LEN
	if (ifr->ifr_addr.sa_len > sizeof(ifr->ifr_addr))
	    i += ifr->ifr_addr.sa_len - sizeof(struct sockaddr);
#endif /* HAVE_SA_LEN */

	/* Skip duplicates and interfaces with NULL addresses. */
	sin = (struct sockaddr_in *) &ifr->ifr_addr;
	if (sin->sin_addr.s_addr == 0 ||
	    strncmp(previfname, ifr->ifr_name, sizeof(ifr->ifr_name) - 1) == 0)
	    continue;

	if (ifr->ifr_addr.sa_family != AF_INET)
		continue;

#ifdef SIOCGIFFLAGS
	memset(&ifr_tmp, 0, sizeof(ifr_tmp));
	strncpy(ifr_tmp.ifr_name, ifr->ifr_name, sizeof(ifr_tmp.ifr_name) - 1);
	if (ioctl(sock, SIOCGIFFLAGS, (caddr_t) &ifr_tmp) < 0)
#endif
	    ifr_tmp = *ifr;
	
	/* Skip interfaces marked "down" and "loopback". */
	if (!(ifr_tmp.ifr_flags & IFF_UP) || (ifr_tmp.ifr_flags & IFF_LOOPBACK))
		continue;

	sin = (struct sockaddr_in *) &ifr->ifr_addr;
	interfaces[num_interfaces].addr.s_addr = sin->sin_addr.s_addr;

	/* Stash the name of the interface we saved. */
	previfname = ifr->ifr_name;

	/* Get the netmask. */
	(void) memset(&ifr_tmp, 0, sizeof(ifr_tmp));
	strncpy(ifr_tmp.ifr_name, ifr->ifr_name, sizeof(ifr_tmp.ifr_name) - 1);
#ifdef SIOCGIFNETMASK
#ifdef _ISC
	STRSET(SIOCGIFNETMASK, (caddr_t) &ifr_tmp, sizeof(ifr_tmp));
	if (ioctl(sock, I_STR, (caddr_t) &strioctl) == 0) {
#else
	if (ioctl(sock, SIOCGIFNETMASK, (caddr_t) &ifr_tmp) == 0) {
#endif /* _ISC */
	    sin = (struct sockaddr_in *) &ifr_tmp.ifr_addr;

	    interfaces[num_interfaces].netmask.s_addr = sin->sin_addr.s_addr;
	} else {
#else
	{
#endif /* SIOCGIFNETMASK */
	    if (IN_CLASSC(interfaces[num_interfaces].addr.s_addr))
		interfaces[num_interfaces].netmask.s_addr = htonl(IN_CLASSC_NET);
	    else if (IN_CLASSB(interfaces[num_interfaces].addr.s_addr))
		interfaces[num_interfaces].netmask.s_addr = htonl(IN_CLASSB_NET);
	    else
		interfaces[num_interfaces].netmask.s_addr = htonl(IN_CLASSA_NET);
	}

	/* Only now can we be sure it was a good/interesting interface. */
	num_interfaces++;
    }

    /* If the expected size < real size, realloc the array. */
    if (n != num_interfaces) {
	if (num_interfaces != 0)
	    interfaces = (struct interface *) erealloc(interfaces,
		sizeof(struct interface) * num_interfaces);
	else
	    free(interfaces);
    }
    free(ifconf_buf);
    (void) close(sock);
}

#else /* !SIOCGIFCONF || STUB_LOAD_INTERFACES */

/*
 * Stub function for those without SIOCGIFCONF
 */
void
load_interfaces()
{
    return;
}

#endif /* SIOCGIFCONF && !STUB_LOAD_INTERFACES */
