use strict;

BEGIN { $^W = 1; $| = 1; print "1..3\n"; }

use Text::Tmpl;

my($return, $okay, $loop_number, $subcontext);
my $context = new Text::Tmpl;

$return = $context->set_values({ ( 1 .. 10000 ) });
if (! $return) {
    print "not ok 1\n";
} else {
    print "ok 1\n";
}

$okay = 1;
foreach (1 .. 1000) {
    $subcontext = $context->loop_iteration( 'loop1' );
    if (! defined($subcontext)) {
        $okay = 0;
        last;
    }
}
if (! $okay) {
    print "not ok 2\n";
} else {
    print "ok 2\n";
}

$context->destroy();
print "ok 3\n";
