/* ====================================================================
 * Copyright 1999 Web Juice, LLC. All rights reserved.
 *
 * template.c
 *
 * The end-user API for the templating library.
 *
 * ==================================================================== */

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/param.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>

#include <template.h>
#include <default_tags.h>


int  parser(context_p ctx, int looping, char *input, char **output);


/* ====================================================================
 * NAME:          template_init
 *
 * DESCRIPTION:   Initializes the templating library end-user API.  This
 *                includes initializing a context and global tag lists,
 *                and registering all the default tags.
 *
 * RETURN VALUES: If there's a problem, returns NULL; otherwise returns
 *                the main context.
 *
 * BUGS:          Hopefully none.
 * ==================================================================== */
context_p
template_init(void)
{
    char *cwd;
    context_p ctx = context_init();

    if (ctx == NULL)
    {
        return NULL;
    }

    ctx->simple_tags = staglist_init();
    if (ctx->simple_tags == NULL)
    {
        return NULL;
    }

    ctx->tag_pairs = tagplist_init();
    if (ctx->tag_pairs == NULL)
    {
        return NULL;
    }

    template_register_simple(ctx, "echo", simple_tag_echo);
    template_register_simple(ctx, "include", simple_tag_include);

    template_register_pair(ctx, 0, "comment", "endcomment", tag_pair_comment);
    template_register_pair(ctx, 1, "loop",    "endloop",    tag_pair_loop);
    template_register_pair(ctx, 0, "if",      "endif",      tag_pair_if);
    template_register_pair(ctx, 0, "ifn",     "endifn",     tag_pair_ifn);
    template_register_pair(ctx, 0, "debug",   "enddebug",   tag_pair_debug);

    cwd = getcwd(NULL, MAXPATHLEN);

    template_set_value(ctx, "INTERNAL_otag",  "<!--#");
    template_set_value(ctx, "INTERNAL_ctag",  "-->");
    template_set_value(ctx, "INTERNAL_debug", "0");
    template_set_value(ctx, "INTERNAL_strip", "1");
    template_set_value(ctx, "INTERNAL_dir",   cwd);

    free(cwd);

    return(ctx);
}




/* ====================================================================
 * NAME:          template_set_debug
 *
 * DESCRIPTION:   Change the debugging level of this context.
 *
 * RETURN VALUES: 0 if there's a problem; 1 on success.
 *               
 * BUGS:          Hopefully none.
 * ==================================================================== */
int
template_set_debug(context_p ctx, int debug_level)
{
    if (ctx == NULL)
    {
        return(0);
    }

    if (debug_level == TEMPLATE_DEBUG_NONE)
    {
        return(template_set_value(ctx, "INTERNAL_debug", "0"));
    }
    else if (debug_level == TEMPLATE_DEBUG_SOME)
    {
        return(template_set_value(ctx, "INTERNAL_debug", "1"));
    }
    else if (debug_level == TEMPLATE_DEBUG_LOTS)
    {
        return(template_set_value(ctx, "INTERNAL_debug", "2"));
    }
    return(0);
}



/* ====================================================================
 * NAME:          template_set_strip
 *
 * DESCRIPTION:   Modify the behavior of the parser - if strip is on,
 *                the parser will not output the first character after
 *                a tag *if* it is a newline.  If strip is off, the parser
 *                outputs all non-tag characters.
 *
 * RETURN VALUES: 0 if there's a problem; 1 on success.
 *               
 * BUGS:          Hopefully none.
 * ==================================================================== */
int
template_set_strip(context_p ctx, int strip)
{
    if (ctx == NULL)
    {
        return(0);
    }

    if (strip)
    {
        return(template_set_value(ctx, "INTERNAL_strip", "1"));
    }
    else
    {
        return(template_set_value(ctx, "INTERNAL_strip", "0"));
    }
    return(0);
}

/* ====================================================================
 * NAME:          template_set_dir
 *
 * DESCRIPTION:   Set the directory in which templates will be sought
 *                by template_parse_file() and the include tag.
 *
 * RETURN VALUES: 0 if there's a problem; 1 on success.
 *                                                    
 * BUGS:          Hopefully none.
 * ==================================================================== */
int
template_set_dir(context_p ctx, char *directory)
{
    if (ctx == NULL)
    {
        return(0);
    }

    if (directory == NULL)
    {
        return(0);
    }

    return(template_set_value(ctx, "INTERNAL_dir", directory));
}



/* ====================================================================
 * NAME:          template_set_delimiters
 *
 * DESCRIPTION:   Allows the user to set the opening and closing delimiters
 *                for template tags.
 *
 * RETURN VALUES: 0 if there's a problem; 1 on success.
 *
 * BUGS:          Hopefully none.
 * ==================================================================== */
int
template_set_delimiters(context_p ctx, char *opentag, char *closetag)
{
    if ((opentag == NULL) || (closetag == NULL))
    {
        return 0;
    }

    if (ctx == NULL)
    {
        return 0;
    }

    if ((! template_set_value(ctx, "INTERNAL_otag", opentag))
     || (! template_set_value(ctx, "INTERNAL_ctag", closetag)))
    {
        return 0;
    }

    return 1;
}



/* ====================================================================
 * NAME:          template_alias_simple
 *
 * DESCRIPTION:   Copy an existing simple tag to a new tag name.
 *
 * RETURN VALUES: The return of staglist_alias (true or false)
 *
 * BUGS:          Should it verify that the tag name isn't taken as a tag
 *                pair name?
 * ==================================================================== */
