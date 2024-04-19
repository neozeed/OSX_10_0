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


/* --------------------------------------------------------------------------
Includes
-------------------------------------------------------------------------- */

#include <stdio.h>
#include <ctype.h>
#include <time.h>
#include <fcntl.h>
#include <signal.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <syslog.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/un.h>
#include <sys/socket.h>
#include <stdarg.h>
#include <termios.h>
#include <sys/ioctl.h>

#include "CCLEngine.h"
#include "../../Controller/ppp_msg.h"
#include "../../Controller/ppp_privmsg.h"

/* --------------------------------------------------------------------------
Defines
-------------------------------------------------------------------------- */

#define min(a,b)			(((a)<=(b))?(a):(b))

enum {
    kUserMsgFLog = 1,
    kUserMsgFStatus = 2
};


#define infd 	STDIN_FILENO
#define outfd 	STDOUT_FILENO

enum
{
    kInvalidTimer = 0,
    kMatchReadTimer,
    kPauseTimer,
    kCharDelayTimer,
    kBreakTimer
};

enum {
    mode_connect = 0,
    mode_disconnect,
    mode_listen
};

struct	callout {
    struct timeval	c_time;		/* time at which to call routine */
    void		*c_arg;		/* argument to routine */
    void		(*c_func)(void *); /* routine */
    struct		callout *c_next;
};

/* --------------------------------------------------------------------------
Globals
-------------------------------------------------------------------------- */

u_int8_t gNullString[2];

char	*Commands[] =
{
    "!\0",			/*  1 */
    "@CCLSCRIPT\0",		/*  2 */
    "@ORIGINATE\0",		/*  3 */
    "@ANSWER\0",		/*  4 */
    "@HANGUP\0",		/*  5 */
    "@LABEL\0",			/*  6 */
    "ASK\0",			/*  7 */
    "CHRDELAY\0",		/*  8 */
    "COMMUNICATINGAT\0",	/*  9 */
    "DECTRIES\0",		/* 10 */
    "DTRSET\0",			/* 11 */
    "DTRCLEAR",			/* 12 */
    "EXIT",			/* 13 */
    "FLUSH",			/* 14 */
    "HSRESET",			/* 15 */
    "IFANSWER",			/* 16 */
    "IFORIGINATE",		/* 17 */
    "IFSTR",			/* 18 */
    "IFTRIES",			/* 19 */
    "INCTRIES",			/* 20 */
    "JUMP",			/* 21 */
    "JSR",			/* 22 */
    "LBREAK",			/* 23 */
    "LOGMSG",			/* 24 */		// undocumented.  use cNote.  KW
    "MATCHCLR",			/* 25 */
    "MATCHREAD",		/* 26 */
    "MATCHSTR",			/* 27 */
    "NOTE",			/* 28 */
    "PAUSE",			/* 29 */
    "RETURN",			/* 30 */
    "SBREAK",			/* 31 */
    "SERRESET",			/* 32 */
    "SETSPEED",			/* 33 */
    "SETTRIES",			/* 34 */
    "USERHOOK",			/* 35 */
    "WRITE"			/* 36 */
};


TRScriptVars	SV;			// script variables structure
u_int8_t	*VarStrings[vsMax+1];


//	The following fields are set from values retrieved from the
//	CCL script 'mlts' resource. The values are received in one
//	or more Script_ccl_setup messages. See ReadVARMessages().
//	NOTE: if the script file has no 'mlts' resource, the setup
//	message is created using a default 'mlts' stored in the
//	ScriptMod configurator (CCL configurator).
u_int8_t	DialString1MaxLen;
u_int8_t	DialString2MaxLen;
u_int8_t	DialString3MaxLen;

u_int8_t	VerboseBuffer[256];//	Buffer for forming text for verbose log messages.
u_int32_t	LastExitError;

// Spackle a security hole.  Was the last ask masked?  if so,
// the next write we do should log masked.
// Don't forget to set and clear this flag in later asks and
// after the write
u_int32_t	LastAskedMasked;

FILE* filefd   = (FILE *) 0;
char *filename    = "";
char *phone_num   = "";
char *username  = "";
char *password  = "";
int pulse = 0;
int dialmode = 0; // 0 = normal, 1 = blind(ignoredialtone), 2 = manual
int speaker = 1;
int terminalMode = 0;

struct callout *callout = NULL;	/* Callout list */
struct timeval timenow;		/* Current time */
int mode = mode_connect;
int ppplink 	  = -1;
int csockfd 	  = -1;
int verbose 	  = 0;

/* --------------------------------------------------------------------------
Forward Declarations
-------------------------------------------------------------------------- */
void SetVarString(u_int32_t vs, u_int8_t * data);
u_int8_t *GetVarString(u_int32_t vs);
int PrepScript();
u_int8_t NextLine();
int NextCommand();
int NextInt(u_int32_t *theIntPtr);
void PrepStr(u_int8_t *destStr, u_int32_t *isVarString, u_int32_t *varIndex);
void SkipBlanks();
void RunScript();
int MatchStr();
void Note();
u_int8_t Write();
void CommunicatingAt();
void WriteContinue();
void UserHook();
void ScheduleTimer(long type, u_int32_t milliseconds);
int IfStr();

void MatchClr();
void Break();
void SetSpeed(void);
void SerReset(void);
void HSReset();
void Flush();
void DTRCommand(short DTRCode);

void terminate(int exitError);
void InitScript();
void Play();
void calltimeout(void);
struct timeval *timeleft(struct timeval *);
void timeout(void (*func)(void *), void *arg, u_long time);
void untimeout(void (*func)(void *), void *arg);
void ReceiveMatchData(u_int8_t nextChar);
void sys_send_confd(int sockfd, u_long code, u_char *data, u_long len, u_long link, u_char expect_result);



/*************** Micro getopt() *********************************************/
#define	OPTION(c,v)	(_O&2&&**v?*(*v)++:!c||_O&4?0:(!(_O&1)&& \
                                (--c,++v),_O=4,c&&**v=='-'&&v[0][1]?*++*v=='-'\
                                &&!v[0][1]?(--c,++v,0):(_O=2,*(*v)++):0))
#define	OPTARG(c,v)	(_O&2?**v||(++v,--c)?(_O=1,--c,*v++): \
                                (_O=4,(char*)0):(char*)0)
#define	OPTONLYARG(c,v)	(_O&2&&**v?(_O=1,--c,*v++):(char*)0)
#define	ARG(c,v)	(c?(--c,*v++):(char*)0)

static int _O = 0;		/* Internal state */
/*************** Micro getopt() *********************************************/




/* --------------------------------------------------------------------------
-------------------------------------------------------------------------- */
void *dup_mem(void *b, size_t c)
{
    void *ans = malloc (c);
    if (!ans)
        terminate(cclErr_NoMemErr);
    
    memcpy (ans, b, c);
    return ans;
}

/* --------------------------------------------------------------------------
-------------------------------------------------------------------------- */
void *copy_of(char *s)
{
    return dup_mem (s, strlen (s) + 1);
}

/* --------------------------------------------------------------------------
-------------------------------------------------------------------------- */
void badsignal(int signo)
{
    //syslog(LOG_INFO, "badsignal = %d\n", signo);
    terminate(cclErr_NoMemErr);
}

