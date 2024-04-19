#ifndef BIND_SERVICE_H
#define BIND_SERVICE_H

#include <winsvc.h>

#define BIND_DISPLAY_NAME "ISC BIND"
#define BIND_SERVICE_NAME "ISC BIND"
#define BIND_WINDOW_TITLE "BIND v8.2.2"

/* control codes that the service will accept */
#define SERVICE_CONTROL_DUMPDB  128
#define SERVICE_CONTROL_RELOAD  129
#define SERVICE_CONTROL_STATS   130
#define SERVICE_CONTROL_TRACE   131
#define SERVICE_CONTROL_NOTRACE 132
#define SERVICE_CONTROL_QRYLOG  133

/* Handle to SCM for updating service status */
SERVICE_STATUS_HANDLE	hServiceStatus;

void UpdateSCM(DWORD);
void ServiceControl(DWORD dwCtrlCode);

#endif