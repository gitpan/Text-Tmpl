use strict;

BEGIN { $^W = 1; $| = 1; print "1..3\n"; }

use Text::Tmpl;

my($return, $subcontext);
my $context = new Text::Tmpl;

$return = $context->set_value( 'var1' => '-' x 40000 );
if (! $return) {
    print "not ok 1\n";
} else {
    print "ok 1\n";
}

$return = $context->set_value( '-' x 40000 => 'value1' );
if (! $return) {
    print "not ok 2\n";
} else {
    print "ok 2\n";
}

$subcontext = $context->loop_iteration( '-' x 40000 );
if (! defined($subcontext)) {
    print "not ok 3\n";
} else {
    print "ok 3\n";
}

$context->destroy();
