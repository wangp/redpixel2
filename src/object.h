#ifndef __included_object_h
#define __included_object_h


#include "mylua.h"

struct BITMAP;
struct map;
struct lua_State;
struct list_head;


typedef struct object object_t;

/* Unique id number.  All machines playing the same game have the same
 * id numbers for objects.  Numbers less than OBJID_PLAYER_MAX are
 * reserved for use by objects controlled by clients. */
typedef unsigned long objid_t;

#define OBJID_PLAYER_MAX	1024


int object_init ();
void object_shutdown ();


object_t *object_create (const char *type_name);
object_t *object_create_ex (const char *type_name, objid_t);
object_t *object_create_proxy (const char *type_name, objid_t);
void object_run_init_func (object_t *);
void object_destroy (object_t *);

struct objtype *object_type (object_t *);
objid_t object_id (object_t *);
int object_is_client (object_t *);
int object_stale (object_t *);
void object_set_stale (object_t *);

float object_x (object_t *);
float object_y (object_t *);
void object_set_xy (object_t *, float, float);
float object_xv (object_t *);
float object_yv (object_t *);
void object_set_xv (object_t *, float);
void object_set_yv (object_t *, float);
void object_set_xvyv (object_t *, float, float);
float object_xa (object_t *);
float object_ya (object_t *);
void object_set_xa (object_t *, float);
void object_set_ya (object_t *, float);
float object_xv_decay (object_t *);
float object_yv_decay (object_t *);
float object_mass (object_t *);
void object_set_mass (object_t *, float);
int object_ramp (object_t *);
void object_set_ramp (object_t *, int);
int object_jump (object_t *);
void object_set_jump (object_t *, int);

void object_set_collision_is_player (object_t *);
void object_set_collision_flags (object_t *, int tiles, int players, int nonplayers);
void object_set_collision_flags_string (object_t *, const char *flags);
int object_collision_tag (object_t *);
void object_set_collision_tag (object_t *, int);

#define OBJECT_REPLICATE_CREATE		0x01
#define OBJECT_REPLICATE_UPDATE		0x02

int object_need_replication (object_t *, int);
void object_set_replication_flag (object_t *, int);
void object_clear_replication_flags (object_t *);

typedef struct creation_field creation_field_t;
struct creation_field {
    creation_field_t *next;
    creation_field_t *prev;
    char name[0];
};

void object_add_creation_field (object_t *, const char *name);
struct list_head *object_creation_fields (object_t *);


/* Update hooks.  */

void object_set_update_hook (object_t *, int msecs, lua_ref_t hook);
void object_remove_update_hook (object_t *);
void object_poll_update_hook (object_t *, int elapsed_msecs);


/* Layers.  */

int object_add_layer (object_t *, const char *key, int offset_x, int offset_y);
int object_replace_layer (object_t *, int layer_id, const char *key, int offset_x, int offset_y);
int object_move_layer (object_t *, int layer_id, int offset_x, int offset_y);
int object_hflip_layer (object_t *, int layer_id, int hflip);
int object_rotate_layer (object_t *, int layer_id, int angle);
int object_remove_layer (object_t *, int layer_id);
void object_remove_all_layers (object_t *);


/* Lights.  */

int object_add_light (object_t *, const char *key, int offset_x, int offset_y);
int object_replace_light (object_t *, int light_id, const char *key, int offset_x, int offset_y);
int object_move_light (object_t *, int light_id, int offset_x, int offset_y);
int object_remove_light (object_t *, int light_id);
void object_remove_all_lights (object_t *);


/* Masks.  */

#define OBJECT_MASK_MAIN	0
#define OBJECT_MASK_TOP		1
#define OBJECT_MASK_BOTTOM	2
#define OBJECT_MASK_LEFT	3
#define OBJECT_MASK_RIGHT	4
#define OBJECT_MASK_MAX		5

int object_set_mask (object_t *, int mask_num, const char *key, int offset_x, int offset_y);
int object_remove_mask (object_t *, int mask_num);
void object_remove_all_masks (object_t *);


/* Collisions.  */

int object_supported (object_t *, struct map *);
int object_supported_at (object_t *, struct map *, float x, float y);


/* Movement.  */

void object_do_physics (object_t *, struct map *);


void object_set_auth_info (object_t *obj, unsigned long time,
			   float x, float y,
			   float xv, float yv,
			   float xa, float ya);
void object_do_simulation (object_t *obj, unsigned long curr_time);


/* Lua table operations.  */

void lua_pushobject (struct lua_State *, object_t *);
object_t *lua_toobject (struct lua_State *, int index);
void object_call (object_t *, const char *method);
float object_get_number (object_t *, const char *var);
void object_set_number (object_t *, const char *var, float value);
const char *object_get_string (object_t *, const char *var);
void object_set_string (object_t *, const char *var, const char *value);


/* Drawing.  */

void object_draw_layers (struct BITMAP *, object_t *, int offset_x, int offset_y);
void object_draw_lit_layers (struct BITMAP *, object_t *, int offset_x, int offset_y, int color);
void object_draw_lights (struct BITMAP *, object_t *, int offset_x, int offset_y);


/* Misc.  */

void object_bounding_box (object_t *, int *rx1, int *ry1, int *rx2, int *ry2);


#endif
