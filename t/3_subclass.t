use strict;
use vars qw($loaded);

BEGIN { $^W = 1; $| = 1; print "1..10\n"; }

package Text::Tmpl::TestSubClass;

use Text::Tmpl;

@Text::Tmpl::TestSubClass::ISA = qw(Text::Tmpl);

sub test_sub {
    my $self = shift || return(0);
    $self->set_value('test_sub', 'poot');
    return(1);
}

sub set_value {
    my $self = shift || return(0);
    my $var  = shift || return(0);
    my $val  = shift || return(0);

    return $self->SUPER::set_value($var, 'subclass-' . $val);
}

package main;

use IO::File;

use constant TEMPLATE => 't/3_subclass.tmpl';
use constant COMPARE  => 't/3_subclass.comp';

my($return, $subcontext, $output, $compare);

my $context = new Text::Tmpl::TestSubClass;
if (! defined($context)) {
    print "not ok 1\n";
    exit(0);
}
my $comp_fh = new IO::File COMPARE, 'r';
if (! defined $comp_fh) {
    print "not ok 1\n";
    exit(0);
}

{
    local $/ = undef;
    $compare = <$comp_fh>;
}

$comp_fh->close;

print "ok 1\n";

$return = $context->set_strip(0);
print "ok 2\n";

$return = $context->test_sub;
if (! $return) {
    print "not ok 3\n";
} else {
    print "ok 3\n";
}

$return = $context->set_value('foo', 'bar');
if (! $return) {
    print "not ok 4\n";
} else {
    print "ok 4\n";
}

$subcontext = $context->loop_iteration('foo');
if (! defined $subcontext) {
    print "not ok 5\n";
} else {
    print "ok 5\n";
}

$subcontext = $context->fetch_loop_iteration('foo', 0);
if (! defined $subcontext) {
    print "not ok 6\n";
} else {
    print "ok 6\n";
}

$return = $subcontext->set_value('bar', 'baz');
if (! $return) {
    print "not ok 7\n";
} else {
    print "ok 7\n";
}

$return = $subcontext->test_sub;
if (! $return) {
    print "not ok 8\n";
} else {
    print "ok 8\n";
}

$output = $context->parse_file(TEMPLATE);
if (! defined($output)) {
    print "not ok 9\n";
} else {
    print "ok 9\n";
}

if ($output ne $compare) {
    print "not ok 10\n";
} else {
    print "ok 10\n";
}
