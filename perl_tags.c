/* ====================================================================
 * Copyright 1999 Web Juice, LLC. All rights reserved.
 *
 * perl_tags.c
 *
 * These are the C functions which bridge the gap between the C template
 * extensions and perl template extensions.
 *
 * ==================================================================== */

#include <EXTERN.h>
#include <perl.h>

#include <stdio.h>
#include <context.h>

#include "perl_tags.h"


/* ====================================================================
 * NAME:          perl_simple_tag
 *
 * DESCRIPTION:   The tag function for perl simple tags.
 *
 * RETURN VALUES: None.
 *
 * BUGS:          Hopefully none.
 * ==================================================================== */
void
perl_simple_tag(context_p ctx, char **output, int argc, char **argv)
{
    HV *perl_simple_tags = perl_get_hv(PERL_TAGS_SIMPLE_TAG_HASH, TRUE);
    int i, n;
    SV **coderef         = hv_fetch(perl_simple_tags, argv[0],
                                    strlen(argv[0]), FALSE);
    SV *perlcontext      = sv_newmortal();
    dSP;
    CV *code;

    if (coderef == NULL)
    {
        *output = NULL;
        return;
    }

    code = (CV *)*coderef;

    ENTER;
    SAVETMPS;

    /* push arguments */
    PUSHMARK(sp);
    /* first the context */
    sv_setref_pv(perlcontext, "context_p", (void *)ctx);
    XPUSHs(perlcontext);
    for (i = 0; i <= argc; i++)
    {
        XPUSHs(sv_2mortal(newSVpv(argv[i], strlen(argv[i]))));
    }

    PUTBACK;

    n = perl_call_sv((SV *)code, G_SCALAR);

    SPAGAIN;

    if (n == 1)
    {
        char *t = POPp;

        *output = (char *)calloc(1, strlen(t) + 1);
        strncpy(*output, t, strlen(t));
        (*output)[strlen(t) + 1] = '\0';
    } else {
        *output = NULL;
    }

    PUTBACK;
    FREETMPS;
    LEAVE;

    return;
}



/* ====================================================================
 * NAME:          perl_tag_pair
 *
 * DESCRIPTION:   The tag pair function for perl tag pairs.
 *
 * RETURN VALUES: None.
 *
 * BUGS:          Hopefully none.
 * ==================================================================== */
void
perl_tag_pair(context_p ctx, int argc, char **argv)
{
    HV *perl_tag_pairs = perl_get_hv(PERL_TAGS_TAG_PAIR_HASH, TRUE);
    int i, n;
    SV **coderef         = hv_fetch(perl_tag_pairs, argv[0],
                                    strlen(argv[0]), FALSE);
    SV *perlcontext      = sv_newmortal();
    dSP;
    CV *code;

    if (coderef == NULL)
    {
        return;
    }

    code = (CV *)*coderef;

    ENTER;
    SAVETMPS;

    /* push arguments */
    PUSHMARK(sp);
    /* first the context */
    sv_setref_pv(perlcontext, "context_p", (void *)ctx);
    XPUSHs(perlcontext);
    for (i = 0; i <= argc; i++)
    {
        XPUSHs(sv_2mortal(newSVpv(argv[i], strlen(argv[i]))));
    }

    PUTBACK;

    n = perl_call_sv((SV *)code, G_DISCARD);

    SPAGAIN;
    PUTBACK;
    FREETMPS;
    LEAVE;

    return;
}
