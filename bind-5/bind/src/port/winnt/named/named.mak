# Microsoft Developer Studio Generated NMAKE File, Based on named.dsp
!IF "$(CFG)" == ""
CFG=named - Win32 Debug
!MESSAGE No configuration specified. Defaulting to named - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "named - Win32 Release" && "$(CFG)" != "named - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
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
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "named - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release
# Begin Custom Macros
OutDir=.\Release
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "..\bin\Release\named.exe" "$(OUTDIR)\named.bsc"

!ELSE 

ALL : "libbind - Win32 Release" "..\bin\Release\named.exe" "$(OUTDIR)\named.bsc"

!ENDIF 

!IF "$(RECURSE)" == "1" 
CLEAN :"libbind - Win32 ReleaseCLEAN" 
!ELSE 
CLEAN :
!ENDIF 
	-@erase "$(INTDIR)\db_dump.obj"
	-@erase "$(INTDIR)\db_dump.sbr"
	-@erase "$(INTDIR)\db_glue.obj"
	-@erase "$(INTDIR)\db_glue.sbr"
	-@erase "$(INTDIR)\db_ixfr.obj"
	-@erase "$(INTDIR)\db_ixfr.sbr"
	-@erase "$(INTDIR)\db_load.obj"
	-@erase "$(INTDIR)\db_load.sbr"
	-@erase "$(INTDIR)\db_lookup.obj"
	-@erase "$(INTDIR)\db_lookup.sbr"
	-@erase "$(INTDIR)\db_save.obj"
	-@erase "$(INTDIR)\db_save.sbr"
	-@erase "$(INTDIR)\db_sec.obj"
	-@erase "$(INTDIR)\db_sec.sbr"
	-@erase "$(INTDIR)\db_tsig.obj"
	-@erase "$(INTDIR)\db_tsig.sbr"
	-@erase "$(INTDIR)\db_update.obj"
	-@erase "$(INTDIR)\db_update.sbr"
	-@erase "$(INTDIR)\ns_config.obj"
	-@erase "$(INTDIR)\ns_config.sbr"
	-@erase "$(INTDIR)\ns_ctl.obj"
	-@erase "$(INTDIR)\ns_ctl.sbr"
	-@erase "$(INTDIR)\ns_forw.obj"
	-@erase "$(INTDIR)\ns_forw.sbr"
	-@erase "$(INTDIR)\ns_glue.obj"
	-@erase "$(INTDIR)\ns_glue.sbr"
	-@erase "$(INTDIR)\ns_init.obj"
	-@erase "$(INTDIR)\ns_init.sbr"
	-@erase "$(INTDIR)\ns_ixfr.obj"
	-@erase "$(INTDIR)\ns_ixfr.sbr"
	-@erase "$(INTDIR)\ns_lexer.obj"
	-@erase "$(INTDIR)\ns_lexer.sbr"
	-@erase "$(INTDIR)\ns_main.obj"
	-@erase "$(INTDIR)\ns_main.sbr"
	-@erase "$(INTDIR)\ns_maint.obj"
	-@erase "$(INTDIR)\ns_maint.sbr"
	-@erase "$(INTDIR)\ns_ncache.obj"
	-@erase "$(INTDIR)\ns_ncache.sbr"
	-@erase "$(INTDIR)\ns_notify.obj"
	-@erase "$(INTDIR)\ns_notify.sbr"
	-@erase "$(INTDIR)\ns_ntmisc.obj"
	-@erase "$(INTDIR)\ns_ntmisc.sbr"
	-@erase "$(INTDIR)\ns_ntservice.obj"
	-@erase "$(INTDIR)\ns_ntservice.sbr"
	-@erase "$(INTDIR)\ns_ntxfer.obj"
	-@erase "$(INTDIR)\ns_ntxfer.sbr"
	-@erase "$(INTDIR)\ns_parser.obj"
	-@erase "$(INTDIR)\ns_parser.sbr"
	-@erase "$(INTDIR)\ns_parseutil.obj"
	-@erase "$(INTDIR)\ns_parseutil.sbr"
	-@erase "$(INTDIR)\ns_req.obj"
	-@erase "$(INTDIR)\ns_req.sbr"
	-@erase "$(INTDIR)\ns_resp.obj"
	-@erase "$(INTDIR)\ns_resp.sbr"
	-@erase "$(INTDIR)\ns_signal.obj"
	-@erase "$(INTDIR)\ns_signal.sbr"
	-@erase "$(INTDIR)\ns_sort.obj"
	-@erase "$(INTDIR)\ns_sort.sbr"
	-@erase "$(INTDIR)\ns_stats.obj"
	-@erase "$(INTDIR)\ns_stats.sbr"
	-@erase "$(INTDIR)\ns_udp.obj"
	-@erase "$(INTDIR)\ns_udp.sbr"
	-@erase "$(INTDIR)\ns_update.obj"
	-@erase "$(INTDIR)\ns_update.sbr"
	-@erase "$(INTDIR)\ns_xfr.obj"
	-@erase "$(INTDIR)\ns_xfr.sbr"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\version.obj"
	-@erase "$(INTDIR)\version.sbr"
	-@erase "$(OUTDIR)\named.bsc"
	-@erase "..\bin\Release\named.exe"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MD /W3 /GX /O2 /I "..\include" /I "..\..\..\include" /D "NDEBUG" /D "WIN32" /D "_CONSOLE" /D "_MBCS" /D "USE_OPTIONS_H" /D "i386" /D "USE_UTIME" /D "HAVE_CUSTOM" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\named.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\named.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\db_dump.sbr" \
	"$(INTDIR)\db_glue.sbr" \
	"$(INTDIR)\db_ixfr.sbr" \
	"$(INTDIR)\db_load.sbr" \
	"$(INTDIR)\db_lookup.sbr" \
	"$(INTDIR)\db_save.sbr" \
	"$(INTDIR)\db_sec.sbr" \
	"$(INTDIR)\db_tsig.sbr" \
	"$(INTDIR)\db_update.sbr" \
	"$(INTDIR)\ns_config.sbr" \
	"$(INTDIR)\ns_ctl.sbr" \
	"$(INTDIR)\ns_forw.sbr" \
	"$(INTDIR)\ns_glue.sbr" \
	"$(INTDIR)\ns_init.sbr" \
	"$(INTDIR)\ns_ixfr.sbr" \
	"$(INTDIR)\ns_lexer.sbr" \
	"$(INTDIR)\ns_main.sbr" \
	"$(INTDIR)\ns_maint.sbr" \
	"$(INTDIR)\ns_ncache.sbr" \
	"$(INTDIR)\ns_notify.sbr" \
	"$(INTDIR)\ns_ntmisc.sbr" \
	"$(INTDIR)\ns_ntservice.sbr" \
	"$(INTDIR)\ns_ntxfer.sbr" \
	"$(INTDIR)\ns_parser.sbr" \
	"$(INTDIR)\ns_parseutil.sbr" \
	"$(INTDIR)\ns_req.sbr" \
	"$(INTDIR)\ns_resp.sbr" \
	"$(INTDIR)\ns_signal.sbr" \
	"$(INTDIR)\ns_sort.sbr" \
	"$(INTDIR)\ns_stats.sbr" \
	"$(INTDIR)\ns_udp.sbr" \
	"$(INTDIR)\ns_update.sbr" \
	"$(INTDIR)\ns_xfr.sbr" \
	"$(INTDIR)\version.sbr"

