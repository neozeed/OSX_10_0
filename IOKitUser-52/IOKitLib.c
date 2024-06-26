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

#define CFRUNLOOP_NEW_API 1

#include <mach/mach.h>

#include <stdlib.h>
#include <stdarg.h>

#include <CoreFoundation/CoreFoundation.h>
#include <CoreFoundation/CFMachPort.h>

#include <IOKit/IOBSD.h>
#include <IOKit/IOKitLib.h>
#include <IOKit/IOKitServer.h>

#include <IOKit/IOCFSerialize.h>
#include <IOKit/IOCFUnserialize.h>

#include <IOKit/iokitmig.h> 	// mig generated
#include <IOKit/IOKitLibPrivate.h>


/*
 * Ports
 */

extern kern_return_t
bootstrap_ports(mach_port_t bootstrap,
                mach_port_t *priv_host,
                mach_port_t *priv_device,
                mach_port_t *wired_ledger,
                mach_port_t *paged_ledger,
                mach_port_t *host_security );

extern 	mach_port_t 	mach_task_self();

kern_return_t
IOMasterPort( mach_port_t bootstrapPort,
		mach_port_t * masterPort )
{
    return host_get_io_master(mach_host_self(), masterPort);
}

kern_return_t
IOCreateReceivePort( int msgType, mach_port_t * recvPort )
{
    kern_return_t res;
    switch (msgType) {
        case kOSNotificationMessageID:
        case kOSAsyncCompleteMessageID:
            res = mach_port_allocate(mach_task_self(),
                                     MACH_PORT_RIGHT_RECEIVE, recvPort);

            break;

        default:
            res = kIOReturnBadArgument;
    }
    return res;
}

/*
 * IOObject
 */

kern_return_t
IOObjectRelease(
	io_object_t	object )
{
    return( mach_port_deallocate( mach_task_self(), object ));
}

kern_return_t
IOObjectGetClass(
	io_object_t	object,
	io_name_t       className )
{
    return( io_object_get_class( object, className ));
}

boolean_t
IOObjectConformsTo(
	io_object_t	object,
	const io_name_t	className )
{
    boolean_t	conforms;

    if( kIOReturnSuccess != io_object_conforms_to(
		object, (char *) className, &conforms ))
	conforms = 0;

    return( conforms );
}

boolean_t
IOObjectIsEqualTo(
	io_object_t	object,
	io_object_t	anObject )
{
    return( object == anObject );
}

int
IOObjectGetRetainCount(
	io_object_t	object )
{
    int	count;

    if( kIOReturnSuccess != io_object_get_retain_count( object, &count))
	count = 0;

    return( count );
}


/*
 * IOIterator
 */

io_object_t
IOIteratorNext(
	io_iterator_t	iterator )
{
    io_object_t	next;

    if( kIOReturnSuccess != io_iterator_next( iterator, &next))
	next = 0;

    return( next );
}

void
IOIteratorReset(
	io_iterator_t	iterator )
{
    io_iterator_reset( iterator );
}

