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
	int		debug_level

int
template_set_strip(ctx, strip)
	context_p	ctx
	int		strip

int
template_set_dir(ctx, directory)
	context_p	ctx
	char *		directory

int
template_set_value(ctx, name, value)
	context_p	ctx
	char *		name
	char *		value

void
template_destroy(ctx)
	context_p	ctx

context_p
template_loop_iteration(ctx, loop_name)
	context_p	ctx
	SV *	loop_name
	PREINIT:
		char *r_loop_name = NULL;
	INIT:
		if (loop_name == &sv_undef) {
		    XSRETURN_UNDEF;
		}
		r_loop_name = (char *)SvPV(loop_name, na);
	CODE:
		RETVAL = template_loop_iteration(ctx, r_loop_name);
	OUTPUT:
		RETVAL

SV *
template_parse_file(ctx, template_filename)
	context_p	ctx
	SV *		template_filename
        PREINIT:
		char *output = NULL;
		char *r_template_filename = NULL;
	INIT:
		if (template_filename == &sv_undef) {
		    XSRETURN_UNDEF;
		}
		r_template_filename = (char *)SvPV(template_filename, na);
	CODE:
		template_parse_file(ctx, r_template_filename, &output);
                if (output != NULL) {
                    RETVAL = newSVpv(output, strlen(output));
                    free(output);
                } else {
                    XSRETURN_UNDEF;
                }
	OUTPUT:
		RETVAL

SV *
template_parse_string(ctx, template)
	context_p	ctx
	SV *		template
	PREINIT:
		char *output = NULL;
		char *r_template = NULL;
	INIT:
		if (template == &sv_undef) {
		    XSRETURN_UNDEF;
		}
		r_template = (char *)SvPV(template, na);
	CODE:
		template_parse_string(ctx, r_template, &output);
                if (output != NULL) {
                    RETVAL = newSVpv(output, strlen(output));
                    free(output);
                } else {
                    XSRETURN_UNDEF;
                }
	OUTPUT:
		RETVAL

int
template_register_simple(name, code)
	char *		name
	CV *		code
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
	CV *		code
	PREINIT:
		HV *perl_tag_pairs = perl_get_hv(PERL_TAGS_TAG_PAIR_HASH,
                                                 TRUE);
	CODE:
                hv_store(perl_tag_pairs, open_name,
                         strlen(open_name), newRV((SV*)code), 0);
		RETVAL = template_register_pair((char)named_context,
                                                open_name, close_name,
		                                perl_tag_pair);
	OUTPUT:
		RETVAL

char *
context_get_value(ctx, name)
	context_p	ctx
	char *		name

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

void
context_output_contents(ctx, output_contents)
	context_p	ctx
	int		output_contents
	CODE:
		context_output_contents(ctx, (char)output_contents);
