/* object.c
 *
 * Peter Wang <tjaden@psynet.net>
 */


#include <stdlib.h>
#include <allegro.h>
#include "alloc.h"
#include "bitmask.h"
#include "list.h"
#include "magic4x4.h"
#include "mylua.h"
#include "object.h"
#include "objmask.h"
#include "objlayer.h"
#include "objlight.h"
#include "objtypes.h"
#include "store.h"


static ltag_t object_tag = LUA_NOTAG;

static objid_t next_id;


object_t *object_create (const char *type_name)
{
    lua_State *L = lua_state;
    objtype_t *type;
    object_t *obj;

    if (object_tag == LUA_NOTAG)
	object_tag = newtag (L);

    if (!(type = objtypes_lookup (type_name)))
	return NULL;

    obj = alloc (sizeof (object_t));

    obj->type = type;
    obj->id = next_id++;

    /* C object references Lua table.  */
    newtable (L);
    obj->table = ref (L, 1);

    /* Lua table references C object.  */
    getref (L, obj->table);
    pushstring (L, "_object");
    pushusertag (L, obj, object_tag);
    settable (L, -3);
    pop (L, 1);

    /* Initialise some cvars.  */
    init_list (obj->cvar.layers);
    object_add_layer (obj, obj->type->icon, 0, 0);
    
    init_list (obj->cvar.lights);

    /* Call object init function if any.  */
    if (obj->type->init_func != LUA_NOREF) {
	getref (L, obj->type->init_func);
	getref (L, obj->table);
	call (L, 1, 0);
    }
    
    return obj;
}


void object_destroy (object_t *obj)
{
    unref (lua_state, obj->table);
    object_remove_all_layers (obj);
    object_remove_all_lights (obj);
    object_remove_collision_mask (obj);
    free (obj);
}


/* Layers.  */


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
    
    foreach (layer, obj->cvar.layers)
	if (id == layer->id) {
	    id = layer->id + 1;
	    goto retry;
	}

    layer = alloc (sizeof *layer);
    if (set_layer (layer, id, key, offset_x, offset_y) < 0) {
	free (layer);
	return -1;
    }
    add_to_list (obj->cvar.layers, layer);
    return id;
}


int object_replace_layer (object_t *obj, int layer_id, const char *key,
				int offset_x, int offset_y)
{
    objlayer_t *layer;

    foreach (layer, obj->cvar.layers)
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

    foreach (layer, obj->cvar.layers)
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

    foreach (layer, obj->cvar.layers)
	if (layer_id == layer->id) {
	    del_from_list (layer);
	    free (layer);
	    return 0;
	}

    return -1;
}


void object_remove_all_layers (object_t *obj)
{
    free_list (obj->cvar.layers, free);
}


/* Lights.  */

/* Note: Currently lights are very much like layers, but I am not
 * generalising them yet.  (Remember John Harper's quote.)  */


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
    
    foreach (light, obj->cvar.lights)
	if (id == light->id) {
	    id = light->id + 1;
	    goto retry;
	}

    light = alloc (sizeof *light);
    if (set_light (light, id, key, offset_x, offset_y) < 0) {
	free (light);
	return -1;
    }
    add_to_list (obj->cvar.lights, light);
    return id;
}


int object_replace_light (object_t *obj, int light_id, const char *key,
			  int offset_x, int offset_y)
{
    objlight_t *light;

    foreach (light, obj->cvar.lights)
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

    foreach (light, obj->cvar.lights)
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

    foreach (light, obj->cvar.lights)
	if (light_id == light->id) {
	    del_from_list (light);
	    free (light);
	    return 0;
	}

    return -1;
}


void object_remove_all_lights (object_t *obj)
{
    free_list (obj->cvar.lights, free);
}


/* Collision.  */


int object_set_collision_mask (object_t *obj, const char *key,
			       int offset_x, int offset_y)
{
    bitmask_t *mask;
    objmask_t *objmask;

    mask = store_dat (key);
    if (!mask)
	return -1;

    object_remove_collision_mask (obj);

    objmask = alloc (sizeof *objmask);
    objmask->mask = mask;
    objmask->offset_x = offset_x;
    objmask->offset_y = offset_y;
    obj->cvar.mask = objmask;
    return 0;
}


void object_remove_collision_mask (object_t *obj)
{
    if (obj->cvar.mask) {
	free (obj->cvar.mask);
	obj->cvar.mask = NULL;
    }
}


