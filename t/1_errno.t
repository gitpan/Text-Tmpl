use strict;
use vars qw($loaded);

BEGIN { $^W = 1; $| = 1; print "1..6\n"; }

use Text::Tmpl;

my($return, $errno, $errstr);
my($context) = Text::Tmpl::init();
if (! defined($context)) {
    print "not ok 1\n";
} else {
    print "ok 1\n";
}

$return = $context->parse_file('nonexistent.tmpl');
$errno  = Text::Tmpl::errno();
$errstr = Text::Tmpl::strerror();
if (($return) || ($errno != 8) || ($errstr ne 'file not found')) {
    print "not ok 2\n";
} else {
    print "ok 2\n";
}

$return = $context->context_get_value('nonexistent');
$errno  = Text::Tmpl::errno();
$errstr = Text::Tmpl::strerror();
if (($return) || ($errno != 4) || ($errstr ne 'no such variable')) {
    print "not ok 3\n";
} else {
    print "ok 3\n";
}

$return = $context->context_get_named_child('nonexistent');
$errno  = Text::Tmpl::errno();
$errstr = Text::Tmpl::strerror();
if (($return) || ($errno != 5) || ($errstr ne 'no such named context')) {
    print "not ok 4\n";
} else {
    print "ok 4\n";
}

$return = $context->parse_string('<!--#loop "unbalanced"');
$errno  = Text::Tmpl::errno();
$errstr = Text::Tmpl::strerror();
if (($errno != 10) || ($errstr ne 'unable to parse')) {
    print "not ok 5\n";
} else {
    print "ok 5\n";
}

$context->destroy();
print "ok 6\n";
