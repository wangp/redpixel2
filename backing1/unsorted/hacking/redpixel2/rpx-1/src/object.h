#ifndef _included_object_h_
#define _included_object_h_

#include <lua.h>
#include <allegro.h>

typedef struct object_def
{
    char *name;
    lua_Object table;
    BITMAP *icon;
    struct object_def *prev, *next;
} object_def_t;

typedef struct object 
{
    object_def_t *def;
    unsigned int x, y;
    struct object *prev, *next;
} object_t;

void register_object_type(char *name, lua_Object table, BITMAP *icon);
object_def_t *get_object_type(int i);
void free_object_types();

struct object *create_object(struct object_def *def);
void destroy_object(struct object *obj);

#endif
