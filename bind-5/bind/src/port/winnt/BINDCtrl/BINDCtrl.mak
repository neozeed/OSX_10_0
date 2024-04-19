# Microsoft Developer Studio Generated NMAKE File, Based on BINDCtrl.dsp
!IF "$(CFG)" == ""
CFG=BINDCtrl - Win32 Debug
!MESSAGE No configuration specified. Defaulting to BINDCtrl - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "BINDCtrl - Win32 Release" && "$(CFG)" != "BINDCtrl - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "BINDCtrl.mak" CFG="BINDCtrl - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "BINDCtrl - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "BINDCtrl - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "BINDCtrl - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release
# Begin Custom Macros
OutDir=.\Release
# End Custom Macros

ALL : "..\bin\Release\BINDCtrl.exe" "$(OUTDIR)\BINDCtrl.bsc"


CLEAN :
	-@erase "$(INTDIR)\BINDCtrl.obj"
	-@erase "$(INTDIR)\BINDCtrl.pch"
	-@erase "$(INTDIR)\BINDCtrl.res"
	-@erase "$(INTDIR)\BINDCtrl.sbr"
	-@erase "$(INTDIR)\BINDCtrlDlg.obj"
	-@erase "$(INTDIR)\BINDCtrlDlg.sbr"
	-@erase "$(INTDIR)\StdAfx.obj"
	-@erase "$(INTDIR)\StdAfx.sbr"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\BINDCtrl.bsc"
	-@erase "..\bin\Release\BINDCtrl.exe"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MD /W3 /GX /O2 /I "..\include" /I "..\..\..\include" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_AFXDLL" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\BINDCtrl.pch" /Yu"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\BINDCtrl.res" /d "NDEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\BINDCtrl.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\BINDCtrl.sbr" \
	"$(INTDIR)\BINDCtrlDlg.sbr" \
	"$(INTDIR)\StdAfx.sbr"

"$(OUTDIR)\BINDCtrl.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
LINK32_FLAGS=/nologo /subsystem:windows /pdb:none /machine:I386 /out:"..\bin\Release\BINDCtrl.exe" 
LINK32_OBJS= \
	"$(INTDIR)\BINDCtrl.obj" \
	"$(INTDIR)\BINDCtrlDlg.obj" \
	"$(INTDIR)\StdAfx.obj" \
	"$(INTDIR)\BINDCtrl.res"

"..\bin\Release\BINDCtrl.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "BINDCtrl - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug
# Begin Custom Macros
OutDir=.\Debug
# End Custom Macros

ALL : "..\bin\Debug\BINDCtrl.exe" "$(OUTDIR)\BINDCtrl.bsc"


CLEAN :
	-@erase "$(INTDIR)\BINDCtrl.obj"
	-@erase "$(INTDIR)\BINDCtrl.pch"
	-@erase "$(INTDIR)\BINDCtrl.res"
	-@erase "$(INTDIR)\BINDCtrl.sbr"
	-@erase "$(INTDIR)\BINDCtrlDlg.obj"
	-@erase "$(INTDIR)\BINDCtrlDlg.sbr"
	-@erase "$(INTDIR)\StdAfx.obj"
	-@erase "$(INTDIR)\StdAfx.sbr"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\BINDCtrl.bsc"
	-@erase "..\bin\Debug\BINDCtrl.exe"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MDd /W3 /Gm /GX /Zi /Od /I "..\include" /I "..\..\..\include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_AFXDLL" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\BINDCtrl.pch" /Yu"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\BINDCtrl.res" /d "_DEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\BINDCtrl.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\BINDCtrl.sbr" \
	"$(INTDIR)\BINDCtrlDlg.sbr" \
	"$(INTDIR)\StdAfx.sbr"

"$(OUTDIR)\BINDCtrl.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
LINK32_FLAGS=/nologo /subsystem:windows /pdb:none /debug /machine:I386 /out:"..\bin\Debug\BINDCtrl.exe" 
LINK32_OBJS= \
	"$(INTDIR)\BINDCtrl.obj" \
	"$(INTDIR)\BINDCtrlDlg.obj" \
	"$(INTDIR)\StdAfx.obj" \
	"$(INTDIR)\BINDCtrl.res"

"..\bin\Debug\BINDCtrl.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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
!IF EXISTS("BINDCtrl.dep")
!INCLUDE "BINDCtrl.dep"
!ELSE 
!MESSAGE Warning: cannot find "BINDCtrl.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "BINDCtrl - Win32 Release" || "$(CFG)" == "BINDCtrl - Win32 Debug"
SOURCE=.\BINDCtrl.cpp

"$(INTDIR)\BINDCtrl.obj"	"$(INTDIR)\BINDCtrl.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\BINDCtrl.pch"


SOURCE=.\BINDCtrl.rc

"$(INTDIR)\BINDCtrl.res" : $(SOURCE) "$(INTDIR)"
	$(RSC) $(RSC_PROJ) $(SOURCE)


SOURCE=.\BINDCtrlDlg.cpp

"$(INTDIR)\BINDCtrlDlg.obj"	"$(INTDIR)\BINDCtrlDlg.sbr" : $(SOURCE) "$(INTDIR)" "$(INTDIR)\BINDCtrl.pch"


SOURCE=.\StdAfx.cpp

!IF  "$(CFG)" == "BINDCtrl - Win32 Release"

CPP_SWITCHES=/nologo /MD /W3 /GX /O2 /I "..\include" /I "..\..\..\include" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_AFXDLL" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\BINDCtrl.pch" /Yc"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\StdAfx.obj"	"$(INTDIR)\StdAfx.sbr"	"$(INTDIR)\BINDCtrl.pch" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "BINDCtrl - Win32 Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Gm /GX /Zi /Od /I "..\include" /I "..\..\..\include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_AFXDLL" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\BINDCtrl.pch" /Yc"stdafx.h" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 

"$(INTDIR)\StdAfx.obj"	"$(INTDIR)\StdAfx.sbr"	"$(INTDIR)\BINDCtrl.pch" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 


!ENDIF 

