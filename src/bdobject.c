/* bdobject.c - Lua export functions (object)
 *
 * Peter Wang <tjaden@psynet.net>
 */


#include <lua.h>
#include "bindings.h"
#include "map.h"
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

    if (!lua_istable (lua_getparam (1))
	|| !lua_isstring (lua_getparam (2)))
	return;

    obj = lua_getparam (1);
    bmp = lua_getstring (lua_getparam (2));

    lua_pushobject (obj);
    lua_pushstring ("_parent");
    p = lua_getuserdata (lua_rawgettable ());

    if (p) p->visual = store_dat (bmp);
}


void __bindings_object_export_functions ()
{
#define e(func)	(lua_register (#func, __export__##func))

    e (register_object);
    e (set_visual);
    
#undef e
}
