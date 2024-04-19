# Microsoft Developer Studio Project File - Name="named" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

CFG=named - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "named.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "named.mak" CFG="named - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "named - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "named - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "named - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "..\include" /I "..\..\..\include" /D "NDEBUG" /D "WIN32" /D "_CONSOLE" /D "_MBCS" /D "USE_OPTIONS_H" /D "i386" /D "USE_UTIME" /D "HAVE_CUSTOM" /FR /YX /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 user32.lib ../libbind/Release/libbind.lib advapi32.lib kernel32.lib /nologo /subsystem:console /pdb:none /machine:I386 /out:"..\bin\Release\named.exe"

!ELSEIF  "$(CFG)" == "named - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /MDd /W3 /Gm /Zi /Od /I "..\include" /I "..\..\..\include" /D "_DEBUG" /D "WIN32" /D "_CONSOLE" /D "_MBCS" /D "USE_OPTIONS_H" /D "i386" /D "USE_UTIME" /D "HAVE_CUSTOM" /FR /FD /c
# SUBTRACT CPP /Gf /YX
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept
# ADD LINK32 user32.lib ../libbind/Debug/libbind.lib advapi32.lib kernel32.lib /nologo /subsystem:console /pdb:none /debug /machine:I386 /out:"..\bin\Debug\named.exe"

!ENDIF 

# Begin Target

# Name "named - Win32 Release"
# Name "named - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "*.c"
# Begin Source File

SOURCE=..\..\..\bin\named\db_dump.c
# End Source File
# Begin Source File

SOURCE=..\..\..\bin\named\db_glue.c
# End Source File
# Begin Source File

SOURCE=..\..\..\bin\named\db_ixfr.c
# End Source File
# Begin Source File

SOURCE=..\..\..\bin\named\db_load.c
# End Source File
# Begin Source File

SOURCE=..\..\..\bin\named\db_lookup.c
# End Source File
# Begin Source File

SOURCE=..\..\..\bin\named\db_save.c
# End Source File
# Begin Source File

SOURCE=..\..\..\bin\named\db_sec.c
# End Source File
# Begin Source File

SOURCE=..\..\..\bin\named\db_tsig.c
# End Source File
# Begin Source File

SOURCE=..\..\..\bin\named\db_update.c
# End Source File
# Begin Source File

SOURCE=..\..\..\bin\named\ns_config.c
# End Source File
# Begin Source File

SOURCE=..\..\..\bin\named\ns_ctl.c
# End Source File
# Begin Source File

SOURCE=..\..\..\bin\named\ns_forw.c
# End Source File
# Begin Source File

SOURCE=..\..\..\bin\named\ns_glue.c
# End Source File
# Begin Source File

SOURCE=..\..\..\bin\named\ns_init.c
# End Source File
# Begin Source File

SOURCE=..\..\..\bin\named\ns_ixfr.c
# End Source File
# Begin Source File

SOURCE=..\..\..\bin\named\ns_lexer.c
# End Source File
# Begin Source File

SOURCE=..\..\..\bin\named\ns_main.c
# End Source File
# Begin Source File

SOURCE=..\..\..\bin\named\ns_maint.c
# End Source File
# Begin Source File

SOURCE=..\..\..\bin\named\ns_ncache.c
# End Source File
# Begin Source File

SOURCE=..\..\..\bin\named\ns_notify.c
# End Source File
# Begin Source File

SOURCE=.\ns_ntmisc.c
# End Source File
# Begin Source File

SOURCE=.\ns_ntservice.c
# End Source File
# Begin Source File

SOURCE=.\ns_ntxfer.c
# End Source File
# Begin Source File

SOURCE=..\..\..\bin\named\ns_parser.c
# End Source File
# Begin Source File

SOURCE=..\..\..\bin\named\ns_parseutil.c
# End Source File
# Begin Source File

SOURCE=..\..\..\bin\named\ns_req.c
# End Source File
# Begin Source File

SOURCE=..\..\..\bin\named\ns_resp.c
# End Source File
# Begin Source File

SOURCE=..\..\..\bin\named\ns_signal.c
# End Source File
# Begin Source File

SOURCE=..\..\..\bin\named\ns_sort.c
# End Source File
# Begin Source File

SOURCE=..\..\..\bin\named\ns_stats.c
# End Source File
# Begin Source File

SOURCE=..\..\..\bin\named\ns_udp.c
# End Source File
# Begin Source File

SOURCE=..\..\..\bin\named\ns_update.c
# End Source File
# Begin Source File

SOURCE=..\..\..\bin\named\ns_xfr.c
# End Source File
# Begin Source File

SOURCE=.\version.c
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "*.h"
# Begin Source File

SOURCE=..\..\..\bin\named\db_defs.h
# End Source File
# Begin Source File

SOURCE=..\..\..\bin\named\db_func.h
# End Source File
# Begin Source File

SOURCE=..\..\..\bin\named\db_glob.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\isc\eventlib.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\isc\heap.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\arpa\inet.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\isc\logging.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\isc\misc.h
# End Source File
# Begin Source File

SOURCE=..\..\..\bin\named\named.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\arpa\nameser.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\arpa\nameser_compat.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\netdb.h
# End Source File
# Begin Source File

SOURCE=..\..\..\bin\named\ns_defs.h
# End Source File
# Begin Source File

SOURCE=..\..\..\bin\named\ns_func.h
# End Source File
# Begin Source File

SOURCE=..\..\..\bin\named\ns_glob.h
# End Source File
# Begin Source File

SOURCE=..\..\..\bin\named\ns_lexer.h
# End Source File
# Begin Source File

SOURCE=..\..\..\bin\named\ns_parser.h
# End Source File
# Begin Source File

SOURCE=..\..\..\bin\named\ns_parseutil.h
# End Source File
# Begin Source File

SOURCE=..\..\..\bin\named\pathnames.h
# End Source File
# Begin Source File

SOURCE=..\include\paths.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\resolv.h
# End Source File
# Begin Source File

SOURCE=..\include\signal.h
# End Source File
# Begin Source File

SOURCE=..\..\..\include\isc\tree.h
# End Source File
# End Group
# End Target
# End Project
