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


#ifndef PPP_PRIVMSG_H
#define PPP_PRIVMSG_H

#include <machine/types.h>

/* private extension to PPP client API */
enum {

    /* API extension for script engine writers */
    PPP_CCLRESULT = 0x100,
    PPP_CCLNOTE,
    PPP_CCLSPEED,
    PPP_CCLWRITETEXT,
    PPP_CCLMATCHTEXT,
    PPP_CCLASK,
    PPP_OPENFD,
    PPP_CLOSEFD,
    PPP_WRITEFD,
    PPP_READFD,

    PPP_OPT_AUTOCONNECT = 0x80000000,		// 4 bytes
    PPP_OPT_COMM_LISTENFILTER,		// string
    PPP_OPT_IPCP_IN_DNS1,		// 4 bytes
    PPP_OPT_IPCP_IN_DNS2,		// 4 bytes
    PPP_OPT_COMM_LOOPBACK,		// 4 bytes (used for test purpose)
    PPP_OPT_AUTOLISTEN,			// 4 bytes
    PPP_OPT_IPCP_IN_LOCALADDR,		// 4 bytes
    PPP_OPT_IPCP_IN_REMOTEADDR,		// 4 bytes
};
enum {

    PPP_LISTEN = 0x200	,	// 4 bytes
    PPP_APPLY 			// 4 bytes
};


enum {

    cclErr_AbortMatchRead = -6000,	// internal error used to abort match read
    cclErr_BadParameter = -6001,	// Bad parameter given to the engine
    cclErr_DuplicateLabel = -6002,	// Duplicate label
    cclErr_LabelUndefined = -6003,	// Label undefined
    cclErr_SubroutineOverFlow = -6004,	// Subroutine overflow
    cclErr_NoMemErr = -6005,		// No memory...
    cclErr = -6006,			// CCL error base
    cclErr_CloseError = -6007,		// There is at least one script open
    cclErr_ScriptCancelled = -6008,	// Script Canceled
    cclErr_TooManyLines = -6009,	// Script contains too many lines
    cclErr_ScriptTooBig = -6010,	// Script contains too many characters
    cclErr_NotInitialized = -6011,	// CCL has not been initialized
    cclErr_CancelInProgress = -6012,	// Cancel in progress.
    cclErr_PlayInProgress = -6013,	// Play command already in progress.
    cclErr_ExitOK = -6014,	 	// Exit with no error.
    cclErr_BadLabel = -6015,		// Label out of range.
    cclErr_BadCommand = -6016,		// Bad command.
    cclErr_EndOfScriptErr = -6017,	// End of script reached, expecting Exit.
    cclErr_MatchStrIndxErr = -6018,	// Match string index is out of bounds.
    cclErr_ModemErr = -6019, 		// Modem error, modem not responding.
    cclErr_NoDialTone = -6020,		// No dial tone.
    cclErr_NoCarrierErr = -6021, 	// No carrier.
    cclErr_LineBusyErr = -6022, 	// Line busy.
    cclErr_NoAnswerErr = -6023,		// No answer.
    cclErr_NoOriginateLabel = -6024,	// No @ORIGINATE label
    cclErr_NoAnswerLabel = -6025,	// No @ANSWER label
    cclErr_NoHangUpLabel = -6026,	// No @HANGUP label

};

#endif /* PPP_MSG_H */

