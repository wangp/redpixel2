/* bdobject.c - Lua export functions (object)
 *
 * Peter Wang <tjaden@psynet.net>
 */


#include <lua.h>
#include "bindings.h"
#include "map.h"
#include "object.h"
#include "objtypes.h"
#include "store.h"


#define getnumber(n)	(lua_getnumber (lua_getparam (n)))
#define getparam(n)	(lua_getparam (n))
#define getstring(n)	(lua_getstring (lua_getparam (n)))
#define gettable(n)	(lua_getparam (n))

#define isnil(n)	(lua_isnil (lua_getparam (n)))
#define isnumber(n)	(lua_isnumber (lua_getparam (n)))
#define isstring(n)	(lua_isstring (lua_getparam (n)))
#define istable(n)	(lua_istable (lua_getparam (n)))

static void *get_element (lua_Object table, const char *index)
{
    lua_pushobject (table);
    lua_pushstring (index);
    return lua_rawgettable ();
}

#define get_userdata_element(t,i)  (lua_getuserdata (get_element (t, i)))

/*----------------------------------------------------------------------*/

static void (*object_type_register_hook) (const char *, lua_Object,
					  const char *, const char *);

void set_object_type_register_hook (void (*hook) (const char *, lua_Object,
						  const char *, const char *))
{
    object_type_register_hook = hook;
}


static void __export__object_type_register (void)
    /* (name, table, type, icon) : (nil on error) */
{
    const char *name;
    lua_Object table;
    const char *type;
    const char *icon;

    if (!isstring (1)
	|| (!istable (2) && !isnil (2))
	|| !isstring (3)
	|| !isstring (4))
	lua_pushnil ();

    name  = getstring (1);
    table = getparam  (2);
    type  = getstring (3);
    icon  = getstring (4);

    object_types_register (name, table, type, icon);

    if (object_type_register_hook)
	object_type_register_hook (name, table, type, icon);

    lua_pushnumber (1);
}


static void __export__object_set_visual_bitmap (void)
    /* (obj, bmp) : (none) */
{
    lua_Object obj;
    const char *bmp;
    object_t *p;

    if (!istable (1) || !isstring (2))
	return;
    obj = gettable (1);
    bmp = getstring (2);

    p = get_userdata_element (obj, "_parent");
    if (!p) return;

    object_set_render_mode (p, OBJECT_RENDER_MODE_BITMAP, store_dat (bmp));
}


static void __export__object_set_visual_image (void)
    /* (obj, visual-rep) : (none) */
{
    lua_Object obj, rep;
    lua_Object elem;
    object_t *p;
    object_image_t *image;
    BITMAP *bmp;
    int i;

    if (!istable (1) || !istable (2)) 
	return;

    obj = gettable (1);
    rep = gettable (2);

    p = get_userdata_element (obj, "_parent");
    if (!p) return;
    
    image = object_image_create ();

    for (i = 1; ; i++) {
	lua_pushobject (rep);
	lua_pushnumber (i);
	elem = lua_gettable ();

	if (lua_isstring (elem)) {
	    bmp = store_dat (lua_getstring (elem));
	    if (!bmp) goto error;
	    
	    object_image_add_layer (image, bmp, 0, 0, OBJECT_LAYER_NO_ANGLE);
	}
	else if (lua_istable (elem)) {
	    lua_Object x;
	    int offsetx = 0, offsety = 0;
	    fixed angle = OBJECT_LAYER_NO_ANGLE;

	    lua_pushobject (elem), lua_pushnumber (1);
	    if (!lua_isstring (x = lua_gettable ()))
		goto error;
	    
	    bmp = store_dat (lua_getstring (x));
	    if (!bmp) goto error;
	    
	    lua_pushobject (elem), lua_pushstring ("offsetx");
	    if (lua_isnumber (x = lua_gettable ()))
		offsetx = lua_getnumber (x);

	    lua_pushobject (elem), lua_pushstring ("offsety");
	    if (lua_isnumber (x = lua_gettable ()))
		offsety = lua_getnumber (x);

	    lua_pushobject (elem), lua_pushstring ("angle");
	    if (lua_isnumber (x = lua_gettable ()))
		angle = ftofix (lua_getnumber (x));

	    object_image_add_layer (image, bmp, offsetx, offsety, angle);
	}
	else if (lua_isnil (elem))
	    break;
	else
	    goto error;
    }

    if (p->image) {
	object_image_destroy (p->image);
	p->image = 0;
    }

    object_set_render_mode (p, OBJECT_RENDER_MODE_IMAGE, image);
    return;
    
  error:

    object_image_destroy (image);
}


static void __export__object_set_visual_anim (void)
    /* (obj, visual-rep, time) : (none) */
{
    lua_Object obj, rep, elem;
    float time;
    object_t *p;
    object_anim_t *anim;
    BITMAP *bmp;
    int i;

    if (!istable (1) || !istable (2) || !isnumber (3))
	return;

    obj = gettable (1);
    rep = gettable (2);
    time = getnumber (3);

    p = get_userdata_element (obj, "_parent");
    if (!p) return;

    anim = object_anim_create ();

    for (i = 1; ; i++) {
	lua_pushobject (rep);
	lua_pushnumber (i);
	elem = lua_gettable ();

	if (lua_isstring (elem)) {
	    bmp = store_dat (lua_getstring (elem));
	    if (!bmp) goto error;

	    object_anim_add_frame (anim, bmp, time * 60); /* XXX */
	}
	else if (lua_isnil (elem))
	    break;
	else
	    goto error;
    }

    object_set_render_mode (p, OBJECT_RENDER_MODE_ANIM, anim);
    return;

  error:

    object_anim_destroy (anim);
}


static void __export__object_set_collision_masks (void)
    /* (obj, mask, mask-top, mask-bottom, mask-left, mask-right) : (none) */
{
    lua_Object obj;
    object_t *p;
    bitmask_t *mask;
    int i;

    if (!istable (1)) return;
    obj = gettable (1);

    p = get_userdata_element (obj, "_parent");
    if (!p) return;

    for (i = 2; i <= 6; i++) {
	mask = isstring (i) ? (store_dat (getstring (i))) : 0;

	switch (i) {
	    case 2: p->mask        = mask; break;
	    case 3: p->mask_top    = mask; break;
	    case 4: p->mask_bottom = mask; break;
	    case 5: p->mask_left   = mask; break;
	    case 6: p->mask_right  = mask; break;
	}
    }
}


static void __export__object_destroy (void)
    /* (obj) : (none) */
{
    object_t *p;

    if (istable (1)) {
	p = get_userdata_element (gettable (1), "_parent");
	if (p)
	    p->dying = 1;
    }
}


void __bindings_object_export_functions ()
{
#define e(func)	(lua_register (#func, __export__##func))

    e (object_type_register);
    e (object_set_visual_bitmap);
    e (object_set_visual_image);
    e (object_set_visual_anim);
    e (object_set_collision_masks);
    e (object_destroy);

#undef e
}
