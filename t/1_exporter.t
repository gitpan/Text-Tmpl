use strict;
use vars qw($loaded);

BEGIN { $^W = 1; $| = 1; print "1..4\n"; }
 
use Text::Tmpl qw(TEMPLATE_TRUE
                  TEMPLATE_FALSE);

my $return;
my($context) = new Text::Tmpl;
if (! defined($context)) {
    print "not ok 1\n";
} else {
    print "ok 1\n";
}

$return = $context->set_value('foo', TEMPLATE_TRUE);
if (! $return) {
    print "not ok 2\n";
} else {
    print "ok 2\n";
}

$return = $context->set_value('foo', TEMPLATE_FALSE);
if (! $return) {
    print "not ok 3\n";
} else {
    print "ok 3\n";
}

$context->destroy();
print "ok 4\n";
