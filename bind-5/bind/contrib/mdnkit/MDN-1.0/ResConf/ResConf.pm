# $Id: ResConf.pm,v 1.1.1.1 2001/01/31 03:58:53 zarzycki Exp $
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
package MDN::ResConf;

use strict;
use Carp;
use vars qw($VERSION @ISA @EXPORT @EXPORT_OK $AUTOLOAD);

use MDN::Converter;
use MDN::Normalizer;

require Exporter;
require DynaLoader;

@ISA = qw(Exporter DynaLoader);
# Items to export into callers namespace by default. Note: do not export
# names by default without a very good reason. Use EXPORT_OK instead.
# Do not simply export all your public functions/methods/constants.
@EXPORT = qw();
$VERSION = '1.0';

bootstrap MDN::ResConf $VERSION;

# Preloaded methods go here.

sub localtodns {
    my ($self, $local_name) = @_;
    my ($ucs_name, $normalized_name);

    $ucs_name = $self->localtoucs($local_name);
    return undef if (!defined($ucs_name));

    $normalized_name = $self->normalize($ucs_name);
    return undef if (!defined($normalized_name));

    return $self->ucstodns($normalized_name);
}

sub dnstolocal {
    my ($self, $dns_name) = @_;
    my ($ucs_name);

    $ucs_name = $self->dnstoucs($dns_name);
    return undef if (!defined($ucs_name));

    return $self->ucstolocal($ucs_name);
}

# Autoload methods go after =cut, and are processed by the autosplit program.

package MDN::ResConf::Converter;
use vars qw(@ISA $VERSION);
@ISA = qw(MDN::Converter);
$VERSION = '1.0';
# Override DESTROY method, since we don't wanna let MDN::Converter call
# mdn_converter_destroy().
sub DESTROY {
    # print "MDN::ResConf::Converter::DESTROY\n";
}

package MDN::ResConf::Normalizer;
use vars qw(@ISA $VERSION);
@ISA = qw(MDN::Normalizer);
$VERSION = '1.0';
# Override DESTROY method, since we don't wanna let MDN::Normalizer call
# mdn_normalizer_destroy().
sub DESTROY {
    # print "MDN::ResConf::Normalizer::DESTROY\n";
}

1;
__END__

=head1 NAME

MDN::ResConf - Perl extension for libmdn resolver configuration module.

=head1 SYNOPSIS

  use MDN::ResConf;
  $res = MDN::ResConf->resconf('/usr/local/etc/mdnres.conf');

  $ucs_domain_name = $res->localtoucs($local_domain_name);
  $normalized_name = $res->normalize($ucs_domain_name);
  $dns_domain_name = $res->ucstodns($normalized_name);

=head1 DESCRIPTION

C<MDN::ResConf> provides a Perl object interface to the resolver
configuration and resolver utility modules of the MDN library
(a C library for handling multilingual domain names) in the mDNkit.

This module provides high-level interface to the library,
suitable for use in applications who needs MDN capability with ease.

=head1 CONSTRUCTOR

=over 4

=item new([$filename])

Creates a resolver configuration object.
C<$filename> specifies the configuration file name to be loaded.
If C<$filename> is omitted, the default file
(e.g. C</usr/local/etc/mdnres.conf>) is loaded.

Returns C<undef> if the configuration file is not loaded correctly.

=back

=head1 METHODS

=over 4

=item localtoucs($domain_name)

Converts C<$domain_name> from local encoding (determined by the
current locale and environment variable C<$MDN_LOCAL_CODESET>) to UTF-8,
and returns the result.

Returns C<undef> if C<$domain_name> is not a valid local encoding string.

=item ucstolocal($domain_name)

Converts C<$domain_name> from UTF-8 to the local encoding, and returns
the result.
This method does the reverse of C<localtoucs()>.

Returns C<undef> if C<$domain_name> is not a valid UTF-8 string.

=item normalize($domain_name)

Performs normalization on C<$domain_name> according to the normalization
schemes specified by the configuration, and then returns the result.
C<$domain_name> must be a valid UTF-8 encoded string.

Returns C<undef> if C<$domain_name> is not a valid UTF-8 string.

=item dnstoucs($domain_name)

Converts C<$domain_name> from the encoding used in DNS protocol 
(determined by the C<server-encoding> entry in the configuration file)
to UTF-8, and then returns the result.

Returns C<undef> if C<$domain_name> has invalid byte sequence.

=item ucstodns($domain_name)

Converts C<$domain_name> from UTF-8 to the encoding used in DNS protocol,
and returns the result.
This method does the reverse of C<dnstoucs()>.

Returns C<undef> if C<$domain_name> is not a valid UTF-8 string.

=item localtodns($domain_name)

Converts C<$domain_name> from the local encoding to the encoding used
in DNS protocol.
Also normalization is performed.

This is a convenient method and similar to:

    $conf->ucstodns($conf->normalize($conf->localtoucs($domain_name)))

=item dnstolocal($domain_name)

Converts C<$domain_name> from the encoding used in DNS protol to the
local encoding.
Note that unlike C<localtodns()>, normalization is B<NOT> performed.

This is a convenient method and similar to:

    $conf->ucstolocal($conf->dnstoucs($domain_name))

=item localconverter(), serverconverter(), alternateconverter()

Returns encoding converters for local encoding, server encoding
and alternate encoding.
The converters could convert a domain name from UTF-8 to the local,
server and alternate encoding, and vice versa.
(The alternate encoding is determined by the C<alternate-encoding>
entry in the configuration file.)

The converters that the methods return are objects of the
C<MDN::ResConf::Converter> type, which is sub class of L<MDN::Converter>.
C<MDN::ResConf::Converter> inherits all contructors, methods and
class methods from L<MDN::Converter>.

C<serverconverter()> and C<alternateconverter()> return C<undef>
if the configuration file doesn't define the corresponding entry.

=item normalizer()

Returns a domain name normalizer.
The normalizer that the method returns is an object of the
C<MDN::ResConf::Normalizer> type which is sub class of
L<MDN::Normalizer>.
All contructors, methods, and class methods that L<MDN::Normalizer>
provides are also available to C<MDN::ResConf::Normalizer>.

The normalizer could normalize a domain name according with
C<normalize> entries in the configuration file.

=item zld()

Returns a zero-level-domain name specified by C<server-zld> entry
in the configuration file.
If no zld is defined in the configuration file, this function returns
C<undef>.

=back

=head1 CLASS METHOD

=over 4

=item lasterror()

Returns the error message string corresponding to the last error occurred in
this module.

=back

=head1 SEE ALSO

L<MDN::Converter>, L<MDN::Normalizer>, L<mdnres.conf(5)>

MDN library specification

=cut
