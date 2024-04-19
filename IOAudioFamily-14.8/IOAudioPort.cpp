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

#include <IOKit/audio/IOAudioPort.h>
#include <IOKit/audio/IOAudioControl.h>
#include <IOKit/audio/IOAudioDevice.h>
#include <IOKit/audio/IOAudioTypes.h>
#include <IOKit/audio/IOAudioDefines.h>

#include <libkern/c++/OSSet.h>
#include <libkern/c++/OSCollectionIterator.h>

#include <IOKit/IOLib.h>

#define super IOService

OSDefineMetaClassAndStructors(IOAudioPort, IOService)

IOAudioPort *IOAudioPort::withAttributes(const char *portType, const char *portName, const char *subType, OSDictionary *properties)
{
    IOAudioPort *port;

    port = new IOAudioPort;
    if (port) {
        if (!port->initWithAttributes(portType, portName, subType, properties)) {
            port->release();
            port = 0;
        }
    }

    return port;
}

bool IOAudioPort::initWithAttributes(const char *portType, const char *portName, const char *subType, OSDictionary *properties)
{
    if (!init(properties)) {
        return false;
    }

    if (!portType) {
        return false;
    }

    audioDevice = 0;
    isRegistered = false;
    
    setProperty(IOAUDIOPORT_TYPE_KEY, portType);

    if (portName) {
        setProperty(IOAUDIOPORT_NAME_KEY, portName);
    }

    if (subType) {
        setProperty(IOAUDIOPORT_SUBTYPE_KEY, subType);
    }

    audioControls = OSSet::withCapacity(1);
    if (!audioControls) {
        return false;
    }

    return true;
}

void IOAudioPort::free()
{
    if (audioControls) {
        audioControls->release();
    }

    super::free();
}

bool IOAudioPort::start(IOService *provider)
{
    if (!super::start(provider)) {
        return false;
    }

    if (!(audioDevice = OSDynamicCast(IOAudioDevice, provider))) {
        return false;
    }

    return true;
}

void IOAudioPort::stop(IOService *provider)
{
    deactivateAudioControls();
    super::stop(provider);
}

void IOAudioPort::registerService(IOOptionBits options = 0)
{
    super::registerService(options);

    if (audioControls && !isRegistered) {
        OSCollectionIterator *iterator;

        iterator = OSCollectionIterator::withCollection(audioControls);
	if (iterator) {
            IOAudioControl *control;
    
            while (control = (IOAudioControl *)iterator->getNextObject()) {
                control->registerService();
            }
            iterator->release();
	}
    }

    isRegistered = true;
}

IOAudioDevice *IOAudioPort::getAudioDevice()
{
    return audioDevice;
}

bool IOAudioPort::addAudioControl(IOAudioControl *control)
{
    if (!control || !audioControls) {
        return false;
    }

    if (!control->attach(this)) {
        return false;
    }

    if (!control->start(this)) {
        control->detach(this);
        return false;
    }

    audioControls->setObject(control);

    if (isRegistered) {
        control->registerService();
    }

    return true;
}

void IOAudioPort::deactivateAudioControls()
{
    OSCollectionIterator *iterator;

    if (!audioControls) {
        return;
    }

    iterator = OSCollectionIterator::withCollection(audioControls);

    if (iterator) {
        IOAudioControl *control;

        while (control = (IOAudioControl *)iterator->getNextObject()) {
            if (!isInactive()) {
                control->terminate();
            }
        }

        iterator->release();
    }

    audioControls->flushCollection();
}

IOReturn IOAudioPort::performAudioControlValueChange(IOAudioControl *control, UInt32 newValue)
{
    IOReturn result = kIOReturnError;
    
    if (audioDevice) {
        result = audioDevice->performAudioControlValueChange(control, newValue);
    }
    
    return result;
}
