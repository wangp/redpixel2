/* bdstore.c - Lua export functions (store)
 *
 * Peter Wang <tjaden@psynet.net>
 */


#include <lua.h>
#include <allegro.h>
#include "bindings.h"
#include "extdata.h"
#include "path.h"
#include "store.h"


static void __export__store_load (void)
    /* (filename, prefix) : (zero on success) */
{
    const char *filename, *prefix;
    char **p, tmp[1024];
    
    if (!lua_isstring (lua_getparam (1)) ||
	!lua_isstring (lua_getparam (2)))
	goto error;

    filename = lua_getstring (lua_getparam (1));
    prefix = lua_getstring (lua_getparam (2));

    for (p = path_share; *p; p++) {
	ustrncpy (tmp, *p, sizeof tmp);
	ustrncat (tmp, filename, sizeof tmp);

	if (store_load_ex (tmp, prefix, load_extended_datafile) >= 0) {
	    lua_pushnumber (0);
	    return;
	}
    }

  error:
    
    lua_pushnumber (-1);
}



static void __export__store_dat (void)
    /* (name) : (data) */
{
    if (!lua_isstring (lua_getparam (1)))
	lua_pushnil ();

    lua_pushuserdata (store_dat (lua_getstring (lua_getparam (1))));
}


void __bindings_store_export_functions ()
{
#define e(func)	(lua_register (#func, __export__##func))

    e (store_load);
    e (store_dat);

#undef e
}
