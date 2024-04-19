#ifndef _SYSLOG_H
#define _SYSLOG_H

/* Constant definitions for openlog() */
#define LOG_PID		1
#define LOG_CONS	2

void syslog(int level, const char *fmt, ...);
void openlog(char *, int, ...);

void ModifyLogLevel(int level);
void InitNTLogging(FILE *, int);
void ntPrintf(int, const char *, ...);

#endif
