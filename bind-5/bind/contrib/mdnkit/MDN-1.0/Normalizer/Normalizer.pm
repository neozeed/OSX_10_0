# $Id: Normalizer.pm,v 1.1.1.1 2001/01/31 03:58:53 zarzycki Exp $
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
package MDN::Normalizer;

use strict;
use Carp;
use vars qw($VERSION @ISA @EXPORT @EXPORT_OK);

require Exporter;
require DynaLoader;

@ISA = qw(Exporter DynaLoader);
# Items to export into callers namespace by default. Note: do not export
# names by default without a very good reason. Use EXPORT_OK instead.
# Do not simply export all your public functions/methods/constants.
@EXPORT = qw();
$VERSION = '1.0';

bootstrap MDN::Normalizer $VERSION;

# Preloaded methods go here.

sub new {
    my ($package_name, @scheme_names) = @_;
    my ($self, $scheme_name);

    $self = $package_name->new_internal() or return undef;
    $self->add(@scheme_names) or return undef;
    return $self;
}

# Autoload methods go after =cut, and are processed by the autosplit program.

1;
__END__

=head1 NAME

MDN::Normalizer - Perl extension for libmdn normalizer module.

=head1 SYNOPSIS

  use MDN::Normalizer;
  $context = MDN::Normalizer->new();
  $context->add('unicode-uppercase', 'unicode-form-kc');

  # Upon success, $normalized should contain "DOMAIN-NAME".
  $normalized = $context->normalize("domain-name")
      or die MDN::Normalizer->lasterror(), "\n";

=head1 DESCRIPTION

C<MDN::Normalizer> provides a Perl object interface to domain name
normalization module of the MDN library (a C library for handling
multilingual domain names) in the mDNkit.

Each normalization object takes a UTF-8 encoded string as an input,
applies normalization schemes to the string, and returns the result.

=head1 CONSTRUCTOR

=over 4

=item new([$scheme_name...])

Creates a multilingual domain name normalizer.
If C<$scheme_name>s are specified, they are passed to the C<add()> mehtod.
If C<add()> returns 0, this constructor returns C<undef>.

=back

=head1 METHODS

=over 4

=item add([$scheme_name...])

Adds normalization schemes to the normalizer.

Acceptable scheme names are
C<'ascii-uppercase'>,
C<'ascii-lowercase'>,
C<'unicode-uppercase'>,
C<'unicode-lowercase'>,
C<'unicode-unicode-form-c'>,
C<'unicode-unicode-form-kc'>,
C<'ja-compose-voiced-sound'>,
C<'ja-kana-fullwidth'>,
C<'ja-alnum-halfwidth'>,
C<'ja-minus-hack'> and C<'ja-delimiter-hack'>.
See L<mdnres.conf(5)> for more details about each of the schemes.

This method returns 1 upon success, 0 otherwise.

=item normalize($from)

Normalizes the string C<$from> according to the registered schemes, and
returns the result.
If multiple schemes have been registered, each scheme will be applied
in turn in the order of registration.
C<$from> must be a string encoded in UTF-8.

It returns C<undef> if C<$from> is not a valid UTF-8 string.

=back

=head1 CLASS METHOD

=over 4

=item lasterror()

Returns the error message string corresponding to the last error occurred in
this module.

=back

=head1 SEE ALSO

L<MDN::Converter>, L<mdnres.conf(5)>

MDN library specification

=cut