"$(OUTDIR)\named.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
LINK32_FLAGS=user32.lib ../libbind/Release/libbind.lib advapi32.lib kernel32.lib /nologo /subsystem:console /pdb:none /machine:I386 /out:"..\bin\Release\named.exe" 
LINK32_OBJS= \
	"$(INTDIR)\db_dump.obj" \
	"$(INTDIR)\db_glue.obj" \
	"$(INTDIR)\db_ixfr.obj" \
	"$(INTDIR)\db_load.obj" \
	"$(INTDIR)\db_lookup.obj" \
	"$(INTDIR)\db_save.obj" \
	"$(INTDIR)\db_sec.obj" \
	"$(INTDIR)\db_tsig.obj" \
	"$(INTDIR)\db_update.obj" \
	"$(INTDIR)\ns_config.obj" \
	"$(INTDIR)\ns_ctl.obj" \
	"$(INTDIR)\ns_forw.obj" \
	"$(INTDIR)\ns_glue.obj" \
	"$(INTDIR)\ns_init.obj" \
	"$(INTDIR)\ns_ixfr.obj" \
	"$(INTDIR)\ns_lexer.obj" \
	"$(INTDIR)\ns_main.obj" \
	"$(INTDIR)\ns_maint.obj" \
	"$(INTDIR)\ns_ncache.obj" \
	"$(INTDIR)\ns_notify.obj" \
	"$(INTDIR)\ns_ntmisc.obj" \
	"$(INTDIR)\ns_ntservice.obj" \
	"$(INTDIR)\ns_ntxfer.obj" \
	"$(INTDIR)\ns_parser.obj" \
	"$(INTDIR)\ns_parseutil.obj" \
	"$(INTDIR)\ns_req.obj" \
	"$(INTDIR)\ns_resp.obj" \
	"$(INTDIR)\ns_signal.obj" \
	"$(INTDIR)\ns_sort.obj" \
	"$(INTDIR)\ns_stats.obj" \
	"$(INTDIR)\ns_udp.obj" \
	"$(INTDIR)\ns_update.obj" \
	"$(INTDIR)\ns_xfr.obj" \
	"$(INTDIR)\version.obj" \
	"..\libbind\Release\libbind.lib"

