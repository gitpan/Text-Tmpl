/* ====================================================================
 * Copyright 1999 Web Juice, LLC. All rights reserved.
 *
 * nclist.c
 *
 * Functions for manipulating the named context list structure in the
 * template library.
 *
 * ==================================================================== */

#include <stdlib.h>
#include <string.h>

#include <nclist.h>
#include <context.h>



/* ====================================================================
 * NAME:          nclist_init
 *
 * DESCRIPTION:   Initializes and returns a pointer to a new named
 *                context structure.
 *
 * RETURN VALUES: Returns NULL if the memory allocation fails; otherwise
 *                returns a pointer to a named context structure.
 *
 * BUGS:          Hopefully none.
 * ==================================================================== */
nclist_p
nclist_init(void)
{
    nclist_p named_context_list;

    named_context_list = (nclist_p)calloc(1, sizeof(nclist));
    if (named_context_list == NULL)
    {
        return NULL;
    }

    named_context_list->name    = NULL;
    named_context_list->context = NULL;
    named_context_list->next    = NULL;

    return(named_context_list);
}



/* ====================================================================
 * NAME:          nclist_destroy
 *
 * DESCRIPTION:   Frees up all memory associated with a named context list.
 *
 * RETURN VALUES: None.
 *
 * BUGS:          Because a free()d pointer still *looks* valid, it is
 *                difficult to protect against the problems that arise
 *                if the user calls this function too early.
 * ==================================================================== */
void
nclist_destroy(nclist_p named_context_list)
{
    nclist_p next;

    if (named_context_list == NULL)
    {
        return;
    }

    next = named_context_list->next;

    if (named_context_list->context != NULL)
    {
        context_destroy(named_context_list->context);
    }

    named_context_list->next    = NULL;
    if (named_context_list->name != NULL)
    {
        free(named_context_list->name);
    }
    named_context_list->context = NULL;
    free(named_context_list);

    nclist_destroy(next);
}



/* ====================================================================
 * NAME:          nclist_get_context
 *
 * DESCRIPTION:   Returns a pointer to a context stored under the
 *                given name.
 *
 * RETURN VALUES: Returns NULL if there's any trouble; otherwise returns
 *                a context pointer.
 *
 * BUGS:          Hopefully none.
 * ==================================================================== */
context_p
nclist_get_context(nclist_p named_context_list, char *name)
{
    nclist_p current = named_context_list;

    if (named_context_list == NULL)
    {
        return NULL;
    }

    while (current != NULL)
    {
        if ((current->name != NULL) && (current->context != NULL)
            && (strcmp(current->name, name) == 0))
        {
            return(current->context);
        }
        current = current->next;
    }
    return NULL;
}



/* ====================================================================
 * NAME:          nclist_new_context
 *
 * DESCRIPTION:   Store a new context under the given name.  This will
 *                grow the list as needed.
 *
 * RETURN VALUES: Returns 0 if there's any trouble.  Returns 1 if the
 *                context was successfully added.
 *
 * BUGS:          Hopefully none.
 * ==================================================================== */
int
nclist_new_context(nclist_p named_context_list, char *name)
{
    nclist_p current  = named_context_list;
    nclist_p last     = NULL;

     
    if (named_context_list == NULL)
    {
        return 0;
    }

    while (current != NULL)
    {
        if ((current->name != NULL) && (strcmp(current->name, name) == 0))
        {
            return 0;
        }
        last    = current;
        current = current->next;
    }

    if (last->context == NULL)
    {
        current = last;
    } else {
        last->next = nclist_init();
        current    = last->next;

        if (current == NULL)
        {
            return 0;
        }
    }
    current->name = (char *)malloc(strlen(name) + 1);
    strncpy(current->name, name, strlen(name));
    current->name[strlen(name)] = '\0';
    current->context = context_init();

    return 1;
}
