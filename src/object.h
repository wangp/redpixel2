#ifndef __included_object_h
#define __included_object_h


#include <allegro.h>
#include "bitmask.h"
#include "mylua.h"
#include "objtypes.h"


typedef int objid_t;


typedef struct object {
    struct object *next;
    struct object *prev;
    objtype_t *type;
    objid_t id;

    /* Lua table ref.  */
    lref_t table;

    /* C variables.  */
    struct {
	float x, y;
	float xv, yv;
	BITMAP *light_source;
    } cvar;
} object_t;


object_t *object_create (const char *type_name);
void object_destroy (object_t *obj);

int object_set_light_source (object_t *obj, const char *key);

/* Access / modify object Lua table values.  */
float object_get_number (object_t *obj, const char *var);
void object_set_number (object_t *obj, const char *var, float value);
const char *object_get_string (object_t *obj, const char *var);
void object_set_string (object_t *obj, const char *var, const char *value);

object_t *table_object (lua_State *L, int index);


#endif