/* --------------------------------------------------------------------------
-------------------------------------------------------------------------- */
int main(int argc, char **argv)
{
    int 		option, ret, maxfd, nready, status, doclose;
    char 		*arg, c;
    struct stat 	statbuf;
    fd_set		rset, allset;
    struct timeval 	timo;
    u_long		len;
    struct sockaddr_un	adr;
    struct ppp_msg_hdr	msg;

    signal(SIGHUP, badsignal);		/* Hangup */
    signal(SIGINT, badsignal);		/* Interrupt */
    signal(SIGTERM, badsignal);		/* Terminate */
    signal(SIGCHLD, badsignal);
    signal(SIGUSR1, badsignal);
    signal(SIGUSR2, badsignal);
    signal(SIGABRT, badsignal);
    signal(SIGALRM, badsignal);
    signal(SIGFPE, badsignal);
    signal(SIGILL, badsignal);
    signal(SIGPIPE, badsignal);
    signal(SIGQUIT, badsignal);
    signal(SIGSEGV, badsignal);

    while ((option = OPTION(argc, argv)) != 0) {
        switch (option) {

            case 's':
                if ((arg = OPTARG(argc, argv)) != NULL)
                    speaker = atoi(arg);
                else
                    terminate(cclErr_BadParameter);
                break;

            case 'd':
                if ((arg = OPTARG(argc, argv)) != NULL)
                    dialmode = atoi(arg);
                else
                    terminate(cclErr_BadParameter);
                break;

            case 'p':
                if ((arg = OPTARG(argc, argv)) != NULL)
                    pulse = atoi(arg);
                else
                    terminate(cclErr_BadParameter);
                break;

            case 'v':
                verbose = 1;
                break;

            case 'm': // 0 = connect, 1 = disconnect, 2 = answer
                if ((arg = OPTARG(argc, argv)) != NULL)
                    mode = atoi(arg);
                else
                    terminate(cclErr_BadParameter);
                break;

            case 'M': // 0 = modem script mode, 1 = terminal mode
                if ((arg = OPTARG(argc, argv)) != NULL)
                    terminalMode = atoi(arg);
                else
                    terminate(cclErr_BadParameter);
                break;

            case 'l':
                if ((arg = OPTARG(argc, argv)) != NULL)
                    ppplink = atoi(arg);
                else
                    terminate(cclErr_BadParameter);

                if ((csockfd = socket(AF_LOCAL, SOCK_STREAM, 0)) < 0) {
                    terminate(cclErr_NoMemErr);  // better error message ???
                }

                    bzero(&adr, sizeof(adr));
                adr.sun_family = AF_LOCAL;
                strcpy(adr.sun_path, PPP_PATH);
                if (connect(csockfd,  (struct sockaddr *)&adr, sizeof(adr)) < 0) {
                    terminate(cclErr_NoMemErr);  // better error message ???
                }
                    break;

            case 'f':
                if ((arg = OPTARG(argc, argv)) != NULL)
                    filename = copy_of(arg);
                else
                    terminate(cclErr_BadParameter);
                break;

            case 'T':
                if ((arg = OPTARG(argc, argv)) != NULL)
                    phone_num = copy_of(arg);
                else
                    terminate(cclErr_BadParameter);
                break;

            case 'U':
                if ((arg = OPTARG(argc, argv)) != NULL)
                    username = copy_of(arg);
                else
                    terminate(cclErr_BadParameter);
                break;

            case 'P':
                if ((arg = OPTARG(argc, argv)) != NULL)
                    password = copy_of(arg);
                else
                    terminate(cclErr_BadParameter);
                break;

            default:
                terminate(cclErr_BadParameter);
                break;
        }
    }
    
    InitScript();
    ret = stat(filename, &statbuf);
    if (ret < 0)
        terminate(cclErr_BadParameter);

    SV.script = malloc((int)statbuf.st_size);
    if (!SV.script)
        terminate(cclErr_NoMemErr);

    SV.scriptSize = (int)statbuf.st_size;

    filefd = fopen(filename, "r");
    if (filefd) {
        ret = fread(SV.script, statbuf.st_size, 1, filefd);
        if (ret < 0)
            terminate(cclErr_BadParameter);
    }

    Play();

    maxfd = (ppplink == -1 ? 0 : csockfd);
    FD_ZERO(&allset);
    FD_SET(csockfd, &allset);
    if (!terminalMode) 
        FD_SET(infd, &allset);

    for ( ; ; ) {

        if (terminalMode && (ppplink != -1)) {
            len = 1;
            sys_send_confd(csockfd, PPP_READFD, (char*)&len, sizeof(len), ppplink, 0);
        }
        //read(infd, (char*)&len, sizeof(len));

        rset = allset;
        nready = select(maxfd+1, &rset, NULL, NULL, timeleft(&timo));

        if (FD_ISSET(infd, &rset)) {
            status = read(infd, &c, 1);
            switch (status) {
                case 1:
                    ReceiveMatchData(c);
                    break;
                default:
                    terminate(0);
            }
        }

        if ((ppplink != -1) && FD_ISSET(csockfd, &rset)) {
            // Fix Me : Need to be reworked when connector plugin will be in place

            doclose = 1;

            if (terminalMode) {
                status = read(csockfd, &msg, sizeof(msg));
                if (status == sizeof(msg)) {
                    status = read(csockfd, &c, 1);
                    if (status == 1) {
                        doclose = 0;
                        ReceiveMatchData(c);
                    }
               }
            }
            if (doclose) {
                close(infd);
                close(outfd);
                exit(0);
            }
        }
        calltimeout();

    }

    // NOT REACHED
    exit(0);       // insure the process exit status is 0
    return 0;      // ...and make main fit the ANSI spec.
}

/* --------------------------------------------------------------------------
 init everything
-------------------------------------------------------------------------- */
void InitScript()
{
    u_int32_t	len;
    int 	i;
    char 	text[256], text1[256];

    gNullString[0] = 1;
    gNullString[1] = ' ';
    
    //
    // Initialize script variables and varStrings
    //
    bzero(&SV, sizeof(struct TRScriptVars));

    for (i = 0; i <= vsMax; i++)
        VarStrings[i] = 0;

    text[0] = 1; text[1] = '1';
    SetVarString(vsErrorCorrection, text);
    SetVarString(vsDataCompression, text);

    LastExitError		= 0;

    DialString1MaxLen		= 20;
    DialString2MaxLen		= 20;
    DialString3MaxLen		= 20;

    SV.topOfStack		= cclNestingLimit;	// this stack is for JSR and RETURN commands.
    SV.theAbortErr		= 0;
    SV.lineCount		= 0;
    SV.scriptPrepped		= 0;		// PrepScript has not yet been called
    SV.scriptPrepFailed		= 0;		// PrepScript has not yet rejected the script
    SV.scriptAllocSize		= 0;
    SV.scriptSize		= 0;
    SV.script			= NULL;
    SV.askLabel			= 0;
    SV.indexTable		= NULL;
    SV.commands			= NULL;

    LastAskedMasked		= 0;


    text[0] =  strlen(username);
    bcopy(username, &text[1], text[0]);
    SetVarString(vsUserName, text);

    text[0] =  strlen(password);
    bcopy(password, &text[1], text[0]);
    SetVarString(vsPassWord, text);

    sprintf(text, "%d", speaker);
    text1[0] =  strlen(text);
    bcopy(text, &text1[1], text[0]);
    SetVarString(vsModemSpeaker, text1);

    sprintf(text, "%d", dialmode);
    text1[0] =  strlen(text);
    bcopy(text, &text1[1], text[0]);
    SetVarString(vsDialMode, text1);

    text1[0] =  1;
    text1[1] =  pulse ? 'P' : 'T';
    SetVarString(vsTonePulse, text1);

    if (ppplink != -1) {
        // Fix Me : Need to be reworked when connector plugin will be in place
        len = 1;//1500;	// buffer we want to use
        sys_send_confd(csockfd, PPP_OPENFD, (u_char *)&len, sizeof(len), ppplink, 1);
    }
}

/* --------------------------------------------------------------------------
 dispose everything
-------------------------------------------------------------------------- */
void DisposeScript()
{
    int 	i;
    
    if (SV.script) {
        free(SV.script);
        SV.script = 0;
    }

    if (SV.commands) {
        free(SV.commands);
        SV.commands = 0;
    }

    if (SV.indexTable) {
        free(SV.indexTable);
        SV.indexTable = 0;
    }

    for (i = 0; i <= vsMax; i++) {
        if (VarStrings[i]) {
            free(VarStrings[i]);
            VarStrings[i] = 0;
        }
    }
}

/* --------------------------------------------------------------------------
var strings are held in the fVarStrings array.  each element is NULL until
SetVarString is called, in which case memory is allocated and the data copied in.
if Set has not been called, Get will return a pointer to gNullString, a zero length
pstring.
the reason no attempt was made to flag out-of-memory errors here, is also
because it results in the varString being retrieved by GetVarString being NULL.
The CCL script will play, but won't make a connection.
-------------------------------------------------------------------------- */
void SetVarString(u_int32_t vs, u_int8_t * data)
{
    if (vs > vsMax)
        return;

    if (VarStrings[vs]) {
        free(VarStrings[vs]);
        VarStrings[vs] = 0;
    }

    if (data) {
        VarStrings[vs] = malloc(*data + 1);
        if (VarStrings[vs])
            bcopy(data, VarStrings[vs], *data + 1 );
    }
}

/* --------------------------------------------------------------------------
-------------------------------------------------------------------------- */
u_int8_t *GetVarString(u_int32_t vs)
{
    if (vs > vsMax)
        return gNullString;

    if (VarStrings[vs])
        return VarStrings[vs];

    return gNullString;
}


/* --------------------------------------------------------------------------
called to start execution of the script.  this is true regardless as
to the connect/answer mode set by SetAnswering().
-------------------------------------------------------------------------- */
void Play(void)  
{	
    u_int32_t	remaining;
    u_int8_t	*src, len = 0, buf[256];
    char	text[256];

    LastExitError = 0;

    if (!SV.scriptPrepped ) { // if the Script has not yet been preflighted...
        if (SV.theAbortErr = PrepScript()) { // if the preflight returns an error...
            SV.scriptPrepFailed = 1;		// so Disconnect won't try to re-preflight the script.
            terminate(SV.theAbortErr);	// a script syntax error was detected
            return;
        }
        else
            SV.scriptPrepped = 1;
    }

    SV.chrDelayValue	= 0;

    switch (mode) {
        case mode_connect:
            SV.ctlFlags		&= ~cclHangupMode;
            SV.ctlFlags |= cclOriginateMode;		// turn on OriginateMode
            SV.scriptLine = SV.originateLine;		// set the script entry point

            // if the dial string has not yet arrived in a Script_ccl_setup message,
            // fetch the dial string from ScriptMod, which received it from either a
            // T_conn_req or a Script_execute message.

            text[0] =  strlen(phone_num);
            bcopy(phone_num, &text[1], text[0]);
            if (GetVarString(vsDialString) == gNullString)
                SetVarString(vsDialString, text);

            SetVarString(vsDialString1, NULL);
            SetVarString(vsDialString2, NULL);
            SetVarString(vsDialString3, NULL);

            if (src = GetVarString(vsDialString)) {

                remaining = *src++;

                if (len = min(DialString1MaxLen, remaining)) {
                    *buf = len;
                    bcopy(src, buf+1, len);
                    SetVarString(vsDialString1, buf);
                    src += len;
                    remaining -= len;
                }

                if (len = min(DialString2MaxLen, remaining)) {
                    *buf = len;
                    bcopy(src, buf+1, len);
                    SetVarString(vsDialString2, buf);
                    src += len;
                    remaining -= len;
                }

                if (len = min(DialString3MaxLen, remaining)) {
                    *buf = len;
                    bcopy(src, buf+1, len);
                    SetVarString(vsDialString3, buf);
                    src += len;
                    remaining -= len;
                }

                // and ignore any remaining characters
            }

           break;
        case mode_disconnect:
            SV.ctlFlags	&= ~cclAnswerMode;		// though we'll return to Answering, on disconnect
            SV.ctlFlags	|= cclHangupMode;
            SV.scriptLine = SV.hangUpLine;		// set the script entry point
            break;
        case mode_listen:
            SV.ctlFlags |= cclAnswerMode;		// turn on AnswerMode
            SV.ctlFlags		&= ~cclHangupMode;
            SV.scriptLine = SV.answerLine;		// set the script entry point
            break;
    }

    RunScript();	// plays the script from @Originate until EXIT 0
}

