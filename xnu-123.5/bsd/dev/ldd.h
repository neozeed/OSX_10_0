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
/*	@(#)ldd.h	2.0	03/20/90	(c) 1990 NeXT	
 *
 * ldd.h - kernel prototypes used by loadable device drivers
 *
 * HISTORY
 * 22-May-91	Gregg Kellogg (gk) at NeXT
 *	Split out public interface.
 *
 * 16-Aug-90  Gregg Kellogg (gk) at NeXT
 *	Removed a lot of stuff that's defined in other header files. 
 *	Eventually this file should either go away or contain only imports of
 *	other files.
 *
 * 20-Mar-90	Doug Mitchell at NeXT
 *	Created.
 *
 */

#ifndef	_BSD_DEV_LDD_PRIV_
#define _BSD_DEV_LDD_PRIV_

#include <sys/cdefs.h>
#include <dev/disk.h>

typedef int (*PFI)();

#endif	/* _BSD_DEV_LDD_PRIV_ */

