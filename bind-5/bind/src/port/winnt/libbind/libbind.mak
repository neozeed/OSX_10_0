# Microsoft Developer Studio Generated NMAKE File, Based on libbind.dsp
!IF "$(CFG)" == ""
CFG=libbind - Win32 Debug
!MESSAGE No configuration specified. Defaulting to libbind - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "libbind - Win32 Release" && "$(CFG)" != "libbind - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
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

!IF  "$(CFG)" == "libbind - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release
# Begin Custom Macros
OutDir=.\Release
# End Custom Macros

ALL : "..\bin\Release\libbind.dll" "$(OUTDIR)\libbind.bsc"


CLEAN :
	-@erase "$(INTDIR)\ahcbcpad.obj"
	-@erase "$(INTDIR)\ahcbcpad.sbr"
	-@erase "$(INTDIR)\ahchdig.obj"
	-@erase "$(INTDIR)\ahchdig.sbr"
	-@erase "$(INTDIR)\ahchencr.obj"
	-@erase "$(INTDIR)\ahchencr.sbr"
	-@erase "$(INTDIR)\ahchgen.obj"
	-@erase "$(INTDIR)\ahchgen.sbr"
	-@erase "$(INTDIR)\ahchrand.obj"
	-@erase "$(INTDIR)\ahchrand.sbr"
	-@erase "$(INTDIR)\ahdigest.obj"
	-@erase "$(INTDIR)\ahdigest.sbr"
	-@erase "$(INTDIR)\ahencryp.obj"
	-@erase "$(INTDIR)\ahencryp.sbr"
	-@erase "$(INTDIR)\ahgen.obj"
	-@erase "$(INTDIR)\ahgen.sbr"
	-@erase "$(INTDIR)\ahrandom.obj"
	-@erase "$(INTDIR)\ahrandom.sbr"
	-@erase "$(INTDIR)\ahrsaenc.obj"
	-@erase "$(INTDIR)\ahrsaenc.sbr"
	-@erase "$(INTDIR)\ahrsaepr.obj"
	-@erase "$(INTDIR)\ahrsaepr.sbr"
	-@erase "$(INTDIR)\ahrsaepu.obj"
	-@erase "$(INTDIR)\ahrsaepu.sbr"
	-@erase "$(INTDIR)\aichdig.obj"
	-@erase "$(INTDIR)\aichdig.sbr"
	-@erase "$(INTDIR)\aichenc8.obj"
	-@erase "$(INTDIR)\aichenc8.sbr"
	-@erase "$(INTDIR)\aichencn.obj"
	-@erase "$(INTDIR)\aichencn.sbr"
	-@erase "$(INTDIR)\aichencr.obj"
	-@erase "$(INTDIR)\aichencr.sbr"
	-@erase "$(INTDIR)\aichgen.obj"
	-@erase "$(INTDIR)\aichgen.sbr"
	-@erase "$(INTDIR)\aichrand.obj"
	-@erase "$(INTDIR)\aichrand.sbr"
	-@erase "$(INTDIR)\aimd5.obj"
	-@erase "$(INTDIR)\aimd5.sbr"
	-@erase "$(INTDIR)\aimd5ran.obj"
	-@erase "$(INTDIR)\aimd5ran.sbr"
	-@erase "$(INTDIR)\ainfotyp.obj"
	-@erase "$(INTDIR)\ainfotyp.sbr"
	-@erase "$(INTDIR)\ainull.obj"
	-@erase "$(INTDIR)\ainull.sbr"
	-@erase "$(INTDIR)\airsaepr.obj"
	-@erase "$(INTDIR)\airsaepr.sbr"
	-@erase "$(INTDIR)\airsaepu.obj"
	-@erase "$(INTDIR)\airsaepu.sbr"
	-@erase "$(INTDIR)\airsakgn.obj"
	-@erase "$(INTDIR)\airsakgn.sbr"
	-@erase "$(INTDIR)\airsaprv.obj"
	-@erase "$(INTDIR)\airsaprv.sbr"
	-@erase "$(INTDIR)\airsapub.obj"
	-@erase "$(INTDIR)\airsapub.sbr"
	-@erase "$(INTDIR)\algchoic.obj"
	-@erase "$(INTDIR)\algchoic.sbr"
	-@erase "$(INTDIR)\algobj.obj"
	-@erase "$(INTDIR)\algobj.sbr"
	-@erase "$(INTDIR)\amcrte.obj"
	-@erase "$(INTDIR)\amcrte.sbr"
	-@erase "$(INTDIR)\ammd5.obj"
	-@erase "$(INTDIR)\ammd5.sbr"
	-@erase "$(INTDIR)\ammd5r.obj"
	-@erase "$(INTDIR)\ammd5r.sbr"
	-@erase "$(INTDIR)\amrkg.obj"
	-@erase "$(INTDIR)\amrkg.sbr"
	-@erase "$(INTDIR)\amrsae.obj"
	-@erase "$(INTDIR)\amrsae.sbr"
	-@erase "$(INTDIR)\assertions.obj"
	-@erase "$(INTDIR)\assertions.sbr"
	-@erase "$(INTDIR)\balg.obj"
	-@erase "$(INTDIR)\balg.sbr"
	-@erase "$(INTDIR)\base64.obj"
	-@erase "$(INTDIR)\base64.sbr"
	-@erase "$(INTDIR)\bgclrbit.obj"
	-@erase "$(INTDIR)\bgclrbit.sbr"
	-@erase "$(INTDIR)\bgmdmpyx.obj"
	-@erase "$(INTDIR)\bgmdmpyx.sbr"
	-@erase "$(INTDIR)\bgmdsqx.obj"
	-@erase "$(INTDIR)\bgmdsqx.sbr"
	-@erase "$(INTDIR)\bgmodexp.obj"
	-@erase "$(INTDIR)\bgmodexp.sbr"
	-@erase "$(INTDIR)\bgpegcd.obj"
	-@erase "$(INTDIR)\bgpegcd.sbr"
	-@erase "$(INTDIR)\big2exp.obj"
	-@erase "$(INTDIR)\big2exp.sbr"
	-@erase "$(INTDIR)\bigabs.obj"
	-@erase "$(INTDIR)\bigabs.sbr"
	-@erase "$(INTDIR)\bigacc.obj"
	-@erase "$(INTDIR)\bigacc.sbr"
	-@erase "$(INTDIR)\bigarith.obj"
	-@erase "$(INTDIR)\bigarith.sbr"
	-@erase "$(INTDIR)\bigcmp.obj"
	-@erase "$(INTDIR)\bigcmp.sbr"
	-@erase "$(INTDIR)\bigconst.obj"
	-@erase "$(INTDIR)\bigconst.sbr"
	-@erase "$(INTDIR)\biginv.obj"
	-@erase "$(INTDIR)\biginv.sbr"
	-@erase "$(INTDIR)\biglen.obj"
	-@erase "$(INTDIR)\biglen.sbr"
	-@erase "$(INTDIR)\bigmodx.obj"
	-@erase "$(INTDIR)\bigmodx.sbr"
	-@erase "$(INTDIR)\bigmpy.obj"
	-@erase "$(INTDIR)\bigmpy.sbr"
	-@erase "$(INTDIR)\bigpdiv.obj"
	-@erase "$(INTDIR)\bigpdiv.sbr"
	-@erase "$(INTDIR)\bigpmpy.obj"
	-@erase "$(INTDIR)\bigpmpy.sbr"
	-@erase "$(INTDIR)\bigpmpyh.obj"
	-@erase "$(INTDIR)\bigpmpyh.sbr"
	-@erase "$(INTDIR)\bigpmpyl.obj"
	-@erase "$(INTDIR)\bigpmpyl.sbr"
	-@erase "$(INTDIR)\bigpsq.obj"
	-@erase "$(INTDIR)\bigpsq.sbr"
	-@erase "$(INTDIR)\bigqrx.obj"
	-@erase "$(INTDIR)\bigqrx.sbr"
	-@erase "$(INTDIR)\bigsmod.obj"
	-@erase "$(INTDIR)\bigsmod.sbr"
	-@erase "$(INTDIR)\bigtocan.obj"
	-@erase "$(INTDIR)\bigtocan.sbr"
	-@erase "$(INTDIR)\bigu.obj"
	-@erase "$(INTDIR)\bigu.sbr"
	-@erase "$(INTDIR)\bigunexp.obj"
	-@erase "$(INTDIR)\bigunexp.sbr"
	-@erase "$(INTDIR)\binfocsh.obj"
	-@erase "$(INTDIR)\binfocsh.sbr"
	-@erase "$(INTDIR)\bitncmp.obj"
	-@erase "$(INTDIR)\bitncmp.sbr"
	-@erase "$(INTDIR)\bits.obj"
	-@erase "$(INTDIR)\bits.sbr"
	-@erase "$(INTDIR)\bkey.obj"
	-@erase "$(INTDIR)\bkey.sbr"
	-@erase "$(INTDIR)\bmempool.obj"
	-@erase "$(INTDIR)\bmempool.sbr"
	-@erase "$(INTDIR)\bn.obj"
	-@erase "$(INTDIR)\bn.sbr"
	-@erase "$(INTDIR)\bn00.obj"
	-@erase "$(INTDIR)\bn00.sbr"
	-@erase "$(INTDIR)\bsafe_link.obj"
	-@erase "$(INTDIR)\bsafe_link.sbr"
	-@erase "$(INTDIR)\cantobig.obj"
	-@erase "$(INTDIR)\cantobig.sbr"
	-@erase "$(INTDIR)\crt2.obj"
	-@erase "$(INTDIR)\crt2.sbr"
	-@erase "$(INTDIR)\ctk_prime.obj"
	-@erase "$(INTDIR)\ctk_prime.sbr"
	-@erase "$(INTDIR)\ctl_clnt.obj"
	-@erase "$(INTDIR)\ctl_clnt.sbr"
	-@erase "$(INTDIR)\ctl_p.obj"
	-@erase "$(INTDIR)\ctl_p.sbr"
	-@erase "$(INTDIR)\ctl_srvr.obj"
	-@erase "$(INTDIR)\ctl_srvr.sbr"
	-@erase "$(INTDIR)\cylink_link.obj"
	-@erase "$(INTDIR)\cylink_link.sbr"
	-@erase "$(INTDIR)\digest.obj"
	-@erase "$(INTDIR)\digest.sbr"
	-@erase "$(INTDIR)\digrand.obj"
	-@erase "$(INTDIR)\digrand.sbr"
	-@erase "$(INTDIR)\dirent.obj"
	-@erase "$(INTDIR)\dirent.sbr"
	-@erase "$(INTDIR)\DLLMain.obj"
	-@erase "$(INTDIR)\DLLMain.sbr"
	-@erase "$(INTDIR)\dns.obj"
	-@erase "$(INTDIR)\dns.sbr"
	-@erase "$(INTDIR)\dns_gr.obj"
	-@erase "$(INTDIR)\dns_gr.sbr"
	-@erase "$(INTDIR)\dns_ho.obj"
	-@erase "$(INTDIR)\dns_ho.sbr"
	-@erase "$(INTDIR)\dns_nw.obj"
	-@erase "$(INTDIR)\dns_nw.sbr"
	-@erase "$(INTDIR)\dns_pr.obj"
	-@erase "$(INTDIR)\dns_pr.sbr"
	-@erase "$(INTDIR)\dns_pw.obj"
	-@erase "$(INTDIR)\dns_pw.sbr"
	-@erase "$(INTDIR)\dns_sv.obj"
	-@erase "$(INTDIR)\dns_sv.sbr"
	-@erase "$(INTDIR)\dss.obj"
	-@erase "$(INTDIR)\dss.sbr"
	-@erase "$(INTDIR)\dst_api.obj"
	-@erase "$(INTDIR)\dst_api.sbr"
	-@erase "$(INTDIR)\eay_dss_link.obj"
	-@erase "$(INTDIR)\eay_dss_link.sbr"
	-@erase "$(INTDIR)\encrypt.obj"
	-@erase "$(INTDIR)\encrypt.sbr"
	-@erase "$(INTDIR)\ev_connects.obj"
	-@erase "$(INTDIR)\ev_connects.sbr"
	-@erase "$(INTDIR)\ev_files.obj"
	-@erase "$(INTDIR)\ev_files.sbr"
	-@erase "$(INTDIR)\ev_streams.obj"
	-@erase "$(INTDIR)\ev_streams.sbr"
	-@erase "$(INTDIR)\ev_timers.obj"
	-@erase "$(INTDIR)\ev_timers.sbr"
	-@erase "$(INTDIR)\ev_waits.obj"
	-@erase "$(INTDIR)\ev_waits.sbr"
	-@erase "$(INTDIR)\eventlib.obj"
	-@erase "$(INTDIR)\eventlib.sbr"
	-@erase "$(INTDIR)\files.obj"
	-@erase "$(INTDIR)\files.sbr"
	-@erase "$(INTDIR)\gai_strerror.obj"
	-@erase "$(INTDIR)\gai_strerror.sbr"
	-@erase "$(INTDIR)\gen.obj"
	-@erase "$(INTDIR)\gen.sbr"
	-@erase "$(INTDIR)\gen_gr.obj"
	-@erase "$(INTDIR)\gen_gr.sbr"
	-@erase "$(INTDIR)\gen_ho.obj"
	-@erase "$(INTDIR)\gen_ho.sbr"
	-@erase "$(INTDIR)\gen_ng.obj"
	-@erase "$(INTDIR)\gen_ng.sbr"
	-@erase "$(INTDIR)\gen_nw.obj"
	-@erase "$(INTDIR)\gen_nw.sbr"
	-@erase "$(INTDIR)\gen_pr.obj"
	-@erase "$(INTDIR)\gen_pr.sbr"
	-@erase "$(INTDIR)\gen_pw.obj"
	-@erase "$(INTDIR)\gen_pw.sbr"
	-@erase "$(INTDIR)\gen_sv.obj"
	-@erase "$(INTDIR)\gen_sv.sbr"
	-@erase "$(INTDIR)\generate.obj"
	-@erase "$(INTDIR)\generate.sbr"
	-@erase "$(INTDIR)\getaddrinfo.obj"
	-@erase "$(INTDIR)\getaddrinfo.sbr"
	-@erase "$(INTDIR)\getgrent.obj"
	-@erase "$(INTDIR)\getgrent.sbr"
	-@erase "$(INTDIR)\getgrent_r.obj"
	-@erase "$(INTDIR)\getgrent_r.sbr"
	-@erase "$(INTDIR)\gethostent.obj"
	-@erase "$(INTDIR)\gethostent.sbr"
	-@erase "$(INTDIR)\gethostent_r.obj"
	-@erase "$(INTDIR)\gethostent_r.sbr"
	-@erase "$(INTDIR)\getnameinfo.obj"
	-@erase "$(INTDIR)\getnameinfo.sbr"
	-@erase "$(INTDIR)\getnetent.obj"
	-@erase "$(INTDIR)\getnetent.sbr"
	-@erase "$(INTDIR)\getnetent_r.obj"
	-@erase "$(INTDIR)\getnetent_r.sbr"
	-@erase "$(INTDIR)\getnetgrent.obj"
	-@erase "$(INTDIR)\getnetgrent.sbr"
	-@erase "$(INTDIR)\getnetgrent_r.obj"
	-@erase "$(INTDIR)\getnetgrent_r.sbr"
	-@erase "$(INTDIR)\getopt.obj"
	-@erase "$(INTDIR)\getopt.sbr"
	-@erase "$(INTDIR)\getprotoent.obj"
	-@erase "$(INTDIR)\getprotoent.sbr"
	-@erase "$(INTDIR)\getprotoent_r.obj"
	-@erase "$(INTDIR)\getprotoent_r.sbr"
	-@erase "$(INTDIR)\getpwent.obj"
	-@erase "$(INTDIR)\getpwent.sbr"
	-@erase "$(INTDIR)\getpwent_r.obj"
	-@erase "$(INTDIR)\getpwent_r.sbr"
	-@erase "$(INTDIR)\getservent.obj"
	-@erase "$(INTDIR)\getservent.sbr"
	-@erase "$(INTDIR)\getservent_r.obj"
	-@erase "$(INTDIR)\getservent_r.sbr"
	-@erase "$(INTDIR)\gettimeofday.obj"
	-@erase "$(INTDIR)\gettimeofday.sbr"
	-@erase "$(INTDIR)\heap.obj"
	-@erase "$(INTDIR)\heap.sbr"
	-@erase "$(INTDIR)\herror.obj"
	-@erase "$(INTDIR)\herror.sbr"
	-@erase "$(INTDIR)\hesiod.obj"
	-@erase "$(INTDIR)\hesiod.sbr"
	-@erase "$(INTDIR)\hmac_link.obj"
	-@erase "$(INTDIR)\hmac_link.sbr"
	-@erase "$(INTDIR)\inet_addr.obj"
	-@erase "$(INTDIR)\inet_addr.sbr"
	-@erase "$(INTDIR)\inet_cidr_ntop.obj"
	-@erase "$(INTDIR)\inet_cidr_ntop.sbr"
	-@erase "$(INTDIR)\inet_cidr_pton.obj"
	-@erase "$(INTDIR)\inet_cidr_pton.sbr"
	-@erase "$(INTDIR)\inet_lnaof.obj"
	-@erase "$(INTDIR)\inet_lnaof.sbr"
	-@erase "$(INTDIR)\inet_makeaddr.obj"
	-@erase "$(INTDIR)\inet_makeaddr.sbr"
	-@erase "$(INTDIR)\inet_net_ntop.obj"
	-@erase "$(INTDIR)\inet_net_ntop.sbr"
	-@erase "$(INTDIR)\inet_net_pton.obj"
	-@erase "$(INTDIR)\inet_net_pton.sbr"
	-@erase "$(INTDIR)\inet_neta.obj"
	-@erase "$(INTDIR)\inet_neta.sbr"
	-@erase "$(INTDIR)\inet_netof.obj"
	-@erase "$(INTDIR)\inet_netof.sbr"
	-@erase "$(INTDIR)\inet_network.obj"
	-@erase "$(INTDIR)\inet_network.sbr"
	-@erase "$(INTDIR)\inet_ntoa.obj"
	-@erase "$(INTDIR)\inet_ntoa.sbr"
	-@erase "$(INTDIR)\inet_ntop.obj"
	-@erase "$(INTDIR)\inet_ntop.sbr"
	-@erase "$(INTDIR)\inet_pton.obj"
	-@erase "$(INTDIR)\inet_pton.sbr"
	-@erase "$(INTDIR)\intbits.obj"
	-@erase "$(INTDIR)\intbits.sbr"
	-@erase "$(INTDIR)\interface.obj"
	-@erase "$(INTDIR)\interface.sbr"
	-@erase "$(INTDIR)\intitem.obj"
	-@erase "$(INTDIR)\intitem.sbr"
	-@erase "$(INTDIR)\ioctl_if.obj"
	-@erase "$(INTDIR)\ioctl_if.sbr"
	-@erase "$(INTDIR)\irp.obj"
	-@erase "$(INTDIR)\irp.sbr"
	-@erase "$(INTDIR)\irp_gr.obj"
	-@erase "$(INTDIR)\irp_gr.sbr"
	-@erase "$(INTDIR)\irp_ho.obj"
	-@erase "$(INTDIR)\irp_ho.sbr"
	-@erase "$(INTDIR)\irp_ng.obj"
	-@erase "$(INTDIR)\irp_ng.sbr"
	-@erase "$(INTDIR)\irp_nw.obj"
	-@erase "$(INTDIR)\irp_nw.sbr"
	-@erase "$(INTDIR)\irp_pr.obj"
	-@erase "$(INTDIR)\irp_pr.sbr"
	-@erase "$(INTDIR)\irp_pw.obj"
	-@erase "$(INTDIR)\irp_pw.sbr"
	-@erase "$(INTDIR)\irp_sv.obj"
	-@erase "$(INTDIR)\irp_sv.sbr"
	-@erase "$(INTDIR)\irpmarshall.obj"
	-@erase "$(INTDIR)\irpmarshall.sbr"
	-@erase "$(INTDIR)\irs_data.obj"
	-@erase "$(INTDIR)\irs_data.sbr"
	-@erase "$(INTDIR)\keyobj.obj"
	-@erase "$(INTDIR)\keyobj.sbr"
	-@erase "$(INTDIR)\ki8byte.obj"
	-@erase "$(INTDIR)\ki8byte.sbr"
	-@erase "$(INTDIR)\kifulprv.obj"
	-@erase "$(INTDIR)\kifulprv.sbr"
	-@erase "$(INTDIR)\kiitem.obj"
	-@erase "$(INTDIR)\kiitem.sbr"
	-@erase "$(INTDIR)\kinfotyp.obj"
	-@erase "$(INTDIR)\kinfotyp.sbr"
	-@erase "$(INTDIR)\kipkcrpr.obj"
	-@erase "$(INTDIR)\kipkcrpr.sbr"
	-@erase "$(INTDIR)\kirsacrt.obj"
	-@erase "$(INTDIR)\kirsacrt.sbr"
	-@erase "$(INTDIR)\kirsapub.obj"
	-@erase "$(INTDIR)\kirsapub.sbr"
	-@erase "$(INTDIR)\lbn00.obj"
	-@erase "$(INTDIR)\lbn00.sbr"
	-@erase "$(INTDIR)\lbnmem.obj"
	-@erase "$(INTDIR)\lbnmem.sbr"
	-@erase "$(INTDIR)\lcl.obj"
	-@erase "$(INTDIR)\lcl.sbr"
	-@erase "$(INTDIR)\lcl_gr.obj"
	-@erase "$(INTDIR)\lcl_gr.sbr"
	-@erase "$(INTDIR)\lcl_ho.obj"
	-@erase "$(INTDIR)\lcl_ho.sbr"
	-@erase "$(INTDIR)\lcl_ng.obj"
	-@erase "$(INTDIR)\lcl_ng.sbr"
	-@erase "$(INTDIR)\lcl_nw.obj"
	-@erase "$(INTDIR)\lcl_nw.sbr"
	-@erase "$(INTDIR)\lcl_pr.obj"
	-@erase "$(INTDIR)\lcl_pr.sbr"
	-@erase "$(INTDIR)\lcl_pw.obj"
	-@erase "$(INTDIR)\lcl_pw.sbr"
	-@erase "$(INTDIR)\lcl_sv.obj"
	-@erase "$(INTDIR)\lcl_sv.sbr"
	-@erase "$(INTDIR)\legal.obj"
	-@erase "$(INTDIR)\legal.sbr"
	-@erase "$(INTDIR)\logging.obj"
	-@erase "$(INTDIR)\logging.sbr"
	-@erase "$(INTDIR)\math.obj"
	-@erase "$(INTDIR)\math.sbr"
	-@erase "$(INTDIR)\md5.obj"
	-@erase "$(INTDIR)\md5.sbr"
	-@erase "$(INTDIR)\md5_dgst.obj"
	-@erase "$(INTDIR)\md5_dgst.sbr"
	-@erase "$(INTDIR)\md5rand.obj"
	-@erase "$(INTDIR)\md5rand.sbr"
	-@erase "$(INTDIR)\memcluster.obj"
	-@erase "$(INTDIR)\memcluster.sbr"
	-@erase "$(INTDIR)\movefile.obj"
	-@erase "$(INTDIR)\movefile.sbr"
	-@erase "$(INTDIR)\nameserver.obj"
	-@erase "$(INTDIR)\nameserver.sbr"
	-@erase "$(INTDIR)\nis.obj"
	-@erase "$(INTDIR)\nis.sbr"
	-@erase "$(INTDIR)\nis_gr.obj"
	-@erase "$(INTDIR)\nis_gr.sbr"
	-@erase "$(INTDIR)\nis_ho.obj"
	-@erase "$(INTDIR)\nis_ho.sbr"
	-@erase "$(INTDIR)\nis_ng.obj"
	-@erase "$(INTDIR)\nis_ng.sbr"
	-@erase "$(INTDIR)\nis_nw.obj"
	-@erase "$(INTDIR)\nis_nw.sbr"
	-@erase "$(INTDIR)\nis_pr.obj"
	-@erase "$(INTDIR)\nis_pr.sbr"
	-@erase "$(INTDIR)\nis_pw.obj"
	-@erase "$(INTDIR)\nis_pw.sbr"
	-@erase "$(INTDIR)\nis_sv.obj"
	-@erase "$(INTDIR)\nis_sv.sbr"
	-@erase "$(INTDIR)\ns_date.obj"
	-@erase "$(INTDIR)\ns_date.sbr"
	-@erase "$(INTDIR)\ns_name.obj"
	-@erase "$(INTDIR)\ns_name.sbr"
	-@erase "$(INTDIR)\ns_netint.obj"
	-@erase "$(INTDIR)\ns_netint.sbr"
	-@erase "$(INTDIR)\ns_parse.obj"
	-@erase "$(INTDIR)\ns_parse.sbr"
	-@erase "$(INTDIR)\ns_print.obj"
	-@erase "$(INTDIR)\ns_print.sbr"
	-@erase "$(INTDIR)\ns_samedomain.obj"
	-@erase "$(INTDIR)\ns_samedomain.sbr"
	-@erase "$(INTDIR)\ns_sign.obj"
	-@erase "$(INTDIR)\ns_sign.sbr"
	-@erase "$(INTDIR)\ns_ttl.obj"
	-@erase "$(INTDIR)\ns_ttl.sbr"
	-@erase "$(INTDIR)\ns_verify.obj"
	-@erase "$(INTDIR)\ns_verify.sbr"
	-@erase "$(INTDIR)\nsap_addr.obj"
	-@erase "$(INTDIR)\nsap_addr.sbr"
	-@erase "$(INTDIR)\nul_ng.obj"
	-@erase "$(INTDIR)\nul_ng.sbr"
	-@erase "$(INTDIR)\prandom.obj"
	-@erase "$(INTDIR)\prandom.sbr"
	-@erase "$(INTDIR)\prime.obj"
	-@erase "$(INTDIR)\prime.sbr"
	-@erase "$(INTDIR)\putenv.obj"
	-@erase "$(INTDIR)\putenv.sbr"
	-@erase "$(INTDIR)\rand.obj"
	-@erase "$(INTDIR)\rand.sbr"
	-@erase "$(INTDIR)\random.obj"
	-@erase "$(INTDIR)\random.sbr"
	-@erase "$(INTDIR)\readv.obj"
	-@erase "$(INTDIR)\readv.sbr"
	-@erase "$(INTDIR)\res_comp.obj"
	-@erase "$(INTDIR)\res_comp.sbr"
	-@erase "$(INTDIR)\res_data.obj"
	-@erase "$(INTDIR)\res_data.sbr"
	-@erase "$(INTDIR)\res_debug.obj"
	-@erase "$(INTDIR)\res_debug.sbr"
	-@erase "$(INTDIR)\res_findzonecut.obj"
	-@erase "$(INTDIR)\res_findzonecut.sbr"
	-@erase "$(INTDIR)\res_init.obj"
	-@erase "$(INTDIR)\res_init.sbr"
	-@erase "$(INTDIR)\res_mkquery.obj"
	-@erase "$(INTDIR)\res_mkquery.sbr"
	-@erase "$(INTDIR)\res_mkupdate.obj"
	-@erase "$(INTDIR)\res_mkupdate.sbr"
	-@erase "$(INTDIR)\res_query.obj"
	-@erase "$(INTDIR)\res_query.sbr"
	-@erase "$(INTDIR)\res_send.obj"
	-@erase "$(INTDIR)\res_send.sbr"
	-@erase "$(INTDIR)\res_sendsigned.obj"
	-@erase "$(INTDIR)\res_sendsigned.sbr"
	-@erase "$(INTDIR)\res_update.obj"
	-@erase "$(INTDIR)\res_update.sbr"
	-@erase "$(INTDIR)\rsa.obj"
	-@erase "$(INTDIR)\rsa.sbr"
	-@erase "$(INTDIR)\rsakeygn.obj"
	-@erase "$(INTDIR)\rsakeygn.sbr"
	-@erase "$(INTDIR)\rsaref_link.obj"
	-@erase "$(INTDIR)\rsaref_link.sbr"
	-@erase "$(INTDIR)\seccbcd.obj"
	-@erase "$(INTDIR)\seccbcd.sbr"
	-@erase "$(INTDIR)\seccbce.obj"
	-@erase "$(INTDIR)\seccbce.sbr"
	-@erase "$(INTDIR)\setenv.obj"
	-@erase "$(INTDIR)\setenv.sbr"
	-@erase "$(INTDIR)\setitimer.obj"
	-@erase "$(INTDIR)\setitimer.sbr"
	-@erase "$(INTDIR)\sha.obj"
	-@erase "$(INTDIR)\sha.sbr"
	-@erase "$(INTDIR)\signal.obj"
	-@erase "$(INTDIR)\signal.sbr"
	-@erase "$(INTDIR)\socket.obj"
	-@erase "$(INTDIR)\socket.sbr"
	-@erase "$(INTDIR)\strcasecmp.obj"
	-@erase "$(INTDIR)\strcasecmp.sbr"
	-@erase "$(INTDIR)\strdup.obj"
	-@erase "$(INTDIR)\strdup.sbr"
	-@erase "$(INTDIR)\strpbrk.obj"
	-@erase "$(INTDIR)\strpbrk.sbr"
	-@erase "$(INTDIR)\strsep.obj"
	-@erase "$(INTDIR)\strsep.sbr"
	-@erase "$(INTDIR)\strtoul.obj"
	-@erase "$(INTDIR)\strtoul.sbr"
	-@erase "$(INTDIR)\support.obj"
	-@erase "$(INTDIR)\support.sbr"
	-@erase "$(INTDIR)\surrendr.obj"
	-@erase "$(INTDIR)\surrendr.sbr"
	-@erase "$(INTDIR)\swap.obj"
	-@erase "$(INTDIR)\swap.sbr"
	-@erase "$(INTDIR)\syslog.obj"
	-@erase "$(INTDIR)\syslog.sbr"
	-@erase "$(INTDIR)\tree.obj"
	-@erase "$(INTDIR)\tree.sbr"
	-@erase "$(INTDIR)\unistd.obj"
	-@erase "$(INTDIR)\unistd.sbr"
	-@erase "$(INTDIR)\util.obj"
	-@erase "$(INTDIR)\util.sbr"
	-@erase "$(INTDIR)\utimes.obj"
	-@erase "$(INTDIR)\utimes.sbr"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\writev.obj"
	-@erase "$(INTDIR)\writev.sbr"
	-@erase "$(OUTDIR)\libbind.bsc"
	-@erase "$(OUTDIR)\libbind.exp"
	-@erase "$(OUTDIR)\libbind.lib"
	-@erase "..\bin\Release\libbind.dll"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MD /W3 /GX /O2 /I "..\include" /I "..\..\..\include" /I "..\..\..\lib\dnssafe" /D "NDEBUG" /D "HAVE_CONFIG_H" /D "WIN32" /D "_WINDOWS" /D "LIB" /D "DNSSAFE" /D "i386" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\libbind.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\libbind.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\gettimeofday.sbr" \
	"$(INTDIR)\putenv.sbr" \
	"$(INTDIR)\readv.sbr" \
	"$(INTDIR)\setenv.sbr" \
	"$(INTDIR)\setitimer.sbr" \
	"$(INTDIR)\strcasecmp.sbr" \
	"$(INTDIR)\strdup.sbr" \
	"$(INTDIR)\strpbrk.sbr" \
	"$(INTDIR)\strsep.sbr" \
	"$(INTDIR)\strtoul.sbr" \
	"$(INTDIR)\utimes.sbr" \
	"$(INTDIR)\writev.sbr" \
	"$(INTDIR)\assertions.sbr" \
	"$(INTDIR)\base64.sbr" \
	"$(INTDIR)\bitncmp.sbr" \
	"$(INTDIR)\ctl_clnt.sbr" \
	"$(INTDIR)\ctl_p.sbr" \
	"$(INTDIR)\ctl_srvr.sbr" \
	"$(INTDIR)\ev_connects.sbr" \
	"$(INTDIR)\ev_files.sbr" \
	"$(INTDIR)\ev_streams.sbr" \
	"$(INTDIR)\ev_timers.sbr" \
	"$(INTDIR)\ev_waits.sbr" \
	"$(INTDIR)\eventlib.sbr" \
	"$(INTDIR)\heap.sbr" \
	"$(INTDIR)\logging.sbr" \
	"$(INTDIR)\memcluster.sbr" \
	"$(INTDIR)\tree.sbr" \
	"$(INTDIR)\dns.sbr" \
	"$(INTDIR)\dns_gr.sbr" \
	"$(INTDIR)\dns_ho.sbr" \
	"$(INTDIR)\dns_nw.sbr" \
	"$(INTDIR)\dns_pr.sbr" \
	"$(INTDIR)\dns_pw.sbr" \
	"$(INTDIR)\dns_sv.sbr" \
	"$(INTDIR)\gai_strerror.sbr" \
	"$(INTDIR)\gen.sbr" \
	"$(INTDIR)\gen_gr.sbr" \
	"$(INTDIR)\gen_ho.sbr" \
	"$(INTDIR)\gen_ng.sbr" \
	"$(INTDIR)\gen_nw.sbr" \
	"$(INTDIR)\gen_pr.sbr" \
	"$(INTDIR)\gen_pw.sbr" \
	"$(INTDIR)\gen_sv.sbr" \
	"$(INTDIR)\getaddrinfo.sbr" \
	"$(INTDIR)\getgrent.sbr" \
	"$(INTDIR)\getgrent_r.sbr" \
	"$(INTDIR)\gethostent.sbr" \
	"$(INTDIR)\gethostent_r.sbr" \
	"$(INTDIR)\getnameinfo.sbr" \
	"$(INTDIR)\getnetent.sbr" \
	"$(INTDIR)\getnetent_r.sbr" \
	"$(INTDIR)\getnetgrent.sbr" \
	"$(INTDIR)\getnetgrent_r.sbr" \
	"$(INTDIR)\getprotoent.sbr" \
	"$(INTDIR)\getprotoent_r.sbr" \
	"$(INTDIR)\getpwent.sbr" \
	"$(INTDIR)\getpwent_r.sbr" \
	"$(INTDIR)\getservent.sbr" \
	"$(INTDIR)\getservent_r.sbr" \
	"$(INTDIR)\hesiod.sbr" \
	"$(INTDIR)\irp.sbr" \
	"$(INTDIR)\irp_gr.sbr" \
	"$(INTDIR)\irp_ho.sbr" \
	"$(INTDIR)\irp_ng.sbr" \
	"$(INTDIR)\irp_nw.sbr" \
	"$(INTDIR)\irp_pr.sbr" \
	"$(INTDIR)\irp_pw.sbr" \
	"$(INTDIR)\irp_sv.sbr" \
	"$(INTDIR)\irpmarshall.sbr" \
	"$(INTDIR)\irs_data.sbr" \
	"$(INTDIR)\lcl.sbr" \
	"$(INTDIR)\lcl_gr.sbr" \
	"$(INTDIR)\lcl_ho.sbr" \
	"$(INTDIR)\lcl_ng.sbr" \
	"$(INTDIR)\lcl_nw.sbr" \
	"$(INTDIR)\lcl_pr.sbr" \
	"$(INTDIR)\lcl_pw.sbr" \
	"$(INTDIR)\lcl_sv.sbr" \
	"$(INTDIR)\nis.sbr" \
	"$(INTDIR)\nis_gr.sbr" \
	"$(INTDIR)\nis_ho.sbr" \
	"$(INTDIR)\nis_ng.sbr" \
	"$(INTDIR)\nis_nw.sbr" \
	"$(INTDIR)\nis_pr.sbr" \
	"$(INTDIR)\nis_pw.sbr" \
	"$(INTDIR)\nis_sv.sbr" \
	"$(INTDIR)\nul_ng.sbr" \
	"$(INTDIR)\util.sbr" \
	"$(INTDIR)\herror.sbr" \
	"$(INTDIR)\res_comp.sbr" \
	"$(INTDIR)\res_data.sbr" \
	"$(INTDIR)\res_debug.sbr" \
	"$(INTDIR)\res_findzonecut.sbr" \
	"$(INTDIR)\res_init.sbr" \
	"$(INTDIR)\res_mkquery.sbr" \
	"$(INTDIR)\res_mkupdate.sbr" \
	"$(INTDIR)\res_query.sbr" \
	"$(INTDIR)\res_send.sbr" \
	"$(INTDIR)\res_sendsigned.sbr" \
	"$(INTDIR)\res_update.sbr" \
	"$(INTDIR)\ns_date.sbr" \
	"$(INTDIR)\ns_name.sbr" \
	"$(INTDIR)\ns_netint.sbr" \
	"$(INTDIR)\ns_parse.sbr" \
	"$(INTDIR)\ns_print.sbr" \
	"$(INTDIR)\ns_samedomain.sbr" \
	"$(INTDIR)\ns_sign.sbr" \
	"$(INTDIR)\ns_ttl.sbr" \
	"$(INTDIR)\ns_verify.sbr" \
	"$(INTDIR)\inet_addr.sbr" \
	"$(INTDIR)\inet_cidr_ntop.sbr" \
	"$(INTDIR)\inet_cidr_pton.sbr" \
	"$(INTDIR)\inet_lnaof.sbr" \
	"$(INTDIR)\inet_makeaddr.sbr" \
	"$(INTDIR)\inet_net_ntop.sbr" \
	"$(INTDIR)\inet_net_pton.sbr" \
	"$(INTDIR)\inet_neta.sbr" \
	"$(INTDIR)\inet_netof.sbr" \
	"$(INTDIR)\inet_network.sbr" \
	"$(INTDIR)\inet_ntoa.sbr" \
	"$(INTDIR)\inet_ntop.sbr" \
	"$(INTDIR)\inet_pton.sbr" \
	"$(INTDIR)\nsap_addr.sbr" \
	"$(INTDIR)\bits.sbr" \
	"$(INTDIR)\bn.sbr" \
	"$(INTDIR)\bn00.sbr" \
	"$(INTDIR)\ctk_prime.sbr" \
	"$(INTDIR)\dss.sbr" \
	"$(INTDIR)\lbn00.sbr" \
	"$(INTDIR)\lbnmem.sbr" \
	"$(INTDIR)\legal.sbr" \
	"$(INTDIR)\math.sbr" \
	"$(INTDIR)\rand.sbr" \
	"$(INTDIR)\sha.sbr" \
	"$(INTDIR)\swap.sbr" \
	"$(INTDIR)\bsafe_link.sbr" \
	"$(INTDIR)\cylink_link.sbr" \
	"$(INTDIR)\dst_api.sbr" \
	"$(INTDIR)\eay_dss_link.sbr" \
	"$(INTDIR)\hmac_link.sbr" \
	"$(INTDIR)\md5_dgst.sbr" \
	"$(INTDIR)\prandom.sbr" \
	"$(INTDIR)\rsaref_link.sbr" \
	"$(INTDIR)\support.sbr" \
	"$(INTDIR)\dirent.sbr" \
	"$(INTDIR)\DLLMain.sbr" \
	"$(INTDIR)\files.sbr" \
	"$(INTDIR)\getopt.sbr" \
	"$(INTDIR)\interface.sbr" \
	"$(INTDIR)\ioctl_if.sbr" \
	"$(INTDIR)\movefile.sbr" \
	"$(INTDIR)\nameserver.sbr" \
	"$(INTDIR)\signal.sbr" \
	"$(INTDIR)\socket.sbr" \
	"$(INTDIR)\syslog.sbr" \
	"$(INTDIR)\unistd.sbr" \
	"$(INTDIR)\ahcbcpad.sbr" \
	"$(INTDIR)\ahchdig.sbr" \
	"$(INTDIR)\ahchencr.sbr" \
	"$(INTDIR)\ahchgen.sbr" \
	"$(INTDIR)\ahchrand.sbr" \
	"$(INTDIR)\ahdigest.sbr" \
	"$(INTDIR)\ahencryp.sbr" \
	"$(INTDIR)\ahgen.sbr" \
	"$(INTDIR)\ahrandom.sbr" \
	"$(INTDIR)\ahrsaenc.sbr" \
	"$(INTDIR)\ahrsaepr.sbr" \
	"$(INTDIR)\ahrsaepu.sbr" \
	"$(INTDIR)\aichdig.sbr" \
	"$(INTDIR)\aichenc8.sbr" \
	"$(INTDIR)\aichencn.sbr" \
	"$(INTDIR)\aichencr.sbr" \
	"$(INTDIR)\aichgen.sbr" \
	"$(INTDIR)\aichrand.sbr" \
	"$(INTDIR)\aimd5.sbr" \
	"$(INTDIR)\aimd5ran.sbr" \
	"$(INTDIR)\ainfotyp.sbr" \
	"$(INTDIR)\ainull.sbr" \
	"$(INTDIR)\airsaepr.sbr" \
	"$(INTDIR)\airsaepu.sbr" \
	"$(INTDIR)\airsakgn.sbr" \
	"$(INTDIR)\airsaprv.sbr" \
	"$(INTDIR)\airsapub.sbr" \
	"$(INTDIR)\algchoic.sbr" \
	"$(INTDIR)\algobj.sbr" \
	"$(INTDIR)\amcrte.sbr" \
	"$(INTDIR)\ammd5.sbr" \
	"$(INTDIR)\ammd5r.sbr" \
	"$(INTDIR)\amrkg.sbr" \
	"$(INTDIR)\amrsae.sbr" \
	"$(INTDIR)\balg.sbr" \
	"$(INTDIR)\bgclrbit.sbr" \
	"$(INTDIR)\bgmdmpyx.sbr" \
	"$(INTDIR)\bgmdsqx.sbr" \
	"$(INTDIR)\bgmodexp.sbr" \
	"$(INTDIR)\bgpegcd.sbr" \
	"$(INTDIR)\big2exp.sbr" \
	"$(INTDIR)\bigabs.sbr" \
	"$(INTDIR)\bigacc.sbr" \
	"$(INTDIR)\bigarith.sbr" \
	"$(INTDIR)\bigcmp.sbr" \
	"$(INTDIR)\bigconst.sbr" \
	"$(INTDIR)\biginv.sbr" \
	"$(INTDIR)\biglen.sbr" \
	"$(INTDIR)\bigmodx.sbr" \
	"$(INTDIR)\bigmpy.sbr" \
	"$(INTDIR)\bigpdiv.sbr" \
	"$(INTDIR)\bigpmpy.sbr" \
	"$(INTDIR)\bigpmpyh.sbr" \
	"$(INTDIR)\bigpmpyl.sbr" \
	"$(INTDIR)\bigpsq.sbr" \
	"$(INTDIR)\bigqrx.sbr" \
	"$(INTDIR)\bigsmod.sbr" \
	"$(INTDIR)\bigtocan.sbr" \
	"$(INTDIR)\bigu.sbr" \
	"$(INTDIR)\bigunexp.sbr" \
	"$(INTDIR)\binfocsh.sbr" \
	"$(INTDIR)\bkey.sbr" \
	"$(INTDIR)\bmempool.sbr" \
	"$(INTDIR)\cantobig.sbr" \
	"$(INTDIR)\crt2.sbr" \
	"$(INTDIR)\digest.sbr" \
	"$(INTDIR)\digrand.sbr" \
	"$(INTDIR)\encrypt.sbr" \
	"$(INTDIR)\generate.sbr" \
	"$(INTDIR)\intbits.sbr" \
	"$(INTDIR)\intitem.sbr" \
	"$(INTDIR)\keyobj.sbr" \
	"$(INTDIR)\ki8byte.sbr" \
	"$(INTDIR)\kifulprv.sbr" \
	"$(INTDIR)\kiitem.sbr" \
	"$(INTDIR)\kinfotyp.sbr" \
	"$(INTDIR)\kipkcrpr.sbr" \
	"$(INTDIR)\kirsacrt.sbr" \
	"$(INTDIR)\kirsapub.sbr" \
	"$(INTDIR)\md5.sbr" \
	"$(INTDIR)\md5rand.sbr" \
	"$(INTDIR)\prime.sbr" \
	"$(INTDIR)\random.sbr" \
	"$(INTDIR)\rsa.sbr" \
	"$(INTDIR)\rsakeygn.sbr" \
	"$(INTDIR)\seccbcd.sbr" \
	"$(INTDIR)\seccbce.sbr" \
	"$(INTDIR)\surrendr.sbr"

