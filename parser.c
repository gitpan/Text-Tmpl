/* ====================================================================
 * Copyright 1999 Web Juice, LLC. All rights reserved.
 *
 * parser.c
 *
 * The parsing bits of the template library.
 *
 * ==================================================================== */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include <template.h>


int  parser(context_p ctx, int looping, char *input, char **output);
void parse_tag(context_p ctx, char *tag, char **tagname,
               int *argc, char ***argv);
void parse_arg(context_p ctx, char *inarg, int size, char **outarg);
void append_output(char **output, char *append, int append_length,
                   int *current_size, int *current_length);


/* ====================================================================
 * NAME:          parser
 *
 * DESCRIPTION:   parses the given input using the current context, and
 *                outputs to a new string pointed to by the output
 *                parameter.
 *
 * RETURN VALUES: Returns 0 if there's a problem, 1 if the parsing was
 *                successful.  The real output goes into the output
 *                string (which the caller is responsible for freeing!)
 *
 * BUGS:          There may be ways of removing some string copying from
 *                this function.  I wasn't able to think of any good ones
 *                that wouldn't make it even more hideous than it already
 *                is.
 * ==================================================================== */
int
parser(context_p ctx, int looping, char *input, char **output)
{
    context_p  current       = ctx;
    int        output_size   = 1024;
    int        output_length = 0;
    int        i;
    int        strip       = ctx_is_strip(ctx);
    char *     otag        = context_get_value(ctx, TMPL_VARNAME_OTAG);
    char *     ctag        = context_get_value(ctx, TMPL_VARNAME_CTAG);
    context_p  rootctx     = ctx;
    staglist_p simple_tags = NULL;
    tagplist_p tag_pairs   = NULL;

    while (rootctx->parent_context != NULL)
    {
        rootctx = rootctx->parent_context;
    }
    simple_tags = rootctx->simple_tags;
    tag_pairs   = rootctx->tag_pairs;

    *output = (char *)malloc(output_size);
    if (*output == NULL)
    {
        template_errno = TMPL_EMALLOC;
        return 0;
    }
    (*output)[0] = '\0';

    if (ctx == NULL)
    {
        template_errno = TMPL_ENULLARG;
        return 0;
    }
    if (input == NULL)
    {
        template_errno = TMPL_ENULLARG;
        return 0;
    }

    do
    {
        char *position = input;
        char *open_tag;
        char *open_tag_name;
        int  open_tag_argc;
        char **open_tag_argv;
        char *tagstart, *tptr;
        int  size;

        /* let's avoid doing any work as long as we can */
        if (! ctx_is_output(current))
        {
            if (looping)
            {
                current = current->next_context;
            }
            continue;
        }

        /* while we can see the beginning of a tag */
        while ((tagstart = strstr(position, otag)) != NULL)
        {
            /* copy the stuff before the tag into the output buffer */
            append_output(output, position,
                          (strlen(position) - strlen(tagstart)), &output_size,
                          &output_length);

            /* now find the end of the tag */
            tptr = strstr(tagstart, ctag);
            if (tptr == NULL)
            {
                /* The tag opened but never closed.  Bummer! */
                template_errno = TMPL_EPARSE;
                return 0;
            }
            /* move the position marker up to the tag's end */
            position = tptr + strlen(ctag);
            if ((strip) && (*position == '\n')) position++;

            /* copy the tag */
            size = strlen(otag);
            open_tag = (char *)malloc(tptr - tagstart - size + 1);
            strncpy(open_tag, tagstart + size,
                    tptr - tagstart - size);
            open_tag[tptr - tagstart - size] = '\0';

            /* parse the tag into parts */
            parse_tag(current, open_tag, &open_tag_name,
                      &open_tag_argc, &open_tag_argv);

            /* deal with the simple tag case */
            if (staglist_exists(simple_tags, open_tag_name))
            {
                char *result;

                if ((staglist_exec(simple_tags, open_tag_name, current, &result,
                                   open_tag_argc, open_tag_argv))
                    && (result != NULL))
                {
                    char *parsed_result = NULL;

                    parser(current, 0, result, &parsed_result);

                    append_output(output, parsed_result, strlen(parsed_result),
                                  &output_size, &output_length);
                    free(result);
                    free(parsed_result);
                }
            /* deal with the tag pair case */
            } else if (tagplist_is_opentag(tag_pairs, open_tag_name))
            {
                int  depth = 1;
                char *subpos = position;
                char *close_tag_name;
                int  length;

                while ((tagstart = strstr(subpos, otag)) != NULL)
                {
                    char *close_tag;

                    /* now find the end of the tag */
                    tptr = strstr(tagstart, ctag);
                    if (tptr == NULL)
                    {
                        /* The tag opened but never closed.  Bummer! */
                        template_errno = TMPL_EPARSE;
                        return 0;
                    }
                    /* move the position marker up to the tag's end */
                    subpos = tptr + strlen(ctag);
                    if ((strip) && (*subpos == '\n')) subpos++;


                    /* figure out the tag name */
                    close_tag  = tagstart + size;
                    close_tag += strspn(close_tag, "\011\012\013\014\015\040");

                    length = strcspn(close_tag, "\011\012\013\014\015\040");
                    if (length > strlen(close_tag) - strlen(tptr))
                    {
                        length = strlen(close_tag) - strlen(tptr);
                    }
                    close_tag_name = (char *)malloc(length + 1);
                    strncpy(close_tag_name, close_tag, length);
                    close_tag_name[length] = '\0';


                    /* if the close tag is the same as the open tag, we're
                       nesting... */
                    if (strcmp(open_tag_name, close_tag_name) == 0)
                    {
                        ++depth;
                    /* if the close tag and open tag form a pair, we're
                       un-nesting... */
                    } else if (tagplist_is_closetag(tag_pairs, open_tag_name,
                                                    close_tag_name))
                    {
                        --depth;
                    }

                    /* if depth is zero, this close tag is *the* close tag. */
                    if (depth == 0)
                    {
                        char *region;
                        context_p newcontext;

                        region = (char *)malloc(tagstart - position + 1);
                        strncpy(region, position, tagstart - position);
                        region[tagstart - position] = '\0';

                        newcontext = tagplist_exec(tag_pairs, open_tag_name,
                                                   current, open_tag_argc,
                                                   open_tag_argv);
                        if (newcontext != NULL)
                        {
                            char *parsed_region = NULL;

                            parser(newcontext, 1, region, &parsed_region);
                            append_output(output, parsed_region,
                                          strlen(parsed_region), &output_size,
                                          &output_length);
                            free(parsed_region);

                            if (ctx_is_anonymous(newcontext))
                            {
                                context_destroy(newcontext);
                            }

                            free(close_tag_name);
                            free(region);

                            break;
                        }

                        free(region);
                    }

                    free(close_tag_name);
                }
                position = subpos;
            }

            for (i = 0; i <= open_tag_argc; i++)
            {
                free(open_tag_argv[i]);
            }
            free(open_tag_argv);
            free(open_tag);
        }

        append_output(output, position, strlen(position), &output_size,
                      &output_length);

        /* done this iteration - move to the next */
        if (looping)
        {
            current = current->next_context;
        }

    } while ((looping) && (current != NULL));

    return 1;
}



