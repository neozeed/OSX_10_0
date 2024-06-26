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
#ifndef _MACH_O_DYLD_H_
#define _MACH_O_DYLD_H_

#if defined(__MWERKS__) && !defined(__private_extern__)
#define __private_extern__ __declspec(private_extern)
#endif

#import "mach-o/loader.h"

#ifndef ENUM_DYLD_BOOL
#define ENUM_DYLD_BOOL
#undef FALSE
#undef TRUE
#if !defined(__cplusplus) && !defined(__ALTIVEC__) && !defined(DYLD_BOOL)
#define DYLD_BOOL bool
#endif
enum DYLD_BOOL {
    FALSE,
    TRUE
};
#endif /* ENUM_DYLD_BOOL */

/*
 * The high level NS... API.
 */

/* Object file image api */
typedef enum {
    NSObjectFileImageFailure, /* for this a message is printed on stderr */
    NSObjectFileImageSuccess,
    NSObjectFileImageInappropriateFile,
    NSObjectFileImageArch,
    NSObjectFileImageFormat, /* for this a message is printed on stderr */
    NSObjectFileImageAccess
} NSObjectFileImageReturnCode;

typedef void * NSObjectFileImage;

/* limited implementation, only MH_BUNDLE files can be used */
extern NSObjectFileImageReturnCode NSCreateObjectFileImageFromFile(
    const char *pathName,
    NSObjectFileImage *objectFileImage);
extern NSObjectFileImageReturnCode NSCreateCoreFileImageFromFile(
    const char *pathName,
    NSObjectFileImage *objectFileImage);
/* not yet implemented */
extern NSObjectFileImageReturnCode NSCreateObjectFileImageFromMemory(
    void *address,
    unsigned long size, 
    NSObjectFileImage *objectFileImage);
/* not yet implemented */
extern enum DYLD_BOOL NSDestroyObjectFileImage(
    NSObjectFileImage objectFileImage);
/*
 * Need api on NSObjectFileImage's for:
 *   "for Each Symbol Definition In Object File Image" (for Dynamic Bundles)
 *   Could have the same thing for references
 */
/* not yet implemented */
extern unsigned long NSSymbolDefinitionCountInObjectFileImage(
    NSObjectFileImage objectFileImage);
/* not yet implemented */
extern const char * NSSymbolDefinitionNameInObjectFileImage(
    NSObjectFileImage objectFileImage,
    unsigned long ordinal);
/* not yet implemented */
extern unsigned long NSSymbolReferenceCountInObjectFileImage(
    NSObjectFileImage objectFileImage);
/* not yet implemented */
extern const char * NSSymbolReferenceNameInObjectFileImage(
    NSObjectFileImage objectFileImage,
    unsigned long ordinal,
    enum DYLD_BOOL *tentative_definition); /* can be NULL */
/*
 * Other needed api on NSObjectFileImage:
 *   "does Object File Image define symbol name X" (using sorted symbol table)
 *   a way to get the named objective-C section
 */
/* not yet implemented */
extern enum DYLD_BOOL NSIsSymbolDefinedInObjectFileImage(
    NSObjectFileImage objectFileImage,
    const char *symbolName);
/* not yet implemented */
extern void * NSGetSectionDataInObjectFileImage(
    NSObjectFileImage objectFileImage,
    const char *segmentName,
    const char *sectionName);

/* module api */
typedef void * NSModule;
extern const char * NSNameOfModule(
    NSModule m); 
extern const char * NSLibraryNameForModule(
    NSModule m);

/* limited implementation, only MH_BUNDLE files can be linked */
extern NSModule NSLinkModule(
    NSObjectFileImage objectFileImage, 
    const char *moduleName,
    unsigned long options);
#define NSLINKMODULE_OPTION_NONE		0x0
#define NSLINKMODULE_OPTION_BINDNOW		0x1
#define NSLINKMODULE_OPTION_PRIVATE		0x2
#define NSLINKMODULE_OPTION_RETURN_ON_ERROR	0x4

/* limited implementation, only modules loaded with NSLinkModule() can be
   unlinked */
extern enum DYLD_BOOL NSUnLinkModule(
    NSModule module, 
    unsigned long options);
#define NSUNLINKMODULE_OPTION_NONE			0x0
#define NSUNLINKMODULE_OPTION_KEEP_MEMORY_MAPPED	0x1
#define NSUNLINKMODULE_OPTION_RESET_LAZY_REFERENCES	0x2

/* not yet implemented */
extern NSModule NSReplaceModule(
    NSModule moduleToReplace,
    NSObjectFileImage newObjectFileImage, 
    unsigned long options);

/* symbol api */
typedef void * NSSymbol;
extern enum DYLD_BOOL NSIsSymbolNameDefined(
    const char *symbolName);
