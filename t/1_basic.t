use strict;
use vars qw($loaded);

BEGIN { $^W = 1; $| = 1; print "1..33\n"; }

use Text::Tmpl;

use constant TEMPLATE => 't/1_basic.tmpl';

my($return, $subcontext, $output);
my($context) = Text::Tmpl::init();
if (! defined($context)) {
    print "not ok 1\n";
} else {
    print "ok 1\n";
}

$return = Text::Tmpl::set_delimiters($context, '<%', '%>');
if (! $return) {
    print "not ok 2\n";
} else {
    print "ok 2\n";
}

$return = Text::Tmpl::register_simple($context, 'foo', sub { });
if (! $return) {
    print "not ok 3\n";
} else {
    print "ok 3\n";
}

$return = $context->register_simple('foo', sub { });
if (! $return) {
    print "not ok 4\n";
} else {
    print "ok 4\n";
}

$return = Text::Tmpl::register_pair($context, 0, 'bar', 'baz', sub { });
if (! $return) {
    print "not ok 5\n";
} else {
    print "ok 5\n";
}

$return = $context->register_pair(0, 'bar', 'baz', sub { });
if (! $return) {
    print "not ok 6\n";
} else {
    print "ok 6\n";
}

$return = Text::Tmpl::alias_simple($context, 'foo', 'bar');
if (! $return) {
    print "not ok 7\n";
} else {
    print "ok 7\n";
}

$return = $context->alias_simple('foo', 'bar');
if (! $return) {
    print "not ok 8\n";
} else {
    print "ok 8\n";
}

$return = Text::Tmpl::alias_pair($context, 'bar', 'baz', 'barney', 'fred');
if (! $return) {
    print "not ok 9\n";
} else {
    print "ok 9\n";
}

$return = $context->alias_pair('bar', 'baz', 'barney', 'fred');
if (! $return) {
    print "not ok 10\n";
} else {
    print "ok 10\n";
}

$context->set_debug(1);
print "ok 11\n";

$context->set_strip(1);
print "ok 12\n";

$return = $context->set_dir('/');
if (! $return) {
    print "not ok 13\n";
} else {
    print "ok 13\n";
}

$return = $context->set_value('foo', 'bar');
if (! $return) {
    print "not ok 14\n";
} else {
    print "ok 14\n";
}

$return = $context->set_values({'foo' => 'bar',
                                'bar' => 'baz'});
if (! $return) {
    print "not ok 15\n";
} else {
    print "ok 15\n";
}

$subcontext = $context->loop_iteration('foo');
if (! defined $subcontext) {
    print "not ok 16\n";
} else {
    print "ok 16\n";
}

$subcontext = $context->fetch_loop_iteration('foo', 0);
if (! defined $subcontext) {
    print "not ok 17\n";
} else {
    print "ok 17\n";
}

Text::Tmpl::set_debug($context, 1);
print "ok 18\n";

Text::Tmpl::set_strip($context, 1);
print "ok 19\n";

$return = Text::Tmpl::set_dir($context, '/');
if (! $return) {
    print "not ok 20\n";
} else {
    print "ok 20\n";
}

$return = Text::Tmpl::set_value($context, 'bar', 'baz');
if (! $return) {
    print "not ok 21\n";
} else {
    print "ok 21\n";
}

$return = Text::Tmpl::set_values($context, { 'foo' => 'bar',
                                             'bar' => 'baz' });
if (! $return) {
    print "not ok 22\n";
} else {
    print "ok 22\n";
}


$subcontext = Text::Tmpl::loop_iteration($context, 'bar');
if (! defined $subcontext) {
    print "not ok 23\n";
} else {
    print "ok 23\n";
}

$subcontext = Text::Tmpl::fetch_loop_iteration($context, 'bar', 0);
if (! defined $subcontext) {
    print "not ok 24\n";
} else {
    print "ok 24\n";
}

$context = new Text::Tmpl;
if (! defined($context)) {
    print "not ok 25\n";
} else {
    print "ok 25\n";
}

$output = Text::Tmpl::parse_file($context, TEMPLATE);
if (! defined($output)) {
    print "not ok 26\n";
} else {
    print "ok 26\n";
}

$output = $context->parse_file(TEMPLATE);
if (! defined($output)) {
    print "not ok 27\n";
} else {
    print "ok 27\n";
}

$output = Text::Tmpl::parse_string($context, 'this is not a test');
if (! defined($output)) {
    print "not ok 28\n";
} else {
    print "ok 28\n";
}

$output = $context->parse_string('this is not a test');
if (! defined($output)) {
    print "not ok 29\n";
} else {
    print "ok 29\n";
}

$return = $context->set_delimiters('<%', '%>');
if (! $return) {
    print "not ok 30\n";
} else {
    print "ok 30\n";
}

$return = Text::Tmpl::errno();
if (! defined $return) {
    print "not ok 31\n";
} else {
    print "ok 31\n";
}

$return = Text::Tmpl::strerror();
if (! $return) {
    print "not ok 32\n";
} else {
    print "ok 32\n";
}

$context->destroy();
print "ok 33\n";
