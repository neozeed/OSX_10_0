#include "port_before.h"

#include <stdio.h>

#include "port_after.h"

/*
 * UNIX rename() will overwrite a file if it exists, NT rename() will not
 * emulate with a new name (used by named-xfer.c)
 */
int isc_movefile(const char *oldname, const char *newname) {
	int rc;

	ntPrintf(9, "isc_movefile(%s, %s)\n", oldname, newname);
	_chmod(newname, _S_IREAD | _S_IWRITE);
	rc = CopyFile(oldname, newname, FALSE);
	if (rc != 0) {
		_chmod(oldname, _S_IREAD | _S_IWRITE);
		_unlink(oldname);
		return (0);
	}
	return (-1);
}