"$(OUTDIR)\libbind.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
LINK32_FLAGS=user32.lib advapi32.lib netapi32.lib /nologo /subsystem:windows /dll /pdb:none /machine:I386 /def:".\libbind.def" /out:"..\bin\Release\libbind.dll" /implib:"$(OUTDIR)\libbind.lib" 
DEF_FILE= \
	".\libbind.def"
LINK32_OBJS= \
	"$(INTDIR)\gettimeofday.obj" \
	"$(INTDIR)\putenv.obj" \
	"$(INTDIR)\readv.obj" \
	"$(INTDIR)\setenv.obj" \
	"$(INTDIR)\setitimer.obj" \
	"$(INTDIR)\strcasecmp.obj" \
	"$(INTDIR)\strdup.obj" \
	"$(INTDIR)\strpbrk.obj" \
	"$(INTDIR)\strsep.obj" \
	"$(INTDIR)\strtoul.obj" \
	"$(INTDIR)\utimes.obj" \
	"$(INTDIR)\writev.obj" \
	"$(INTDIR)\assertions.obj" \
	"$(INTDIR)\base64.obj" \
	"$(INTDIR)\bitncmp.obj" \
	"$(INTDIR)\ctl_clnt.obj" \
	"$(INTDIR)\ctl_p.obj" \
	"$(INTDIR)\ctl_srvr.obj" \
	"$(INTDIR)\ev_connects.obj" \
	"$(INTDIR)\ev_files.obj" \
	"$(INTDIR)\ev_streams.obj" \
	"$(INTDIR)\ev_timers.obj" \
	"$(INTDIR)\ev_waits.obj" \
	"$(INTDIR)\eventlib.obj" \
	"$(INTDIR)\heap.obj" \
	"$(INTDIR)\logging.obj" \
	"$(INTDIR)\memcluster.obj" \
	"$(INTDIR)\tree.obj" \
	"$(INTDIR)\dns.obj" \
	"$(INTDIR)\dns_gr.obj" \
	"$(INTDIR)\dns_ho.obj" \
	"$(INTDIR)\dns_nw.obj" \
	"$(INTDIR)\dns_pr.obj" \
	"$(INTDIR)\dns_pw.obj" \
	"$(INTDIR)\dns_sv.obj" \
	"$(INTDIR)\gai_strerror.obj" \
	"$(INTDIR)\gen.obj" \
	"$(INTDIR)\gen_gr.obj" \
	"$(INTDIR)\gen_ho.obj" \
	"$(INTDIR)\gen_ng.obj" \
	"$(INTDIR)\gen_nw.obj" \
	"$(INTDIR)\gen_pr.obj" \
	"$(INTDIR)\gen_pw.obj" \
	"$(INTDIR)\gen_sv.obj" \
	"$(INTDIR)\getaddrinfo.obj" \
	"$(INTDIR)\getgrent.obj" \
	"$(INTDIR)\getgrent_r.obj" \
	"$(INTDIR)\gethostent.obj" \
	"$(INTDIR)\gethostent_r.obj" \
	"$(INTDIR)\getnameinfo.obj" \
	"$(INTDIR)\getnetent.obj" \
	"$(INTDIR)\getnetent_r.obj" \
	"$(INTDIR)\getnetgrent.obj" \
	"$(INTDIR)\getnetgrent_r.obj" \
	"$(INTDIR)\getprotoent.obj" \
	"$(INTDIR)\getprotoent_r.obj" \
	"$(INTDIR)\getpwent.obj" \
	"$(INTDIR)\getpwent_r.obj" \
	"$(INTDIR)\getservent.obj" \
	"$(INTDIR)\getservent_r.obj" \
	"$(INTDIR)\hesiod.obj" \
	"$(INTDIR)\irp.obj" \
	"$(INTDIR)\irp_gr.obj" \
	"$(INTDIR)\irp_ho.obj" \
	"$(INTDIR)\irp_ng.obj" \
	"$(INTDIR)\irp_nw.obj" \
	"$(INTDIR)\irp_pr.obj" \
	"$(INTDIR)\irp_pw.obj" \
	"$(INTDIR)\irp_sv.obj" \
	"$(INTDIR)\irpmarshall.obj" \
	"$(INTDIR)\irs_data.obj" \
	"$(INTDIR)\lcl.obj" \
	"$(INTDIR)\lcl_gr.obj" \
	"$(INTDIR)\lcl_ho.obj" \
	"$(INTDIR)\lcl_ng.obj" \
	"$(INTDIR)\lcl_nw.obj" \
	"$(INTDIR)\lcl_pr.obj" \
	"$(INTDIR)\lcl_pw.obj" \
	"$(INTDIR)\lcl_sv.obj" \
	"$(INTDIR)\nis.obj" \
	"$(INTDIR)\nis_gr.obj" \
	"$(INTDIR)\nis_ho.obj" \
	"$(INTDIR)\nis_ng.obj" \
	"$(INTDIR)\nis_nw.obj" \
	"$(INTDIR)\nis_pr.obj" \
	"$(INTDIR)\nis_pw.obj" \
	"$(INTDIR)\nis_sv.obj" \
	"$(INTDIR)\nul_ng.obj" \
	"$(INTDIR)\util.obj" \
	"$(INTDIR)\herror.obj" \
	"$(INTDIR)\res_comp.obj" \
	"$(INTDIR)\res_data.obj" \
	"$(INTDIR)\res_debug.obj" \
	"$(INTDIR)\res_findzonecut.obj" \
	"$(INTDIR)\res_init.obj" \
	"$(INTDIR)\res_mkquery.obj" \
	"$(INTDIR)\res_mkupdate.obj" \
	"$(INTDIR)\res_query.obj" \
	"$(INTDIR)\res_send.obj" \
	"$(INTDIR)\res_sendsigned.obj" \
	"$(INTDIR)\res_update.obj" \
	"$(INTDIR)\ns_date.obj" \
	"$(INTDIR)\ns_name.obj" \
	"$(INTDIR)\ns_netint.obj" \
	"$(INTDIR)\ns_parse.obj" \
	"$(INTDIR)\ns_print.obj" \
	"$(INTDIR)\ns_samedomain.obj" \
	"$(INTDIR)\ns_sign.obj" \
	"$(INTDIR)\ns_ttl.obj" \
	"$(INTDIR)\ns_verify.obj" \
	"$(INTDIR)\inet_addr.obj" \
	"$(INTDIR)\inet_cidr_ntop.obj" \
	"$(INTDIR)\inet_cidr_pton.obj" \
	"$(INTDIR)\inet_lnaof.obj" \
	"$(INTDIR)\inet_makeaddr.obj" \
	"$(INTDIR)\inet_net_ntop.obj" \
	"$(INTDIR)\inet_net_pton.obj" \
	"$(INTDIR)\inet_neta.obj" \
	"$(INTDIR)\inet_netof.obj" \
	"$(INTDIR)\inet_network.obj" \
	"$(INTDIR)\inet_ntoa.obj" \
	"$(INTDIR)\inet_ntop.obj" \
	"$(INTDIR)\inet_pton.obj" \
	"$(INTDIR)\nsap_addr.obj" \
	"$(INTDIR)\bits.obj" \
	"$(INTDIR)\bn.obj" \
	"$(INTDIR)\bn00.obj" \
	"$(INTDIR)\ctk_prime.obj" \
	"$(INTDIR)\dss.obj" \
	"$(INTDIR)\lbn00.obj" \
	"$(INTDIR)\lbnmem.obj" \
	"$(INTDIR)\legal.obj" \
	"$(INTDIR)\math.obj" \
	"$(INTDIR)\rand.obj" \
	"$(INTDIR)\sha.obj" \
	"$(INTDIR)\swap.obj" \
	"$(INTDIR)\bsafe_link.obj" \
	"$(INTDIR)\cylink_link.obj" \
	"$(INTDIR)\dst_api.obj" \
	"$(INTDIR)\eay_dss_link.obj" \
	"$(INTDIR)\hmac_link.obj" \
	"$(INTDIR)\md5_dgst.obj" \
	"$(INTDIR)\prandom.obj" \
	"$(INTDIR)\rsaref_link.obj" \
	"$(INTDIR)\support.obj" \
	"$(INTDIR)\dirent.obj" \
	"$(INTDIR)\DLLMain.obj" \
	"$(INTDIR)\files.obj" \
	"$(INTDIR)\getopt.obj" \
	"$(INTDIR)\interface.obj" \
	"$(INTDIR)\ioctl_if.obj" \
	"$(INTDIR)\movefile.obj" \
	"$(INTDIR)\nameserver.obj" \
	"$(INTDIR)\signal.obj" \
	"$(INTDIR)\socket.obj" \
	"$(INTDIR)\syslog.obj" \
	"$(INTDIR)\unistd.obj" \
	"$(INTDIR)\ahcbcpad.obj" \
	"$(INTDIR)\ahchdig.obj" \
	"$(INTDIR)\ahchencr.obj" \
	"$(INTDIR)\ahchgen.obj" \
	"$(INTDIR)\ahchrand.obj" \
	"$(INTDIR)\ahdigest.obj" \
	"$(INTDIR)\ahencryp.obj" \
	"$(INTDIR)\ahgen.obj" \
	"$(INTDIR)\ahrandom.obj" \
	"$(INTDIR)\ahrsaenc.obj" \
	"$(INTDIR)\ahrsaepr.obj" \
	"$(INTDIR)\ahrsaepu.obj" \
	"$(INTDIR)\aichdig.obj" \
	"$(INTDIR)\aichenc8.obj" \
	"$(INTDIR)\aichencn.obj" \
	"$(INTDIR)\aichencr.obj" \
	"$(INTDIR)\aichgen.obj" \
	"$(INTDIR)\aichrand.obj" \
	"$(INTDIR)\aimd5.obj" \
	"$(INTDIR)\aimd5ran.obj" \
	"$(INTDIR)\ainfotyp.obj" \
	"$(INTDIR)\ainull.obj" \
	"$(INTDIR)\airsaepr.obj" \
	"$(INTDIR)\airsaepu.obj" \
	"$(INTDIR)\airsakgn.obj" \
	"$(INTDIR)\airsaprv.obj" \
	"$(INTDIR)\airsapub.obj" \
	"$(INTDIR)\algchoic.obj" \
	"$(INTDIR)\algobj.obj" \
	"$(INTDIR)\amcrte.obj" \
	"$(INTDIR)\ammd5.obj" \
	"$(INTDIR)\ammd5r.obj" \
	"$(INTDIR)\amrkg.obj" \
	"$(INTDIR)\amrsae.obj" \
	"$(INTDIR)\balg.obj" \
	"$(INTDIR)\bgclrbit.obj" \
	"$(INTDIR)\bgmdmpyx.obj" \
	"$(INTDIR)\bgmdsqx.obj" \
	"$(INTDIR)\bgmodexp.obj" \
	"$(INTDIR)\bgpegcd.obj" \
	"$(INTDIR)\big2exp.obj" \
	"$(INTDIR)\bigabs.obj" \
	"$(INTDIR)\bigacc.obj" \
	"$(INTDIR)\bigarith.obj" \
	"$(INTDIR)\bigcmp.obj" \
	"$(INTDIR)\bigconst.obj" \
	"$(INTDIR)\biginv.obj" \
	"$(INTDIR)\biglen.obj" \
	"$(INTDIR)\bigmodx.obj" \
	"$(INTDIR)\bigmpy.obj" \
	"$(INTDIR)\bigpdiv.obj" \
	"$(INTDIR)\bigpmpy.obj" \
	"$(INTDIR)\bigpmpyh.obj" \
	"$(INTDIR)\bigpmpyl.obj" \
	"$(INTDIR)\bigpsq.obj" \
	"$(INTDIR)\bigqrx.obj" \
	"$(INTDIR)\bigsmod.obj" \
	"$(INTDIR)\bigtocan.obj" \
	"$(INTDIR)\bigu.obj" \
	"$(INTDIR)\bigunexp.obj" \
	"$(INTDIR)\binfocsh.obj" \
	"$(INTDIR)\bkey.obj" \
	"$(INTDIR)\bmempool.obj" \
	"$(INTDIR)\cantobig.obj" \
	"$(INTDIR)\crt2.obj" \
	"$(INTDIR)\digest.obj" \
	"$(INTDIR)\digrand.obj" \
	"$(INTDIR)\encrypt.obj" \
	"$(INTDIR)\generate.obj" \
	"$(INTDIR)\intbits.obj" \
	"$(INTDIR)\intitem.obj" \
	"$(INTDIR)\keyobj.obj" \
	"$(INTDIR)\ki8byte.obj" \
	"$(INTDIR)\kifulprv.obj" \
	"$(INTDIR)\kiitem.obj" \
	"$(INTDIR)\kinfotyp.obj" \
	"$(INTDIR)\kipkcrpr.obj" \
	"$(INTDIR)\kirsacrt.obj" \
	"$(INTDIR)\kirsapub.obj" \
	"$(INTDIR)\md5.obj" \
	"$(INTDIR)\md5rand.obj" \
	"$(INTDIR)\prime.obj" \
	"$(INTDIR)\random.obj" \
	"$(INTDIR)\rsa.obj" \
	"$(INTDIR)\rsakeygn.obj" \
	"$(INTDIR)\seccbcd.obj" \
	"$(INTDIR)\seccbce.obj" \
	"$(INTDIR)\surrendr.obj"

