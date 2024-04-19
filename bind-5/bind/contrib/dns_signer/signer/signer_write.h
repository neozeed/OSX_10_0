/* $Header: /cvs/Darwin/Services/bind/bind/contrib/dns_signer/signer/signer_write.h,v 1.1.1.2 2001/01/31 03:58:43 zarzycki Exp $ */
#ifndef __signer_write_h__
#define __signer_write_h__

#include "dns_signer.h"

int write_set (		FILE			*file,
			struct name_rec		*nr,
			u_int16_t		type_h,
			struct signing_options	*options,
			int			is_glue);

int write_rrset (	FILE			*file,
			struct name_rec		*nr,
			struct rrset_rec	*rrs,
			struct signing_options	*options,
			int			is_glue);

int write_glue (	FILE			*file,
			struct name_rec		*nr,
			struct signing_options	*options);

void write_remove_duplicates (
			struct rrset_rec	*rrs,
			struct signing_options	*options);

#endif

/* Portions Copyright (c) 1995,1996,1997,1998 by Trusted Information Systems, Inc.
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND TRUSTED INFORMATION SYSTEMS DISCLAIMS
 * ALL WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL TRUSTED INFORMATION 
 * SYSTEMS BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL
 * DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR
 * PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS
 * ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
 * SOFTWARE.
 *
 *
 * Trusted Information Systems, Inc. has received approval from the
 * United States Government for export and reexport of TIS/DNSSEC
 * software from the United States of America under the provisions of
 * the Export Administration Regulations (EAR) General Software Note
 * (GSN) license exception for mass market software.  Under the
 * provisions of this license, this software may be exported or
 * reexported to all destinations except for the embargoed countries of
 * Cuba, Iran, Iraq, Libya, North Korea, Sudan and Syria.  Any export
 * or reexport of TIS/DNSSEC software to the embargoed countries
 * requires additional, specific licensing approval from the United
 * States Government. 
 */