int
template_alias_simple(context_p ctx, char *old_name, char *new_name)
{
    context_p current = ctx;

    if (ctx == NULL)
    {
        return 0;
    }

    while (current->parent_context != NULL)
    {
        current = current->parent_context;
    }
    return(staglist_alias(current->simple_tags, old_name, new_name));
}



/* ====================================================================
 * NAME:          template_register_simple
 *
 * DESCRIPTION:   Registers a new simple tag into the global simple tag list.
 *
 * RETURN VALUES: The return of staglist_register (true or false)
 *
 * BUGS:          Should it verify that the tag name isn't taken as a tag
 *                pair name?
 * ==================================================================== */
int
template_register_simple(context_p ctx, char *name,
                         void (*function)(context_p, char **, int, char**))
{
    context_p current = ctx;

    if (ctx == NULL)
    {
        return 0;
    }

    while (current->parent_context != NULL)
    {
        current = current->parent_context;
    }
    return(staglist_register(current->simple_tags, name, function));
}



/* ====================================================================
 * NAME:          template_alias_simple
 *
 * DESCRIPTION:   Copy an existing tag pair to a new tag pair name.
 *
 * RETURN VALUES: The return of tagplist_alias (true or false)
 *
 * BUGS:          Should it verify that the tag name isn't taken as a 
 *                simple tag name?
 * ==================================================================== */
int
template_alias_pair(context_p ctx, char *old_open_name, char *old_close_name,
                    char *new_open_name, char *new_close_name)
{
    context_p current = ctx;

    if (ctx == NULL)
    {
        return 0;
    }

    while (current->parent_context != NULL)
    {
        current = current->parent_context;
    }
    return(tagplist_alias(current->tag_pairs, old_open_name, old_close_name,
                          new_open_name, new_close_name));
}



/* ====================================================================
 * NAME:          template_register_pair
 *
 * DESCRIPTION:   Registers a new tag pair into the global tag pair list.
 *
 * RETURN VALUES: The return of tagplist_register (true or false)
 *
 * BUGS:          Should it verify that the tag name isn't taken as a 
 *                simple tag name?
 * ==================================================================== */
int
template_register_pair(context_p ctx, char named_context, char *open_name,
                       char *close_name,
                       void (*function)(context_p, int, char**))
{
    context_p current = ctx;

    if (ctx == NULL)
    {
        return 0;
    }

    while (current->parent_context != NULL)
    {
        current = current->parent_context;
    }
    return(tagplist_register(current->tag_pairs, named_context, open_name,
                             close_name, function));
}



/* ====================================================================
 * NAME:          template_set_value
 *
 * DESCRIPTION:   Sets a variable within the given context to a new value.
 *
 * RETURN VALUES: The return of context_set_value (true or false)
 *
 * BUGS:          Should it be a macro?
 * ==================================================================== */
int
template_set_value(context_p ctx, char *name, char *value)
{
    return(context_set_value(ctx, name, value));
}



/* ====================================================================
 * NAME:          template_parse_file
 *
 * DESCRIPTION:   Takes a filename and context as input - reads the file,
 *                runs it through parser(), and makes sure that everything
 *                goes okay.
 *
 * RETURN VALUES: Returns 0 if there are any problems with the file or parser,
 *                otherwise returns 1.
 *
 * BUGS:          Hopefully none.
 * ==================================================================== */
int
template_parse_file(context_p ctx, char *template_filename, char **output)
{
    struct stat finfo;
    FILE        *filehandle;
    char        *template;
    char        *real_filename;
    int         retval;

    *output = NULL;

    if ((template_filename == NULL) || (output == NULL))
    {
        return 0;
    }

    if (stat(template_filename, &finfo) != 0)
    {
        char *dir = context_get_value(ctx, "INTERNAL_dir");
        int size  = strlen(template_filename) + strlen(dir) + 2;

        real_filename = (char *)malloc(size);
        if (dir[strlen(dir)] == '/')
        {
            snprintf(real_filename, size, "%s%s", dir, template_filename);
        }
        else
        {
            snprintf(real_filename, size, "%s/%s", dir, template_filename);
        }

        if (stat(real_filename, &finfo) != 0)
        {
            free(real_filename);
            return 0;
        }
    }
    else
    {
        real_filename = (char *)malloc(strlen(template_filename) + 1);
        strcpy(real_filename, template_filename);
    }

    if ((filehandle = fopen(real_filename, "r")) == NULL)
    {
        free(real_filename);
        return 0;
    }

    template = (char *)malloc(finfo.st_size + 1);
    if (template == NULL)
    {
        free(real_filename);
        fclose(filehandle);
        return 0;
    }

    fread(template, 1, finfo.st_size, filehandle);
    template[finfo.st_size] = '\0';

    fclose(filehandle);

    retval = parser(ctx, 1, template, output);
    free(real_filename);
    free(template);

    return(retval);
}



/* ====================================================================
 * NAME:          template_parse_string
 *
 * DESCRIPTION:   Takes an input string and context as input - runs the
 *                input through parser().
 *
 * RETURN VALUES: Returns 0 if the parsing fails, 1 otherwise.
 *
 * BUGS:          Hopefully none.
 * ==================================================================== */
int
template_parse_string(context_p ctx, char *tmpl, char **output)
{
    return(parser(ctx, 1, tmpl, output));
}



/* ====================================================================
 * NAME:          template_destroy
 *
 * DESCRIPTION:   Frees up all the memory used by the templating library.
 *
 * RETURN VALUES: None.
 *
 * BUGS:          Hopefully none.
 * ==================================================================== */
void
template_destroy(context_p ctx)
{
    context_destroy(ctx);
}
