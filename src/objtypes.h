#ifndef __included_objtypes_h
#define __included_objtypes_h


#include "mylua.h"

struct bitmask;


typedef struct objtype {
    char *type;
    char *name;
    char *icon;
    lua_ref_t init_func;
    struct bitmask *icon_mask;
} objtype_t;


void objtypes_init ();
void objtypes_shutdown ();
void objtypes_register (const char *type, const char *name,
			const char *icon, lua_ref_t init_func);
objtype_t *objtypes_lookup (const char *name);
void objtypes_enumerate (void (*proc) (objtype_t *type));


#endif
