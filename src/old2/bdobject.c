/* bdobject.c - Lua export functions (object)
 *
 * Peter Wang <tjaden@psynet.net>
 */


#include <lua.h>
#include "bindings.h"
#include "gamenet.h"
#include "luahelp.h"
#include "map.h"
#include "object.h"
#include "objtypes.h"
#include "store.h"


/*
 *  Export functions.  The comments indicate inputs and outputs.
 */

static int export_object_type_register (lua_State *L)
    /* (name, table, type, icon) : (nil on error) */
{
    if (!lua_checkargs (L, "s[tN]ss")) {
	lua_pushnil (L);
	return 1;
    }

    object_types_register (lua_tostring (L, 1), lua_ref (L, 2),
			   lua_tostring (L, 3), lua_tostring (L, 4));
    lua_pushnumber (L, 1);
    return 1;
}


static int export_object_set_visual_bitmap (lua_State *L)
    /* (object, bitmap) : (none) */
{
    object_t *obj;
    const char *bmp;

    if (!lua_checkargs (L, "ts")) return 0;

    lua_pushstring (L, "_parent");
    lua_gettable (L, 1);
    obj = lua_touserdata (L, -1);
    if (!obj) return 0;

    bmp = lua_tostring (L, 2);

    object_set_render_mode (obj, OBJECT_RENDER_MODE_BITMAP, store_dat (bmp));
    return 0;
}


static int export_object_set_visual_image (lua_State *L)
    /* (object, visual-rep) : (none) */
{
    object_t *p;
    object_image_t *image;
    BITMAP *bmp;
    int i;

    if (!lua_checkargs (L, "tt"))
	return 0;

    lua_pushstring (L, "_parent");
    lua_gettable (L, 1);
    p = lua_touserdata (L, -1);
    if (!p) return 0;
    
    image = object_image_create ();

    for (i = 1; ; i++) {
	lua_rawgeti (L, 2, i);
	/* visual-rep element i is at the top of the stack */

	if (lua_isstring (L, -1)) {
	    bmp = store_dat (lua_tostring (L, -1));
	    if (!bmp) goto error;
	    object_image_add_layer (image, bmp, 0, 0, OBJECT_LAYER_NO_ANGLE);
	}
	else if (lua_istable (L, -1)) {
	    int offsetx = 0, offsety = 0;
	    fixed angle = OBJECT_LAYER_NO_ANGLE;

	    lua_rawgeti (L, -1, 1);
	    if (!lua_isstring (L, -1)) goto error;
	    bmp = store_dat (lua_tostring (L, -1));
	    if (!bmp) goto error;
	    lua_pop (L, 1);
	    
	    lua_pushstring (L, "offsetx");
	    lua_gettable (L, -2);
	    if (lua_isnumber (L, -1))
		offsetx = lua_tonumber (L, -1);
	    lua_pop (L, 1);
	    
	    lua_pushstring (L, "offsety");
	    lua_gettable (L, -2);
	    if (lua_isnumber (L, -1))
		offsety = lua_tonumber (L, -1);
	    lua_pop (L, 1);

	    lua_pushstring (L, "angle");
	    lua_gettable (L, -2);
	    if (lua_isnumber (L, -1))
		angle = ftofix (lua_tonumber (L, -1));
	    lua_pop (L, 1);

	    object_image_add_layer (image, bmp, offsetx, offsety, angle);
	}
	else if (lua_isnil (L, -1))
	    break;
	else
	    goto error;
    }

    object_set_render_mode (p, OBJECT_RENDER_MODE_IMAGE, image);
    return 0;
    
  error:

    object_image_destroy (image);
    return 0;
}


static int export_object_set_visual_anim (lua_State *L)
    /* (object, visual-rep, time) : (none) */
{
    object_t *p;
    object_anim_t *anim;
    BITMAP *bmp;
    int i;

    if (!lua_checkargs (L, "ttn"))
	return 0;

    lua_pushstring (L, "_parent");
    lua_gettable (L, 1);
    p = lua_touserdata (L, -1);
    if (!p) return 0;

    anim = object_anim_create ();

    for (i = 1; ; i++) {
	lua_rawgeti (L, 2, i);
	/* visual-rep element i now on top of stack */

	if (lua_isstring (L, -1)) {
	    bmp = store_dat (lua_tostring (L, -1));
	    if (!bmp) goto error;
	    /* XXX: 60 hardcoded */
	    object_anim_add_frame (anim, bmp, lua_tonumber (L, 3) * 60);
	}
	else if (lua_isnil (L, -1))
	    break;
	else
	    goto error;

	lua_pop (L, 1);
    }

    object_set_render_mode (p, OBJECT_RENDER_MODE_ANIM, anim);
    return 0;

  error:

    object_anim_destroy (anim);
    return 0;
}


static int export_object_set_collision_masks (lua_State *L)
    /* (object, mask, mask-top, mask-bottom, mask-left, mask-right) : (none) */
    /* XXX: replicate to client  */
{
    object_t *p;
    bitmask_t *mask;
    int i;

    if (!lua_istable (L, 1)) return 0;

    lua_pushstring (L, "_parent");
    lua_gettable (L, 1);
    p = lua_touserdata (L, -1);
    if (!p) return 0;
    lua_pop (L, 1);

    for (i = 2; i <= 6; i++) {
	mask = (lua_isstring (L, i)) ? (store_dat (lua_tostring (L, i))) : 0;
	switch (i) {
	    case 2: p->mask        = mask; break;
	    case 3: p->mask_top    = mask; break;
	    case 4: p->mask_bottom = mask; break;
	    case 5: p->mask_left   = mask; break;
	    case 6: p->mask_right  = mask; break;
	}
    }

    return 0;
}


static int export_object_destroy (LS)
    /* (object) : (none) */
{
    object_t *p;
    
    if (lua_istable (L, 1)) {
	lua_pushstring (L, "_parent");
	lua_gettable (L, 1);
	p = lua_touserdata (L, -1);
	
	if ((p) && (p->role == OBJECT_ROLE_AUTHORITY)) {
	    p->dying = 1;
	    gamenet_server_replicate_object_destroy (p->id);
	}
    }

    return 0;
}


void __bindings_object_export_functions (lua_State *L)
{
#define e(func)	(lua_register (L, #func, export_##func))

    e (object_type_register);
    e (object_set_visual_bitmap);
    e (object_set_visual_image);
    e (object_set_visual_anim);
    e (object_set_collision_masks);
    e (object_destroy);

#undef e
}
