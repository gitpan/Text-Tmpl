#include <varlist.h>
#include <nclist.h>
#include <context.h>
#include <staglist.h>
#include <tagplist.h>
#include <default_tags.h>

#ifndef __TEMPLATE_H
#define __TEMPLATE_H


#define TEMPLATE_TRUE           "1"
#define TEMPLATE_FALSE          "0"

#define TEMPLATE_DEBUG_NONE	0
#define TEMPLATE_DEBUG_SOME	1
#define TEMPLATE_DEBUG_LOTS	2


#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

context_p template_init(void);
int       template_set_delimiters(context_p ctx, char *opentag,
                                  char *closetag);
int       template_register_simple(context_p ctx, char *name,
                             void (*function)(context_p, char **, int, char**));
int       template_alias_simple(context_p ctx, char *old_name, char *new_name);
int       template_register_pair(context_p ctx, char named_context,
                                 char *open_name, char *close_name,
                                 void (*function)(context_p, int, char**));
int       template_alias_pair(context_p ctx, char *old_open_name,
                              char *old_close_name, char *new_open_name,
                              char *new_close_name);
int       template_set_debug(context_p ctx, int debug_level);
int       template_set_strip(context_p ctx, int strip);
int       template_set_dir(context_p ctx, char *directory);
int       template_set_value(context_p ctx, char *name, char *value);
void      template_destroy(context_p ctx);
context_p template_loop_iteration(context_p ctx, char *loop_name);
int       template_parse_string(context_p ctx, char *tmpl, char **output);
int       template_parse_file(context_p ctx, char *template_filename,
                              char **output);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __TEMPLATE_H */
