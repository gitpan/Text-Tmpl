use strict;

BEGIN { $^W = 1; $| = 1; print "1..1\n"; }

use Text::Tmpl;

my($return, $subcontext, $okay);
my $context = new Text::Tmpl;

$subcontext = $context;
$okay = 1;
foreach (1 .. 5000) {
    $subcontext = $subcontext->loop_iteration( 'loop1' );
    if (! defined $subcontext) {
        $okay = 0;
        last;
    }
    $return = $subcontext->set_value( 'value1', $_ );
    if (! $return) {
        $okay = 0;
        last;
    }
}

if ($okay) {
    print "ok 1\n";
} else {
    print "not ok 1\n";
}