/* --------------------------------------------------------------------------
This routine prepares a script to be played.
CCL variablesa and labels are set up for playing the script, and commands are checked.
-------------------------------------------------------------------------- */
int PrepScript()
{
    u_int8_t	*bp, *d, *s, c;
    u_int16_t	*indexp, Lindex, i, cmd;
    u_int32_t	labelIndex;
    int		result = 0;

    // The following "do {} while (false);" construct
    // prepares for the task of looping through the lines
    // of script. If there is something wrong with script
    // or if it can't allocate memory, it breaks immediately
    // with an error value in "result".

    do {
        if (SV.scriptSize > MAX_SCRIPT_SIZE) {
            result = cclErr_ScriptTooBig;
            break;
        }

        Lindex = 1;
        bp = SV.script;

        // count the lines in the script:
        for( i = SV.scriptSize; i; --i ) {
            if ((*bp == chrCR) || (*bp == chrNL))
                Lindex++;
            bp++;
        }

        if (Lindex > MAX_SCRIPT_LINES) {
            result = cclErr_TooManyLines;
            break;
        }

        SV.lineCount = Lindex;
        bp = malloc(500);
        if (!bp) {
            result = ENOMEM;
            break;
        }

        SV.commands = bp;
        *((u_int16_t *)bp) = cLastCmd;
        bp += 2;
        for( i = 0; i < cLastCmd; ) {
            s = Commands[i++];
            d = bp + 1;
            while( c = *s++ )
                *d++ = c;
            *bp = (d - bp - 1);		// set count for pascal string
            bp = d;
        }

        SV.indexTable = malloc(SV.lineCount * sizeof(u_int16_t *));
        if (!SV.indexTable) {
            result = ENOMEM;
            break;
        }

        indexp = SV.indexTable;
        for( i = 0; ++i <= SV.lineCount; )
            *indexp++ = 0;

        /* adjust the size of line index array	*/
        indexp = SV.indexTable;

        /* fill line index array, for each line,		*/
        /* SV.indexTable->lineIndex == byte offset	*/
        /* from start of script to start of line.		*/
        bp = SV.script;
        Lindex = 0;
        *indexp++ = 0;
        for (i = 0; i < SV.scriptSize; i++) {
            if ((*bp == chrCR) || (*bp == chrNL))
                *indexp++ = i + 1;
            bp++;
        }

        SV.scriptLine = 0;
    }
    while (0);

    // The initialization loop above may have set
    // result != noErr. If so, the following while
    // loop is not entered.

    while (result == 0 && NextLine()) {	/* process the next line of the script	*/
        cmd = NextCommand();
        switch (cmd)  {
            case cNoCmd:
                result = cclErr_BadCommand;
                break;

            case cOriginateLabel:
                if( SV.originateLine )
                    result = cclErr_DuplicateLabel;
                else
                    SV.originateLine = SV.scriptLine;
                break;

            case cAnswerLabel:
                if( SV.answerLine )
                    result = cclErr_DuplicateLabel;
                else
                    SV.answerLine = SV.scriptLine;
                break;

            case cHangUpLabel:
                if( SV.hangUpLine )
                    result = cclErr_DuplicateLabel;
                else
                    SV.hangUpLine = SV.scriptLine;
                break;

            case cLabel:
                result = NextInt(&labelIndex);
                if (result == 0) {
                    labelIndex--;
                    if ((labelIndex < 0) || (labelIndex >= MAXLABELS))
                        result = cclErr_BadLabel;
                    else {
                        if (SV.labels[labelIndex])
                            result = cclErr_DuplicateLabel;
                        else
                            SV.labels[labelIndex] = SV.scriptLine;
                    }
                }
                break;

                // The following cases are commands for which we check the syntax
                // to verify as much a possible that the parameters are OK
            case cExit:
                // check for exit result parameter
                result = NextInt(&labelIndex);
                // check for result in appropriate range.
                break;

            case cHSReset:
                for (i = 0; result == 0 && i < kHSResetParamCount; i++)
                    result = NextInt(&labelIndex);
                break;

            case cAsk:				// check for a integer maskflag value
            case cChrDelay:			// check for a integer delay value
            case cCommunicatingAt:		// check for a integer modem speed value
            case cIfAnswer:			// 				ditto
            case cIfOriginate:			// 				ditto
            case cJSR:				// 				ditto
            case cJump:				// 				mega ditto
            case cMatchRead:			// check for a integer timeout value
            case cPause:			// check for a integer pause time value
            case cSetSpeed:			// check for a integer interface speed value
            case cSetTries:			// check for a integer tries value
            case cUserHook:			// check for a integer opcode value
                result = NextInt(&labelIndex);
                break;

            case cIfStr:
            case cMatchStr:
                for (i = 0; result == 0 && i < (kIfStrParamCount - 1); i++)
                    result = NextInt(&labelIndex);
                SkipBlanks();
                PrepStr(SV.strBuf, NULL, NULL);
                //
                // Don't allow to match empty string
                //
                if (SV.strBuf[0] == 0 && cmd == cMatchStr)
                    result = cclErr_BadParameter;
                break;

            case cIfTries:
                for (i = 0; result == 0 && i < (kIfTriesParamCount - 1); i++)
                    result = NextInt(&labelIndex);
                break;

            case cLogMsg:			// undocumented.  use cNote.  KW
            case cNote:
            case cWrite:
                SkipBlanks();
                PrepStr(SV.strBuf, NULL, NULL);
                if( SV.strBuf[0] == 0 )
                    result = cclErr_BadParameter;
                    break;

            case cSerReset:
                for (i = 0; result == 0 && i < kSerResetParamCount; i++)
                    result = NextInt(&labelIndex);
                break;

            default:
                break;
        }/* end SWITCH on command type */
    }/* end WHILE */

    if (result == 0) {
        if (terminalMode) {
            if (SV.originateLine == 0)	result = cclErr_NoOriginateLabel;
            else SV.scriptLine = 0;
        }
        else {
            if (SV.originateLine == 0)	result = cclErr_NoOriginateLabel;
            else if (SV.answerLine == 0)	result = cclErr_NoAnswerLabel;
            else if (SV.hangUpLine == 0)	result = cclErr_NoHangUpLabel;
            else SV.scriptLine = 0;
        }

        // process the script again to check that all
        // destination labels are valid:
        while (result == 0 && NextLine()) {
            cmd = NextCommand();
            switch (cmd) {
                case cIfAnswer:
                case cIfOriginate:
                case cJump:
                case cJSR:
                    result = NextInt(&labelIndex);
                    if (result == 0) {
                        labelIndex--;
                        if ((labelIndex < 0) || (labelIndex >= MAXLABELS))
                            result = cclErr_BadLabel;
                        else {
                            if (SV.labels[ labelIndex ] == 0)
                                result = cclErr_LabelUndefined;
                        }
                    }
                    break;

                case cIfStr:
                case cIfTries:
                case cMatchStr:
                    // discard the first parameter
                    result = NextInt(&labelIndex);
                    if( result == 0 ) {
                        result = NextInt(&labelIndex);
                        if (result == 0) {
                            labelIndex--;
                            if ((labelIndex < 0) || (labelIndex >= MAXLABELS))
                                result = cclErr_BadLabel;
                            else {
                                if( SV.labels[ labelIndex ] == 0)
                                    result = cclErr_LabelUndefined;
                            }
                        }
                    }
                    break;

                case cAsk:
                    result = NextInt(&labelIndex);		// check for a integer maskflag value
                    if (result == 0) {
                        SkipBlanks();
                        PrepStr(SV.strBuf, 0, 0 );	// fetch the message Pascal string

                        // this is the optional third parameter, added for ARA 3.0, for Manual Dialing.
                        // NextInt() will return cclErr_BadParameter if the third parameter doesn't exist.

                        if (NextInt(&labelIndex) == 0) {
                            labelIndex--;
                            if ((labelIndex < 0) || (labelIndex >= MAXLABELS))
                                result = cclErr_BadLabel;
                            else {
                                if (SV.labels[labelIndex] == 0)
                                    result = cclErr_LabelUndefined;
                            }	
                        }
                    }
                    break;

                default:
                    break;
            }
        }/* end WHILE */
    }/* end IF no error */
    return result;
}