"..\bin\Release\named.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "named - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug
# Begin Custom Macros
OutDir=.\Debug
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "..\bin\Debug\named.exe" "$(OUTDIR)\named.bsc"

!ELSE 

ALL : "libbind - Win32 Debug" "..\bin\Debug\named.exe" "$(OUTDIR)\named.bsc"

!ENDIF 

!IF "$(RECURSE)" == "1" 
CLEAN :"libbind - Win32 DebugCLEAN" 
!ELSE 
CLEAN :
!ENDIF 
	-@erase "$(INTDIR)\db_dump.obj"
	-@erase "$(INTDIR)\db_dump.sbr"
	-@erase "$(INTDIR)\db_glue.obj"
	-@erase "$(INTDIR)\db_glue.sbr"
	-@erase "$(INTDIR)\db_ixfr.obj"
	-@erase "$(INTDIR)\db_ixfr.sbr"
	-@erase "$(INTDIR)\db_load.obj"
	-@erase "$(INTDIR)\db_load.sbr"
	-@erase "$(INTDIR)\db_lookup.obj"
	-@erase "$(INTDIR)\db_lookup.sbr"
	-@erase "$(INTDIR)\db_save.obj"
	-@erase "$(INTDIR)\db_save.sbr"
	-@erase "$(INTDIR)\db_sec.obj"
	-@erase "$(INTDIR)\db_sec.sbr"
	-@erase "$(INTDIR)\db_tsig.obj"
	-@erase "$(INTDIR)\db_tsig.sbr"
	-@erase "$(INTDIR)\db_update.obj"
	-@erase "$(INTDIR)\db_update.sbr"
	-@erase "$(INTDIR)\ns_config.obj"
	-@erase "$(INTDIR)\ns_config.sbr"
	-@erase "$(INTDIR)\ns_ctl.obj"
	-@erase "$(INTDIR)\ns_ctl.sbr"
	-@erase "$(INTDIR)\ns_forw.obj"
	-@erase "$(INTDIR)\ns_forw.sbr"
	-@erase "$(INTDIR)\ns_glue.obj"
	-@erase "$(INTDIR)\ns_glue.sbr"
	-@erase "$(INTDIR)\ns_init.obj"
	-@erase "$(INTDIR)\ns_init.sbr"
	-@erase "$(INTDIR)\ns_ixfr.obj"
	-@erase "$(INTDIR)\ns_ixfr.sbr"
	-@erase "$(INTDIR)\ns_lexer.obj"
	-@erase "$(INTDIR)\ns_lexer.sbr"
	-@erase "$(INTDIR)\ns_main.obj"
	-@erase "$(INTDIR)\ns_main.sbr"
	-@erase "$(INTDIR)\ns_maint.obj"
	-@erase "$(INTDIR)\ns_maint.sbr"
	-@erase "$(INTDIR)\ns_ncache.obj"
	-@erase "$(INTDIR)\ns_ncache.sbr"
	-@erase "$(INTDIR)\ns_notify.obj"
	-@erase "$(INTDIR)\ns_notify.sbr"
	-@erase "$(INTDIR)\ns_ntmisc.obj"
	-@erase "$(INTDIR)\ns_ntmisc.sbr"
	-@erase "$(INTDIR)\ns_ntservice.obj"
	-@erase "$(INTDIR)\ns_ntservice.sbr"
	-@erase "$(INTDIR)\ns_ntxfer.obj"
	-@erase "$(INTDIR)\ns_ntxfer.sbr"
	-@erase "$(INTDIR)\ns_parser.obj"
	-@erase "$(INTDIR)\ns_parser.sbr"
	-@erase "$(INTDIR)\ns_parseutil.obj"
	-@erase "$(INTDIR)\ns_parseutil.sbr"
	-@erase "$(INTDIR)\ns_req.obj"
	-@erase "$(INTDIR)\ns_req.sbr"
	-@erase "$(INTDIR)\ns_resp.obj"
	-@erase "$(INTDIR)\ns_resp.sbr"
	-@erase "$(INTDIR)\ns_signal.obj"
	-@erase "$(INTDIR)\ns_signal.sbr"
	-@erase "$(INTDIR)\ns_sort.obj"
	-@erase "$(INTDIR)\ns_sort.sbr"
	-@erase "$(INTDIR)\ns_stats.obj"
	-@erase "$(INTDIR)\ns_stats.sbr"
	-@erase "$(INTDIR)\ns_udp.obj"
	-@erase "$(INTDIR)\ns_udp.sbr"
	-@erase "$(INTDIR)\ns_update.obj"
	-@erase "$(INTDIR)\ns_update.sbr"
	-@erase "$(INTDIR)\ns_xfr.obj"
	-@erase "$(INTDIR)\ns_xfr.sbr"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(INTDIR)\version.obj"
	-@erase "$(INTDIR)\version.sbr"
	-@erase "$(OUTDIR)\named.bsc"
	-@erase "..\bin\Debug\named.exe"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MDd /W3 /Gm /Zi /Od /I "..\include" /I "..\..\..\include" /D "_DEBUG" /D "WIN32" /D "_CONSOLE" /D "_MBCS" /D "USE_OPTIONS_H" /D "i386" /D "USE_UTIME" /D "HAVE_CUSTOM" /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\named.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\db_dump.sbr" \
	"$(INTDIR)\db_glue.sbr" \
	"$(INTDIR)\db_ixfr.sbr" \
	"$(INTDIR)\db_load.sbr" \
	"$(INTDIR)\db_lookup.sbr" \
	"$(INTDIR)\db_save.sbr" \
	"$(INTDIR)\db_sec.sbr" \
	"$(INTDIR)\db_tsig.sbr" \
	"$(INTDIR)\db_update.sbr" \
	"$(INTDIR)\ns_config.sbr" \
	"$(INTDIR)\ns_ctl.sbr" \
	"$(INTDIR)\ns_forw.sbr" \
	"$(INTDIR)\ns_glue.sbr" \
	"$(INTDIR)\ns_init.sbr" \
	"$(INTDIR)\ns_ixfr.sbr" \
	"$(INTDIR)\ns_lexer.sbr" \
	"$(INTDIR)\ns_main.sbr" \
	"$(INTDIR)\ns_maint.sbr" \
	"$(INTDIR)\ns_ncache.sbr" \
	"$(INTDIR)\ns_notify.sbr" \
	"$(INTDIR)\ns_ntmisc.sbr" \
	"$(INTDIR)\ns_ntservice.sbr" \
	"$(INTDIR)\ns_ntxfer.sbr" \
	"$(INTDIR)\ns_parser.sbr" \
	"$(INTDIR)\ns_parseutil.sbr" \
	"$(INTDIR)\ns_req.sbr" \
	"$(INTDIR)\ns_resp.sbr" \
	"$(INTDIR)\ns_signal.sbr" \
	"$(INTDIR)\ns_sort.sbr" \
	"$(INTDIR)\ns_stats.sbr" \
	"$(INTDIR)\ns_udp.sbr" \
	"$(INTDIR)\ns_update.sbr" \
	"$(INTDIR)\ns_xfr.sbr" \
	"$(INTDIR)\version.sbr"

