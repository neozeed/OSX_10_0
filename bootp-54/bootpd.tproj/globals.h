
#import <objc/Object.h>
#import "NIDomain.h"
#import "NICache.h"

extern NICache_t	cache;
extern int		bootp_socket;
extern int		debug;
extern NIDomain_t *	ni_local;
extern NIDomainList_t	niSearchDomains;
extern int		quiet;
extern unsigned short	server_priority;
extern u_char		rfc_magic[4];
extern char		server_name[MAXHOSTNAMELEN + 1];
extern id		subnets;
extern char *		testing_control;
extern char		transmit_buffer[];
extern int		verbose;