/* --------------------------------------------------------------------------
Advance to the next line of the script.
-------------------------------------------------------------------------- */
u_int8_t NextLine()
{
    u_int8_t	*bp;
    short	i;

    SV.scriptLineIndex = 0;		/* reset line index			*/
    SV.scriptLine++;				/* check for out of bounds	*/
    if ((SV.scriptLine < 1) || (SV.scriptLine > SV.lineCount))
        return 0;

    /* update the line ptr	*/
    i = SV.indexTable[SV.scriptLine-1];
    SV.scriptLinePtr = SV.script + i;

    SV.scriptLineSize = 0;
    bp = SV.scriptLinePtr;
    while ((*bp != chrCR) && (*bp != chrNL)
           && (i++ < SV.scriptSize)
           && (SV.scriptLineSize < 255)) {
        bp++;
    }
    SV.scriptLineSize = bp - SV.scriptLinePtr;// - 1;
    return 1;
}

/* --------------------------------------------------------------------------
-------------------------------------------------------------------------- */
int equalstring(u_int8_t *s1, u_int8_t *s2)
{
    int 	l1 = *s1, l2 = *s2;

    s1++; s2++;
    while (l1 && l2 && (toupper(*s1) == toupper(*s2))) {
       l1--; l2--;
       s1++; s2++;
    }
    return ((l1 == 0) && (l2 == 0));
}


/* --------------------------------------------------------------------------
Fetch the next command from the script, and set it up for processing.
Return the command number of the command found, or a helpful error
code if no command is found
-------------------------------------------------------------------------- */
int NextCommand()
{
    u_int8_t	*bp, *cmdStrPtr, cmdFound;
    int		cmdIndex, result;
    char 	text[256];
    
    result = cComment;				/* assume a blank line		*/
    SkipBlanks();				/* skip any initial blanks	*/

    bp	= SV.scriptLinePtr + SV.scriptLineIndex;
    cmdStrPtr	= &SV.strBuf[1];
    SV.strBuf[0] = 0;

    while ((SV.scriptLineIndex < SV.scriptLineSize)
           && ((*bp != chrSpace) && (*bp != chrHT))) {
        SV.scriptLineIndex++;
        SV.strBuf[0]++;
        *cmdStrPtr++ = *bp++;
    }
    bcopy(&SV.strBuf[1], &text[0], SV.strBuf[0]);
    text[SV.strBuf[0]] = 0;
    //    printf("nextcmd = strBuf = %s\n", text);
    //syslog(LOG_INFO, "nextcmd = strBuf = %s\n", text);

    if (SV.strBuf[0]) {		/* search for this command in command list */
        bp		= SV.commands + 2;
        cmdIndex	= cFirstCmd;
        result		= cNoCmd;			/* assume that we don't have a command */
        cmdFound	= 0;

        while (cmdIndex <= cLastCmd && !cmdFound) {
            if (cmdIndex == cComment && SV.strBuf[1] == bp[1]) {
                result = cComment;
                cmdFound = 1;
            }
            if( equalstring(&SV.strBuf[0], bp)) {
                result = cmdIndex;
                cmdFound = 1;
            }
            else {
                bp += *bp + 1;
                cmdIndex++;
            }
        }
    }

    return result;
}

/* --------------------------------------------------------------------------
Parse and prepare srcStr (from srcNdx) and place result in destStr;
srcStr and destStr are both pascal strings (Str255).

If the string is derived by substituting for a varString, return true
in "isVarString" and the string index in "varIndex.
-------------------------------------------------------------------------- */
void PrepStr(u_int8_t *destStr, u_int32_t *isVarString, u_int32_t *varIndex)
{
    u_int8_t	*s, *d, *maskPtr;
    u_int16_t	srcStrIndex;
    u_int8_t	done, chDelimiter, escChar, dstStrLen, centerDot = '�';

    //
    // Assume the string is not a varString.
    //
    if ( isVarString != NULL )
        *isVarString = 0;

    srcStrIndex	= SV.scriptLineIndex;
    s		= SV.scriptLinePtr + srcStrIndex;

    //
    // determine string terminators
    // default is space, tab, comma, semicolon
    //
    chDelimiter = 0;
    if ((*s == chrDblQuote) || (*s == chrQuote)) {
        chDelimiter = *s++;
        srcStrIndex++;
    }

    d		= destStr + 1;
    dstStrLen	= 0;
    done	= 0;

    while ((srcStrIndex < SV.scriptLineSize) && !done) {

        // copy & prepare the string
        maskPtr = 0;
        switch (*s) {
            // special delimiters
            case chrDblQuote:
            case chrQuote:
                if (chDelimiter == *s)
                    done = 1;
                else {
                    srcStrIndex++;
                    dstStrLen++;
                    *d++ = *s++;
                }
                break;

                // default delimiters
            case chrHT:
            case chrSpace:
            case chrComma:
            case chrSemiColon:
                if (!chDelimiter)
                    done = 1;
                else {
                    srcStrIndex++;
                    dstStrLen++;
                    *d++ = *s++;
                }
                break;

            // copy escape character into the destStr
            case chrBackSlash:
                s++;
                if ((*s == chrBackSlash) || (*s == chrCaret)) {
                    srcStrIndex += 2;
                    *d++ = *s++;
                }
                else {
                    srcStrIndex += 3;
                    escChar = ((*s++ - '0') * 10);
                    escChar += (*s++ - '0');
                    *d++ = escChar;
                }
                dstStrLen++;
                break;

                // copy the varString into the destStr
            case chrCaret:
            {
                u_int8_t	vs, *vsp;
                int 	i;

                s++;
                srcStrIndex += 2;

                switch (vs = *s++) {
                    case '*':		vs = vsAsk;		break;
                    case 'u': case 'U':	vs = vsUserName;	break;
                    case 'p': case 'P':	vs = vsPassWord;	break;
                    default: {
                        vs -= '0';
                        if (*s >= '0' && *s <= '9') {
                            srcStrIndex++;
                            vs = 10 * vs + *s++ - '0';
                        }
                        break;
                    }
                }

                if (isVarString)
                    *isVarString = 1;

                if (varIndex)
                    *varIndex = vs;

                if (vsp = GetVarString(vs)) {
                    if (SV.logMaskOn && vs == (SV.maskStringId - 1)  && SV.maskStart) {
                        maskPtr = d + SV.maskStart - 1;
                    }

                    for (i = 1; i <= *vsp; i++, dstStrLen++)
                        *d++ = vsp[i];

                    if (maskPtr) {
                        // %% centerDot = GetPasswordBulletChar();
                        for (i = SV.maskStop - SV.maskStart; i >= 0; i--)
                            *maskPtr++ = centerDot;
                    }
                }
                break;
            }

            // copy srcStr byte into the destStr
            default:
                srcStrIndex++;
                dstStrLen++;
                *d++ = *s++;
                break;
        }
    }

    *destStr = dstStrLen;			// pascal string - set length
    SV.scriptLineIndex = srcStrIndex + 1;	// skip over the string terminator
}

/* --------------------------------------------------------------------------
Skip over white space (spaces and tabs)
-------------------------------------------------------------------------- */
void SkipBlanks()
{
    u_int8_t	*s;

    s = SV.scriptLinePtr + SV.scriptLineIndex;
    while ((SV.scriptLineIndex < SV.scriptLineSize)
           && ((*s == chrSpace) || (*s == chrHT))) {
        s++;
        SV.scriptLineIndex++;
    }
}

/* --------------------------------------------------------------------------
Read the next text from the command line and convert the ascii to
an int, and set theIntPtr to the result.
If there is no int to be read, return cclErr_BadParameter.
If everything is OK, return noErr
-------------------------------------------------------------------------- */
int NextInt(u_int32_t *theIntPtr)
{
    int		sign;
    u_int8_t	*intStrPtr, intStrNdx, intStrLen;
    u_int8_t	intStr[256];

    /* skip over blanks in script line, and pull out the number string	*/
    SkipBlanks();
    PrepStr( intStr, NULL, NULL );

    *theIntPtr = 0;
    sign = 1;
    intStrLen = *intStr;
    intStrPtr = intStr + 1;
    if( intStrLen == 0 )
        return cclErr_BadParameter;

    /* check for sign operator	*/
    intStrNdx = 1;
    if (*intStrPtr == '-') {
        sign = -1;
        intStrNdx++;
        intStrPtr++;
    }
    if (intStrNdx > intStrLen)
        return cclErr_BadParameter;

    /* parse out the number	*/
    while ((intStrNdx++ <= intStrLen) && (*intStrPtr >= '0') && (*intStrPtr <= '9'))
        *theIntPtr = (*theIntPtr * 10) + (*intStrPtr++ - '0');

    if (intStrPtr == (intStr + 1))	// the string did not contain numbers
        return cclErr_BadParameter;

    *theIntPtr *= sign;				// set appropriate sign

    return 0;
}

