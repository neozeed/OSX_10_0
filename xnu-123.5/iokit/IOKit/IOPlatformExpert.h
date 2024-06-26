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
 * Copyright (c) 1998 Apple Computer, Inc.  All rights reserved. 
 *
 * HISTORY
 *
 */


#ifndef _IOKIT_IOPLATFORMEXPERT_H
#define _IOKIT_IOPLATFORMEXPERT_H

#ifdef __cplusplus
#include <IOKit/IOLib.h>
#include <IOKit/IOService.h>
#include <IOKit/IOInterrupts.h>
#include <IOKit/IOInterruptController.h>

extern "C" {
#endif

extern boolean_t PEGetMachineName( char * name, int maxLength );
extern boolean_t PEGetModelName( char * name, int maxLength );
extern int PEGetPlatformEpoch( void );

enum {
  kPEHaltCPU,
  kPERestartCPU
};
extern int (*PE_halt_restart)(unsigned int type);
extern int PEHaltRestart(unsigned int type);

extern long PEGetGMTTimeOfDay( void );
extern void PESetGMTTimeOfDay( long secs );
  
#ifdef __cplusplus
} /* extern "C" */

extern OSSymbol * gPlatformInterruptControllerName;

class IORangeAllocator;
class IONVRAMController;
class IOPMrootDomain;

class IOPlatformExpert : public IOService
{
    OSDeclareDefaultStructors(IOPlatformExpert);

private:
    long _peBootROMType;
    long _peChipSetType;
    long _peMachineType;

protected:
    IOPMrootDomain * root;
    int _pePMFeatures;
    int _pePrivPMFeatures;
    int _peNumBatteriesSupported;
    OSArray  * thePowerTree;

    bool       searchingForAdditionalParents;
    OSNumber * multipleParentKeyValue;
    int        numInstancesRegistered;

    struct ExpansionData { };
    ExpansionData *reserved;

    virtual void setBootROMType(long peBootROMType);
    virtual void setChipSetType(long peChipSetType);
    virtual void setMachineType(long peMachineType);

    virtual bool CheckSubTree (OSArray * inSubTree, IOService * theNub, IOService * theDevice, OSDictionary * theParent);
    virtual bool RegisterServiceInTree (IOService * theService, OSDictionary * theTreeNode, OSDictionary * theTreeParentNode, IOService * theProvider);

    virtual void PMInstantiatePowerDomains ( void );

public:
    virtual bool attach( IOService * provider );
    virtual bool start( IOService * provider );
    virtual bool configure( IOService * provider );
    virtual IOService * createNub( OSDictionary * from );

    virtual bool compareNubName( const IOService * nub, OSString * name,
				 OSString ** matched = 0 ) const;
    virtual IOReturn getNubResources( IOService * nub );

    virtual long getBootROMType(void);
    virtual long getChipSetType(void);
    virtual long getMachineType(void);

    virtual bool getModelName( char * name, int maxLength );
    virtual bool getMachineName( char * name, int maxLength );

    virtual int haltRestart(unsigned int type);
    virtual void sleepKernel(void);

    virtual long getGMTTimeOfDay( void );
    virtual void setGMTTimeOfDay( long secs );

    virtual IOReturn getConsoleInfo( PE_Video * consoleInfo );
    virtual IOReturn setConsoleInfo( PE_Video * consoleInfo, unsigned int op );

    virtual void registerNVRAMController( IONVRAMController * nvram );

    virtual IOReturn registerInterruptController(OSSymbol *name, IOInterruptController *interruptController);
    virtual IOInterruptController *lookUpInterruptController(OSSymbol *name);
    virtual void setCPUInterruptProperties(IOService *service);
    virtual bool atInterruptLevel(void);

    virtual IOReturn callPlatformFunction(const OSSymbol *functionName,
					  bool waitForFunction,
					  void *param1, void *param2,
					  void *param3, void *param4);

    virtual IORangeAllocator * getPhysicalRangeAllocator(void);

    virtual bool platformAdjustService(IOService *service);

    virtual void PMRegisterDevice(IOService * theNub, IOService * theDevice);
    virtual void PMLog ( const char *,unsigned long, unsigned long, unsigned long );

    virtual bool hasPMFeature (unsigned long featureMask);
    virtual bool hasPrivPMFeature (unsigned long privFeatureMask);
    virtual int  numBatteriesSupported (void);