"..\bin\Release\libbind.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "libbind - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug
# Begin Custom Macros
OutDir=.\Debug
# End Custom Macros

ALL : "..\bin\Debug\libbind.dll" "$(OUTDIR)\libbind.bsc"


CLEAN :
	-@erase "$(INTDIR)\ahcbcpad.obj"
	-@erase "$(INTDIR)\ahcbcpad.sbr"
	-@erase "$(INTDIR)\ahchdig.obj"
	-@erase "$(INTDIR)\ahchdig.sbr"
	-@erase "$(INTDIR)\ahchencr.obj"
	-@erase "$(INTDIR)\ahchencr.sbr"
	-@erase "$(INTDIR)\ahchgen.obj"
	-@erase "$(INTDIR)\ahchgen.sbr"
	-@erase "$(INTDIR)\ahchrand.obj"
	-@erase "$(INTDIR)\ahchrand.sbr"
	-@erase "$(INTDIR)\ahdigest.obj"
	-@erase "$(INTDIR)\ahdigest.sbr"
	-@erase "$(INTDIR)\ahencryp.obj"
	-@erase "$(INTDIR)\ahencryp.sbr"
	-@erase "$(INTDIR)\ahgen.obj"
	-@erase "$(INTDIR)\ahgen.sbr"
	-@erase "$(INTDIR)\ahrandom.obj"
	-@erase "$(INTDIR)\ahrandom.sbr"
	-@erase "$(INTDIR)\ahrsaenc.obj"
	-@erase "$(INTDIR)\ahrsaenc.sbr"
	-@erase "$(INTDIR)\ahrsaepr.obj"
	-@erase "$(INTDIR)\ahrsaepr.sbr"
	-@erase "$(INTDIR)\ahrsaepu.obj"
	-@erase "$(INTDIR)\ahrsaepu.sbr"
	-@erase "$(INTDIR)\aichdig.obj"
	-@erase "$(INTDIR)\aichdig.sbr"
	-@erase "$(INTDIR)\aichenc8.obj"
	-@erase "$(INTDIR)\aichenc8.sbr"
	-@erase "$(INTDIR)\aichencn.obj"
	-@erase "$(INTDIR)\aichencn.sbr"
	-@erase "$(INTDIR)\aichencr.obj"
	-@erase "$(INTDIR)\aichencr.sbr"
	-@erase "$(INTDIR)\aichgen.obj"
	-@erase "$(INTDIR)\aichgen.sbr"
	-@erase "$(INTDIR)\aichrand.obj"
	-@erase "$(INTDIR)\aichrand.sbr"
	-@erase "$(INTDIR)\aimd5.obj"
	-@erase "$(INTDIR)\aimd5.sbr"
	-@erase "$(INTDIR)\aimd5ran.obj"
	-@erase "$(INTDIR)\aimd5ran.sbr"
	-@erase "$(INTDIR)\ainfotyp.obj"
	-@erase "$(INTDIR)\ainfotyp.sbr"
	-@erase "$(INTDIR)\ainull.obj"
	-@erase "$(INTDIR)\ainull.sbr"
	-@erase "$(INTDIR)\airsaepr.obj"
	-@erase "$(INTDIR)\airsaepr.sbr"
	-@erase "$(INTDIR)\airsaepu.obj"
	-@erase "$(INTDIR)\airsaepu.sbr"
	-@erase "$(INTDIR)\airsakgn.obj"
	-@erase "$(INTDIR)\airsakgn.sbr"
	-@erase "$(INTDIR)\airsaprv.obj"
	-@erase "$(INTDIR)\airsaprv.sbr"
	-@erase "$(INTDIR)\airsapub.obj"
	-@erase "$(INTDIR)\airsapub.sbr"
	-@erase "$(INTDIR)\algchoic.obj"
	-@erase "$(INTDIR)\algchoic.sbr"
	-@erase "$(INTDIR)\algobj.obj"
	-@erase "$(INTDIR)\algobj.sbr"
	-@erase "$(INTDIR)\amcrte.obj"
	-@erase "$(INTDIR)\amcrte.sbr"
	-@erase "$(INTDIR)\ammd5.obj"
	-@erase "$(INTDIR)\ammd5.sbr"
	-@erase "$(INTDIR)\ammd5r.obj"
	-@erase "$(INTDIR)\ammd5r.sbr"
	-@erase "$(INTDIR)\amrkg.obj"
	-@erase "$(INTDIR)\amrkg.sbr"
	-@erase "$(INTDIR)\amrsae.obj"
	-@erase "$(INTDIR)\amrsae.sbr"
	-@erase "$(INTDIR)\assertions.obj"
	-@erase "$(INTDIR)\assertions.sbr"
	-@erase "$(INTDIR)\balg.obj"
	-@erase "$(INTDIR)\balg.sbr"
	-@erase "$(INTDIR)\base64.obj"
	-@erase "$(INTDIR)\base64.sbr"
	-@erase "$(INTDIR)\bgclrbit.obj"
	-@erase "$(INTDIR)\bgclrbit.sbr"
	-@erase "$(INTDIR)\bgmdmpyx.obj"
	-@erase "$(INTDIR)\bgmdmpyx.sbr"
	-@erase "$(INTDIR)\bgmdsqx.obj"
	-@erase "$(INTDIR)\bgmdsqx.sbr"
	-@erase "$(INTDIR)\bgmodexp.obj"
	-@erase "$(INTDIR)\bgmodexp.sbr"
	-@erase "$(INTDIR)\bgpegcd.obj"
	-@erase "$(INTDIR)\bgpegcd.sbr"
	-@erase "$(INTDIR)\big2exp.obj"
	-@erase "$(INTDIR)\big2exp.sbr"
	-@erase "$(INTDIR)\bigabs.obj"
	-@erase "$(INTDIR)\bigabs.sbr"
	-@erase "$(INTDIR)\bigacc.obj"
	-@erase "$(INTDIR)\bigacc.sbr"
	-@erase "$(INTDIR)\bigarith.obj"
	-@erase "$(INTDIR)\bigarith.sbr"
	-@erase "$(INTDIR)\bigcmp.obj"
	-@erase "$(INTDIR)\bigcmp.sbr"
	-@erase "$(INTDIR)\bigconst.obj"
	-@erase "$(INTDIR)\bigconst.sbr"
	-@erase "$(INTDIR)\biginv.obj"
	-@erase "$(INTDIR)\biginv.sbr"
	-@erase "$(INTDIR)\biglen.obj"
	-@erase "$(INTDIR)\biglen.sbr"
	-@erase "$(INTDIR)\bigmodx.obj"
	-@erase "$(INTDIR)\bigmodx.sbr"
	-@erase "$(INTDIR)\bigmpy.obj"
	-@erase "$(INTDIR)\bigmpy.sbr"
	-@erase "$(INTDIR)\bigpdiv.obj"
	-@erase "$(INTDIR)\bigpdiv.sbr"
	-@erase "$(INTDIR)\bigpmpy.obj"
	-@erase "$(INTDIR)\bigpmpy.sbr"
	-@erase "$(INTDIR)\bigpmpyh.obj"
	-@erase "$(INTDIR)\bigpmpyh.sbr"
	-@erase "$(INTDIR)\bigpmpyl.obj"
	-@erase "$(INTDIR)\bigpmpyl.sbr"
	-@erase "$(INTDIR)\bigpsq.obj"
	-@erase "$(INTDIR)\bigpsq.sbr"
	-@erase "$(INTDIR)\bigqrx.obj"
	-@erase "$(INTDIR)\bigqrx.sbr"
	-@erase "$(INTDIR)\bigsmod.obj"
	-@erase "$(INTDIR)\bigsmod.sbr"
	-@erase "$(INTDIR)\bigtocan.obj"
	-@erase "$(INTDIR)\bigtocan.sbr"
	-@erase "$(INTDIR)\bigu.obj"
	-@erase "$(INTDIR)\bigu.sbr"
	-@erase "$(INTDIR)\bigunexp.obj"
	-@erase "$(INTDIR)\bigunexp.sbr"
	-@erase "$(INTDIR)\binfocsh.obj"
	-@erase "$(INTDIR)\binfocsh.sbr"
	-@erase "$(INTDIR)\bitncmp.obj"
	-@erase "$(INTDIR)\bitncmp.sbr"
	-@erase "$(INTDIR)\bits.obj"
	-@erase "$(INTDIR)\bits.sbr"
	-@erase "$(INTDIR)\bkey.obj"
	-@erase "$(INTDIR)\bkey.sbr"
	-@erase "$(INTDIR)\bmempool.obj"
	-@erase "$(INTDIR)\bmempool.sbr"
	-@erase "$(INTDIR)\bn.obj"
	-@erase "$(INTDIR)\bn.sbr"
	-@erase "$(INTDIR)\bn00.obj"
	-@erase "$(INTDIR)\bn00.sbr"
	-@erase "$(INTDIR)\bsafe_link.obj"
	-@erase "$(INTDIR)\bsafe_link.sbr"
	-@erase "$(INTDIR)\cantobig.obj"
	-@erase "$(INTDIR)\cantobig.sbr"
	-@erase "$(INTDIR)\crt2.obj"
	-@erase "$(INTDIR)\crt2.sbr"
	-@erase "$(INTDIR)\ctk_prime.obj"
	-@erase "$(INTDIR)\ctk_prime.sbr"
	-@erase "$(INTDIR)\ctl_clnt.obj"
	-@erase "$(INTDIR)\ctl_clnt.sbr"
	-@erase "$(INTDIR)\ctl_p.obj"
	-@erase "$(INTDIR)\ctl_p.sbr"
	-@erase "$(INTDIR)\ctl_srvr.obj"
	-@erase "$(INTDIR)\ctl_srvr.sbr"
	-@erase "$(INTDIR)\cylink_link.obj"
	-@erase "$(INTDIR)\cylink_link.sbr"
	-@erase "$(INTDIR)\digest.obj"
	-@erase "$(INTDIR)\digest.sbr"
	-@erase "$(INTDIR)\digrand.obj"
	-@erase "$(INTDIR)\digrand.sbr"
	-@erase "$(INTDIR)\dirent.obj"
	-@erase "$(INTDIR)\dirent.sbr"
	-@erase "$(INTDIR)\DLLMain.obj"
	-@erase "$(INTDIR)\DLLMain.sbr"
	-@erase "$(INTDIR)\dns.obj"
	-@erase "$(INTDIR)\dns.sbr"
	-@erase "$(INTDIR)\dns_gr.obj"
	-@erase "$(INTDIR)\dns_gr.sbr"
	-@erase "$(INTDIR)\dns_ho.obj"
	-@erase "$(INTDIR)\dns_ho.sbr"
	-@erase "$(INTDIR)\dns_nw.obj"
	-@erase "$(INTDIR)\dns_nw.sbr"
	-@erase "$(INTDIR)\dns_pr.obj"
	-@erase "$(INTDIR)\dns_pr.sbr"
	-@erase "$(INTDIR)\dns_pw.obj"
	-@erase "$(INTDIR)\dns_pw.sbr"
	-@erase "$(INTDIR)\dns_sv.obj"
	-@erase "$(INTDIR)\dns_sv.sbr"
	-@erase "$(INTDIR)\dss.obj"
	-@erase "$(INTDIR)\dss.sbr"
	-@erase "$(INTDIR)\dst_api.obj"
	-@erase "$(INTDIR)\dst_api.sbr"
	-@erase "$(INTDIR)\eay_dss_link.obj"
	-@erase "$(INTDIR)\eay_dss_link.sbr"
	-@erase "$(INTDIR)\encrypt.obj"
	-@erase "$(INTDIR)\encrypt.sbr"
	-@erase "$(INTDIR)\ev_connects.obj"
	-@erase "$(INTDIR)\ev_connects.sbr"
	-@erase "$(INTDIR)\ev_files.obj"
	-@erase "$(INTDIR)\ev_files.sbr"
	-@erase "$(INTDIR)\ev_streams.obj"
	-@erase "$(INTDIR)\ev_streams.sbr"
	-@erase "$(INTDIR)\ev_timers.obj"
	-@erase "$(INTDIR)\ev_timers.sbr"
	-@erase "$(INTDIR)\ev_waits.obj"
	-@erase "$(INTDIR)\ev_waits.sbr"
	-@erase "$(INTDIR)\eventlib.obj"
	-@erase "$(INTDIR)\eventlib.sbr"
	-@erase "$(INTDIR)\files.obj"
	-@erase "$(INTDIR)\files.sbr"
	-@erase "$(INTDIR)\gai_strerror.obj"
	-@erase "$(INTDIR)\gai_strerror.sbr"
	-@erase "$(INTDIR)\gen.obj"
	-@erase "$(INTDIR)\gen.sbr"
	-@erase "$(INTDIR)\gen_gr.obj"
	-@erase "$(INTDIR)\gen_gr.sbr"
	-@erase "$(INTDIR)\gen_ho.obj"
	-@erase "$(INTDIR)\gen_ho.sbr"
	-@erase "$(INTDIR)\gen_ng.obj"
	-@erase "$(INTDIR)\gen_ng.sbr"
	-@erase "$(INTDIR)\gen_nw.obj"
	-@erase "$(INTDIR)\gen_nw.sbr"
	-@erase "$(INTDIR)\gen_pr.obj"
	-@erase "$(INTDIR)\gen_pr.sbr"
	-@erase "$(INTDIR)\gen_pw.obj"
	-@erase "$(INTDIR)\gen_pw.sbr"
	-@erase "$(INTDIR)\gen_sv.obj"
	-@erase "$(INTDIR)\gen_sv.sbr"
	-@erase "$(INTDIR)\generate.obj"
	-@erase "$(INTDIR)\generate.sbr"
	-@erase "$(INTDIR)\getaddrinfo.obj"
	-@erase "$(INTDIR)\getaddrinfo.sbr"
	-@erase "$(INTDIR)\getgrent.obj"
	-@erase "$(INTDIR)\getgrent.sbr"
	-@erase "$(INTDIR)\getgrent_r.obj"
	-@erase "$(INTDIR)\getgrent_r.sbr"
	-@erase "$(INTDIR)\gethostent.obj"
	-@erase "$(INTDIR)\gethostent.sbr"
	-@erase "$(INTDIR)\gethostent_r.obj"
	-@erase "$(INTDIR)\gethostent_r.sbr"
	-@erase "$(INTDIR)\getnameinfo.obj"
	-@erase "$(INTDIR)\getnameinfo.sbr"
	-@erase "$(INTDIR)\getnetent.obj"
	-@erase "$(INTDIR)\getnetent.sbr"
	-@erase "$(INTDIR)\getnetent_r.obj"
	-@erase "$(INTDIR)\getnetent_r.sbr"
	-@erase "$(INTDIR)\getnetgrent.obj"
	-@erase "$(INTDIR)\getnetgrent.sbr"
	-@erase "$(INTDIR)\getnetgrent_r.obj"
	-@erase "$(INTDIR)\getnetgrent_r.sbr"
	-@erase "$(INTDIR)\getopt.obj"
	-@erase "$(INTDIR)\getopt.sbr"
	-@erase "$(INTDIR)\getprotoent.obj"
	-@erase "$(INTDIR)\getprotoent.sbr"
	-@erase "$(INTDIR)\getprotoent_r.obj"
	-@erase "$(INTDIR)\getprotoent_r.sbr"
	-@erase "$(INTDIR)\getpwent.obj"
	-@erase "$(INTDIR)\getpwent.sbr"
	-@erase "$(INTDIR)\getpwent_r.obj"
	-@erase "$(INTDIR)\getpwent_r.sbr"
	-@erase "$(INTDIR)\getservent.obj"
	-@erase "$(INTDIR)\getservent.sbr"
	-@erase "$(INTDIR)\getservent_r.obj"
	-@erase "$(INTDIR)\getservent_r.sbr"
	-@erase "$(INTDIR)\gettimeofday.obj"
	-@erase "$(INTDIR)\gettimeofday.sbr"
	-@erase "$(INTDIR)\heap.obj"
	-@erase "$(INTDIR)\heap.sbr"
	-@erase "$(INTDIR)\herror.obj"
	-@erase "$(INTDIR)\herror.sbr"
	-@erase "$(INTDIR)\hesiod.obj"
	-@erase "$(INTDIR)\hesiod.sbr"
	-@erase "$(INTDIR)\hmac_link.obj"
	-@erase "$(INTDIR)\hmac_link.sbr"
	-@erase "$(INTDIR)\inet_addr.obj"
	-@erase "$(INTDIR)\inet_addr.sbr"
	-@erase "$(INTDIR)\inet_cidr_ntop.obj"
	-@erase "$(INTDIR)\inet_cidr_ntop.sbr"
	-@erase "$(INTDIR)\inet_cidr_pton.obj"
	-@erase "$(INTDIR)\inet_cidr_pton.sbr"
	-@erase "$(INTDIR)\inet_lnaof.obj"
	-@erase "$(INTDIR)\inet_lnaof.sbr"
	-@erase "$(INTDIR)\inet_makeaddr.obj"
	-@erase "$(INTDIR)\inet_makeaddr.sbr"
	-@erase "$(INTDIR)\inet_net_ntop.obj"
	-@erase "$(INTDIR)\inet_net_ntop.sbr"
	-@erase "$(INTDIR)\inet_net_pton.obj"
	-@erase "$(INTDIR)\inet_net_pton.sbr"
	-@erase "$(INTDIR)\inet_neta.obj"
	-@erase "$(INTDIR)\inet_neta.sbr"
	-@erase "$(INTDIR)\inet_netof.obj"
	-@erase "$(INTDIR)\inet_netof.sbr"
	-@erase "$(INTDIR)\inet_network.obj"
	-@erase "$(INTDIR)\inet_network.sbr"
	-@erase "$(INTDIR)\inet_ntoa.obj"
	-@erase "$(INTDIR)\inet_ntoa.sbr"
	-@erase "$(INTDIR)\inet_ntop.obj"
	-@erase "$(INTDIR)\inet_ntop.sbr"
	-@erase "$(INTDIR)\inet_pton.obj"
	-@erase "$(INTDIR)\inet_pton.sbr"
	-@erase "$(INTDIR)\intbits.obj"
	-@erase "$(INTDIR)\intbits.sbr"
	-@erase "$(INTDIR)\interface.obj"
	-@erase "$(INTDIR)\interface.sbr"
	-@erase "$(INTDIR)\intitem.obj"
	-@erase "$(INTDIR)\intitem.sbr"
	-@erase "$(INTDIR)\ioctl_if.obj"
	-@erase "$(INTDIR)\ioctl_if.sbr"
	-@erase "$(INTDIR)\irp.obj"
	-@erase "$(INTDIR)\irp.sbr"
	-@erase "$(INTDIR)\irp_gr.obj"
	-@erase "$(INTDIR)\irp_gr.sbr"
	-@erase "$(INTDIR)\irp_ho.obj"
	-@erase "$(INTDIR)\irp_ho.sbr"
	-@erase "$(INTDIR)\irp_ng.obj"
	-@erase "$(INTDIR)\irp_ng.sbr"
	-@erase "$(INTDIR)\irp_nw.obj"
	-@erase "$(INTDIR)\irp_nw.sbr"
	-@erase "$(INTDIR)\irp_pr.obj"
	-@erase "$(INTDIR)\irp_pr.sbr"
	-@erase "$(INTDIR)\irp_pw.obj"
	-@erase "$(INTDIR)\irp_pw.sbr"
	-@erase "$(INTDIR)\irp_sv.obj"
	-@erase "$(INTDIR)\irp_sv.sbr"
	-@erase "$(INTDIR)\irpmarshall.obj"
	-@erase "$(INTDIR)\irpmarshall.sbr"
	-@erase "$(INTDIR)\irs_data.obj"
	-@erase "$(INTDIR)\irs_data.sbr"
	-@erase "$(INTDIR)\keyobj.obj"
	-@erase "$(INTDIR)\keyobj.sbr"
	-@erase "$(INTDIR)\ki8byte.obj"
	-@erase "$(INTDIR)\ki8byte.sbr"
	-@erase "$(INTDIR)\kifulprv.obj"
	-@erase "$(INTDIR)\kifulprv.sbr"
	-@erase "$(INTDIR)\kiitem.obj"
	-@erase "$(INTDIR)\kiitem.sbr"
	-@erase "$(INTDIR)\kinfotyp.obj"
	-@erase "$(INTDIR)\kinfotyp.sbr"
	-@erase "$(INTDIR)\kipkcrpr.obj"
	-@erase "$(INTDIR)\kipkcrpr.sbr"
	-@erase "$(INTDIR)\kirsacrt.obj"
	-@erase "$(INTDIR)\kirsacrt.sbr"
	-@erase "$(INTDIR)\kirsapub.obj"
	-@erase "$(INTDIR)\kirsapub.sbr"
	-@erase "$(INTDIR)\lbn00.obj"
	-@erase "$(INTDIR)\lbn00.sbr"
	-@erase "$(INTDIR)\lbnmem.obj"
	-@erase "$(INTDIR)\lbnmem.sbr"
	-@erase "$(INTDIR)\lcl.obj"
	-@erase "$(INTDIR)\lcl.sbr"
	-@erase "$(INTDIR)\lcl_gr.obj"
	-@erase "$(INTDIR)\lcl_gr.sbr"
	-@erase "$(INTDIR)\lcl_ho.obj"
	-@erase "$(INTDIR)\lcl_ho.sbr"
	-@erase "$(INTDIR)\lcl_ng.obj"
	-@erase "$(INTDIR)\lcl_ng.sbr"
	-@erase "$(INTDIR)\lcl_nw.obj"
	-@erase "$(INTDIR)\lcl_nw.sbr"
	-@erase "$(INTDIR)\lcl_pr.obj"
	-@erase "$(INTDIR)\lcl_pr.sbr"
	-@erase "$(INTDIR)\lcl_pw.obj"
	-@erase "$(INTDIR)\lcl_pw.sbr"
	-@erase "$(INTDIR)\lcl_sv.obj"
	-@erase "$(INTDIR)\lcl_sv.sbr"
	-@erase "$(INTDIR)\legal.obj"
	-@erase "$(INTDIR)\legal.sbr"
	-@erase "$(INTDIR)\logging.obj"
	-@erase "$(INTDIR)\logging.sbr"
	-@erase "$(INTDIR)\math.obj"
	-@erase "$(INTDIR)\math.sbr"
	-@erase "$(INTDIR)\md5.obj"
	-@erase "$(INTDIR)\md5.sbr"
	-@erase "$(INTDIR)\md5_dgst.obj"
	-@erase "$(INTDIR)\md5_dgst.sbr"
	-@erase "$(INTDIR)\md5rand.obj"
	-@erase "$(INTDIR)\md5rand.sbr"
	-@erase "$(INTDIR)\memcluster.obj"
	-@erase "$(INTDIR)\memcluster.sbr"
	-@erase "$(INTDIR)\movefile.obj"
	-@erase "$(INTDIR)\movefile.sbr"
	-@erase "$(INTDIR)\nameserver.obj"
	-@erase "$(INTDIR)\nameserver.sbr"
	-@erase "$(INTDIR)\nis.obj"
	-@erase "$(INTDIR)\nis.sbr"
	-@erase "$(INTDIR)\nis_gr.obj"
	-@erase "$(INTDIR)\nis_gr.sbr"
	-@erase "$(INTDIR)\nis_ho.obj"
	-@erase "$(INTDIR)\nis_ho.sbr"
	-@erase "$(INTDIR)\nis_ng.obj"
	-@erase "$(INTDIR)\nis_ng.sbr"
	-@erase "$(INTDIR)\nis_nw.obj"
	-@erase "$(INTDIR)\nis_nw.sbr"
	-@erase "$(INTDIR)\nis_pr.obj"
	-@erase "$(INTDIR)\nis_pr.sbr"
	-@erase "$(INTDIR)\nis_pw.obj"
	-@erase "$(INTDIR)\nis_pw.sbr"
	-@erase "$(INTDIR)\nis_sv.obj"
	-@erase "$(INTDIR)\nis_sv.sbr"
	-@erase "$(INTDIR)\ns_date.obj"
	-@erase "$(INTDIR)\ns_date.sbr"
	-@erase "$(INTDIR)\ns_name.obj"
	-@erase "$(INTDIR)\ns_name.sbr"
	-@erase "$(INTDIR)\ns_netint.obj"
	-@erase "$(INTDIR)\ns_netint.sbr"
	-@erase "$(INTDIR)\ns_parse.obj"
	-@erase "$(INTDIR)\ns_parse.sbr"
	-@erase "$(INTDIR)\ns_print.obj"
	-@erase "$(INTDIR)\ns_print.sbr"
	-@erase "$(INTDIR)\ns_samedomain.obj"
	-@erase "$(INTDIR)\ns_samedomain.sbr"
	-@erase "$(INTDIR)\ns_sign.obj"
	-@erase "$(INTDIR)\ns_sign.sbr"
	-@erase "$(INTDIR)\ns_ttl.obj"
	-@erase "$(INTDIR)\ns_ttl.sbr"
	-@erase "$(INTDIR)\ns_verify.obj"
	-@erase "$(INTDIR)\ns_verify.sbr"
	-@erase "$(INTDIR)\nsap_addr.obj"
	-@erase "$(INTDIR)\nsap_addr.sbr"
	-@erase "$(INTDIR)\nul_ng.obj"
	-@erase "$(INTDIR)\nul_ng.sbr"
	-@erase "$(INTDIR)\prandom.obj"
	-@erase "$(INTDIR)\prandom.sbr"
	-@erase "$(INTDIR)\prime.obj"
	-@erase "$(INTDIR)\prime.sbr"
	-@erase "$(INTDIR)\putenv.obj"
	-@erase "$(INTDIR)\putenv.sbr"
	-@erase "$(INTDIR)\rand.obj"
	-@erase "$(INTDIR)\rand.sbr"
	-@erase "$(INTDIR)\random.obj"
	-@erase "$(INTDIR)\random.sbr"
	-@erase "$(INTDIR)\readv.obj"
	-@erase "$(INTDIR)\readv.sbr"
	-@erase "$(INTDIR)\res_comp.obj"
	-@erase "$(INTDIR)\res_comp.sbr"
	-@erase "$(INTDIR)\res_data.obj"
	-@erase "$(INTDIR)\res_data.sbr"
	-@erase "$(INTDIR)\res_debug.obj"
	-@erase "$(INTDIR)\res_debug.sbr"
	-@erase "$(INTDIR)\res_findzonecut.obj"
	-@erase "$(INTDIR)\res_findzonecut.sbr"
	-@erase "$(INTDIR)\res_init.obj"
	-@erase "$(INTDIR)\res_init.sbr"
	-@erase "$(INTDIR)\res_mkquery.obj"
	-@erase "$(INTDIR)\res_mkquery.sbr"
	-@erase "$(INTDIR)\res_mkupdate.obj"
	-@erase "$(INTDIR)\res_mkupdate.sbr"
	-@erase "$(INTDIR)\res_query.obj"
	-@erase "$(INTDIR)\res_query.sbr"
	-@erase "$(INTDIR)\res_send.obj"
	-@erase "$(INTDIR)\res_send.sbr"
	-@erase "$(INTDIR)\res_sendsigned.obj"
	-@erase "$(INTDIR)\res_sendsigned.sbr"
	-@erase "$(INTDIR)\res_update.obj"
	-@erase "$(INTDIR)\res_update.sbr"
	-@erase "$(INTDIR)\rsa.obj"
	-@erase "$(INTDIR)\rsa.sbr"
	-@erase "$(INTDIR)\rsakeygn.obj"
	-@erase "$(INTDIR)\rsakeygn.sbr"
	-@erase "$(INTDIR)\rsaref_link.obj"
	-@erase "$(INTDIR)\rsaref_link.sbr"
	-@erase "$(INTDIR)\seccbcd.obj"
	-@erase "$(INTDIR)\seccbcd.sbr"
	-@erase "$(INTDIR)\seccbce.obj"
	-@erase "$(INTDIR)\seccbce.sbr"
	-@erase "$(INTDIR)\setenv.obj"
	-@erase "$(INTDIR)\setenv.sbr"
	-@erase "$(INTDIR)\setitimer.obj"
	-@erase "$(INTDIR)\setitimer.sbr"
	-@erase "$(INTDIR)\sha.obj"
	-@erase "$(INTDIR)\sha.sbr"
	-@erase "$(INTDIR)\signal.obj"
	-@erase "$(INTDIR)\signal.sbr"
	-@erase "$(INTDIR)\socket.obj"
	-@erase "$(INTDIR)\socket.sbr"
	-@erase "$(INTDIR)\strcasecmp.obj"
	-@erase "$(INTDIR)\strcasecmp.sbr"
	-@erase "$(INTDIR)\strdup.obj"
	-@erase "$(INTDIR)\strdup.sbr"
	-@erase "$(INTDIR)\strpbrk.obj"
	-@erase "$(INTDIR)\strpbrk.sbr"
	-@erase "$(INTDIR)\strsep.obj"
	-@erase "$(INTDIR)\strsep.sbr"
	-@erase "$(INTDIR)\strtoul.obj"
	-@erase "$(INTDIR)\strtoul.sbr"
	-@erase "$(INTDIR)\support.obj"
	-@erase "$(INTDIR)\support.sbr"
	-@erase "$(INTDIR)\surrendr.obj"
	-@erase "$(INTDIR)\surrendr.sbr"
	-@erase "$(INTDIR)\swap.obj"
	-@erase "$(INTDIR)\swap.sbr"
	-@erase "$(INTDIR)\syslog.obj"
	-@erase "$(INTDIR)\syslog.sbr"
	-@erase "$(INTDIR)\tree.obj"
	-@erase "$(INTDIR)\tree.sbr"
	-@erase "$(INTDIR)\unistd.obj"
	-@erase "$(INTDIR)\unistd.sbr"
	-@erase "$(INTDIR)\util.obj"
	-@erase "$(INTDIR)\util.sbr"
	-@erase "$(INTDIR)\utimes.obj"
	-@erase "$(INTDIR)\utimes.sbr"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(INTDIR)\writev.obj"
	-@erase "$(INTDIR)\writev.sbr"
	-@erase "$(OUTDIR)\libbind.bsc"
	-@erase "$(OUTDIR)\libbind.exp"
	-@erase "$(OUTDIR)\libbind.lib"
	-@erase "..\bin\Debug\libbind.dll"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MDd /W3 /Gm /GX /Zi /Od /I "..\include" /I "..\..\..\include" /I "..\..\..\lib\dnssafe" /D "_DEBUG" /D "HAVE_CONFIG_H" /D "WIN32" /D "_WINDOWS" /D "LIB" /D "DNSSAFE" /D "i386" /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\libbind.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\gettimeofday.sbr" \
	"$(INTDIR)\putenv.sbr" \
	"$(INTDIR)\readv.sbr" \
	"$(INTDIR)\setenv.sbr" \
	"$(INTDIR)\setitimer.sbr" \
	"$(INTDIR)\strcasecmp.sbr" \
	"$(INTDIR)\strdup.sbr" \
	"$(INTDIR)\strpbrk.sbr" \
	"$(INTDIR)\strsep.sbr" \
	"$(INTDIR)\strtoul.sbr" \
	"$(INTDIR)\utimes.sbr" \
	"$(INTDIR)\writev.sbr" \
	"$(INTDIR)\assertions.sbr" \
	"$(INTDIR)\base64.sbr" \
	"$(INTDIR)\bitncmp.sbr" \
	"$(INTDIR)\ctl_clnt.sbr" \
	"$(INTDIR)\ctl_p.sbr" \
	"$(INTDIR)\ctl_srvr.sbr" \
	"$(INTDIR)\ev_connects.sbr" \
	"$(INTDIR)\ev_files.sbr" \
	"$(INTDIR)\ev_streams.sbr" \
	"$(INTDIR)\ev_timers.sbr" \
	"$(INTDIR)\ev_waits.sbr" \
	"$(INTDIR)\eventlib.sbr" \
	"$(INTDIR)\heap.sbr" \
	"$(INTDIR)\logging.sbr" \
	"$(INTDIR)\memcluster.sbr" \
	"$(INTDIR)\tree.sbr" \
	"$(INTDIR)\dns.sbr" \
	"$(INTDIR)\dns_gr.sbr" \
	"$(INTDIR)\dns_ho.sbr" \
	"$(INTDIR)\dns_nw.sbr" \
	"$(INTDIR)\dns_pr.sbr" \
	"$(INTDIR)\dns_pw.sbr" \
	"$(INTDIR)\dns_sv.sbr" \
	"$(INTDIR)\gai_strerror.sbr" \
	"$(INTDIR)\gen.sbr" \
	"$(INTDIR)\gen_gr.sbr" \
	"$(INTDIR)\gen_ho.sbr" \
	"$(INTDIR)\gen_ng.sbr" \
	"$(INTDIR)\gen_nw.sbr" \
	"$(INTDIR)\gen_pr.sbr" \
	"$(INTDIR)\gen_pw.sbr" \
	"$(INTDIR)\gen_sv.sbr" \
	"$(INTDIR)\getaddrinfo.sbr" \
	"$(INTDIR)\getgrent.sbr" \
	"$(INTDIR)\getgrent_r.sbr" \
	"$(INTDIR)\gethostent.sbr" \
	"$(INTDIR)\gethostent_r.sbr" \
	"$(INTDIR)\getnameinfo.sbr" \
	"$(INTDIR)\getnetent.sbr" \
	"$(INTDIR)\getnetent_r.sbr" \
	"$(INTDIR)\getnetgrent.sbr" \
	"$(INTDIR)\getnetgrent_r.sbr" \
	"$(INTDIR)\getprotoent.sbr" \
	"$(INTDIR)\getprotoent_r.sbr" \
	"$(INTDIR)\getpwent.sbr" \
	"$(INTDIR)\getpwent_r.sbr" \
	"$(INTDIR)\getservent.sbr" \
	"$(INTDIR)\getservent_r.sbr" \
	"$(INTDIR)\hesiod.sbr" \
	"$(INTDIR)\irp.sbr" \
	"$(INTDIR)\irp_gr.sbr" \
	"$(INTDIR)\irp_ho.sbr" \
	"$(INTDIR)\irp_ng.sbr" \
	"$(INTDIR)\irp_nw.sbr" \
	"$(INTDIR)\irp_pr.sbr" \
	"$(INTDIR)\irp_pw.sbr" \
	"$(INTDIR)\irp_sv.sbr" \
	"$(INTDIR)\irpmarshall.sbr" \
	"$(INTDIR)\irs_data.sbr" \
	"$(INTDIR)\lcl.sbr" \
	"$(INTDIR)\lcl_gr.sbr" \
	"$(INTDIR)\lcl_ho.sbr" \
	"$(INTDIR)\lcl_ng.sbr" \
	"$(INTDIR)\lcl_nw.sbr" \
	"$(INTDIR)\lcl_pr.sbr" \
	"$(INTDIR)\lcl_pw.sbr" \
	"$(INTDIR)\lcl_sv.sbr" \
	"$(INTDIR)\nis.sbr" \
	"$(INTDIR)\nis_gr.sbr" \
	"$(INTDIR)\nis_ho.sbr" \
	"$(INTDIR)\nis_ng.sbr" \
	"$(INTDIR)\nis_nw.sbr" \
	"$(INTDIR)\nis_pr.sbr" \
	"$(INTDIR)\nis_pw.sbr" \
	"$(INTDIR)\nis_sv.sbr" \
	"$(INTDIR)\nul_ng.sbr" \
	"$(INTDIR)\util.sbr" \
	"$(INTDIR)\herror.sbr" \
	"$(INTDIR)\res_comp.sbr" \
	"$(INTDIR)\res_data.sbr" \
	"$(INTDIR)\res_debug.sbr" \
	"$(INTDIR)\res_findzonecut.sbr" \
	"$(INTDIR)\res_init.sbr" \
	"$(INTDIR)\res_mkquery.sbr" \
	"$(INTDIR)\res_mkupdate.sbr" \
	"$(INTDIR)\res_query.sbr" \
	"$(INTDIR)\res_send.sbr" \
	"$(INTDIR)\res_sendsigned.sbr" \
	"$(INTDIR)\res_update.sbr" \
	"$(INTDIR)\ns_date.sbr" \
	"$(INTDIR)\ns_name.sbr" \
	"$(INTDIR)\ns_netint.sbr" \
	"$(INTDIR)\ns_parse.sbr" \
	"$(INTDIR)\ns_print.sbr" \
	"$(INTDIR)\ns_samedomain.sbr" \
	"$(INTDIR)\ns_sign.sbr" \
	"$(INTDIR)\ns_ttl.sbr" \
	"$(INTDIR)\ns_verify.sbr" \
	"$(INTDIR)\inet_addr.sbr" \
	"$(INTDIR)\inet_cidr_ntop.sbr" \
	"$(INTDIR)\inet_cidr_pton.sbr" \
	"$(INTDIR)\inet_lnaof.sbr" \
	"$(INTDIR)\inet_makeaddr.sbr" \
	"$(INTDIR)\inet_net_ntop.sbr" \
	"$(INTDIR)\inet_net_pton.sbr" \
	"$(INTDIR)\inet_neta.sbr" \
	"$(INTDIR)\inet_netof.sbr" \
	"$(INTDIR)\inet_network.sbr" \
	"$(INTDIR)\inet_ntoa.sbr" \
	"$(INTDIR)\inet_ntop.sbr" \
	"$(INTDIR)\inet_pton.sbr" \
	"$(INTDIR)\nsap_addr.sbr" \
	"$(INTDIR)\bits.sbr" \
	"$(INTDIR)\bn.sbr" \
	"$(INTDIR)\bn00.sbr" \
	"$(INTDIR)\ctk_prime.sbr" \
	"$(INTDIR)\dss.sbr" \
	"$(INTDIR)\lbn00.sbr" \
	"$(INTDIR)\lbnmem.sbr" \
	"$(INTDIR)\legal.sbr" \
	"$(INTDIR)\math.sbr" \
	"$(INTDIR)\rand.sbr" \
	"$(INTDIR)\sha.sbr" \
	"$(INTDIR)\swap.sbr" \
	"$(INTDIR)\bsafe_link.sbr" \
	"$(INTDIR)\cylink_link.sbr" \
	"$(INTDIR)\dst_api.sbr" \
	"$(INTDIR)\eay_dss_link.sbr" \
	"$(INTDIR)\hmac_link.sbr" \
	"$(INTDIR)\md5_dgst.sbr" \
	"$(INTDIR)\prandom.sbr" \
	"$(INTDIR)\rsaref_link.sbr" \
	"$(INTDIR)\support.sbr" \
	"$(INTDIR)\dirent.sbr" \
	"$(INTDIR)\DLLMain.sbr" \
	"$(INTDIR)\files.sbr" \
	"$(INTDIR)\getopt.sbr" \
	"$(INTDIR)\interface.sbr" \
	"$(INTDIR)\ioctl_if.sbr" \
	"$(INTDIR)\movefile.sbr" \
	"$(INTDIR)\nameserver.sbr" \
	"$(INTDIR)\signal.sbr" \
	"$(INTDIR)\socket.sbr" \
	"$(INTDIR)\syslog.sbr" \
	"$(INTDIR)\unistd.sbr" \
	"$(INTDIR)\ahcbcpad.sbr" \
	"$(INTDIR)\ahchdig.sbr" \
	"$(INTDIR)\ahchencr.sbr" \
	"$(INTDIR)\ahchgen.sbr" \
	"$(INTDIR)\ahchrand.sbr" \
	"$(INTDIR)\ahdigest.sbr" \
	"$(INTDIR)\ahencryp.sbr" \
	"$(INTDIR)\ahgen.sbr" \
	"$(INTDIR)\ahrandom.sbr" \
	"$(INTDIR)\ahrsaenc.sbr" \
	"$(INTDIR)\ahrsaepr.sbr" \
	"$(INTDIR)\ahrsaepu.sbr" \
	"$(INTDIR)\aichdig.sbr" \
	"$(INTDIR)\aichenc8.sbr" \
	"$(INTDIR)\aichencn.sbr" \
	"$(INTDIR)\aichencr.sbr" \
	"$(INTDIR)\aichgen.sbr" \
	"$(INTDIR)\aichrand.sbr" \
	"$(INTDIR)\aimd5.sbr" \
	"$(INTDIR)\aimd5ran.sbr" \
	"$(INTDIR)\ainfotyp.sbr" \
	"$(INTDIR)\ainull.sbr" \
	"$(INTDIR)\airsaepr.sbr" \
	"$(INTDIR)\airsaepu.sbr" \
	"$(INTDIR)\airsakgn.sbr" \
	"$(INTDIR)\airsaprv.sbr" \
	"$(INTDIR)\airsapub.sbr" \
	"$(INTDIR)\algchoic.sbr" \
	"$(INTDIR)\algobj.sbr" \
	"$(INTDIR)\amcrte.sbr" \
	"$(INTDIR)\ammd5.sbr" \
	"$(INTDIR)\ammd5r.sbr" \
	"$(INTDIR)\amrkg.sbr" \
	"$(INTDIR)\amrsae.sbr" \
	"$(INTDIR)\balg.sbr" \
	"$(INTDIR)\bgclrbit.sbr" \
	"$(INTDIR)\bgmdmpyx.sbr" \
	"$(INTDIR)\bgmdsqx.sbr" \
	"$(INTDIR)\bgmodexp.sbr" \
	"$(INTDIR)\bgpegcd.sbr" \
	"$(INTDIR)\big2exp.sbr" \
	"$(INTDIR)\bigabs.sbr" \
	"$(INTDIR)\bigacc.sbr" \
	"$(INTDIR)\bigarith.sbr" \
	"$(INTDIR)\bigcmp.sbr" \
	"$(INTDIR)\bigconst.sbr" \
	"$(INTDIR)\biginv.sbr" \
	"$(INTDIR)\biglen.sbr" \
	"$(INTDIR)\bigmodx.sbr" \
	"$(INTDIR)\bigmpy.sbr" \
	"$(INTDIR)\bigpdiv.sbr" \
	"$(INTDIR)\bigpmpy.sbr" \
	"$(INTDIR)\bigpmpyh.sbr" \
	"$(INTDIR)\bigpmpyl.sbr" \
	"$(INTDIR)\bigpsq.sbr" \
	"$(INTDIR)\bigqrx.sbr" \
	"$(INTDIR)\bigsmod.sbr" \
	"$(INTDIR)\bigtocan.sbr" \
	"$(INTDIR)\bigu.sbr" \
	"$(INTDIR)\bigunexp.sbr" \
	"$(INTDIR)\binfocsh.sbr" \
	"$(INTDIR)\bkey.sbr" \
	"$(INTDIR)\bmempool.sbr" \
	"$(INTDIR)\cantobig.sbr" \
	"$(INTDIR)\crt2.sbr" \
	"$(INTDIR)\digest.sbr" \
	"$(INTDIR)\digrand.sbr" \
	"$(INTDIR)\encrypt.sbr" \
	"$(INTDIR)\generate.sbr" \
	"$(INTDIR)\intbits.sbr" \
	"$(INTDIR)\intitem.sbr" \
	"$(INTDIR)\keyobj.sbr" \
	"$(INTDIR)\ki8byte.sbr" \
	"$(INTDIR)\kifulprv.sbr" \
	"$(INTDIR)\kiitem.sbr" \
	"$(INTDIR)\kinfotyp.sbr" \
	"$(INTDIR)\kipkcrpr.sbr" \
	"$(INTDIR)\kirsacrt.sbr" \
	"$(INTDIR)\kirsapub.sbr" \
	"$(INTDIR)\md5.sbr" \
	"$(INTDIR)\md5rand.sbr" \
	"$(INTDIR)\prime.sbr" \
	"$(INTDIR)\random.sbr" \
	"$(INTDIR)\rsa.sbr" \
	"$(INTDIR)\rsakeygn.sbr" \
	"$(INTDIR)\seccbcd.sbr" \
	"$(INTDIR)\seccbce.sbr" \
	"$(INTDIR)\surrendr.sbr"

