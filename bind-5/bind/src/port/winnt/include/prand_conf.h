#ifndef _PRAND_CMD_H_
#define _PRAND_CMD_H_

#ifndef HAVE_DEV_RANDOM
#define HAVE_DEV_RANDOM 1
#endif /* HAVE_DEV_RANDOM */

//#pragma message("TODO: Ensure cmds[] array is valid in prand_conf.h")
char *cmds[] = {
	"net.exe view 2>&1",
	"arp.exe -a 2>&1",
	"netstat.exe -s 2>&1",
	"ipconfig.exe /all 2>&1",
	"nbtstat.exe -c 2>&1",
	"tree.com C:\\  2>&1",
	"net.exe statistics workstation 2>&1",
	NULL
};

//#pragma message("TODO: Ensure dirs[] array is valid in prand_conf.h")
char *dirs[] = {
	".",
	"C:\\",
	"C:\\temp",
	"C:\\program files",
	"C:\\winnt",
	"C:\\winnt\\system32",
	"C:\\winnt\\system",
	NULL
};

#pragma message("TODO: Put something in the files[] array in prand_conf.h")
const char *files[] = {
	NULL
};

#endif /* _PRAND_CMD_H_ */
