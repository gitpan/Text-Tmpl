#ifndef  __CONTEXT_H
#define  __CONTEXT_H

typedef struct context_struct context;
typedef struct context_struct *context_p;

#include <varlist.h>
#include <nclist.h>

struct context_struct
{
    /* table of variables and values in this list */
    varlist_p variables;

    /* table of named child contexts */
    nclist_p  named_child_contexts;

    /* pointer to parent context, or NULL if this is the top context */
    context_p parent_context;

    /* pointer to next context, or NULL if this is the last (or only) element
       in this loop */
    context_p next_context;

    /* boolean value - should the contents of this context be output? */
    char      output_contents;

    /* boolean value - is this context anonymous? */
    char      anonymous;
};

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

context_p context_init(void);
void      context_destroy(context_p ctx);
char *    context_get_value(context_p ctx, char *name);
int       context_set_value(context_p ctx, char *name, char *value);
context_p context_get_anonymous_child(context_p ctx);
context_p context_get_named_child(context_p ctx, char *name);
int       context_set_named_child(context_p ctx, char *name);
context_p context_add_peer(context_p ctx);
void      context_output_contents(context_p ctx, char output_contents);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __CONTEXT_H */
