/* ====================================================================
 * Copyright 1999 Web Juice, LLC. All rights reserved.
 *
 * default_tags.c
 *
 * The functions for the default tags shipped with the template library:
 * echo, include, comment/endcomment, if/endif, ifn/endifn, loop/endloop.
 *
 * ==================================================================== */

#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include <template.h>
#include <context.h>
#include <varlist.h>
#include <nclist.h>
#include <default_tags.h>



/* ====================================================================
 * NAME:          tag_pair_debug
 *  
 * DESCRIPTION:   The tag pair function for debug
 *
 * RETURN VALUES: None.
 *
 * BUGS:          Hopefully none.
 * ==================================================================== */
void
tag_pair_debug(context_p ctx, int argc, char **argv)
{
    context_p top_ctx = ctx;

    /* rewind to the top of the context hierarchy */
    while (top_ctx->parent_context != NULL)
    {
        top_ctx = top_ctx->parent_context;
    }

    dump_context(ctx, top_ctx, 0);
}



/* ====================================================================
 * NAME:          tag_pair_if
 *
 * DESCRIPTION:   The tag pair function for if/endif
 *
 * RETURN VALUES: None.
 *
 * BUGS:          Hopefully none.
 * ==================================================================== */
void
tag_pair_if(context_p ctx, int argc, char **argv)
{
    if (argc != 1)
    {
        return;
    }

    context_output_contents(ctx, string_truth(argv[1]));
    return;
}



/* ====================================================================
 * NAME:          tag_pair_ifn
 *
 * DESCRIPTION:   The tag pair function for ifn/endifn
 *
 * RETURN VALUES: None.
 *
 * BUGS:          Hopefully none.
 * ==================================================================== */
void
tag_pair_ifn(context_p ctx, int argc, char **argv)
{
    if (argc != 1)
    {
        return;
    }

    context_output_contents(ctx, (! string_truth(argv[1])));
    return;
}



/* ====================================================================
 * NAME:          tag_pair_loop
 *
 * DESCRIPTION:   The tag pair function for loop/endloop
 *
 * RETURN VALUES: None.
 *
 * BUGS:          Hopefully none.
 * ==================================================================== */
void
tag_pair_loop(context_p ctx, int argc, char **argv)
{
    return;
}



/* ====================================================================
 * NAME:          tag_pair_comment
 *
 * DESCRIPTION:   The tag pair function for comment/endcomment
 *
 * RETURN VALUES: None.
 *
 * BUGS:          Hopefully none.
 * ==================================================================== */
void
tag_pair_comment(context_p ctx, int argc, char **argv)
{
    context_output_contents(ctx, 0);
    return;
}



/* ====================================================================
 * NAME:          simple_tag_echo
 *
 * DESCRIPTION:   The tag function for echo
 *
 * RETURN VALUES: None.
 *
 * BUGS:          Hopefully none.
 * ==================================================================== */
void
simple_tag_echo(context_p ctx, char **output, int argc, char **argv)
{
    int size;

    if (argc < 1)
    {
        *output = NULL;
        return;
    }

    size = strlen(argv[1]);
    *output = (char *)malloc(size + 1);
    strncpy(*output, argv[1], size);
    (*output)[size] = '\0';

    return;
}



/* ====================================================================
 * NAME:          simple_tag_include
 *
 * DESCRIPTION:   The tag function for include
 *
 * RETURN VALUES: None.
 *
 * BUGS:          Hopefully none.
 * ==================================================================== */
void
simple_tag_include(context_p ctx, char **output, int argc, char **argv)
{
    struct stat finfo;
    FILE        *filehandle;
    char        *filename;

    if (argc != 1)
    {
        *output = NULL;
        return;
    }

    if (stat(argv[1], &finfo) != 0)
    {
        char *dir = context_get_value(ctx, "INTERNAL_dir");
        int size  = strlen(argv[1]) + strlen(dir) + 2;

        filename = (char *)malloc(size);
        if (dir[strlen(dir)] == '/')
        {
            snprintf(filename, size, "%s%s", dir, argv[1]);
        }
        else
        {
            snprintf(filename, size, "%s/%s", dir, argv[1]);
        }

        if (stat(filename, &finfo) != 0)
        {
            free(filename);
            *output = NULL;
            return;
        }
    }    
    else
    {   
        filename = (char *)malloc(strlen(argv[1]) + 1);
        strcpy(filename, argv[1]);
    }

    if ((filehandle = fopen(filename, "r")) == NULL)
    {
        free(filename);
        *output = NULL;
        return;
    }

    *output = (char *)malloc(finfo.st_size + 1);
    if (*output == NULL)
    {
        free(filename);
        return;
    }

    fread(*output, 1, finfo.st_size, filehandle);
    (*output)[finfo.st_size] = '\0';

    fclose(filehandle);

    free(filename);
    return;
}



