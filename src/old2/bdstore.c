/* bdstore.c - Lua export functions (store)
 *
 * Peter Wang <tjaden@users.sourceforge.net>
 */


#include <lua.h>
#include <allegro.h>
#include "bindings.h"
#include "extdata.h"
#include "luahelp.h"
#include "path.h"
#include "store.h"


static int __export__store_load (LS)
    /* (filename, prefix) : (nil on error) */
{
    const char *filename, *prefix;
    char **p, tmp[1024];
    
    if (!lua_checkargs (L, "ss"))
	goto error;
    filename = lua_tostring (L, 1);
    prefix = lua_tostring (L, 2);

    for (p = path_share; *p; p++) {
	ustrncpy (tmp, *p, sizeof tmp);
	ustrncat (tmp, filename, sizeof tmp);

	if (store_load_ex (tmp, prefix, load_extended_datafile) >= 0) {
	    lua_pushnumber (L, 1);
	    return 1;
	}
    }

  error:
    
    lua_pushnil (L);
    return 1;
}



static int __export__store_dat (lua_State *L)
    /* (name) : (data) */
{
    if (lua_isstring (L, 1))
	lua_pushuserdata (L, store_dat (lua_tostring (L, 1)));
    else
	lua_pushnil (L);
    return 1;
}


void __bindings_store_export_functions (lua_State *L)
{
#define e(func)	(lua_register (L, #func, __export__##func))

    e (store_load);
    e (store_dat);

#undef e
}
