/* bindings.c
 *
 * Peter Wang <tjaden@psynet.net>
 */


#include "bindings.h"
#include "mylua.h"


/* Store bindings.  */

#include <allegro.h>
#include "extdata.h"
#include "path.h"
#include "store.h"

static int bind_store_load (LS)
    /* (filename, prefix) : (nil on error) */
{
    const char *filename, *prefix;
    char **p, tmp[1024];
    
    if (!checkargs (L, "ss"))
	goto error;
    filename = tostring (L, 1);
    prefix = tostring (L, 2);

    for (p = path_share; *p; p++) {
	ustrncpy (tmp, *p, sizeof tmp);
	ustrncat (tmp, filename, sizeof tmp);

	if (store_load_ex (tmp, prefix, load_extended_datafile) >= 0) {
	    pushnumber (L, 1);
	    return 1;
	}
    }

  error:

    pushnil (L);
    return 1;
}


/* Object types bindings.  */

#include "objtypes.h"

static int bind_objtype_register (LS)
    /* (type, name, icon, init-func or nil) : (nil on error) */
{
    if (!checkargs (L, "sss[fN-]"))
	pushnil (L);
    else {
	if ((isnil (L, 4)) || (isnull (L, 4)))
	    objtypes_register (tostring (L, 1), tostring (L, 2),
			       tostring (L, 3), LUA_NOREF);
	else
	    objtypes_register (tostring (L, 1), tostring (L, 2),
			       tostring (L, 3), ref (L, 4));
	    
	pushnumber (L, 1);
    }
    return 1;
}


/* Object bindings.  */

#include "object.h"

static int bind_object_set_light_source (LS)
    /* (object, lightmap key or nil) : (nil on error) */
{
    object_t *obj;
    const char *key = NULL;

    if (!checkargs (L, "t[sN]"))
	goto error;
    if (!(obj = table_object (L, 1)))
	goto error;

    if (isstring (L, 2))
	key = tostring (L, 2);
    if (object_set_light_source (obj, key) < 0)
	goto error;

    pushnumber (L, 1);
    return 1;

  error:

    pushnil (L);
    return 1;
}


/* Module stuff.  */

void bindings_init ()
{
    lua_State *L = lua_state;
    lregister (L, "store_load", bind_store_load);
    lregister (L, "objtype_register", bind_objtype_register);
    lregister (L, "object_set_light_source", bind_object_set_light_source);
}

void bindings_shutdown ()
{
}