"$(OUTDIR)\named.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
LINK32_FLAGS=user32.lib ../libbind/Debug/libbind.lib advapi32.lib kernel32.lib /nologo /subsystem:console /pdb:none /debug /machine:I386 /out:"..\bin\Debug\named.exe" 
LINK32_OBJS= \
	"$(INTDIR)\db_dump.obj" \
	"$(INTDIR)\db_glue.obj" \
	"$(INTDIR)\db_ixfr.obj" \
	"$(INTDIR)\db_load.obj" \
	"$(INTDIR)\db_lookup.obj" \
	"$(INTDIR)\db_save.obj" \
	"$(INTDIR)\db_sec.obj" \
	"$(INTDIR)\db_tsig.obj" \
	"$(INTDIR)\db_update.obj" \
	"$(INTDIR)\ns_config.obj" \
	"$(INTDIR)\ns_ctl.obj" \
	"$(INTDIR)\ns_forw.obj" \
	"$(INTDIR)\ns_glue.obj" \
	"$(INTDIR)\ns_init.obj" \
	"$(INTDIR)\ns_ixfr.obj" \
	"$(INTDIR)\ns_lexer.obj" \
	"$(INTDIR)\ns_main.obj" \
	"$(INTDIR)\ns_maint.obj" \
	"$(INTDIR)\ns_ncache.obj" \
	"$(INTDIR)\ns_notify.obj" \
	"$(INTDIR)\ns_ntmisc.obj" \
	"$(INTDIR)\ns_ntservice.obj" \
	"$(INTDIR)\ns_ntxfer.obj" \
	"$(INTDIR)\ns_parser.obj" \
	"$(INTDIR)\ns_parseutil.obj" \
	"$(INTDIR)\ns_req.obj" \
	"$(INTDIR)\ns_resp.obj" \
	"$(INTDIR)\ns_signal.obj" \
	"$(INTDIR)\ns_sort.obj" \
	"$(INTDIR)\ns_stats.obj" \
	"$(INTDIR)\ns_udp.obj" \
	"$(INTDIR)\ns_update.obj" \
	"$(INTDIR)\ns_xfr.obj" \
	"$(INTDIR)\version.obj" \
	"..\libbind\Debug\libbind.lib"

