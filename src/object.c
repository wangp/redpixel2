/* object.c
 *
 * Peter Wang <tjaden@users.sourceforge.net>
 */


#include <stdlib.h>
#include <string.h>
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
    int centre_x;
    int centre_y;
} objmask_t;


/* collision flags */
#define CNFLAG_IS_PLAYER	0x01
#define CNFLAG_TOUCH_TILES	0x02
#define CNFLAG_TOUCH_PLAYERS	0x04
#define CNFLAG_TOUCH_NONPLAYERS	0x08
#define CNFLAG_TOUCH_OBJECTS	(CNFLAG_TOUCH_PLAYERS | CNFLAG_TOUCH_NONPLAYERS)
#define CNFLAG_DEFAULT		(CNFLAG_TOUCH_TILES | CNFLAG_TOUCH_OBJECTS)


struct object {
    object_t *next;
    object_t *prev;

    /*
     * Common
     */

    objtype_t *type;
    objid_t id;
    char is_proxy;
    char is_stale;

    float x, y;
    float xv_decay, yv_decay;	/* not replicated */
    float mass;			/* not replicated */
    lua_ref_t table;

    
    /*
     * Non-proxy
     */

    float xv, yv;
    float xa, ya;
    float old_xa, old_ya;

    char _ramp;
    char jump;
    char collision_flags;
    char collision_tag;
    objmask_t mask[OBJECT_MASK_MAX];
    char replication_flags;


    /*
     * Proxy
     */

    /* Our last known authoritative pva received from the server, and
     * the game time at which that was sent. */
    /* [pva == position, velocity, acceleration] */
    struct {
	unsigned long time;
	float x, y;
	float xv, yv;
	float xa, ya;
    } auth;

    /* The target pv, i.e. this is where we think we are supposed to
     * be at a point in time, extrapolated from authoritative pv.
     * This is not where we _actually_ are, which is stored in x, y.
     */
    struct {
	float x, y;
	float xv, yv;
    } target;

    list_head_t layers;
    list_head_t lights;
};


#include "objecttm.inc"


static void set_default_masks (object_t *obj);


static lua_tag_t object_tag;
static objid_t next_id;


int object_init ()
{
    object_tag = lua_newtag (lua_state);
    REGISTER_OBJECT_TAG_METHODS (lua_state, object_tag);
    next_id = OBJID_PLAYER_MAX;
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
    BITMAP *icon;

    if (!(type = objtypes_lookup (type_name)))
	return NULL;

    obj = alloc (sizeof (object_t));

    obj->type = type;
    obj->id = id;

    obj->xv_decay = 1.0;
    obj->yv_decay = 1.0;

    lua_newtable (L);
    obj->table = lua_ref (L, 1);

    set_default_masks (obj);

    obj->collision_flags = CNFLAG_DEFAULT;

    list_init (obj->layers);
    icon = store_dat (objtype_icon (obj->type));
    object_add_layer (obj, objtype_icon (obj->type), 
		      icon->w/3/2, icon->h/2);

    list_init (obj->lights);

    return obj;
}


object_t *object_create_proxy (const char *type_name, objid_t id)
{
    object_t *obj;
    
    obj = object_create_ex (type_name, id);
    if (obj)
	obj->is_proxy = 1;

    return obj;
}


