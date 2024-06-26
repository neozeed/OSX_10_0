/*
 * Copyright (c) 2000 Apple Computer, Inc. All rights reserved.
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
 * catch_exception_raise will be implemented by user programs
 * This implementation is provided to resolve the reference in
 * exc_server().
 */

#include <mach/boolean.h>
#include <mach/message.h>
#include <mach/kern_return.h>
#include <mach/mach_traps.h>
#include <mach/mach_types.h>
#include <mach/clock_types.h>

extern kern_return_t _host_semaphore_traps_return_;
                                       
kern_return_t semaphore_signal(
    mach_port_t signal_semaphore)
{
        return semaphore_signal_trap(signal_semaphore);
}

kern_return_t semaphore_signal_all(
    mach_port_t signal_semaphore)
{
        return semaphore_signal_all_trap(signal_semaphore);
}

kern_return_t semaphore_signal_thread(
    mach_port_t signal_semaphore,
    mach_port_t thread_act)
{
        return semaphore_signal_thread_trap(signal_semaphore, thread_act);
}

kern_return_t semaphore_wait (
    mach_port_t wait_semaphore)
{
        return semaphore_wait_trap(wait_semaphore);
}

kern_return_t semaphore_timedwait (
    mach_port_t wait_semaphore,
    mach_timespec_t wait_time)
{
        return semaphore_timedwait_trap(wait_semaphore,
                                        wait_time.tv_sec,
                                        wait_time.tv_nsec);
}

kern_return_t semaphore_wait_signal (
    mach_port_t wait_semaphore,
    mach_port_t signal_semaphore)
{
        return semaphore_wait_signal_trap(wait_semaphore, signal_semaphore);
}

kern_return_t semaphore_timedwait_signal (
    mach_port_t wait_semaphore,
    mach_port_t signal_semaphore,
    mach_timespec_t wait_time)
{
        return semaphore_timedwait_signal_trap(wait_semaphore,
                                               signal_semaphore,
                                               wait_time.tv_sec,
                                               wait_time.tv_nsec);
}
