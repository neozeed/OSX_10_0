#include "port_before.h"

#include <stdio.h>
#include <windows.h>
#include <string.h>
#include <stdlib.h>
#include <syslog.h>
#include <isc/logging.h>

#include <bindevt.h>

#include "port_after.h"

static HANDLE hAppLog = NULL;
static FILE *log_stream;
static int debug_level = 0;

/*
 * Log to the NT Event Log
 */
void syslog(int level, const char *fmt, ...)
{
	va_list ap;
	char buf[1024];
	char *str[1];

	str[0] = buf;

	va_start(ap, fmt);
	vsprintf(buf, fmt, ap);
	va_end(ap);

	switch (level)
	{
		case LOG_INFO:
		case LOG_NOTICE:
		case LOG_DEBUG:
			ReportEvent(hAppLog, EVENTLOG_INFORMATION_TYPE, 0, BIND_INFO_MSG, NULL, 1, 0, str, NULL);
			break;
		case LOG_WARNING:
			ReportEvent(hAppLog, EVENTLOG_WARNING_TYPE, 0, BIND_WARN_MSG, NULL, 1, 0, str, NULL);
			break;
		default:
			ReportEvent(hAppLog, EVENTLOG_ERROR_TYPE, 0, BIND_ERR_MSG, NULL, 1, 0, str, NULL);
			break;
	}
}

/*
 * Initialize event logging
 */
void openlog(char *name, int flags, ...)
{
  /* Get a handle to the Application event log */
  hAppLog = RegisterEventSource(NULL, name);
}

/*
 * Keep event logging synced with the current debug level
 */
void ModifyLogLevel(int level)
{
	debug_level = level;	
}

/*
 * Initialize logging for the port section of libbind.
 * Piggyback onto stream given.
 */
void InitNTLogging(FILE *stream, int debug)
{
	log_stream = stream;
	ModifyLogLevel(debug);
}

/*
 * Log a message
 */
void ntPrintf(int level, const char *format, ...)
{
	va_list ap;

	va_start(ap, format);
	if (log_stream != NULL && debug_level >= level) {
		vfprintf(log_stream, format, ap);
		fflush(log_stream);
	}
	va_end(ap);
}