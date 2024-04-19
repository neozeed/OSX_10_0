# $Id: Log.pm,v 1.1.1.1 2001/01/31 03:58:53 zarzycki Exp $
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
package MDN::Log;

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

bootstrap MDN::Log $VERSION;

# Preloaded methods go here.

# Autoload methods go after =cut, and are processed by the autosplit program.

1;
__END__

=head1 NAME

MDN::Log - Perl extension for libmdn log module.

=head1 SYNOPSIS

  use MDN::Log;
  MDN::Log->setproc_none();
  MDN::Log->setproc_stderr();
  MDN::Log->setlevel('warning');
  $level = MDN::Log->getlevel();

=head1 DESCRIPTION

C<MDN::Log> provides a Perl interface to the log module of
the MDN library (a C library for handling
multilingual domain names) in the mDNkit.

With this module you can control how log messages are handled.

=head1 CLASS METHODS

Although this module does not provide object interface,
all the functions should be called as class methods,
in order to be consistent with other modules in C<MDN::>.

	MDN::Log->setlevel('warning');	# OK
	MDN::Log::setlevel('warning');	# NG

=over 4

=item setlevel($level)

Sets log level to C<$level>.
Valid values for C<$level> are C<'fatal'>, C<'error'>, C<'warning'>,
C<'info'>, C<'trace'>, and C<'debug'>.

The default level is determined by the value of the enrironment
variable C<MDN_LOG_LEVEL>.

=item getlevel()

Returns the current log level.

=item setproc_stderr()

Resets the log handler to the default one, which outputs the log
messages to standard error.

=item setproc_none()

Sets the log handler to the one which does nothing.

=item setproc([&handler])

Sets log handler to C<&handler>.
C<&handler> receives log level and message string as arguments
in that order.

	sub my_handler {
	    my ($level, $message) = @_;
	    # (process log message...)
	}

If C<&handler> is ommited, log handler is reset to the default
one.
This is the same as C<setproc_stderr()>.

=back

=head1 SEE ALSO

MDN library specification

=cut
