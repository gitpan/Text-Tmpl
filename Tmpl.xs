#include "EXTERN.h"
#include "perl.h"
#include "XSUB.h"

#include "template.h"
#include "perl_tags.h"

MODULE = Text::Tmpl PACKAGE = Text::Tmpl PREFIX = template_
PROTOTYPES: ENABLE


context_p
template_init()

int
template_set_delimiters(opentag, closetag)
	char *	opentag
	char *	closetag

int
template_set_debug(ctx, debug_level)
	context_p	ctx
	int	debug_level

int
template_set_strip(ctx, strip)
	context_p	ctx
	int	strip

int
template_set_dir(ctx, directory)
	context_p	ctx
	char *	directory

int
template_set_value(ctx, name, value)
	context_p	ctx
	char *	name
	char *	value

void
template_destroy(ctx)
	context_p	ctx

context_p
template_loop_iteration(ctx, loop_name)
	context_p	ctx
	char *	loop_name

SV *
template_parse_file(ctx, template_filename)
	context_p	ctx
	char *		template_filename
        PREINIT:
		char *output = NULL;
	CODE:
		template_parse_file(ctx, template_filename, &output);
                if (output != NULL) {
                    RETVAL = newSVpv(output, strlen(output));
                    free(output);
                } else {
                    ST(0) = &sv_undef;
                    XSRETURN(1);
                }
	OUTPUT:
		RETVAL

SV *
template_parse_string(ctx, template)
	context_p	ctx
	char *		template
	PREINIT:
		char *output = NULL;
	CODE:
		template_parse_string(ctx, template, &output);
                if (output != NULL) {
                    RETVAL = newSVpv(output, strlen(output));
                    free(output);
                } else {
                    ST(0) = &sv_undef;
                    XSRETURN(1);
                }
	OUTPUT:
		RETVAL

int
template_register_simple(name, code)
	char *		name
	CV   *          code
	PREINIT:
		HV *perl_simple_tags = perl_get_hv(PERL_TAGS_SIMPLE_TAG_HASH,
                                                   TRUE);
	CODE:
                hv_store(perl_simple_tags, name, strlen(name),
                         newRV((SV*)code), 0);
		RETVAL = template_register_simple(name, perl_simple_tag);
	OUTPUT:
		RETVAL

int
template_register_pair(named_context, open_name, close_name, code)
	int		named_context
	char *		open_name
	char *		close_name
	CV   *		code
	PREINIT:
		HV *perl_tag_pairs = perl_get_hv(PERL_TAGS_TAG_PAIR_HASH,
                                                 TRUE);
	CODE:
                hv_store(perl_tag_pairs, open_name,
                         strlen(open_name), newRV((SV*)code), 0);
		RETVAL = template_register_pair((char)named_context, open_name,
						close_name, perl_tag_pair);
	OUTPUT:
		RETVAL

context_p
context_get_anonymous_child(ctx)
	context_p	ctx

context_p
context_get_named_child(ctx, name)
	context_p	ctx
	char *		name

int
context_set_named_child(ctx, name)
	context_p	ctx
	char *		name

context_p
context_add_peer(ctx)
	context_p	ctx

void *
context_output_contents(ctx, output_contents)
	context_p	ctx
	int		output_contents
	CODE:
		context_output_contents(ctx, (char)output_contents);
