use strict;

BEGIN { $^W = 1; $| = 1; print "1..1\n"; }

use IO::File;
use Text::Tmpl;

use constant TEMPLATE => 't/3_multiplicity.tmpl';
use constant COMPARE  => 't/3_multiplicity.comp';

my($compare, $output);

my $context = new Text::Tmpl;
if (! defined $context) {
    print "not ok 1\n";
    exit(0);
}
my $context2 = new Text::Tmpl;
if (! defined $context) {
    print "not ok 1\n";
    exit(0);
}
my $comp_fh = new IO::File COMPARE, 'r';
if (! defined $comp_fh) {
    print "not ok 1\n";
    exit(0);
}

# $context2->destroy();

{
    local $/ = undef;
    $compare = <$comp_fh>;
}

$comp_fh->close;

$context->set_strip(0);
$context->set_value('var1', 'value1');
$context->set_value('var2', 'value2');
$output = $context->parse_file(TEMPLATE);

if ($output ne $compare) {
    print "not ok 1\n";
} else {
    print "ok 1\n";
}
