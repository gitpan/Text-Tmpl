use strict;

BEGIN { $^W = 1; $| = 1; print "1..5\n"; }

use IO::File;
use Text::Tmpl;

use constant TEMPLATE => 't/3_huge_input.tmpl';
use constant COMPARE  => 't/3_huge_input.comp';

my($return, $subcontext, $compare, $output);
my $context = new Text::Tmpl;
if (! defined $context) {
    print "not ok 1\n";
    exit(0);
}

my $comp_fh = new IO::File COMPARE, 'r';
if (! defined $comp_fh) {
    print "not ok 1\n";
    exit(0);
}

{
    local $/ = undef;
    $compare = <$comp_fh>;
}

$comp_fh->close;

$context->set_strip(0);

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

my $thing = qq#
the quick brown fox jumped over the lazy dog the quick brown fox jumped over
the quick brown fox jumped over the lazy dog the quick brown fox jumped over
the quick brown fox jumped over the lazy dog the quick brown fox jumped over
the quick brown fox jumped over the lazy dog the quick brown fox jumped over
the quick brown fox jumped over the lazy dog the quick brown fox jumped over
the quick brown fox jumped over the lazy dog the quick brown fox jumped over
the quick brown fox jumped over the lazy dog the quick brown fox jumped over
the quick brown fox jumped over the lazy dog the quick brown fox jumped over
the quick brown fox jumped over the lazy dog the quick brown fox jumped over
the quick brown fox jumped over the lazy dog the quick brown fox jumped over
the quick brown fox jumped over the lazy dog the quick brown fox jumped over
the quick brown fox jumped over the lazy dog the quick brown fox jumped over
the quick brown fox jumped over the lazy dog the quick brown fox jumped over
the quick brown fox jumped over the lazy dog the quick brown fox jumped over
the quick brown fox jumped over the lazy dog the quick brown fox jumped over
the quick brown fox jumped over the lazy dog the quick brown fox jumped over
the quick brown fox jumped over the lazy dog the quick brown fox jumped over
#;

$return = $context->set_value( 'var1' => $thing );
if (! $return) {
    print "not ok 4\n";
} else {
    print "ok 4\n";
}

$output = $context->parse_file(TEMPLATE);
if (! defined($output)) {
    print "not ok 5\n";
}

if ($output ne $compare) {
    print "not ok 5\n";
} else {
    print "ok 5\n";
}
