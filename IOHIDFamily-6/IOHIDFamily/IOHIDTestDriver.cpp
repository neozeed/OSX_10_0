
#include <IOKit/IOBufferMemoryDescriptor.h>
#include "IOHIDEventQueue.h"
#include "IOHIDTestDriver.h"

#undef  super
#define super IOHIDDevice
OSDefineMetaClassAndStructors( IOHIDTestDriver, IOHIDDevice )

void IOHIDTestDriver::free()
{
    if ( _timerSource )
    {
        _timerSource->cancelTimeout();
        _timerSource->release();
        _timerSource = 0;
    }

    if ( _workLoop )
    {
        _workLoop->release();
        _workLoop = 0;
    }

    super::free();
}

static void generateFakeReport(OSObject * owner, IOTimerEventSource * sender)
{
    ((IOHIDTestDriver *) owner)->issueFakeReport();

    sender->setTimeoutMS(1000);
}

void IOHIDTestDriver::handleStop( IOService * provider )
{
    if ( _timerSource ) _timerSource->cancelTimeout();
    
    super::handleStop(provider);
}

bool IOHIDTestDriver::handleStart( IOService * provider )
{
    if ( super::handleStart(provider) == false )
        return false;

    _workLoop = IOWorkLoop::workLoop();
    if ( _workLoop == 0 )
        return false;
        
    _timerSource = IOTimerEventSource::timerEventSource(
                     this, 
                     (IOTimerEventSource::Action) &generateFakeReport );
    if ( _timerSource == 0)
        return false;
    
    if ( _workLoop->addEventSource( _timerSource ) != kIOReturnSuccess )
        return false;

    _timerSource->setTimeoutMS(1000);

    return true;
}

//---------------------------------------------------------------------------
// Report fake report descriptor and report data.

