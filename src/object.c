/* object.c
 *
 * Peter Wang <tjaden@users.sourceforge.net>
 */


#include <stdlib.h>
#include <allegro.h>
#include "alloc.h"
#include "bitmask.h"
#include "bitmaskg.h"
#include "bitmaskr.h"
#include "list.h"
#include "magic4x4.h"
#include "map.h"
#include "mylua.h"
#include "object.h"
#include "objtypes.h"
#include "store.h"


typedef struct objmask {
    bitmask_ref_t *ref;
    int offset_x;
    int offset_y;
} objmask_t;


struct object {
    object_t *next;
    object_t *prev;
    objtype_t *type;
    objid_t id;

    /* C variables.  */
    float x, y;
    float xv, yv;
    float mass;
    float ramp;
    struct list_head layers;
    struct list_head lights;
    objmask_t mask[5];

    /* Lua table ref.  */
    lua_ref_t table;
};


#include "objecttm.inc"


static void set_default_masks (object_t *obj);


static lua_tag_t object_tag;
static objid_t next_id;


int object_init ()
{
    object_tag = lua_newtag (lua_state);
    REGISTER_OBJECT_TAG_METHODS(lua_state, object_tag);
    next_id = 0;
    return 0;
}


void object_shutdown ()
{
}


object_t *object_create (const char *type_name)
{
    object_t *obj;

    obj = object_create_ex (type_name, next_id);
    if (obj)
	next_id++;

    return obj;
}


/* Only use this when you know what you are doing.  If you mix this
 * with `object_create', two objects may end up with the same id.  */
object_t *object_create_ex (const char *type_name, objid_t id)
{
    lua_State *L = lua_state;
    objtype_t *type;
    object_t *obj;

    if (!(type = objtypes_lookup (type_name)))
	return NULL;

    obj = alloc (sizeof (object_t));

    obj->type = type;
    obj->id = id;

    /* C object references Lua table.  */
    lua_newtable (L);
    obj->table = lua_ref (L, 1);

    /* Initialise some C variables.  */
    list_init (obj->layers);
    object_add_layer (obj, objtype_icon (obj->type), 0, 0);

    list_init (obj->lights);

    set_default_masks (obj);

    /* Call base object init function.  */
    lua_getglobal (L, "object_init");
    lua_pushobject (L, obj);
    lua_call (L, 1, 0);

    /* Call object init function if any.  */
    if (objtype_init_func (obj->type) != LUA_NOREF) {
	lua_getref (L, objtype_init_func (obj->type));
	lua_pushobject (L, obj);
	lua_call (L, 1, 0);
    }
    
    return obj;
}


void object_destroy (object_t *obj)
{
    object_remove_all_masks (obj);
    object_remove_all_lights (obj);
    object_remove_all_layers (obj);
    lua_unref (lua_state, obj->table);
    free (obj);
}


objtype_t *object_type (object_t *obj)
{
    return obj->type;
}


objid_t object_id (object_t *obj)
{
    return obj->id;
}


float object_x (object_t *obj)
{
    return obj->x;
}


float object_y (object_t *obj)
{
    return obj->y;
}


void object_set_xy (object_t *obj, float x, float y)
{
    obj->x = x;
    obj->y = y;
}


float object_xv (object_t *obj)
{
    return obj->xv;
}


float object_yv (object_t *obj)
{
    return obj->yv;
}


void object_set_xv (object_t *obj, float xv)
{
    obj->xv = xv;
}


void object_set_yv (object_t *obj, float yv)
{
    obj->yv = yv;
}


float object_mass (object_t *obj)
{
    return obj->mass;
}


void object_set_mass (object_t *obj, float mass)
{
    obj->mass = mass;
}


float object_ramp (object_t *obj)
{
    return obj->ramp;
}


void object_set_ramp (object_t *obj, float ramp)
{
    obj->ramp = ramp;
}


/* Layers.  */


typedef struct objlayer objlayer_t;

struct objlayer {
    objlayer_t *next;
    objlayer_t *prev;
    int id;
    BITMAP *bmp;
    int offset_x;
    int offset_y;
};


static int set_layer (objlayer_t *layer, int id, const char *key,
		      int offset_x, int offset_y)
{
    BITMAP *bmp;

    bmp = store_dat (key);
    if (!bmp)
	return -1;

    layer->id = id;
    layer->bmp = bmp;
    layer->offset_x = offset_x;
    layer->offset_y = offset_y;
    return 0;
}


