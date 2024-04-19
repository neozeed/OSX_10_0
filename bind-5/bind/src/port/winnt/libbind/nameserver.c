/*
 * Copyright (C) 2000  Danny Mayer.
 * 
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 * 
 * Portions Copyright (C) 2000  Internet Software Consortium.
 * 
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS" AND INTERNET SOFTWARE CONSORTIUM
 * DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL
 * INTERNET SOFTWARE CONSORTIUM BE LIABLE FOR ANY SPECIAL, DIRECT,
 * INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING
 * FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT,
 * NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION
 * WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#include "port_before.h"

#include <arpa/inet.h>
#include <arpa/nameser.h>
#include <resolv.h>
#include <unistd.h>
#include <windows.h>
#include <bind_registry.h>

#include "port_after.h"


#define MAX_NAMELENGTH	20

static char server[MAXNS][MAX_NAMELENGTH + 1];
static int numservers;
static char host_domain_name[BUFSIZ];

 
/*
 * Fetch IP info from registry entries
 */
int
init_nameserver_list() {
	HKEY hKey;
	char *cp;
	DWORD dwType = REG_SZ;
	UINT len = BUFSIZ;
	BYTE buf[BUFSIZ];
	int i;

	/* Initialize everything */

	numservers = 0;
	for (i = 0; i < MAXNS; i++)
		memset(server[i], 0, MAX_NAMELENGTH + 1);

	memset(host_domain_name, 0, BUFSIZ);

	if (RegOpenKey(HKEY_LOCAL_MACHINE,
		       "System\\CurrentControlSet\\Services\\Tcpip\\Parameters",
		       &hKey) != ERROR_SUCCESS)
		return (-1);		/* Couldn't initialize */


	if (RegQueryValueEx(hKey, "Domain", 0, &dwType, buf,
			    &len) != ERROR_SUCCESS)
	{
		RegCloseKey(hKey);
		return (-1);		/* Couldn't initialize */
	}

	if (len < BUFSIZ)		/* Guard against buffer overflow */
		strcpy(host_domain_name, buf);
	else
		strncpy(host_domain_name, buf, BUFSIZ - 1);

	/*
	 * Reset len - it was changed in the previous call to
	 * RegQueryVaueEx()
	 */

	len = BUFSIZ;
	if (RegQueryValueEx(hKey, "NameServer", 0, &dwType, buf,
			    &len) != ERROR_SUCCESS)
	{
		RegCloseKey(hKey);
		return (-1);		/* Couldn't initialize */
	}
	RegCloseKey(hKey);

	/* Process the nameserver IP Addresses */

	cp = strtok((char *)buf, " \0");
	while (cp != NULL) {
		if (strlen(cp) <= MAX_NAMELENGTH && numservers < MAXNS) {
			strcpy(server[numservers], cp);
			numservers++;
		}
		cp = strtok(NULL, " \0");
	}
	return (numservers);
}

char *
get_nameserver_addr(int index) {
	if (index < numservers)
		return (&server[index][0]);
	else
		return (NULL);	/* no such nameserver address */
}

char *
get_host_domain() {
	return (&host_domain_name[0]);
}

int
get_nameservers(res_state statp) {
	int n;
	int nserv;

	nserv = init_nameserver_list();
	if (nserv > 0) {
		nserv = min(nserv, MAXNS);
		strcpy(statp->defdname, get_host_domain());
		for (n = 0; n < nserv; n++) {
			struct in_addr a;
			inet_aton(get_nameserver_addr(n), &a);
			statp->nsaddr_list[n].sin_addr = a;
			statp->nsaddr_list[n].sin_family = AF_INET;
			statp->nsaddr_list[n].sin_port =
				 htons(NAMESERVER_PORT);
		}
		statp->nscount = nserv;
	}
	return (nserv);
}
