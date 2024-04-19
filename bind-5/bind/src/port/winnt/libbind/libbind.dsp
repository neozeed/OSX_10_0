# Microsoft Developer Studio Project File - Name="libbind" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=libbind - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "libbind.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "libbind.mak" CFG="libbind - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "libbind - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "libbind - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "libbind - Win32 Release"

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
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "..\include" /I "..\..\..\include" /I "..\..\..\lib\cylink" /I "..\..\..\lib\dnssafe" /D "_DEBUG" /D "HAVE_CONFIG_H" /D "WIN32" /D "_WINDOWS" /D "LIB" /D "CYLINK_DSS" /D "HMAC_MD5" /D "USE_MD5" /D "DNSSAFE" /D "i386" /FR /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 user32.lib advapi32.lib netapi32.lib /nologo /subsystem:windows /dll /pdb:none /map /machine:I386 /out:"..\bin\Release\libbind.dll"

!ELSEIF  "$(CFG)" == "libbind - Win32 Debug"

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
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /I "..\include" /I "..\..\..\include" /I "..\..\..\lib\cylink" /I "..\..\..\lib\dnssafe" /D "_DEBUG" /D "HAVE_CONFIG_H" /D "WIN32" /D "_WINDOWS" /D "LIB" /D "CYLINK_DSS" /D "HMAC_MD5" /D "USE_MD5" /D "DNSSAFE" /D "i386" /FR /FD /c
# SUBTRACT CPP /FA<none> /YX
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 user32.lib advapi32.lib netapi32.lib /nologo /subsystem:windows /dll /pdb:none /debug /machine:I386 /out:"..\bin\Debug\libbind.dll"

!ENDIF 

# Begin Target

