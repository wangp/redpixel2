#ifndef __included_object_h
#define __included_object_h


#include <allegro.h>
#include "bitmaskr.h"
#include "list.h"
#include "map.h"
#include "mylua.h"
#include "objmin.h"
#include "objtypes.h"


typedef int objid_t;


typedef struct objmask {
    bitmask_ref_t *ref;
    int offset_x;
    int offset_y;
} objmask_t;


struct object {
    struct object *next;
    struct object *prev;
    objtype_t *type;
    objid_t id;

    /* Lua table ref.  */
    lref_t table;

    /* C variables.  */
    float x, y;
    float xv, yv;
    float mass;
    float ramp;
    struct list_head layers;
    struct list_head lights;
    objmask_t mask[5];

    /* XXX - player specific.  */
    int jump;
};


/* Creation.  */

object_t *object_create (const char *type_name);
void object_destroy (object_t *obj);


/* Mass.  */

void object_set_mass (object_t *obj, float mass);


/* Ramp.  */

void object_set_ramp (object_t *obj, float ramp);


/* Layers.  */

int object_add_layer (object_t *obj, const char *key,
		      int offset_x, int offset_y);
int object_replace_layer (object_t *obj, int layer_id, const char *key,
			  int offset_x, int offset_y);
int object_move_layer (object_t *obj, int layer_id,
		       int offset_x, int offset_y);
int object_remove_layer (object_t *obj, int layer_id);
void object_remove_all_layers (object_t *obj);


/* Lights.  */

int object_add_light (object_t *obj, const char *key,
		      int offset_x, int offset_y);
int object_replace_light (object_t *obj, int light_id, const char *key,
			  int offset_x, int offset_y);
int object_move_light (object_t *obj, int light_id,
		       int offset_x, int offset_y);
int object_remove_light (object_t *obj, int light_id);
void object_remove_all_lights (object_t *obj);


/* Masks.  */

int object_set_mask (object_t *obj, int mask_num, const char *key,
		     int offset_x, int offset_y);
int object_remove_mask (object_t *obj, int mask_num);
void object_remove_all_masks (object_t *obj);


/* Collisions.  */

int object_supported_at (object_t *obj, map_t *map, float x, float y);


/* Movement.  */

int object_move (object_t *obj, int mask_num, map_t *map, float xv, float yv);
int object_move_x_with_ramp (object_t *obj, int mask_num, map_t *map,
			     float dx, float ramp_amount);


/* Lua table operations.  */

void object_call (object_t *obj, const char *method);
float object_get_number (object_t *obj, const char *var);
void object_set_number (object_t *obj, const char *var, float value);
const char *object_get_string (object_t *obj, const char *var);
void object_set_string (object_t *obj, const char *var, const char *value);

object_t *table_object (lua_State *L, int index);


/* Drawing.  */

void object_draw_layers (BITMAP *dest, object_t *obj,
			 int offset_x, int offset_y);
void object_draw_lit_layers (BITMAP *dest, object_t *obj,
			     int offset_x, int offset_y, int color);
void object_draw_lights (BITMAP *dest, object_t *obj,
			 int offset_x, int offset_y);


/* Misc.  */

void object_bounding_box (object_t *obj, int *rx1, int *ry1, int *rx2, int *ry2);


#endif