IOReturn IOHIDTestDriver::newReportDescriptor(IOMemoryDescriptor ** desc) const
{
#if 0
    // Keyboard
    char report[] = { 0x05, 0x01,
                      0x09, 0x06,
                      0xA1, 0x01,
                      0x05, 0x07,
                      0x19, 0xE0,
                      0x29, 0xE7,
                      0x15, 0x00,
                      0x25, 0x01,
                      0x75, 0x01,
                      0x95, 0x08,
                      0x81, 0x02,
                      0x95, 0x01,
                      0x75, 0x08,
                      0x81, 0x01,
                      0x95, 0x05,
                      0x75, 0x01,
                      0x05, 0x08,
                      0x19, 0x01,
                      0x29, 0x05,
                      0x91, 0x02,
                      0x95, 0x01,
                      0x75, 0x03,
                      0x91, 0x01,
                      0x95, 0x06,
                      0x75, 0x08,
                      0x15, 0x00,
                      0x25, 0x65,
                      0x05, 0x07,
                      0x19, 0x00,
                      0x29, 0x65,
                      0x81, 0x00, 0xC0 };

    // macally joystick
    char report[] = {
        0x05, 0x01, 0x09, 0x04, 0xA1, 0x01, 0x09, 0x01,
        0xA1, 0x00, 0x05, 0x01, 0x09, 0x30, 0x09, 0x31,
        0x15, 0x00, 0x25, 0x7F, 0x35, 0x00, 0x46, 0xFF,
        0x00, 0x66, 0x00, 0x00, 0x75, 0x08, 0x95, 0x02,
        0x81, 0x02, 0x75, 0x08, 0x95, 0x01, 0x81, 0x01,
        0x09, 0x39, 0x15, 0x01, 0x25, 0x08, 0x36, 0x00,
        0x00, 0x46, 0x3B, 0x01, 0x65, 0x14, 0x75, 0x08,
        0x95, 0x01, 0x81, 0x02, 0x05, 0x09, 0x19, 0x01,
        0x29, 0x04, 0x15, 0x00, 0x25, 0x01, 0x35, 0x00,
        0x45, 0x01, 0x75, 0x01, 0x95, 0x04, 0x81, 0x02,
        0x95, 0x01, 0x75, 0x04, 0x81, 0x01, 0xC0, 0x05,
        0x01, 0x09, 0x36, 0x15, 0x00, 0x26, 0xFF, 0x00,
        0x35, 0x00, 0x46, 0xFF, 0x00, 0x75, 0x08, 0x66,
        0x00, 0x00, 0x95, 0x01, 0x81, 0x02, 0xC0, 0x00,
        0x00, 0x00
    };

    // macally gamepad
    char report[] = {
        0x05, 0x01, 0x09, 0x05, 0xA1, 0x01, 0x09, 0x01,
        0xA1, 0x00, 0x05, 0x01, 0x09, 0x30, 0x09, 0x31,
        0x09, 0x35, 0x09, 0x36, 0x15, 0x00, 0x25, 0x7F,
        0x35, 0x00, 0x46, 0xFF, 0x00, 0x65, 0x00, 0x75,
        0x08, 0x95, 0x04, 0x81, 0x02, 0x05, 0x09, 0x19,
        0x01, 0x29, 0x14, 0x15, 0x00, 0x25, 0x01, 0x35,
        0x00, 0x45, 0x01, 0x75, 0x01, 0x95, 0x14, 0x81,
        0x02, 0x75, 0x01, 0x95, 0x04, 0x81, 0x01, 0xC0,
        0xC0
    };

    // gravis gamepad
    char report[] = {
        0x05, 0x01, 0x09, 0x05, 0xA1, 0x01, 0x05, 0x01,
        0x09, 0x01, 0xA1, 0x00, 0x09, 0x30, 0x09, 0x31,
        0x15, 0x00, 0x26, 0xFF, 0x00, 0x75, 0x08, 0x95,
        0x02, 0x81, 0x02, 0xC0, 0x05, 0x09, 0x19, 0x01,
        0x29, 0x0A, 0x15, 0x00, 0x25, 0x01, 0x75, 0x01,
        0x95, 0x0A, 0x81, 0x02, 0x95, 0x01, 0x75, 0x06,
        0x81, 0x01, 0xC0 
    };
    
    // sidewinder gamepad
    char report[] = {
        0x05, 0x01, 0x09, 0x05, 0xA1, 0x01, 0x09, 0x01,
        0xA1, 0x00, 0x05, 0x01, 0x09, 0x30, 0x09, 0x31,
        0x15, 0x00, 0x25, 0x7F, 0x35, 0x00, 0x45, 0x7F,
        0x66, 0x00, 0x00, 0x75, 0x08, 0x95, 0x02, 0x81,
        0x02, 0xC0, 0x05, 0x09, 0x19, 0x01, 0x29, 0x06,
        0x15, 0x00, 0x25, 0x01, 0x35, 0x00, 0x45, 0x01,
        0x75, 0x01, 0x95, 0x06, 0x81, 0x02, 0x95, 0x02,
        0x81, 0x01, 0xC0
    };

    // Xterminator
    char report[] = {
        0x05, 0x01, 0x09, 0x04, 0xA1, 0x01, 0x09, 0x01,
        0xA1, 0x00, 0x05, 0x01, 0x09, 0x30, 0x09, 0x31,
        0x15, 0x00, 0x26, 0xFF, 0x00, 0x75, 0x08, 0x95,
        0x02, 0x81, 0x02, 0xC0, 0x05, 0x01, 0x09, 0x35,
        0x15, 0x00, 0x26, 0xFF, 0x00, 0x35, 0x00, 0x46,
        0xFF, 0x00, 0x75, 0x08, 0x95, 0x01, 0x81, 0x02,
        0x09, 0x37, 0x15, 0x00, 0x26, 0xFF, 0x00, 0x35,
        0x00, 0x46, 0xFF, 0x00, 0x75, 0x08, 0x95, 0x01,
        0x81, 0x02, 0x09, 0x32, 0x15, 0x00, 0x26, 0xFF,
        0x00, 0x35, 0x00, 0x46, 0xFF, 0x00, 0x75, 0x08,
        0x95, 0x01, 0x81, 0x02, 0x05, 0x01, 0x09, 0x39,
        0x15, 0x01, 0x25, 0x08, 0x75, 0x08, 0x95, 0x01,
        0x81, 0x42, 0x05, 0x09, 0x19, 0x01, 0x29, 0x09,
        0x15, 0x00, 0x25, 0x01, 0x75, 0x01, 0x95, 0x09,
        0x81, 0x02, 0x75, 0x01, 0x95, 0x07, 0x81, 0x01,
        0xC0
    };

    // Thrustmaster
    char report[] = {
        0x05, 0x01, 0x09, 0x04, 0xA1, 0x01, 0x09, 0x01,
        0xA1, 0x00, 0x09, 0x30, 0x09, 0x31, 0x15, 0x80,
        0x25, 0x7F, 0x35, 0x00, 0x46, 0xFF, 0x00, 0x75,
        0x08, 0x95, 0x02, 0x81, 0x02, 0xC0, 0x09, 0x39, 
        0x15, 0x01, 0x25, 0x08, 0x35, 0x00, 0x46, 0x3B,
        0x01, 0x66, 0x14, 0x00, 0x75, 0x04, 0x95, 0x01,
        0x81, 0x42, 0x05, 0x09, 0x15, 0x00, 0x25, 0x01,
        0x35, 0x00, 0x45, 0x01, 0x19, 0x01, 0x29, 0x04,
        0x75, 0x01, 0x95, 0x04, 0x81, 0x02, 0x75, 0x08,
        0x95, 0x01, 0x81, 0x01, 0xC0
    };
#endif

    // macally gamepad
    char report[] = {
        0x05, 0x01, 0x09, 0x05, 0xA1, 0x01, 0x09, 0x01,
        0xA1, 0x00, 0x05, 0x01, 0x09, 0x30, 0x09, 0x31,
        0x09, 0x35, 0x09, 0x36, 0x15, 0x00, 0x25, 0x7F,
        0x35, 0x00, 0x46, 0xFF, 0x00, 0x65, 0x00, 0x75,
        0x08, 0x95, 0x04, 0x81, 0x02, 0x05, 0x09, 0x19,
        0x01, 0x29, 0x14, 0x15, 0x00, 0x25, 0x01, 0x35,
        0x00, 0x45, 0x01, 0x75, 0x01, 0x95, 0x14, 0x81,
        0x02, 0x75, 0x01, 0x95, 0x04, 0x81, 0x01, 0xC0,
        0xC0
    };

    *desc = IOBufferMemoryDescriptor::withBytes( report,
                                                 sizeof(report),
                                                 kIODirectionNone );

    return kIOReturnSuccess;
}

