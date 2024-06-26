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
/* Copyright (c) 1997 Apple Computer, Inc.  All rights reserved.
 * Copyright (c) 1994-1996 NeXT Software, Inc.  All rights reserved. 
 *
 * AppleIOPSSafeCondLock.m. Lock object with exported condition variable, 
 *	kernel version.
 *
 * HISTORY
 * 1997-11-
 * 01-Aug-91    Doug Mitchell at NeXT
 *      Created. 
 */

#include <IOKit/IOConditionLock.h>

#define super OSObject
OSDefineMetaClassAndStructors(IOConditionLock, OSObject)

bool IOConditionLock::initWithCondition(int inCondition, bool inIntr = true)
{
    if (!super::init())
        return false;

    cond_interlock = IOLockAlloc();
    sleep_interlock = IOLockAlloc();

    condition = inCondition;
    want_lock    = false;
    waiting      = false;
    interruptible = (inIntr) ? THREAD_INTERRUPTIBLE : THREAD_UNINT;

    return cond_interlock && sleep_interlock;
}

IOConditionLock *IOConditionLock::withCondition(int condition, bool intr = true)
{
    IOConditionLock *me = new IOConditionLock;

    if (me && !me->initWithCondition(condition, intr)) {
        me->free();
        return 0;
    }

    return me;
}
void IOConditionLock::free()
{
    if (cond_interlock)
        IOLockFree(cond_interlock);
    if (sleep_interlock)
        IOLockFree(sleep_interlock);
    super::free();
}

bool IOConditionLock::getInterruptible() const
{
    return interruptible;
}

int IOConditionLock:: getCondition() const
{
    return condition;
}

int IOConditionLock:: setCondition(int inCondition)
{
    int old = condition;

    condition = inCondition;
    thread_wakeup_one((void *) &condition);

    return old;
}

void IOConditionLock::unlock()
{
    IOTakeLock(sleep_interlock);

    thread_wakeup_one((void *) &condition);

    want_lock = false;
    if (waiting) {
	waiting = false;
	thread_wakeup(this);	// Wakeup everybody
    }

    IOUnlock(sleep_interlock);
}

void IOConditionLock::unlockWith(int inCondition)
{
    IOTakeLock(sleep_interlock);
    IOTakeLock(cond_interlock);
    
    condition = inCondition;

    IOUnlock(cond_interlock);
    IOUnlock(sleep_interlock);

    unlock();
}

bool IOConditionLock::tryLock()
{
    bool result;

    IOTakeLock(sleep_interlock);

    result = !want_lock;
    if (result)
	want_lock = true;

    IOUnlock(sleep_interlock);

    return result;
}

int IOConditionLock::lock()
{
    int thread_res = THREAD_AWAKENED;

    IOTakeLock(sleep_interlock);
    
    /* Try to acquire the want_lock bit. */
    while (want_lock && thread_res == THREAD_AWAKENED)
    {
	waiting = true;

        assert_wait((void *) this, interruptible);	/* assert event */
	IOUnlock(sleep_interlock);			/* release the lock */
	thread_res = thread_block((void (*)(void)) 0);	/* block ourselves */

        IOTakeLock(sleep_interlock);
    }
    if (thread_res == THREAD_AWAKENED)
	want_lock = true;
    
    IOUnlock(sleep_interlock);

    return thread_res;
}

int IOConditionLock::lockWhen(int inCondition)
{
    int thread_res;
    
    do
    {
	/* First get the actual lock */
	thread_res = lock();
	if (thread_res != THREAD_AWAKENED)
	    break;	// Failed to acquire lock

	if (inCondition == condition)
	    break;	// Hold lock and condition is expected value

	/*
	 * Need to hold a IOTakeLock when we call thread_sleep().
	 * Both _cond_interlock and want_lock must be held to 
	 * change _condition.
	 */
	IOTakeLock(cond_interlock);
        unlock();	// Release lock and sleep
	
	/*
	 * this is the critical section on a multi in which
	 * another thread could hold _sleep_interlock, but they 
	 * can't change _condition. Holding _cond_interlock here
	 * (until after assert_wait() is called from 
	 * thread_sleep()) ensures that we'll be notified
	 * of changes in _condition.
	 */
        assert_wait((void *) &condition, interruptible); /* assert event */
	IOUnlock(cond_interlock);			/* release the lock */
	thread_res = thread_block((void (*)(void)) 0);	/* block ourselves */
    } while (thread_res == THREAD_AWAKENED);
    
    return thread_res;
}
