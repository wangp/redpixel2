#ifndef __included_objtypes_h
#define __included_objtypes_h


#include "mylua.h"


typedef struct objtype objtype_t;


void objtypes_init (void);
void objtypes_shutdown (void);
int objtypes_register (const char *type, const char *name,
		       const char *icon, lua_ref_t init_func); /* Lua binding */
objtype_t *objtypes_lookup (const char *name);
void objtypes_enumerate (void (*proc) (objtype_t *type));

const char *objtype_type (objtype_t *);
const char *objtype_name (objtype_t *);
const char *objtype_icon (objtype_t *);
lua_ref_t objtype_init_func (objtype_t *);
struct bitmask *objtype_icon_mask (objtype_t *);


#endif
