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
/*
 * Copyright (c) 1999 Apple Computer, Inc.  All rights reserved.
 *
 */

#include <ppc/proc_reg.h>

#include <IOKit/IOLib.h>

#include "USBKeyLargo.h"

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#define super IOService

OSDefineMetaClassAndStructors(USBKeyLargo, IOService);

// --------------------------------------------------------------------------
// Method: initForBus
//
// Purpose:
//   initialize the driver for a specific bus, so that the driver knows which bus
//   it is responsable for.
bool
USBKeyLargo::initForBus(UInt32 busNumber)
{
    setProperty("usb", (UInt64)busNumber, 32);
    setProperty("IOClass", "USBKeyLargo");

    // initialize for Power Management
    initForPM(getProvider());

    return true;
}

// --------------------------------------------------------------------------
// Method: initForPM
//
// Purpose:
//   initialize the driver for power managment and register ourselves with
//   superclass policy-maker
void USBKeyLargo::initForPM (IOService *provider)
{
    PMinit();                   // initialize superclass variables
    provider->joinPMtree(this); // attach into the power management hierarchy

    // KeyLargo has only 2 power states::
    // 0 OFF
    // 1 all ON
    // Pwer state fields:
    // unsigned long	version;		// version number of this struct
    // IOPMPowerFlags	capabilityFlags;	// bits that describe (to interested drivers) the capability of the device in this state
    // IOPMPowerFlags	outputPowerCharacter;	// description (to power domain children) of the power provided in this state
    // IOPMPowerFlags	inputPowerRequirement;	// description (to power domain parent) of input power required in this state
    // unsigned long	staticPower;		// average consumption in milliwatts
    // unsigned long	unbudgetedPower;	// additional consumption from separate power supply (mw)
    // unsigned long	powerToAttain;		// additional power to attain this state from next lower state (in mw)
    // unsigned long	timeToAttain;		// time required to enter this state from next lower state (in microseconds)
    // unsigned long	settleUpTime;		// settle time required after entering this state from next lower state (microseconds)
    // unsigned long	timeToLower;		// time required to enter next lower state from this one (in microseconds)
    // unsigned long	settleDownTime;		// settle time required after entering next lower state from this state (microseconds)
    // unsigned long	powerDomainBudget;	// power in mw a domain in this state can deliver to its children

    // NOTE: all these values are made up since now I do not have areal clue of what to put.
#define number_of_power_states 2

    static IOPMPowerState ourPowerStates[number_of_power_states] = {
    {1,0,0,0,0,0,0,0,0,0,0,0},
    {1,IOPMDeviceUsable,IOPMClockNormal,IOPMPowerOn,0,0,0,0,0,0,0,0}
    };


    // register ourselves with ourself as policy-maker
    if (pm_vars != NULL)
        registerPowerDriver(this, ourPowerStates, number_of_power_states);
}

// Method: setPowerState
//
// Purpose:
IOReturn USBKeyLargo::setPowerState(unsigned long powerStateOrdinal, IOService* whatDevice)
{
    // Do not do anything if the state is inavalid.
    if (powerStateOrdinal >= 2)
        return IOPMNoSuchState;

    // Which bus id going to change state ?
    OSNumber *propertyValue;

    propertyValue = OSDynamicCast( OSNumber, getProperty("usb"));
    if (propertyValue == NULL) {
        return IOPMNoSuchState;
    }

    if ( powerStateOrdinal == 0 ) {
        kprintf("USBKeyLargo would be powered off here\n");
        turnOffUSB(propertyValue->unsigned32BitValue());
    }
    if ( powerStateOrdinal == 1 ) {
        kprintf("USBKeyLargo would be powered on here\n");
        turnOnUSB(propertyValue->unsigned32BitValue());
    }
    return IOPMAckImplied;
}