extern enum DYLD_BOOL NSIsSymbolNameDefinedWithHint(
    const char *symbolName,
    const char *libraryNameHint);
extern NSSymbol NSLookupAndBindSymbol(
    const char *symbolName);
extern NSSymbol NSLookupAndBindSymbolWithHint(
    const char *symbolName,
    const char *libraryNameHint);
extern NSSymbol NSLookupSymbolInModule(
    NSModule module,
    const char *symbolName);
extern const char * NSNameOfSymbol(
    NSSymbol symbol);
extern void * NSAddressOfSymbol(
    NSSymbol symbol);
extern NSModule NSModuleForSymbol(
    NSSymbol symbol);

/* error handling api */
typedef enum {
    NSLinkEditFileAccessError,
    NSLinkEditFileFormatError,
    NSLinkEditMachResourceError,
    NSLinkEditUnixResourceError,
    NSLinkEditOtherError,
    NSLinkEditWarningError,
    NSLinkEditMultiplyDefinedError,
    NSLinkEditUndefinedError
} NSLinkEditErrors;

/*
 * For the NSLinkEditErrors value NSLinkEditOtherError these are the values
 * passed to the link edit error handler as the errorNumber (what would be an
 * errno value for NSLinkEditUnixResourceError or a kern_return_t value for
 * NSLinkEditMachResourceError).
 */
typedef enum {
    NSOtherErrorRelocation, 
    NSOtherErrorLazyBind,
    NSOtherErrorIndrLoop,
    NSOtherErrorLazyInit
} NSOtherErrorNumbers;

extern void NSLinkEditError(
    NSLinkEditErrors *c,
    int *errorNumber, 
    const char **fileName,
    const char **errorString);

typedef struct {
     void     (*undefined)(const char *symbolName);
     NSModule (*multiple)(NSSymbol s, NSModule oldModule, NSModule newModule); 
     void     (*linkEdit)(NSLinkEditErrors errorClass, int errorNumber,
                          const char *fileName, const char *errorString);
} NSLinkEditErrorHandlers;

extern void NSInstallLinkEditErrorHandlers(
    NSLinkEditErrorHandlers *handlers);

/* other api */
extern enum DYLD_BOOL NSAddLibrary(
    const char *pathName);
extern enum DYLD_BOOL NSAddLibraryWithSearching(
    const char *pathName);
extern long NSVersionOfRunTimeLibrary(
    const char *libraryName);
extern long NSVersionOfLinkTimeLibrary(
    const char *libraryName);

/*
 * The low level _dyld_... API.
 * (used by the objective-C runtime primarily)
 */
extern unsigned long _dyld_present(
    void);

extern unsigned long _dyld_image_count(
    void);
extern struct mach_header * _dyld_get_image_header(
    unsigned long image_index);
extern unsigned long _dyld_get_image_vmaddr_slide(
    unsigned long image_index);
extern char * _dyld_get_image_name(
    unsigned long image_index);

extern void _dyld_register_func_for_add_image(
    void (*func)(struct mach_header *mh, unsigned long vmaddr_slide));
extern void _dyld_register_func_for_remove_image(
    void (*func)(struct mach_header *mh, unsigned long vmaddr_slide));
extern void _dyld_register_func_for_link_module(
    void (*func)(NSModule module));
/* not yet implemented */
extern void _dyld_register_func_for_unlink_module(
    void (*func)(NSModule module));
/* not yet implemented */
extern void _dyld_register_func_for_replace_module(
    void (*func)(NSModule oldmodule, NSModule newmodule));
extern void _dyld_get_objc_module_sect_for_module(
    NSModule module,
    void **objc_module,
    unsigned long *size);
extern void _dyld_bind_objc_module(
    void *objc_module);
extern enum DYLD_BOOL _dyld_bind_fully_image_containing_address(
    unsigned long *address);

extern void _dyld_moninit(
    void (*monaddition)(char *lowpc, char *highpc));
extern enum DYLD_BOOL _dyld_launched_prebound(
    void);

extern void _dyld_lookup_and_bind(
    const char *symbol_name,
    unsigned long *address,
    void **module);
extern void _dyld_lookup_and_bind_with_hint(
    const char *symbol_name,
    const char *library_name_hint,
    unsigned long *address,
    void **module);
extern void _dyld_lookup_and_bind_objc(
    const char *symbol_name,
    unsigned long *address,
    void **module);
extern void _dyld_lookup_and_bind_fully(
    const char *symbol_name,
    unsigned long *address,
    void **module);

__private_extern__ int _dyld_func_lookup(
    const char *dyld_func_name,
    unsigned long *address);

#endif /* _MACH_O_DYLD_H_ */
