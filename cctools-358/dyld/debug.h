/*
 * Copyright (c) 1999 Apple Computer, Inc. All rights reserved.
 *
 * @APPLE_LICENSE_HEADER_START@
 * 
 * Portions Copyright (c) 1999 Apple Computer, Inc.  All Rights
 * Reserved.  This file contains Original Code and/or Modifications of
 * Original Code as defined in and that are subject to the Apple Public
 * Source License Version 1.1 (the "License").  You may not use this file
 * except in compliance with the License.  Please obtain a copy of the
 * License at http://www.apple.com/publicsource and read it before using
 * this file.
 * 
 * The Original Code and all software distributed under the License are
 * distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY KIND, EITHER
 * EXPRESS OR IMPLIED, AND APPLE HEREBY DISCLAIMS ALL SUCH WARRANTIES,
 * INCLUDING WITHOUT LIMITATION, ANY WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE OR NON- INFRINGEMENT.  Please see the
 * License for the specific language governing rights and limitations
 * under the License.
 * 
 * @APPLE_LICENSE_HEADER_END@
 */
#import <stuff/bool.h>
#import <mach-o/dyld_debug.h>

extern mach_port_t debug_port;
extern mach_port_t debug_thread;
extern volatile enum bool debug_thread_waiting_for_lock;
extern volatile enum bool lock_in_multiply_defined_handler;
extern enum bool dyld_debug_trace;

extern void multiply_defined_enter(
    void);
extern void multiply_defined_exit(
    void);
extern void linkedit_error_enter(
    void);
extern void start_debug_thread(
    enum bool core_image);
extern void send_event(
    struct dyld_event *event);