    OSMetaClassDeclareReservedUnused(IOPlatformExpert,  0);
    OSMetaClassDeclareReservedUnused(IOPlatformExpert,  1);
    OSMetaClassDeclareReservedUnused(IOPlatformExpert,  2);
    OSMetaClassDeclareReservedUnused(IOPlatformExpert,  3);
    OSMetaClassDeclareReservedUnused(IOPlatformExpert,  4);
    OSMetaClassDeclareReservedUnused(IOPlatformExpert,  5);
    OSMetaClassDeclareReservedUnused(IOPlatformExpert,  6);
    OSMetaClassDeclareReservedUnused(IOPlatformExpert,  7);
    OSMetaClassDeclareReservedUnused(IOPlatformExpert,  8);
    OSMetaClassDeclareReservedUnused(IOPlatformExpert,  9);
    OSMetaClassDeclareReservedUnused(IOPlatformExpert, 10);
    OSMetaClassDeclareReservedUnused(IOPlatformExpert, 11);
};

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

class IODTNVRAM;

class IODTPlatformExpert : public IOPlatformExpert
{
    OSDeclareAbstractStructors(IODTPlatformExpert);

private:
    IODTNVRAM *dtNVRAM;

    struct ExpansionData { };
    ExpansionData *reserved;

public:
    virtual IOService * probe(	IOService * 	provider,
				SInt32 	  *	score );
    virtual bool configure( IOService * provider );

    virtual void processTopLevel( IORegistryEntry * root );
    virtual const char * deleteList( void ) = 0;
    virtual const char * excludeList( void ) = 0;
    virtual IOService * createNub( IORegistryEntry * from );
    virtual bool createNubs( IOService * parent, OSIterator * iter );

    virtual bool compareNubName( const IOService * nub, OSString * name,
				 OSString ** matched = 0 ) const;

    virtual IOReturn getNubResources( IOService * nub );

    virtual bool getModelName( char * name, int maxLength );
    virtual bool getMachineName( char * name, int maxLength );
    
    virtual void registerNVRAMController( IONVRAMController * nvram );

    virtual int haltRestart(unsigned int type);

    /* virtual */ IOReturn readXPRAM(IOByteCount offset, UInt8 * buffer,
				     IOByteCount length);
    /* virtual */ IOReturn writeXPRAM(IOByteCount offset, UInt8 * buffer,
				      IOByteCount length);

    virtual IOReturn readNVRAMProperty(
	IORegistryEntry * entry,
	const OSSymbol ** name, OSData ** value );

    virtual IOReturn writeNVRAMProperty(
	IORegistryEntry * entry,
	const OSSymbol * name, OSData * value );


    OSMetaClassDeclareReservedUnused(IODTPlatformExpert,  0);
    OSMetaClassDeclareReservedUnused(IODTPlatformExpert,  1);
    OSMetaClassDeclareReservedUnused(IODTPlatformExpert,  2);
    OSMetaClassDeclareReservedUnused(IODTPlatformExpert,  3);
    OSMetaClassDeclareReservedUnused(IODTPlatformExpert,  4);
    OSMetaClassDeclareReservedUnused(IODTPlatformExpert,  5);
    OSMetaClassDeclareReservedUnused(IODTPlatformExpert,  6);
    OSMetaClassDeclareReservedUnused(IODTPlatformExpert,  7);
};

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

/* generic root nub of service tree */

class IOPlatformExpertDevice : public IOService
{
    OSDeclareDefaultStructors(IOPlatformExpertDevice)

private:
    IOWorkLoop *workLoop;

    struct ExpansionData { };
    ExpansionData *reserved;

public:
    virtual bool initWithArgs( void * p1, void * p2,
					void * p3, void *p4 );
    virtual bool compareName( OSString * name, OSString ** matched = 0 ) const;

    virtual IOWorkLoop *getWorkLoop() const;

    virtual void free();

    OSMetaClassDeclareReservedUnused(IOPlatformExpertDevice,  0);
    OSMetaClassDeclareReservedUnused(IOPlatformExpertDevice,  1);
    OSMetaClassDeclareReservedUnused(IOPlatformExpertDevice,  2);
    OSMetaClassDeclareReservedUnused(IOPlatformExpertDevice,  3);
};

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

/* generic nub for motherboard devices */

class IOPlatformDevice : public IOService
{
    OSDeclareDefaultStructors(IOPlatformDevice)

    struct ExpansionData { };
    ExpansionData *reserved;

public:
    virtual bool compareName( OSString * name, OSString ** matched = 0 ) const;
    virtual IOService * matchLocation( IOService * client );
    virtual IOReturn getResources( void );

    OSMetaClassDeclareReservedUnused(IOPlatformDevice,  0);
    OSMetaClassDeclareReservedUnused(IOPlatformDevice,  1);
    OSMetaClassDeclareReservedUnused(IOPlatformDevice,  2);
    OSMetaClassDeclareReservedUnused(IOPlatformDevice,  3);
};

#endif /* __cplusplus */

#endif /* ! _IOKIT_IOPLATFORMEXPERT_H */