# Name "libbind - Win32 Release"
# Name "libbind - Win32 Debug"
# Begin Group "bsd"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\lib\bsd\gettimeofday.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\bsd\putenv.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\bsd\readv.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\bsd\setenv.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\bsd\setitimer.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\bsd\strcasecmp.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\bsd\strdup.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\bsd\strpbrk.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\bsd\strsep.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\bsd\strtoul.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\bsd\utimes.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\bsd\writev.c
# End Source File
# End Group
# Begin Group "isc"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\lib\isc\assertions.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\isc\base64.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\isc\bitncmp.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\isc\ctl_clnt.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\isc\ctl_p.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\isc\ctl_srvr.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\isc\ev_connects.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\isc\ev_files.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\isc\ev_streams.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\isc\ev_timers.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\isc\ev_waits.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\isc\eventlib.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\isc\heap.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\isc\logging.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\isc\memcluster.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\isc\tree.c
# End Source File
# End Group
# Begin Group "irs"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\lib\irs\dns.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\irs\dns_gr.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\irs\dns_ho.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\irs\dns_nw.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\irs\dns_pr.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\irs\dns_pw.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\irs\dns_sv.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\irs\gai_strerror.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\irs\gen.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\irs\gen_gr.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\irs\gen_ho.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\irs\gen_ng.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\irs\gen_nw.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\irs\gen_pr.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\irs\gen_pw.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\irs\gen_sv.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\irs\getaddrinfo.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\irs\getgrent.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\irs\getgrent_r.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\irs\gethostent.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\irs\gethostent_r.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\irs\getnameinfo.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\irs\getnetent.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\irs\getnetent_r.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\irs\getnetgrent.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\irs\getnetgrent_r.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\irs\getprotoent.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\irs\getprotoent_r.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\irs\getpwent.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\irs\getpwent_r.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\irs\getservent.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\irs\getservent_r.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\irs\hesiod.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\irs\irp.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\irs\irp_gr.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\irs\irp_ho.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\irs\irp_ng.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\irs\irp_nw.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\irs\irp_pr.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\irs\irp_pw.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\irs\irp_sv.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\irs\irpmarshall.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\irs\irs_data.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\irs\lcl.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\irs\lcl_gr.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\irs\lcl_ho.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\irs\lcl_ng.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\irs\lcl_nw.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\irs\lcl_pr.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\irs\lcl_pw.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\irs\lcl_sv.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\irs\nis.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\irs\nis_gr.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\irs\nis_ho.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\irs\nis_ng.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\irs\nis_nw.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\irs\nis_pr.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\irs\nis_pw.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\irs\nis_sv.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\irs\nul_ng.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\irs\util.c
# End Source File
# End Group
# Begin Group "resolv"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\lib\resolv\herror.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\resolv\res_comp.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\resolv\res_data.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\resolv\res_debug.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\resolv\res_findzonecut.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\resolv\res_init.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\resolv\res_mkquery.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\resolv\res_mkupdate.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\resolv\res_query.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\resolv\res_send.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\resolv\res_sendsigned.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\resolv\res_update.c
# End Source File
# End Group
# Begin Group "nameser"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\lib\nameser\ns_date.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\nameser\ns_name.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\nameser\ns_netint.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\nameser\ns_parse.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\nameser\ns_print.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\nameser\ns_samedomain.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\nameser\ns_sign.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\nameser\ns_ttl.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\nameser\ns_verify.c
# End Source File
# End Group
# Begin Group "inet"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\lib\inet\inet_addr.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\inet\inet_cidr_ntop.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\inet\inet_cidr_pton.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\inet\inet_lnaof.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\inet\inet_makeaddr.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\inet\inet_net_ntop.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\inet\inet_net_pton.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\inet\inet_neta.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\inet\inet_netof.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\inet\inet_network.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\inet\inet_ntoa.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\inet\inet_ntop.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\inet\inet_pton.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\inet\nsap_addr.c
# End Source File
# End Group
# Begin Group "cylink"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\lib\cylink\bits.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\cylink\bn.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\cylink\bn00.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\cylink\ctk_prime.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\cylink\dss.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\cylink\lbn00.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\cylink\lbnmem.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\cylink\legal.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\cylink\math.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\cylink\rand.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\cylink\sha.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\cylink\swap.c
# End Source File
# End Group
# Begin Group "dst"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\lib\dst\bsafe_link.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\dst\cylink_link.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\dst\dst_api.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\dst\eay_dss_link.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\dst\hmac_link.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\dst\md5_dgst.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\dst\prandom.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\dst\rsaref_link.c
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\dst\support.c
# End Source File
# End Group
# Begin Group "port"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\dirent.c
# End Source File
# Begin Source File

SOURCE=.\DLLMain.c
# End Source File
# Begin Source File

SOURCE=.\files.c
# End Source File
# Begin Source File

SOURCE=.\getopt.c
# End Source File
# Begin Source File

SOURCE=.\interface.c
# End Source File
# Begin Source File

SOURCE=.\ioctl_if.c
# End Source File
# Begin Source File

SOURCE=.\movefile.c
# End Source File
# Begin Source File

SOURCE=.\nameserver.c
# End Source File
# Begin Source File

SOURCE=.\signal.c
# End Source File
# Begin Source File

SOURCE=.\socket.c
# End Source File
# Begin Source File

SOURCE=.\syslog.c
# End Source File
# Begin Source File

SOURCE=.\unistd.c
# End Source File
# End Group
# Begin Group "dnssafe"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\lib\dnssafe\ahcbcpad.c

!IF  "$(CFG)" == "libbind - Win32 Release"

!ELSEIF  "$(CFG)" == "libbind - Win32 Debug"

# SUBTRACT CPP /YX

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\lib\dnssafe\ahchdig.c

!IF  "$(CFG)" == "libbind - Win32 Release"

!ELSEIF  "$(CFG)" == "libbind - Win32 Debug"

# SUBTRACT CPP /YX

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\lib\dnssafe\ahchencr.c

!IF  "$(CFG)" == "libbind - Win32 Release"

!ELSEIF  "$(CFG)" == "libbind - Win32 Debug"