/* --------------------------------------------------------------------------
Run the script, one line at a time, by reading the command and
dispatching to the proper routine. This continues until some command
defers the script execution. This could be due to encountering
the end of the script, or waiting for an asynchronous command to complete.
-------------------------------------------------------------------------- */
void RunScript()  
{
    u_int8_t	running, cmd, jumpLabel;
    int		result = 0;
    u_int32_t	i, exitError, lval;

    if (SV.scriptLine == 0) {			// no entry point, so terminate the script.
        SV.ctlFlags &= ~cclPlaying;				// don't play this script.
        SV.theAbortErr = cclErr_EndOfScriptErr;	// for WrapScript()->ScriptComplete().
        terminate(cclErr_EndOfScriptErr);	// a script syntax error was detected
        return;
    }

    SV.ctlFlags |= cclPlaying;		// turn on the cclPlaying flag
    running = 1;			// keep running until SerReset or other interruption
    while (running) {			// find the next script line
        if (!NextLine()) {		// no more lines in the script, so we've hit the end without an exit. 
        
            SV.ctlFlags &= ~cclPlaying;			// don't play this script.
            SV.theAbortErr = cclErr_EndOfScriptErr;	// for WrapScript()->ScriptComplete().
            terminate(cclErr_EndOfScriptErr);	// a script syntax error was detected
            return;
        }

        /* process script line	*/
        cmd = NextCommand();			// get the next CCL Command from the script.
        //printf("run : cmd = %d\n", cmd);
       switch (cmd) {
            case cAsk:
                //running = !Ask();		// wait until User data arrives in ReceiveDataFromAbove().
                break;

            case cChrDelay:
                NextInt( &i );			// delay is specified in tenths of a second
               // if (i > 0)		// CA : Why is it impossible to reset delay ?
                SV.chrDelayValue = i * 100;
                break;

            case cCommunicatingAt:
                CommunicatingAt();		// tell upstairs what our modulation speed is
                break;

            case cDecTries:
                SV.loopCounter--;		// decrement loop counter
                break;

            case cDTRClear:
                DTRCommand(DTR_CLEAR);		// Issue the DTR Clear command
                break;

            case cDTRSet:
                DTRCommand(DTR_SET);		// Issue the DTR Set command
                break;

            case cExit:
                running = 0;
                SV.ctlFlags &= ~cclPlaying;

                NextInt(&i);				// fetch the Exit code
                if (i) { 				// non-zero means trouble
                    SV.theAbortErr = i;			// will be used by WrapScript().
                    exitError = i;			// for use by terminate().
                    SkipBlanks();			// get to the string.
                    PrepStr(SV.strBuf, 0, 0);		// returns a pointer to a [optional] Pascal string.
                    terminate(exitError);	// send an ARA_Notify message upstream.
                }
                    terminate(0);	// send an ARA_Notify message upstream.
                break;

            case cFlush:
                Flush();
                break;

            case cHSReset:
                HSReset();				// Configure the serial driver handshake options
                break;

            case cIfAnswer:
                if (SV.ctlFlags & cclAnswerMode) {
                    NextInt(&i);
                    SV.scriptLine = SV.labels[i - 1];
                }
                break;

            case cIfOriginate:
                if (SV.ctlFlags & cclOriginateMode) {
                    NextInt(&i);
                    SV.scriptLine = SV.labels[i - 1];
                }
                break;

            case cIfStr:
                // IfStr() will return either 0 if the comparison fails,
                // or the label to jump to if the comparison succeeds.
                jumpLabel = IfStr();
                if (jumpLabel > 0)
                    SV.scriptLine = jumpLabel;
                break;

            case cIfTries:
                NextInt(&i);
                if (SV.loopCounter >= i) {
                    NextInt(&i);
                    SV.scriptLine = SV.labels[i - 1];
                }
                break;

            case cIncTries:
                SV.loopCounter++;					// increment the loop counter
                break;

            case cJump:
               NextInt(&i);
                SV.scriptLine = SV.labels[i - 1];
                break;

            case cJSR:
                if (SV.topOfStack == 0) {
                    running = 0;
                    SV.ctlFlags &= ~cclPlaying;
                    SV.theAbortErr = cclErr_SubroutineOverFlow;	// for WrapScript()->ScriptComplete().
                    terminate(cclErr_SubroutineOverFlow);
                }
                else {
                    SV.stack[--SV.topOfStack] = SV.scriptLine;	// save return line
                    NextInt(&i);
                    SV.scriptLine = SV.labels[i - 1];
                }
                break;

            case cLogMsg:			// undocumented.  use cNote.  KW
            case cNote:
                Note();				// post a Note to the Status window and/or Log File.
                break;

            case cMatchClr:
                MatchClr();			// clear the match buffer.
                break;

            case cMatchRead:
                SV.ctlFlags |= cclMatchPending;	// any Serial data is for CCL.
                for(i = 0; i < maxMatch; i++)	{	// reset match string indices
                    SV.matchStr[i].matchStrIndex	= 0;
                    SV.matchStr[i].inVarStr		= 0;
                }

                NextInt(&i);	// get the timeout value
                if( i > 0 ) {
                    // post read to serial driver and set timer:
                    running = 0;	// stop running script til match or timeout
                    ScheduleTimer(kMatchReadTimer, i * 100);
                }
                break;

            case cMatchStr:
                result = MatchStr();		// add a string to the match buffer
                if (result) {			// bad command and/or matchstr index
                    running = 0;
                    SV.ctlFlags &= ~cclPlaying;
                    SV.theAbortErr = result;	// for WrapScript()->ScriptComplete().
                    terminate(result);
                }
                break;

            case cPause:
                NextInt(&i);
                if (i > 0) {
                    lval = SV.pauseTimer = i;
                    lval *= 100; // get it in milliseconds
                    usleep(lval * 1000);
                }
                break;

            case cReturn:
                if (SV.topOfStack == cclNestingLimit) {
                    running = 0;
                    SV.ctlFlags &= ~cclPlaying;
                    SV.theAbortErr = cclErr_SubroutineOverFlow;	// for WrapScript()->ScriptComplete().
                    terminate(cclErr_SubroutineOverFlow);
                }
                else
                    SV.scriptLine = SV.stack[SV.topOfStack++];
                break;

            case cLBreak:
                Break(LONGBREAK);			
                break;

            case cSBreak:
                Break(SHORTBREAK);
                break;

            case cSerReset:
                SerReset();
                break;

            case cSetSpeed:
                SetSpeed();
                break;

            case cSetTries:
                NextInt(&i);
                SV.loopCounter = i;
                break;

            case cUserHook:
                UserHook();			// get an event and pass it up to the Client.
                break;

            case cWrite:
                running = Write();
                break;

            default:		// "!", "@CCLSCRIPT", "@ORIGINATE", "@ANSWER", "@HANGUP", "@LABEL" (cmd 1-6).
                break;

        }/* end SWITCH */
    }/* end WHILE running */
}

/* --------------------------------------------------------------------------
Copy the match string into the approptiate MatchStr variable
-------------------------------------------------------------------------- */
int MatchStr()
{
    u_int32_t		matchIndex, i;
    TPMatchStrInfo	matchInfo;
    int			result;

    result = NextInt(&matchIndex);			// Get the matchStr index
    if (result == 0) {
        matchIndex--;
        if ((matchIndex < 0) || (matchIndex >= maxMatch)) {	// index out of bounds, abort the script play
            return cclErr_MatchStrIndxErr;
        }
        
        matchInfo = &SV.matchStr[matchIndex];
        // get script line index from specified label
        result = NextInt(&i);
        if (result == 0) {
            matchInfo->matchLine = SV.labels[i - 1];

            SkipBlanks();

            /* determine string terminators - default is space, tab, comma, semicolon */
            if ((SV.scriptLinePtr[SV.scriptLineIndex] == chrDblQuote)
                || (SV.scriptLinePtr[SV.scriptLineIndex] == chrQuote)) {
                matchInfo->delimiterChar = SV.scriptLinePtr[SV.scriptLineIndex];
                SV.scriptLineIndex++;
            }
            else
                matchInfo->delimiterChar = 0;
            matchInfo->matchStr = &SV.scriptLinePtr[SV.scriptLineIndex];
        }
    }

    return result;
}

/* --------------------------------------------------------------------------
Clear all the match strings
-------------------------------------------------------------------------- */
void MatchClr()
{
    int			matchIndex;
    TPMatchStrInfo	matchInfo;

    for (matchIndex = 0; matchIndex < maxMatch; matchIndex++) {
        matchInfo = &SV.matchStr[matchIndex];
        matchInfo->matchStr		= 0;
        matchInfo->matchStrIndex	= 0;
        matchInfo->delimiterChar	= 0;
        matchInfo->varStr		= 0;
        matchInfo->inVarStr		= 0;
        matchInfo->varStrIndex		= 0;
        matchInfo->matchLine		= 0;
    }
}

