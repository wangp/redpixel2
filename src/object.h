#ifndef __included_object_h
#define __included_object_h


#include <allegro.h>
#include "bitmask.h"
#include "list.h"
#include "mylua.h"
#include "objtypes.h"


typedef int objid_t;


struct object_layer {
    struct object_layer *next;
    struct object_layer *prev;
    int id;
    BITMAP *bmp;
    int offset_x;
    int offset_y;
};


struct object_light {
    struct object_light *next;
    struct object_light *prev;
    int id;
    BITMAP *bmp;
    int offset_x;
    int offset_y;    
};


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
	struct list_head layers;
	struct list_head lights;
    } cvar;
} object_t;


object_t *object_create (const char *type_name);
void object_destroy (object_t *obj);


int object_add_layer (object_t *obj, const char *key,
		      int offset_x, int offset_y);
int object_replace_layer (object_t *obj, int layer_id, const char *key,
			  int offset_x, int offset_y);
int object_remove_layer (object_t *obj, int layer_id);
void object_remove_all_layers (object_t *obj);


int object_add_light (object_t *obj, const char *key,
		      int offset_x, int offset_y);
int object_replace_light (object_t *obj, int light_id, const char *key,
			  int offset_x, int offset_y);
int object_remove_light (object_t *obj, int light_id);
void object_remove_all_lights (object_t *obj);


float object_get_number (object_t *obj, const char *var);
void object_set_number (object_t *obj, const char *var, float value);
const char *object_get_string (object_t *obj, const char *var);
void object_set_string (object_t *obj, const char *var, const char *value);

object_t *table_object (lua_State *L, int index);


#endif