# SUBTRACT CPP /YX

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\lib\dnssafe\ahchgen.c

!IF  "$(CFG)" == "libbind - Win32 Release"

!ELSEIF  "$(CFG)" == "libbind - Win32 Debug"

# SUBTRACT CPP /YX

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\lib\dnssafe\ahchrand.c

!IF  "$(CFG)" == "libbind - Win32 Release"

!ELSEIF  "$(CFG)" == "libbind - Win32 Debug"

# SUBTRACT CPP /YX

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\lib\dnssafe\ahdigest.c

!IF  "$(CFG)" == "libbind - Win32 Release"

!ELSEIF  "$(CFG)" == "libbind - Win32 Debug"

# SUBTRACT CPP /YX

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\lib\dnssafe\ahencryp.c

!IF  "$(CFG)" == "libbind - Win32 Release"

!ELSEIF  "$(CFG)" == "libbind - Win32 Debug"

# SUBTRACT CPP /YX

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\lib\dnssafe\ahgen.c

!IF  "$(CFG)" == "libbind - Win32 Release"

!ELSEIF  "$(CFG)" == "libbind - Win32 Debug"

# SUBTRACT CPP /YX

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\lib\dnssafe\ahrandom.c

!IF  "$(CFG)" == "libbind - Win32 Release"

!ELSEIF  "$(CFG)" == "libbind - Win32 Debug"

# SUBTRACT CPP /YX

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\lib\dnssafe\ahrsaenc.c

!IF  "$(CFG)" == "libbind - Win32 Release"

!ELSEIF  "$(CFG)" == "libbind - Win32 Debug"

# SUBTRACT CPP /YX

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\lib\dnssafe\ahrsaepr.c

!IF  "$(CFG)" == "libbind - Win32 Release"

!ELSEIF  "$(CFG)" == "libbind - Win32 Debug"

# SUBTRACT CPP /YX

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\lib\dnssafe\ahrsaepu.c

!IF  "$(CFG)" == "libbind - Win32 Release"

!ELSEIF  "$(CFG)" == "libbind - Win32 Debug"

# SUBTRACT CPP /YX

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\lib\dnssafe\aichdig.c

!IF  "$(CFG)" == "libbind - Win32 Release"

!ELSEIF  "$(CFG)" == "libbind - Win32 Debug"

# SUBTRACT CPP /YX

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\lib\dnssafe\aichenc8.c

!IF  "$(CFG)" == "libbind - Win32 Release"

!ELSEIF  "$(CFG)" == "libbind - Win32 Debug"

# SUBTRACT CPP /YX

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\lib\dnssafe\aichencn.c

!IF  "$(CFG)" == "libbind - Win32 Release"

!ELSEIF  "$(CFG)" == "libbind - Win32 Debug"

# SUBTRACT CPP /YX

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\lib\dnssafe\aichencr.c

!IF  "$(CFG)" == "libbind - Win32 Release"

!ELSEIF  "$(CFG)" == "libbind - Win32 Debug"

# SUBTRACT CPP /YX

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\lib\dnssafe\aichgen.c

!IF  "$(CFG)" == "libbind - Win32 Release"

!ELSEIF  "$(CFG)" == "libbind - Win32 Debug"

# SUBTRACT CPP /YX

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\lib\dnssafe\aichrand.c

!IF  "$(CFG)" == "libbind - Win32 Release"

!ELSEIF  "$(CFG)" == "libbind - Win32 Debug"

# SUBTRACT CPP /YX

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\lib\dnssafe\aimd5.c

!IF  "$(CFG)" == "libbind - Win32 Release"

!ELSEIF  "$(CFG)" == "libbind - Win32 Debug"

# SUBTRACT CPP /YX

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\lib\dnssafe\aimd5ran.c

!IF  "$(CFG)" == "libbind - Win32 Release"

!ELSEIF  "$(CFG)" == "libbind - Win32 Debug"