void object_run_init_func (object_t *obj)
{
    lua_State *L = lua_state;

    /* Call base object init hook.  */
    lua_getglobal (L, "_object_init_hook");
    lua_pushobject (L, obj);
    lua_call (L, 1, 0);
    
    /* Call type specific init function, if any.  */
    if (objtype_init_func (obj->type) != LUA_NOREF) {
	lua_getref (L, objtype_init_func (obj->type));
	lua_pushobject (L, obj);
	lua_call (L, 1, 0);
    }
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


int object_is_client (object_t *obj)
{
    return obj->id < OBJID_PLAYER_MAX;
}


inline int object_stale (object_t *obj)
{
    return obj->is_stale;
}


void object_set_stale (object_t *obj)
{
    obj->is_stale = 1;
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


void object_set_xvyv (object_t *obj, float xv, float yv)
{
    obj->xv = xv;
    obj->yv = yv;
}


float object_xa (object_t *obj)
{
    return obj->xa;
}


float object_ya (object_t *obj)
{
    return obj->ya;
}


void object_set_xa (object_t *obj, float xa)
{
    obj->xa = xa;
}


void object_set_ya (object_t *obj, float ya)
{
    obj->ya = ya;
}


float object_xv_decay (object_t *obj)
{
    return obj->xv_decay;
}


float object_yv_decay (object_t *obj)
{
    return obj->yv_decay;
}


float object_mass (object_t *obj)
{
    return obj->mass;
}


void object_set_mass (object_t *obj, float mass)
{
    obj->mass = mass;
}


int object_ramp (object_t *obj)
{
    return obj->_ramp;
}


void object_set_ramp (object_t *obj, int ramp)
{
    obj->_ramp = ramp;
}


int object_jump (object_t *obj)
{
    return obj->jump;
}


void object_set_jump (object_t *obj, int jump)
{
    obj->jump = jump;
}


void object_set_collision_is_player (object_t *obj)
{
    obj->collision_flags |= CNFLAG_IS_PLAYER;
}


void object_set_collision_flags (object_t *obj, int tiles, int players,
				 int nonplayers)
{
    obj->collision_flags &= CNFLAG_IS_PLAYER;
    obj->collision_flags |= ((tiles ? CNFLAG_TOUCH_TILES : 0)
			     | (players ? CNFLAG_TOUCH_PLAYERS : 0)
			     | (nonplayers ? CNFLAG_TOUCH_NONPLAYERS : 0));
}


/* for lua bindings */
void object_set_collision_flags_string (object_t *obj, const char *flags)
{
    int tiles = 0, players = 0, nonplayers = 0;

    for (; *flags; flags++) {
	if (*flags == 't') tiles = 1;
	else if (*flags == 'p') players = 1;
	else if (*flags == 'n') nonplayers = 1;
    }

    object_set_collision_flags (obj, tiles, players, nonplayers);
}


int object_collision_tag (object_t *obj)
{
    return obj->collision_tag;
}


void object_set_collision_tag (object_t *obj, int tag)
{
    obj->collision_tag = tag;
}


int object_need_replication (object_t *obj, int flag)
{
    return obj->replication_flags & flag;
}


void object_set_replication_flag (object_t *obj, int flag)
{
    obj->replication_flags |= flag;
}


void object_clear_replication_flags (object_t *obj)
{
    obj->replication_flags = 0;
}


/* Layers.  */


typedef struct objlayer objlayer_t;

struct objlayer {
    objlayer_t *next;
    objlayer_t *prev;
    int id;
    BITMAP *bmp;
    int centre_x;
    int centre_y;
    int hflip;
    int angle;
};


static int set_layer (objlayer_t *layer, int id, const char *key,
		      int centre_x, int centre_y)
{
    BITMAP *bmp;

    bmp = store_dat (key);
    if (!bmp)
	return -1;

    layer->id = id;
    layer->bmp = bmp;
    layer->centre_x = centre_x;
    layer->centre_y = centre_y;
    return 0;
}


int object_add_layer (object_t *obj, const char *key,
		      int centre_x, int centre_y)
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
    if (set_layer (layer, id, key, centre_x, centre_y) < 0) {
	free (layer);
	return -1;
    }
    list_append (obj->layers, layer);
    return id;
}


int object_replace_layer (object_t *obj, int layer_id, const char *key,
			  int centre_x, int centre_y)
{
    objlayer_t *layer;

    list_for_each (layer, &obj->layers)
	if (layer_id == layer->id) {
	    if (set_layer (layer, layer_id, key, centre_x, centre_y) == 0)
		return 0;
	    break;
	}

    return -1;
}


int object_move_layer (object_t *obj, int layer_id, int centre_x, int centre_y)
{
    objlayer_t *layer;

    list_for_each (layer, &obj->layers)
	if (layer_id == layer->id) {
	    layer->centre_x = centre_x;
	    layer->centre_y = centre_y;
	    return 0;
	}

    return -1;
}


int object_hflip_layer (object_t *obj, int layer_id, int hflip)
{
    objlayer_t *layer;

    list_for_each (layer, &obj->layers)
	if (layer_id == layer->id) {
	    layer->hflip = hflip;
	    return 0;
	}

    return -1;    
}


int object_rotate_layer (object_t *obj, int layer_id, int angle)
{
    objlayer_t *layer;

    list_for_each (layer, &obj->layers)
	if (layer_id == layer->id) {
	    layer->angle = angle;
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
		     int free, int centre_x, int centre_y)
{
    bitmask_ref_t *save = obj->mask[mask_num].ref;

    obj->mask[mask_num].ref = bitmask_ref_create (mask, free);
    obj->mask[mask_num].centre_x = centre_x;
    obj->mask[mask_num].centre_y = centre_y;

    /* Destroy here instead of at start of the function in case the
     * new one is the same as the old one.  */
    bitmask_ref_destroy (save);

    return 0;
}


int object_set_mask (object_t *obj, int mask_num, const char *key,
		     int centre_x, int centre_y)
{
    bitmask_t *mask;
    
    mask = store_dat (key);
    if ((!mask) || (mask_num < 0) || (mask_num > 4))
	return -1;

    return set_mask (obj, mask_num, mask, 0, centre_x, centre_y);
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
    for (i = 0; i < OBJECT_MASK_MAX; i++)
	object_remove_mask (obj, i);
}


static void set_default_masks (object_t *obj)
{
    bitmask_t *mask;
    int xcen, ycen, i;

    mask = objtype_icon_mask (obj->type);
    xcen = bitmask_width (mask) / 2;
    ycen = bitmask_height (mask) / 2;

    for (i = 0; i < OBJECT_MASK_MAX; i++)
	set_mask (obj, i, mask, 0, xcen, ycen);
}


/* Collisions.  */


#define is_player(o)		((o)->collision_flags & CNFLAG_IS_PLAYER)
#define is_nonplayer(o)		(!is_player (o))
#define touch_tiles(o)		((o)->collision_flags & CNFLAG_TOUCH_TILES)
#define touch_players(o)	((o)->collision_flags & CNFLAG_TOUCH_PLAYERS)
#define touch_nonplayers(o)	((o)->collision_flags & CNFLAG_TOUCH_NONPLAYERS)
#define touch_objects(o)	((o)->collision_flags & CNFLAG_TOUCH_OBJECTS)


static int check_collision_with_tiles (object_t *obj, int mask_num, map_t *map, int x, int y)
{
    objmask_t *mask;

    if (!touch_tiles (obj))
	return 0;

    mask = obj->mask;
    if (!mask[mask_num].ref)
	return 0;

    if (bitmask_check_collision (bitmask_ref_bitmask (mask[mask_num].ref),
				 map_tile_mask (map),
				 x - mask[mask_num].centre_x,
				 y - mask[mask_num].centre_y,
				 0, 0)) {
	object_call (obj, "tile_collide_hook");
	return 1;
    }

    return 0;
}


static void call_collide_hook (object_t *obj, object_t *touched_obj)
{
    lua_State *L = lua_state;
    int top = lua_gettop (L);

    lua_pushobject (L, obj);
    lua_pushstring (L, "collide_hook");
    lua_gettable (L, -2);
    if (lua_isfunction (L, -1)) {
	lua_pushvalue (L, -2);
	lua_pushobject (L, touched_obj);
	lua_call (L, 2, 0);
    }

    lua_settop (L, top);
}


static int check_collision_with_objects (object_t *obj, int mask_num,
					 map_t *map, int x, int y)
{
    list_head_t *list;
    objmask_t *mask;
    object_t *p;

    if (object_stale (obj) || !touch_objects (obj))
	return 0;

    mask = obj->mask;
    if (!mask[mask_num].ref)
	return 0;

    list = map_object_list (map);
    list_for_each (p, list) {
	if ((p == obj) || (object_stale (p)) || (!p->mask[0].ref))
	    continue;
	
	if ((p->collision_tag) && (p->collision_tag == obj->collision_tag))
	    continue;
	
	if ((is_player (obj) && !touch_players (p)) ||
	    (is_nonplayer (obj) && !touch_nonplayers (p)))
	    continue;
	
	if (!(bitmask_check_collision
	      (bitmask_ref_bitmask (mask[mask_num].ref),
	       bitmask_ref_bitmask (p->mask[0].ref),
	       x - mask[mask_num].centre_x, y - mask[mask_num].centre_y,
	       p->x - p->mask[0].centre_x,
	       p->y - p->mask[0].centre_y)))
	    continue;

	/* we have a collision! */
	call_collide_hook (p, obj);
	if (object_stale (p)) continue;
	if (object_stale (obj)) return 0;

	call_collide_hook (obj, p);
	if (object_stale (p)) continue;
	if (object_stale (obj)) return 0;

	return 1;
    }
    
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


/* Object movement.  */


#define SIGN(a)	((a < 0) ? -1 : 1)


static inline int object_move (object_t *obj, int mask_num, map_t *map, float dx, float dy)
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


static inline int object_move_x_with_ramp (object_t *obj, int mask_num, map_t *map,
					   float dx, int ramp)
{
    float idx;
    int ir;

    while (dx) {
	idx = (ABS (dx) < 1) ? dx : SIGN (dx);

	for (ir = 0; ir <= ramp; ir++)
	    if (!check_collision (obj, mask_num, map, obj->x+idx, obj->y-ir))
		break;
	if (ir > ramp)
	    return -1;

	obj->x += idx;
	if (ir) {
	    obj->y -= ir;
	    object_set_replication_flag (obj, OBJECT_REPLICATE_UPDATE);
	}

	dx = (ABS (dx) < 1) ? 0 : SIGN (dx) * (ABS (dx) - 1);
    }

    return 0;
}


static inline int too_far_off_map (object_t *obj, map_t *map)
{
    const float margin = 160;
    return ((object_x (obj) < -margin) || (object_y (obj) < -margin)
	    || (object_x (obj) > (map_width (map) * 16 + margin))
	    || (object_y (obj) > (map_height (map) * 16 + margin)));
}


void object_do_physics (object_t *obj, map_t *map)
{
    int rep = 0;
    float old_yv = obj->yv;

    obj->ya += obj->mass;

    obj->xv = (obj->xv + obj->xa) * obj->xv_decay;
    obj->yv = (obj->yv + obj->ya) * obj->yv_decay;

    if (obj->xv != 0) {
	if (obj->_ramp) {
	    if (object_move_x_with_ramp (obj, ((obj->xv < 0) ? OBJECT_MASK_LEFT : OBJECT_MASK_RIGHT),
					 map, obj->xv, obj->_ramp) < 0) {
		/* object stopped short of an entire xv */
		obj->xa = 0;
		obj->xv = 0;
		rep = 1;
	    }
	}
	else {
	    if (object_move (obj, ((obj->xv < 0) ? OBJECT_MASK_LEFT : OBJECT_MASK_RIGHT), map, obj->xv, 0)) {
		obj->xa = 0;
		obj->xv = 0;
		rep = 1;
	    }
	}
    }

    if (obj->yv != 0) {
	if (object_move (obj, ((obj->yv < 0) ? OBJECT_MASK_TOP : OBJECT_MASK_BOTTOM), map, 0, obj->yv)) {
	    obj->ya = 0;
	    obj->yv = 0;
	    if (old_yv) rep = 1;
	}
    }

    if ((obj->old_xa != obj->xa) || (obj->old_ya != obj->ya)) {
	obj->old_xa = obj->xa;
	obj->old_ya = obj->ya;
	rep = 1;
    }

    if ((obj->id >= OBJID_PLAYER_MAX) && (too_far_off_map (obj, map)))
	obj->is_stale = 1;
    else if (rep)
	obj->replication_flags |= OBJECT_REPLICATE_UPDATE;
}


/* Proxy simulation.  */


void object_set_auth_info (object_t *obj,
			   unsigned long time,
			   float x, float y,
			   float xv, float yv,
			   float xa, float ya)
{
    obj->auth.time = time;
    obj->auth.x = x;
    obj->auth.y = y;
    obj->auth.xv = xv;
    obj->auth.yv = yv;
    obj->auth.xa = xa;
    obj->auth.ya = ya;
}


void object_do_simulation (object_t *obj, unsigned long curr_time)
{
    int interval;

    obj->target.x = obj->auth.x;
    obj->target.y = obj->auth.y;
    obj->target.xv = obj->auth.xv;
    obj->target.yv = obj->auth.yv;

    /* testing */
    if (0) {
	obj->x = obj->auth.x;
	obj->y = obj->auth.y;
	return;
    }

    if (!obj->auth.xv && !obj->auth.yv && !obj->auth.xa && !obj->auth.ya) {
	obj->x = obj->auth.x;
	obj->y = obj->auth.y;
	return;
    }

    interval = curr_time - obj->auth.time;
    while (interval-- > 0) {
	obj->target.xv = (obj->target.xv + obj->auth.xa) * obj->xv_decay;
	obj->target.yv = (obj->target.yv + obj->auth.ya) * obj->yv_decay;
	obj->target.x += obj->target.xv;
	obj->target.y += obj->target.yv;
    }

    if (1) {
	obj->x += ((obj->target.x - obj->x) * 0.8);
	obj->y += ((obj->target.y - obj->y) * 0.8);
    }
    else {
	obj->x = obj->target.x;
	obj->y = obj->target.y;
    }
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
    int top = lua_gettop (L);

    lua_pushobject (L, obj);
    lua_pushstring (L, method);
    lua_gettable (L, -2);
    if (lua_isfunction (L, -1)) {
	lua_pushvalue (L, -2);
	lua_call (L, 1, 0);
    }

    lua_settop (L, top);
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
	if (layer->angle == 0) {
	    if (!layer->hflip)
		draw_magic_sprite (dest, bmp,
		   obj->x - offset_x - layer->centre_x,
		   obj->y - offset_y - layer->centre_y);
	    else
		draw_magic_sprite_h_flip (dest, bmp,
		   obj->x - offset_x - (bmp->w / 3 - layer->centre_x),
		   obj->y - offset_y - layer->centre_y);
	} else {
	    if (!layer->hflip)
		pivot_magic_sprite (dest, bmp, 
		   obj->x - offset_x, obj->y - offset_y,
		   layer->centre_x, layer->centre_y,
		   layer->angle << 16);
	    else
		pivot_magic_sprite_v_flip (dest, bmp, 
		   obj->x - offset_x, obj->y - offset_y,
		   layer->centre_x, bmp->h - layer->centre_y,
		   (layer->angle + 128) << 16);
	}
    }
}


void object_draw_lit_layers (BITMAP *dest, object_t *obj,
			     int offset_x, int offset_y, int color)
{
    objlayer_t *layer;
    BITMAP *bmp;

    list_for_each (layer, &obj->layers) {
	bmp = layer->bmp;
	if ((layer->angle != 0) && (!layer->hflip))
	    pivot_lit_magic_sprite (dest, bmp, 
	       obj->x - offset_x, obj->y - offset_y,
	       layer->centre_x, layer->centre_y,
	       layer->angle << 16, color);
	else if (!layer->hflip)
	    draw_lit_magic_sprite (dest, bmp,
	       obj->x - offset_x - layer->centre_x,
	       obj->y - offset_y - layer->centre_y, color);
	else
	    pivot_lit_magic_sprite_v_flip (dest, bmp, 
	       obj->x - offset_x, obj->y - offset_y,
	       layer->centre_x, bmp->h - layer->centre_y,
	       (layer->angle + 128) << 16, color);
    }
}


void object_draw_lights (BITMAP *dest, object_t *obj,
			 int offset_x, int offset_y)
{
    objlight_t *light;
    BITMAP *bmp;

    list_for_each (light, &obj->lights) {
	bmp = light->bmp;
	draw_trans_magic_sprite (dest, bmp,
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
	x = -layer->centre_x;
	y = -layer->centre_y;

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
