#include <varlist.h>
#include <nclist.h>
#include <context.h>
#include <staglist.h>
#include <tagplist.h>
#include <default_tags.h>

#ifndef __TEMPLATE_H
#define __TEMPLATE_H


#define TEMPLATE_DELIMITER_SIZE 6
#define TEMPLATE_TRUE           "1"
#define TEMPLATE_FALSE          "0"

#define TEMPLATE_DEBUG_NONE	0
#define TEMPLATE_DEBUG_SOME	1
#define TEMPLATE_DEBUG_LOTS	2

extern staglist_p simple_tags;
extern tagplist_p tag_pairs;
extern char otag[TEMPLATE_DELIMITER_SIZE];
extern char ctag[TEMPLATE_DELIMITER_SIZE];


#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

context_p template_init(void);
int       template_set_delimiters(char *opentag, char *closetag);
int       template_register_simple(char *name,
                             void (*function)(context_p, char **, int, char**));
int       template_register_pair(char named_context, char *open_name,
                                 char *close_name,
                                 void (*function)(context_p, int, char**));
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