"$(OUTDIR)\libbind.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
LINK32_FLAGS=user32.lib advapi32.lib netapi32.lib /nologo /subsystem:windows /dll /pdb:none /debug /machine:I386 /def:".\libbind.def" /out:"..\bin\Debug\libbind.dll" /implib:"$(OUTDIR)\libbind.lib" 
DEF_FILE= \
	".\libbind.def"
LINK32_OBJS= \
	"$(INTDIR)\gettimeofday.obj" \
	"$(INTDIR)\putenv.obj" \
	"$(INTDIR)\readv.obj" \
	"$(INTDIR)\setenv.obj" \
	"$(INTDIR)\setitimer.obj" \
	"$(INTDIR)\strcasecmp.obj" \
	"$(INTDIR)\strdup.obj" \
	"$(INTDIR)\strpbrk.obj" \
	"$(INTDIR)\strsep.obj" \
	"$(INTDIR)\strtoul.obj" \
	"$(INTDIR)\utimes.obj" \
	"$(INTDIR)\writev.obj" \
	"$(INTDIR)\assertions.obj" \
	"$(INTDIR)\base64.obj" \
	"$(INTDIR)\bitncmp.obj" \
	"$(INTDIR)\ctl_clnt.obj" \
	"$(INTDIR)\ctl_p.obj" \
	"$(INTDIR)\ctl_srvr.obj" \
	"$(INTDIR)\ev_connects.obj" \
	"$(INTDIR)\ev_files.obj" \
	"$(INTDIR)\ev_streams.obj" \
	"$(INTDIR)\ev_timers.obj" \
	"$(INTDIR)\ev_waits.obj" \
	"$(INTDIR)\eventlib.obj" \
	"$(INTDIR)\heap.obj" \
	"$(INTDIR)\logging.obj" \
	"$(INTDIR)\memcluster.obj" \
	"$(INTDIR)\tree.obj" \
	"$(INTDIR)\dns.obj" \
	"$(INTDIR)\dns_gr.obj" \
	"$(INTDIR)\dns_ho.obj" \
	"$(INTDIR)\dns_nw.obj" \
	"$(INTDIR)\dns_pr.obj" \
	"$(INTDIR)\dns_pw.obj" \
	"$(INTDIR)\dns_sv.obj" \
	"$(INTDIR)\gai_strerror.obj" \
	"$(INTDIR)\gen.obj" \
	"$(INTDIR)\gen_gr.obj" \
	"$(INTDIR)\gen_ho.obj" \
	"$(INTDIR)\gen_ng.obj" \
	"$(INTDIR)\gen_nw.obj" \
	"$(INTDIR)\gen_pr.obj" \
	"$(INTDIR)\gen_pw.obj" \
	"$(INTDIR)\gen_sv.obj" \
	"$(INTDIR)\getaddrinfo.obj" \
	"$(INTDIR)\getgrent.obj" \
	"$(INTDIR)\getgrent_r.obj" \
	"$(INTDIR)\gethostent.obj" \
	"$(INTDIR)\gethostent_r.obj" \
	"$(INTDIR)\getnameinfo.obj" \
	"$(INTDIR)\getnetent.obj" \
	"$(INTDIR)\getnetent_r.obj" \
	"$(INTDIR)\getnetgrent.obj" \
	"$(INTDIR)\getnetgrent_r.obj" \
	"$(INTDIR)\getprotoent.obj" \
	"$(INTDIR)\getprotoent_r.obj" \
	"$(INTDIR)\getpwent.obj" \
	"$(INTDIR)\getpwent_r.obj" \
	"$(INTDIR)\getservent.obj" \
	"$(INTDIR)\getservent_r.obj" \
	"$(INTDIR)\hesiod.obj" \
	"$(INTDIR)\irp.obj" \
	"$(INTDIR)\irp_gr.obj" \
	"$(INTDIR)\irp_ho.obj" \
	"$(INTDIR)\irp_ng.obj" \
	"$(INTDIR)\irp_nw.obj" \
	"$(INTDIR)\irp_pr.obj" \
	"$(INTDIR)\irp_pw.obj" \
	"$(INTDIR)\irp_sv.obj" \
	"$(INTDIR)\irpmarshall.obj" \
	"$(INTDIR)\irs_data.obj" \
	"$(INTDIR)\lcl.obj" \
	"$(INTDIR)\lcl_gr.obj" \
	"$(INTDIR)\lcl_ho.obj" \
	"$(INTDIR)\lcl_ng.obj" \
	"$(INTDIR)\lcl_nw.obj" \
	"$(INTDIR)\lcl_pr.obj" \
	"$(INTDIR)\lcl_pw.obj" \
	"$(INTDIR)\lcl_sv.obj" \
	"$(INTDIR)\nis.obj" \
	"$(INTDIR)\nis_gr.obj" \
	"$(INTDIR)\nis_ho.obj" \
	"$(INTDIR)\nis_ng.obj" \
	"$(INTDIR)\nis_nw.obj" \
	"$(INTDIR)\nis_pr.obj" \
	"$(INTDIR)\nis_pw.obj" \
	"$(INTDIR)\nis_sv.obj" \
	"$(INTDIR)\nul_ng.obj" \
	"$(INTDIR)\util.obj" \
	"$(INTDIR)\herror.obj" \
	"$(INTDIR)\res_comp.obj" \
	"$(INTDIR)\res_data.obj" \
	"$(INTDIR)\res_debug.obj" \
	"$(INTDIR)\res_findzonecut.obj" \
	"$(INTDIR)\res_init.obj" \
	"$(INTDIR)\res_mkquery.obj" \
	"$(INTDIR)\res_mkupdate.obj" \
	"$(INTDIR)\res_query.obj" \
	"$(INTDIR)\res_send.obj" \
	"$(INTDIR)\res_sendsigned.obj" \
	"$(INTDIR)\res_update.obj" \
	"$(INTDIR)\ns_date.obj" \
	"$(INTDIR)\ns_name.obj" \
	"$(INTDIR)\ns_netint.obj" \
	"$(INTDIR)\ns_parse.obj" \
	"$(INTDIR)\ns_print.obj" \
	"$(INTDIR)\ns_samedomain.obj" \
	"$(INTDIR)\ns_sign.obj" \
	"$(INTDIR)\ns_ttl.obj" \
	"$(INTDIR)\ns_verify.obj" \
	"$(INTDIR)\inet_addr.obj" \
	"$(INTDIR)\inet_cidr_ntop.obj" \
	"$(INTDIR)\inet_cidr_pton.obj" \
	"$(INTDIR)\inet_lnaof.obj" \
	"$(INTDIR)\inet_makeaddr.obj" \
	"$(INTDIR)\inet_net_ntop.obj" \
	"$(INTDIR)\inet_net_pton.obj" \
	"$(INTDIR)\inet_neta.obj" \
	"$(INTDIR)\inet_netof.obj" \
	"$(INTDIR)\inet_network.obj" \
	"$(INTDIR)\inet_ntoa.obj" \
	"$(INTDIR)\inet_ntop.obj" \
	"$(INTDIR)\inet_pton.obj" \
	"$(INTDIR)\nsap_addr.obj" \
	"$(INTDIR)\bits.obj" \
	"$(INTDIR)\bn.obj" \
	"$(INTDIR)\bn00.obj" \
	"$(INTDIR)\ctk_prime.obj" \
	"$(INTDIR)\dss.obj" \
	"$(INTDIR)\lbn00.obj" \
	"$(INTDIR)\lbnmem.obj" \
	"$(INTDIR)\legal.obj" \
	"$(INTDIR)\math.obj" \
	"$(INTDIR)\rand.obj" \
	"$(INTDIR)\sha.obj" \
	"$(INTDIR)\swap.obj" \
	"$(INTDIR)\bsafe_link.obj" \
	"$(INTDIR)\cylink_link.obj" \
	"$(INTDIR)\dst_api.obj" \
	"$(INTDIR)\eay_dss_link.obj" \
	"$(INTDIR)\hmac_link.obj" \
	"$(INTDIR)\md5_dgst.obj" \
	"$(INTDIR)\prandom.obj" \
	"$(INTDIR)\rsaref_link.obj" \
	"$(INTDIR)\support.obj" \
	"$(INTDIR)\dirent.obj" \
	"$(INTDIR)\DLLMain.obj" \
	"$(INTDIR)\files.obj" \
	"$(INTDIR)\getopt.obj" \
	"$(INTDIR)\interface.obj" \
	"$(INTDIR)\ioctl_if.obj" \
	"$(INTDIR)\movefile.obj" \
	"$(INTDIR)\nameserver.obj" \
	"$(INTDIR)\signal.obj" \
	"$(INTDIR)\socket.obj" \
	"$(INTDIR)\syslog.obj" \
	"$(INTDIR)\unistd.obj" \
	"$(INTDIR)\ahcbcpad.obj" \
	"$(INTDIR)\ahchdig.obj" \
	"$(INTDIR)\ahchencr.obj" \
	"$(INTDIR)\ahchgen.obj" \
	"$(INTDIR)\ahchrand.obj" \
	"$(INTDIR)\ahdigest.obj" \
	"$(INTDIR)\ahencryp.obj" \
	"$(INTDIR)\ahgen.obj" \
	"$(INTDIR)\ahrandom.obj" \
	"$(INTDIR)\ahrsaenc.obj" \
	"$(INTDIR)\ahrsaepr.obj" \
	"$(INTDIR)\ahrsaepu.obj" \
	"$(INTDIR)\aichdig.obj" \
	"$(INTDIR)\aichenc8.obj" \
	"$(INTDIR)\aichencn.obj" \
	"$(INTDIR)\aichencr.obj" \
	"$(INTDIR)\aichgen.obj" \
	"$(INTDIR)\aichrand.obj" \
	"$(INTDIR)\aimd5.obj" \
	"$(INTDIR)\aimd5ran.obj" \
	"$(INTDIR)\ainfotyp.obj" \
	"$(INTDIR)\ainull.obj" \
	"$(INTDIR)\airsaepr.obj" \
	"$(INTDIR)\airsaepu.obj" \
	"$(INTDIR)\airsakgn.obj" \
	"$(INTDIR)\airsaprv.obj" \
	"$(INTDIR)\airsapub.obj" \
	"$(INTDIR)\algchoic.obj" \
	"$(INTDIR)\algobj.obj" \
	"$(INTDIR)\amcrte.obj" \
	"$(INTDIR)\ammd5.obj" \
	"$(INTDIR)\ammd5r.obj" \
	"$(INTDIR)\amrkg.obj" \
	"$(INTDIR)\amrsae.obj" \
	"$(INTDIR)\balg.obj" \
	"$(INTDIR)\bgclrbit.obj" \
	"$(INTDIR)\bgmdmpyx.obj" \
	"$(INTDIR)\bgmdsqx.obj" \
	"$(INTDIR)\bgmodexp.obj" \
	"$(INTDIR)\bgpegcd.obj" \
	"$(INTDIR)\big2exp.obj" \
	"$(INTDIR)\bigabs.obj" \
	"$(INTDIR)\bigacc.obj" \
	"$(INTDIR)\bigarith.obj" \
	"$(INTDIR)\bigcmp.obj" \
	"$(INTDIR)\bigconst.obj" \
	"$(INTDIR)\biginv.obj" \
	"$(INTDIR)\biglen.obj" \
	"$(INTDIR)\bigmodx.obj" \
	"$(INTDIR)\bigmpy.obj" \
	"$(INTDIR)\bigpdiv.obj" \
	"$(INTDIR)\bigpmpy.obj" \
	"$(INTDIR)\bigpmpyh.obj" \
	"$(INTDIR)\bigpmpyl.obj" \
	"$(INTDIR)\bigpsq.obj" \
	"$(INTDIR)\bigqrx.obj" \
	"$(INTDIR)\bigsmod.obj" \
	"$(INTDIR)\bigtocan.obj" \
	"$(INTDIR)\bigu.obj" \
	"$(INTDIR)\bigunexp.obj" \
	"$(INTDIR)\binfocsh.obj" \
	"$(INTDIR)\bkey.obj" \
	"$(INTDIR)\bmempool.obj" \
	"$(INTDIR)\cantobig.obj" \
	"$(INTDIR)\crt2.obj" \
	"$(INTDIR)\digest.obj" \
	"$(INTDIR)\digrand.obj" \
	"$(INTDIR)\encrypt.obj" \
	"$(INTDIR)\generate.obj" \
	"$(INTDIR)\intbits.obj" \
	"$(INTDIR)\intitem.obj" \
	"$(INTDIR)\keyobj.obj" \
	"$(INTDIR)\ki8byte.obj" \
	"$(INTDIR)\kifulprv.obj" \
	"$(INTDIR)\kiitem.obj" \
	"$(INTDIR)\kinfotyp.obj" \
	"$(INTDIR)\kipkcrpr.obj" \
	"$(INTDIR)\kirsacrt.obj" \
	"$(INTDIR)\kirsapub.obj" \
	"$(INTDIR)\md5.obj" \
	"$(INTDIR)\md5rand.obj" \
	"$(INTDIR)\prime.obj" \
	"$(INTDIR)\random.obj" \
	"$(INTDIR)\rsa.obj" \
	"$(INTDIR)\rsakeygn.obj" \
	"$(INTDIR)\seccbcd.obj" \
	"$(INTDIR)\seccbce.obj" \
	"$(INTDIR)\surrendr.obj"

