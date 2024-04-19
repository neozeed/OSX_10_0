/* Windows-specific path definitions */

#ifndef _PATHS_H
#define _PATHS_H

#undef _PATH_HOSTS
#undef _PATH_NETWORKS
#undef _PATH_PROTOCOL
#undef _PATH_SERVICES
#undef _PATH_RESCONF

#define PATH_ETC_ID			0
#define PATH_DEBUG_ID		1
#define PATH_CONF_ID		2
#define PATH_PIDFILE_ID		3
#define PATH_TMPXFER_ID		4
#define PATH_XFER_ID		5
#define PATH_NAMED_ID		6
#define	PATH_HOSTS_ID		7
#define	PATH_NETWORKS_ID	8
#define	PATH_PROTOCOL_ID	9
#define	PATH_SERVICES_ID	10
#define PATH_RESCONF_ID		11
#define PATH_NDCSOCK_ID		12

/*
 * Since locations can vary, we need to map to something intelligent
 * to determine where these point to.
 */
#define _PATH_ETC		BINDPath(PATH_ETC_ID)
#define _PATH_DEBUG		BINDPath(PATH_DEBUG_ID)
#define _PATH_CONF		BINDPath(PATH_CONF_ID)
#define _PATH_PIDFILE	BINDPath(PATH_PIDFILE_ID)
#define _PATH_TMPXFER	BINDPath(PATH_TMPXFER_ID)

#define _PATH_XFER		BINDPath(PATH_XFER_ID)
#define _PATH_NAMED		BINDPath(PATH_NAMED_ID)

#define	_PATH_HOSTS		BINDPath(PATH_HOSTS_ID)
#define	_PATH_NETWORKS	BINDPath(PATH_NETWORKS_ID)
#define	_PATH_PROTOCOL	BINDPath(PATH_PROTOCOL_ID)
#define	_PATH_SERVICES	BINDPath(PATH_SERVICES_ID)
#define _PATH_RESCONF	BINDPath(PATH_RESCONF_ID)
#define _PATH_NDCSOCK	BINDPath(PATH_NDCSOCK_ID)
#define _PATH_HELPFILE	"%SystemRoot%\\System32\\nslookup.help |more"	//Pipe it to more
#define _PATH_PAGERCMD	"type"


#endif /* _PATHS_H */
