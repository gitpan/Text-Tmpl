use strict;
use vars qw($loaded);

BEGIN { $^W = 1; $| = 1; print "1..10\n"; }

use Text::Tmpl qw(set_delimiters
                  register_simple
                  register_pair
                  TEMPLATE_DEBUG_NONE
                  TEMPLATE_DEBUG_SOME
                  TEMPLATE_DEBUG_LOTS
                  TEMPLATE_TRUE
                  TEMPLATE_FALSE);

my $return;
my($context) = new Text::Tmpl;
if (! defined($context)) {
    print "not ok 1\n";
} else {
    print "ok 1\n";
}

$return = set_delimiters('<%', '%>');
if (! $return) {
    print "not ok 2\n";
} else {
    print "ok 2\n";
}

$return = register_simple('foo', sub { });
if (! $return) {
    print "not ok 3\n";
} else {
    print "ok 3\n";
}

$return = register_pair(0, 'bar', 'baz', sub { });
if (! $return) {
    print "not ok 4\n";
} else {
    print "ok 4\n";
}

$return = $context->set_debug(TEMPLATE_DEBUG_NONE);
if (! $return) {
    print "not ok 5\n";
} else {
    print "ok 5\n";
}

$return = $context->set_debug(TEMPLATE_DEBUG_SOME);
if (! $return) {
    print "not ok 6\n";
} else {
    print "ok 6\n";
}

$return = $context->set_debug(TEMPLATE_DEBUG_LOTS);
if (! $return) {
    print "not ok 7\n";
} else {
    print "ok 7\n";
}

$return = $context->set_value('foo', TEMPLATE_TRUE);
if (! $return) {
    print "not ok 8\n";
} else {
    print "ok 8\n";
}

$return = $context->set_value('foo', TEMPLATE_FALSE);
if (! $return) {
    print "not ok 9\n";
} else {
    print "ok 9\n";
}

$context->destroy();
print "ok 10\n";