void IOHIDTestDriver::issueFakeReport()
{
    UInt8 report[][7] = {
    { 0x44, 0x3B, 0x49, 0x43, 0x80, 0x00, 0x00 },
    { 0x44, 0x3B, 0x49, 0x43, 0x00, 0x00, 0x00 },
    { 0x44, 0x3B, 0x49, 0x43, 0x20, 0x00, 0x00 },
    { 0x44, 0x3B, 0x49, 0x43, 0x00, 0x00, 0x00 },
    { 0x44, 0x3B, 0x49, 0x43, 0x10, 0x00, 0x00 },
    { 0x44, 0x3B, 0x49, 0x43, 0x00, 0x00, 0x00 },
    { 0x44, 0x3B, 0x49, 0x43, 0x00, 0x80, 0x00 },
    { 0x44, 0x3B, 0x49, 0x43, 0x00, 0x00, 0x00 },
    { 0x44, 0x36, 0x49, 0x43, 0x00, 0x00, 0x00 },
    { 0x48, 0x19, 0x49, 0x43, 0x00, 0x00, 0x00 },
    { 0x48, 0x15, 0x49, 0x43, 0x00, 0x00, 0x00 },
    { 0x4C, 0x11, 0x49, 0x43, 0x00, 0x00, 0x00 } };

    const  UInt32 reportCount = sizeof(report)/sizeof(report[0]);
    static UInt32 reportIndex = 0;

    IOMemoryDescriptor * desc = IOMemoryDescriptor::withAddress(
                                        report[reportIndex],
                                        sizeof(report[0]),
                                        kIODirectionOutIn );

    if (desc)
    {
        handleReport( desc );
        desc->release();
    }

    if ( ++reportIndex >= reportCount )
        reportIndex = 0;
}

//---------------------------------------------------------------------------
// Dummy properties.

OSString * IOHIDTestDriver::newTransportString() const
{
    return OSString::withCStringNoCopy("USB");
}

OSString * IOHIDTestDriver::newManufacturerString() const
{
    return OSString::withCStringNoCopy("ACME");
}

OSString * IOHIDTestDriver::newProductString() const
{
    return OSString::withCStringNoCopy("AxisStick");
}

OSNumber * IOHIDTestDriver::newVendorIDNumber() const
{
    return OSNumber::withNumber(64443, 32);
}

OSNumber * IOHIDTestDriver::newProductIDNumber() const
{
    return OSNumber::withNumber(0xffff, 32);
}

OSNumber * IOHIDTestDriver::newVersionNumber() const
{
    return OSNumber::withNumber(256, 32);
}

OSNumber * IOHIDTestDriver::newSerialNumber() const
{
    return OSNumber::withNumber(0x1234, 32);
}

OSNumber * IOHIDTestDriver::newPrimaryUsageNumber() const
{
    return OSNumber::withNumber(4, 32);
}

OSNumber * IOHIDTestDriver::newPrimaryUsagePageNumber() const
{
    return OSNumber::withNumber(1, 32);
}
