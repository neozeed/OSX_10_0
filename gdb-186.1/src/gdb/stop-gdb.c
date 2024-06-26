/* A client to make GDB return to command level in Mach 3.
   Copyright (C) 1992, 1993 Free Software Foundation, Inc.

   This file is part of GDB.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.  */

/* Authors: Jukka Virtanen <jtv@hut.fi> and Peter Stout <pds@cs.cmu.edu>.

   A simple client to make GDB (versions 4.4 and later) on Mach 3 return
   to the command level when it is waiting for the inferior to stop.

   Actions: Lookup the send right to the GDB message port from the
   NetMsgServer.

   Send an asynchronous message with msgh_id
   GDB_MESSAGE_ID_STOP to that port.
 */

#include <stdio.h>

#include "defs.h"

#include <mach.h>
#include <mach/message.h>
#include <mach_error.h>
#include <servers/netname.h>
#include <servers/netname_defs.h>

void
main (int argc, char **argv)
{
  kern_return_t kr;
  mach_msg_header_t msg;
  mach_port_t gdb_port;
  char *host;
  char *name;

  if (argc == 1)
    argv[argc++] = GDB_DEF_NAME;

  if (argc != 2)
    {
      fprintf (stderr, "Usage : %s <GDB name>\n", argv[0]);
      exit (1);
    }

  /* Allow the user to specify a remote host.  */
  host = strchr (argv[1], '@');
  if (host)
    *(host++) = '\0';
  else
    host = (char *) "";

  name = malloc (strlen (argv[1]) + sizeof (GDB_NAME_PREFIX));
  if (name == NULL)
    {
      fprintf (stderr, "Unable to allocate memory for name.");
      exit (1);
    }

  strcpy (name, GDB_NAME_PREFIX);
  strcat (name, argv[1]);

  /* Look up the GDB service port.  For convenience, add the
     GDB_NAME_PREFIX the argument before looking up the name.
     For backwards compatibility, do it without.  */

  kr = netname_look_up (name_server_port, host, name, &gdb_port);
  if (kr == NETNAME_NOT_CHECKED_IN)
    kr = netname_look_up (name_server_port, host, argv[1], &gdb_port);
  if (kr != KERN_SUCCESS)
    {
      fprintf (stderr, "Unable to lookup the GDB service port: %s.\n",
	       mach_error_string (kr));
      exit (1);
    }

  /* Code generated by mig stub generator, with minor cleanups :-)

     simpleroutine stop_inferior(gdb_port : mach_port_t);  */

  msg.msgh_bits = MACH_MSGH_BITS (MACH_MSG_TYPE_COPY_SEND, 0);
  msg.msgh_remote_port = gdb_port;
  msg.msgh_local_port = MACH_PORT_NULL;
  msg.msgh_size = sizeof (msg);
  msg.msgh_seqno = 0;
  msg.msgh_id = GDB_MESSAGE_ID_STOP;

  kr = mach_msg_send (&msg);
  if (kr != KERN_SUCCESS)
    fprintf (stderr, "Message not sent, return code %d : %s\n", kr,
	     mach_error_string (kr));

  exit (kr != KERN_SUCCESS);
}