"..\bin\Debug\named.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 

.c{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.c{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<


!IF "$(NO_EXTERNAL_DEPS)" != "1"
!IF EXISTS("named.dep")
!INCLUDE "named.dep"
!ELSE 
!MESSAGE Warning: cannot find "named.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "named - Win32 Release" || "$(CFG)" == "named - Win32 Debug"
SOURCE=..\..\..\bin\named\db_dump.c

"$(INTDIR)\db_dump.obj"	"$(INTDIR)\db_dump.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\bin\named\db_glue.c

"$(INTDIR)\db_glue.obj"	"$(INTDIR)\db_glue.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\bin\named\db_ixfr.c

"$(INTDIR)\db_ixfr.obj"	"$(INTDIR)\db_ixfr.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\bin\named\db_load.c

"$(INTDIR)\db_load.obj"	"$(INTDIR)\db_load.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\bin\named\db_lookup.c

"$(INTDIR)\db_lookup.obj"	"$(INTDIR)\db_lookup.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\bin\named\db_save.c

"$(INTDIR)\db_save.obj"	"$(INTDIR)\db_save.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\bin\named\db_sec.c

"$(INTDIR)\db_sec.obj"	"$(INTDIR)\db_sec.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\bin\named\db_tsig.c

"$(INTDIR)\db_tsig.obj"	"$(INTDIR)\db_tsig.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\bin\named\db_update.c

"$(INTDIR)\db_update.obj"	"$(INTDIR)\db_update.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\bin\named\ns_config.c

"$(INTDIR)\ns_config.obj"	"$(INTDIR)\ns_config.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\bin\named\ns_ctl.c

"$(INTDIR)\ns_ctl.obj"	"$(INTDIR)\ns_ctl.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\bin\named\ns_forw.c

"$(INTDIR)\ns_forw.obj"	"$(INTDIR)\ns_forw.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\bin\named\ns_glue.c

"$(INTDIR)\ns_glue.obj"	"$(INTDIR)\ns_glue.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\bin\named\ns_init.c

"$(INTDIR)\ns_init.obj"	"$(INTDIR)\ns_init.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\bin\named\ns_ixfr.c

"$(INTDIR)\ns_ixfr.obj"	"$(INTDIR)\ns_ixfr.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\bin\named\ns_lexer.c

"$(INTDIR)\ns_lexer.obj"	"$(INTDIR)\ns_lexer.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\bin\named\ns_main.c

"$(INTDIR)\ns_main.obj"	"$(INTDIR)\ns_main.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\bin\named\ns_maint.c

"$(INTDIR)\ns_maint.obj"	"$(INTDIR)\ns_maint.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\bin\named\ns_ncache.c

"$(INTDIR)\ns_ncache.obj"	"$(INTDIR)\ns_ncache.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\bin\named\ns_notify.c

"$(INTDIR)\ns_notify.obj"	"$(INTDIR)\ns_notify.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\ns_ntmisc.c

"$(INTDIR)\ns_ntmisc.obj"	"$(INTDIR)\ns_ntmisc.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\ns_ntservice.c

"$(INTDIR)\ns_ntservice.obj"	"$(INTDIR)\ns_ntservice.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\ns_ntxfer.c

"$(INTDIR)\ns_ntxfer.obj"	"$(INTDIR)\ns_ntxfer.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=..\..\..\bin\named\ns_parser.c

"$(INTDIR)\ns_parser.obj"	"$(INTDIR)\ns_parser.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\bin\named\ns_parseutil.c

"$(INTDIR)\ns_parseutil.obj"	"$(INTDIR)\ns_parseutil.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\bin\named\ns_req.c

"$(INTDIR)\ns_req.obj"	"$(INTDIR)\ns_req.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\bin\named\ns_resp.c

"$(INTDIR)\ns_resp.obj"	"$(INTDIR)\ns_resp.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\bin\named\ns_signal.c

"$(INTDIR)\ns_signal.obj"	"$(INTDIR)\ns_signal.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\bin\named\ns_sort.c

"$(INTDIR)\ns_sort.obj"	"$(INTDIR)\ns_sort.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\bin\named\ns_stats.c

"$(INTDIR)\ns_stats.obj"	"$(INTDIR)\ns_stats.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\bin\named\ns_udp.c

"$(INTDIR)\ns_udp.obj"	"$(INTDIR)\ns_udp.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\bin\named\ns_update.c

"$(INTDIR)\ns_update.obj"	"$(INTDIR)\ns_update.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\bin\named\ns_xfr.c

"$(INTDIR)\ns_xfr.obj"	"$(INTDIR)\ns_xfr.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\version.c

"$(INTDIR)\version.obj"	"$(INTDIR)\version.sbr" : $(SOURCE) "$(INTDIR)"


!IF  "$(CFG)" == "named - Win32 Release"

"libbind - Win32 Release" : 
   cd "\bind8.2.3-T6B\src\port\winnt\libbind"
   $(MAKE) /$(MAKEFLAGS) /F ".\libbind.mak" CFG="libbind - Win32 Release" 
   cd "..\named"

"libbind - Win32 ReleaseCLEAN" : 
   cd "\bind8.2.3-T6B\src\port\winnt\libbind"
   $(MAKE) /$(MAKEFLAGS) /F ".\libbind.mak" CFG="libbind - Win32 Release" RECURSE=1 CLEAN 
   cd "..\named"

!ELSEIF  "$(CFG)" == "named - Win32 Debug"

"libbind - Win32 Debug" : 
   cd "\bind8.2.3-T6B\src\port\winnt\libbind"
   $(MAKE) /$(MAKEFLAGS) /F ".\libbind.mak" CFG="libbind - Win32 Debug" 
   cd "..\named"

"libbind - Win32 DebugCLEAN" : 
   cd "\bind8.2.3-T6B\src\port\winnt\libbind"
   $(MAKE) /$(MAKEFLAGS) /F ".\libbind.mak" CFG="libbind - Win32 Debug" RECURSE=1 CLEAN 
   cd "..\named"

!ENDIF 


!ENDIF 

