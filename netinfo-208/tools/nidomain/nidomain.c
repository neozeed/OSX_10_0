/*
 * nidomain - administer nibindd
 * Copyright 1989-94, NeXT Computer Inc.
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netinfo/ni.h>
#include <netdb.h>
#include "nibind_glue.h"
#include <NetInfo/system.h>

/*
 * Translate a string into an IP address. The string
 * is either a number or a name.
 */
struct in_addr getaddr(char *host)
{
	struct hostent *h;
	struct in_addr addr;

	addr.s_addr = inet_addr(host);
	if (addr.s_addr != -1)  return (addr);

	h = gethostbyname(host);
	if (h == NULL) addr.s_addr = -1;
	else bcopy(h->h_addr, &addr, sizeof(addr));

	return (addr);
}

void usage(char *name)
{
	fprintf(stderr, 
		"usage: %s -l [host]                # list tags served\n"
		"       %s -g tag [host]            # lookup tag\n"
		"       %s -m tag [host]            # create master server\n"
		"       %s -d tag [host]            # destroy server\n"
		"       %s -u tag [host]            # unregister server\n"
		"       %s -c tag master/tag [host] # create clone of master server\n"
		"       %s -a tag alias [host]      # register existing tag under an alias\n",
		name, name, name, name, name, name, name);
	exit(1);
}

typedef enum nibind_opt {
	NB_USAGE,
	NB_LIST,
	NB_GET,
	NB_CREATEMASTER,
	NB_CREATECLONE,
	NB_UNREG,
	NB_DESTROY,
	NB_ALIAS
} nibind_opt;

int main(int argc, char *argv[])
{
	char *myname;
	char *tag = "";
	char *alias = "";
	char *host;
	char *master = "";
	char *master_name;
	char *master_tag;
	nibind_opt opt;
	ni_index i;
	void *nb;
	struct in_addr addr;
	ni_status status = NI_FAILED;
	nibind_registration *reg;
	nibind_addrinfo *addrs;
	unsigned nreg;

	myname = argv[0];
	opt = NB_USAGE;

	argc--;
	argv++;
	opt = NB_USAGE;
	host = NULL;

	while (argc > 0 && **argv == '-')
	{
		if (strcmp(*argv, "-l") == 0)
		{
			argc--;
			argv++;
			opt = NB_LIST;
			break;
		}
		else if (strcmp(*argv, "-g") == 0)
		{
			if (argc < 2) break;

			tag = argv[1];
			argc -= 2;
			argv += 2;
			opt = NB_GET;
			break;
		}
		else if (strcmp(*argv, "-u") == 0)
		{
			if (argc < 2) break;

			tag = argv[1];
			argc -= 2;
			argv += 2;
			opt = NB_UNREG;
			break;
		}
		else if (strcmp(*argv, "-m") == 0)
		{
			if (argc < 2) break;

			tag = argv[1];
			argc -= 2;
			argv += 2;
			opt = NB_CREATEMASTER;
			break;
		}
		else if (strcmp(*argv, "-d") == 0)
		{
			if (argc < 2) break;

			tag = argv[1];
			argc -= 2;
			argv += 2;
			opt = NB_DESTROY;
			break;
		}
		else if (strcmp(*argv, "-c") == 0)
		{
			if (argc < 3) break;

			tag = argv[1];
			master = argv[2];
			argc -= 3;
			argv += 3;
			opt = NB_CREATECLONE;
			break;
		}
		else if (strcmp(*argv, "-a") == 0)
		{
			if (argc < 3) break;

			tag = argv[1];
			alias = argv[2];
			argc -= 3;
			argv += 3;
			opt = NB_ALIAS;
			break;
		}
		else break;
	}

	if (argc > 1 || opt == NB_USAGE) usage(myname);

	if (argc == 1) host = argv[0];

	if (host != NULL)
	{
		addr = getaddr(host);
		if (addr.s_addr == -1)
		{
			fprintf(stderr, "%s: unknown host - %s\n", myname, host);
			exit(1);
		}
	}
	else addr.s_addr = htonl(INADDR_LOOPBACK);

	nb = nibind_new(&addr);
	if (nb == NULL)
	{
		fprintf(stderr, "%s: cannot connect to server %s\n", myname,
			host == NULL ? "localhost" : host);
		exit(1);
	}

	switch (opt)
	{
		case NB_CREATEMASTER:
			status = nibind_createmaster(nb, tag);
			break;

		case NB_CREATECLONE:
			master_tag = index(master, '/');
			if (tag == NULL)
			{
				fprintf(stderr, "%s: missing '/' in %s\n",
					myname, master);
				exit(1);
			}

			*master_tag++ = 0;
			master_name = master;
			addr = getaddr(master_name);
			if (addr.s_addr == -1)
			{
				fprintf(stderr, "%s: unknown host - %s\n",
					myname, master_name);
				exit(1);
			}

			status = nibind_createclone(nb, tag, master_name, &addr,
				master_tag);
			break;

		case NB_DESTROY:
			status = nibind_destroydomain(nb, tag);
			break;

		case NB_LIST:
			status = nibind_listreg(nb, &reg, &nreg);
			if (status == NI_OK)
			{
				for (i = 0; i < nreg; i++)
				{
					printf("tag=%s udp=%d tcp=%d\n",
						reg[i].tag,
						reg[i].addrs.udp_port,
						reg[i].addrs.tcp_port);
				}
			}
			break;

		case NB_GET:
			status = nibind_getregister(nb, tag, &addrs);
			if (status == NI_OK)
			{
				printf("tag=%s udp=%d tcp=%d\n",
					tag,
					addrs->udp_port,
					addrs->tcp_port);
			}
			break;

		case NB_UNREG:
			status = nibind_unregister(nb, tag);
			break;

		case NB_ALIAS:
			status = nibind_getregister(nb, tag, &addrs);
			if (status != NI_OK)
			{
				fprintf(stderr, "%s: unknown tag %s\n", myname, tag);
				exit(1);
			}

			reg = (nibind_registration *)malloc(sizeof(nibind_registration));
			reg->tag = alias;
			reg->addrs.udp_port = addrs->udp_port;
			reg->addrs.tcp_port = addrs->tcp_port;
			status = nibind_register(nb, reg);
			break;

		default:
			fprintf(stderr, "should never happen\n");
			abort();
	}

	if (status != NI_OK)
	{
		fprintf(stderr, "%s: operation failed - %s\n",
			myname, ni_error(status));
		exit(1);
	}

	exit(0);
}