# SUBTRACT CPP /YX

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\lib\dnssafe\ainfotyp.c

!IF  "$(CFG)" == "libbind - Win32 Release"

!ELSEIF  "$(CFG)" == "libbind - Win32 Debug"

# SUBTRACT CPP /YX

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\lib\dnssafe\ainull.c

!IF  "$(CFG)" == "libbind - Win32 Release"

!ELSEIF  "$(CFG)" == "libbind - Win32 Debug"

# SUBTRACT CPP /YX

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\lib\dnssafe\airsaepr.c

!IF  "$(CFG)" == "libbind - Win32 Release"

!ELSEIF  "$(CFG)" == "libbind - Win32 Debug"

# SUBTRACT CPP /YX

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\lib\dnssafe\airsaepu.c

!IF  "$(CFG)" == "libbind - Win32 Release"

!ELSEIF  "$(CFG)" == "libbind - Win32 Debug"

# SUBTRACT CPP /YX

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\lib\dnssafe\airsakgn.c

!IF  "$(CFG)" == "libbind - Win32 Release"

!ELSEIF  "$(CFG)" == "libbind - Win32 Debug"

# SUBTRACT CPP /YX

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\lib\dnssafe\airsaprv.c

!IF  "$(CFG)" == "libbind - Win32 Release"

!ELSEIF  "$(CFG)" == "libbind - Win32 Debug"

# SUBTRACT CPP /YX

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\lib\dnssafe\airsapub.c

!IF  "$(CFG)" == "libbind - Win32 Release"

!ELSEIF  "$(CFG)" == "libbind - Win32 Debug"

# SUBTRACT CPP /YX

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\lib\dnssafe\algchoic.c

!IF  "$(CFG)" == "libbind - Win32 Release"

!ELSEIF  "$(CFG)" == "libbind - Win32 Debug"

# SUBTRACT CPP /YX

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\lib\dnssafe\algobj.c

!IF  "$(CFG)" == "libbind - Win32 Release"

!ELSEIF  "$(CFG)" == "libbind - Win32 Debug"

# SUBTRACT CPP /YX

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\lib\dnssafe\amcrte.c

!IF  "$(CFG)" == "libbind - Win32 Release"

!ELSEIF  "$(CFG)" == "libbind - Win32 Debug"

# SUBTRACT CPP /YX

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\lib\dnssafe\ammd5.c

!IF  "$(CFG)" == "libbind - Win32 Release"

!ELSEIF  "$(CFG)" == "libbind - Win32 Debug"

# SUBTRACT CPP /YX

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\lib\dnssafe\ammd5r.c

!IF  "$(CFG)" == "libbind - Win32 Release"

!ELSEIF  "$(CFG)" == "libbind - Win32 Debug"

# SUBTRACT CPP /YX

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\lib\dnssafe\amrkg.c

!IF  "$(CFG)" == "libbind - Win32 Release"

!ELSEIF  "$(CFG)" == "libbind - Win32 Debug"

# SUBTRACT CPP /YX

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\lib\dnssafe\amrsae.c

!IF  "$(CFG)" == "libbind - Win32 Release"

!ELSEIF  "$(CFG)" == "libbind - Win32 Debug"

# SUBTRACT CPP /YX

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\lib\dnssafe\balg.c

!IF  "$(CFG)" == "libbind - Win32 Release"

!ELSEIF  "$(CFG)" == "libbind - Win32 Debug"

# SUBTRACT CPP /YX

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\lib\dnssafe\bgclrbit.c

!IF  "$(CFG)" == "libbind - Win32 Release"

!ELSEIF  "$(CFG)" == "libbind - Win32 Debug"

# SUBTRACT CPP /YX

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\lib\dnssafe\bgmdmpyx.c

!IF  "$(CFG)" == "libbind - Win32 Release"

!ELSEIF  "$(CFG)" == "libbind - Win32 Debug"

