#include <port_before.h>
#include <bind_registry.h>
#include <port_after.h>

extern char *cmds[];
extern char *dirs[];
/*
 * Module Variables
 */
static char systemDir[MAX_PATH];
static char namedBase[MAX_PATH];
static DWORD baseLen = MAX_PATH;

void Init_PRandom();

BOOL InitPaths()
{
	HKEY hKey;
	BOOL keyFound = TRUE;

	memset(namedBase, 0, MAX_PATH);
	if(RegOpenKeyEx(HKEY_LOCAL_MACHINE, BIND_SUBKEY, 0, KEY_READ, &hKey) != ERROR_SUCCESS)
		keyFound = FALSE;
	
	if(keyFound == TRUE)
	{
		// Get the named directory
		if(RegQueryValueEx(hKey, "InstallDir", NULL, NULL, (LPBYTE)namedBase, &baseLen) != ERROR_SUCCESS)
			keyFound = FALSE;
	}
	
	RegCloseKey(hKey);

	GetSystemDirectory(systemDir, MAX_PATH);

	if(keyFound == FALSE)
		strcpy(namedBase, systemDir);		// Use the System Directory as a default
/*
 * Add code to fix up the prand_conf.h data so we can support all types
 * of different directory structures
 */
	Init_PRandom();

	return(TRUE);	
}
/*
 * Initialization code to fix up the arrays of commands, directories and
 * files defined in prand_conf.h
 */
void Init_PRandom()
{
	char *buf;
	int i;

	char temp[MAX_PATH];	/* Temporary location for the data while copying */
	char *sysdrive; /* System Drive */
	char windDir[MAX_PATH]; /* Windows Directory */
	char tempPath[MAX_PATH]; /* Temp Directory Path */

	/* First the cmds */

	for (i = 0; cmds[i] != NULL; i++)
	{
		strcpy(temp, cmds[i]);
		cmds[i] = (char *) malloc(MAX_PATH);	/* Allocate some space */
		sprintf(cmds[i], "%s\\%s", systemDir, temp);
	}

	/* Now for the directories */
	sysdrive = getenv("SystemDrive");
	GetTempPath( MAX_PATH, tempPath);
	i = strlen(tempPath) - 1;
	tempPath[i] = '\0';		/* Delete the trailing backslash */

	GetWindowsDirectory(windDir, MAX_PATH);

	/* Now write all the directories back into the dirs array
	 */

	for (i = 1; i<= 6; i++)
	{
		dirs[i] = (char *) malloc(MAX_PATH); /* Allocate some space to keep */
	}
	sprintf(dirs[1], "%s\\", sysdrive);
	sprintf(dirs[2], "%s", tempPath);
	sprintf(dirs[3], "%s\\program files", sysdrive);
	strcpy(dirs[4], windDir);
	sprintf(dirs[5], "%s\\system32", windDir);
	sprintf(dirs[6], "%s\\system", windDir);
}


char *BINDPath(int id)
{
	static char buf[_MAX_PATH];
	memset(buf, 0, _MAX_PATH);

	switch(id)
	{
		case PATH_ETC_ID:
			sprintf(buf, "%s\\etc", namedBase);
			break;
		case PATH_DEBUG_ID:
			sprintf(buf, "%s\\etc\\named.run", namedBase);
			break;
		case PATH_CONF_ID:
			sprintf(buf, "%s\\etc\\named.conf", namedBase);
			break;
		case PATH_PIDFILE_ID:
			sprintf(buf, "%s\\etc\\named.pid", namedBase);
			break;
		case PATH_TMPXFER_ID:
			sprintf(buf, "%s\\etc\\xfer.ddt.XXXXX", namedBase);
			break;
		case PATH_XFER_ID:
			sprintf(buf, "%s\\bin\\named-xfer.exe", namedBase);
			break;
		case PATH_NAMED_ID:
			sprintf(buf, "%s\\bin\\named.exe", namedBase);
			break;
		case PATH_HOSTS_ID:
			sprintf(buf, "%s\\Drivers\\etc\\HOSTS", systemDir);
			break;
		case PATH_NETWORKS_ID:
			sprintf(buf, "%s\\Drivers\\etc\\NETWORKS", systemDir);
			break;
		case PATH_PROTOCOL_ID:		
			sprintf(buf, "%s\\Drivers\\etc\\PROTOCOL", systemDir);
			break;
		case PATH_SERVICES_ID:		
			sprintf(buf, "%s\\Drivers\\etc\\SERVICES", systemDir);
			break;
		case PATH_RESCONF_ID:			
			sprintf(buf, "%s\\Drivers\\etc\\resolv.conf", systemDir);
			break;
		case PATH_NDCSOCK_ID:
			sprintf(buf, "%s\\bin\\ndc", namedBase);
			break;
		default:
			return(NULL);
			break;
	}
	return(buf);
}