"..\bin\Debug\libbind.dll" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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
!IF EXISTS("libbind.dep")
!INCLUDE "libbind.dep"
!ELSE 
!MESSAGE Warning: cannot find "libbind.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "libbind - Win32 Release" || "$(CFG)" == "libbind - Win32 Debug"
SOURCE=..\..\..\lib\bsd\gettimeofday.c

"$(INTDIR)\gettimeofday.obj"	"$(INTDIR)\gettimeofday.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\lib\bsd\putenv.c

"$(INTDIR)\putenv.obj"	"$(INTDIR)\putenv.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\lib\bsd\readv.c

"$(INTDIR)\readv.obj"	"$(INTDIR)\readv.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\lib\bsd\setenv.c

"$(INTDIR)\setenv.obj"	"$(INTDIR)\setenv.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\lib\bsd\setitimer.c

"$(INTDIR)\setitimer.obj"	"$(INTDIR)\setitimer.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\lib\bsd\strcasecmp.c

"$(INTDIR)\strcasecmp.obj"	"$(INTDIR)\strcasecmp.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\lib\bsd\strdup.c

"$(INTDIR)\strdup.obj"	"$(INTDIR)\strdup.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\lib\bsd\strpbrk.c

"$(INTDIR)\strpbrk.obj"	"$(INTDIR)\strpbrk.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\lib\bsd\strsep.c

"$(INTDIR)\strsep.obj"	"$(INTDIR)\strsep.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\lib\bsd\strtoul.c

"$(INTDIR)\strtoul.obj"	"$(INTDIR)\strtoul.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\lib\bsd\utimes.c

"$(INTDIR)\utimes.obj"	"$(INTDIR)\utimes.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\lib\bsd\writev.c

"$(INTDIR)\writev.obj"	"$(INTDIR)\writev.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\lib\isc\assertions.c

"$(INTDIR)\assertions.obj"	"$(INTDIR)\assertions.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\lib\isc\base64.c

"$(INTDIR)\base64.obj"	"$(INTDIR)\base64.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\lib\isc\bitncmp.c

"$(INTDIR)\bitncmp.obj"	"$(INTDIR)\bitncmp.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\lib\isc\ctl_clnt.c

"$(INTDIR)\ctl_clnt.obj"	"$(INTDIR)\ctl_clnt.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\lib\isc\ctl_p.c

"$(INTDIR)\ctl_p.obj"	"$(INTDIR)\ctl_p.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\lib\isc\ctl_srvr.c

"$(INTDIR)\ctl_srvr.obj"	"$(INTDIR)\ctl_srvr.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\lib\isc\ev_connects.c

"$(INTDIR)\ev_connects.obj"	"$(INTDIR)\ev_connects.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\lib\isc\ev_files.c

"$(INTDIR)\ev_files.obj"	"$(INTDIR)\ev_files.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\lib\isc\ev_streams.c

"$(INTDIR)\ev_streams.obj"	"$(INTDIR)\ev_streams.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\lib\isc\ev_timers.c

"$(INTDIR)\ev_timers.obj"	"$(INTDIR)\ev_timers.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\lib\isc\ev_waits.c

"$(INTDIR)\ev_waits.obj"	"$(INTDIR)\ev_waits.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\lib\isc\eventlib.c

"$(INTDIR)\eventlib.obj"	"$(INTDIR)\eventlib.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\lib\isc\heap.c

"$(INTDIR)\heap.obj"	"$(INTDIR)\heap.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\lib\isc\logging.c

"$(INTDIR)\logging.obj"	"$(INTDIR)\logging.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\lib\isc\memcluster.c

"$(INTDIR)\memcluster.obj"	"$(INTDIR)\memcluster.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\lib\isc\tree.c

"$(INTDIR)\tree.obj"	"$(INTDIR)\tree.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\lib\irs\dns.c

"$(INTDIR)\dns.obj"	"$(INTDIR)\dns.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\lib\irs\dns_gr.c

"$(INTDIR)\dns_gr.obj"	"$(INTDIR)\dns_gr.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\lib\irs\dns_ho.c

"$(INTDIR)\dns_ho.obj"	"$(INTDIR)\dns_ho.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\lib\irs\dns_nw.c

"$(INTDIR)\dns_nw.obj"	"$(INTDIR)\dns_nw.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\lib\irs\dns_pr.c

"$(INTDIR)\dns_pr.obj"	"$(INTDIR)\dns_pr.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\lib\irs\dns_pw.c

"$(INTDIR)\dns_pw.obj"	"$(INTDIR)\dns_pw.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\lib\irs\dns_sv.c

"$(INTDIR)\dns_sv.obj"	"$(INTDIR)\dns_sv.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\lib\irs\gai_strerror.c

"$(INTDIR)\gai_strerror.obj"	"$(INTDIR)\gai_strerror.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\lib\irs\gen.c

"$(INTDIR)\gen.obj"	"$(INTDIR)\gen.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\lib\irs\gen_gr.c

"$(INTDIR)\gen_gr.obj"	"$(INTDIR)\gen_gr.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\lib\irs\gen_ho.c

"$(INTDIR)\gen_ho.obj"	"$(INTDIR)\gen_ho.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\lib\irs\gen_ng.c

"$(INTDIR)\gen_ng.obj"	"$(INTDIR)\gen_ng.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\lib\irs\gen_nw.c

"$(INTDIR)\gen_nw.obj"	"$(INTDIR)\gen_nw.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\lib\irs\gen_pr.c

"$(INTDIR)\gen_pr.obj"	"$(INTDIR)\gen_pr.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\lib\irs\gen_pw.c

"$(INTDIR)\gen_pw.obj"	"$(INTDIR)\gen_pw.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\lib\irs\gen_sv.c

"$(INTDIR)\gen_sv.obj"	"$(INTDIR)\gen_sv.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\lib\irs\getaddrinfo.c

"$(INTDIR)\getaddrinfo.obj"	"$(INTDIR)\getaddrinfo.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\lib\irs\getgrent.c

"$(INTDIR)\getgrent.obj"	"$(INTDIR)\getgrent.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\lib\irs\getgrent_r.c

"$(INTDIR)\getgrent_r.obj"	"$(INTDIR)\getgrent_r.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\lib\irs\gethostent.c

"$(INTDIR)\gethostent.obj"	"$(INTDIR)\gethostent.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\lib\irs\gethostent_r.c

"$(INTDIR)\gethostent_r.obj"	"$(INTDIR)\gethostent_r.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\lib\irs\getnameinfo.c

"$(INTDIR)\getnameinfo.obj"	"$(INTDIR)\getnameinfo.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\lib\irs\getnetent.c

"$(INTDIR)\getnetent.obj"	"$(INTDIR)\getnetent.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\lib\irs\getnetent_r.c

"$(INTDIR)\getnetent_r.obj"	"$(INTDIR)\getnetent_r.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\lib\irs\getnetgrent.c

"$(INTDIR)\getnetgrent.obj"	"$(INTDIR)\getnetgrent.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\lib\irs\getnetgrent_r.c

"$(INTDIR)\getnetgrent_r.obj"	"$(INTDIR)\getnetgrent_r.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\lib\irs\getprotoent.c

"$(INTDIR)\getprotoent.obj"	"$(INTDIR)\getprotoent.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\lib\irs\getprotoent_r.c

"$(INTDIR)\getprotoent_r.obj"	"$(INTDIR)\getprotoent_r.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\lib\irs\getpwent.c

"$(INTDIR)\getpwent.obj"	"$(INTDIR)\getpwent.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\lib\irs\getpwent_r.c

"$(INTDIR)\getpwent_r.obj"	"$(INTDIR)\getpwent_r.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\lib\irs\getservent.c

"$(INTDIR)\getservent.obj"	"$(INTDIR)\getservent.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\lib\irs\getservent_r.c

"$(INTDIR)\getservent_r.obj"	"$(INTDIR)\getservent_r.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\lib\irs\hesiod.c

"$(INTDIR)\hesiod.obj"	"$(INTDIR)\hesiod.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\lib\irs\irp.c

"$(INTDIR)\irp.obj"	"$(INTDIR)\irp.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\lib\irs\irp_gr.c

"$(INTDIR)\irp_gr.obj"	"$(INTDIR)\irp_gr.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\lib\irs\irp_ho.c

"$(INTDIR)\irp_ho.obj"	"$(INTDIR)\irp_ho.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\lib\irs\irp_ng.c

"$(INTDIR)\irp_ng.obj"	"$(INTDIR)\irp_ng.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\lib\irs\irp_nw.c

"$(INTDIR)\irp_nw.obj"	"$(INTDIR)\irp_nw.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\lib\irs\irp_pr.c

"$(INTDIR)\irp_pr.obj"	"$(INTDIR)\irp_pr.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\lib\irs\irp_pw.c

"$(INTDIR)\irp_pw.obj"	"$(INTDIR)\irp_pw.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\lib\irs\irp_sv.c

"$(INTDIR)\irp_sv.obj"	"$(INTDIR)\irp_sv.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\lib\irs\irpmarshall.c

"$(INTDIR)\irpmarshall.obj"	"$(INTDIR)\irpmarshall.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\lib\irs\irs_data.c

"$(INTDIR)\irs_data.obj"	"$(INTDIR)\irs_data.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\lib\irs\lcl.c

"$(INTDIR)\lcl.obj"	"$(INTDIR)\lcl.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\lib\irs\lcl_gr.c

"$(INTDIR)\lcl_gr.obj"	"$(INTDIR)\lcl_gr.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\lib\irs\lcl_ho.c

"$(INTDIR)\lcl_ho.obj"	"$(INTDIR)\lcl_ho.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\lib\irs\lcl_ng.c

"$(INTDIR)\lcl_ng.obj"	"$(INTDIR)\lcl_ng.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\lib\irs\lcl_nw.c

"$(INTDIR)\lcl_nw.obj"	"$(INTDIR)\lcl_nw.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\lib\irs\lcl_pr.c

"$(INTDIR)\lcl_pr.obj"	"$(INTDIR)\lcl_pr.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\lib\irs\lcl_pw.c

"$(INTDIR)\lcl_pw.obj"	"$(INTDIR)\lcl_pw.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\lib\irs\lcl_sv.c

"$(INTDIR)\lcl_sv.obj"	"$(INTDIR)\lcl_sv.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\lib\irs\nis.c

"$(INTDIR)\nis.obj"	"$(INTDIR)\nis.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\lib\irs\nis_gr.c

"$(INTDIR)\nis_gr.obj"	"$(INTDIR)\nis_gr.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\lib\irs\nis_ho.c

"$(INTDIR)\nis_ho.obj"	"$(INTDIR)\nis_ho.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\lib\irs\nis_ng.c

"$(INTDIR)\nis_ng.obj"	"$(INTDIR)\nis_ng.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\lib\irs\nis_nw.c

"$(INTDIR)\nis_nw.obj"	"$(INTDIR)\nis_nw.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\lib\irs\nis_pr.c

"$(INTDIR)\nis_pr.obj"	"$(INTDIR)\nis_pr.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\lib\irs\nis_pw.c

"$(INTDIR)\nis_pw.obj"	"$(INTDIR)\nis_pw.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\lib\irs\nis_sv.c

"$(INTDIR)\nis_sv.obj"	"$(INTDIR)\nis_sv.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\lib\irs\nul_ng.c

"$(INTDIR)\nul_ng.obj"	"$(INTDIR)\nul_ng.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\lib\irs\util.c

"$(INTDIR)\util.obj"	"$(INTDIR)\util.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\lib\resolv\herror.c

"$(INTDIR)\herror.obj"	"$(INTDIR)\herror.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\lib\resolv\res_comp.c

"$(INTDIR)\res_comp.obj"	"$(INTDIR)\res_comp.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\lib\resolv\res_data.c

"$(INTDIR)\res_data.obj"	"$(INTDIR)\res_data.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\lib\resolv\res_debug.c

"$(INTDIR)\res_debug.obj"	"$(INTDIR)\res_debug.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\lib\resolv\res_findzonecut.c

"$(INTDIR)\res_findzonecut.obj"	"$(INTDIR)\res_findzonecut.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\lib\resolv\res_init.c

"$(INTDIR)\res_init.obj"	"$(INTDIR)\res_init.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\lib\resolv\res_mkquery.c

"$(INTDIR)\res_mkquery.obj"	"$(INTDIR)\res_mkquery.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\lib\resolv\res_mkupdate.c

"$(INTDIR)\res_mkupdate.obj"	"$(INTDIR)\res_mkupdate.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\lib\resolv\res_query.c

"$(INTDIR)\res_query.obj"	"$(INTDIR)\res_query.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\lib\resolv\res_send.c

"$(INTDIR)\res_send.obj"	"$(INTDIR)\res_send.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\lib\resolv\res_sendsigned.c

"$(INTDIR)\res_sendsigned.obj"	"$(INTDIR)\res_sendsigned.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\lib\resolv\res_update.c

"$(INTDIR)\res_update.obj"	"$(INTDIR)\res_update.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\lib\nameser\ns_date.c

"$(INTDIR)\ns_date.obj"	"$(INTDIR)\ns_date.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\lib\nameser\ns_name.c

"$(INTDIR)\ns_name.obj"	"$(INTDIR)\ns_name.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\lib\nameser\ns_netint.c

"$(INTDIR)\ns_netint.obj"	"$(INTDIR)\ns_netint.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\lib\nameser\ns_parse.c

"$(INTDIR)\ns_parse.obj"	"$(INTDIR)\ns_parse.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\lib\nameser\ns_print.c

"$(INTDIR)\ns_print.obj"	"$(INTDIR)\ns_print.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\lib\nameser\ns_samedomain.c

"$(INTDIR)\ns_samedomain.obj"	"$(INTDIR)\ns_samedomain.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\lib\nameser\ns_sign.c

"$(INTDIR)\ns_sign.obj"	"$(INTDIR)\ns_sign.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\lib\nameser\ns_ttl.c

"$(INTDIR)\ns_ttl.obj"	"$(INTDIR)\ns_ttl.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\lib\nameser\ns_verify.c

"$(INTDIR)\ns_verify.obj"	"$(INTDIR)\ns_verify.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\lib\inet\inet_addr.c

"$(INTDIR)\inet_addr.obj"	"$(INTDIR)\inet_addr.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\lib\inet\inet_cidr_ntop.c

"$(INTDIR)\inet_cidr_ntop.obj"	"$(INTDIR)\inet_cidr_ntop.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\lib\inet\inet_cidr_pton.c

"$(INTDIR)\inet_cidr_pton.obj"	"$(INTDIR)\inet_cidr_pton.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\lib\inet\inet_lnaof.c

"$(INTDIR)\inet_lnaof.obj"	"$(INTDIR)\inet_lnaof.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\lib\inet\inet_makeaddr.c

"$(INTDIR)\inet_makeaddr.obj"	"$(INTDIR)\inet_makeaddr.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\lib\inet\inet_net_ntop.c

"$(INTDIR)\inet_net_ntop.obj"	"$(INTDIR)\inet_net_ntop.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\lib\inet\inet_net_pton.c

"$(INTDIR)\inet_net_pton.obj"	"$(INTDIR)\inet_net_pton.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\lib\inet\inet_neta.c

"$(INTDIR)\inet_neta.obj"	"$(INTDIR)\inet_neta.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\lib\inet\inet_netof.c

"$(INTDIR)\inet_netof.obj"	"$(INTDIR)\inet_netof.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\lib\inet\inet_network.c

"$(INTDIR)\inet_network.obj"	"$(INTDIR)\inet_network.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\lib\inet\inet_ntoa.c

"$(INTDIR)\inet_ntoa.obj"	"$(INTDIR)\inet_ntoa.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\lib\inet\inet_ntop.c

"$(INTDIR)\inet_ntop.obj"	"$(INTDIR)\inet_ntop.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\lib\inet\inet_pton.c

"$(INTDIR)\inet_pton.obj"	"$(INTDIR)\inet_pton.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\lib\inet\nsap_addr.c

"$(INTDIR)\nsap_addr.obj"	"$(INTDIR)\nsap_addr.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\lib\cylink\bits.c

"$(INTDIR)\bits.obj"	"$(INTDIR)\bits.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\lib\cylink\bn.c

"$(INTDIR)\bn.obj"	"$(INTDIR)\bn.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\lib\cylink\bn00.c

"$(INTDIR)\bn00.obj"	"$(INTDIR)\bn00.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\lib\cylink\ctk_prime.c

"$(INTDIR)\ctk_prime.obj"	"$(INTDIR)\ctk_prime.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\lib\cylink\dss.c

"$(INTDIR)\dss.obj"	"$(INTDIR)\dss.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\lib\cylink\lbn00.c

"$(INTDIR)\lbn00.obj"	"$(INTDIR)\lbn00.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\lib\cylink\lbnmem.c

"$(INTDIR)\lbnmem.obj"	"$(INTDIR)\lbnmem.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\lib\cylink\legal.c

"$(INTDIR)\legal.obj"	"$(INTDIR)\legal.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\lib\cylink\math.c

"$(INTDIR)\math.obj"	"$(INTDIR)\math.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\lib\cylink\rand.c

"$(INTDIR)\rand.obj"	"$(INTDIR)\rand.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\lib\cylink\sha.c

"$(INTDIR)\sha.obj"	"$(INTDIR)\sha.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\lib\cylink\swap.c

"$(INTDIR)\swap.obj"	"$(INTDIR)\swap.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\lib\dst\bsafe_link.c

"$(INTDIR)\bsafe_link.obj"	"$(INTDIR)\bsafe_link.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\lib\dst\cylink_link.c

"$(INTDIR)\cylink_link.obj"	"$(INTDIR)\cylink_link.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\lib\dst\dst_api.c

"$(INTDIR)\dst_api.obj"	"$(INTDIR)\dst_api.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\lib\dst\eay_dss_link.c

"$(INTDIR)\eay_dss_link.obj"	"$(INTDIR)\eay_dss_link.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\lib\dst\hmac_link.c

"$(INTDIR)\hmac_link.obj"	"$(INTDIR)\hmac_link.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\lib\dst\md5_dgst.c

"$(INTDIR)\md5_dgst.obj"	"$(INTDIR)\md5_dgst.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\lib\dst\prandom.c

"$(INTDIR)\prandom.obj"	"$(INTDIR)\prandom.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\lib\dst\rsaref_link.c

"$(INTDIR)\rsaref_link.obj"	"$(INTDIR)\rsaref_link.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\..\..\lib\dst\support.c

"$(INTDIR)\support.obj"	"$(INTDIR)\support.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\dirent.c

"$(INTDIR)\dirent.obj"	"$(INTDIR)\dirent.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\DLLMain.c

"$(INTDIR)\DLLMain.obj"	"$(INTDIR)\DLLMain.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\files.c

"$(INTDIR)\files.obj"	"$(INTDIR)\files.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\getopt.c

"$(INTDIR)\getopt.obj"	"$(INTDIR)\getopt.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\interface.c

"$(INTDIR)\interface.obj"	"$(INTDIR)\interface.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\ioctl_if.c

"$(INTDIR)\ioctl_if.obj"	"$(INTDIR)\ioctl_if.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\movefile.c

"$(INTDIR)\movefile.obj"	"$(INTDIR)\movefile.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\nameserver.c

"$(INTDIR)\nameserver.obj"	"$(INTDIR)\nameserver.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\signal.c

"$(INTDIR)\signal.obj"	"$(INTDIR)\signal.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\socket.c

