/* Host-dependent definitions for FreeBSD/i386.
   Copyright (C) 2000 Free Software Foundation, Inc.

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

#ifndef XM_FBSD_H
#define XM_FBSD_H

#define HOST_BYTE_ORDER LITTLE_ENDIAN

#define HOST_LONG_DOUBLE_FORMAT &floatformat_i387_ext

#endif /* XM_FBSD_H */
