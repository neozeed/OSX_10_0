		Window NT/2000 implementation of BIND8.2.3

Date: 6-Jan-2001.

  This is a release of BIND 8.2.3 for Windows NT/2000 systems. This
  release has undergone a fair amount of testing, but if you choose
  to install this kit, you should run it first on a test system,
  preferably as a slave server. While I have cleaned up all of the
  NT-specific issues and bugs that I know about, you should report
  any issues to the bind-users@isc.org mailing list. The kit can
  be installed on both Windows NT 4.0 and Windows 2000 (any type).

  NOTE: IPv6 testing has not been done.  It was not a goal of this
  port to Windows NT.  We would, however, be interested to hear
  whether or not it works with IPv6 sockets.  There has been work
  done to retrieve the IPv6 Interfaces, but there has been no real
  testing.

	Kit Installation Information

  Unpack the kit into any convenient directory and run the BINDInstall
  program.  This will install the named and associated programs
  into the correct directories and set up the required registry
  keys.  If you have been running BIND 8.2.x on the system before, you
  do not need to uninstall it first. If you have been running any
  other version of DNS, you will need to go to the services control
  panel and stop and disable it before installing this version of
  BIND or run its uninstall program.  BINDInstall copies itself to
  the same directory as named and can be run from there to uninstall
  BIND.

	Controlling BIND

  Windows NT/2000 has a Windows application called BINDCtrl which should
  be a complete replacement for Unix's ndc (named daemon controller)
  program.  In addition named is installed as a system service and
  will start up automatically when the system boots. BINDCtrl is
  placed in the same directory as named:  System32\dns\bin and can
  be used from there.  The installer does not create a menu shortcut
  or desktop shortcut, so you may choose to do this yourself.

	DNS Tools

  I have built versions of the following tools for Windows NT: dig,
  nslookup and host.  These tools are not copied anywhere. You
  should copy them to any convenient directory.  Note that these
  tools require libbind.dll so it is important that this file is
  in the path. The install places this file in the system32 directory
  so the tools will work on the system that you install BIND on.
  If you wish to use the tools on another NT system you can copy
  the tools along with the libbind.dll to that other system and
  run them there.  Again, libbind.dll must be in the path.  The
  simplest way of accomplishing this is to copy the tools and
  libbind.dll to the same directory. The tools will NOT run on
  Win9x, only WinNT and Win2000.

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

