use strict;

BEGIN { $^W = 1; $| = 1; print "1..4\n"; }

use IO::File;
use Text::Tmpl;

use constant TEMPLATE => 't/2_ext_pair.tmpl';
use constant COMPARE  => 't/2_ext_pair.comp';

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

$return = $context->register_pair(0, 'poot', 'endpoot', \&tag_pair_poot);
if ($return != 1) {
    print "not ok 1\n";
} else {
    print "ok 1\n";
}

$return = $context->alias_pair('poot', 'endpoot', 'toop', 'endtoop');
if ($return != 1) {
    print "not ok 2\n";
} else {
    print "ok 2\n";
}

$return = $context->alias_pair('comment', 'endcomment', 'foo', '/foo');
if ($return != 1) {
    print "not ok 3\n";
} else {
    print "ok 3\n"
}

$context->set_strip(0);

$output = $context->parse_file(TEMPLATE);

if ($output ne $compare) {
    print "not ok 4\n";
} else {
    print "ok 4\n";
}

Text::Tmpl::destroy($context);

sub tag_pair_poot {
    my($context, $name, @args) = @_;

    $context->set_value('poot', 'pootpoot');

    return;
}