/* --------------------------------------------------------------------------
Search each match string, at it's current index, for a match with
newChar.  If newChar matches, andvance the strings index and see if
if the whole string has been matched.  Else, reset the match string's
index.  Returns which matchstring matched, if any
-------------------------------------------------------------------------- */
int MatchFind(u_int8_t newChar)
{
    int			i, matchFound;
    TPMatchStrInfo	matchInfo;
    char text[256];
   char		matchStrChar;

    matchFound = 0;								// assume no match found
    newChar &= 0x7f;							// some PADs have bogus high bit
    for (i = 0; i < maxMatch && !matchFound; i++) {
        matchInfo = &SV.matchStr[i];
       if (matchInfo->matchStr) {		// we have a matchStr, so test it
            if (matchInfo->inVarStr) {
                matchStrChar = matchInfo->varStr[matchInfo->varStrIndex];
                matchInfo->varStrIndex++;
                if (matchStrChar == chrBackSlash) {
                    matchStrChar = matchInfo->varStr[matchInfo->varStrIndex];
                    matchInfo->varStrIndex++;
                    if ((matchStrChar !=  chrBackSlash)
                        && (matchStrChar != chrCaret)) {
                        matchStrChar = (matchStrChar - 0x30) * 10;
                        matchStrChar +=  matchInfo->varStr[matchInfo->varStrIndex] - 0x30;
                        matchInfo->varStrIndex++;
                    }
                }
                if (matchInfo->varStrIndex == (matchInfo->varStrSize + 1))
                    matchInfo->inVarStr = 0;
            }
            else {
                matchStrChar = matchInfo->matchStr[matchInfo->matchStrIndex];
                matchInfo->matchStrIndex++;			// move index to next char
                if (matchStrChar == chrBackSlash) {
                    matchStrChar = matchInfo->matchStr[matchInfo->matchStrIndex];
                    matchInfo->matchStrIndex++;		// update index for next char
                    if ((matchStrChar != chrBackSlash)
                        && (matchStrChar != chrCaret)) {
                        matchStrChar = (matchStrChar - 0x30) * 10;
                        matchStrChar += matchInfo->matchStr[matchInfo->matchStrIndex] - 0x30;
                        matchInfo->matchStrIndex++;
                    }
                }
                else if (matchStrChar == chrCaret) {
                    u_int8_t	vs;

                    matchStrChar = matchInfo->matchStr[matchInfo->matchStrIndex];
                    matchInfo->matchStrIndex++;		// skip past var string index
                    if (matchStrChar == '*')
                        vs = vsAsk;
                    else if (matchStrChar == 'u' ||  matchStrChar == 'U')
                        vs = vsUserName;
                    else if (matchStrChar == 'p' ||  matchStrChar == 'P')
                        vs = vsPassWord;
                    else {
                        vs = matchStrChar - '0';
                        matchStrChar = matchInfo->matchStr[matchInfo->matchStrIndex];
                        if (matchStrChar >= '0' && matchStrChar <= '9') {
                            matchInfo->matchStrIndex++;		// skip past var string index
                            vs = 10 * vs + matchStrChar - '0';
                        }
                    }
                    matchInfo->varStr = GetVarString(vs);
                    if (matchInfo->varStr) {
                        matchStrChar = matchInfo->varStr[1];
                        matchInfo->varStrIndex	= 2;
                        matchInfo->inVarStr	= 1;
                        matchInfo->varStrSize	= matchInfo->varStr[0];
                        if( (matchInfo->varStrSize + 1) == matchInfo->varStrIndex)
                            // we are done with the var string, clear inVarStr flag
                            matchInfo->inVarStr = 0;
                    }
                    else {
                        matchStrChar = matchInfo->matchStr[matchInfo->matchStrIndex];
                        matchInfo->matchStrIndex++;
                    }
                }
            }

            //printf("MatchFind, expect = %d, newchar = %d\n", matchStrChar, newChar);
            if (newChar == matchStrChar) {		// check to see if whole string matched
                if (!matchInfo->inVarStr)
                    switch (matchInfo->matchStr[ matchInfo->matchStrIndex ]) {
                        // special delimiters
                        case chrDblQuote:
                        case chrQuote:
                            if (matchInfo->delimiterChar)
                                matchFound = i + 1;
                            break;

                            // default delimiters
                        case chrHT:
                        case chrSpace:
                        case chrComma:
                        case chrSemiColon:
                            if (!matchInfo->delimiterChar)
                                matchFound = i + 1;
                            break;

                        case chrCR:
                        case chrNL:
                            matchFound = i + 1;
                            break;
                    }/* end SWITCH */
            }
            else {		// newChar does not match, reset matchInfo
                matchInfo->matchStrIndex = 0;
                matchInfo->inVarStr = 0;
                matchInfo->varStr = 0;
            }
        }
    }

    if (matchFound) {
        int x;

        if (matchInfo->inVarStr) {
            for (x = 0; x < matchInfo->varStrIndex; x++)
                VerboseBuffer[x+1] = matchInfo->varStr[x];
            VerboseBuffer[0] = x;
        }
        else {
            for (x = 0; x < matchInfo->matchStrIndex; x++)
               VerboseBuffer[x+1] = matchInfo->matchStr[x];
            VerboseBuffer[0] = x;
        }

        //
        // Use the kUserMsgFAny flag. That allows the message
        // resource to determine the destination and log level.
        //

        bcopy(&VerboseBuffer[1], &text[0], VerboseBuffer[0]);
        text[VerboseBuffer[0]] = 0;
        if (verbose)
            syslog(LOG_INFO, "CCLMatched : %s\n", text);
        if (ppplink != -1) {
            sys_send_confd(csockfd, PPP_CCLMATCHTEXT, text, strlen(text), ppplink, 0);
        }
    }

    return matchFound;
}

/* --------------------------------------------------------------------------
-------------------------------------------------------------------------- */
void Break(u_int32_t len)
{
    /*
     Man pages : 
     The tcsendbreak function transmits a continuous stream of zero-valued
    bits for four-tenths of a second to the terminal referenced by fd. The
    len parameter is ignored in this implementation. */
    /* we need to fix the question of Short break vs. Long Break */

    tcsendbreak(outfd, len);
}

/* --------------------------------------------------------------------------
A macro to return a Boolean result for a given baud number. Only those
listed are valid
-------------------------------------------------------------------------- */
#define IsValidBaudRate(a)		\
        (	((a) == 300)	||		\
                ((a) == 600)	||		\
                ((a) == 1200)	||		\
                ((a) == 1800)	||		\
                ((a) == 2400)	||		\
                ((a) == 3600)	||		\
                ((a) == 4800)	||		\
                ((a) == 7200)	||		\
                ((a) == 9600)	||		\
                ((a) == 14400)	||		\
                ((a) == 19200)	||		\
                ((a) == 28800)	||		\
                ((a) == 38400)	||		\
                ((a) == 57600)	||		\
                ((a) == 115200)	||		\
                ((a) == 230400))

/* --------------------------------------------------------------------------
Reset the Serial driver transmission rate, parity, number of data bits,
and number of stop bits as specified on the command line
-------------------------------------------------------------------------- */
void SerReset(void)
{
    struct termios 	tios;
    u_int32_t 		temp;

    if (tcgetattr(infd, &tios) < 0)
        return;

    // Get the baud rate from the script and filter out invalid values.
    NextInt(&temp);
    SV.serialSpeed	= IsValidBaudRate(temp) ? temp : 9600;
    cfsetispeed(&tios, SV.serialSpeed);
    cfsetospeed(&tios, SV.serialSpeed);

    // Get the parity setting from the script;
    NextInt(&temp);
    switch (temp) {
        case 1:
            tios.c_cflag |= PARENB + PARODD;     // parity enable + odd parity
            break;
        case 2:
            tios.c_cflag |= PARENB;
            tios.c_cflag &= ~PARODD;     // parity enable + even parity
            break;
        default:
            tios.c_cflag &= ~(PARENB + PARODD);     // parity enable + even parity
            break;
    }

    // Get the data bits setting from the script.
    tios.c_cflag &= ~CSIZE;     // parity enable + even parity
    NextInt(&temp);
    switch (temp) {
        case 5:
            tios.c_cflag |= CS5;     // 5 data bits
            break;
        case 6:
            tios.c_cflag |= CS6;     // 6 data bits
            break;
        case 7:
            tios.c_cflag |= CS7;     // 7 data bits
            break;
        default:
            tios.c_cflag |= CS8;     // 8 data bits
    }

    // Get the stop bits setting...
    NextInt(&temp);
    switch (temp) {
        case 2:
            tios.c_cflag |= CSTOPB;     // 2 stop bits
            break;
        default:
            tios.c_cflag &= ~CSTOPB;     // 1 stop bit
            break;
    }

    // set the same settings for inout and output fd
    if (tcsetattr(infd, TCSAFLUSH, &tios) < 0)
        syslog(LOG_INFO, "tcsetattr infd, err = %d\n", errno);
    if (tcsetattr(outfd, TCSAFLUSH, &tios) < 0)
        syslog(LOG_INFO, "tcsetattr outfd, err = %d\n", errno);
}

