# $Id: UTF8.pm,v 1.1.1.1 2001/01/31 03:58:53 zarzycki Exp $
#
# Copyright (c) 2000 Japan Network Information Center.  All rights reserved.
#  
# By using this file, you agree to the terms and conditions set forth bellow.
# 
#                      LICENSE TERMS AND CONDITIONS 
# 
# The following License Terms and Conditions apply, unless a different
# license is obtained from Japan Network Information Center ("JPNIC"),
# a Japanese association, Fuundo Bldg., 1-2 Kanda Ogawamachi, Chiyoda-ku,
# Tokyo, Japan.
# 
# 1. Use, Modification and Redistribution (including distribution of any
#    modified or derived work) in source and/or binary forms is permitted
#    under this License Terms and Conditions.
# 
# 2. Redistribution of source code must retain the copyright notices as they
#    appear in each source code file, this License Terms and Conditions.
# 
# 3. Redistribution in binary form must reproduce the Copyright Notice,
#    this License Terms and Conditions, in the documentation and/or other
#    materials provided with the distribution.  For the purposes of binary
#    distribution the "Copyright Notice" refers to the following language:
#    "Copyright (c) Japan Network Information Center.  All rights reserved."
# 
# 4. Neither the name of JPNIC may be used to endorse or promote products
#    derived from this Software without specific prior written approval of
#    JPNIC.
# 
# 5. Disclaimer/Limitation of Liability: THIS SOFTWARE IS PROVIDED BY JPNIC
#    "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
#    LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
#    PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL JPNIC BE LIABLE
#    FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
#    CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
#    SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
#    BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
#    WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
#    OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
#    ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
# 
# 6. Indemnification by Licensee
#    Any person or entities using and/or redistributing this Software under
#    this License Terms and Conditions shall defend indemnify and hold
#    harmless JPNIC from and against any and all judgements damages,
#    expenses, settlement liabilities, cost and other liabilities of any
#    kind as a result of use and redistribution of this Software or any
#    claim, suite, action, litigation or proceeding by any third party
#    arising out of or relates to this License Terms and Conditions.
# 
# 7. Governing Law, Jurisdiction and Venue
#    This License Terms and Conditions shall be governed by and and
#    construed in accordance with the law of Japan. Any person or entities
#    using and/or redistributing this Software under this License Terms and
#    Conditions hereby agrees and consent to the personal and exclusive
#    jurisdiction and venue of Tokyo District Court of Japan.
#
package MDN::UTF8;

use strict;
use vars qw($VERSION @ISA @EXPORT @EXPORT_OK);

require Exporter;
require DynaLoader;

@ISA = qw(Exporter DynaLoader);
# Items to export into callers namespace by default. Note: do not export
# names by default without a very good reason. Use EXPORT_OK instead.
# Do not simply export all your public functions/methods/constants.
@EXPORT = qw();
$VERSION = '1.0';

bootstrap MDN::UTF8 $VERSION;

# Preloaded methods go here.

sub mblen {
    my ($package_name, $string) = @_;
    my ($wc, $length);

    if (($wc, $length) = $package_name->getwc($string)) {
	return $length;
    }
    return 0;
}

# Autoload methods go after =cut, and are processed by the autosplit program.

1;
__END__

=head1 NAME

MDN::UTF8 - Perl extension for libmdn utf8 module.

=head1 SYNOPSIS

  use MDN::UTF8;
  $length = MDN::UTF8->mblen($utf8_string);
  @ucs4_characters = MDN::UTF8->unpack($utf8_string);
  $utf8_string = MDN::UTF8->pack(@ucs4_characters);
  die if (!MDN::UTF8->isvalid($utf8_string));

=head1 DESCRIPTION

C<MDN::UTF8> provides a Perl interface to UTF-8 utility
module of the MDN library (a C library for handling
multilingual domain names) in the mDNkit.

=head1 CLASS METHODS

Although this module does not provide object interface,
all the functions should be called as class methods,
in order to be consistent with other modules in C<MDN::>.

	MDN::UTF8->mblen($string);	# OK
	MDN::UTF8::mblen($string);	# NG

=over 4

=item mblen($utf8_string)

Returns the length (in bytes) of the first character of C<$utf8_string>.
If the character is not a valid UTF-8 character, this method returns 0.

=item getwc($utf8_string)

Inspects the first character of C<$utf8_string>, and resturns the
result as a list with two elements.
The first elemnt of the list is the integer code value of the character
in the form of UCS-4, and the second is the length (in bytes) of the
character in the form of UTF-8.

	($wc, $length) = MDN::UTF8->getwc($string);

The value of the second element is the same as the one retruned from
C<mblen()>.
If the character is not a valid UTF-8 character, this method returns
an empty list.
Note that it also returns an empty list for an empty UTF-8 string.

=item unpack($utf8_string)

Unpacks C<$utf8_string> into a list of UCS-4 characters, and 
returns the list of integer code values of them.
An empty list is returned if C<$utf8_string> contains an invalid
character or C<$utf8_string> is empty.

=item pack(@ucs4_characters)

Packs a list of UCS-4 characters into an UTF-8 string, and returns
the string.  This is the reverse of C<unpack> method above.
If C<@ucs4_characters> contains an invalid UCS-4 character, it returns 
C<undef>.

=item isvalid($utf8_string)

Checks if C<$utf8_string> is a valid UTF-8 encoded string.
Returns 1 if it is valid, 0 otherwise.

=back

=head1 SEE ALSO

MDN library specification

=cut
