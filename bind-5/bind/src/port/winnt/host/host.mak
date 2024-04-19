# Microsoft Developer Studio Generated NMAKE File, Based on host.dsp
!IF "$(CFG)" == ""
CFG=host - Win32 Debug
!MESSAGE No configuration specified. Defaulting to host - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "host - Win32 Release" && "$(CFG)" != "host - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "host.mak" CFG="host - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "host - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "host - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

!IF  "$(CFG)" == "host - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release

ALL : "..\bin\Release\host.exe"


CLEAN :
	-@erase "$(INTDIR)\host.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "..\bin\Release\host.exe"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MD /W3 /GX /O2 /I "..\include" /I "..\..\..\include" /D "NDEBUG" /D "WIN32" /D "_CONSOLE" /D "_MBCS" /D "i386" /Fp"$(INTDIR)\host.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

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

RSC=rc.exe
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\host.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=../libbind/Release/libbind.lib /nologo /subsystem:console /incremental:no /pdb:"$(OUTDIR)\host.pdb" /machine:I386 /out:"..\bin\Release\host.exe" 
LINK32_OBJS= \
	"$(INTDIR)\host.obj"

"..\bin\Release\host.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "host - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug
# Begin Custom Macros
OutDir=.\Debug
# End Custom Macros

ALL : "..\bin\Debug\host.exe" "$(OUTDIR)\host.bsc"


CLEAN :
	-@erase "$(INTDIR)\host.obj"
	-@erase "$(INTDIR)\host.sbr"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\host.bsc"
	-@erase "..\bin\Debug\host.exe"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\include" /I "..\..\..\include" /D "_DEBUG" /D "i386" /D "WIN32" /D "_CONSOLE" /D "_MBCS" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\host.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

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

RSC=rc.exe
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\host.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\host.sbr"

"$(OUTDIR)\host.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
LINK32_FLAGS=../libbind/Debug/libbind.lib /nologo /subsystem:console /pdb:none /debug /machine:I386 /out:"..\bin\Debug\host.exe" 
LINK32_OBJS= \
	"$(INTDIR)\host.obj"

"..\bin\Debug\host.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 


!IF "$(NO_EXTERNAL_DEPS)" != "1"
!IF EXISTS("host.dep")
!INCLUDE "host.dep"
!ELSE 
!MESSAGE Warning: cannot find "host.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "host - Win32 Release" || "$(CFG)" == "host - Win32 Debug"
SOURCE=..\..\..\bin\host\host.c

!IF  "$(CFG)" == "host - Win32 Release"


"$(INTDIR)\host.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "host - Win32 Debug"


"$(INTDIR)\host.obj"	"$(INTDIR)\host.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 


!ENDIF 

