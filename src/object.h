#ifndef __included_object_h
#define __included_object_h


#include <allegro.h>
#include "bitmask.h"
#include "mylua.h"
#include "objtypes.h"


typedef struct object {
    struct object *next;
    struct object *prev;
    objtype_t *type;
    unsigned long id;

    /* Lua table ref.  */
    lref_t table;

    /* C variables.  */
    float x, y;
    float xv, yv;
} object_t;


object_t *object_create (const char *type_name);
void object_destroy (object_t *);


#endif
