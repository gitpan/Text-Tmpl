use strict;

BEGIN { $^W = 1; $| = 1; }

use Text::Tmpl;

use constant MAXVARNAMESIZE => 100;
use constant MAXCONTENTSIZE => 100;

print '1..', MAXVARNAMESIZE, "\n";

foreach my $varnamesize ( 1 .. MAXVARNAMESIZE ) {
    my $varname  = 'a' x $varnamesize;
    my $template = '<!--#loop "l"--><!--#echo $' . $varname
                 . '--><!--#endloop-->';
    my $ctx = new Text::Tmpl;

    foreach my $contentsize ( 1 .. MAXCONTENTSIZE ) {
        my $ictx = $ctx->loop_iteration('l');
        $ictx->set_value($varname, 'v' x $contentsize);
    }

    my $output = $ctx->parse_string($template);

    if ($output ne 'v' x numo(MAXCONTENTSIZE)) {
        print "not ok $varnamesize\n";
    } else {
        print "ok $varnamesize\n";
    }
}

sub numo {
    my $sum = 0;

    foreach ( 1 .. shift ) {
        $sum += $_;
    }
    return $sum;
}
