use strict;

BEGIN { $^W = 1; $| = 1; print "1..8\n"; }

use Text::Tmpl;

my($return, $subcontext);
my $context = Text::Tmpl::init();

$return = Text::Tmpl::set_delimiters(undef, undef);
if ($return) {
    print "not ok 1\n";
} else {
    print "ok 1\n";
}

$return = Text::Tmpl::register_simple(undef, undef);
if ($return) {
    print "not ok 2\n";
} else {
    print "ok 2\n";
}

$return = Text::Tmpl::register_pair(undef, undef, undef, undef);
if ($return) {
    print "not ok 3\n";
} else {
    print "ok 3\n";
}

$return = $context->set_debug(undef);
if ($return) {
    print "not ok 4\n";
} else {
    print "ok 4\n";
}

$return = $context->set_strip(undef);
if ($return) {
    print "not ok 5\n";
} else {
    print "ok 5\n";
}

$return = $context->set_dir(undef);
if ($return) {
    print "not ok 6\n";
} else {
    print "ok 6\n";
}

$return = $context->set_value(undef, undef);
if ($return) {
    print "not ok 7\n";
} else {
    print "ok 7\n";
}

$subcontext = $context->loop_iteration(undef);
if (defined $subcontext) {
    print "not ok 8\n";
} else {
    print "ok 8\n";
}