/* --------------------------------------------------------------------------
*	Get the serial speed from the CCL script and send it down as an
*	XTI option to the layer below.
*
*	Called by TCCLScript::RunScript().
*
*	NOTE on OT handling of speeds:
*	For speeds that return false from the IsValidBaudRate macro, the OT
*	serial module will usually return T_PARTSUCCESS and return the value
*	it likes closest to the requested value.  For OT 1.1 the exceptions are:
*		14400: sets rate to 19,200, returns T_PARTSUCCESS
*		any speed btw 100k and 230k: sets rate to 19,200, returns T_FAILURE
*		230,400: T_SUCCESS
*		any speed other speed btw 230k and 235,930, sets rate to 230,400 and
*			returns T_PARTSUCCESS
*		any speed > 235,930, set rate to 19,200 and returns T_FAILURE
-------------------------------------------------------------------------- */
void SetSpeed(void)
{
    u_int32_t 		temp;
    struct termios 	tios;

    NextInt(&temp);
    SV.serialSpeed = IsValidBaudRate(temp) ? temp : 2400;

    if (tcgetattr(infd, &tios) >= 0) {
        cfsetispeed(&tios, SV.serialSpeed);
        cfsetospeed(&tios, SV.serialSpeed);
        tcsetattr(infd, TCSAFLUSH, &tios);
        tcsetattr(outfd, TCSAFLUSH, &tios);
    }
}

/* --------------------------------------------------------------------------
pass a string up to the Client, along with where it should be displayed
-------------------------------------------------------------------------- */
void Note()
{
    char text[256];
    u_int32_t	msgDestination, msgLevel;	// will contain code for destination.

    SkipBlanks();				// get to the string.
    PrepStr(SV.strBuf, 0, 0);	// returns a pointer to a Pascal string.

    NextInt(&msgLevel);				// get the destination.
    switch (msgLevel) {
        case 1:
            msgDestination = kUserMsgFLog;
            break;
        case 2:
        default:
            msgDestination = kUserMsgFStatus;
            break;
        case 3:
            msgDestination = kUserMsgFLog | kUserMsgFStatus;
            break;
    }

    bcopy(&SV.strBuf[1], &text[0], SV.strBuf[0]);
    text[SV.strBuf[0]] = 0;
    //fprintf(stderr, "NOTE %s\n", text);
    if (verbose)
        syslog(LOG_INFO, "CCLNote : %s\n", text);
    if (ppplink != -1)
        sys_send_confd(csockfd, PPP_CCLNOTE, text, strlen(text), ppplink, 0);
}


/* --------------------------------------------------------------------------
Returns "true" if entire buffer is written with character delays. Returns
"false" if write processing continues in WriteContinue because character
delays are being inserted
-------------------------------------------------------------------------- */
u_int8_t Write()
{
    u_int32_t	isVarString;
    u_int16_t	i, j;
    int32_t	varIndex;
    char 	text[256];

    SkipBlanks();
    PrepStr(SV.strBuf, &isVarString, &varIndex );

    if (isVarString && (varIndex == vsPassWord || ((varIndex == vsAsk) && LastAskedMasked))) {
        VerboseBuffer[0] = SV.strBuf[0];
        for (i = 1; i <= SV.strBuf[0]; i++)
            VerboseBuffer[i] = '�';
    }
    else {
       for (i = 1,  j = 1; i <= SV.strBuf[0] && j < 256; i++) {
            u_int8_t c = SV.strBuf[i];
            if (c < 0x20) {
                VerboseBuffer[j++] = '\\';
                VerboseBuffer[j++] = '0' + c / 10;
                VerboseBuffer[j++] = '0' + c % 10;
            }
            else {
                VerboseBuffer[j++] = c;
            }
        }
        VerboseBuffer[0] = j - 1;
    }

    bcopy(&SV.strBuf[1], &text[0], SV.strBuf[0]);
    text[SV.strBuf[0]] = 0;
    if (verbose)
        syslog(LOG_INFO, "CCLWrite : %s\n", text);
    if (ppplink != -1)
        sys_send_confd(csockfd, PPP_CCLWRITETEXT, &VerboseBuffer[1], VerboseBuffer[0], ppplink, 0);

    //
    // skip the pascal string length byte
    //
    SV.writeBufIndex = 1;

    if (SV.chrDelayValue) {
        // complete asynchronously through WriteContinue
        WriteContinue();
        ScheduleTimer(kCharDelayTimer, SV.chrDelayValue);
        return 0;
    }
    else {
        if (terminalMode) {
            if (ppplink != -1)
                sys_send_confd(csockfd, PPP_WRITEFD, &SV.strBuf[1], SV.strBuf[0], ppplink, 0);
        }
        else
            write(outfd, &SV.strBuf[1], SV.strBuf[0]);
        
        return 1;
    }
}

/* --------------------------------------------------------------------------
Continue the current write command.  This routine is called when
the char delay timer expires
-------------------------------------------------------------------------- */
void WriteContinue()
{

    //syslog(LOG_INFO, " ----> delayed '%c'\n", SV.strBuf[SV.writeBufIndex]);

    if (terminalMode) {
        if (ppplink != -1)
            sys_send_confd(csockfd, PPP_WRITEFD, &SV.strBuf[SV.writeBufIndex], 1, ppplink, 0);
    }
    else
        write(outfd, &SV.strBuf[SV.writeBufIndex], 1);

    if (SV.writeBufIndex < SV.strBuf[0] )		// if this char is not the last char
        SV.writeBufIndex++;			// 		bump index to the next char

    return;
}

/* --------------------------------------------------------------------------
the timer scheduled by fScript -> ScriptStartTimer() has expired.
only one timer can be active at a time.  the timer type specified in
ScriptStartTimer is fed into TimerExpired as the long parameter
-------------------------------------------------------------------------- */
void TimerExpired(long type)
{

    if (SV.ctlFlags & cclPlaying) {
        switch (type)  {

            case kMatchReadTimer:
                RunScript();					// go get the next ccl cmd
                break;

            case kCharDelayTimer:
                if (SV.writeBufIndex == SV.strBuf[0] ) {		// this is the last char to write
                    WriteContinue();			// write the char
                    RunScript();				// go get the next ccl cmd
                }
                else {
                    if (SV.writeBufIndex < SV.strBuf[0]) {	// this char is not the last char
                        WriteContinue();	// write the char
                        ScheduleTimer(kCharDelayTimer, SV.chrDelayValue);
                    }
                }

                break;
        }
    }
}

/* --------------------------------------------------------------------------
report modem speed & negotiated compression to client
-------------------------------------------------------------------------- */
void CommunicatingAt()
{
    u_int32_t	 speed;

    NextInt(&speed);		// modem speed reported by script
    if (verbose)
        syslog(LOG_INFO, "CCLCommunicatingAt : %ld\n", speed);
    if (ppplink != -1)
        sys_send_confd(csockfd, PPP_CCLSPEED, (u_char*)&speed, sizeof(u_int32_t), ppplink, 0);
}

/* --------------------------------------------------------------------------
report various events to client
// UserHook 1 == claim the Serial Port for an incoming/outgoing call.
// UserHook 2 == report that the modem is doing error correction (other than MNP 10).
// UserHook 3 == request that ARA turn off data compression.
// UserHook 4 == report that the modem is doing MNP 10 error correction.
-------------------------------------------------------------------------- */
void UserHook()
{
    u_int32_t	event;			// event reported by script

    NextInt( &event );			// get the event

#if 0
    // now use a PrivateMessage to send 'event' up the Stream, to whom it may concern.

    UInt8	len = 0;
    TStreamMessage	*mp;

    len = sizeof( ARA_notify_msg ) + sizeof( UInt32 );
    mp = new ( len ) TStreamMessage;
    if( !mp )
    {
        DebugStop("\p TCCLScript::UserHook: no memory for message!");
        return;		// as if everything is OK, so we expect to fail gracefully later.
    }
    mp->Reset( len );
    mp->SetType( M_PCPROTO );

    ARA_notify_msg* PrvMsg = (ARA_notify_msg*) mp->GetDataPointer();

    PrvMsg->PRIM_type	= kARAPrvMsg;		// always.
    PrvMsg->CODE_type	= kNotifyMsg;		// always.
    PrvMsg->MODL_dst	= kAnyModuleId;		// 'any '.
    PrvMsg->MODL_src	= kScriptModuleId;	// 'scri'.
    PrvMsg->MSG_type	= kNotifyUserHook;
    PrvMsg->MSG_error	= 0;
    PrvMsg->MSG_flags	= 0;

    *((UInt32*) (PrvMsg + 1)) = event;		// put the event type just behind the header

    fScriptMod -> ScriptSendDataUp( mp );	// send this up to the Controller.
#endif
}

/* --------------------------------------------------------------------------
-------------------------------------------------------------------------- */
void ScheduleTimer(long type, u_int32_t milliseconds)
{
    if (milliseconds)
        timeout((void*)TimerExpired, (void*)type, milliseconds);
    else
        untimeout((void*)TimerExpired, (void*)type);
}

/* --------------------------------------------------------------------------
Compare the specified VarStr with the parsed string.
If the strings match, return the parsed label, else return 0
-------------------------------------------------------------------------- */
int IfStr()
{
    u_int8_t		noMatch = 0, labelIndex, *src;
    u_int32_t		i = 0, strIndex;

    NextInt( &strIndex );				// this is which varString to check

    src = GetVarString(strIndex);

    /* pull out the search string - if nil return state = continue	*/

    NextInt(&i);					// parse the jump label, if strings match
    labelIndex = SV.labels[i - 1];			// save the jump label (??)
    SkipBlanks();					// skip to the match string
    PrepStr(SV.strBuf, 0, 0);				// fetch the match string

    if (equalstring(&SV.strBuf[0], src)) {	// returns TRUE if they match...
        return labelIndex;
    }
    else {
        return noMatch;
    }
}

