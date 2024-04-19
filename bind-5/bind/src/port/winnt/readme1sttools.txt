		Window NT/2000 implementation of BIND8.2.3 Tools

Date: 6-Jan-2001.

  This is a release of BIND 8.2.3 for Windows NT/2000 systems. This
  release has undergone a fair amount of testing. While I have cleaned
  up all of the NT-specific issues and bugs that I know about, you
  should report any issues to the bind-users@isc.org mailing list.

	DNS Tools Kit Installation Information

  Unpack the kit into any convenient directory and run the tools.
  Note that libbind.dll should be in the same directory or the
  system32 directory to work.  It's preferable to keep them in the
  same directory as the tools.  The tools will NOT run on Win9x,
  only WinNT and Win2000.

	About the Tools

  I have built versions of the following tools for Windows NT/2000:
  dig, nslookup and host. If you wish to use the tools on another
  NT/2000 system you can copy the tools along with the libbind.dll to
  that other system and run them there.  

  Please be aware that the tools MUST retrieve a nameserver IP
  address in order to work.  There is a specific order that will
  be used to find the addresses. The order is as follows: it will
  look for the resolv.conf file in system32\Drivers\etc and try to
  open and read that file.  Only if no nameserver IP Address has
  been found will it use the registry settings to obtain a nameserver
  IP Address. The main implication of this is that the DNS Tools MAY
  behave differently on a system running named than on one NOT running
  named, depending on a) whether or not you have customized resolv.conf
  and b) have moved the nameserver to a different system but not
  modified resolv.conf.

	Known Problems

  On Windows 2000 the tools may fail to work because they need a
  nameserver IP address and the file resolv.conf which lists the
  nameserver IP addresses may not exist in the system32\Drivers\etc
  directory.  Since the location of the list in the Windows registry
  has changed, the resolver cannot find the list.  The workaround is
  to create the resolv.conf file.  Windows NT 4.0 does not have this
  problem and will work without the resolv.conf file.

	Danny Mayer
	mayer@gis.net