# SUBTRACT CPP /YX

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\lib\dnssafe\bgmdsqx.c

!IF  "$(CFG)" == "libbind - Win32 Release"

!ELSEIF  "$(CFG)" == "libbind - Win32 Debug"

# SUBTRACT CPP /YX

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\lib\dnssafe\bgmodexp.c

!IF  "$(CFG)" == "libbind - Win32 Release"

!ELSEIF  "$(CFG)" == "libbind - Win32 Debug"

# SUBTRACT CPP /YX

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\lib\dnssafe\bgpegcd.c

!IF  "$(CFG)" == "libbind - Win32 Release"

!ELSEIF  "$(CFG)" == "libbind - Win32 Debug"

# SUBTRACT CPP /YX

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\lib\dnssafe\big2exp.c

!IF  "$(CFG)" == "libbind - Win32 Release"

!ELSEIF  "$(CFG)" == "libbind - Win32 Debug"

# SUBTRACT CPP /YX

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\lib\dnssafe\bigabs.c

!IF  "$(CFG)" == "libbind - Win32 Release"

!ELSEIF  "$(CFG)" == "libbind - Win32 Debug"

# SUBTRACT CPP /YX

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\lib\dnssafe\bigacc.c

!IF  "$(CFG)" == "libbind - Win32 Release"

!ELSEIF  "$(CFG)" == "libbind - Win32 Debug"

# SUBTRACT CPP /YX

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\lib\dnssafe\bigarith.c

!IF  "$(CFG)" == "libbind - Win32 Release"

!ELSEIF  "$(CFG)" == "libbind - Win32 Debug"

# SUBTRACT CPP /YX

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\lib\dnssafe\bigcmp.c

!IF  "$(CFG)" == "libbind - Win32 Release"

!ELSEIF  "$(CFG)" == "libbind - Win32 Debug"

# SUBTRACT CPP /YX

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\lib\dnssafe\bigconst.c

!IF  "$(CFG)" == "libbind - Win32 Release"

!ELSEIF  "$(CFG)" == "libbind - Win32 Debug"

# SUBTRACT CPP /YX

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\lib\dnssafe\biginv.c

!IF  "$(CFG)" == "libbind - Win32 Release"

!ELSEIF  "$(CFG)" == "libbind - Win32 Debug"

# SUBTRACT CPP /YX

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\lib\dnssafe\biglen.c

!IF  "$(CFG)" == "libbind - Win32 Release"

!ELSEIF  "$(CFG)" == "libbind - Win32 Debug"

# SUBTRACT CPP /YX

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\lib\dnssafe\bigmodx.c

!IF  "$(CFG)" == "libbind - Win32 Release"

!ELSEIF  "$(CFG)" == "libbind - Win32 Debug"

# SUBTRACT CPP /YX

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\lib\dnssafe\bigmpy.c

!IF  "$(CFG)" == "libbind - Win32 Release"

!ELSEIF  "$(CFG)" == "libbind - Win32 Debug"

# SUBTRACT CPP /YX

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\lib\dnssafe\bigpdiv.c

!IF  "$(CFG)" == "libbind - Win32 Release"

!ELSEIF  "$(CFG)" == "libbind - Win32 Debug"

# SUBTRACT CPP /YX

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\lib\dnssafe\bigpmpy.c

!IF  "$(CFG)" == "libbind - Win32 Release"

!ELSEIF  "$(CFG)" == "libbind - Win32 Debug"

# SUBTRACT CPP /YX

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\lib\dnssafe\bigpmpyh.c

!IF  "$(CFG)" == "libbind - Win32 Release"

!ELSEIF  "$(CFG)" == "libbind - Win32 Debug"

# SUBTRACT CPP /YX

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\lib\dnssafe\bigpmpyl.c

!IF  "$(CFG)" == "libbind - Win32 Release"

!ELSEIF  "$(CFG)" == "libbind - Win32 Debug"

# SUBTRACT CPP /YX

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\lib\dnssafe\bigpsq.c

