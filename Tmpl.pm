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

$VERSION = '0.12';

bootstrap Text::Tmpl $VERSION;

###############################################################################
## set_values
##   Purpose:
##     Load a hash table into a template
##   Usage:
##     $context->set_values({ 'var1' => 'value1',
##                            'var2' => 'value2' });
##   Return Values:
##     Returns 1 on success, 0 on failure.
###############################################################################
sub set_values {
    my $self = shift;
    if (! ref $self) {
        return 0;
    }
    my($hashref) = shift;
    if ((ref $hashref) ne 'HASH') {
        return 0;
    }

    foreach my $key (keys %{$hashref}) {
        my $ret = $self->set_value($key, $hashref->{$key});
        if (! $ret) {
            return 0;
        }
    }
    return 1;
}
### end set_values ############################################################

1;

__END__

=head1 NAME

Text::Tmpl - Templating system perl library

=head1 SYNOPSIS

B<use Text::Tmpl;>

$context    = B<Text::Tmpl::init>();

$return     = B<Text::Tmpl::set_delimiters>($opentag,
                 $closetag);

$return     = B<Text::Tmpl::register_simple>($name,
                 $coderef);

$return     = B<Text::Tmpl::register_pair>($isnamed,
                 $open_name, $close_name, $coderef);

$return     = $context->B<set_debug>($debug_level);

$return     = $context->B<set_strip>($to_strip_or_not_to);

$return     = $context->B<set_dir>($directory);

$return     = $context->B<set_value>($name, $value);

$return     = $context->B<set_values>($hashref);

$subcontext = $context->B<loop_iteration>($loop_name);

$output     = $context->B<parse_file>($template_filename);

$output     = $context->B<parse_string>($template);

$context->B<destroy>();

=head1 DESCRIPTION

=head2 Design goals

simplicity, reusability, speed, complete separation of logic from
formatting.

=head2 Feature set

variables, loops, conditionals, extensibility of tags, includes,
arbitrary delimiters.

=head2 Usage

For starters, make sure you 'use Text::Tmpl'.

Each function is described below:

=over 3

=item B<Text::Tmpl::init>

This function initializes the library.  It allocates and returns the "global"
context structure, and also configures all of the default tag behavior.

=item B<Text::Tmpl::set_delimiters>

This function lets you change the delimiters marking the beginning and end
of a tag (by default, these are "<!--#" and "-->".  There is an arbitrary
limit of 6 characters on these strings.

=item B<Text::Tmpl::set_value>

This function stores the name=value pair in the current context.

=item B<Text::Tmpl::set_values>

This function dumps the name=value pairs from a hash reference into the
current context.

=item B<Text::Tmpl::set_debug>

This function sets the debugging level (from 0 = silent to 2 = verbose).  Note
that debugging output hasn't been written yet - this is just a placeholder.

=item B<Text::Tmpl::set_strip>

This function enables or disables the newline stripping feature.  If enabled,
the parser removes a single newline (if present) from after any tag.

=item B<Text::Tmpl::set_dir>

This function sets the directory where templates will be sought, both by
parse_file and by the include tag.  Search order is always current directory
then this searched directory.

=item B<Text::Tmpl::loop_iteration>

This function adds an iteration to the loop named loop_name, and returns
a unique context for that loop iteration.

=item B<Text::Tmpl::parse_file>

This function opens $template_filename, and parses the contents of
that file as a template, returning the output.

=item B<Text::Tmpl::parse_string>

This function parses template directly, in the same way that
Text::Tmpl::parse_file does.

=item B<Text::Tmpl::register_simple>

This function registers a new simple tag named $name, which when encountered
will cause the parser to call $coderef.  See template_extend(1) for the
gory details.

=item B<Text::Tmpl::register_pair>

This function registers a new tag pair $open_name/$close_name, which when
encountered will cause the parser to call $coderef.  See template_extend
for the gory details.

=item B<Text::Tmpl::destroy>

This function blows away all of the memory allocated within the given context.
You should really *only* call this on the context returned by
Text::Tmpl::init, and only at the end of your code.

=back

=head1 RETURN VALUES

All of the above functions which return numeric values will return 0 if they
fail, or 1 otherwise.  The ones which return contexts will return undef if
they fail, or a valid pointer otherwise.

=head1 BUGS

Hopefully none.

=head1 AUTHOR

J. David Lowe, dlowe@pootpoot.com

=head1 SEE ALSO

libtmpl(1), template_syntax(1), template_extend(1)

=cut