static int check_collision (object_t *obj, map_t *map, int x, int y)
{
    objmask_t *mask;

    mask = obj->cvar.mask;
    if (!obj->cvar.mask)
	return 0;

    return bitmask_check_collision (mask->mask, map->tile_mask,
				    x + mask->offset_x,
				    y + mask->offset_y, 0, 0);
}


int object_collides_with_map_tiles (object_t *obj, map_t *map)
{
    return check_collision (obj, map, obj->cvar.x, obj->cvar.y);
}


int object_will_collide_with_map_tiles (object_t *obj, map_t *map)
{
    return check_collision (obj, map,
			    obj->cvar.x + obj->cvar.xv,
			    obj->cvar.y + obj->cvar.yv);
}


void object_move_until_collision_with_map_tiles (object_t *obj, map_t *map)
{
    float dxv, dyv;

    /* Move in tiny increments.  */
    dxv = obj->cvar.xv / 32.0;
    dyv = obj->cvar.yv / 32.0;

    while (!check_collision (obj, map, obj->cvar.x+dxv, obj->cvar.y+dyv)) {
  	obj->cvar.x += dxv;
  	obj->cvar.y += dyv;
    }
}


/* Lua table accessor and modifiers.  */


float object_get_number (object_t *obj, const char *var)
{
    lua_State *L = lua_state;
    float val = 0.0;

    getref (L, obj->table);
    pushstring (L, var);
    gettable (L, -2);
    if (isnumber (L, -1))
	val = tonumber (L, -1);
    pop (L, 2);

    return val;
}


void object_set_number (object_t *obj, const char *var, float value)
{
    lua_State *L = lua_state;

    getref (L, obj->table);
    pushstring (L, var);
    pushnumber (L, value);
    settable (L, -3);
    pop (L, 1);
}


const char *object_get_string (object_t *obj, const char *var)
{
    lua_State *L = lua_state;
    const char *str = NULL;

    getref (L, obj->table);
    pushstring (L, var);
    gettable (L, -2);
    if (isstring (L, -1))
	str = tostring (L, -1);
    pop (L, 2);

    return str;
}


void object_set_string (object_t *obj, const char *var, const char *value)
{
    lua_State *L = lua_state;

    getref (L, obj->table);
    pushstring (L, var);
    pushstring (L, value);
    settable (L, -3);
    pop (L, 1);
}


object_t *table_object (lua_State *L, int index)
{
    object_t *obj = NULL;

    pushstring (L, "_object");
    gettable (L, index);
    if (tag (L, -1) == object_tag)
	obj = touserdata (L, -1);
    pop (L, 1);

    return obj;
}


/* Drawing.  */


void object_draw_layers (BITMAP *dest, object_t *obj,
			 int offset_x, int offset_y)
{
    objlayer_t *layer;
    BITMAP *bmp;

    foreach (layer, obj->cvar.layers) {
	bmp = layer->bmp;
	draw_magic_sprite
	    (dest, bmp,
	     obj->cvar.x - offset_x + layer->offset_x - bmp->w/3/2,
	     obj->cvar.y - offset_y + layer->offset_y - bmp->h/2);
    }
}


void object_draw_lit_layers (BITMAP *dest, object_t *obj,
			     int offset_x, int offset_y, int color)
{
    objlayer_t *layer;
    BITMAP *bmp;

    foreach (layer, obj->cvar.layers) {
	bmp = layer->bmp;
	draw_lit_magic_sprite
	    (dest, bmp,
	     obj->cvar.x - offset_x + layer->offset_x - bmp->w/3/2,
	     obj->cvar.y - offset_y + layer->offset_y - bmp->h/2,
	     color);
    }
}


void object_draw_lights (BITMAP *dest, object_t *obj,
			 int offset_x, int offset_y)
{
    objlight_t *light;
    BITMAP *bmp;

    foreach (light, obj->cvar.lights) {
	bmp = light->bmp;
	draw_trans_magic_sprite
	    (dest, bmp,
	     obj->cvar.x - offset_x + light->offset_x - bmp->w/3/2,
	     obj->cvar.y - offset_y + light->offset_y - bmp->h/2);
    }
}


/* Misc.  */


void object_bounding_box (object_t *obj, int *rx1, int *ry1, int *rx2, int *ry2)
{
    objlayer_t *layer;
    int x, y, x1, y1, x2, y2;

    x1 = y1 = x2 = y2 = 0;
    
    foreach (layer, obj->cvar.layers) {
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
