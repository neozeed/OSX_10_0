/*
 * DNSpider v1.2
 * A fairly simple program which takes a CIDR address block and walks
 * through it, resolving each address and printing the results to stdout
 * ---
 * Thanks to Ken Harris (boii/ken@arpa.com) for the idea
 * ---
 * dnsp -h for help
 * Copyright (C) 1999 Chip Norkus
 */

/* 
 * HISTORY:
 * 1.1: initial release, it's not clean, but it works.
 * 1.2: Got rid of the malloc() bullshit, now you can scan an /8 if you're
 * some sort of freak of nature, without killing your box.  Also
 * added netmasks as well as bitmasks for the true networking nerd.
 */

#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#define VERSION "1.2"

/* print out usage */
void usage(void) {
	printf(
"Usage: dnsp [-a|-b|-h|-i|-I [#]|-r|-R] <cidr address block>\n"
"   -a: print all addresses, resolved or not\n"
"   -b: print out only IPs which resolve in named style (addr in ptr name)\n"
"   -h: help\n"
"   -i: print out start - end IPs in given CIDR block\n"
"   -I [addresses per line]: print out all IPs in\n"
"   -r: print out only IPs which resolve at least one way\n"
"   -R: print out only IPs which resolve both ways\n"
"   -v: display version information!\n"
"Decoding output:\n"
"Basically you will see three kinds of things:\n"
" address: [Unresolved] (Error message)\n"
" - This simply says that the address could not initially be resolved.\n"
" address: some.name.org (U: Error message/reason)\n"
" - This means the IP resolved, but trying to reverse resolve failed\n"
" address: some.name.org (R)\n"
" - This means the IP resolved, and trying to reverse back to the IP\n"
"   worked.  R is a good thing, try to keep all your IP/host pairs in\n"
"   this state.\n");
   exit(0);
}

void version(void) {
	printf("Copyright (C) 1999 Chip Norkus\n");
	printf("Idle props to kenny for harassing me about this 'til I did it,\n");
	printf("It's kinda neat.\n");
	exit(0);
}
	
int main(int argc, char **argv) {
	char o=0;
	unsigned long netmask=0, bitmask, i, addrperline, j;
	unsigned long a=0, b=0, c=0, d=0;
	unsigned long start;
	unsigned long end;
	unsigned long mask;
	unsigned long size;
	char ip[16], nmask[16];
	struct in_addr in;	
	printf("DNSpider version %s by Chip Norkus (wd/white_dragon) (wd@routing.org)\n", VERSION);
	
	if(argc<2)
	  usage();	

	o=getopt(argc, argv, "abhiI:rRv");
	switch(o) {
	 case 'I':
		addrperline=atoi(optarg);
	 case 'i':
	 case 'r':
	 case 'R':
	 case 'a':
	 case 'b':
		break;
	 case 'h':
	 case '?':
		usage();
		break;
	 case 'v':
		version();
		break;
	}
	
	if(o<=0) o='r'; /* default behavior. */
	
	if(o=='I' && !(0 < addrperline <= 10)) {
		printf("Bad number for addresses per line\n");
		_exit(0);
	}
	
	if(strchr(argv[optind], ':'))
	  netmask=1;
	
	if(!netmask)
	  sscanf(argv[optind], "%[^/]/%lu", ip, &bitmask);
	else {
		sscanf(argv[optind], "%[^:]", ip, nmask);
		/* sscanf this way won't work...don't ask me why! */
		strcpy(nmask, (char *)(strchr(argv[optind], ':')+1));
	}
	
   sscanf(ip, "%lu.%lu.%lu.%lu", &a, &b, &c, &d);
	start=(a<<24)+(b<<16)+(c<<8)+d;
	if(!netmask) {
		if(!(0<bitmask<33)) {
			printf("Invalid bitmask!  Giving up!\n");
			exit(0);
		}
		mask=(0xFFFFFFFF<<(32-bitmask));
	}
	else {
		sscanf(nmask, "%lu.%lu.%lu.%lu", &a, &b, &c, &d);
		mask=(a<<24)+(b<<16)+(c<<8)+d;
		if((start&~mask)) {
			printf("Invalid netmask!  Giving up!\n");
			exit(0);
		}
	}
	end=start+(~mask);
	size=end-start+1;
	
	start=htonl(start);
	end=htonl(end);
	if(o=='i') {
		in.s_addr=start;
		printf("Start : %s\n", inet_ntoa(in));
		in.s_addr=end;
		printf("End   : %s\n", inet_ntoa(in));
		printf("Length: %lu\n", size);
	}
	else if(o=='I') {
		if(!netmask)
		  printf("IP listing for network [%s/%i] (%lu addresses)\n", 
					ip, bitmask, size);
		else
		  printf("IP listing for network [%s:%s] (%lu addresses)\n",
					ip, nmask, size);
		printf("%i addresses per line\n", addrperline);
		for(i=0, j=addrperline;i<size;i++,j--) {
			if(!j) {
				printf("\n");
				j=addrperline;
			}
			in.s_addr=start+htonl(i);
			printf("%-16s", inet_ntoa(in));
		}
		printf("\n");
	}
	else { /* excitement, we now walk the list and resolve things! */
		struct hostent *hp_ip;  /* One when we gethostbaddr.. */
		struct hostent *hp_res; /* and one when we gethostbyname */ 
		char hostname[64]; /* hostname we resolved */
		if(!netmask)
		  printf("IP/hostname listing for network [%s/%i] (%lu addresses)\n",
					ip, bitmask, size);
		else
		  printf("IP/hostname listing for network [%s:%s] (%lu addresses)\n",
					ip, nmask, size);
		switch(o) {
		 case 'b':
			printf("Printing in named/bind compatible format\n");
			break;
		 case 'a':
			printf("Printing all addresses\n");
			break;
		 case 'r':
			printf("Printing only addresses which resolve at least one way\n");
			break;
		 case 'R':
			printf("Printing only addresses which resolve both ways\n");
			break;
		 default:
			break;
		}
		for(i=0;i<size;i++, hp_ip=hp_res=NULL) {
			in.s_addr=start+(htonl(i));
			hp_ip=gethostbyaddr((const char *)&in, 4, AF_INET);
			if((hp_ip==NULL || hp_ip->h_name==NULL)) {
				if(o=='a')
				  printf("%-15s: [Unresolved] (%s)\n", inet_ntoa(in), 
							hstrerror(h_errno));
				continue;
			}
			strncpy(hostname, hp_ip->h_name, 63);
			if(o!='b' && o!='R') {
				printf("%-15s: ", inet_ntoa(in));
				fflush(stdout); /* neat waiting effect...I'm a geek with artistic
									  * blood in my veins! */
			}
			hp_res=gethostbyname(hostname);
			if(hp_res==NULL || strcasecmp(hostname, hp_res->h_name)) {
				if(o=='b') /* this is really limited, oh well! */
				  printf("%i\tIN\tPTR\t%s.\n",i-1, hostname);
				else if(o!='R')
				  printf("%s (U: %s)\n", hostname, (hp_res == NULL) ?
							hstrerror(h_errno) : "reverse check did not match");
			}
			else {
				if(o=='b')
				  printf("%i\tIN\tPTR\t%s.\n", i-1, hostname);
				else {
					if(o=='R')
					  printf("%-15s: ", inet_ntoa(in));
					printf("%s (R)\n", hostname);
				}
			}
		}
	}
	printf("\n");	 
	_exit(0);
}