int object_add_layer (object_t *obj, const char *key,
		      int offset_x, int offset_y)
{
    objlayer_t *layer;
    int id = 0;

  retry:
    
    list_for_each (layer, &obj->layers)
	if (id == layer->id) {
	    id = layer->id + 1;
	    goto retry;
	}

    layer = alloc (sizeof *layer);
    if (set_layer (layer, id, key, offset_x, offset_y) < 0) {
	free (layer);
	return -1;
    }
    list_append (obj->layers, layer);
    return id;
}


int object_replace_layer (object_t *obj, int layer_id, const char *key,
			  int offset_x, int offset_y)
{
    objlayer_t *layer;

    list_for_each (layer, &obj->layers)
	if (layer_id == layer->id) {
	    if (set_layer (layer, layer_id, key, offset_x, offset_y) == 0)
		return 0;
	    break;
	}

    return -1;
}


int object_move_layer (object_t *obj, int layer_id, int offset_x, int offset_y)
{
    objlayer_t *layer;

    list_for_each (layer, &obj->layers)
	if (layer_id == layer->id) {
	    layer->offset_x = offset_x;
	    layer->offset_y = offset_y;
	    return 0;
	}

    return -1;
}


int object_remove_layer (object_t *obj, int layer_id)
{
    objlayer_t *layer;

    list_for_each (layer, &obj->layers)
	if (layer_id == layer->id) {
	    list_remove (layer);
	    free (layer);
	    return 0;
	}

    return -1;
}


void object_remove_all_layers (object_t *obj)
{
    list_free (obj->layers, free);
}


/* Lights.  */

/* Note: Currently lights are very much like layers, but I am not
 * generalising them yet.  (Remember John Harper's quote.)  */


typedef struct objlight objlight_t;

struct objlight {
    objlight_t *next;
    objlight_t *prev;
    int id;
    BITMAP *bmp;
    int offset_x;
    int offset_y;    
};


static int set_light (objlight_t *light, int id, const char *key,
		      int offset_x, int offset_y)
{
    BITMAP *bmp;

    bmp = store_dat (key);
    if (!bmp)
	return -1;

    light->id = id;
    light->bmp = bmp;
    light->offset_x = offset_x;
    light->offset_y = offset_y;
    return 0;
}


int object_add_light (object_t *obj, const char *key,
		      int offset_x, int offset_y)
{
    objlight_t *light;
    int id = 0;

  retry:
    
    list_for_each (light, &obj->lights)
	if (id == light->id) {
	    id = light->id + 1;
	    goto retry;
	}

    light = alloc (sizeof *light);
    if (set_light (light, id, key, offset_x, offset_y) < 0) {
	free (light);
	return -1;
    }
    list_add (obj->lights, light);
    return id;
}


int object_replace_light (object_t *obj, int light_id, const char *key,
			  int offset_x, int offset_y)
{
    objlight_t *light;

    list_for_each (light, &obj->lights)
	if (light_id == light->id) {
	    if (set_light (light, light_id, key, offset_x, offset_y) == 0)
		return 0;
	    break;
	}

    return -1;
}


int object_move_light (object_t *obj, int light_id, int offset_x, int offset_y)
{
    objlight_t *light;

    list_for_each (light, &obj->lights)
	if (light_id == light->id) {
	    light->offset_x = offset_x;
	    light->offset_y = offset_y;
	    return 0;
	}

    return -1;
}


int object_remove_light (object_t *obj, int light_id)
{
    objlight_t *light;

    list_for_each (light, &obj->lights)
	if (light_id == light->id) {
	    list_remove (light);
	    free (light);
	    return 0;
	}

    return -1;
}


void object_remove_all_lights (object_t *obj)
{
    list_free (obj->lights, free);
}


/* Masks.  */


static int set_mask (object_t *obj, int mask_num, bitmask_t *mask,
		     int free, int offset_x, int offset_y)
{
    bitmask_ref_t *save = obj->mask[mask_num].ref;

    obj->mask[mask_num].ref = bitmask_ref_create (mask, free);
    obj->mask[mask_num].offset_x = offset_x;
    obj->mask[mask_num].offset_y = offset_y;

    /* Destroy here instead of at start of the function in case the
     * new one is the same as the old one.  */
    bitmask_ref_destroy (save);

    return 0;
}


