use strict;

BEGIN { $^W = 1; $| = 1; print "1..1\n"; }

use Text::Tmpl;

use constant TEMPLATE => 't/2_debug.tmpl';

my($output);

my $context = new Text::Tmpl;
if (! defined $context) {
    print "not ok 1\n";
    exit(0);
}

$context->set_strip(0);

$context->set_value('var1', 'value1');
$context->set_value('var2', 'value2');
foreach my $inum (1 .. 2) {
    my $iter = $context->loop_iteration('loop1');
    $iter->set_value('iter' => $inum);
}

$output = $context->parse_file(TEMPLATE);

if (! defined $output) {
    print "not ok 1\n";
} else {
    print "ok 1\n";
}

$context->destroy();