/* --------------------------------------------------------------------------
-------------------------------------------------------------------------- */
void ReceiveMatchData(u_int8_t nextChar)
{
    int32_t		matchIndex;

    if ((matchIndex = MatchFind(nextChar)) != 0) {
        // On a match we clear the pending flag and cancel the match timer
        SV.scriptLine = SV.matchStr[--matchIndex].matchLine;
        SV.ctlFlags &= ~cclMatchPending;
        ScheduleTimer(kMatchReadTimer, 0);

        RunScript();
    }
}

/* --------------------------------------------------------------------------
flush the read queue.  free all messages going upstream.
-------------------------------------------------------------------------- */
void Flush()
{
    tcflush(infd, TCIFLUSH);
}

/* --------------------------------------------------------------------------
Issue an asychronous call to the serial driver to set the handshaking options
-------------------------------------------------------------------------- */
void HSReset()
{
    u_int32_t	outputXON_OFF, outputCTS, XonChar, XoffChar, inputXON_OFF, inputDTR;
    struct termios 	tios;

    if (tcgetattr(outfd, &tios) < 0)
        return;

    NextInt(&outputXON_OFF);
    // if this bit is set, use the XON/XOFF chars for in-band, output flow control.
    // users shouldn't be doing this.  output of compressor could be the XOFF
    // flow control character, and thereby throttle the output stream.
    if (outputXON_OFF) 
        tios.c_iflag |= IXON;
    else 
        tios.c_iflag &= ~IXON;
    
    NextInt(&outputCTS);
    // if this bit is set, use CTS for out-band, output flow control.
    // ARA calls this signal outputCTS, while Open Transport calls it kOTSrlCTSInputHandshake.
    // we both mean that if the modem deasserts CTS, the Mac
    // shouldn't send it more data, until the modem reasserts CTS.
    // this scheme depends on a correctly wired modem cable.
    if (outputCTS)
        tios.c_cflag |= CCTS_OFLOW;
    else
        tios.c_cflag &= ~CCTS_OFLOW;

    // default onChar is 0x11, offChar is 0x13.
    // users should NEVER be specifying an alternate XonChar or XoffChar.
    // when the script is pre-flighted, any attempt to specify a char,
    // whether it's an alternate character or a default character, will barf.
    // therefore no error checking is done here.  assume these parms will be zero.  skip 'em.
    NextInt(&XonChar);
    NextInt(&XoffChar);
    tios.c_cc[VSTOP] = XoffChar;	/* DC3 = XOFF = ^S */
    tios.c_cc[VSTART] = XonChar;	/* DC1 = XON  = ^Q */

    NextInt(&inputXON_OFF);
    // if this bit is set, use the XON/XOFF chars for in-band, input flow control.
    // users shouldn't be doing this.  output of compressor could be the XOFF
    // flow control character, and thereby throttle the input stream.
    if (inputXON_OFF)
        tios.c_iflag |= IXOFF;
    else
        tios.c_iflag &= ~IXOFF;

    NextInt(&inputDTR);
    // if this bit is set, use DTR for out-band, input flow control.
    // ARA calls this signal inputDTR, while Open Transport calls it kOTSrlDTROutputHandshake.
    // we both mean that if the Mac deasserts DTR,
    // the modem shouldn't send it more data, until the Mac reasserts DTR.
    // this scheme depends on a correctly wired modem cable.
    if (inputDTR)
        tios.c_cflag |= CRTS_IFLOW;
    else
        tios.c_cflag &= ~CRTS_IFLOW;

    // set the same settings for inout and output fd
    if (tcsetattr(infd, TCSAFLUSH, &tios) < 0)
        syslog(LOG_INFO, "tcsetattr infd, err = %d\n", errno);
    if (tcsetattr(outfd, TCSAFLUSH, &tios) < 0)
        syslog(LOG_INFO, "tcsetattr outfd, err = %d\n", errno);
}

/* --------------------------------------------------------------------------
Issue an asychronous call to the serial driver to set
DTR or Clear DTR, depending on the opcode passed in
-------------------------------------------------------------------------- */
void DTRCommand(short DTRCode)
{
    ioctl(infd, DTRCode == DTR_SET ? TIOCSDTR : TIOCCDTR);
}

/* --------------------------------------------------------------------------
Send up a user notification about the current CCL exit error.
-------------------------------------------------------------------------- */
void terminate(int exitError)
{
    u_long m = exitError;

    if (verbose)
        syslog(LOG_INFO, "CCLExit : %d\n", exitError);

    if (ppplink != -1) 
        sys_send_confd(csockfd, PPP_CCLRESULT, (u_char *)&m, sizeof(m), ppplink, 0);

//    fprintf(stderr, "EXIT %d\n", exitError);
    close(infd);
    close(outfd);
    exit(exitError);
}

/* --------------------------------------------------------------------------
* timeout - Schedule a timeout.
* Note that this timeout takes the number of milliseconds, NOT hz (as in
* the kernel).
-------------------------------------------------------------------------- */
void timeout(void (*func)(void *), void *arg, u_long time)
{
    struct callout *newp, *p, **pp;

    /* Allocate timeout. */
    if ((newp = (struct callout *) malloc(sizeof(struct callout))) == NULL)
        terminate(cclErr_NoMemErr);

    newp->c_arg = arg;
    newp->c_func = func;
    gettimeofday(&timenow, NULL);
    newp->c_time.tv_sec = timenow.tv_sec + time/1000;
    newp->c_time.tv_usec = timenow.tv_usec + ((time%1000)*1000);
    if (newp->c_time.tv_usec >= 1000000) {
        newp->c_time.tv_sec++;
        newp->c_time.tv_usec -= 1000000;
    }
        
    /*
     * Find correct place and link it in.
     */
    for (pp = &callout; (p = *pp); pp = &p->c_next)
        if (newp->c_time.tv_sec < p->c_time.tv_sec
            || (newp->c_time.tv_sec == p->c_time.tv_sec
                && newp->c_time.tv_usec < p->c_time.tv_usec))
            break;
    newp->c_next = p;
    *pp = newp;
}


/* --------------------------------------------------------------------------
* untimeout - Unschedule a timeout.
-------------------------------------------------------------------------- */
void untimeout(void (*func)(void *), void *arg)
{
    struct callout **copp, *freep;

    /*
     * Find first matching timeout and remove it from the list.
     */
    for (copp = &callout; (freep = *copp); copp = &freep->c_next)
        if (freep->c_func == func && freep->c_arg == arg) {
            *copp = freep->c_next;
            //printf("untimeout [ppp%d]\n", ((struct fsm *)arg)->unit);
            free((char *) freep);
            break;
        }
}

/* --------------------------------------------------------------------------
* calltimeout - Call any timeout routines which are now due.
-------------------------------------------------------------------------- */
void calltimeout()
{
    struct callout *p;

    while (callout != NULL) {
        p = callout;

        if (gettimeofday(&timenow, NULL) < 0)
            terminate(cclErr_NoMemErr);

        if (!(p->c_time.tv_sec < timenow.tv_sec
              || (p->c_time.tv_sec == timenow.tv_sec
                  && p->c_time.tv_usec <= timenow.tv_usec)))
            break;		/* no, it's not time yet */

        callout = p->c_next;
        (*p->c_func)(p->c_arg);

        free((char *) p);
    }
}

/* --------------------------------------------------------------------------
* timeleft - return the length of time until the next timeout is due.
-------------------------------------------------------------------------- */
struct timeval *timeleft(struct timeval *tvp)
{
    if (callout == NULL)
        return NULL;

    gettimeofday(&timenow, NULL);
    tvp->tv_sec = callout->c_time.tv_sec - timenow.tv_sec;
    tvp->tv_usec = callout->c_time.tv_usec - timenow.tv_usec;
    if (tvp->tv_usec < 0) {
        tvp->tv_usec += 1000000;
        tvp->tv_sec -= 1;
    }
    if (tvp->tv_sec < 0)
        tvp->tv_sec = tvp->tv_usec = 0;

    return tvp;
}

/* --------------------------------------------------------------------------
-------------------------------------------------------------------------- */
void sys_send_confd(int sockfd, u_long code, u_char *data, u_long len, u_long link, u_char expect_result)
{
    struct ppp_msg_hdr	msg;
    char 		*tmp;

    if (sockfd == -1)
        return;

    bzero(&msg, sizeof(msg));
    msg.m_type = code;
    msg.m_len = len;
    msg.m_link = link;

    if (write(sockfd, &msg, sizeof(msg)) != sizeof(msg)) {
        return;
    }

    if (len && data && write(sockfd, data, len) != len) {
        return;
    }

    if (expect_result) {
       if (read(csockfd, &msg, sizeof(msg)) != sizeof(msg)) {
            return; //error
        }
        if (msg.m_len) {
            tmp = malloc(msg.m_len);
            if (tmp) {
                if (read(csockfd, tmp, msg.m_len) != msg.m_len) {
                    free(tmp);
                   return;//error
                }
                free(tmp);
            }
        }
    }
}
