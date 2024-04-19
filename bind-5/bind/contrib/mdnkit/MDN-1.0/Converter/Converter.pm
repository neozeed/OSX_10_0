# $Id: Converter.pm,v 1.1.1.1 2001/01/31 03:58:53 zarzycki Exp $
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
package MDN::Converter;

use strict;
use Carp;
use vars qw($VERSION @ISA @EXPORT @EXPORT_OK $AUTOLOAD);

require Exporter;
require DynaLoader;
require AutoLoader;

@ISA = qw(Exporter DynaLoader);
# Items to export into callers namespace by default. Note: do not export
# names by default without a very good reason. Use EXPORT_OK instead.
# Do not simply export all your public functions/methods/constants.
@EXPORT = qw();
$VERSION = '1.0';

sub AUTOLOAD {
    # This AUTOLOAD is used to 'autoload' constants from the constant()
    # XS function.  If a constant is not found then control is passed
    # to the AUTOLOAD in AutoLoader.

    my $constname;
    ($constname = $AUTOLOAD) =~ s/.*:://;
    croak "& not defined" if $constname eq 'constant';
    my $val = constant($constname, @_ ? $_[0] : 0);
    if ($! != 0) {
	if ($! =~ /Invalid/) {
	    $AutoLoader::AUTOLOAD = $AUTOLOAD;
	    goto &AutoLoader::AUTOLOAD;
	}
	else {
		croak "Your vendor has not defined MDN::Converter macro $constname";
	}
    }
    no strict 'refs';
    *$AUTOLOAD = sub () { $val };
    goto &$AUTOLOAD;
}

bootstrap MDN::Converter $VERSION;

# Preloaded methods go here.

sub new {
    my ($package_name, $encoding_name, @options) = @_;
    my ($self, $option, $options_value);

    $options_value = 0;
    foreach $option (@options) {
	if ($option eq 'delayedopen') {
	    $options_value |= MDN::Converter->DELAYEDOPEN;
	} elsif ($option eq 'rtcheck') {
	    $options_value |= MDN::Converter->RTCHECK;
	}
    }
    $self = $package_name->new_internal($encoding_name, $options_value)
	or return undef;
    return $self;
}

sub convert_l2u {
    my ($self, $from) = @_;
    return $self->convert("l2u", $from);
}

sub convert_localtoutf8 {
    my ($self, $from) = @_;
    return $self->convert("localtoutf8", $from);
}

sub convert_u2l {
    my ($self, $from) = @_;
    return $self->convert("u2l", $from);
}

sub convert_utf8tolocal {
    my ($self, $from) = @_;
    return $self->convert("utf8tolocal", $from);
}

# Autoload methods go after =cut, and are processed by the autosplit program.

1;
__END__

=head1 NAME

MDN::Converter - Perl extension for libmdn converter module.

=head1 SYNOPSIS

  use MDN::Converter;
  $context = MDN::Converter->new('RACE');
  $local = $context->convert_utf8tolocal("utf8-domain-name"); 

=head1 DESCRIPTION

C<MDN::Converter> provides a Perl object interface to the encoding
converter module of the MDN library (a C library for handling
multilingual domain names) in the mDNkit.

Each converter object provides a encoding conversion between
a specific local encoding and UTF-8.

=head1 CONSTRUCTOR

=over 4

=item new($encoding_name [, $options...])

Creates a multilingual domain name converter.
C<$encoding_name> specifies the local encoding name.
The valid encoding names are the ones which I<iconv> (a generic
codeset converter function which MDN library uses internally)
accepts, and a few encodings specially designed for multilingual
domain names such as C<`RACE'>.

The created converter can translate a domain name from UTF-8 to
the specified local encoding, and vice versa.

The converter currently understands two options,
C<`delayedopen'> and C<`rtcheck'>.
C<`delayedopen'> delays opening the internal I<iconv> converter until
the conversion is actually performed.
C<`rtcheck'> enables round trip check, which is to ensure that the
conversion is correctly performed.

The constructor returns C<undef> if the given encoding name is unknown.

=back

=head1 METHODS

=over 4

=item convert($from, $direction)

Converts the encoding of the string C<$from> and returns the result.
If C<$direction> is C<`l2u'> or C<`localtoutf8'>, the converter assumes
C<$from> is a local encoding string and converts it to UTF-8.
If C<$direction> is C<`u2l'> or C<`utf8tolocal'>, the converter assumes
C<$from> is an UTF-8 string and converts it to the local encoding.

This method returns C<undef> if C<$from> is not a valid UTF-8 or local
encoding string, or the given direction is invalid.

=item convert_localtoutf8($from)

Equivalent to C<convert($from, 'l2u')>.

=item convert_l2u($from)

Equivalent to C<convert($from, 'l2u')>.

=item convert_utf8tolocal($from)

Equivalent to C<convert($from, 'u2l')>.

=item convert_u2l($from)

Equivalent to C<convert($from, 'u2l')>.

=item localencoding()

Returns the local encoding name as a string.

=item isasciicompatible()

Returns 1 if the local encoding is an ASCII-compatible encoding
(ACE for short), which is a special encoding having a property
that strings encoded in this encoding just look like ordinary
ASCII domain names (i.e. consisting of only alphanumeric letters and
hyphens).
Otherwise it returns 0.
Some of the encodings specially designed for multilingual domain names
(such as RACE) fall into this category.

=back

=head1 CLASS METHODS

=over 4

=item addalias($alias_name, $real_name)

Defines an alias name C<$alias_name> for a local encoding name
C<$real_name>.  C<$real_name> may be a previously defined alias.

=item aliasfile($file_name)

Registers aliases defined in the specified file.

=item resetalias()

Unregister all the aliases.

=item lasterror()

Returns the error message string corresponding to the last error occurred in
this module.

=back

=head1 SEE ALSO

L<MDN::Normalizer>

MDN library specification

=cut
