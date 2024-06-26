/*
 * Copyright (c) 1999 Apple Computer, Inc. All rights reserved.
 *
 * @APPLE_LICENSE_HEADER_START@
 * 
 * "Portions Copyright (c) 1999 Apple Computer, Inc.  All Rights
 * Reserved.  This file contains Original Code and/or Modifications of
 * Original Code as defined in and that are subject to the Apple Public
 * Source License Version 1.0 (the 'License').  You may not use this file
 * except in compliance with the License.  Please obtain a copy of the
 * License at http://www.apple.com/publicsource and read it before using
 * this file.
 * 
 * The Original Code and all software distributed under the License are
 * distributed on an 'AS IS' basis, WITHOUT WARRANTY OF ANY KIND, EITHER
 * EXPRESS OR IMPLIED, AND APPLE HEREBY DISCLAIMS ALL SUCH WARRANTIES,
 * INCLUDING WITHOUT LIMITATION, ANY WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE OR NON-INFRINGEMENT.  Please see the
 * License for the specific language governing rights and limitations
 * under the License."
 * 
 * @APPLE_LICENSE_HEADER_END@
 */
#include "SRuntime.h"

#if BSD

#include <unistd.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <dev/disk.h>	/* driver kit stuff */

#else

#include <Files.h>
#include <Device.h>
#include <Disks.h>

#endif


OSErr GetDeviceSize(int driveRefNum, UInt32 *numBlocks, UInt32 *blockSize)
{
#if BSD
	int devBlockCount = 0;
	int devBlockSize = 0;

	if (ioctl(driveRefNum, DKIOCNUMBLKS, &devBlockCount) < 0) {
		printf("ioctl(DKIOCNUMBLKS) for fd %d: %s\n", driveRefNum, strerror(errno));
		return (-1);
	}
	
	if (ioctl(driveRefNum, DKIOCBLKSIZE, &devBlockSize) < 0) {
		printf("ioctl(DKIOCBLKSIZE) for fd %d: %s\n", driveRefNum, strerror(errno));
		return (-1);
	}

	if (devBlockSize != 512) {
		devBlockCount = ((UInt64)devBlockCount * (UInt64)devBlockSize) / 512;
		devBlockSize = 512;
	}

	*numBlocks = devBlockCount;
	*blockSize = devBlockSize;

	return (0);
#else
	/* Various Mac OS device constants */
	enum
	{
		/* return format list status code */
		kFmtLstCode = 6,
		
		/* reference number of .SONY driver */
		kSonyRefNum = 0xfffb,
		
		/* values returned by DriveStatus in DrvSts.twoSideFmt */
		kSingleSided = 0,
		kDoubleSided = -1,
		kSingleSidedSize = 800,		/* 400K */
		kDoubleSidedSize = 1600,	/* 800K */
		
		/* values in DrvQEl.qType */
		kWordDrvSiz = 0,
		kLongDrvSiz = 1,
		
		/* more than enough formatListRecords */
		kMaxFormatListRecs = 16
	};
	
	ParamBlockRec	pb;
	FormatListRec	formatListRecords[kMaxFormatListRecs];
	DrvSts			status;
	short			formatListRecIndex;
	OSErr			result;
	unsigned long	blocks			= 0;

	
	/* Attempt to get the drive's format list. */
	/* (see the Technical Note "What Your Sony Drives For You") */
	
	pb.cntrlParam.ioVRefNum = driveQElementPtr->dQDrive;
	pb.cntrlParam.ioCRefNum = driveQElementPtr->dQRefNum;
	pb.cntrlParam.csCode = kFmtLstCode;
	pb.cntrlParam.csParam[0] = kMaxFormatListRecs;
	*(long *)&pb.cntrlParam.csParam[1] = (long)&formatListRecords[0];
	
	result = PBStatusSync(&pb);
	
	if ( result == noErr )
	{
		/* The drive supports ReturnFormatList status call. */
		
		/* Get the current disk's size. */
		for( formatListRecIndex = 0;
			 formatListRecIndex < pb.cntrlParam.csParam[0];
			 ++formatListRecIndex )
		{
			if ( (formatListRecords[formatListRecIndex].formatFlags &
				  diCIFmtFlagsCurrentMask) != 0 )
			{
				blocks = formatListRecords[formatListRecIndex].volSize;
			}
		}
		if ( blocks == 0 )
		{
			/* This should never happen */
			result = paramErr;
		}
	}
	else if ( driveQElementPtr->dQRefNum == (short)kSonyRefNum )
	{
		/* The drive is a non-SuperDrive floppy which only supports 400K and 800K disks */
		
		result = DriveStatus(driveQElementPtr->dQDrive, &status);
		if ( result == noErr )
		{
			switch ( status.twoSideFmt )
			{
				case kSingleSided:
					blocks = kSingleSidedSize;
					break;
					
				case kDoubleSided:
					blocks = kDoubleSidedSize;
					break;
					
				default:		//	This should never happen
					result = paramErr;
					break;
			}
		}
	}
	else
	{
		/* The drive is not a floppy and it doesn't support ReturnFormatList */
		/* so use the dQDrvSz field(s) */
		
		result = noErr;	/* reset result */
		
		switch ( driveQElementPtr->qType )
		{
			case kWordDrvSiz:
				blocks = driveQElementPtr->dQDrvSz;
				break;
				
			case kLongDrvSiz:
				blocks = ((unsigned long)driveQElementPtr->dQDrvSz2 << 16) +
						 driveQElementPtr->dQDrvSz;
				break;
				
			default:		//	This should never happen
				result = paramErr;
				break;
		}
	}

	*numBlocks = blocks;
	*blockSize = 512;
	
	return( result );
#endif
}