/* ====================================================================
 * NAME:          template_loop_iteration
 *
 * DESCRIPTION:   This function pre-builds loops via named contexts.
 *                It is the end-user method for building loops.
 *
 * RETURN VALUES: Returns a context for this iteration.
 *
 * BUGS:          Hopefully none.
 * ==================================================================== */
context_p
template_loop_iteration(context_p ctx, char *loop_name)
{
    context_p return_context;

    if (ctx == NULL)
    {
        return NULL;
    }

    return_context = context_get_named_child(ctx, loop_name);
    if (return_context == NULL)
    {
        context_set_named_child(ctx, loop_name);
        return_context = context_get_named_child(ctx, loop_name);
        return(return_context);
    }
    return(context_add_peer(return_context));
}



/* ====================================================================
 * NAME:          string_truth
 *     
 * DESCRIPTION:   Decides whether an input string is "true" or not.
 *            
 * RETURN VALUES: 0 or 1, a boolean value for the input string.
 *              
 * BUGS:          Hopefully none.
 * ==================================================================== */
char string_truth(char *input)                                        
{                            
    if (input == NULL)
    {                
        return 0;
    }           
    if (atoi(input))
    {              
        return 1;
    }           
    return 0;
}



/* ====================================================================
 * NAME:          dump_context
 * 
 * DESCRIPTION:   Dumps all of the data from a given context into the
 *                namespace of another context.
 *
 * RETURN VALUES: None.
 *
 * BUGS:          Hopefully none.
 * ==================================================================== */
void
dump_context(context_p ctx, context_p dump_ctx, int number)
{
    varlist_p current_var = dump_ctx->variables;
    nclist_p  current_nc  = dump_ctx->named_child_contexts;
    char *number_s;
    char *var_loop_name;
    char *nc_loop_name;
    int size;

    size = (number / 10) + 1;
    number_s = (char *)malloc(size + 1);
    snprintf(number_s, size + 1, "%d", number);
    number_s[size] = '\0';

    size = strlen(number_s) + strlen("variables-");
    var_loop_name = (char *)malloc(size + 1);
    snprintf(var_loop_name, size + 1, "variables-%s", number_s);
    var_loop_name[size] = '\0';

    size = strlen(number_s) + strlen("named_children-");
    nc_loop_name = (char *)malloc(size + 1);
    snprintf(nc_loop_name, size + 1, "named_children-%s", number_s);
    nc_loop_name[size] = '\0';

    template_set_value(ctx, "number", number_s);
    
    /* dump variables into a "variables" loop */
    while ((current_var != NULL) && (current_var->name != NULL))
    {
        context_p iteration = template_loop_iteration(ctx, var_loop_name);
        
        template_set_value(iteration, "variable_name",  current_var->name);
        template_set_value(iteration, "variable_value", current_var->value);

        current_var = current_var->next;
    }

    /* dump loops into a "named_children" loop */
    while ((current_nc != NULL) && (current_nc->context != NULL))
    {
        context_p iteration = template_loop_iteration(ctx, nc_loop_name);

        template_set_value(iteration, "nc_name", current_nc->name);
        context_set_named_child(iteration, current_nc->name);

        dump_context(context_get_named_child(iteration, current_nc->name),
                     current_nc->context, number + 1);

        current_nc = current_nc->next;
    }

    /* if this is a loop, add an iteration to the ouput context and dump
     * into it */
    if (dump_ctx->next_context != NULL)
    {
        dump_context(context_add_peer(ctx), dump_ctx->next_context,
                     number + 1);
    }

    free(number_s);
    free(var_loop_name);
    free(nc_loop_name);

    return;
}
