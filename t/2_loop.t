use strict;

BEGIN { $^W = 1; $| = 1; print "1..1\n"; }

use IO::File;
use Text::Tmpl;

use constant TEMPLATE => 't/2_loop.tmpl';
use constant COMPARE  => 't/2_loop.comp';

my($compare, $output);

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
foreach my $number (0 .. 10) {
    my $subcontext = $context->loop_iteration('loop1');
    $subcontext->set_value('index', $number);
}

{
    my $subcontext = $context->fetch_loop_iteration('loop1', 8);
    $subcontext->set_value('index', 'overwritten');
}

$output = $context->parse_file(TEMPLATE);

if ($output ne $compare) {
    print "not ok 1\n";
} else {
    print "ok 1\n";
}
