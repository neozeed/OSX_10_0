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
	File:		HIDMaxUsageListLength.c
*/

#include "HIDLib.h"

/*
 *------------------------------------------------------------------------------
 *
 * HIDMaxUsageListLength
 *
 *	 Input:
 *			  reportType		   - HIDP_Input, HIDP_Output, HIDP_Feature
 *			  usagePage			   - Page Criteria or zero
 *			  ptPreparsedData		- Pre-Parsed Data
 *	 Output:
 *	 Returns: length of list
 *
 *------------------------------------------------------------------------------
*/
UInt32
HIDMaxUsageListLength	   (HIDReportType reportType,
							HIDUsage usagePage,
							HIDPreparsedDataRef preparsedDataRef)
{
#pragma unused(usagePage)	// not used, see comment below

	HIDPreparsedDataPtr ptPreparsedData = (HIDPreparsedDataPtr) preparsedDataRef;
	HIDReportItem *ptReportItem;
	int iButtons;
	int i;

	
/*
 *	Disallow Null Pointers
*/
	if (ptPreparsedData == NULL)
		return 0;
	if (ptPreparsedData->hidTypeIfValid != kHIDOSType)
		return kHIDInvalidPreparsedDataErr;
/*
 *	Go through the ReportItems
 *	Filter on ReportType
 *	Sum the button counts
 *
 * NOTE: A more precise value for the maximum list length
 *		 may be obtained by filtering out the usages that
 *		 are not on the specified usage page.  Most of
 *		 the time the number returned below is the same
 *		 as that returned by filtering usages.	It is
 *		 never smaller.	 The tradeoff is sometimes wasting
 *		 a few words of RAM in exchange for speed.
*/
	iButtons = 0;
	for (i=0; i<ptPreparsedData->reportItemCount; i++)
	{
		ptReportItem = &ptPreparsedData->reportItems[i];
		if ((ptReportItem->reportType == reportType)
		 && HIDIsButton(ptReportItem, preparsedDataRef))
			iButtons += ptReportItem->globals.reportCount;
	}
	return iButtons;
}