int object_set_mask (object_t *obj, int mask_num, const char *key,
		     int offset_x, int offset_y)
{
    bitmask_t *mask;
    
    mask = store_dat (key);
    if ((!mask) || (mask_num < 0) || (mask_num > 4))
	return -1;

    return set_mask (obj, mask_num, mask, 0, offset_x, offset_y);
}


int object_remove_mask (object_t *obj, int mask_num)
{
    if ((mask_num < 0) || (mask_num > 4))
	return -1;
    bitmask_ref_destroy (obj->mask[mask_num].ref);
    obj->mask[mask_num].ref = NULL;
    return 0;
}


void object_remove_all_masks (object_t *obj)
{
    int i;
    for (i = 0; i < 5; i++)
	object_remove_mask (obj, i);
}


static void set_default_masks (object_t *obj)
{
    bitmask_t *mask;
    int xoff, yoff, i;

    mask = objtype_icon_mask (obj->type);
    xoff = - bitmask_width (mask) / 2;
    yoff = - bitmask_height (mask) / 2;

    for (i = 0; i < 5; i++)
	set_mask (obj, i, mask, 0, xoff, yoff);
}


/* Collisions.  */


static int check_collision_with_tiles (object_t *obj, int mask_num,
				       map_t *map, int x, int y)
{
    objmask_t *mask;

    mask = obj->mask;
    if (!mask[mask_num].ref)
	return 0;

    return bitmask_check_collision (bitmask_ref_bitmask (mask[mask_num].ref),
				    map_tile_mask (map),
				    x + mask[mask_num].offset_x,
				    y + mask[mask_num].offset_y,
				    0, 0);
}


static int check_collision_with_objects (object_t *obj, int mask_num,
					 map_t *map, int x, int y)
{
    struct list_head *list;
    objmask_t *mask;
    object_t *p;

    mask = obj->mask;
    if (!mask[mask_num].ref)
	return 0;

    list = map_object_list (map);
    list_for_each (p, list) if ((obj->id != p->id) && (p->mask[0].ref))
	if (bitmask_check_collision (bitmask_ref_bitmask (mask[mask_num].ref),
				     bitmask_ref_bitmask (p->mask[0].ref),
				     x + mask[mask_num].offset_x,
				     y + mask[mask_num].offset_y,
				     p->x + p->mask[0].offset_x, 
				     p->y + p->mask[0].offset_y))
	    return 1;
    
    return 0;
}


static inline int check_collision (object_t *obj, int mask_num, map_t *map,
				   float x, float y)
{
    return (check_collision_with_tiles (obj, mask_num, map, x, y) ||
	    check_collision_with_objects (obj, mask_num, map, x, y));
}


int object_supported (object_t *obj, map_t *map)
{
    return object_supported_at (obj, map, obj->x, obj->y);
}


int object_supported_at (object_t *obj, map_t *map, float x, float y)
{
    return check_collision (obj, 2, map, x, y+1);
}


/* Movement.  */


#define SIGN(a)	((a < 0) ? -1 : 1)


int object_move (object_t *obj, int mask_num, map_t *map, float dx, float dy)
{
    float idx, idy;

    while ((dx) || (dy)) {
	idx = (ABS (dx) < 1) ? dx : SIGN (dx);
	idy = (ABS (dy) < 1) ? dy : SIGN (dy);

	if (check_collision (obj, mask_num, map, obj->x + idx, obj->y + idy))
	    return -1;

	obj->x += idx;
	obj->y += idy;

	dx = (ABS (dx) < 1) ? 0 : SIGN (dx) * (ABS (dx) - 1);
	dy = (ABS (dy) < 1) ? 0 : SIGN (dy) * (ABS (dy) - 1);
    }

    return 0;
}


int object_move_x_with_ramp (object_t *obj, int mask_num, map_t *map,
			     float dx, float ramp)
{
    float idx, ir;

    while (dx) {
	idx = (ABS (dx) < 1) ? dx : SIGN (dx);

	for (ir = 0; ir <= ramp; ir++)
	    if (!check_collision (obj, mask_num, map, obj->x+idx, obj->y-ir))
		break;
	if (ir > ramp)
	    return -1;

	obj->x += idx;
	obj->y -= ir;

	dx = (ABS (dx) < 1) ? 0 : SIGN (dx) * (ABS (dx) - 1);
    }

    return 0;
}


