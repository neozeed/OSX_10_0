/* Handle shared libraries for GDB, the GNU Debugger.
   Copyright 2000
   Free Software Foundation, Inc.

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

/* Critical offsets and sizes which describe struct r_debug and
   struct link_map on SVR4-like targets.  All offsets and sizes are
   in bytes unless otherwise specified.  */

struct link_map_offsets
  {
    /* Size of struct r_debug (or equivalent), or at least enough of it to
       be able to obtain the r_map field.  */
    int r_debug_size;

    /* Offset to the r_map field in struct r_debug.  */
    int r_map_offset;

    /* Size of the r_map field in struct r_debug.  */
    int r_map_size;

    /* Size of struct link_map (or equivalent), or at least enough of it
       to be able to obtain the fields below.  */
    int link_map_size;

    /* Offset to l_addr field in struct link_map.  */
    int l_addr_offset;

    /* Size of l_addr field in struct link_map.  */
    int l_addr_size;

    /* Offset to l_next field in struct link_map.  */
    int l_next_offset;

    /* Size of l_next field in struct link_map.  */
    int l_next_size;

    /* Offset to l_prev field in struct link_map.  */
    int l_prev_offset;

    /* Size of l_prev field in struct link_map.  */
    int l_prev_size;

    /* Offset to l_name field in struct link_map.  */
    int l_name_offset;

    /* Size of l_name field in struct link_map.  */
    int l_name_size;
  };

#ifndef SVR4_FETCH_LINK_MAP_OFFSETS
extern struct link_map_offsets *default_svr4_fetch_link_map_offsets (void);
#define SVR4_FETCH_LINK_MAP_OFFSETS() default_svr4_fetch_link_map_offsets ()
#endif
