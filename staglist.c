/* ====================================================================
 * Copyright 1999 Web Juice, LLC. All rights reserved.
 *
 * staglist.c
 *
 * Functions for manipulating the simple tag list structure in the
 * template library.
 *
 * ==================================================================== */

#include <string.h>
#include <stdlib.h>

#include <context.h>
#include <staglist.h>


/* ====================================================================
 * NAME:          staglist_init
 *
 * DESCRIPTION:   Initializes and returns a pointer to a new simple tag
 *                structure.
 *
 * RETURN VALUES: Returns NULL if the memory allocation fails; otherwise
 *                returns a pointer to a simple tag structure.
 *
 * BUGS:          Hopefully none.
 * ==================================================================== */
staglist_p
staglist_init(void)
{
    staglist_p simple_tag_list;

    simple_tag_list = (staglist_p)calloc(1, sizeof(staglist));
    if (simple_tag_list == NULL)
    {
        return NULL;
    }

    simple_tag_list->name     = NULL;
    simple_tag_list->function = NULL;
    simple_tag_list->next     = NULL;

    return(simple_tag_list);
}



/* ====================================================================
 * NAME:          staglist_destroy
 *
 * DESCRIPTION:   Frees up all memory associated with a simple tag list
 *
 * RETURN VALUES: None.
 *
 * BUGS:          Because a free()d pointer still *looks* valid, it is
 *                difficult to protect against the problems that arise
 *                if the user calls this function too early.
 * ==================================================================== */
void
staglist_destroy(staglist_p simple_tag_list)
{
    staglist_p next;

    if (simple_tag_list == NULL)
    {
        return;
    }

    next = simple_tag_list->next;

    simple_tag_list->next = NULL;
    if (simple_tag_list->name != NULL)
    {
        free(simple_tag_list->name);
    }
    free(simple_tag_list);

    staglist_destroy(next);
}



/* ====================================================================
 * NAME:          staglist_alias
 *
 * DESCRIPTION:   Copy an existing tag to a new name.
 *
 * RETURN VALUES: Returns 0 upon failure; 1 on success.
 *
 * BUGS:          Hopefully none.
 * ==================================================================== */
int
staglist_alias(staglist_p *simple_tag_list, char *old_name, char *new_name)
{
    staglist_p current  = *simple_tag_list;

    /* Make sure the simple tag list isn't NULL */
    if (*simple_tag_list == NULL)
    {
        return 0;
    }

    /* Make sure that neither name is NULL */
    if ((old_name == NULL) || (new_name == NULL))
    {
        return 0;
    }

    /* Walk through the list to find old_name */
    while (current != NULL)
    {
        if ((current->name != NULL) && (strcmp(current->name, old_name) == 0))
        {
            return staglist_register(simple_tag_list, new_name,
                                     current->function);
        }
        current = current->next;
    }

    return 0;
}



/* ====================================================================
 * NAME:          staglist_register
 *
 * DESCRIPTION:   Register a new tag and associated function to call.
 *
 * RETURN VALUES: Returns 0 upon failure; 1 on success.
 *
 * BUGS:          Hopefully none.
 * ==================================================================== */
int
staglist_register(staglist_p *simple_tag_list, char *name,
                  void (*function)(context_p, char **, int, char**))
{
    staglist_p new = NULL;

    /* Make sure the function isn't NULL */
    if (function == NULL)
    {
        return 0;
    }

    /* Make sure the name isn't NULL */
    if (name == NULL)
    {
        return 0;
    }

    /* Make sure the pointer passed in wasn't NULL */
    if (*simple_tag_list == NULL)
    {
        return 0;
    }

    new = staglist_init();
    if (new == NULL)
    {
        return 0;
    }

    new->function = function;

    new->name = (char *)malloc(strlen(name) + 1);
    strncpy(new->name, name, strlen(name));
    new->name[strlen(name)] = '\0';

    new->next = *simple_tag_list;

    *simple_tag_list = new;

    return 1;
}



/* ====================================================================
 * NAME:          staglist_exists
 *      
 * DESCRIPTION:   Find out whether a particular tag is a legitimate
 *                simple tag.
 *   
 * RETURN VALUES: Returns 0 if there's a problem or if the name is not
 *                a valid simple tag, or 1 if the name is a valid tag.
 *  
 * BUGS:          Hopefully none.
 * ==================================================================== */
int
staglist_exists(staglist_p simple_tag_list, char *name)
{
    staglist_p current = simple_tag_list;

    if (simple_tag_list == NULL)
    {
        return 0;
    }

    while (current != NULL)
    {
        if ((current->name != NULL) && (current->function != NULL)
            && (strcmp(current->name, name) == 0))
        {
            return 1;
        }
        current = current->next;
    }
    return 0;
}



/* ====================================================================
 * NAME:          staglist_exec
 *      
 * DESCRIPTION:   Executes the function associated with a given tag.
 *   
 * RETURN VALUES: Returns NULL if there's a problem or the string result
 *                of the function on success.
 *  
 * BUGS:          Hopefully none.
 * ==================================================================== */
int
staglist_exec(staglist_p simple_tag_list, char *name, context_p ctx,
              char **output, int argc, char **argv)
{
    staglist_p current = simple_tag_list;

    if (simple_tag_list == NULL)
    {
        return 0;
    }

    while (current != NULL)
    {
        if ((current->name != NULL) && (current->function != NULL)
            && (strcmp(current->name, name) == 0))
        {
            current->function(ctx, output, argc, argv);
            return 1;
        }
        current = current->next;
    }
    return 0;
}