boolean_t
IOIteratorIsValid(
	io_iterator_t	iterator )
{
    boolean_t	valid;

    if( kIOReturnSuccess != io_iterator_is_valid( iterator, &valid ))
	valid = FALSE;

    return( valid );
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

/*
 * IOService
 */

kern_return_t
IOServiceGetMatchingServices(
        mach_port_t	masterPort,
	CFDictionaryRef	matching,
	io_iterator_t * existing )
{
    kern_return_t	kr;
    CFDataRef		data;

    if( !matching)
	return( kIOReturnBadArgument);

    data = IOCFSerialize( matching, kNilOptions );
    if( !data)
	return( kIOReturnUnsupported );

    kr = io_service_get_matching_services( masterPort,
		(char *) CFDataGetBytePtr(data), existing );

    CFRelease( data );
    CFRelease( matching );

    return( kr );
}

kern_return_t
IOServiceMatchPropertyTable( io_service_t service, CFDictionaryRef matching,
                  boolean_t * matches )
{
    kern_return_t	kr;
    CFDataRef		data;

    if( !matching)
	return( kIOReturnBadArgument);

    data = IOCFSerialize( matching, kNilOptions );
    if( !data)
	return( kIOReturnUnsupported );

    kr = io_service_match_property_table( service,
		(char *) CFDataGetBytePtr(data), matches );

    CFRelease( data );

    return( kr );
}

kern_return_t
IOServiceAddNotification(
        mach_port_t	masterPort,
	const io_name_t	notificationType,
	CFDictionaryRef	matching,
	mach_port_t	wakePort,
	int		reference,
	io_iterator_t * notification )
{
    kern_return_t	kr;
    CFDataRef		data;

    if( !matching)
	return( kIOReturnBadArgument);

    data = IOCFSerialize( matching, kNilOptions );
    if( !data)
	return( kIOReturnUnsupported );

    kr = io_service_add_notification( masterPort, (char *) notificationType,
		(char *) CFDataGetBytePtr(data),
		wakePort, &reference, 1, notification );

    CFRelease( data );
    CFRelease( matching );

    return( kr );
}

kern_return_t
IOServiceAddMatchingNotification(
	IONotificationPortRef	notifyPort,
	const io_name_t		notificationType,
	CFDictionaryRef		matching,
        IOServiceMatchingCallback callback,
        void *			refcon,
	io_iterator_t * 	notification )
{
    kern_return_t	kr;
    CFDataRef		data;
    natural_t		asyncRef[kIOMatchingCalloutCount];

    if( !matching)
	return( kIOReturnBadArgument);

    data = IOCFSerialize( matching, kNilOptions );
    if( !data)
	return( kIOReturnUnsupported );

    asyncRef[kIOMatchingCalloutFuncIndex] = (natural_t) callback;
    asyncRef[kIOMatchingCalloutRefconIndex] = (natural_t) refcon;
    
    kr = io_service_add_notification( notifyPort->masterPort,
                (char *) notificationType,
		(char *) CFDataGetBytePtr(data),
		notifyPort->wakePort,
                asyncRef, kIOMatchingCalloutCount, notification );

    CFRelease( data );
    CFRelease( matching );

    return( kr );
}

kern_return_t
IOServiceAddInterestNotification(
	IONotificationPortRef	notifyPort,
        io_service_t		service,
	const io_name_t 	interestType,
        IOServiceInterestCallback callback,
        void *			refcon,
        io_object_t *		notification )
{
    natural_t		asyncRef[kIOInterestCalloutCount];
    kern_return_t	kr;

    asyncRef[kIOInterestCalloutFuncIndex] = (natural_t) callback;
    asyncRef[kIOInterestCalloutRefconIndex] = (natural_t) refcon;
    asyncRef[kIOInterestCalloutServiceIndex] = (natural_t) service;
    
    kr = io_service_add_interest_notification( service, (char *) interestType,
                            notifyPort->wakePort,
                            asyncRef, kIOInterestCalloutCount,
                            notification );

    return( kr );
}

IONotificationPortRef
IONotificationPortCreate(
	mach_port_t	masterPort )
{
    kern_return_t 	 kr;
    IONotificationPort * notify;
    
    notify = malloc( sizeof( IONotificationPort));
    if( !notify)
        return( 0 );

    bzero( notify, sizeof( IONotificationPort));
    notify->masterPort = masterPort;

    kr = IOCreateReceivePort(kOSNotificationMessageID, &notify->wakePort);
    if( kr != kIOReturnSuccess) {
        free( notify );
        return( 0 );
    }
    
    return notify;
}

void
IONotificationPortDestroy(
	IONotificationPortRef	notify )
{
    if( notify->source)
        CFRelease( notify->source);

    mach_port_destroy( mach_task_self(), notify->wakePort);

    free( notify );
}

CFRunLoopSourceRef
IONotificationPortGetRunLoopSource(
	IONotificationPortRef	notify )
{
    CFMachPortRef 	cfPort;
    CFMachPortContext 	context;
    CFRunLoopSourceRef	cfSource;
    Boolean		shouldFreeInfo;

    if( notify->source)
        return( notify->source );
    
    context.version = 1;
    context.info = (void *) notify;
    context.retain = NULL;
    context.release = NULL;
    context.copyDescription = NULL;

    cfPort = CFMachPortCreateWithPort(NULL, notify->wakePort,
        IODispatchCalloutFromCFMessage, &context, &shouldFreeInfo);
    if (!cfPort)
        return( 0 );
    
    cfSource = CFMachPortCreateRunLoopSource(NULL, cfPort, 0);
    CFRelease(cfPort);
    notify->source = cfSource;
    return( cfSource );
}

mach_port_t
IONotificationPortGetMachPort(
	IONotificationPortRef	notify )
{
    return( notify->wakePort );
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

/*
 * Matching creation helpers
 */

CFMutableDictionaryRef
IOMakeMatching(
        mach_port_t	masterPort,
	unsigned int	type,
	unsigned int	options,
	void *		args,
	unsigned int	argsSize )
{
    IOReturn			err;
    CFMutableDictionaryRef	result = 0;
    char * 			matching;

    matching = (char *) malloc( sizeof( io_string_t));

    if( matching) {
        err = io_make_matching( masterPort, type, options,
                    (char *)args, argsSize, matching );
        if( KERN_SUCCESS == err) {
	    result = (CFMutableDictionaryRef) IOCFUnserialize( matching,
				kCFAllocatorDefault, kNilOptions, 0 );
	}
        free( matching );
    }

    return( result );
}

static CFMutableDictionaryRef
MakeOneStringProp(
	CFStringRef	key,
	const char *	name )
{
    CFMutableDictionaryRef	dict;
    CFStringRef			string;

    dict = CFDictionaryCreateMutable( kCFAllocatorDefault, 0,
		&kCFTypeDictionaryKeyCallBacks,
		&kCFTypeDictionaryValueCallBacks);

    if( !dict)
	return( dict);

    string = CFStringCreateWithCString( kCFAllocatorDefault, name,
					kCFStringEncodingMacRoman );

    if( string) {
        CFDictionarySetValue( dict, key, string );
        CFRelease( string );
    } else {
	CFRelease( dict );
	dict = 0;
    }

    return( dict );
}

CFMutableDictionaryRef
IOServiceMatching(
	const char *	name )
{
    return( MakeOneStringProp( CFSTR(kIOProviderClassKey), name ) );
}

CFMutableDictionaryRef
IOServiceNameMatching(
	const char *	name )
{
    return( MakeOneStringProp( CFSTR(kIONameMatchKey), name ));
}

CFMutableDictionaryRef
IOBSDNameMatching(
        mach_port_t	masterPort,
	unsigned int	options,
	const char *	name )
{
    return( IOMakeMatching( masterPort, kIOBSDNameMatching, options,
				(void *)name, strlen( name) + 1));
}

CFMutableDictionaryRef
IOOpenFirmwarePathMatching(
        mach_port_t	masterPort,
	unsigned int	options,
	const char *	path )
{
    return( IOMakeMatching( masterPort, kIOOFPathMatching, options,
				(void *)path, strlen( path) + 1));
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

kern_return_t
OSGetNotificationFromMessage(
	mach_msg_header_t     *	msg,
	unsigned int	  	index,
        unsigned long int     *	type,
        unsigned long int     *	reference,
	void		     ** content,
        vm_size_t	      *	size )
{
    OSNotificationHeader *	header;

    if( msg->msgh_id != kOSNotificationMessageID)
	return( kIOReturnBadMessageID );

    if( msg->msgh_size < (sizeof( mach_msg_header_t)
			+ sizeof( OSNotificationHeader)))
	return( kIOReturnNoResources );

    if( index)
	return( kIOReturnNoResources );

    header = (OSNotificationHeader *) (msg + 1);
    if( type)
        *type = header->type;
    if( reference)
        *reference = header->reference[0];
    if( size)
        *size = header->size;
    if( content) {
	if( header->size)
            *content = &header->content[0];
        else
            *content = 0;
    }

    return( kIOReturnSuccess );
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

void
IODispatchCalloutFromMessage(void *cfPort, mach_msg_header_t *msg, void *info)
{
    return( IODispatchCalloutFromCFMessage( cfPort, msg, -1, info ));
}

void
IODispatchCalloutFromCFMessage(CFMachPortRef port, void *_msg, CFIndex size, void *info)
{
    int leftOver;
    mach_msg_header_t *msg = (mach_msg_header_t *)_msg;
    OSNotificationHeader *header;
    
    if( msg->msgh_id != kOSNotificationMessageID)
	return;
    header = (OSNotificationHeader *) (msg + 1);
    
    switch( header->type ) {
        
      case kIOAsyncCompletionNotificationType:
      {
        IOAsyncCompletionContent *asyncHdr;
          
        asyncHdr = (IOAsyncCompletionContent *)(header + 1);
        leftOver = (msg->msgh_size
                    - sizeof(*msg)
                    - sizeof(*header)
                    - sizeof(*asyncHdr) ) / sizeof(void *);
        switch (leftOver) {
            case 0:
                ((IOAsyncCallback0 )header->reference[kIOAsyncCalloutFuncIndex])(
                    (void *) header->reference[kIOAsyncCalloutRefconIndex],
                    asyncHdr->result);
                break;
            case 1:
                ((IOAsyncCallback1 )header->reference[kIOAsyncCalloutFuncIndex])(
                    (void *) header->reference[kIOAsyncCalloutRefconIndex],
                    asyncHdr->result,
                    asyncHdr->args[0]);
                break;
            case 2:
                ((IOAsyncCallback2 )header->reference[kIOAsyncCalloutFuncIndex])(
                    (void *) header->reference[kIOAsyncCalloutRefconIndex],
                    asyncHdr->result,
                    asyncHdr->args[0], asyncHdr->args[1]);
                break;
            default:
                ((IOAsyncCallback )header->reference[kIOAsyncCalloutFuncIndex])(
                    (void *) header->reference[kIOAsyncCalloutRefconIndex],
                    asyncHdr->result,
                    asyncHdr->args, leftOver);
                break;
        }
        break;
      }          
      case kIOServiceMessageNotificationType:
      {    
        IOServiceInterestContent * interestHdr;
        void * arg;
        
        interestHdr = (IOServiceInterestContent *)(header + 1);
        leftOver = (msg->msgh_size
                    - sizeof(*msg)
                    - sizeof(*header)
                    - sizeof(*interestHdr) ) / sizeof(void *);
        if (leftOver <= 1)
            arg = interestHdr->messageArgument[0];
        else
            arg = &interestHdr->messageArgument[0];
        ((IOServiceInterestCallback)header->reference[kIOInterestCalloutFuncIndex])(
            (void *) header->reference[kIOInterestCalloutRefconIndex],
            (io_service_t) header->reference[kIOInterestCalloutServiceIndex],
            interestHdr->messageType, arg );
        break;
          
      case kIOServicePublishNotificationType:
      case kIOServiceMatchedNotificationType:
      case kIOServiceTerminatedNotificationType:
          
        ((IOServiceMatchingCallback)header->reference[kIOMatchingCalloutFuncIndex])(
            (void *) header->reference[kIOMatchingCalloutRefconIndex],
            // remote port is the notification (an iterator_t) that fired
            (io_iterator_t) msg->msgh_remote_port);
        break;
      }
    }
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

kern_return_t
IOServiceGetBusyState(
	io_service_t    service,
	int *		busyState )
{
    kern_return_t	kr;

    kr = io_service_get_busy_state( service, busyState );

    if( kr != KERN_SUCCESS)
	*busyState = 0;

    return( kr );
}

kern_return_t
IOKitGetBusyState(
        mach_port_t	masterPort,
	int *		busyState )
{
    io_service_t 	root;
    kern_return_t	kr;

    kr = io_registry_entry_from_path( masterPort,
			kIOServicePlane ":/", &root );

    if( kr == KERN_SUCCESS) {
	kr = io_service_get_busy_state( root, busyState );
	IOObjectRelease( root );

    } else
	*busyState = 0;

    return( kr );
}

kern_return_t
IOServiceWaitQuiet(
	io_service_t      service,
	mach_timespec_t * waitTime )
{
    kern_return_t	kr;
    mach_timespec_t	defaultWait = { 0, -1 };

    if( 0 == waitTime)
        waitTime = &defaultWait;

    kr = io_service_wait_quiet( service, *waitTime );

    return( kr );
}


kern_return_t
IOKitWaitQuiet(
        mach_port_t	masterPort,
	mach_timespec_t * waitTime )
{
    io_service_t 	root;
    kern_return_t	kr;
    mach_timespec_t	defaultWait = { 0, -1 };

    kr = io_registry_entry_from_path( masterPort,
			kIOServicePlane ":/", &root );

    if( kr == KERN_SUCCESS) {
	if( 0 == waitTime)
	    waitTime = &defaultWait;
	kr = io_service_wait_quiet( root, *waitTime );
	IOObjectRelease( root );
    }

    return( kr );
}

kern_return_t
IOServiceOpen(
	io_service_t    service,
	task_port_t	owningTask,
	unsigned int	type,
	io_connect_t  *	connect )
{
    return( io_service_open( service,
	owningTask, type, connect ));
}

kern_return_t
IOServiceClose(
	io_connect_t	connect )
{
    kern_return_t	kr;

    kr = io_service_close( connect);
    if( kr == KERN_SUCCESS)
	IOObjectRelease( connect );

    return( kr );
}

kern_return_t
IOServiceRequestProbe(
	io_service_t    service,
	unsigned int	options )
{
    return( io_service_request_probe( service, options ));
}

/*
 * IOService connection
 */
kern_return_t
IOConnectAddRef(
	io_connect_t	connect )
{
    return mach_port_mod_refs(mach_task_self(),
                              connect,
                              MACH_PORT_RIGHT_SEND,
                              1);
}

kern_return_t
IOConnectRelease(
	io_connect_t	connect )
{
    // @@@ gvdl: Check with Simon about last reference removal
    return mach_port_mod_refs(mach_task_self(),
                              connect,
                              MACH_PORT_RIGHT_SEND,
                              -1);
}

kern_return_t
IOConnectGetService(
	io_connect_t	connect,
	io_service_t  *	service )
{
    return( io_connect_get_service( connect, service ));
}

kern_return_t
IOConnectSetNotificationPort(
	io_connect_t	connect,
	unsigned int	type,
	mach_port_t	port,
	unsigned int	reference )
{
    return( io_connect_set_notification_port( connect,
		type, port, reference));
}

kern_return_t
IOConnectMapMemory(
	mach_port_t	connect,
	unsigned int	memoryType,
	task_port_t	intoTask,
	vm_address_t *	atAddress,
	vm_size_t    *	ofSize,
	IOOptionBits	options )
{
    return( io_connect_map_memory( connect, memoryType, intoTask,
	atAddress, ofSize, options));
}

kern_return_t
IOConnectUnmapMemory(
	mach_port_t	connect,
	unsigned int	memoryType,
	task_port_t	intoTask,
	vm_address_t	atAddress )
{
    return( io_connect_unmap_memory( connect, memoryType, intoTask,
	atAddress));
}

kern_return_t
IOConnectAddClient(
	io_connect_t	connect,
	io_connect_t	client )
{
    return( io_connect_add_client( connect, client));
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

kern_return_t
IOConnectMethodScalarIScalarO( 
	io_connect_t	connect,
        unsigned int	index,
        IOItemCount	scalarInputCount,
        IOItemCount	scalarOutputCount,
        ... )
{
    int			params[8];
    int * 		input;
    int ** 		output;
    kern_return_t	err;
    int			i;

    input = (int *) __builtin_next_arg( scalarOutputCount );
    output = (int **)(input + scalarInputCount);

    err = io_connect_method_scalarI_scalarO( connect, index,
                    input, scalarInputCount,
                    params, (unsigned int *) &scalarOutputCount);

    if( kIOReturnSuccess == err) {
        for( i = 0; i < scalarOutputCount; i++)
            *output[i] = params[i];
    }

    return( err );
}

kern_return_t
IOConnectMethodScalarIStructureO(
	io_connect_t	connect,
        unsigned int	index,
        IOItemCount	scalarInputCount,
        IOByteCount *	structureSize,
        ... )
{
    int * 		input;
    char *		structure;
    kern_return_t	err;

    input = (int *) __builtin_next_arg( structureSize );
    structure = (char *) input[scalarInputCount];

    err = io_connect_method_scalarI_structureO( connect, index,
                    input, scalarInputCount,
                    structure, (unsigned int *) structureSize);

    return( err );
}

kern_return_t
IOConnectMethodScalarIStructureI(
	io_connect_t	connect,
        unsigned int	index,
        IOItemCount	scalarInputCount,
        IOByteCount	structureSize,
        ... )
{
    int * 		input;
    char *		structure;
    kern_return_t	err;

    input = (int *) __builtin_next_arg( structureSize );
    structure = (char *) input[scalarInputCount];

    err = io_connect_method_scalarI_structureI( connect, index,
                    input, scalarInputCount,
                    structure, structureSize );

    return( err );
}

kern_return_t
IOConnectMethodStructureIStructureO(
	io_connect_t	connect,
        unsigned int	index,
        IOItemCount	structureInputSize,
        IOByteCount *	structureOutputSize,
        void *		inputStructure,
        void *		ouputStructure )
{
    return( io_connect_method_structureI_structureO( connect, index,
                    inputStructure, structureInputSize,
                    ouputStructure, (unsigned int *) structureOutputSize ));
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

kern_return_t
IOConnectTrap0(
        io_connect_t	connect,
        unsigned int	index )
{
    return iokit_user_client_trap(connect, index, NULL, NULL, NULL, NULL, NULL, NULL);
}

kern_return_t
IOConnectTrap1(
        io_connect_t	connect,
        unsigned int	index,
        void 		*p1 )
{
    return iokit_user_client_trap(connect, index, p1, NULL, NULL, NULL, NULL, NULL);
}

kern_return_t
IOConnectTrap2(
        io_connect_t	connect,
        unsigned int	index,
        void 		*p1,
        void 		*p2 )
{
    return iokit_user_client_trap(connect, index, p1, p2, NULL, NULL, NULL, NULL);
}

kern_return_t
IOConnectTrap3(
        io_connect_t	connect,
        unsigned int	index,
        void 		*p1,
        void 		*p2,
        void		*p3 )
{
    return iokit_user_client_trap(connect, index, p1, p2, p3, NULL, NULL, NULL);
}

kern_return_t
IOConnectTrap4(
        io_connect_t	connect,
        unsigned int	index,
        void 		*p1,
        void 		*p2,
        void		*p3,
        void		*p4 )
{
    return iokit_user_client_trap(connect, index, p1, p2, p3, p4, NULL, NULL);
}

kern_return_t
IOConnectTrap5(
        io_connect_t	connect,
        unsigned int	index,
        void 		*p1,
        void 		*p2,
        void		*p3,
        void		*p4,
        void		*p5 )
{
    return iokit_user_client_trap(connect, index, p1, p2, p3, p4, p5, NULL);
}

kern_return_t
IOConnectTrap6(
        io_connect_t	connect,
        unsigned int	index,
        void 		*p1,
        void 		*p2,
        void		*p3,
        void		*p4,
        void		*p5,
        void		*p6 )
{
    return iokit_user_client_trap(connect, index, p1, p2, p3, p4, p5, p6);
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

kern_return_t
IOConnectSetCFProperties(
	io_connect_t	connect,
	CFTypeRef	properties )
{
    CFDataRef		data;
    kern_return_t	kr;
    kern_return_t	result;

    data = IOCFSerialize( properties, kNilOptions );
    if( !data)
	return( kIOReturnUnsupported );

    kr = io_connect_set_properties( connect,
            (UInt8 *) CFDataGetBytePtr(data), CFDataGetLength(data),
            &result );

    CFRelease(data);

    if( KERN_SUCCESS == kr)
        kr = result;

    return( kr );
}

kern_return_t
IOConnectSetCFProperty(
	io_connect_t	connect,
        CFStringRef	propertyName,
	CFTypeRef	property )
{
    CFMutableDictionaryRef	dict;
    kern_return_t		kr;

    dict = CFDictionaryCreateMutable( kCFAllocatorDefault, 0,
		&kCFTypeDictionaryKeyCallBacks,
		&kCFTypeDictionaryValueCallBacks);

    if( !dict)
	return( kIOReturnNoMemory );

    CFDictionarySetValue( dict, propertyName, property );
    kr = IOConnectSetCFProperties( connect, dict );
    CFRelease( dict );

    return( kr );
}


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

/*
 * IORegistry accessors
 */

kern_return_t
IORegistryCreateIterator(
        mach_port_t	masterPort,
	const io_name_t	plane,
	IOOptionBits	options,
	io_iterator_t * iterator )
{
    return( io_registry_create_iterator( masterPort, (char *) plane,
		options, iterator));
}

kern_return_t
IORegistryEntryCreateIterator(
	io_registry_entry_t	entry,
	const io_name_t		plane,
	IOOptionBits		options,
	io_iterator_t 	      * iterator )
{
    return( io_registry_entry_create_iterator( entry, (char *) plane,
		options, iterator));
}

kern_return_t
IORegistryIteratorEnterEntry(
	io_iterator_t	iterator )
{
    return( io_registry_iterator_enter_entry( iterator));
}


kern_return_t
IORegistryIteratorExitEntry(
	io_iterator_t	iterator )
{
    return( io_registry_iterator_exit_entry( iterator));
}

io_registry_entry_t
IORegistryEntryFromPath(
        mach_port_t		masterPort,
	const io_string_t	path )
{
    kern_return_t	kr;
    io_registry_entry_t	entry;

    kr = io_registry_entry_from_path( masterPort, (char *) path, &entry );
    if( kIOReturnSuccess != kr)
	entry = 0;

    return( entry );
}

io_registry_entry_t
IORegistryGetRootEntry(
        mach_port_t	masterPort )
{
    kern_return_t	kr;
    io_registry_entry_t	entry;

    kr = io_registry_get_root_entry( masterPort, &entry );
    if( kIOReturnSuccess != kr)
	entry = 0;

    return( entry );
}

kern_return_t
IORegistryEntryGetPath(
	io_registry_entry_t	entry,
	const io_name_t		plane,
	io_string_t		path )
{
    return( io_registry_entry_get_path( entry, (char *) plane, path ));
}

boolean_t
IORegistryEntryInPlane(
	io_registry_entry_t	entry,
	const io_name_t 	plane )
{
    boolean_t		inPlane;

    if( kIOReturnSuccess != io_registry_entry_in_plane(
		entry, (char *) plane, &inPlane ))
	inPlane = false;

    return( inPlane );
}

kern_return_t
IORegistryEntryGetName(
	io_registry_entry_t	entry,
	io_name_t 	        name )
{
    return( io_registry_entry_get_name( entry, name ));
}

kern_return_t
IORegistryEntryGetNameInPlane(
	io_registry_entry_t	entry,
	const io_name_t 	plane,
	io_name_t 	        name )
{
    return( io_registry_entry_get_name_in_plane( entry,
						(char *) plane, name ));
}

kern_return_t
IORegistryEntryCreateCFProperties(
	io_registry_entry_t	entry,
	CFMutableDictionaryRef * properties,
        CFAllocatorRef		allocator,
	IOOptionBits		options )
{
    kern_return_t 	kr;
    unsigned int 	size;
    char *	 	propertiesBuffer;
    CFStringRef  	errorString;
    const char * 	cstr;

    kr = io_registry_entry_get_properties(entry, &propertiesBuffer, &size);
    if (kr != kIOReturnSuccess) return kr;

    *properties = (CFMutableDictionaryRef)
                        IOCFUnserialize(propertiesBuffer, allocator,
					0, &errorString);
    if (!(*properties)) {

        if ((cstr = CFStringGetCStringPtr(errorString,
					kCFStringEncodingMacRoman)))
            printf("%s\n", cstr);
	CFRelease(errorString);
    }

    // free propertiesBuffer !
    vm_deallocate(mach_task_self(), (vm_address_t)propertiesBuffer, size);

    return *properties ? kIOReturnSuccess : kIOReturnInternalError;
}

CFTypeRef
IORegistryEntryCreateCFProperty(
	io_registry_entry_t	entry,
	CFStringRef		key,
        CFAllocatorRef		allocator,
	IOOptionBits		options )
{
    IOReturn		kr;
    CFMutableDictionaryRef	props;
    CFTypeRef		type;

    kr = IORegistryEntryCreateCFProperties( entry, &props, allocator, options );
    if( kr != kIOReturnSuccess )
	return( 0 );

    type = CFDictionaryGetValue( props, key );
    if( type)
	CFRetain( type );
    CFRelease( props );

    return( type );
}


kern_return_t
IORegistryEntryGetProperty(
	io_registry_entry_t	entry,
	const io_name_t		name,
	io_struct_inband_t	buffer,
	unsigned int	      * size )
{
    return( io_registry_entry_get_property( entry, (char *) name,
						buffer, size ));
}


kern_return_t
IORegistryEntrySetCFProperties(
	io_registry_entry_t	entry,
	CFTypeRef	 	properties )
{
    CFDataRef		data;
    kern_return_t	kr;
    kern_return_t	result;

    data = IOCFSerialize( properties, kNilOptions );
    if( !data)
	return( kIOReturnUnsupported );

    kr = io_registry_entry_set_properties( entry,
            (UInt8 *) CFDataGetBytePtr(data), CFDataGetLength(data),
            &result );

    CFRelease(data);

    if( KERN_SUCCESS == kr)
        kr = result;

    return( kr );
}

kern_return_t
IORegistryEntrySetCFProperty(
	io_registry_entry_t	entry,
        CFStringRef		propertyName,
	CFTypeRef	 	property )
{
    CFMutableDictionaryRef	dict;
    kern_return_t		kr;

    dict = CFDictionaryCreateMutable( kCFAllocatorDefault, 0,
		&kCFTypeDictionaryKeyCallBacks,
		&kCFTypeDictionaryValueCallBacks);

    if( !dict)
	return( kIOReturnNoMemory );

    CFDictionarySetValue( dict, propertyName, property );
    kr = IORegistryEntrySetCFProperties( entry, dict );
    CFRelease( dict );

    return( kr );
}

kern_return_t
IORegistryEntryGetChildIterator(
	io_registry_entry_t	entry,
	const io_name_t		plane,
	io_iterator_t	      * iterator )
{
    return( io_registry_entry_get_child_iterator( entry,
						(char *) plane, iterator));
}

kern_return_t
IORegistryEntryGetParentIterator(
	io_registry_entry_t	entry,
	const io_name_t		plane,
	io_iterator_t	      * iterator )
{
    return( io_registry_entry_get_parent_iterator( entry,
						(char *) plane, iterator));
}

kern_return_t
IORegistryEntryGetChildEntry(
	io_registry_entry_t	entry,
	const io_name_t		plane,
	io_registry_entry_t   * child )
{
    kern_return_t       kr;
    io_iterator_t	iter;

    kr = IORegistryEntryGetChildIterator( entry, plane, &iter );

    if( KERN_SUCCESS == kr) {

	*child = IOIteratorNext( iter );
	IOObjectRelease( iter );
	if( MACH_PORT_NULL == *child)
	    kr = kIOReturnNoDevice;
    }

    return( kr );
}

kern_return_t
IORegistryEntryGetParentEntry(
	io_registry_entry_t	entry,
	const io_name_t		plane,
	io_registry_entry_t   * parent )
{
    kern_return_t       kr;
    io_iterator_t	iter;

    kr = IORegistryEntryGetParentIterator( entry, plane, &iter );

    if( KERN_SUCCESS == kr) {

	*parent = IOIteratorNext( iter );
	IOObjectRelease( iter );
	if( MACH_PORT_NULL == *parent)
	    kr = kIOReturnNoDevice;
    }

    return( kr );
}
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

kern_return_t
IOServiceOFPathToBSDName(mach_port_t	 masterPort,
                         const io_name_t openFirmwarePath,
                         io_name_t   	 bsdName)
{
    kern_return_t       kr;
    io_registry_entry_t service;
    io_iterator_t       services;

    // Initialize return values.

    bsdName[0] = 0;

    // Find objects matching the given open firmware name.

    kr = IOServiceGetMatchingServices(
             /* mach_port_t   */ masterPort,
             /* void *        */ IOOpenFirmwarePathMatching(
                                     /* mach_port_t  */ masterPort,
                                     /* unsigned int */ 0,
                                     /* const char * */ openFirmwarePath ),
             /* io_iterator * */ &services );

    if( KERN_SUCCESS != kr )  return( kr );

    // Obtain the first (and presumably the only) match.

    service = IOIteratorNext( services );

    if( service ) {

        // Obtain the BSD name property from this object.

        unsigned int bsdNameSize = sizeof(io_name_t);

        kr = IORegistryEntryGetProperty(
                 /* mach_port_t        */ service,
                 /* io_name_t          */ kIOBSDName,
                 /* io_struct_inband_t */ bsdName,
                 /* unsigned int *     */ &bsdNameSize);

        if( KERN_SUCCESS != kr )  bsdName[0] = 0;

        IOObjectRelease( service );
    }
    else {
        kr = KERN_FAILURE;
    }

    IOObjectRelease( services );

    return kr;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

kern_return_t
IOCatalogueSendData(
        mach_port_t		masterPort,
        int                     flag,
        const char             *buffer,
        int                     size )
{
    kern_return_t kr;
    kern_return_t result;

    kr = io_catalog_send_data( masterPort, flag,
                            (char *) buffer, size, &result );
    if( KERN_SUCCESS == kr)
        kr = result;
        
    return( kr );
}

kern_return_t
IOCatalogueTerminate(
        mach_port_t		masterPort,
        int                     flag,
        io_name_t		description )
{
    return( io_catalog_terminate( masterPort, flag, description ));
}

kern_return_t
IOCatalogueGetData(
        mach_port_t		masterPort,
        int                     flag,
        char                  **buffer,
        int                    *size )
{
    return ( io_catalog_get_data( masterPort, flag, (char **)buffer, (unsigned *)size ) );
}

kern_return_t
IOCatlogueGetGenCount(
        mach_port_t		masterPort,
        int                    *genCount )
{
    return ( io_catalog_get_gen_count( masterPort, genCount ) );
}

kern_return_t
IOCatalogueModuleLoaded(
        mach_port_t		masterPort,
        io_name_t               name )
{
    return ( io_catalog_module_loaded( masterPort, name ) );
}

kern_return_t
IOCatalogueReset(
        mach_port_t		masterPort,
	int			flag )
{
    return ( io_catalog_reset(masterPort, flag) );
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

// obsolete API

kern_return_t
IORegistryCreateEnumerator(
        mach_port_t	masterPort,
	mach_port_t *	enumerator )
{
    return( io_registry_create_iterator( masterPort,
                                         "IOService", true, enumerator));
}

kern_return_t
IORegistryEnumeratorReset(
	mach_port_t	enumerator )
{
    return( io_iterator_reset( enumerator));
}


static io_object_t lastRegIter;

kern_return_t
IORegistryEnumeratorNextConforming(
	mach_port_t	enumerator,
	const char *	name,
	boolean_t	recursive )
{
    io_object_t	next 	= 0;

    while( (next = IOIteratorNext( enumerator ))) {

	if( IOObjectConformsTo( next, (char *) name )) 
	    break;

	IOObjectRelease( next );
    }

    lastRegIter = next;

    return( next ? kIOReturnSuccess : kIOReturnNoDevice );
}


#if 0
kern_return_t
IORegistryGetProperties(
	mach_port_t	enumerator ,
	void **		properties )
{
    return( IORegistryEntryGetProperties( lastRegIter,
		(struct IOObject **)properties));
}
#endif

kern_return_t
IOOpenConnection(
	mach_port_t	enumerator,
	task_port_t	owningTask,
	unsigned int	type,
	mach_port_t *	connect )
{
    kern_return_t	kr;

    kr = io_service_open( lastRegIter,
	owningTask, type, connect );

    IOObjectRelease( lastRegIter );

    return( kr );
}


kern_return_t
IOCloseConnection(
	mach_port_t	connect )
{
    return( io_service_close( connect));
}


kern_return_t
IOSetNotificationPort(
	mach_port_t	connect,
	unsigned int	type,
	mach_port_t	port )
{
    return( io_connect_set_notification_port( connect, type, port, 0));
}
kern_return_t
IORegisterClient(
	mach_port_t	connect,
	mach_port_t	client )
{
    return( io_connect_add_client( connect, client));
}

kern_return_t
IORegistryDisposeEnumerator(
	mach_port_t	enumerator )
{
    return( IOObjectRelease( enumerator ));
}

kern_return_t
IOMapMemory(
	mach_port_t	connect,
	unsigned int	memoryType,
	task_port_t	intoTask,
	vm_address_t *	atAddress,
	vm_size_t    *	ofSize,
	unsigned int	flags )
{
    return( io_connect_map_memory( connect, memoryType, intoTask,
	atAddress, ofSize, flags));
}

/* -------------------------- */

kern_return_t
IOCompatibiltyNumber(
	mach_port_t	connect,
	unsigned int *	objectNumber )
{
	*objectNumber = 1;
	return( kIOReturnSuccess);
}


void
IOInitContainerClasses()
{
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

