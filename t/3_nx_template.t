use strict;

BEGIN { $^W = 1; $| = 1; print "1..3\n"; }

use Text::Tmpl;

my $context = new Text::Tmpl;

my $output = $context->parse_file('nonexistent.tmpl');
if (defined $output) {
    print "not ok 1\n";
} else {
    print "ok 1\n";
}

undef $output;

$output = $context->parse_file(undef);
if (defined $output) {
    print "not ok 2\n";
} else {
    print "ok 2\n";
}

undef $output;

$output = $context->parse_string(undef);
if (defined $output) {
    print "not ok 3\n";
} else {
    print "ok 3\n";
}

Text::Tmpl::destroy($context);
