# Master named make file.  Makes all targets.

!IF "$(CFG)" == ""
CFG=Debug
!MESSAGE No configuration specified. Defaulting to Debug.
!ENDIF 

!IF "$(CFG)" != "Release" && "$(CFG)" != "Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "named.mak" CFG="Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Release" (based on "Win32 (x86) Console Application")
!MESSAGE "Debug" (based on "Win32 (x86) Console Application")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF  "$(CFG)" == "Release"

ALL:
	-if not exist bin mkdir bin
	-cd bin
	-if not exist Release mkdir Release
	-cd ../libbind
	-nmake /f libbind.mak CFG="libbind - Win32 Release" NO_EXTERNAL_DEPS="1"
	-cd ../xfer
	-nmake /f xfer.mak CFG="xfer - Win32 Release" NO_EXTERNAL_DEPS="1"
	-cd ../named
	-nmake /f named.mak CFG="named - Win32 Release" NO_EXTERNAL_DEPS="1"
	-cd ../bindevt
	-nmake /f bindevt.mak CFG="bindevt - Win32 Release" NO_EXTERNAL_DEPS="1"
	-cd ../BINDCtrl
	-nmake /f BINDCtrl.mak CFG="BINDCtrl - Win32 Release" NO_EXTERNAL_DEPS="1"
	-cd ../BINDInstall
	-nmake /f BINDInstall.mak CFG="BINDInstall - Win32 Release" NO_EXTERNAL_DEPS="1"
	-cd ../nsupdate
	-nmake /f nsupdate.mak CFG="nsupdate - Win32 Release" NO_EXTERNAL_DEPS="1"

CLEAN:
	-cd libbind
	-nmake /f libbind.mak CFG="libbind - Win32 Release" /x NUL CLEAN
	-cd ../xfer
	-nmake /f xfer.mak CFG="xfer - Win32 Release" /x NUL CLEAN
	-cd ../named
	-nmake /f named.mak CFG="named - Win32 Release" /x NUL CLEAN
	-cd ../bindevt
	-nmake /f bindevt.mak CFG="bindevt - Win32 Release" /x NUL CLEAN
	-cd ../BINDCtrl
	-nmake /f BINDCtrl.mak CFG="BINDCtrl - Win32 Release" /x NUL CLEAN
	-cd ../BINDInstall
	-nmake /f BINDInstall.mak CFG="BINDInstall - Win32 Release" /x NUL CLEAN
	-cd ../nsupdate
	-nmake /f nsupdate.mak CFG="nsupdate - Win32 Release" /x NUL CLEAN

!ELSEIF  "$(CFG)" == "Debug"

ALL:
	-if not exist bin mkdir bin
	-cd bin
	-if not exist Debug mkdir Debug
	-cd ../libbind
	-nmake /f libbind.mak CFG="libbind - Win32 Debug" NO_EXTERNAL_DEPS="1"
	-cd ../xfer
	-nmake /f xfer.mak CFG="xfer - Win32 Debug" NO_EXTERNAL_DEPS="1"
	-cd ../named
	-nmake /f named.mak CFG="named - Win32 Debug" NO_EXTERNAL_DEPS="1"
	-cd ../bindevt
	-nmake /f bindevt.mak CFG="bindevt - Win32 Debug" NO_EXTERNAL_DEPS="1"
	-cd ../BINDCtrl
	-nmake /f BINDCtrl.mak CFG="BINDCtrl - Win32 Debug" NO_EXTERNAL_DEPS="1"
	-cd ../BINDInstall
	-nmake /f BINDInstall.mak CFG="BINDInstall - Win32 Debug" NO_EXTERNAL_DEPS="1"
	-cd ../nsupdate
	-nmake /f nsupdate.mak CFG="nsupdate - Win32 Debug" NO_EXTERNAL_DEPS="1"

CLEAN:
	-cd libbind
	-nmake /f libbind.mak CFG="libbind - Win32 Debug" /x NUL CLEAN
	-cd ../xfer
	-nmake  /f xfer.mak CFG="xfer - Win32 Debug" /x NUL CLEAN
	-cd ../named
	-nmake  /f named.mak CFG="named - Win32 Debug" /x NUL CLEAN
	-cd ../bindevt
	-nmake  /f bindevt.mak CFG="bindevt - Win32 Debug" /x NUL CLEAN
	-cd ../BINDCtrl
	-nmake  /f BINDCtrl.mak CFG="BINDCtrl - Win32 Debug" /x NUL CLEAN
	-cd ../BINDInstall
	-nmake  /f BINDInstall.mak CFG="BINDInstall - Win32 Debug" /x NUL CLEAN
	-cd ../nsupdate
	-nmake  /f nsupdate.mak CFG="nsupdate - Win32 Debug" /x NUL CLEAN

!ENDIF