"$(INTDIR)\socket.obj"	"$(INTDIR)\socket.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\syslog.c

"$(INTDIR)\syslog.obj"	"$(INTDIR)\syslog.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\unistd.c

"$(INTDIR)\unistd.obj"	"$(INTDIR)\unistd.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=..\..\..\lib\dnssafe\ahcbcpad.c

!IF  "$(CFG)" == "libbind - Win32 Release"

CPP_SWITCHES=/nologo /MD /W3 /GX /O2 /I "..\include" /I "..\..\..\include" /I "..\..\..\lib\dnssafe" /D "NDEBUG" /D "HAVE_CONFIG_H" /D "WIN32" /D "_WINDOWS" /D "LIB" /D "DNSSAFE" /D "i386" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\libbind.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\ahcbcpad.obj"	"$(INTDIR)\ahcbcpad.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "libbind - Win32 Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Gm /GX /Zi /Od /I "..\include" /I "..\..\..\include" /I "..\..\..\lib\dnssafe" /D "_DEBUG" /D "HAVE_CONFIG_H" /D "WIN32" /D "_WINDOWS" /D "LIB" /D "DNSSAFE" /D "i386" /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\ahcbcpad.obj"	"$(INTDIR)\ahcbcpad.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=..\..\..\lib\dnssafe\ahchdig.c

!IF  "$(CFG)" == "libbind - Win32 Release"

CPP_SWITCHES=/nologo /MD /W3 /GX /O2 /I "..\include" /I "..\..\..\include" /I "..\..\..\lib\dnssafe" /D "NDEBUG" /D "HAVE_CONFIG_H" /D "WIN32" /D "_WINDOWS" /D "LIB" /D "DNSSAFE" /D "i386" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\libbind.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\ahchdig.obj"	"$(INTDIR)\ahchdig.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "libbind - Win32 Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Gm /GX /Zi /Od /I "..\include" /I "..\..\..\include" /I "..\..\..\lib\dnssafe" /D "_DEBUG" /D "HAVE_CONFIG_H" /D "WIN32" /D "_WINDOWS" /D "LIB" /D "DNSSAFE" /D "i386" /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\ahchdig.obj"	"$(INTDIR)\ahchdig.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=..\..\..\lib\dnssafe\ahchencr.c

!IF  "$(CFG)" == "libbind - Win32 Release"

CPP_SWITCHES=/nologo /MD /W3 /GX /O2 /I "..\include" /I "..\..\..\include" /I "..\..\..\lib\dnssafe" /D "NDEBUG" /D "HAVE_CONFIG_H" /D "WIN32" /D "_WINDOWS" /D "LIB" /D "DNSSAFE" /D "i386" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\libbind.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\ahchencr.obj"	"$(INTDIR)\ahchencr.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "libbind - Win32 Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Gm /GX /Zi /Od /I "..\include" /I "..\..\..\include" /I "..\..\..\lib\dnssafe" /D "_DEBUG" /D "HAVE_CONFIG_H" /D "WIN32" /D "_WINDOWS" /D "LIB" /D "DNSSAFE" /D "i386" /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\ahchencr.obj"	"$(INTDIR)\ahchencr.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=..\..\..\lib\dnssafe\ahchgen.c

!IF  "$(CFG)" == "libbind - Win32 Release"

CPP_SWITCHES=/nologo /MD /W3 /GX /O2 /I "..\include" /I "..\..\..\include" /I "..\..\..\lib\dnssafe" /D "NDEBUG" /D "HAVE_CONFIG_H" /D "WIN32" /D "_WINDOWS" /D "LIB" /D "DNSSAFE" /D "i386" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\libbind.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\ahchgen.obj"	"$(INTDIR)\ahchgen.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "libbind - Win32 Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Gm /GX /Zi /Od /I "..\include" /I "..\..\..\include" /I "..\..\..\lib\dnssafe" /D "_DEBUG" /D "HAVE_CONFIG_H" /D "WIN32" /D "_WINDOWS" /D "LIB" /D "DNSSAFE" /D "i386" /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\ahchgen.obj"	"$(INTDIR)\ahchgen.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=..\..\..\lib\dnssafe\ahchrand.c

!IF  "$(CFG)" == "libbind - Win32 Release"

CPP_SWITCHES=/nologo /MD /W3 /GX /O2 /I "..\include" /I "..\..\..\include" /I "..\..\..\lib\dnssafe" /D "NDEBUG" /D "HAVE_CONFIG_H" /D "WIN32" /D "_WINDOWS" /D "LIB" /D "DNSSAFE" /D "i386" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\libbind.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\ahchrand.obj"	"$(INTDIR)\ahchrand.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "libbind - Win32 Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Gm /GX /Zi /Od /I "..\include" /I "..\..\..\include" /I "..\..\..\lib\dnssafe" /D "_DEBUG" /D "HAVE_CONFIG_H" /D "WIN32" /D "_WINDOWS" /D "LIB" /D "DNSSAFE" /D "i386" /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\ahchrand.obj"	"$(INTDIR)\ahchrand.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=..\..\..\lib\dnssafe\ahdigest.c

!IF  "$(CFG)" == "libbind - Win32 Release"

CPP_SWITCHES=/nologo /MD /W3 /GX /O2 /I "..\include" /I "..\..\..\include" /I "..\..\..\lib\dnssafe" /D "NDEBUG" /D "HAVE_CONFIG_H" /D "WIN32" /D "_WINDOWS" /D "LIB" /D "DNSSAFE" /D "i386" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\libbind.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\ahdigest.obj"	"$(INTDIR)\ahdigest.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "libbind - Win32 Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Gm /GX /Zi /Od /I "..\include" /I "..\..\..\include" /I "..\..\..\lib\dnssafe" /D "_DEBUG" /D "HAVE_CONFIG_H" /D "WIN32" /D "_WINDOWS" /D "LIB" /D "DNSSAFE" /D "i386" /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\ahdigest.obj"	"$(INTDIR)\ahdigest.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=..\..\..\lib\dnssafe\ahencryp.c

!IF  "$(CFG)" == "libbind - Win32 Release"

CPP_SWITCHES=/nologo /MD /W3 /GX /O2 /I "..\include" /I "..\..\..\include" /I "..\..\..\lib\dnssafe" /D "NDEBUG" /D "HAVE_CONFIG_H" /D "WIN32" /D "_WINDOWS" /D "LIB" /D "DNSSAFE" /D "i386" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\libbind.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\ahencryp.obj"	"$(INTDIR)\ahencryp.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "libbind - Win32 Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Gm /GX /Zi /Od /I "..\include" /I "..\..\..\include" /I "..\..\..\lib\dnssafe" /D "_DEBUG" /D "HAVE_CONFIG_H" /D "WIN32" /D "_WINDOWS" /D "LIB" /D "DNSSAFE" /D "i386" /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\ahencryp.obj"	"$(INTDIR)\ahencryp.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=..\..\..\lib\dnssafe\ahgen.c

!IF  "$(CFG)" == "libbind - Win32 Release"

CPP_SWITCHES=/nologo /MD /W3 /GX /O2 /I "..\include" /I "..\..\..\include" /I "..\..\..\lib\dnssafe" /D "NDEBUG" /D "HAVE_CONFIG_H" /D "WIN32" /D "_WINDOWS" /D "LIB" /D "DNSSAFE" /D "i386" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\libbind.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\ahgen.obj"	"$(INTDIR)\ahgen.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "libbind - Win32 Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Gm /GX /Zi /Od /I "..\include" /I "..\..\..\include" /I "..\..\..\lib\dnssafe" /D "_DEBUG" /D "HAVE_CONFIG_H" /D "WIN32" /D "_WINDOWS" /D "LIB" /D "DNSSAFE" /D "i386" /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\ahgen.obj"	"$(INTDIR)\ahgen.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=..\..\..\lib\dnssafe\ahrandom.c

!IF  "$(CFG)" == "libbind - Win32 Release"

CPP_SWITCHES=/nologo /MD /W3 /GX /O2 /I "..\include" /I "..\..\..\include" /I "..\..\..\lib\dnssafe" /D "NDEBUG" /D "HAVE_CONFIG_H" /D "WIN32" /D "_WINDOWS" /D "LIB" /D "DNSSAFE" /D "i386" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\libbind.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\ahrandom.obj"	"$(INTDIR)\ahrandom.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "libbind - Win32 Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Gm /GX /Zi /Od /I "..\include" /I "..\..\..\include" /I "..\..\..\lib\dnssafe" /D "_DEBUG" /D "HAVE_CONFIG_H" /D "WIN32" /D "_WINDOWS" /D "LIB" /D "DNSSAFE" /D "i386" /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\ahrandom.obj"	"$(INTDIR)\ahrandom.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=..\..\..\lib\dnssafe\ahrsaenc.c

!IF  "$(CFG)" == "libbind - Win32 Release"

CPP_SWITCHES=/nologo /MD /W3 /GX /O2 /I "..\include" /I "..\..\..\include" /I "..\..\..\lib\dnssafe" /D "NDEBUG" /D "HAVE_CONFIG_H" /D "WIN32" /D "_WINDOWS" /D "LIB" /D "DNSSAFE" /D "i386" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\libbind.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\ahrsaenc.obj"	"$(INTDIR)\ahrsaenc.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "libbind - Win32 Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Gm /GX /Zi /Od /I "..\include" /I "..\..\..\include" /I "..\..\..\lib\dnssafe" /D "_DEBUG" /D "HAVE_CONFIG_H" /D "WIN32" /D "_WINDOWS" /D "LIB" /D "DNSSAFE" /D "i386" /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\ahrsaenc.obj"	"$(INTDIR)\ahrsaenc.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=..\..\..\lib\dnssafe\ahrsaepr.c

!IF  "$(CFG)" == "libbind - Win32 Release"

CPP_SWITCHES=/nologo /MD /W3 /GX /O2 /I "..\include" /I "..\..\..\include" /I "..\..\..\lib\dnssafe" /D "NDEBUG" /D "HAVE_CONFIG_H" /D "WIN32" /D "_WINDOWS" /D "LIB" /D "DNSSAFE" /D "i386" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\libbind.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\ahrsaepr.obj"	"$(INTDIR)\ahrsaepr.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "libbind - Win32 Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Gm /GX /Zi /Od /I "..\include" /I "..\..\..\include" /I "..\..\..\lib\dnssafe" /D "_DEBUG" /D "HAVE_CONFIG_H" /D "WIN32" /D "_WINDOWS" /D "LIB" /D "DNSSAFE" /D "i386" /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\ahrsaepr.obj"	"$(INTDIR)\ahrsaepr.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=..\..\..\lib\dnssafe\ahrsaepu.c

!IF  "$(CFG)" == "libbind - Win32 Release"

CPP_SWITCHES=/nologo /MD /W3 /GX /O2 /I "..\include" /I "..\..\..\include" /I "..\..\..\lib\dnssafe" /D "NDEBUG" /D "HAVE_CONFIG_H" /D "WIN32" /D "_WINDOWS" /D "LIB" /D "DNSSAFE" /D "i386" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\libbind.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\ahrsaepu.obj"	"$(INTDIR)\ahrsaepu.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "libbind - Win32 Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Gm /GX /Zi /Od /I "..\include" /I "..\..\..\include" /I "..\..\..\lib\dnssafe" /D "_DEBUG" /D "HAVE_CONFIG_H" /D "WIN32" /D "_WINDOWS" /D "LIB" /D "DNSSAFE" /D "i386" /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\ahrsaepu.obj"	"$(INTDIR)\ahrsaepu.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=..\..\..\lib\dnssafe\aichdig.c

!IF  "$(CFG)" == "libbind - Win32 Release"

CPP_SWITCHES=/nologo /MD /W3 /GX /O2 /I "..\include" /I "..\..\..\include" /I "..\..\..\lib\dnssafe" /D "NDEBUG" /D "HAVE_CONFIG_H" /D "WIN32" /D "_WINDOWS" /D "LIB" /D "DNSSAFE" /D "i386" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\libbind.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\aichdig.obj"	"$(INTDIR)\aichdig.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "libbind - Win32 Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Gm /GX /Zi /Od /I "..\include" /I "..\..\..\include" /I "..\..\..\lib\dnssafe" /D "_DEBUG" /D "HAVE_CONFIG_H" /D "WIN32" /D "_WINDOWS" /D "LIB" /D "DNSSAFE" /D "i386" /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\aichdig.obj"	"$(INTDIR)\aichdig.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=..\..\..\lib\dnssafe\aichenc8.c

!IF  "$(CFG)" == "libbind - Win32 Release"

CPP_SWITCHES=/nologo /MD /W3 /GX /O2 /I "..\include" /I "..\..\..\include" /I "..\..\..\lib\dnssafe" /D "NDEBUG" /D "HAVE_CONFIG_H" /D "WIN32" /D "_WINDOWS" /D "LIB" /D "DNSSAFE" /D "i386" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\libbind.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\aichenc8.obj"	"$(INTDIR)\aichenc8.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "libbind - Win32 Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Gm /GX /Zi /Od /I "..\include" /I "..\..\..\include" /I "..\..\..\lib\dnssafe" /D "_DEBUG" /D "HAVE_CONFIG_H" /D "WIN32" /D "_WINDOWS" /D "LIB" /D "DNSSAFE" /D "i386" /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\aichenc8.obj"	"$(INTDIR)\aichenc8.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=..\..\..\lib\dnssafe\aichencn.c

!IF  "$(CFG)" == "libbind - Win32 Release"

CPP_SWITCHES=/nologo /MD /W3 /GX /O2 /I "..\include" /I "..\..\..\include" /I "..\..\..\lib\dnssafe" /D "NDEBUG" /D "HAVE_CONFIG_H" /D "WIN32" /D "_WINDOWS" /D "LIB" /D "DNSSAFE" /D "i386" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\libbind.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\aichencn.obj"	"$(INTDIR)\aichencn.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "libbind - Win32 Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Gm /GX /Zi /Od /I "..\include" /I "..\..\..\include" /I "..\..\..\lib\dnssafe" /D "_DEBUG" /D "HAVE_CONFIG_H" /D "WIN32" /D "_WINDOWS" /D "LIB" /D "DNSSAFE" /D "i386" /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\aichencn.obj"	"$(INTDIR)\aichencn.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=..\..\..\lib\dnssafe\aichencr.c

!IF  "$(CFG)" == "libbind - Win32 Release"

CPP_SWITCHES=/nologo /MD /W3 /GX /O2 /I "..\include" /I "..\..\..\include" /I "..\..\..\lib\dnssafe" /D "NDEBUG" /D "HAVE_CONFIG_H" /D "WIN32" /D "_WINDOWS" /D "LIB" /D "DNSSAFE" /D "i386" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\libbind.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\aichencr.obj"	"$(INTDIR)\aichencr.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "libbind - Win32 Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Gm /GX /Zi /Od /I "..\include" /I "..\..\..\include" /I "..\..\..\lib\dnssafe" /D "_DEBUG" /D "HAVE_CONFIG_H" /D "WIN32" /D "_WINDOWS" /D "LIB" /D "DNSSAFE" /D "i386" /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\aichencr.obj"	"$(INTDIR)\aichencr.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=..\..\..\lib\dnssafe\aichgen.c

!IF  "$(CFG)" == "libbind - Win32 Release"

CPP_SWITCHES=/nologo /MD /W3 /GX /O2 /I "..\include" /I "..\..\..\include" /I "..\..\..\lib\dnssafe" /D "NDEBUG" /D "HAVE_CONFIG_H" /D "WIN32" /D "_WINDOWS" /D "LIB" /D "DNSSAFE" /D "i386" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\libbind.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\aichgen.obj"	"$(INTDIR)\aichgen.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "libbind - Win32 Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Gm /GX /Zi /Od /I "..\include" /I "..\..\..\include" /I "..\..\..\lib\dnssafe" /D "_DEBUG" /D "HAVE_CONFIG_H" /D "WIN32" /D "_WINDOWS" /D "LIB" /D "DNSSAFE" /D "i386" /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\aichgen.obj"	"$(INTDIR)\aichgen.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=..\..\..\lib\dnssafe\aichrand.c

!IF  "$(CFG)" == "libbind - Win32 Release"

CPP_SWITCHES=/nologo /MD /W3 /GX /O2 /I "..\include" /I "..\..\..\include" /I "..\..\..\lib\dnssafe" /D "NDEBUG" /D "HAVE_CONFIG_H" /D "WIN32" /D "_WINDOWS" /D "LIB" /D "DNSSAFE" /D "i386" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\libbind.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\aichrand.obj"	"$(INTDIR)\aichrand.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "libbind - Win32 Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Gm /GX /Zi /Od /I "..\include" /I "..\..\..\include" /I "..\..\..\lib\dnssafe" /D "_DEBUG" /D "HAVE_CONFIG_H" /D "WIN32" /D "_WINDOWS" /D "LIB" /D "DNSSAFE" /D "i386" /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\aichrand.obj"	"$(INTDIR)\aichrand.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=..\..\..\lib\dnssafe\aimd5.c

!IF  "$(CFG)" == "libbind - Win32 Release"

CPP_SWITCHES=/nologo /MD /W3 /GX /O2 /I "..\include" /I "..\..\..\include" /I "..\..\..\lib\dnssafe" /D "NDEBUG" /D "HAVE_CONFIG_H" /D "WIN32" /D "_WINDOWS" /D "LIB" /D "DNSSAFE" /D "i386" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\libbind.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\aimd5.obj"	"$(INTDIR)\aimd5.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "libbind - Win32 Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Gm /GX /Zi /Od /I "..\include" /I "..\..\..\include" /I "..\..\..\lib\dnssafe" /D "_DEBUG" /D "HAVE_CONFIG_H" /D "WIN32" /D "_WINDOWS" /D "LIB" /D "DNSSAFE" /D "i386" /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\aimd5.obj"	"$(INTDIR)\aimd5.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=..\..\..\lib\dnssafe\aimd5ran.c

!IF  "$(CFG)" == "libbind - Win32 Release"

CPP_SWITCHES=/nologo /MD /W3 /GX /O2 /I "..\include" /I "..\..\..\include" /I "..\..\..\lib\dnssafe" /D "NDEBUG" /D "HAVE_CONFIG_H" /D "WIN32" /D "_WINDOWS" /D "LIB" /D "DNSSAFE" /D "i386" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\libbind.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\aimd5ran.obj"	"$(INTDIR)\aimd5ran.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "libbind - Win32 Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Gm /GX /Zi /Od /I "..\include" /I "..\..\..\include" /I "..\..\..\lib\dnssafe" /D "_DEBUG" /D "HAVE_CONFIG_H" /D "WIN32" /D "_WINDOWS" /D "LIB" /D "DNSSAFE" /D "i386" /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\aimd5ran.obj"	"$(INTDIR)\aimd5ran.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=..\..\..\lib\dnssafe\ainfotyp.c

!IF  "$(CFG)" == "libbind - Win32 Release"

CPP_SWITCHES=/nologo /MD /W3 /GX /O2 /I "..\include" /I "..\..\..\include" /I "..\..\..\lib\dnssafe" /D "NDEBUG" /D "HAVE_CONFIG_H" /D "WIN32" /D "_WINDOWS" /D "LIB" /D "DNSSAFE" /D "i386" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\libbind.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\ainfotyp.obj"	"$(INTDIR)\ainfotyp.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "libbind - Win32 Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Gm /GX /Zi /Od /I "..\include" /I "..\..\..\include" /I "..\..\..\lib\dnssafe" /D "_DEBUG" /D "HAVE_CONFIG_H" /D "WIN32" /D "_WINDOWS" /D "LIB" /D "DNSSAFE" /D "i386" /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\ainfotyp.obj"	"$(INTDIR)\ainfotyp.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=..\..\..\lib\dnssafe\ainull.c

!IF  "$(CFG)" == "libbind - Win32 Release"

CPP_SWITCHES=/nologo /MD /W3 /GX /O2 /I "..\include" /I "..\..\..\include" /I "..\..\..\lib\dnssafe" /D "NDEBUG" /D "HAVE_CONFIG_H" /D "WIN32" /D "_WINDOWS" /D "LIB" /D "DNSSAFE" /D "i386" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\libbind.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\ainull.obj"	"$(INTDIR)\ainull.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "libbind - Win32 Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Gm /GX /Zi /Od /I "..\include" /I "..\..\..\include" /I "..\..\..\lib\dnssafe" /D "_DEBUG" /D "HAVE_CONFIG_H" /D "WIN32" /D "_WINDOWS" /D "LIB" /D "DNSSAFE" /D "i386" /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\ainull.obj"	"$(INTDIR)\ainull.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=..\..\..\lib\dnssafe\airsaepr.c

!IF  "$(CFG)" == "libbind - Win32 Release"

CPP_SWITCHES=/nologo /MD /W3 /GX /O2 /I "..\include" /I "..\..\..\include" /I "..\..\..\lib\dnssafe" /D "NDEBUG" /D "HAVE_CONFIG_H" /D "WIN32" /D "_WINDOWS" /D "LIB" /D "DNSSAFE" /D "i386" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\libbind.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\airsaepr.obj"	"$(INTDIR)\airsaepr.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "libbind - Win32 Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Gm /GX /Zi /Od /I "..\include" /I "..\..\..\include" /I "..\..\..\lib\dnssafe" /D "_DEBUG" /D "HAVE_CONFIG_H" /D "WIN32" /D "_WINDOWS" /D "LIB" /D "DNSSAFE" /D "i386" /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\airsaepr.obj"	"$(INTDIR)\airsaepr.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=..\..\..\lib\dnssafe\airsaepu.c

!IF  "$(CFG)" == "libbind - Win32 Release"

CPP_SWITCHES=/nologo /MD /W3 /GX /O2 /I "..\include" /I "..\..\..\include" /I "..\..\..\lib\dnssafe" /D "NDEBUG" /D "HAVE_CONFIG_H" /D "WIN32" /D "_WINDOWS" /D "LIB" /D "DNSSAFE" /D "i386" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\libbind.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\airsaepu.obj"	"$(INTDIR)\airsaepu.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "libbind - Win32 Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Gm /GX /Zi /Od /I "..\include" /I "..\..\..\include" /I "..\..\..\lib\dnssafe" /D "_DEBUG" /D "HAVE_CONFIG_H" /D "WIN32" /D "_WINDOWS" /D "LIB" /D "DNSSAFE" /D "i386" /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\airsaepu.obj"	"$(INTDIR)\airsaepu.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=..\..\..\lib\dnssafe\airsakgn.c

!IF  "$(CFG)" == "libbind - Win32 Release"

CPP_SWITCHES=/nologo /MD /W3 /GX /O2 /I "..\include" /I "..\..\..\include" /I "..\..\..\lib\dnssafe" /D "NDEBUG" /D "HAVE_CONFIG_H" /D "WIN32" /D "_WINDOWS" /D "LIB" /D "DNSSAFE" /D "i386" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\libbind.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\airsakgn.obj"	"$(INTDIR)\airsakgn.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "libbind - Win32 Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Gm /GX /Zi /Od /I "..\include" /I "..\..\..\include" /I "..\..\..\lib\dnssafe" /D "_DEBUG" /D "HAVE_CONFIG_H" /D "WIN32" /D "_WINDOWS" /D "LIB" /D "DNSSAFE" /D "i386" /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\airsakgn.obj"	"$(INTDIR)\airsakgn.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=..\..\..\lib\dnssafe\airsaprv.c

!IF  "$(CFG)" == "libbind - Win32 Release"

CPP_SWITCHES=/nologo /MD /W3 /GX /O2 /I "..\include" /I "..\..\..\include" /I "..\..\..\lib\dnssafe" /D "NDEBUG" /D "HAVE_CONFIG_H" /D "WIN32" /D "_WINDOWS" /D "LIB" /D "DNSSAFE" /D "i386" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\libbind.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\airsaprv.obj"	"$(INTDIR)\airsaprv.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "libbind - Win32 Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Gm /GX /Zi /Od /I "..\include" /I "..\..\..\include" /I "..\..\..\lib\dnssafe" /D "_DEBUG" /D "HAVE_CONFIG_H" /D "WIN32" /D "_WINDOWS" /D "LIB" /D "DNSSAFE" /D "i386" /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\airsaprv.obj"	"$(INTDIR)\airsaprv.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=..\..\..\lib\dnssafe\airsapub.c

!IF  "$(CFG)" == "libbind - Win32 Release"

CPP_SWITCHES=/nologo /MD /W3 /GX /O2 /I "..\include" /I "..\..\..\include" /I "..\..\..\lib\dnssafe" /D "NDEBUG" /D "HAVE_CONFIG_H" /D "WIN32" /D "_WINDOWS" /D "LIB" /D "DNSSAFE" /D "i386" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\libbind.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\airsapub.obj"	"$(INTDIR)\airsapub.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "libbind - Win32 Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Gm /GX /Zi /Od /I "..\include" /I "..\..\..\include" /I "..\..\..\lib\dnssafe" /D "_DEBUG" /D "HAVE_CONFIG_H" /D "WIN32" /D "_WINDOWS" /D "LIB" /D "DNSSAFE" /D "i386" /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\airsapub.obj"	"$(INTDIR)\airsapub.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=..\..\..\lib\dnssafe\algchoic.c

!IF  "$(CFG)" == "libbind - Win32 Release"

CPP_SWITCHES=/nologo /MD /W3 /GX /O2 /I "..\include" /I "..\..\..\include" /I "..\..\..\lib\dnssafe" /D "NDEBUG" /D "HAVE_CONFIG_H" /D "WIN32" /D "_WINDOWS" /D "LIB" /D "DNSSAFE" /D "i386" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\libbind.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\algchoic.obj"	"$(INTDIR)\algchoic.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "libbind - Win32 Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Gm /GX /Zi /Od /I "..\include" /I "..\..\..\include" /I "..\..\..\lib\dnssafe" /D "_DEBUG" /D "HAVE_CONFIG_H" /D "WIN32" /D "_WINDOWS" /D "LIB" /D "DNSSAFE" /D "i386" /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\algchoic.obj"	"$(INTDIR)\algchoic.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=..\..\..\lib\dnssafe\algobj.c

!IF  "$(CFG)" == "libbind - Win32 Release"

CPP_SWITCHES=/nologo /MD /W3 /GX /O2 /I "..\include" /I "..\..\..\include" /I "..\..\..\lib\dnssafe" /D "NDEBUG" /D "HAVE_CONFIG_H" /D "WIN32" /D "_WINDOWS" /D "LIB" /D "DNSSAFE" /D "i386" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\libbind.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\algobj.obj"	"$(INTDIR)\algobj.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "libbind - Win32 Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Gm /GX /Zi /Od /I "..\include" /I "..\..\..\include" /I "..\..\..\lib\dnssafe" /D "_DEBUG" /D "HAVE_CONFIG_H" /D "WIN32" /D "_WINDOWS" /D "LIB" /D "DNSSAFE" /D "i386" /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\algobj.obj"	"$(INTDIR)\algobj.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=..\..\..\lib\dnssafe\amcrte.c

