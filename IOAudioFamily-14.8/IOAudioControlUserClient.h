/*
 * Copyright (c) 1998-2000 Apple Computer, Inc. All rights reserved.
 *
 * @APPLE_LICENSE_HEADER_START@
 *
 * The contents of this file constitute Original Code as defined in and
 * are subject to the Apple Public Source License Version 1.1 (the
 * "License").  You may not use this file except in compliance with the
 * License.  Please obtain a copy of the License at
 * http://www.apple.com/publicsource and read it before using this file.
 *
 * This Original Code and all software distributed under the License are
 * distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY KIND, EITHER
 * EXPRESS OR IMPLIED, AND APPLE HEREBY DISCLAIMS ALL SUCH WARRANTIES,
 * INCLUDING WITHOUT LIMITATION, ANY WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE OR NON-INFRINGEMENT.  Please see the
 * License for the specific language governing rights and limitations
 * under the License.
 *
 * @APPLE_LICENSE_HEADER_END@
 */

#ifndef _IOKIT_IOAUDIOCONTROLUSERCLIENT_H
#define _IOKIT_IOAUDIOCONTROLUSERCLIENT_H

#include <IOKit/IOUserClient.h>

#include <IOKit/audio/IOAudioTypes.h>

class IOAudioControl;

class IOAudioControlUserClient : public IOUserClient
{
    OSDeclareDefaultStructors(IOAudioControlUserClient)
    
protected:
    task_t 				clientTask;
    IOAudioControl *			audioControl;
    IOAudioNotificationMessage *	notificationMessage;

    IOExternalMethod	methods[IOAUDIOCONTROL_NUM_CALLS];

    virtual IOReturn clientClose();
    virtual IOReturn clientDied();

    virtual IOReturn setControlValue(UInt32 value);
    virtual IOReturn getControlValue(UInt32 *value);

public:
    static IOAudioControlUserClient *withAudioControl(IOAudioControl *control, task_t clientTask, void *securityID, UInt32 type);

    virtual bool initWithAudioControl(IOAudioControl *control, task_t owningTask, void *securityID, UInt32 type);
    virtual void free();

    virtual IOExternalMethod *getExternalMethodForIndex(UInt32 index);
    virtual IOReturn registerNotificationPort(mach_port_t port, UInt32 type, UInt32 refCon);

    virtual IOReturn setProperties(OSObject *properties);

    virtual void sendValueChangeNotification();
};

#endif /* _IOKIT_IOAUDIOCONTROLUSERCLIENT_H */
