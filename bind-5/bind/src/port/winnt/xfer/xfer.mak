# Microsoft Developer Studio Generated NMAKE File, Based on xfer.dsp
!IF "$(CFG)" == ""
CFG=xfer - Win32 Debug
!MESSAGE No configuration specified. Defaulting to xfer - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "xfer - Win32 Release" && "$(CFG)" != "xfer - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "xfer.mak" CFG="xfer - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "xfer - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "xfer - Win32 Debug" (based on "Win32 (x86) Console Application")
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

!IF  "$(CFG)" == "xfer - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release
# Begin Custom Macros
OutDir=.\Release
# End Custom Macros

ALL : "..\bin\Release\named-xfer.exe" "$(OUTDIR)\xfer.bsc"


CLEAN :
	-@erase "$(INTDIR)\db_glue.obj"
	-@erase "$(INTDIR)\db_glue.sbr"
	-@erase "$(INTDIR)\named-xfer.obj"
	-@erase "$(INTDIR)\named-xfer.sbr"
	-@erase "$(INTDIR)\ns_glue.obj"
	-@erase "$(INTDIR)\ns_glue.sbr"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\version.obj"
	-@erase "$(INTDIR)\version.sbr"
	-@erase "$(OUTDIR)\xfer.bsc"
	-@erase "..\bin\Release\named-xfer.exe"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MD /W3 /GX /O2 /I "..\include" /I "..\..\..\include" /D "NDEBUG" /D "WIN32" /D "_CONSOLE" /D "_MBCS" /D "i386" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\xfer.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\xfer.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\db_glue.sbr" \
	"$(INTDIR)\named-xfer.sbr" \
	"$(INTDIR)\ns_glue.sbr" \
	"$(INTDIR)\version.sbr"

"$(OUTDIR)\xfer.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
LINK32_FLAGS=../libbind/Release/libbind.lib /nologo /subsystem:console /pdb:none /machine:I386 /out:"..\bin\Release\named-xfer.exe" 
LINK32_OBJS= \
	"$(INTDIR)\db_glue.obj" \
	"$(INTDIR)\named-xfer.obj" \
	"$(INTDIR)\ns_glue.obj" \
	"$(INTDIR)\version.obj"

"..\bin\Release\named-xfer.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "xfer - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug
# Begin Custom Macros
OutDir=.\Debug
# End Custom Macros

ALL : "..\bin\Debug\named-xfer.exe" "$(OUTDIR)\xfer.bsc"


CLEAN :
	-@erase "$(INTDIR)\db_glue.obj"
	-@erase "$(INTDIR)\db_glue.sbr"
	-@erase "$(INTDIR)\named-xfer.obj"
	-@erase "$(INTDIR)\named-xfer.sbr"
	-@erase "$(INTDIR)\ns_glue.obj"
	-@erase "$(INTDIR)\ns_glue.sbr"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(INTDIR)\version.obj"
	-@erase "$(INTDIR)\version.sbr"
	-@erase "$(OUTDIR)\xfer.bsc"
	-@erase "..\bin\Debug\named-xfer.exe"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MDd /W3 /Gm /GX /Zi /Od /I "..\include" /I "..\..\..\include" /D "_DEBUG" /D "WIN32" /D "_CONSOLE" /D "_MBCS" /D "i386" /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\xfer.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\db_glue.sbr" \
	"$(INTDIR)\named-xfer.sbr" \
	"$(INTDIR)\ns_glue.sbr" \
	"$(INTDIR)\version.sbr"

"$(OUTDIR)\xfer.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
LINK32_FLAGS=../libbind/Debug/libbind.lib /nologo /subsystem:console /pdb:none /debug /machine:I386 /out:"..\bin\Debug\named-xfer.exe" 
LINK32_OBJS= \
	"$(INTDIR)\db_glue.obj" \
	"$(INTDIR)\named-xfer.obj" \
	"$(INTDIR)\ns_glue.obj" \
	"$(INTDIR)\version.obj"

"..\bin\Debug\named-xfer.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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
!IF EXISTS("xfer.dep")
!INCLUDE "xfer.dep"
!ELSE 
!MESSAGE Warning: cannot find "xfer.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "xfer - Win32 Release" || "$(CFG)" == "xfer - Win32 Debug"
SOURCE=..\..\..\bin\named\db_glue.c

"$(INTDIR)\db_glue.obj"	"$(INTDIR)\db_glue.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE="..\..\..\bin\named-xfer\named-xfer.c"

"$(INTDIR)\named-xfer.obj"	"$(INTDIR)\named-xfer.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\bin\named\ns_glue.c

"$(INTDIR)\ns_glue.obj"	"$(INTDIR)\ns_glue.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\named\version.c

"$(INTDIR)\version.obj"	"$(INTDIR)\version.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)



!ENDIF 