!IF  "$(CFG)" == "libbind - Win32 Release"

CPP_SWITCHES=/nologo /MD /W3 /GX /O2 /I "..\include" /I "..\..\..\include" /I "..\..\..\lib\dnssafe" /D "NDEBUG" /D "HAVE_CONFIG_H" /D "WIN32" /D "_WINDOWS" /D "LIB" /D "DNSSAFE" /D "i386" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\libbind.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\amcrte.obj"	"$(INTDIR)\amcrte.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "libbind - Win32 Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Gm /GX /Zi /Od /I "..\include" /I "..\..\..\include" /I "..\..\..\lib\dnssafe" /D "_DEBUG" /D "HAVE_CONFIG_H" /D "WIN32" /D "_WINDOWS" /D "LIB" /D "DNSSAFE" /D "i386" /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\amcrte.obj"	"$(INTDIR)\amcrte.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=..\..\..\lib\dnssafe\ammd5.c

!IF  "$(CFG)" == "libbind - Win32 Release"

CPP_SWITCHES=/nologo /MD /W3 /GX /O2 /I "..\include" /I "..\..\..\include" /I "..\..\..\lib\dnssafe" /D "NDEBUG" /D "HAVE_CONFIG_H" /D "WIN32" /D "_WINDOWS" /D "LIB" /D "DNSSAFE" /D "i386" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\libbind.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\ammd5.obj"	"$(INTDIR)\ammd5.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "libbind - Win32 Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Gm /GX /Zi /Od /I "..\include" /I "..\..\..\include" /I "..\..\..\lib\dnssafe" /D "_DEBUG" /D "HAVE_CONFIG_H" /D "WIN32" /D "_WINDOWS" /D "LIB" /D "DNSSAFE" /D "i386" /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\ammd5.obj"	"$(INTDIR)\ammd5.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=..\..\..\lib\dnssafe\ammd5r.c

!IF  "$(CFG)" == "libbind - Win32 Release"

CPP_SWITCHES=/nologo /MD /W3 /GX /O2 /I "..\include" /I "..\..\..\include" /I "..\..\..\lib\dnssafe" /D "NDEBUG" /D "HAVE_CONFIG_H" /D "WIN32" /D "_WINDOWS" /D "LIB" /D "DNSSAFE" /D "i386" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\libbind.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\ammd5r.obj"	"$(INTDIR)\ammd5r.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "libbind - Win32 Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Gm /GX /Zi /Od /I "..\include" /I "..\..\..\include" /I "..\..\..\lib\dnssafe" /D "_DEBUG" /D "HAVE_CONFIG_H" /D "WIN32" /D "_WINDOWS" /D "LIB" /D "DNSSAFE" /D "i386" /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\ammd5r.obj"	"$(INTDIR)\ammd5r.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=..\..\..\lib\dnssafe\amrkg.c

!IF  "$(CFG)" == "libbind - Win32 Release"

CPP_SWITCHES=/nologo /MD /W3 /GX /O2 /I "..\include" /I "..\..\..\include" /I "..\..\..\lib\dnssafe" /D "NDEBUG" /D "HAVE_CONFIG_H" /D "WIN32" /D "_WINDOWS" /D "LIB" /D "DNSSAFE" /D "i386" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\libbind.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\amrkg.obj"	"$(INTDIR)\amrkg.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "libbind - Win32 Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Gm /GX /Zi /Od /I "..\include" /I "..\..\..\include" /I "..\..\..\lib\dnssafe" /D "_DEBUG" /D "HAVE_CONFIG_H" /D "WIN32" /D "_WINDOWS" /D "LIB" /D "DNSSAFE" /D "i386" /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\amrkg.obj"	"$(INTDIR)\amrkg.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=..\..\..\lib\dnssafe\amrsae.c

!IF  "$(CFG)" == "libbind - Win32 Release"

CPP_SWITCHES=/nologo /MD /W3 /GX /O2 /I "..\include" /I "..\..\..\include" /I "..\..\..\lib\dnssafe" /D "NDEBUG" /D "HAVE_CONFIG_H" /D "WIN32" /D "_WINDOWS" /D "LIB" /D "DNSSAFE" /D "i386" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\libbind.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\amrsae.obj"	"$(INTDIR)\amrsae.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "libbind - Win32 Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Gm /GX /Zi /Od /I "..\include" /I "..\..\..\include" /I "..\..\..\lib\dnssafe" /D "_DEBUG" /D "HAVE_CONFIG_H" /D "WIN32" /D "_WINDOWS" /D "LIB" /D "DNSSAFE" /D "i386" /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\amrsae.obj"	"$(INTDIR)\amrsae.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=..\..\..\lib\dnssafe\balg.c

!IF  "$(CFG)" == "libbind - Win32 Release"

CPP_SWITCHES=/nologo /MD /W3 /GX /O2 /I "..\include" /I "..\..\..\include" /I "..\..\..\lib\dnssafe" /D "NDEBUG" /D "HAVE_CONFIG_H" /D "WIN32" /D "_WINDOWS" /D "LIB" /D "DNSSAFE" /D "i386" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\libbind.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\balg.obj"	"$(INTDIR)\balg.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "libbind - Win32 Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Gm /GX /Zi /Od /I "..\include" /I "..\..\..\include" /I "..\..\..\lib\dnssafe" /D "_DEBUG" /D "HAVE_CONFIG_H" /D "WIN32" /D "_WINDOWS" /D "LIB" /D "DNSSAFE" /D "i386" /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\balg.obj"	"$(INTDIR)\balg.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=..\..\..\lib\dnssafe\bgclrbit.c

!IF  "$(CFG)" == "libbind - Win32 Release"

CPP_SWITCHES=/nologo /MD /W3 /GX /O2 /I "..\include" /I "..\..\..\include" /I "..\..\..\lib\dnssafe" /D "NDEBUG" /D "HAVE_CONFIG_H" /D "WIN32" /D "_WINDOWS" /D "LIB" /D "DNSSAFE" /D "i386" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\libbind.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\bgclrbit.obj"	"$(INTDIR)\bgclrbit.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "libbind - Win32 Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Gm /GX /Zi /Od /I "..\include" /I "..\..\..\include" /I "..\..\..\lib\dnssafe" /D "_DEBUG" /D "HAVE_CONFIG_H" /D "WIN32" /D "_WINDOWS" /D "LIB" /D "DNSSAFE" /D "i386" /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\bgclrbit.obj"	"$(INTDIR)\bgclrbit.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=..\..\..\lib\dnssafe\bgmdmpyx.c

!IF  "$(CFG)" == "libbind - Win32 Release"

CPP_SWITCHES=/nologo /MD /W3 /GX /O2 /I "..\include" /I "..\..\..\include" /I "..\..\..\lib\dnssafe" /D "NDEBUG" /D "HAVE_CONFIG_H" /D "WIN32" /D "_WINDOWS" /D "LIB" /D "DNSSAFE" /D "i386" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\libbind.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\bgmdmpyx.obj"	"$(INTDIR)\bgmdmpyx.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "libbind - Win32 Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Gm /GX /Zi /Od /I "..\include" /I "..\..\..\include" /I "..\..\..\lib\dnssafe" /D "_DEBUG" /D "HAVE_CONFIG_H" /D "WIN32" /D "_WINDOWS" /D "LIB" /D "DNSSAFE" /D "i386" /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\bgmdmpyx.obj"	"$(INTDIR)\bgmdmpyx.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=..\..\..\lib\dnssafe\bgmdsqx.c

!IF  "$(CFG)" == "libbind - Win32 Release"

CPP_SWITCHES=/nologo /MD /W3 /GX /O2 /I "..\include" /I "..\..\..\include" /I "..\..\..\lib\dnssafe" /D "NDEBUG" /D "HAVE_CONFIG_H" /D "WIN32" /D "_WINDOWS" /D "LIB" /D "DNSSAFE" /D "i386" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\libbind.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\bgmdsqx.obj"	"$(INTDIR)\bgmdsqx.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "libbind - Win32 Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Gm /GX /Zi /Od /I "..\include" /I "..\..\..\include" /I "..\..\..\lib\dnssafe" /D "_DEBUG" /D "HAVE_CONFIG_H" /D "WIN32" /D "_WINDOWS" /D "LIB" /D "DNSSAFE" /D "i386" /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\bgmdsqx.obj"	"$(INTDIR)\bgmdsqx.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=..\..\..\lib\dnssafe\bgmodexp.c

!IF  "$(CFG)" == "libbind - Win32 Release"

CPP_SWITCHES=/nologo /MD /W3 /GX /O2 /I "..\include" /I "..\..\..\include" /I "..\..\..\lib\dnssafe" /D "NDEBUG" /D "HAVE_CONFIG_H" /D "WIN32" /D "_WINDOWS" /D "LIB" /D "DNSSAFE" /D "i386" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\libbind.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\bgmodexp.obj"	"$(INTDIR)\bgmodexp.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "libbind - Win32 Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Gm /GX /Zi /Od /I "..\include" /I "..\..\..\include" /I "..\..\..\lib\dnssafe" /D "_DEBUG" /D "HAVE_CONFIG_H" /D "WIN32" /D "_WINDOWS" /D "LIB" /D "DNSSAFE" /D "i386" /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\bgmodexp.obj"	"$(INTDIR)\bgmodexp.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=..\..\..\lib\dnssafe\bgpegcd.c

!IF  "$(CFG)" == "libbind - Win32 Release"

CPP_SWITCHES=/nologo /MD /W3 /GX /O2 /I "..\include" /I "..\..\..\include" /I "..\..\..\lib\dnssafe" /D "NDEBUG" /D "HAVE_CONFIG_H" /D "WIN32" /D "_WINDOWS" /D "LIB" /D "DNSSAFE" /D "i386" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\libbind.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\bgpegcd.obj"	"$(INTDIR)\bgpegcd.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "libbind - Win32 Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Gm /GX /Zi /Od /I "..\include" /I "..\..\..\include" /I "..\..\..\lib\dnssafe" /D "_DEBUG" /D "HAVE_CONFIG_H" /D "WIN32" /D "_WINDOWS" /D "LIB" /D "DNSSAFE" /D "i386" /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\bgpegcd.obj"	"$(INTDIR)\bgpegcd.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=..\..\..\lib\dnssafe\big2exp.c

!IF  "$(CFG)" == "libbind - Win32 Release"

CPP_SWITCHES=/nologo /MD /W3 /GX /O2 /I "..\include" /I "..\..\..\include" /I "..\..\..\lib\dnssafe" /D "NDEBUG" /D "HAVE_CONFIG_H" /D "WIN32" /D "_WINDOWS" /D "LIB" /D "DNSSAFE" /D "i386" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\libbind.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\big2exp.obj"	"$(INTDIR)\big2exp.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "libbind - Win32 Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Gm /GX /Zi /Od /I "..\include" /I "..\..\..\include" /I "..\..\..\lib\dnssafe" /D "_DEBUG" /D "HAVE_CONFIG_H" /D "WIN32" /D "_WINDOWS" /D "LIB" /D "DNSSAFE" /D "i386" /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\big2exp.obj"	"$(INTDIR)\big2exp.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=..\..\..\lib\dnssafe\bigabs.c

!IF  "$(CFG)" == "libbind - Win32 Release"

CPP_SWITCHES=/nologo /MD /W3 /GX /O2 /I "..\include" /I "..\..\..\include" /I "..\..\..\lib\dnssafe" /D "NDEBUG" /D "HAVE_CONFIG_H" /D "WIN32" /D "_WINDOWS" /D "LIB" /D "DNSSAFE" /D "i386" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\libbind.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\bigabs.obj"	"$(INTDIR)\bigabs.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "libbind - Win32 Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Gm /GX /Zi /Od /I "..\include" /I "..\..\..\include" /I "..\..\..\lib\dnssafe" /D "_DEBUG" /D "HAVE_CONFIG_H" /D "WIN32" /D "_WINDOWS" /D "LIB" /D "DNSSAFE" /D "i386" /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\bigabs.obj"	"$(INTDIR)\bigabs.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=..\..\..\lib\dnssafe\bigacc.c

!IF  "$(CFG)" == "libbind - Win32 Release"

CPP_SWITCHES=/nologo /MD /W3 /GX /O2 /I "..\include" /I "..\..\..\include" /I "..\..\..\lib\dnssafe" /D "NDEBUG" /D "HAVE_CONFIG_H" /D "WIN32" /D "_WINDOWS" /D "LIB" /D "DNSSAFE" /D "i386" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\libbind.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\bigacc.obj"	"$(INTDIR)\bigacc.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "libbind - Win32 Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Gm /GX /Zi /Od /I "..\include" /I "..\..\..\include" /I "..\..\..\lib\dnssafe" /D "_DEBUG" /D "HAVE_CONFIG_H" /D "WIN32" /D "_WINDOWS" /D "LIB" /D "DNSSAFE" /D "i386" /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\bigacc.obj"	"$(INTDIR)\bigacc.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=..\..\..\lib\dnssafe\bigarith.c

!IF  "$(CFG)" == "libbind - Win32 Release"

CPP_SWITCHES=/nologo /MD /W3 /GX /O2 /I "..\include" /I "..\..\..\include" /I "..\..\..\lib\dnssafe" /D "NDEBUG" /D "HAVE_CONFIG_H" /D "WIN32" /D "_WINDOWS" /D "LIB" /D "DNSSAFE" /D "i386" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\libbind.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\bigarith.obj"	"$(INTDIR)\bigarith.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "libbind - Win32 Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Gm /GX /Zi /Od /I "..\include" /I "..\..\..\include" /I "..\..\..\lib\dnssafe" /D "_DEBUG" /D "HAVE_CONFIG_H" /D "WIN32" /D "_WINDOWS" /D "LIB" /D "DNSSAFE" /D "i386" /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\bigarith.obj"	"$(INTDIR)\bigarith.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=..\..\..\lib\dnssafe\bigcmp.c

!IF  "$(CFG)" == "libbind - Win32 Release"

CPP_SWITCHES=/nologo /MD /W3 /GX /O2 /I "..\include" /I "..\..\..\include" /I "..\..\..\lib\dnssafe" /D "NDEBUG" /D "HAVE_CONFIG_H" /D "WIN32" /D "_WINDOWS" /D "LIB" /D "DNSSAFE" /D "i386" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\libbind.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\bigcmp.obj"	"$(INTDIR)\bigcmp.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "libbind - Win32 Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Gm /GX /Zi /Od /I "..\include" /I "..\..\..\include" /I "..\..\..\lib\dnssafe" /D "_DEBUG" /D "HAVE_CONFIG_H" /D "WIN32" /D "_WINDOWS" /D "LIB" /D "DNSSAFE" /D "i386" /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\bigcmp.obj"	"$(INTDIR)\bigcmp.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=..\..\..\lib\dnssafe\bigconst.c

!IF  "$(CFG)" == "libbind - Win32 Release"

CPP_SWITCHES=/nologo /MD /W3 /GX /O2 /I "..\include" /I "..\..\..\include" /I "..\..\..\lib\dnssafe" /D "NDEBUG" /D "HAVE_CONFIG_H" /D "WIN32" /D "_WINDOWS" /D "LIB" /D "DNSSAFE" /D "i386" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\libbind.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\bigconst.obj"	"$(INTDIR)\bigconst.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "libbind - Win32 Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Gm /GX /Zi /Od /I "..\include" /I "..\..\..\include" /I "..\..\..\lib\dnssafe" /D "_DEBUG" /D "HAVE_CONFIG_H" /D "WIN32" /D "_WINDOWS" /D "LIB" /D "DNSSAFE" /D "i386" /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\bigconst.obj"	"$(INTDIR)\bigconst.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=..\..\..\lib\dnssafe\biginv.c

!IF  "$(CFG)" == "libbind - Win32 Release"

CPP_SWITCHES=/nologo /MD /W3 /GX /O2 /I "..\include" /I "..\..\..\include" /I "..\..\..\lib\dnssafe" /D "NDEBUG" /D "HAVE_CONFIG_H" /D "WIN32" /D "_WINDOWS" /D "LIB" /D "DNSSAFE" /D "i386" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\libbind.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\biginv.obj"	"$(INTDIR)\biginv.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "libbind - Win32 Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Gm /GX /Zi /Od /I "..\include" /I "..\..\..\include" /I "..\..\..\lib\dnssafe" /D "_DEBUG" /D "HAVE_CONFIG_H" /D "WIN32" /D "_WINDOWS" /D "LIB" /D "DNSSAFE" /D "i386" /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\biginv.obj"	"$(INTDIR)\biginv.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=..\..\..\lib\dnssafe\biglen.c

!IF  "$(CFG)" == "libbind - Win32 Release"

CPP_SWITCHES=/nologo /MD /W3 /GX /O2 /I "..\include" /I "..\..\..\include" /I "..\..\..\lib\dnssafe" /D "NDEBUG" /D "HAVE_CONFIG_H" /D "WIN32" /D "_WINDOWS" /D "LIB" /D "DNSSAFE" /D "i386" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\libbind.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\biglen.obj"	"$(INTDIR)\biglen.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "libbind - Win32 Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Gm /GX /Zi /Od /I "..\include" /I "..\..\..\include" /I "..\..\..\lib\dnssafe" /D "_DEBUG" /D "HAVE_CONFIG_H" /D "WIN32" /D "_WINDOWS" /D "LIB" /D "DNSSAFE" /D "i386" /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\biglen.obj"	"$(INTDIR)\biglen.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=..\..\..\lib\dnssafe\bigmodx.c

!IF  "$(CFG)" == "libbind - Win32 Release"

CPP_SWITCHES=/nologo /MD /W3 /GX /O2 /I "..\include" /I "..\..\..\include" /I "..\..\..\lib\dnssafe" /D "NDEBUG" /D "HAVE_CONFIG_H" /D "WIN32" /D "_WINDOWS" /D "LIB" /D "DNSSAFE" /D "i386" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\libbind.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\bigmodx.obj"	"$(INTDIR)\bigmodx.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "libbind - Win32 Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Gm /GX /Zi /Od /I "..\include" /I "..\..\..\include" /I "..\..\..\lib\dnssafe" /D "_DEBUG" /D "HAVE_CONFIG_H" /D "WIN32" /D "_WINDOWS" /D "LIB" /D "DNSSAFE" /D "i386" /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\bigmodx.obj"	"$(INTDIR)\bigmodx.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=..\..\..\lib\dnssafe\bigmpy.c

!IF  "$(CFG)" == "libbind - Win32 Release"

CPP_SWITCHES=/nologo /MD /W3 /GX /O2 /I "..\include" /I "..\..\..\include" /I "..\..\..\lib\dnssafe" /D "NDEBUG" /D "HAVE_CONFIG_H" /D "WIN32" /D "_WINDOWS" /D "LIB" /D "DNSSAFE" /D "i386" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\libbind.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\bigmpy.obj"	"$(INTDIR)\bigmpy.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "libbind - Win32 Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Gm /GX /Zi /Od /I "..\include" /I "..\..\..\include" /I "..\..\..\lib\dnssafe" /D "_DEBUG" /D "HAVE_CONFIG_H" /D "WIN32" /D "_WINDOWS" /D "LIB" /D "DNSSAFE" /D "i386" /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\bigmpy.obj"	"$(INTDIR)\bigmpy.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=..\..\..\lib\dnssafe\bigpdiv.c

!IF  "$(CFG)" == "libbind - Win32 Release"

CPP_SWITCHES=/nologo /MD /W3 /GX /O2 /I "..\include" /I "..\..\..\include" /I "..\..\..\lib\dnssafe" /D "NDEBUG" /D "HAVE_CONFIG_H" /D "WIN32" /D "_WINDOWS" /D "LIB" /D "DNSSAFE" /D "i386" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\libbind.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\bigpdiv.obj"	"$(INTDIR)\bigpdiv.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "libbind - Win32 Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Gm /GX /Zi /Od /I "..\include" /I "..\..\..\include" /I "..\..\..\lib\dnssafe" /D "_DEBUG" /D "HAVE_CONFIG_H" /D "WIN32" /D "_WINDOWS" /D "LIB" /D "DNSSAFE" /D "i386" /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\bigpdiv.obj"	"$(INTDIR)\bigpdiv.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=..\..\..\lib\dnssafe\bigpmpy.c

!IF  "$(CFG)" == "libbind - Win32 Release"

CPP_SWITCHES=/nologo /MD /W3 /GX /O2 /I "..\include" /I "..\..\..\include" /I "..\..\..\lib\dnssafe" /D "NDEBUG" /D "HAVE_CONFIG_H" /D "WIN32" /D "_WINDOWS" /D "LIB" /D "DNSSAFE" /D "i386" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\libbind.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\bigpmpy.obj"	"$(INTDIR)\bigpmpy.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "libbind - Win32 Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Gm /GX /Zi /Od /I "..\include" /I "..\..\..\include" /I "..\..\..\lib\dnssafe" /D "_DEBUG" /D "HAVE_CONFIG_H" /D "WIN32" /D "_WINDOWS" /D "LIB" /D "DNSSAFE" /D "i386" /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\bigpmpy.obj"	"$(INTDIR)\bigpmpy.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=..\..\..\lib\dnssafe\bigpmpyh.c

!IF  "$(CFG)" == "libbind - Win32 Release"

CPP_SWITCHES=/nologo /MD /W3 /GX /O2 /I "..\include" /I "..\..\..\include" /I "..\..\..\lib\dnssafe" /D "NDEBUG" /D "HAVE_CONFIG_H" /D "WIN32" /D "_WINDOWS" /D "LIB" /D "DNSSAFE" /D "i386" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\libbind.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\bigpmpyh.obj"	"$(INTDIR)\bigpmpyh.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "libbind - Win32 Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Gm /GX /Zi /Od /I "..\include" /I "..\..\..\include" /I "..\..\..\lib\dnssafe" /D "_DEBUG" /D "HAVE_CONFIG_H" /D "WIN32" /D "_WINDOWS" /D "LIB" /D "DNSSAFE" /D "i386" /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\bigpmpyh.obj"	"$(INTDIR)\bigpmpyh.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=..\..\..\lib\dnssafe\bigpmpyl.c

!IF  "$(CFG)" == "libbind - Win32 Release"

CPP_SWITCHES=/nologo /MD /W3 /GX /O2 /I "..\include" /I "..\..\..\include" /I "..\..\..\lib\dnssafe" /D "NDEBUG" /D "HAVE_CONFIG_H" /D "WIN32" /D "_WINDOWS" /D "LIB" /D "DNSSAFE" /D "i386" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\libbind.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\bigpmpyl.obj"	"$(INTDIR)\bigpmpyl.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "libbind - Win32 Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Gm /GX /Zi /Od /I "..\include" /I "..\..\..\include" /I "..\..\..\lib\dnssafe" /D "_DEBUG" /D "HAVE_CONFIG_H" /D "WIN32" /D "_WINDOWS" /D "LIB" /D "DNSSAFE" /D "i386" /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\bigpmpyl.obj"	"$(INTDIR)\bigpmpyl.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=..\..\..\lib\dnssafe\bigpsq.c

!IF  "$(CFG)" == "libbind - Win32 Release"

CPP_SWITCHES=/nologo /MD /W3 /GX /O2 /I "..\include" /I "..\..\..\include" /I "..\..\..\lib\dnssafe" /D "NDEBUG" /D "HAVE_CONFIG_H" /D "WIN32" /D "_WINDOWS" /D "LIB" /D "DNSSAFE" /D "i386" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\libbind.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\bigpsq.obj"	"$(INTDIR)\bigpsq.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "libbind - Win32 Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Gm /GX /Zi /Od /I "..\include" /I "..\..\..\include" /I "..\..\..\lib\dnssafe" /D "_DEBUG" /D "HAVE_CONFIG_H" /D "WIN32" /D "_WINDOWS" /D "LIB" /D "DNSSAFE" /D "i386" /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\bigpsq.obj"	"$(INTDIR)\bigpsq.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=..\..\..\lib\dnssafe\bigqrx.c

!IF  "$(CFG)" == "libbind - Win32 Release"

CPP_SWITCHES=/nologo /MD /W3 /GX /O2 /I "..\include" /I "..\..\..\include" /I "..\..\..\lib\dnssafe" /D "NDEBUG" /D "HAVE_CONFIG_H" /D "WIN32" /D "_WINDOWS" /D "LIB" /D "DNSSAFE" /D "i386" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\libbind.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\bigqrx.obj"	"$(INTDIR)\bigqrx.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "libbind - Win32 Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Gm /GX /Zi /Od /I "..\include" /I "..\..\..\include" /I "..\..\..\lib\dnssafe" /D "_DEBUG" /D "HAVE_CONFIG_H" /D "WIN32" /D "_WINDOWS" /D "LIB" /D "DNSSAFE" /D "i386" /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\bigqrx.obj"	"$(INTDIR)\bigqrx.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=..\..\..\lib\dnssafe\bigsmod.c

!IF  "$(CFG)" == "libbind - Win32 Release"

CPP_SWITCHES=/nologo /MD /W3 /GX /O2 /I "..\include" /I "..\..\..\include" /I "..\..\..\lib\dnssafe" /D "NDEBUG" /D "HAVE_CONFIG_H" /D "WIN32" /D "_WINDOWS" /D "LIB" /D "DNSSAFE" /D "i386" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\libbind.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\bigsmod.obj"	"$(INTDIR)\bigsmod.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "libbind - Win32 Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Gm /GX /Zi /Od /I "..\include" /I "..\..\..\include" /I "..\..\..\lib\dnssafe" /D "_DEBUG" /D "HAVE_CONFIG_H" /D "WIN32" /D "_WINDOWS" /D "LIB" /D "DNSSAFE" /D "i386" /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\bigsmod.obj"	"$(INTDIR)\bigsmod.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=..\..\..\lib\dnssafe\bigtocan.c

!IF  "$(CFG)" == "libbind - Win32 Release"

CPP_SWITCHES=/nologo /MD /W3 /GX /O2 /I "..\include" /I "..\..\..\include" /I "..\..\..\lib\dnssafe" /D "NDEBUG" /D "HAVE_CONFIG_H" /D "WIN32" /D "_WINDOWS" /D "LIB" /D "DNSSAFE" /D "i386" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\libbind.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\bigtocan.obj"	"$(INTDIR)\bigtocan.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "libbind - Win32 Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Gm /GX /Zi /Od /I "..\include" /I "..\..\..\include" /I "..\..\..\lib\dnssafe" /D "_DEBUG" /D "HAVE_CONFIG_H" /D "WIN32" /D "_WINDOWS" /D "LIB" /D "DNSSAFE" /D "i386" /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\bigtocan.obj"	"$(INTDIR)\bigtocan.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=..\..\..\lib\dnssafe\bigu.c

!IF  "$(CFG)" == "libbind - Win32 Release"

