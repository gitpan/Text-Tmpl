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


staglist_p simple_tags;
tagplist_p tag_pairs;
char otag[TEMPLATE_DELIMITER_SIZE] = "<!--#";
char ctag[TEMPLATE_DELIMITER_SIZE] = "-->";


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

    simple_tags = staglist_init();
    if (simple_tags == NULL)
    {
        return NULL;
    }

    tag_pairs = tagplist_init();
    if (tag_pairs == NULL)
    {
        return NULL;
    }

    if (! staglist_exists(simple_tags, "echo")) {
        template_register_simple("echo",    simple_tag_echo);
    }
    if (! staglist_exists(simple_tags, "include")) {
        template_register_simple("include", simple_tag_include);
    }

    if (! tagplist_is_closetag(tag_pairs, "comment", "endcomment")) {
        template_register_pair(0, "comment", "endcomment", tag_pair_comment);
    }
    if (! tagplist_is_closetag(tag_pairs, "loop", "endloop")) {
        template_register_pair(1, "loop",    "endloop",    tag_pair_loop);
    }
    if (! tagplist_is_closetag(tag_pairs, "if", "endif")) {
        template_register_pair(0, "if",      "endif",      tag_pair_if);
    }
    if (! tagplist_is_closetag(tag_pairs, "ifn", "endifn")) {
        template_register_pair(0, "ifn",     "endifn",     tag_pair_ifn);
    }
    if (! tagplist_is_closetag(tag_pairs, "debug", "enddebug")) {
        template_register_pair(0, "debug",   "enddebug",   tag_pair_debug);
    }

    cwd = getcwd(NULL, MAXPATHLEN);

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
template_set_delimiters(char *opentag, char *closetag)
{
    if ((strlen(opentag) > TEMPLATE_DELIMITER_SIZE)
        || (strlen(closetag) > TEMPLATE_DELIMITER_SIZE))
    {
        return 0;
    }

    strncpy(otag,  opentag, TEMPLATE_DELIMITER_SIZE);
    strncpy(ctag, closetag, TEMPLATE_DELIMITER_SIZE);

    return 1;
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
template_register_simple(char *name,
                         void (*function)(context_p, char **, int, char**))
{
    return(staglist_register(simple_tags, name, function));
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
template_register_pair(char named_context, char *open_name, char *close_name,
                       void (*function)(context_p, int, char**))
{
    return(tagplist_register(tag_pairs, named_context, open_name, close_name,
                             function));
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
        int size = strlen(template_filename)
                 + strlen(context_get_value(ctx, "INTERNAL_dir")) + 1;
        real_filename = (char *)malloc(size);
        snprintf(real_filename, size, "%s%s",
                 context_get_value(ctx, "INTERNAL_dir"), template_filename);
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
    staglist_destroy(simple_tags);
    simple_tags = NULL;
    tagplist_destroy(tag_pairs);
    tag_pairs = NULL;
}
