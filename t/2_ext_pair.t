use strict;

BEGIN { $^W = 1; $| = 1; print "1..1\n"; }

use IO::File;
use Text::Tmpl;

use constant TEMPLATE => 't/2_ext_pair.tmpl';
use constant COMPARE  => 't/2_ext_pair.comp';

my($compare, $output);

my $context = Text::Tmpl::init();
if (! defined $context) {
    print "not ok 1\n";
    exit(0);
}
my $comp_fh = new IO::File COMPARE, "r";
if (! defined $comp_fh) {
    print "not ok 1\n";
    exit(0);
}

{
    local $/ = undef;
    $compare = <$comp_fh>;
}

$comp_fh->close;

Text::Tmpl::register_pair(0, "poot", "endpoot", \&tag_pair_poot);

$context->set_strip(0);

$output = $context->parse_file(TEMPLATE);

if ($output ne $compare) {
    print "not ok 1\n";
} else {
    print "ok 1\n";
}

Text::Tmpl::destroy($context);

sub tag_pair_poot {
    my($context, $name, @args) = @_;

    $context->set_value('poot', 'pootpoot');

    return;
}
