#ifndef _PRAND_CMD_H_
#define _PRAND_CMD_H_

const char *cmds[] = {
	"/bin/ps -ef 2>&1",
	"/etc/arp -n -a 2>&1",
	"/usr/bin/netstat -an 2>&1",
	"/bin/df  2>&1",
	"/usr/ucb/uptime  2>&1",
	"/usr/bin/netstat -s 2>&1",
	NULL
};

const char *dirs[] = {
	"/tmp",
	"/usr/tmp",
	".",
	"/",
	"/usr/spool",
	"/dev",
	"/home",
	NULL
};

const char *files[] = {
	NULL
};

#endif /* _PRAND_CMD_H_ */
