#ifndef __included_objtypes_h
#define __included_objtypes_h


#include <lua.h>


typedef struct object_type {
    char *name;
    int table;	/* Lua table reference */
    char *type;
    char *icon;
} object_type_t;


void object_types_init ();
void object_types_shutdown ();
void object_types_register (const char *, lua_Object,
			    const char *, const char *);
object_type_t *object_types_lookup (const char *name);
void object_types_enumerate (void (*proc) (object_type_t *type));


#endif
