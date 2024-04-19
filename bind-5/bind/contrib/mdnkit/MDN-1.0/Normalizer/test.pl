# $Id 
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

# Before `make install' is performed this script should be runnable with
# `make test'. After `make install' it should work as `perl test.pl'

######################### We start with some black magic to print on failure.

# Change 1..1 below to 1..last_test_to_print .
# (It may become useful if the test is moved to ./t subdirectory.)

BEGIN { $| = 1; print "1..14\n"; }
END {print "not ok 1\n" unless $loaded;}
use MDN::Normalizer;
$loaded = 1;
print "ok 1\n";

######################### End of black magic.

# Insert your test code below (better if it prints "ok 13"
# (correspondingly "not ok 13") depending on the success of chunk 13
# of the test code):

$test_id = 2;

#####
print "# create a normalizer\n";
$context = MDN::Normalizer->new();
$result = (ref($context) eq 'MDN::Normalizer');
print $result ? 'ok ' : 'not ok ', $test_id++, "\n";

#####
print "# create a normalizer, ascii-uppercase\n";
$context = MDN::Normalizer->new('ascii-uppercase');
$result = (ref($context) eq 'MDN::Normalizer');
print $result ? 'ok ' : 'not ok ', $test_id++, "\n";

#####
print "# create a normalizer, ascii-uppercase, unicode-uppercase\n";
$context = MDN::Normalizer->new('ascii-uppercase', 'unicode-uppercase');
$result = (ref($context) eq 'MDN::Normalizer');
print $result ? 'ok ' : 'not ok ', $test_id++, "\n";

#####
print "# create a normalizer, invalid scheme\n";
$context = MDN::Normalizer->new('invalid-scheme');
$result = (!ref($context)
	   && MDN::Normalizer->lasterror() =~ /invalid encoding name/);
print $result ? 'ok ' : 'not ok ', $test_id++, "\n";

#####
print "# add (empty)\n";
$context = MDN::Normalizer->new();
$result = $context->add();
print $result ? 'ok ' : 'not ok ', $test_id++, "\n";

#####
print "# add, ascii-uppercase\n";
$context = MDN::Normalizer->new();
$result = $context->add('ascii-uppercase');
print $result ? 'ok ' : 'not ok ', $test_id++, "\n";

#####
print "# add, ascii-uppercase, unicode-uppercase\n";
$context = MDN::Normalizer->new();
$result = $context->add('ascii-uppercase', 'unicode-uppercase');
print $result ? 'ok ' : 'not ok ', $test_id++, "\n";

#####
print "# normalize, ascii-uppercase\n";
$context = MDN::Normalizer->new('ascii-uppercase');
$result = ($context->normalize('Domain-Name') eq 'DOMAIN-NAME');
print $result ? 'ok ' : 'not ok ', $test_id++, "\n";

#####
print "# normalize, ascii-lowercase\n";
$context = MDN::Normalizer->new('ascii-lowercase');
$result = ($context->normalize('Domain-Name') eq 'domain-name');
print $result ? 'ok ' : 'not ok ', $test_id++, "\n";

#####
print "# normalize, unicode-uppercase\n";
$context = MDN::Normalizer->new('unicode-uppercase');
$result = ($context->normalize("\xc3\x86\xc3\xa6") eq "\xc3\x86\xc3\x86");
print $result ? 'ok ' : 'not ok ', $test_id++, "\n";

#####
print "# normalize, unicode-lowercase\n";
$context = MDN::Normalizer->new('unicode-lowercase');
$result = ($context->normalize("\xc3\x86\xc3\xa6") eq "\xc3\xa6\xc3\xa6");
print $result ? 'ok ' : 'not ok ', $test_id++, "\n";

#####
print "# normalize, ja-fullwidth\n";
$context = MDN::Normalizer->new('ja-fullwidth');
$result = ($context->normalize("\xef\xbd\xb1\xef\xbe\x9d")
	   eq "\xe3\x82\xa2\xe3\x83\xb3");
print $result ? 'ok ' : 'not ok ', $test_id++, "\n";

#####
print "# normalize, ja-minus-hack\n";
$context = MDN::Normalizer->new('ja-minus-hack');
$result = ($context->normalize("\xe2\x88\x92") eq '-');
print $result ? 'ok ' : 'not ok ', $test_id++, "\n";