/* ====================================================================
 * NAME:          parse_tag
 *
 * DESCRIPTION:   This function takes a single tag (sans tag delimiters)
 *                and a context as input, and outputs the tagname, and
 *                appropriate argc and argv values, to be used for calling
 *                the tag's underlying function.
 *
 * RETURN VALUES: None - all output is stuffed into the output parameters
 *                tagname, argc and argv.
 *
 * BUGS:          Pointer madness with char ***argv.  (There has to be
 *                a better way!)  And the string is parsed character by
 *                character, which might not be the brightest move, but
 *                seemed natural for this application.
 * ==================================================================== */
void
parse_tag(context_p ctx, char *tag, char **tagname, int *argc, char ***argv)
{
    int length;
    char *p, *argbegin;
    char last, instring;
    char **targv;

    *argc = 0;
    targv = (char **)malloc((*argc + 1) * sizeof(char **));

    tag += strspn(tag, "\011\012\013\014\015\040");
    length = strcspn(tag, "\011\012\013\014\015\040");

    targv[0] = (char *)malloc(length + 1);
    strncpy(targv[0], tag, length);
    (targv[0])[length] = '\0';

    *tagname  = targv[0];

    last      = '\0';
    instring  = 0;
    *argc     = 0;
    argbegin  = NULL;
    for (p = (tag + length); *p != (char)NULL; last = *p, ++p)
    {
        if ((! isspace((int)*p)) && (*argc == 0))
        {
            argbegin = p;
            (*argc)++;
            targv = (char **)realloc(targv, (*argc + 1) * (sizeof(char *)));
        }
        if (*p == '"')
        {
            if ((instring) && (last != '\\'))
            {
                instring = 0;
            } else if (! instring)
            {
                instring = 1;
            }
        } else if (*p == ',')
        {
            if (! instring)
            {
                /* parse the current argument string into argv */
                parse_arg(ctx, argbegin, p - argbegin, &targv[*argc]);

                /* point to the next argument string */
                argbegin = p + 1;
                (*argc)++;
                targv = (char **)realloc(targv, (*argc + 1) * (sizeof(char *)));
            }
        }
    }
    if (*argc > 0)
    {
        parse_arg(ctx, argbegin, strlen(argbegin), &targv[*argc]);
    }

    *argv = targv;
}



