use strict;

BEGIN { $^W = 1; $| = 1; print "1..4\n"; }

use IO::File;
use Text::Tmpl;

use constant TEMPLATE => 't/2_ext_simple.tmpl';
use constant COMPARE  => 't/2_ext_simple.comp';

my($return, $compare, $output);

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

$return = $context->register_simple('poot', \&simple_tag_poot);
if ($return != 1) {
    print "not ok 1\n";
} else {
    print "ok 1\n";
}

$return = $context->alias_simple('poot', 'toop');
if ($return != 1) {
    print "not ok 2\n";
} else {
    print "ok 2\n";
}

$return = $context->alias_simple('echo', '=');
if ($return != 1) {
    print "not ok 3\n";
} else {
    print "ok 3\n";
}

$context->set_strip(0);

$output = $context->parse_file(TEMPLATE);

if ($output ne $compare) {
    print "not ok 4\n";
} else {
    print "ok 4\n";
}

sub simple_tag_poot {
    my($context, $name, @args) = @_;

    return 'pootpoot';
}
