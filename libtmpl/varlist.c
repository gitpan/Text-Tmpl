/* ====================================================================
 * Copyright 1999 Web Juice, LLC. All rights reserved.
 *
 * varlist.c
 *
 * Functions for manipulating the variable list structure in the template
 * library.
 *
 * ==================================================================== */

#include <string.h>
#include <stdlib.h>

#include <varlist.h>
#include <template.h>



/* ====================================================================
 * NAME:          varlist_init
 *
 * DESCRIPTION:   Initializes and returns a pointer to a new varlist
 *                structure.
 *
 * RETURN VALUES: Returns NULL if the memory allocation fails; otherwise
 *                returns a pointer to a varlist structure.
 *
 * BUGS:          Hopefully none.
 * ==================================================================== */
varlist_p
varlist_init(void)
{
    varlist_p variable_list;

    variable_list = (varlist_p)calloc(1, sizeof(varlist));
    if (variable_list == NULL)
    {
        return NULL;
    }

    variable_list->next  = NULL;
    variable_list->name  = NULL;
    variable_list->value = NULL;

    return(variable_list);
}



/* ====================================================================
 * NAME:          varlist_destroy
 *
 * DESCRIPTION:   Frees up all memory associated with a varlist.
 *
 * RETURN VALUES: None.
 *
 * BUGS:          Because a free()d pointer still *looks* valid, it is
 *                difficult to protect against the problems that arise
 *                if the user calls this function too early.
 * ==================================================================== */
void
varlist_destroy(varlist_p variable_list)
{
    varlist_p next;

    if (variable_list == NULL)
    {
        return;
    }

    next = variable_list->next;

    variable_list->next = NULL;
    if (variable_list->name != NULL)
    {
        free(variable_list->name);
    }
    if (variable_list->value != NULL)
    {
        free(variable_list->value);
    }
    free(variable_list);

    varlist_destroy(next);
}



/* ====================================================================
 * NAME:          varlist_get_value
 *
 * DESCRIPTION:   Returns the value of the variable stored under the given
 *                name, if it exists.
 *
 * RETURN VALUES: This function will return NULL if there are any problems
 *                or if there is no value.  It returns a string containing
 *                the value on success.
 *
 * BUGS:          Hopefully none.
 * ==================================================================== */
char *
varlist_get_value(varlist_p variable_list, char *name)
{
    varlist_p current = variable_list;

    while (current != NULL)
    {
        if ((current->name != NULL) && (current->value != NULL)
            && (strcmp(current->name, name) == 0))
        {
            return(current->value);
        }
        current = current->next;
    }
    return NULL;
}



/* ====================================================================
 * NAME:          varlist_set_value
 *
 * DESCRIPTION:   Set a variable in the given varlist to be equal to
 *                the value given.  This will overwrite existing values
 *                silently, and will also expand the list if need be.
 *
 * RETURN VALUES: Returns 0 if there's any trouble.  Returns 1 if the
 *                variable was successfully set.
 *
 * BUGS:          Hopefully none.
 * ==================================================================== */
int
varlist_set_value(varlist_p variable_list, char *name, char *value)
{
    varlist_p current  = variable_list;
    varlist_p existing = NULL;
    varlist_p last     = NULL;


    /* Make sure that the pointer passed in wasn't NULL */
    if (variable_list == NULL)
    {
        return 0;
    }

    while (current != NULL)
    {
        if ((current->name != NULL) && (current->value != NULL)
            && (strcmp(current->name, name) == 0))
        {
            existing = current;
        }
        last    = current;
        current = current->next;
    }

    if (existing == NULL)
    {
        if (last->value == NULL)
        {
            existing = last;
        } else {
            last->next    = varlist_init();
            existing      = last->next;

            if (existing == NULL)
            {
                return 0;
            }
        }
        existing->name = (char *)malloc(strlen(name) + 1);
        strncpy(existing->name, name, strlen(name));
        existing->name[strlen(name)] = '\0';
    }
    if (existing->value != NULL)
    {
        free(existing->value);
    }
    existing->value = (char *)malloc(strlen(value) + 1);
    strncpy(existing->value, value, strlen(value));
    existing->value[strlen(value)] = '\0';

    return 1;
}