CPP_SWITCHES=/nologo /MD /W3 /GX /O2 /I "..\include" /I "..\..\..\include" /I "..\..\..\lib\dnssafe" /D "NDEBUG" /D "HAVE_CONFIG_H" /D "WIN32" /D "_WINDOWS" /D "LIB" /D "DNSSAFE" /D "i386" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\libbind.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\bigu.obj"	"$(INTDIR)\bigu.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "libbind - Win32 Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Gm /GX /Zi /Od /I "..\include" /I "..\..\..\include" /I "..\..\..\lib\dnssafe" /D "_DEBUG" /D "HAVE_CONFIG_H" /D "WIN32" /D "_WINDOWS" /D "LIB" /D "DNSSAFE" /D "i386" /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\bigu.obj"	"$(INTDIR)\bigu.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=..\..\..\lib\dnssafe\bigunexp.c

!IF  "$(CFG)" == "libbind - Win32 Release"

CPP_SWITCHES=/nologo /MD /W3 /GX /O2 /I "..\include" /I "..\..\..\include" /I "..\..\..\lib\dnssafe" /D "NDEBUG" /D "HAVE_CONFIG_H" /D "WIN32" /D "_WINDOWS" /D "LIB" /D "DNSSAFE" /D "i386" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\libbind.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\bigunexp.obj"	"$(INTDIR)\bigunexp.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "libbind - Win32 Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Gm /GX /Zi /Od /I "..\include" /I "..\..\..\include" /I "..\..\..\lib\dnssafe" /D "_DEBUG" /D "HAVE_CONFIG_H" /D "WIN32" /D "_WINDOWS" /D "LIB" /D "DNSSAFE" /D "i386" /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\bigunexp.obj"	"$(INTDIR)\bigunexp.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=..\..\..\lib\dnssafe\binfocsh.c

!IF  "$(CFG)" == "libbind - Win32 Release"

CPP_SWITCHES=/nologo /MD /W3 /GX /O2 /I "..\include" /I "..\..\..\include" /I "..\..\..\lib\dnssafe" /D "NDEBUG" /D "HAVE_CONFIG_H" /D "WIN32" /D "_WINDOWS" /D "LIB" /D "DNSSAFE" /D "i386" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\libbind.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\binfocsh.obj"	"$(INTDIR)\binfocsh.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "libbind - Win32 Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Gm /GX /Zi /Od /I "..\include" /I "..\..\..\include" /I "..\..\..\lib\dnssafe" /D "_DEBUG" /D "HAVE_CONFIG_H" /D "WIN32" /D "_WINDOWS" /D "LIB" /D "DNSSAFE" /D "i386" /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\binfocsh.obj"	"$(INTDIR)\binfocsh.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=..\..\..\lib\dnssafe\bkey.c

!IF  "$(CFG)" == "libbind - Win32 Release"

CPP_SWITCHES=/nologo /MD /W3 /GX /O2 /I "..\include" /I "..\..\..\include" /I "..\..\..\lib\dnssafe" /D "NDEBUG" /D "HAVE_CONFIG_H" /D "WIN32" /D "_WINDOWS" /D "LIB" /D "DNSSAFE" /D "i386" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\libbind.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\bkey.obj"	"$(INTDIR)\bkey.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "libbind - Win32 Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Gm /GX /Zi /Od /I "..\include" /I "..\..\..\include" /I "..\..\..\lib\dnssafe" /D "_DEBUG" /D "HAVE_CONFIG_H" /D "WIN32" /D "_WINDOWS" /D "LIB" /D "DNSSAFE" /D "i386" /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\bkey.obj"	"$(INTDIR)\bkey.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=..\..\..\lib\dnssafe\bmempool.c

!IF  "$(CFG)" == "libbind - Win32 Release"

CPP_SWITCHES=/nologo /MD /W3 /GX /O2 /I "..\include" /I "..\..\..\include" /I "..\..\..\lib\dnssafe" /D "NDEBUG" /D "HAVE_CONFIG_H" /D "WIN32" /D "_WINDOWS" /D "LIB" /D "DNSSAFE" /D "i386" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\libbind.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\bmempool.obj"	"$(INTDIR)\bmempool.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "libbind - Win32 Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Gm /GX /Zi /Od /I "..\include" /I "..\..\..\include" /I "..\..\..\lib\dnssafe" /D "_DEBUG" /D "HAVE_CONFIG_H" /D "WIN32" /D "_WINDOWS" /D "LIB" /D "DNSSAFE" /D "i386" /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\bmempool.obj"	"$(INTDIR)\bmempool.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=..\..\..\lib\dnssafe\cantobig.c

!IF  "$(CFG)" == "libbind - Win32 Release"

CPP_SWITCHES=/nologo /MD /W3 /GX /O2 /I "..\include" /I "..\..\..\include" /I "..\..\..\lib\dnssafe" /D "NDEBUG" /D "HAVE_CONFIG_H" /D "WIN32" /D "_WINDOWS" /D "LIB" /D "DNSSAFE" /D "i386" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\libbind.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\cantobig.obj"	"$(INTDIR)\cantobig.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "libbind - Win32 Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Gm /GX /Zi /Od /I "..\include" /I "..\..\..\include" /I "..\..\..\lib\dnssafe" /D "_DEBUG" /D "HAVE_CONFIG_H" /D "WIN32" /D "_WINDOWS" /D "LIB" /D "DNSSAFE" /D "i386" /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\cantobig.obj"	"$(INTDIR)\cantobig.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=..\..\..\lib\dnssafe\crt2.c

!IF  "$(CFG)" == "libbind - Win32 Release"

CPP_SWITCHES=/nologo /MD /W3 /GX /O2 /I "..\include" /I "..\..\..\include" /I "..\..\..\lib\dnssafe" /D "NDEBUG" /D "HAVE_CONFIG_H" /D "WIN32" /D "_WINDOWS" /D "LIB" /D "DNSSAFE" /D "i386" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\libbind.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\crt2.obj"	"$(INTDIR)\crt2.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "libbind - Win32 Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Gm /GX /Zi /Od /I "..\include" /I "..\..\..\include" /I "..\..\..\lib\dnssafe" /D "_DEBUG" /D "HAVE_CONFIG_H" /D "WIN32" /D "_WINDOWS" /D "LIB" /D "DNSSAFE" /D "i386" /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\crt2.obj"	"$(INTDIR)\crt2.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=..\..\..\lib\dnssafe\digest.c

!IF  "$(CFG)" == "libbind - Win32 Release"

CPP_SWITCHES=/nologo /MD /W3 /GX /O2 /I "..\include" /I "..\..\..\include" /I "..\..\..\lib\dnssafe" /D "NDEBUG" /D "HAVE_CONFIG_H" /D "WIN32" /D "_WINDOWS" /D "LIB" /D "DNSSAFE" /D "i386" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\libbind.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\digest.obj"	"$(INTDIR)\digest.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "libbind - Win32 Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Gm /GX /Zi /Od /I "..\include" /I "..\..\..\include" /I "..\..\..\lib\dnssafe" /D "_DEBUG" /D "HAVE_CONFIG_H" /D "WIN32" /D "_WINDOWS" /D "LIB" /D "DNSSAFE" /D "i386" /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\digest.obj"	"$(INTDIR)\digest.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=..\..\..\lib\dnssafe\digrand.c

!IF  "$(CFG)" == "libbind - Win32 Release"

CPP_SWITCHES=/nologo /MD /W3 /GX /O2 /I "..\include" /I "..\..\..\include" /I "..\..\..\lib\dnssafe" /D "NDEBUG" /D "HAVE_CONFIG_H" /D "WIN32" /D "_WINDOWS" /D "LIB" /D "DNSSAFE" /D "i386" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\libbind.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\digrand.obj"	"$(INTDIR)\digrand.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "libbind - Win32 Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Gm /GX /Zi /Od /I "..\include" /I "..\..\..\include" /I "..\..\..\lib\dnssafe" /D "_DEBUG" /D "HAVE_CONFIG_H" /D "WIN32" /D "_WINDOWS" /D "LIB" /D "DNSSAFE" /D "i386" /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\digrand.obj"	"$(INTDIR)\digrand.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=..\..\..\lib\dnssafe\encrypt.c

!IF  "$(CFG)" == "libbind - Win32 Release"

CPP_SWITCHES=/nologo /MD /W3 /GX /O2 /I "..\include" /I "..\..\..\include" /I "..\..\..\lib\dnssafe" /D "NDEBUG" /D "HAVE_CONFIG_H" /D "WIN32" /D "_WINDOWS" /D "LIB" /D "DNSSAFE" /D "i386" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\libbind.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\encrypt.obj"	"$(INTDIR)\encrypt.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "libbind - Win32 Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Gm /GX /Zi /Od /I "..\include" /I "..\..\..\include" /I "..\..\..\lib\dnssafe" /D "_DEBUG" /D "HAVE_CONFIG_H" /D "WIN32" /D "_WINDOWS" /D "LIB" /D "DNSSAFE" /D "i386" /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\encrypt.obj"	"$(INTDIR)\encrypt.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=..\..\..\lib\dnssafe\generate.c

!IF  "$(CFG)" == "libbind - Win32 Release"

CPP_SWITCHES=/nologo /MD /W3 /GX /O2 /I "..\include" /I "..\..\..\include" /I "..\..\..\lib\dnssafe" /D "NDEBUG" /D "HAVE_CONFIG_H" /D "WIN32" /D "_WINDOWS" /D "LIB" /D "DNSSAFE" /D "i386" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\libbind.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\generate.obj"	"$(INTDIR)\generate.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "libbind - Win32 Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Gm /GX /Zi /Od /I "..\include" /I "..\..\..\include" /I "..\..\..\lib\dnssafe" /D "_DEBUG" /D "HAVE_CONFIG_H" /D "WIN32" /D "_WINDOWS" /D "LIB" /D "DNSSAFE" /D "i386" /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\generate.obj"	"$(INTDIR)\generate.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=..\..\..\lib\dnssafe\intbits.c

!IF  "$(CFG)" == "libbind - Win32 Release"

CPP_SWITCHES=/nologo /MD /W3 /GX /O2 /I "..\include" /I "..\..\..\include" /I "..\..\..\lib\dnssafe" /D "NDEBUG" /D "HAVE_CONFIG_H" /D "WIN32" /D "_WINDOWS" /D "LIB" /D "DNSSAFE" /D "i386" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\libbind.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\intbits.obj"	"$(INTDIR)\intbits.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "libbind - Win32 Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Gm /GX /Zi /Od /I "..\include" /I "..\..\..\include" /I "..\..\..\lib\dnssafe" /D "_DEBUG" /D "HAVE_CONFIG_H" /D "WIN32" /D "_WINDOWS" /D "LIB" /D "DNSSAFE" /D "i386" /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\intbits.obj"	"$(INTDIR)\intbits.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=..\..\..\lib\dnssafe\intitem.c

!IF  "$(CFG)" == "libbind - Win32 Release"

CPP_SWITCHES=/nologo /MD /W3 /GX /O2 /I "..\include" /I "..\..\..\include" /I "..\..\..\lib\dnssafe" /D "NDEBUG" /D "HAVE_CONFIG_H" /D "WIN32" /D "_WINDOWS" /D "LIB" /D "DNSSAFE" /D "i386" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\libbind.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\intitem.obj"	"$(INTDIR)\intitem.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "libbind - Win32 Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Gm /GX /Zi /Od /I "..\include" /I "..\..\..\include" /I "..\..\..\lib\dnssafe" /D "_DEBUG" /D "HAVE_CONFIG_H" /D "WIN32" /D "_WINDOWS" /D "LIB" /D "DNSSAFE" /D "i386" /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\intitem.obj"	"$(INTDIR)\intitem.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=..\..\..\lib\dnssafe\keyobj.c

!IF  "$(CFG)" == "libbind - Win32 Release"

CPP_SWITCHES=/nologo /MD /W3 /GX /O2 /I "..\include" /I "..\..\..\include" /I "..\..\..\lib\dnssafe" /D "NDEBUG" /D "HAVE_CONFIG_H" /D "WIN32" /D "_WINDOWS" /D "LIB" /D "DNSSAFE" /D "i386" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\libbind.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\keyobj.obj"	"$(INTDIR)\keyobj.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "libbind - Win32 Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Gm /GX /Zi /Od /I "..\include" /I "..\..\..\include" /I "..\..\..\lib\dnssafe" /D "_DEBUG" /D "HAVE_CONFIG_H" /D "WIN32" /D "_WINDOWS" /D "LIB" /D "DNSSAFE" /D "i386" /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\keyobj.obj"	"$(INTDIR)\keyobj.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=..\..\..\lib\dnssafe\ki8byte.c

!IF  "$(CFG)" == "libbind - Win32 Release"

CPP_SWITCHES=/nologo /MD /W3 /GX /O2 /I "..\include" /I "..\..\..\include" /I "..\..\..\lib\dnssafe" /D "NDEBUG" /D "HAVE_CONFIG_H" /D "WIN32" /D "_WINDOWS" /D "LIB" /D "DNSSAFE" /D "i386" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\libbind.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\ki8byte.obj"	"$(INTDIR)\ki8byte.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "libbind - Win32 Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Gm /GX /Zi /Od /I "..\include" /I "..\..\..\include" /I "..\..\..\lib\dnssafe" /D "_DEBUG" /D "HAVE_CONFIG_H" /D "WIN32" /D "_WINDOWS" /D "LIB" /D "DNSSAFE" /D "i386" /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\ki8byte.obj"	"$(INTDIR)\ki8byte.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=..\..\..\lib\dnssafe\kifulprv.c

!IF  "$(CFG)" == "libbind - Win32 Release"

CPP_SWITCHES=/nologo /MD /W3 /GX /O2 /I "..\include" /I "..\..\..\include" /I "..\..\..\lib\dnssafe" /D "NDEBUG" /D "HAVE_CONFIG_H" /D "WIN32" /D "_WINDOWS" /D "LIB" /D "DNSSAFE" /D "i386" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\libbind.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\kifulprv.obj"	"$(INTDIR)\kifulprv.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "libbind - Win32 Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Gm /GX /Zi /Od /I "..\include" /I "..\..\..\include" /I "..\..\..\lib\dnssafe" /D "_DEBUG" /D "HAVE_CONFIG_H" /D "WIN32" /D "_WINDOWS" /D "LIB" /D "DNSSAFE" /D "i386" /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\kifulprv.obj"	"$(INTDIR)\kifulprv.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=..\..\..\lib\dnssafe\kiitem.c

!IF  "$(CFG)" == "libbind - Win32 Release"

CPP_SWITCHES=/nologo /MD /W3 /GX /O2 /I "..\include" /I "..\..\..\include" /I "..\..\..\lib\dnssafe" /D "NDEBUG" /D "HAVE_CONFIG_H" /D "WIN32" /D "_WINDOWS" /D "LIB" /D "DNSSAFE" /D "i386" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\libbind.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\kiitem.obj"	"$(INTDIR)\kiitem.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "libbind - Win32 Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Gm /GX /Zi /Od /I "..\include" /I "..\..\..\include" /I "..\..\..\lib\dnssafe" /D "_DEBUG" /D "HAVE_CONFIG_H" /D "WIN32" /D "_WINDOWS" /D "LIB" /D "DNSSAFE" /D "i386" /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\kiitem.obj"	"$(INTDIR)\kiitem.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=..\..\..\lib\dnssafe\kinfotyp.c

!IF  "$(CFG)" == "libbind - Win32 Release"

CPP_SWITCHES=/nologo /MD /W3 /GX /O2 /I "..\include" /I "..\..\..\include" /I "..\..\..\lib\dnssafe" /D "NDEBUG" /D "HAVE_CONFIG_H" /D "WIN32" /D "_WINDOWS" /D "LIB" /D "DNSSAFE" /D "i386" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\libbind.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\kinfotyp.obj"	"$(INTDIR)\kinfotyp.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "libbind - Win32 Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Gm /GX /Zi /Od /I "..\include" /I "..\..\..\include" /I "..\..\..\lib\dnssafe" /D "_DEBUG" /D "HAVE_CONFIG_H" /D "WIN32" /D "_WINDOWS" /D "LIB" /D "DNSSAFE" /D "i386" /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\kinfotyp.obj"	"$(INTDIR)\kinfotyp.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=..\..\..\lib\dnssafe\kipkcrpr.c

!IF  "$(CFG)" == "libbind - Win32 Release"

CPP_SWITCHES=/nologo /MD /W3 /GX /O2 /I "..\include" /I "..\..\..\include" /I "..\..\..\lib\dnssafe" /D "NDEBUG" /D "HAVE_CONFIG_H" /D "WIN32" /D "_WINDOWS" /D "LIB" /D "DNSSAFE" /D "i386" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\libbind.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\kipkcrpr.obj"	"$(INTDIR)\kipkcrpr.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "libbind - Win32 Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Gm /GX /Zi /Od /I "..\include" /I "..\..\..\include" /I "..\..\..\lib\dnssafe" /D "_DEBUG" /D "HAVE_CONFIG_H" /D "WIN32" /D "_WINDOWS" /D "LIB" /D "DNSSAFE" /D "i386" /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\kipkcrpr.obj"	"$(INTDIR)\kipkcrpr.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=..\..\..\lib\dnssafe\kirsacrt.c

!IF  "$(CFG)" == "libbind - Win32 Release"

CPP_SWITCHES=/nologo /MD /W3 /GX /O2 /I "..\include" /I "..\..\..\include" /I "..\..\..\lib\dnssafe" /D "NDEBUG" /D "HAVE_CONFIG_H" /D "WIN32" /D "_WINDOWS" /D "LIB" /D "DNSSAFE" /D "i386" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\libbind.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\kirsacrt.obj"	"$(INTDIR)\kirsacrt.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "libbind - Win32 Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Gm /GX /Zi /Od /I "..\include" /I "..\..\..\include" /I "..\..\..\lib\dnssafe" /D "_DEBUG" /D "HAVE_CONFIG_H" /D "WIN32" /D "_WINDOWS" /D "LIB" /D "DNSSAFE" /D "i386" /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\kirsacrt.obj"	"$(INTDIR)\kirsacrt.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=..\..\..\lib\dnssafe\kirsapub.c

!IF  "$(CFG)" == "libbind - Win32 Release"

CPP_SWITCHES=/nologo /MD /W3 /GX /O2 /I "..\include" /I "..\..\..\include" /I "..\..\..\lib\dnssafe" /D "NDEBUG" /D "HAVE_CONFIG_H" /D "WIN32" /D "_WINDOWS" /D "LIB" /D "DNSSAFE" /D "i386" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\libbind.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\kirsapub.obj"	"$(INTDIR)\kirsapub.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "libbind - Win32 Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Gm /GX /Zi /Od /I "..\include" /I "..\..\..\include" /I "..\..\..\lib\dnssafe" /D "_DEBUG" /D "HAVE_CONFIG_H" /D "WIN32" /D "_WINDOWS" /D "LIB" /D "DNSSAFE" /D "i386" /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\kirsapub.obj"	"$(INTDIR)\kirsapub.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=..\..\..\lib\dnssafe\md5.c

!IF  "$(CFG)" == "libbind - Win32 Release"

CPP_SWITCHES=/nologo /MD /W3 /GX /O2 /I "..\include" /I "..\..\..\include" /I "..\..\..\lib\dnssafe" /D "NDEBUG" /D "HAVE_CONFIG_H" /D "WIN32" /D "_WINDOWS" /D "LIB" /D "DNSSAFE" /D "i386" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\libbind.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\md5.obj"	"$(INTDIR)\md5.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "libbind - Win32 Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Gm /GX /Zi /Od /I "..\include" /I "..\..\..\include" /I "..\..\..\lib\dnssafe" /D "_DEBUG" /D "HAVE_CONFIG_H" /D "WIN32" /D "_WINDOWS" /D "LIB" /D "DNSSAFE" /D "i386" /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\md5.obj"	"$(INTDIR)\md5.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=..\..\..\lib\dnssafe\md5rand.c

!IF  "$(CFG)" == "libbind - Win32 Release"

CPP_SWITCHES=/nologo /MD /W3 /GX /O2 /I "..\include" /I "..\..\..\include" /I "..\..\..\lib\dnssafe" /D "NDEBUG" /D "HAVE_CONFIG_H" /D "WIN32" /D "_WINDOWS" /D "LIB" /D "DNSSAFE" /D "i386" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\libbind.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\md5rand.obj"	"$(INTDIR)\md5rand.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "libbind - Win32 Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Gm /GX /Zi /Od /I "..\include" /I "..\..\..\include" /I "..\..\..\lib\dnssafe" /D "_DEBUG" /D "HAVE_CONFIG_H" /D "WIN32" /D "_WINDOWS" /D "LIB" /D "DNSSAFE" /D "i386" /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\md5rand.obj"	"$(INTDIR)\md5rand.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=..\..\..\lib\dnssafe\prime.c

!IF  "$(CFG)" == "libbind - Win32 Release"

CPP_SWITCHES=/nologo /MD /W3 /GX /O2 /I "..\include" /I "..\..\..\include" /I "..\..\..\lib\dnssafe" /D "NDEBUG" /D "HAVE_CONFIG_H" /D "WIN32" /D "_WINDOWS" /D "LIB" /D "DNSSAFE" /D "i386" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\libbind.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\prime.obj"	"$(INTDIR)\prime.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "libbind - Win32 Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Gm /GX /Zi /Od /I "..\include" /I "..\..\..\include" /I "..\..\..\lib\dnssafe" /D "_DEBUG" /D "HAVE_CONFIG_H" /D "WIN32" /D "_WINDOWS" /D "LIB" /D "DNSSAFE" /D "i386" /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\prime.obj"	"$(INTDIR)\prime.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=..\..\..\lib\dnssafe\random.c

!IF  "$(CFG)" == "libbind - Win32 Release"

CPP_SWITCHES=/nologo /MD /W3 /GX /O2 /I "..\include" /I "..\..\..\include" /I "..\..\..\lib\dnssafe" /D "NDEBUG" /D "HAVE_CONFIG_H" /D "WIN32" /D "_WINDOWS" /D "LIB" /D "DNSSAFE" /D "i386" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\libbind.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\random.obj"	"$(INTDIR)\random.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "libbind - Win32 Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Gm /GX /Zi /Od /I "..\include" /I "..\..\..\include" /I "..\..\..\lib\dnssafe" /D "_DEBUG" /D "HAVE_CONFIG_H" /D "WIN32" /D "_WINDOWS" /D "LIB" /D "DNSSAFE" /D "i386" /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\random.obj"	"$(INTDIR)\random.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=..\..\..\lib\dnssafe\rsa.c

!IF  "$(CFG)" == "libbind - Win32 Release"

CPP_SWITCHES=/nologo /MD /W3 /GX /O2 /I "..\include" /I "..\..\..\include" /I "..\..\..\lib\dnssafe" /D "NDEBUG" /D "HAVE_CONFIG_H" /D "WIN32" /D "_WINDOWS" /D "LIB" /D "DNSSAFE" /D "i386" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\libbind.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\rsa.obj"	"$(INTDIR)\rsa.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "libbind - Win32 Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Gm /GX /Zi /Od /I "..\include" /I "..\..\..\include" /I "..\..\..\lib\dnssafe" /D "_DEBUG" /D "HAVE_CONFIG_H" /D "WIN32" /D "_WINDOWS" /D "LIB" /D "DNSSAFE" /D "i386" /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\rsa.obj"	"$(INTDIR)\rsa.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=..\..\..\lib\dnssafe\rsakeygn.c

!IF  "$(CFG)" == "libbind - Win32 Release"

CPP_SWITCHES=/nologo /MD /W3 /GX /O2 /I "..\include" /I "..\..\..\include" /I "..\..\..\lib\dnssafe" /D "NDEBUG" /D "HAVE_CONFIG_H" /D "WIN32" /D "_WINDOWS" /D "LIB" /D "DNSSAFE" /D "i386" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\libbind.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\rsakeygn.obj"	"$(INTDIR)\rsakeygn.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "libbind - Win32 Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Gm /GX /Zi /Od /I "..\include" /I "..\..\..\include" /I "..\..\..\lib\dnssafe" /D "_DEBUG" /D "HAVE_CONFIG_H" /D "WIN32" /D "_WINDOWS" /D "LIB" /D "DNSSAFE" /D "i386" /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\rsakeygn.obj"	"$(INTDIR)\rsakeygn.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=..\..\..\lib\dnssafe\seccbcd.c

!IF  "$(CFG)" == "libbind - Win32 Release"

CPP_SWITCHES=/nologo /MD /W3 /GX /O2 /I "..\include" /I "..\..\..\include" /I "..\..\..\lib\dnssafe" /D "NDEBUG" /D "HAVE_CONFIG_H" /D "WIN32" /D "_WINDOWS" /D "LIB" /D "DNSSAFE" /D "i386" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\libbind.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\seccbcd.obj"	"$(INTDIR)\seccbcd.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "libbind - Win32 Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Gm /GX /Zi /Od /I "..\include" /I "..\..\..\include" /I "..\..\..\lib\dnssafe" /D "_DEBUG" /D "HAVE_CONFIG_H" /D "WIN32" /D "_WINDOWS" /D "LIB" /D "DNSSAFE" /D "i386" /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\seccbcd.obj"	"$(INTDIR)\seccbcd.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=..\..\..\lib\dnssafe\seccbce.c

!IF  "$(CFG)" == "libbind - Win32 Release"

CPP_SWITCHES=/nologo /MD /W3 /GX /O2 /I "..\include" /I "..\..\..\include" /I "..\..\..\lib\dnssafe" /D "NDEBUG" /D "HAVE_CONFIG_H" /D "WIN32" /D "_WINDOWS" /D "LIB" /D "DNSSAFE" /D "i386" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\libbind.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\seccbce.obj"	"$(INTDIR)\seccbce.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "libbind - Win32 Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Gm /GX /Zi /Od /I "..\include" /I "..\..\..\include" /I "..\..\..\lib\dnssafe" /D "_DEBUG" /D "HAVE_CONFIG_H" /D "WIN32" /D "_WINDOWS" /D "LIB" /D "DNSSAFE" /D "i386" /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\seccbce.obj"	"$(INTDIR)\seccbce.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=..\..\..\lib\dnssafe\surrendr.c

!IF  "$(CFG)" == "libbind - Win32 Release"

CPP_SWITCHES=/nologo /MD /W3 /GX /O2 /I "..\include" /I "..\..\..\include" /I "..\..\..\lib\dnssafe" /D "NDEBUG" /D "HAVE_CONFIG_H" /D "WIN32" /D "_WINDOWS" /D "LIB" /D "DNSSAFE" /D "i386" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\libbind.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\surrendr.obj"	"$(INTDIR)\surrendr.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "libbind - Win32 Debug"

CPP_SWITCHES=/nologo /MDd /W3 /Gm /GX /Zi /Od /I "..\include" /I "..\..\..\include" /I "..\..\..\lib\dnssafe" /D "_DEBUG" /D "HAVE_CONFIG_H" /D "WIN32" /D "_WINDOWS" /D "LIB" /D "DNSSAFE" /D "i386" /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\surrendr.obj"	"$(INTDIR)\surrendr.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 


!ENDIF 