!IF  "$(CFG)" == "libbind - Win32 Release"

!ELSEIF  "$(CFG)" == "libbind - Win32 Debug"

# SUBTRACT CPP /YX

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\lib\dnssafe\bigqrx.c

!IF  "$(CFG)" == "libbind - Win32 Release"

!ELSEIF  "$(CFG)" == "libbind - Win32 Debug"

# SUBTRACT CPP /YX

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\lib\dnssafe\bigsmod.c

!IF  "$(CFG)" == "libbind - Win32 Release"

!ELSEIF  "$(CFG)" == "libbind - Win32 Debug"

# SUBTRACT CPP /YX

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\lib\dnssafe\bigtocan.c

!IF  "$(CFG)" == "libbind - Win32 Release"

!ELSEIF  "$(CFG)" == "libbind - Win32 Debug"

# SUBTRACT CPP /YX

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\lib\dnssafe\bigu.c

!IF  "$(CFG)" == "libbind - Win32 Release"

!ELSEIF  "$(CFG)" == "libbind - Win32 Debug"

# SUBTRACT CPP /YX

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\lib\dnssafe\bigunexp.c

!IF  "$(CFG)" == "libbind - Win32 Release"

!ELSEIF  "$(CFG)" == "libbind - Win32 Debug"

# SUBTRACT CPP /YX

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\lib\dnssafe\binfocsh.c

!IF  "$(CFG)" == "libbind - Win32 Release"

!ELSEIF  "$(CFG)" == "libbind - Win32 Debug"

# SUBTRACT CPP /YX

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\lib\dnssafe\bkey.c

!IF  "$(CFG)" == "libbind - Win32 Release"

!ELSEIF  "$(CFG)" == "libbind - Win32 Debug"

# SUBTRACT CPP /YX

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\lib\dnssafe\bmempool.c

!IF  "$(CFG)" == "libbind - Win32 Release"

!ELSEIF  "$(CFG)" == "libbind - Win32 Debug"

# SUBTRACT CPP /YX

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\lib\dnssafe\cantobig.c

!IF  "$(CFG)" == "libbind - Win32 Release"

!ELSEIF  "$(CFG)" == "libbind - Win32 Debug"

# SUBTRACT CPP /YX

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\lib\dnssafe\crt2.c

!IF  "$(CFG)" == "libbind - Win32 Release"

!ELSEIF  "$(CFG)" == "libbind - Win32 Debug"

# SUBTRACT CPP /YX

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\lib\dnssafe\digest.c

!IF  "$(CFG)" == "libbind - Win32 Release"

!ELSEIF  "$(CFG)" == "libbind - Win32 Debug"

# SUBTRACT CPP /YX

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\lib\dnssafe\digrand.c

!IF  "$(CFG)" == "libbind - Win32 Release"

!ELSEIF  "$(CFG)" == "libbind - Win32 Debug"

# SUBTRACT CPP /YX

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\lib\dnssafe\encrypt.c

!IF  "$(CFG)" == "libbind - Win32 Release"

!ELSEIF  "$(CFG)" == "libbind - Win32 Debug"

# SUBTRACT CPP /YX

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\lib\dnssafe\generate.c

!IF  "$(CFG)" == "libbind - Win32 Release"

!ELSEIF  "$(CFG)" == "libbind - Win32 Debug"

# SUBTRACT CPP /YX

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\lib\dnssafe\intbits.c

!IF  "$(CFG)" == "libbind - Win32 Release"

!ELSEIF  "$(CFG)" == "libbind - Win32 Debug"

# SUBTRACT CPP /YX

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\lib\dnssafe\intitem.c

!IF  "$(CFG)" == "libbind - Win32 Release"

!ELSEIF  "$(CFG)" == "libbind - Win32 Debug"

# SUBTRACT CPP /YX

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\lib\dnssafe\keyobj.c

!IF  "$(CFG)" == "libbind - Win32 Release"

