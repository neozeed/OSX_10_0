# Microsoft Developer Studio Generated NMAKE File, Based on dig.dsp
!IF "$(CFG)" == ""
CFG=dig - Win32 Debug
!MESSAGE No configuration specified. Defaulting to dig - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "dig - Win32 Release" && "$(CFG)" != "dig - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "dig.mak" CFG="dig - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "dig - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "dig - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

!IF  "$(CFG)" == "dig - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release

ALL : "..\bin\Release\dig.exe"


CLEAN :
	-@erase "$(INTDIR)\debug.obj"
	-@erase "$(INTDIR)\dig.obj"
	-@erase "$(INTDIR)\list.obj"
	-@erase "$(INTDIR)\send.obj"
	-@erase "$(INTDIR)\subr.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "..\bin\Release\dig.exe"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MD /W3 /GX /O2 /I "..\include" /I "..\..\..\include" /D "NDEBUG" /D "WIN32" /D "_CONSOLE" /D "_MBCS" /D "i386" /Fp"$(INTDIR)\dig.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

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
BSC32_FLAGS=/nologo /o"$(OUTDIR)\dig.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=../libbind/Release/libbind.lib /nologo /subsystem:console /incremental:no /pdb:"$(OUTDIR)\dig.pdb" /machine:I386 /out:"..\bin\Release\dig.exe" 
LINK32_OBJS= \
	"$(INTDIR)\debug.obj" \
	"$(INTDIR)\dig.obj" \
	"$(INTDIR)\list.obj" \
	"$(INTDIR)\send.obj" \
	"$(INTDIR)\subr.obj"

"..\bin\Release\dig.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "dig - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug
# Begin Custom Macros
OutDir=.\Debug
# End Custom Macros

ALL : "..\bin\Debug\dig.exe" "$(OUTDIR)\dig.bsc"


CLEAN :
	-@erase "$(INTDIR)\debug.obj"
	-@erase "$(INTDIR)\debug.sbr"
	-@erase "$(INTDIR)\dig.obj"
	-@erase "$(INTDIR)\dig.sbr"
	-@erase "$(INTDIR)\list.obj"
	-@erase "$(INTDIR)\list.sbr"
	-@erase "$(INTDIR)\send.obj"
	-@erase "$(INTDIR)\send.sbr"
	-@erase "$(INTDIR)\subr.obj"
	-@erase "$(INTDIR)\subr.sbr"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\dig.bsc"
	-@erase "..\bin\Debug\dig.exe"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP=cl.exe
CPP_PROJ=/nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\include" /I "..\..\..\include" /D "_DEBUG" /D "i386" /D "WIN32" /D "_CONSOLE" /D "_MBCS" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\dig.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

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
BSC32_FLAGS=/nologo /o"$(OUTDIR)\dig.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\debug.sbr" \
	"$(INTDIR)\dig.sbr" \
	"$(INTDIR)\list.sbr" \
	"$(INTDIR)\send.sbr" \
	"$(INTDIR)\subr.sbr"

"$(OUTDIR)\dig.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
LINK32_FLAGS=../libbind/Debug/libbind.lib /nologo /subsystem:console /pdb:none /debug /machine:I386 /out:"..\bin\Debug\dig.exe" 
LINK32_OBJS= \
	"$(INTDIR)\debug.obj" \
	"$(INTDIR)\dig.obj" \
	"$(INTDIR)\list.obj" \
	"$(INTDIR)\send.obj" \
	"$(INTDIR)\subr.obj"

"..\bin\Debug\dig.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 


!IF "$(NO_EXTERNAL_DEPS)" != "1"
!IF EXISTS("dig.dep")
!INCLUDE "dig.dep"
!ELSE 
!MESSAGE Warning: cannot find "dig.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "dig - Win32 Release" || "$(CFG)" == "dig - Win32 Debug"
SOURCE=..\..\..\bin\nslookup\debug.c

!IF  "$(CFG)" == "dig - Win32 Release"


"$(INTDIR)\debug.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "dig - Win32 Debug"


"$(INTDIR)\debug.obj"	"$(INTDIR)\debug.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\..\bin\dig\dig.c

!IF  "$(CFG)" == "dig - Win32 Release"


"$(INTDIR)\dig.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "dig - Win32 Debug"


"$(INTDIR)\dig.obj"	"$(INTDIR)\dig.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\..\bin\nslookup\list.c

!IF  "$(CFG)" == "dig - Win32 Release"


"$(INTDIR)\list.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "dig - Win32 Debug"


"$(INTDIR)\list.obj"	"$(INTDIR)\list.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\..\bin\nslookup\send.c

!IF  "$(CFG)" == "dig - Win32 Release"


"$(INTDIR)\send.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "dig - Win32 Debug"


"$(INTDIR)\send.obj"	"$(INTDIR)\send.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\..\bin\nslookup\subr.c

!IF  "$(CFG)" == "dig - Win32 Release"


"$(INTDIR)\subr.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "dig - Win32 Debug"


"$(INTDIR)\subr.obj"	"$(INTDIR)\subr.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 


!ENDIF 

