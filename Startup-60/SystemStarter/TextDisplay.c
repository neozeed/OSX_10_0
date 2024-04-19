/**
 * StartupDisplay.c - Show Boot Status via the console
 * Wilfredo Sanchez | wsanchez@apple.com
 * $Apple$
 **
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
 **
 * Draws status text by printing to the console.
 * This also serves as the default built-in display plug-in, which
 * checks for and loads other plug-ins.
 **/

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <mach-o/dyld.h>
#include "Log.h"
#include "main.h"

/*
 * This is an opaque (void *) pointer in StarupDisplay.h.
 * The following definition is private to this file.
 */
typedef struct _Text_DisplayContext {
} *DisplayContext;

/*
 * Pointers to functions to call in loaded display plug-in.
 */
static void* (*fInitDisplayContext)(void) = NULL;
static void  (*fFreeDisplayContext)(DisplayContext) = NULL;
static int   (*fDisplayStatus     )(DisplayContext, CFStringRef, float) = NULL;

#define _StartupDisplay_C_
#include "StartupDisplay.h"

#define getSymbol(aSymbolName, aPointer) \
{ NSSymbol aSymbol = NSLookupSymbolInModule(aModule, aSymbolName); aPointer = NSAddressOfSymbol(aSymbol); }

/*
 * This function will walk through a list of plugin names to try to load.
 * If it succeeds on any of them, it will set the display function pointers up so that
 * we use them instead of the text drawing code.
 * The first plugin in the list to load is used.
 */
static void autoLoadDisplayPlugIn()
{
    /* It would be swell if I could just use CFPlugIn */

    /* FIXME: This list should be in a plist. */
    char* aPluginList[] = { "QuartzDisplay", "X11Display", NULL };
    char* aPlugin;
    int   aPluginIndex;

    for (aPluginIndex = 0; (aPlugin = aPluginList[aPluginIndex]); aPluginIndex++)
    {
        NSModule          aModule = NULL;
        NSObjectFileImage anImage;

        char* aBundlePath = (char*)malloc(strlen(kBundleDirectory) + 1 + /* Path to bundle dir   */
                                          (strlen(aPlugin)*2)          + /* <name>.bundle/<name> */
					  strlen(kBundleExtension) + 2); /* .bundle/ + null      */

        sprintf(aBundlePath, "%s/%s.%s/%s", kBundleDirectory, aPlugin, kBundleExtension, aPlugin);

        debug(CFSTR("Trying plugin %s..."), aBundlePath);

        if (NSCreateObjectFileImageFromFile(aBundlePath, &anImage) != NSObjectFileImageSuccess ||
            !(aModule = NSLinkModule(anImage, "Display", NSLINKMODULE_OPTION_PRIVATE        |
                                                         NSLINKMODULE_OPTION_RETURN_ON_ERROR)))
            debug(CFSTR("failed\n"));
        else
        {
            getSymbol("_initDisplayContext", fInitDisplayContext);
            getSymbol("_freeDisplayContext", fFreeDisplayContext);
            getSymbol("_displayStatus"     , fDisplayStatus     );

            if (fInitDisplayContext &&
                fFreeDisplayContext &&
                fDisplayStatus      )
            {
                debug(CFSTR("loaded\n"));
                break;
            }
            else
            {
                debug(CFSTR("failed to lookup symbols\n"));
                error(CFSTR("Load failure for possibly damaged plugin %s.\n"), aBundlePath);

                if (!NSUnLinkModule(aModule, NSUNLINKMODULE_OPTION_NONE))
                    error(CFSTR("Failed to unload symbols for busted plugin.\n"));

                /* Make sure we aren't partly initialized. */
                fInitDisplayContext = NULL;
                fFreeDisplayContext = NULL;
                fDisplayStatus      = NULL;
            }
        }
    }
}

/*
 * Default drawing routines.
 * Try function pointers if set up, else fall back to text display.
 */

DisplayContext initDisplayContext()
{
    autoLoadDisplayPlugIn();

    if (fInitDisplayContext) return fInitDisplayContext();

    {
        DisplayContext aContext = (DisplayContext)malloc(sizeof(struct _Text_DisplayContext));

        return(aContext);
    }
}

void freeDisplayContext (DisplayContext aContext)
{
    if (fFreeDisplayContext) return fFreeDisplayContext(aContext);

    if (aContext) free(aContext);
}

int displayStatus (DisplayContext aDisplayContext,
                   CFStringRef aMessage, float aPercentage)
{
    if (fDisplayStatus) return fDisplayStatus(aDisplayContext, aMessage, aPercentage);

    /**
     * Draw text.
     **/
    if (aMessage)
      {
	message(CFSTR("%@\n"), aMessage);

	return(0);
      }
    return(1);
}