/* Lua table operations.  */


void lua_pushobject (lua_State *L, object_t *obj)
{
    lua_pushusertag(L, obj, object_tag);
}


object_t *lua_toobject (lua_State *L, int index)
{
    if (!lua_isuserdata(L, index) || (lua_tag(L, index) != object_tag))
	return NULL;

    return lua_touserdata(L, index);
}


void object_call (object_t *obj, const char *method)
{
    lua_State *L = lua_state;

    lua_pushobject (L, obj);
    lua_pushstring (L, method);
    lua_gettable (L, -2);
    lua_pushobject (L, obj);
    lua_call (L, 1, 0);
    lua_pop (L, 1);    
}


float object_get_number (object_t *obj, const char *var)
{
    lua_State *L = lua_state;
    float val = 0.0;

    lua_pushobject (L, obj);
    lua_pushstring (L, var);
    lua_gettable (L, -2);
    if (lua_isnumber (L, -1))
	val = lua_tonumber (L, -1);
    lua_pop (L, 2);

    return val;
}


void object_set_number (object_t *obj, const char *var, float value)
{
    lua_State *L = lua_state;

    lua_pushobject (L, obj);
    lua_pushstring (L, var);
    lua_pushnumber (L, value);
    lua_settable (L, -3);
    lua_pop (L, 1);
}


const char *object_get_string (object_t *obj, const char *var)
{
    lua_State *L = lua_state;
    const char *str = NULL;

    lua_pushobject (L, obj);
    lua_pushstring (L, var);
    lua_gettable (L, -2);
    if (lua_isstring (L, -1))
	str = lua_tostring (L, -1);
    lua_pop (L, 2);

    return str;
}


void object_set_string (object_t *obj, const char *var, const char *value)
{
    lua_State *L = lua_state;

    lua_pushobject (L, obj);
    lua_pushstring (L, var);
    lua_pushstring (L, value);
    lua_settable (L, -3);
    lua_pop (L, 1);
}


/* Drawing.  */


void object_draw_layers (BITMAP *dest, object_t *obj,
			 int offset_x, int offset_y)
{
    objlayer_t *layer;
    BITMAP *bmp;

    list_for_each (layer, &obj->layers) {
	bmp = layer->bmp;
	draw_magic_sprite
	    (dest, bmp,
	     obj->x - offset_x + layer->offset_x - bmp->w/3/2,
	     obj->y - offset_y + layer->offset_y - bmp->h/2);
    }
}


void object_draw_lit_layers (BITMAP *dest, object_t *obj,
			     int offset_x, int offset_y, int color)
{
    objlayer_t *layer;
    BITMAP *bmp;

    list_for_each (layer, &obj->layers) {
	bmp = layer->bmp;
	draw_lit_magic_sprite
	    (dest, bmp,
	     obj->x - offset_x + layer->offset_x - bmp->w/3/2,
	     obj->y - offset_y + layer->offset_y - bmp->h/2,
	     color);
    }
}


void object_draw_lights (BITMAP *dest, object_t *obj,
			 int offset_x, int offset_y)
{
    objlight_t *light;
    BITMAP *bmp;

    list_for_each (light, &obj->lights) {
	bmp = light->bmp;
	draw_trans_magic_sprite
	    (dest, bmp,
	     obj->x - offset_x + light->offset_x - bmp->w/3/2,
	     obj->y - offset_y + light->offset_y - bmp->h/2);
    }
}


/* Misc.  */


void object_bounding_box (object_t *obj, int *rx1, int *ry1, int *rx2, int *ry2)
{
    objlayer_t *layer;
    int x, y, x1, y1, x2, y2;

    x1 = y1 = x2 = y2 = 0;
    
    list_for_each (layer, &obj->layers) {
	x = layer->offset_x - layer->bmp->w/3/2;
	y = layer->offset_y - layer->bmp->h/2;

	x1 = MIN (x1, x);
	y1 = MIN (y1, y);
	x2 = MAX (x2, x + layer->bmp->w/3 - 1);
	y2 = MAX (y2, y + layer->bmp->h - 1);
    }

    *rx1 = x1;
    *ry1 = y1;
    *rx2 = x2;
    *ry2 = y2;
}
