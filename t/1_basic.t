use strict;
use vars qw($loaded);

BEGIN { $^W = 1; $| = 1; print "1..22\n"; }

use Text::Tmpl;

use constant TEMPLATE => 't/1_basic.tmpl';

my($return, $subcontext, $output);
my($context) = Text::Tmpl::init();
if (! defined($context)) {
    print "not ok 1\n";
} else {
    print "ok 1\n";
}

$return = Text::Tmpl::set_delimiters('<%', '%>');
if (! $return) {
    print "not ok 2\n";
} else {
    print "ok 2\n";
}

$return = Text::Tmpl::register_simple('foo', sub { });
if (! $return) {
    print "not ok 3\n";
} else {
    print "ok 3\n";
}

$return = Text::Tmpl::register_pair(0, 'bar', 'baz', sub { });
if (! $return) {
    print "not ok 4\n";
} else {
    print "ok 4\n";
}

$return = $context->set_debug(1);
if (! $return) {
    print "not ok 5\n";
} else {
    print "ok 5\n";
}

$return = $context->set_strip(1);
if (! $return) {
    print "not ok 6\n";
} else {
    print "ok 6\n";
}

$return = $context->set_dir("/");
if (! $return) {
    print "not ok 7\n";
} else {
    print "ok 7\n";
}

$return = $context->set_value('foo', 'bar');
if (! $return) {
    print "not ok 8\n";
} else {
    print "ok 8\n";
}

$return = $context->set_values({'foo' => 'bar',
                                'bar' => 'baz'});
if (! $return) {
    print "not ok 9\n";
} else {
    print "ok 9\n";
}

$subcontext = $context->loop_iteration('foo');
if (! defined $subcontext) {
    print "not ok 10\n";
} else {
    print "ok 10\n";
}

$return = Text::Tmpl::set_debug($context, 1);
if (! $return) {
    print "not ok 11\n";
} else {
    print "ok 11\n";
}

$return = Text::Tmpl::set_strip($context, 1);
if (! $return) {
    print "not ok 12\n";
} else {
    print "ok 12\n";
}

$return = Text::Tmpl::set_dir($context, '/');
if (! $return) {
    print "not ok 13\n";
} else {
    print "ok 13\n";
}

$return = Text::Tmpl::set_value($context, 'bar', 'baz');
if (! $return) {
    print "not ok 14\n";
} else {
    print "ok 14\n";
}

$return = Text::Tmpl::set_values($context, { 'foo' => 'bar',
                                             'bar' => 'baz' });
if (! $return) {
    print "not ok 15\n";
} else {
    print "ok 15\n";
}


$subcontext = Text::Tmpl::loop_iteration($context, 'bar');
if (! defined $subcontext) {
    print "not ok 16\n";
} else {
    print "ok 16\n";
}

Text::Tmpl::destroy($context);
print "ok 17\n";

$context = Text::Tmpl::init();

$output = Text::Tmpl::parse_file($context, TEMPLATE);
if (! defined($output)) {
    print "not ok 18\n";
} else {
    print "ok 18\n";
}

$output = $context->parse_file(TEMPLATE);
if (! defined($output)) {
    print "not ok 19\n";
} else {
    print "ok 19\n";
}

$output = Text::Tmpl::parse_string($context, "this is not a test");
if (! defined($output)) {
    print "not ok 20\n";
} else {
    print "ok 20\n";
}

$output = $context->parse_string("this is not a test");
if (! defined($output)) {
    print "not ok 21\n";
} else {
    print "ok 21\n";
}

$context->destroy();
print "ok 22\n";
