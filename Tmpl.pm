package Text::Tmpl;

use strict;
use Carp;
use vars qw($VERSION @ISA $AUTOLOAD);

use constant TEMPLATE_DEBUG_NONE => 0;
use constant TEMPLATE_DEBUG_SOME => 1;
use constant TEMPLATE_DEBUG_LOTS => 2;

use constant TEMPLATE_TRUE  => "1";
use constant TEMPLATE_FALSE => "0";

require DynaLoader;
require AutoLoader;

@ISA = qw(DynaLoader);

$VERSION = '0.03';

bootstrap Text::Tmpl $VERSION;

1;

__END__