/* ====================================================================
 * NAME:          parse_arg
 *
 * DESCRIPTION:   Parses a string (inarg) as a single argument.  Does
 *                variable substitution and string concatentation, and
 *                outputs the result into outarg.
 *
 * RETURN VALUES: None - output is placed into outarg.
 *
 * BUGS:          Character by character parsing may be avoidable - not
 *                sure.
 * ==================================================================== */
void
parse_arg(context_p ctx, char *inarg, int size, char **outarg)
{
    char *begin, *p;
    char instring, last;
    int  index, cursize, i;

    i       = 0;
    index   = 0;
    cursize = (size * 2) + 1;
    *outarg = (char *)calloc(1, cursize);

    /* move past leading whitespace */
    for (begin = inarg; isspace((int)*begin); ++begin, ++i) ;

    instring = 0;
    last     = '\0';
    for (p = begin; i <= size; last = *p, p++, i++)
    {
        if (*p == '"')
        {
            --index;
            if (instring)
            {
                if (last == '\\')
                {
                    (*outarg)[index] = '"';
                } else {
                    instring = 0;
                }
            } else if (! instring)
            {
                instring = 1;
            }
        } else if (*p == '$')
        {
            if (instring)
            {
                (*outarg)[index] = *p;
            } else
            {
                int length;
                char *varname, *varvalue;
                char *b = ++p;

                for (++i; ((*p != (char)NULL) && (isalnum((int)*p) || (*p == '_') || (*p == '.'))); p++, i++) ;

                length = p - b;
                varname = (char *)malloc(length + 1);
                strncpy(varname, b, length);
                varname[length] = '\0';

                varvalue = context_get_value(ctx, varname);
                if (varvalue != NULL)
                {
                    length = strlen(varvalue);
                    while ((index + length + 1) > cursize)
                    {
                        char *t;

                        cursize = cursize * 2;
                        t = (char *)calloc(1, cursize);
                        strncpy(t, *outarg, index);

                        free(*outarg);
                        *outarg = t;
                    }

                    strncat(&((*outarg)[index]), varvalue, length);
                    index += length - 1;
                    --p;
                } else
                {
                    --index;
                }
                free(varname);
            }
        } else
        {
            if (instring)
            {
                (*outarg)[index] = *p;
            } else
            {
                --index;
            }
        }


        ++index;
        if ((index + 1) >= cursize)
        {
            char *t;

            if ((index + 1) >= (cursize * 2))
            {
                cursize = index + 1;
            } else
            {
                cursize = cursize * 2;
            }
            t = (char *)malloc(cursize);
            strncpy(t, *outarg, index);
            t[index] = '\0';

            free(*outarg);
            *outarg = t;
        }
    }
}



/* ====================================================================
 * NAME:          append_output
 *
 * DESCRIPTION:   Function used by parser to dynamically expand a string
 *                as needed.  This is really a glorified strncat which
 *                grows the destination string as needed.
 *
 * RETURN VALUES: None, but *output is modified.
 *
 * BUGS:          Hopefully none.
 * ==================================================================== */
void
append_output(char **output, char *append, int append_size, int *current_size,
              int *current_length)
{
    if (((*current_length) + append_size + 1) > *current_size) {
        char *temp;

        if (((*current_length) + append_size + 1) > ((*current_size) * 2))
        {
            *current_size = (*current_length) + append_size + 1;
        } else
        {
            *current_size = (*current_size) * 2;
        }
        temp = (char *)malloc(*current_size);

        strncpy(temp, *output, *current_length);
        temp[*current_length] = '\0';

        free(*output);
        *output = temp;
    }

    strncpy((*output) + (*current_length), append, append_size);
    (*output)[(*current_length) + append_size] = '\0';

    (*current_length) += append_size;
}
