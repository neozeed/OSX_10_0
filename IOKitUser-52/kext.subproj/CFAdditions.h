/*
 * Copyright (c) 1999-2000 Apple Computer, Inc. All rights reserved.
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
 * HISTORY
 *
 */

#ifndef _CFADDITIONS_H_
#define _CFADDITIONS_H_

#include <CoreFoundation/CoreFoundation.h>

Boolean CFDictionaryMatch(CFDictionaryRef dict1, CFDictionaryRef dict2, CFTypeRef match);

void CFArrayMergeArray(CFMutableArrayRef array1, CFArrayRef array2);

CFDataRef CFStringCreateCStringData(CFAllocatorRef allocator, CFStringRef string, CFStringEncoding encoding);

#endif _CFADDITIONS_H_