void USBKeyLargo::turnOffUSB(UInt32 busNumber)
{
    UInt32 regTemp;
    KeyLargo *provider = OSDynamicCast(KeyLargo, getProvider());

    if (provider == NULL) {
        IOLog("USBKeyLargo::turnOffUSB missing provider, can not proceed");
        return;
    }

    // well, USB requires special timing of the various FCRx register
    // changes, and should probably be broken out separately. However,
    // I will hack in the necessary USB changes here.

    // USB requires the FCR4 bits before any of the other registers are
    // touched. Since FCR4 is ALL USB, then I moved it to the top of
    // the programming list
    regTemp = provider->readRegUInt32(kKeyLargoFCR4);
    //kprintf("USBKeyLargo::turnOffUSB: FCR4 at beginning %08x\n",regTemp);

    if (busNumber == 0) {
        regTemp |= kKeyLargoFCR4USB0SleepBitsSet;
        regTemp &= ~kKeyLargoFCR4USB0SleepBitsClear;      
    }
    else {
        regTemp |= kKeyLargoFCR4USB1SleepBitsSet;
        regTemp &= ~kKeyLargoFCR4USB1SleepBitsClear;        
    }
    
    provider->writeRegUInt32(kKeyLargoFCR4, regTemp);
    //regTemp = provider->readRegUInt32(kKeyLargoFCR4);
    //kprintf("USBKeyLargo::turnOffUSB: FCR4 at end %08x\n",regTemp);

    // WE SHOULD HAVE A 1 MICROSECOND DELAY IN HERE
    IODelay(1); // Marco changed this from IOSleep because I do not want other threads run at this time

    // now some stuff in FCR0 which cannot be done all at once

    // clear the Cell Enable bits for USB (turns off the 48 MHz clocks)
    regTemp = provider->readRegUInt32(kKeyLargoFCR0);
    //kprintf("USBKeyLargo::turnOffUSB: FCR0 at beginning %08x\n",regTemp);

    if (busNumber == 0) {
        regTemp &= ~kKeyLargoFCR0USB0CellEnable;
    }
    else {
        regTemp &= ~kKeyLargoFCR0USB1CellEnable;        
    }
        
    provider->writeRegUInt32(kKeyLargoFCR0, regTemp);
    //regTemp = provider->readRegUInt32(kKeyLargoFCR0);
    //kprintf("USBKeyLargo::turnOffUSB: FCR0 at middle %08x\n",regTemp);

    // NEED A 600 nanosecond delay in here
    IODelay(1); // Marco changed this from IOSleep because I do not want other threads run at this time

    // now set the pad suspend bits
    regTemp = provider->readRegUInt32(kKeyLargoFCR0);

    if (busNumber == 0) {
        regTemp |= (kKeyLargoFCR0USB0PadSuspend0 |
                    kKeyLargoFCR0USB0PadSuspend1 );
    }
    else {
        regTemp |= (kKeyLargoFCR0USB1PadSuspend0 |
                    kKeyLargoFCR0USB1PadSuspend1);
    }
    
    provider->writeRegUInt32(kKeyLargoFCR0, regTemp);
    //regTemp = provider->readRegUInt32(kKeyLargoFCR0);
    //kprintf("USBKeyLargo::turnOffUSB: FCR0 at end %08x\n",regTemp);

    // NEED A 600 nanosecond delay in here
    IODelay(1); // Marco changed this from IOSleep because I do not want other threads run at this time
}

void USBKeyLargo::turnOnUSB(UInt32 busNumber)
{
    UInt32 regTemp;
    KeyLargo *provider = OSDynamicCast(KeyLargo, getProvider());

    if (provider == NULL) {
        IOLog("USBKeyLargo::turnOnUSB missing provider, can not proceed");
        return;
    }

    // now we clear the individual pad suspend bits
    // now set the pad suspend bits
    regTemp = provider->readRegUInt32(kKeyLargoFCR0);

    if (busNumber == 0) {
        regTemp &= ~(kKeyLargoFCR0USB0PadSuspend0 |
                     kKeyLargoFCR0USB0PadSuspend1);
    }
    else {
        regTemp &= ~(kKeyLargoFCR0USB1PadSuspend0 |
                     kKeyLargoFCR0USB1PadSuspend1);
    }

    provider->writeRegUInt32(kKeyLargoFCR0, regTemp);

    IODelay(1000);

    // now we go ahead and turn on the USB cell clocks
    regTemp = provider->readRegUInt32(kKeyLargoFCR0);

    if (busNumber == 0)
        regTemp |= (kKeyLargoFCR0USB0CellEnable);
    else
        regTemp |= (kKeyLargoFCR0USB1CellEnable);
    
    provider->writeRegUInt32(kKeyLargoFCR0, regTemp);


    // now turn off the remote wakeup bits
    regTemp = provider->readRegUInt32(kKeyLargoFCR4);

    if (busNumber == 0) {
        regTemp &= ~kKeyLargoFCR4USB0SleepBitsSet;
        regTemp |= kKeyLargoFCR4USB0SleepBitsClear;        
    }
    else {
        regTemp &= ~kKeyLargoFCR4USB1SleepBitsSet;
        regTemp |= kKeyLargoFCR4USB1SleepBitsClear;    
    }
    
    provider->writeRegUInt32(kKeyLargoFCR4, regTemp);
    
}