off_t io_bytes_read = 0;
int io_count = 0;

void printIOstats(void)
{
	printf("IO Summary:  %d I/O calls, for %qd bytes\n", io_count, io_bytes_read);
}


OSErr DeviceRead(int device, int drive, void* buffer, SInt64 offset, UInt32 reqBytes, UInt32 *actBytes)
{
#if BSD
	off_t seek_off;
	ssize_t	nbytes;
	
	*actBytes = 0;

	seek_off = lseek(device, offset, SEEK_SET);
	if (seek_off == -1) {
		printf("# DeviceRead: lseek(%qd) failed with %d\n", offset, errno);
		return (errno);
	}

	nbytes = read(device, buffer, reqBytes);
	if (nbytes == -1)
		return (errno);
	if (nbytes == 0) {
		printf("CANNOT READ: BLK %ld", (long)offset/512);
		return (5);
	}

	io_count++;
	io_bytes_read += nbytes;

	*actBytes = nbytes;
	return (0);

#else
	OSErr err;
	XIOParam pb;

	pb.ioVRefNum	= drive;
	pb.ioRefNum	= device;
	pb.ioPosMode	= fsFromStart;
	pb.ioReqCount	= reqBytes;
	pb.ioBuffer	= buffer;

	if ( (offset & 0xFFFFFFFF00000000) != 0 )
	{
		*(SInt64*)&pb.ioWPosOffset = offset;
		pb.ioPosMode |= (1 << kWidePosOffsetBit);
	}
	else
	{
		((IOParam*)&pb)->ioPosOffset = offset;
	}

	err = PBReadSync( (ParamBlockRec *)&pb );

	return (err);
#endif
}


OSErr DeviceWrite(int device, int drive, void* buffer, SInt64 offset, UInt32 reqBytes, UInt32 *actBytes)
{
#if BSD
	off_t seek_off;
	ssize_t	nbytes;

	*actBytes = 0;

	seek_off = lseek(device, offset, SEEK_SET);
	if (seek_off == -1) {
		printf("# DeviceRead: lseek(%qd) failed with %d\n", offset, errno);
		return (errno);
	}

	nbytes = write(device, buffer, reqBytes);
	if (nbytes == -1) {
		return (errno);
	}
	if (nbytes == 0) {
		printf("CANNOT WRITE: BLK %ld", (long)offset/512);
		return (5);
	}

	*actBytes = nbytes;
	return (0);
#else
	OSErr err;
	XIOParam pb;

	pb.ioVRefNum	= drive;
	pb.ioRefNum	= device;
	pb.ioPosMode	= fsFromStart;
	pb.ioReqCount	= reqBytes;
	pb.ioBuffer	= buffer;

	if ( (offset & 0xFFFFFFFF00000000) != 0 )
	{
		*(SInt64*)&pb.ioWPosOffset = offset;
		pb.ioPosMode |= (1 << kWidePosOffsetBit);
	}
	else
	{
		((IOParam*)&pb)->ioPosOffset = offset;
	}

	err = PBWriteSync( (ParamBlockRec *)&pb );

	return (err);
#endif
}