!ELSEIF  "$(CFG)" == "libbind - Win32 Debug"

# SUBTRACT CPP /YX

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\lib\dnssafe\ki8byte.c

!IF  "$(CFG)" == "libbind - Win32 Release"

!ELSEIF  "$(CFG)" == "libbind - Win32 Debug"

# SUBTRACT CPP /YX

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\lib\dnssafe\kifulprv.c

!IF  "$(CFG)" == "libbind - Win32 Release"

!ELSEIF  "$(CFG)" == "libbind - Win32 Debug"

# SUBTRACT CPP /YX

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\lib\dnssafe\kiitem.c

!IF  "$(CFG)" == "libbind - Win32 Release"

!ELSEIF  "$(CFG)" == "libbind - Win32 Debug"

# SUBTRACT CPP /YX

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\lib\dnssafe\kinfotyp.c

!IF  "$(CFG)" == "libbind - Win32 Release"

!ELSEIF  "$(CFG)" == "libbind - Win32 Debug"

# SUBTRACT CPP /YX

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\lib\dnssafe\kipkcrpr.c

!IF  "$(CFG)" == "libbind - Win32 Release"

!ELSEIF  "$(CFG)" == "libbind - Win32 Debug"

# SUBTRACT CPP /YX

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\lib\dnssafe\kirsacrt.c

!IF  "$(CFG)" == "libbind - Win32 Release"

!ELSEIF  "$(CFG)" == "libbind - Win32 Debug"

# SUBTRACT CPP /YX

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\lib\dnssafe\kirsapub.c

!IF  "$(CFG)" == "libbind - Win32 Release"

!ELSEIF  "$(CFG)" == "libbind - Win32 Debug"

# SUBTRACT CPP /YX

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\lib\dnssafe\md5.c

!IF  "$(CFG)" == "libbind - Win32 Release"

!ELSEIF  "$(CFG)" == "libbind - Win32 Debug"

# SUBTRACT CPP /YX

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\lib\dnssafe\md5rand.c

!IF  "$(CFG)" == "libbind - Win32 Release"

!ELSEIF  "$(CFG)" == "libbind - Win32 Debug"

# SUBTRACT CPP /YX

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\lib\dnssafe\prime.c

!IF  "$(CFG)" == "libbind - Win32 Release"

!ELSEIF  "$(CFG)" == "libbind - Win32 Debug"

# SUBTRACT CPP /YX

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\lib\dnssafe\random.c

!IF  "$(CFG)" == "libbind - Win32 Release"

!ELSEIF  "$(CFG)" == "libbind - Win32 Debug"

# SUBTRACT CPP /YX

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\lib\dnssafe\rsa.c

!IF  "$(CFG)" == "libbind - Win32 Release"

!ELSEIF  "$(CFG)" == "libbind - Win32 Debug"

# SUBTRACT CPP /YX

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\lib\dnssafe\rsakeygn.c

!IF  "$(CFG)" == "libbind - Win32 Release"

!ELSEIF  "$(CFG)" == "libbind - Win32 Debug"

# SUBTRACT CPP /YX

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\lib\dnssafe\seccbcd.c

!IF  "$(CFG)" == "libbind - Win32 Release"

!ELSEIF  "$(CFG)" == "libbind - Win32 Debug"

# SUBTRACT CPP /YX

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\lib\dnssafe\seccbce.c

!IF  "$(CFG)" == "libbind - Win32 Release"

!ELSEIF  "$(CFG)" == "libbind - Win32 Debug"

# SUBTRACT CPP /YX

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\..\lib\dnssafe\surrendr.c

!IF  "$(CFG)" == "libbind - Win32 Release"

!ELSEIF  "$(CFG)" == "libbind - Win32 Debug"

# SUBTRACT CPP /YX

!ENDIF 

# End Source File
# End Group
# Begin Source File

SOURCE=.\libbind.def
# End Source File
# End Target
# End Project
