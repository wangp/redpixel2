#ifndef _included_weapon_h_
#define _included_weapon_h_

#include <lua.h>

typedef struct weapon_type
{
    char *name;
    lua_Object table;
    struct weapon_type *prev, *next;
} weapon_type_t;

void register_weapon_type(char *name, lua_Object table);
void free_weapon_types();

#endif
