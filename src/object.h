#ifndef __included_object_h
#define __included_object_h


#include "mylua.h"

struct BITMAP;
struct map;


typedef struct object object_t;

/* Unique id number.  All machines playing the same game have the same
 * id numbers for objects.  Negative numbers are reserved for use by
 * objects controlled by clients.  */
typedef int objid_t;


int object_init ();
void object_shutdown ();


object_t *object_create (const char *type_name);
object_t *object_create_ex (const char *type_name, objid_t id);
object_t *object_create_proxy (const char *type_name, objid_t id);
void object_destroy (object_t *obj);

struct objtype *object_type (object_t *obj);
objid_t object_id (object_t *obj);
float object_x (object_t *obj);
float object_y (object_t *obj);
void object_set_xy (object_t *obj, float x, float y);
float object_real_x (object_t *obj);
float object_real_y (object_t *obj);
void object_set_real_xy (object_t *obj, float x, float y);
float object_xv (object_t *obj);
float object_yv (object_t *obj);
void object_set_xv (object_t *obj, float xv);
void object_set_yv (object_t *obj, float yv);
float object_client_yv (object_t *obj);
void object_set_client_yv (object_t *obj, float yv);
float object_mass (object_t *obj);
void object_set_mass (object_t *obj, float mass);
int object_ramp (object_t *obj);
void object_set_ramp (object_t *obj, int ramp);
int object_jump (object_t *obj);
void object_set_jump (object_t *obj, int jump);
int object_need_replication (object_t *obj);
void object_set_need_replication (object_t *obj);
void object_clear_need_replication (object_t *obj);
unsigned long object_proxy_time (object_t *obj);
void object_set_proxy_time (object_t *obj, unsigned long proxy_time);


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

int object_supported (object_t *obj, struct map *map);
int object_supported_at (object_t *obj, struct map *map, float x, float y);


/* Movement.  */

int object_move (object_t *obj, int mask_num, struct map *map, float xv, float yv);
int object_move_x_with_ramp (object_t *obj, int mask_num, struct map *map,
			     float dx, int ramp_amount);


/* Lua table operations.  */

void lua_pushobject (lua_State *L, object_t *obj);
object_t *lua_toobject (lua_State *L, int index);
void object_call (object_t *obj, const char *method);
float object_get_number (object_t *obj, const char *var);
void object_set_number (object_t *obj, const char *var, float value);
const char *object_get_string (object_t *obj, const char *var);
void object_set_string (object_t *obj, const char *var, const char *value);


/* Drawing.  */

void object_draw_layers (struct BITMAP *dest, object_t *obj,
			 int offset_x, int offset_y);
void object_draw_lit_layers (struct BITMAP *dest, object_t *obj,
			     int offset_x, int offset_y, int color);
void object_draw_lights (struct BITMAP *dest, object_t *obj,
			 int offset_x, int offset_y);


/* Misc.  */

void object_bounding_box (object_t *obj, int *rx1, int *ry1, int *rx2, int *ry2);


#endif
