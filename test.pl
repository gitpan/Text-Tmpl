######################### We start with some black magic to print on failure.

use strict;
use vars qw($loaded);

BEGIN { $^W = 1; $| = 1; print "1..9\n"; }
END   {print "not ok 1\n" unless $loaded;}
use Text::Tmpl;
$loaded = 1;
print "ok 1\n";

######################### End of black magic.

my($context) = Text::Tmpl::init();
if (! defined($context)) {
    print "not ok 2\n";
} else {
    print "ok 2\n";
}

if (! $context->set_value('foo' => 'bar')) {
    print "not ok 3\n";
} else {
    print "ok 3\n";
}

my $output = $context->parse_string('<!--#echo $foo-->');
if (! defined($output)) {
    print "not ok 4\n";
} else {
    print "ok 4\n";
}

if ($output ne 'bar') {
    print "not ok 5\n";
} else {
    print "ok 5\n";
}

undef $output;

$output = $context->parse_file('test.tmpl');
if (! defined($output)) {
    print "not ok 6\n";
} else {
    print "ok 6\n";
}

if ($output ne 'bar') {
    print "not ok 7\n";
} else {
    print "ok 7\n";
}

undef $output;

$output = $context->parse_file('nonexistent.tmpl');
if (defined($output)) {
    print "not ok 8\n";
} else {
    print "ok 8\n";
}

Text::Tmpl::destroy($context);
print "ok 9\n";
