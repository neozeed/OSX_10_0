export MakeInc_cmd=${SRCROOT}/makedefs/MakeInc.cmd
export MakeInc_def=${SRCROOT}/makedefs/MakeInc.def
export MakeInc_rule=${SRCROOT}/makedefs/MakeInc.rule
export MakeInc_dir=${SRCROOT}/makedefs/MakeInc.dir

IOKIT_FRAMEDIR = $(FRAMEDIR)/IOKit.framework/Versions/A
export INCDIR = $(IOKIT_FRAMEDIR)/Headers
export LCLDIR = $(IOKIT_FRAMEDIR)/PrivateHeaders

include $(MakeInc_cmd)
include $(MakeInc_def)

INSTINC_SUBDIRS = \
        adb \
	cdb \
        graphics \
        hidsystem \
	network \
	nvram \
        pci \
	platform \
	power \
	pwr_mgt \
	rtc \
	scsi \
	storage \
	system_management

INSTINC_SUBDIRS_PPC = \
	ppc \
	ndrvsupport

INSTINC_SUBDIRS_I386 = \
	i386 \
	ps2

EXPINC_SUBDIRS = ${INSTINC_SUBDIRS}
EXPINC_SUBDIRS_PPC = ${INSTINC_SUBDIRS_PPC}
EXPINC_SUBDIRS_I386 = ${INSTINC_SUBDIRS_I386}

NOT_EXPORT_HEADERS = 
NOT_LOCAL_HEADERS = 

ALL_HEADERS = $(shell (cd $(SOURCE); echo *.h))

INSTALL_MI_LIST	= IOBSD.h IOKitKeys.h IOKitServer.h IOReturn.h\
		  IOSharedLock.h IOTypes.h OSMessageNotification.h\
		  IODataQueueShared.h IOMessage.h
                  
INSTALL_MI_LCL_LIST = ""

INSTALL_MI_DIR = .

EXPORT_MI_LIST	= $(filter-out $(NOT_EXPORT_HEADERS), $(ALL_HEADERS))

EXPORT_MI_DIR = IOKit

include $(MakeInc_rule)
include $(MakeInc_dir)
