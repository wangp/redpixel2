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


static void (*register_object_hook) (const char *, lua_Object,
				     const char *, const char *);

void set_register_object_hook (void (*hook) (const char *, lua_Object,
					     const char *, const char *))
{
    register_object_hook = hook;
}


static void __export__register_object (void)
    /* (name, table, type, icon) : (nil on error) */
{
    const char *name;
    lua_Object table;
    const char *type;
    const char *icon;

    if (!lua_isstring (lua_getparam (1))
	|| (!lua_istable (lua_getparam (2))
	    && !lua_isnil (lua_getparam (2)))
	|| !lua_isstring (lua_getparam (3))
	|| !lua_isstring (lua_getparam (4)))
	lua_pushnil ();

    name  = lua_getstring (lua_getparam (1));
    table = lua_getparam (2);
    type  = lua_getstring (lua_getparam (3));
    icon  = lua_getstring (lua_getparam (4));

    object_types_register (name, table, type, icon);

    if (register_object_hook)
	register_object_hook (name, table, type, icon);
    
    lua_pushnumber (1);
}


static void __export__set_visual (void)
    /* (obj, bmp) : (none) */
{
    lua_Object obj;
    const char *bmp;
    object_t *p;

    if (!lua_istable (lua_getparam (1)) ||
	!lua_isstring (lua_getparam (2)))
	return;

    obj = lua_getparam (1);
    bmp = lua_getstring (lua_getparam (2));

    lua_pushobject (obj), lua_pushstring ("_parent");
    p = lua_getuserdata (lua_rawgettable ());
    if (!p) return;

    if (p->layer) {
	object_layer_list_destroy (p->layer);
	p->layer = 0;
    }

    p->render = OBJECT_RENDER_MODE_BITMAP;
    p->bitmap = store_dat (bmp);
}


static void __export__set_visual_layers (void)
    /* (obj, visual-rep) : (none) */
{
    lua_Object obj, rep;
    lua_Object elem;
    object_t *p;
    object_layer_list_t *list;
    BITMAP *bmp;
    int i;

    lua_beginblock ();

    if (!lua_istable (obj = lua_getparam (1)) ||
	!lua_istable (rep = lua_getparam (2)))
	goto end;

    lua_pushobject (obj);
    lua_pushstring ("_parent");
    p = lua_getuserdata (lua_rawgettable ());
    if (!p) goto end;
    
    list = object_layer_list_create ();

    for (i = 1; ; i++) {
	lua_pushobject (rep);
	lua_pushnumber (i);
	elem = lua_gettable ();

	if (lua_isstring (elem)) {
	    bmp = store_dat (lua_getstring (elem));
	    if (!bmp) goto error;
	    
	    object_layer_list_add (list, bmp, 0, 0, OBJECT_LAYER_NO_ANGLE);
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
	    if (!lua_isnumber (x = lua_gettable ()))
		angle = ftofix (lua_getnumber (x));

	    object_layer_list_add (list, bmp, offsetx, offsety, angle);
	}
	else if (lua_isnil (elem))
	    break;
	else
	    goto error;
    }

    if (p->layer) {
	object_layer_list_destroy (p->layer);
	p->layer = 0;
    }

    p->render = OBJECT_RENDER_MODE_LAYERED;
    p->layer = list;

    goto end;
    
  error:
    
    object_layer_list_destroy (list);

  end:

    lua_endblock ();
}


void __bindings_object_export_functions ()
{
#define e(func)	(lua_register (#func, __export__##func))

    e (register_object);
    e (set_visual);
    e (set_visual_layers);
    
#undef e
}
