/* ====================================================================
 * Copyright 1999 Web Juice, LLC. All rights reserved.
 *
 * tagplist.c
 *
 * Functions for manipulating the tag pair list structure in the template
 * library.
 *
 * ==================================================================== */

#include <stdlib.h>
#include <string.h>

#include <context.h>
#include <tagplist.h>


/* ====================================================================
 * NAME:          tagplist_init
 *
 * DESCRIPTION:   Initializes and returns a pointer to a new tag pair
 *                list structure.
 *
 * RETURN VALUES: Returns NULL if the memory allocation fails; otherwise
 *                returns a pointer to a tag pair list structure.
 *
 * BUGS:          Hopefully none.
 * ==================================================================== */
tagplist_p
tagplist_init()
{
    tagplist_p tag_pair_list;

    tag_pair_list = (tagplist_p)calloc(1, sizeof(tagplist));
    if (tag_pair_list == NULL)
    { 
        return NULL;
    }

    tag_pair_list->open_name     = NULL;
    tag_pair_list->close_name    = NULL;
    tag_pair_list->function      = NULL;
    tag_pair_list->next          = NULL;
    tag_pair_list->named_context = 0;

    return(tag_pair_list);
}



/* ====================================================================
 * NAME:          tagplist_destroy
 *
 * DESCRIPTION:   Frees up all memory associated with a tag pair list.
 *
 * RETURN VALUES: None.
 *
 * BUGS:          Because a free()d pointer still *looks* valid, it is
 *                difficult to protect against the problems that arise
 *                if the user calls this function too early.
 * ==================================================================== */
void
tagplist_destroy(tagplist_p tag_pair_list)
{
    tagplist_p next;

    if (tag_pair_list == NULL)
    {
        return;
    }

    next = tag_pair_list->next;

    tag_pair_list->next = NULL;
    if (tag_pair_list->open_name != NULL)
    {
        free(tag_pair_list->open_name);
    }
    if (tag_pair_list->close_name != NULL)
    {
        free(tag_pair_list->close_name);
    }
    free(tag_pair_list);

    tagplist_destroy(next);
}



/* ====================================================================
 * NAME:          tagplist_register
 *
 * DESCRIPTION:   Register a new tag pair and associated function to call.
 *
 * RETURN VALUES: Returns 0 upon failure; 1 on success.
 *
 * BUGS:          Hopefully none.
 * ==================================================================== */
int
tagplist_register(tagplist_p tag_pair_list, char named_context, char *open_name,
                  char *close_name, void (*function) (context_p, int, char**))
{
    tagplist_p current  = tag_pair_list;
    tagplist_p existing = NULL;
    tagplist_p last     = NULL;

    /* Make sure the function isn't NULL */
    if (function == NULL)
    {
        return 0;
    }

    /* Walk through the whole list, marking any pre-existing tag of
       this name, and marking the last element in the list */
    while (current != NULL)
    {
        if ((current->open_name != NULL) && (current->close_name != NULL)
           && (strcmp(current->open_name, open_name) == 0)
           && (strcmp(current->close_name, close_name) == 0))
        {
            existing = current;
        }
        last    = current;
        current = current->next;
    }

    /* If the tag doesn't already exist, we may have to create a new list
       element */
    if (existing == NULL)
    {
        if (last->function == NULL)
        {
            existing   = last;
        } else {
            last->next = tagplist_init();
            existing   = last->next;

            if (existing == NULL)
            {
                return 0;
            }
        }
        existing->open_name = (char *)malloc(strlen(open_name) + 1);
        strncpy(existing->open_name, open_name, strlen(open_name));
        existing->open_name[strlen(open_name)] = '\0';

        existing->close_name = (char *)malloc(strlen(close_name) + 1);
        strncpy(existing->close_name, close_name, strlen(close_name));
        existing->close_name[strlen(close_name)] = '\0';
    }
    existing->function      = function;
    existing->named_context = named_context;

    return 1;
}



/* ====================================================================
 * NAME:          tagplist_is_opentag
 *
 * DESCRIPTION:   Find out whether a particular tag is a legitimate
 *                opening tag.
 *
 * RETURN VALUES: Returns 0 if there's a problem or if the name is not
 *                a valid opening tag, or 1 if the name is a valid tag.
 *
 * BUGS:          Hopefully none.
 * ==================================================================== */
int
tagplist_is_opentag(tagplist_p tag_pair_list, char *open_name)
{
    tagplist_p current  = tag_pair_list;

    while (current != NULL)
    {
        if ((current->open_name != NULL) && (current->function != NULL)
            && (strcmp(current->open_name, open_name) == 0))
        {
            return 1;
        }
        current = current->next;
    }

    return 0;
}



/* ====================================================================
 * NAME:          tagplist_is_closetag
 *
 * DESCRIPTION:   Find out whether a particular tag is a valid closing
 *                tag for another (i.e. whether you have a valid tag pair).
 *
 * RETURN VALUES: Returns 0 if there's a problem or if the names do not
 *                form a valid tag pair; otherwise returns 1.
 *
 * BUGS:          Hopefully none.
 * ==================================================================== */
int
tagplist_is_closetag(tagplist_p tag_pair_list, char *open_name,
                     char *close_name)
{
    tagplist_p current  = tag_pair_list;

    while (current != NULL)
    {
        if ((current->open_name != NULL) && (current->close_name != NULL)
            && (strcmp(current->open_name, open_name) == 0)
            && (strcmp(current->close_name, close_name) == 0))
        {
            return 1;
        }
        current = current->next;
    }

    return 0;
}



/* ====================================================================
 * NAME:          tagplist_exec
 *
 * DESCRIPTION:   Executes the function associated with a given tag pair.
 *
 * RETURN VALUES: Returns NULL if there's a problem or the context
 *                modified by the function otherwise.
 *
 * BUGS:          Hopefully none.
 * ==================================================================== */
context_p
tagplist_exec(tagplist_p tag_pair_list, char *open_name, context_p ctx,
              int argc, char **argv)
{
    tagplist_p current = tag_pair_list;

    if (tag_pair_list == NULL)
    {
        return NULL;
    }

    while (current != NULL)
    {
        if ((current->open_name != NULL) && (current->function != NULL)
            && (strcmp(current->open_name, open_name) == 0))
        {
            if ((current->named_context) && (argc >= 0))
            {
                context_p named_context;

                named_context = context_get_named_child(ctx, argv[1]);
                if (named_context == NULL)
                {
                    named_context = context_get_anonymous_child(ctx);
                    named_context->output_contents = 0;
                    return(named_context);
                }

                current->function(named_context, argc, argv);

                return(named_context);
            } else {
                context_p anonymous_context;

                anonymous_context = context_get_anonymous_child(ctx);
                if (anonymous_context == NULL)
                {
                    return NULL;
                }

                current->function(anonymous_context, argc, argv);
                return(anonymous_context);
            }
        }
        current = current->next;
    }

    return NULL;
}