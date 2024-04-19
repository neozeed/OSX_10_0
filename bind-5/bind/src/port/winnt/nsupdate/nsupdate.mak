# Microsoft Developer Studio Generated NMAKE File, Based on nsupdate.dsp
!IF "$(CFG)" == ""
CFG=nsupdate - Win32 Debug
!MESSAGE No configuration specified. Defaulting to nsupdate - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "nsupdate - Win32 Release" && "$(CFG)" != "nsupdate - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "nsupdate.mak" CFG="nsupdate - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "nsupdate - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "nsupdate - Win32 Debug" (based on "Win32 (x86) Console Application")
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

!IF  "$(CFG)" == "nsupdate - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release
# Begin Custom Macros
OutDir=.\Release
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "..\bin\Release\nsupdate.exe" "$(OUTDIR)\nsupdate.bsc"

!ELSE 

ALL : "libbind - Win32 Release" "..\bin\Release\nsupdate.exe" "$(OUTDIR)\nsupdate.bsc"

!ENDIF 

!IF "$(RECURSE)" == "1" 
CLEAN :"libbind - Win32 ReleaseCLEAN" 
!ELSE 
CLEAN :
!ENDIF 
	-@erase "$(INTDIR)\nsupdate.obj"
	-@erase "$(INTDIR)\nsupdate.sbr"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\nsupdate.bsc"
	-@erase "..\bin\Release\nsupdate.exe"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MD /W3 /GX /O2 /I "..\include" /I "..\..\..\include" /D "NDEBUG" /D "WIN32" /D "_CONSOLE" /D "_MBCS" /D "i386" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\nsupdate.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\nsupdate.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\nsupdate.sbr"

"$(OUTDIR)\nsupdate.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
LINK32_FLAGS=../libbind/Release/libbind.lib /nologo /subsystem:console /incremental:no /pdb:"$(OUTDIR)\nsupdate.pdb" /machine:I386 /out:"..\bin\Release\nsupdate.exe" 
LINK32_OBJS= \
	"$(INTDIR)\nsupdate.obj" \
	"..\libbind\Release\libbind.lib"

"..\bin\Release\nsupdate.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "nsupdate - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug
# Begin Custom Macros
OutDir=.\Debug
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "..\bin\Debug\nsupdate.exe" "$(OUTDIR)\nsupdate.bsc"

!ELSE 

ALL : "libbind - Win32 Debug" "..\bin\Debug\nsupdate.exe" "$(OUTDIR)\nsupdate.bsc"

!ENDIF 

!IF "$(RECURSE)" == "1" 
CLEAN :"libbind - Win32 DebugCLEAN" 
!ELSE 
CLEAN :
!ENDIF 
	-@erase "$(INTDIR)\nsupdate.obj"
	-@erase "$(INTDIR)\nsupdate.sbr"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\nsupdate.bsc"
	-@erase "..\bin\Debug\nsupdate.exe"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\include" /I "..\..\..\include" /D "_DEBUG" /D "i386" /D "WIN32" /D "_CONSOLE" /D "_MBCS" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\nsupdate.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\nsupdate.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\nsupdate.sbr"

"$(OUTDIR)\nsupdate.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
LINK32_FLAGS=../libbind/Debug/libbind.lib /nologo /subsystem:console /pdb:none /debug /machine:I386 /out:"..\bin\Debug\nsupdate.exe" 
LINK32_OBJS= \
	"$(INTDIR)\nsupdate.obj" \
	"..\libbind\Debug\libbind.lib"

"..\bin\Debug\nsupdate.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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
!IF EXISTS("nsupdate.dep")
!INCLUDE "nsupdate.dep"
!ELSE 
!MESSAGE Warning: cannot find "nsupdate.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "nsupdate - Win32 Release" || "$(CFG)" == "nsupdate - Win32 Debug"

!IF  "$(CFG)" == "nsupdate - Win32 Release"

"libbind - Win32 Release" : 
   cd "\bind8.2.3-T6B\src\port\winnt\libbind"
   $(MAKE) /$(MAKEFLAGS) /F ".\libbind.mak" CFG="libbind - Win32 Release" 
   cd "..\nsupdate"

"libbind - Win32 ReleaseCLEAN" : 
   cd "\bind8.2.3-T6B\src\port\winnt\libbind"
   $(MAKE) /$(MAKEFLAGS) /F ".\libbind.mak" CFG="libbind - Win32 Release" RECURSE=1 CLEAN 
   cd "..\nsupdate"

!ELSEIF  "$(CFG)" == "nsupdate - Win32 Debug"

"libbind - Win32 Debug" : 
   cd "\bind8.2.3-T6B\src\port\winnt\libbind"
   $(MAKE) /$(MAKEFLAGS) /F ".\libbind.mak" CFG="libbind - Win32 Debug" 
   cd "..\nsupdate"

"libbind - Win32 DebugCLEAN" : 
   cd "\bind8.2.3-T6B\src\port\winnt\libbind"
   $(MAKE) /$(MAKEFLAGS) /F ".\libbind.mak" CFG="libbind - Win32 Debug" RECURSE=1 CLEAN 
   cd "..\nsupdate"

!ENDIF 

SOURCE=..\..\..\bin\nsupdate\nsupdate.c

"$(INTDIR)\nsupdate.obj"	"$(INTDIR)\nsupdate.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)



!ENDIF 

