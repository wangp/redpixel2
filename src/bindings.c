/* bindings.c
 *
 * Peter Wang <tjaden@psynet.net>
 */


#include "bindings.h"
#include "mylua.h"


/*----------------------------------------------------------------------*/
/* Store bindings.  							*/
/*----------------------------------------------------------------------*/


#include <allegro.h>
#include "extdata.h"
#include "path.h"
#include "store.h"


static int bind_store_load (LS)
    /* (filename, prefix) : (nil on error) */
{
    const char *filename;
    const char *prefix;
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


/*----------------------------------------------------------------------*/
/* Object types bindings.  						*/
/*----------------------------------------------------------------------*/


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


/*----------------------------------------------------------------------*/
/* Object bindings.  							*/
/*----------------------------------------------------------------------*/


#include "object.h"


/* Layers.  */


static int bind_object_add_layer (LS)
    /* (object, key, x offset, y offset) : (layer id on success) */
{
    object_t *obj;
    int id;

    if (!checkargs (L, "tsnn"))
	goto error;
    if (!(obj = table_object (L, 1)))
	goto error;

    if ((id = object_add_layer (obj, tostring (L, 2), tonumber (L, 3),
				tonumber (L, 4))) < 0)
	goto error;

    pushnumber (L, id);
    return 1;

  error:

    pushnil (L);
    return 1;
}


static int bind_object_replace_layer (LS)
    /* (object, layer id, key, x offset, y offset) : (nil on error) */
{
    object_t *obj;

    if (!checkargs (L, "tnsnn"))
	goto error;
    if (!(obj = table_object (L, 1)))
	goto error;

    if (object_replace_layer (obj, tonumber (L, 2), tostring (L, 3),
			      tonumber (L, 4), tonumber (L, 5)) < 0)
	goto error;

    pushnumber (L, 1);
    return 1;

  error:

    pushnil (L);
    return 1;
}


static int bind_object_move_layer (LS)
    /* (object, layer id, x offset, y offset) : (nil on error) */
{
    object_t *obj;

    if (!checkargs (L, "tnnn"))
	goto error;
    if (!(obj = table_object (L, 1)))
	goto error;

    if (object_move_layer (obj, tonumber (L, 2), tonumber (L, 3),
			   tonumber (L, 4)) < 0)
	goto error;

    pushnumber (L, 1);
    return 1;

  error:

    pushnil (L);
    return 1;
}


static int bind_object_remove_layer (LS)
    /* (object, layer id) : (nil on error) */
{
    object_t *obj;

    if (!checkargs (L, "tn"))
	goto error;
    if (!(obj = table_object (L, 1)))
	goto error;

    if (object_remove_layer (obj, tonumber (L, 2)) < 0)
	goto error;

    pushnumber (L, 1);
    return 1;

  error:

    pushnil (L);
    return 1;
}


static int bind_object_remove_all_layers (LS)
    /* (object) : (nil on error) */
{
    object_t *obj;

    if (!istable (L, 1))
	goto error;
    if (!(obj = table_object (L, 1)))
	goto error;
	
    object_remove_all_layers (obj);

    pushnumber (L, 1);
    return 1;

  error:

    pushnil (L);
    return 1;
}


/* Lights.  */


static int bind_object_add_light (LS)
    /* (object, key, x offset, y offset) : (light id on success) */
{
    object_t *obj;
    int id;

    if (!checkargs (L, "tsnn"))
	goto error;
    if (!(obj = table_object (L, 1)))
	goto error;

    if ((id = object_add_light (obj, tostring (L, 2), tonumber (L, 3),
				tonumber (L, 4))) < 0)
	goto error;

    pushnumber (L, id);
    return 1;

  error:

    pushnil (L);
    return 1;
}


static int bind_object_replace_light (LS)
    /* (object, light id, key, x offset, y offset) : (nil on error) */
{
    object_t *obj;

    if (!checkargs (L, "tnsnn"))
	goto error;
    if (!(obj = table_object (L, 1)))
	goto error;

    if (object_replace_light (obj, tonumber (L, 2), tostring (L, 3),
			      tonumber (L, 4), tonumber (L, 5)) < 0)
	goto error;

    pushnumber (L, 1);
    return 1;

  error:

    pushnil (L);
    return 1;
}


static int bind_object_move_light (LS)
    /* (object, light id, x offset, y offset) : (nil on error) */
{
    object_t *obj;

    if (!checkargs (L, "tnnn"))
	goto error;
    if (!(obj = table_object (L, 1)))
	goto error;

    if (object_move_light (obj, tonumber (L, 2), tonumber (L, 3),
			   tonumber (L, 4)) < 0)
	goto error;

    pushnumber (L, 1);
    return 1;

  error:

    pushnil (L);
    return 1;
}


static int bind_object_remove_light (LS)
    /* (object, light id) : (nil on error) */
{
    object_t *obj;

    if (!checkargs (L, "tn"))
	goto error;
    if (!(obj = table_object (L, 1)))
	goto error;

    if (object_remove_light (obj, tonumber (L, 2)) < 0)
	goto error;

    pushnumber (L, 1);
    return 1;

  error:

    pushnil (L);
    return 1;
}


static int bind_object_remove_all_lights (LS)
    /* (object) : (nil on error) */
{
    object_t *obj;

    if (!istable (L, 1))
	goto error;
    if (!(obj = table_object (L, 1)))
	goto error;
	
    object_remove_all_lights (obj);

    pushnumber (L, 1);
    return 1;

  error:

    pushnil (L);
    return 1;
}


/* Collision.  */


static int bind_object_set_collision_mask (LS)
    /* (object, key, offsetx, offsety) : (nil on error) */
{
    object_t *obj;

    if (!checkargs (L, "tsnn"))
	goto error;
    if (!(obj = table_object (L, 1)))
	goto error;

    if (object_set_collision_mask (obj, tostring (L, 2),
				   tonumber (L, 3), tonumber (L, 4)) < 0)
	goto error;

    pushnumber (L, 1);
    return 1;
    
  error:

    pushnil (L);
    return 1;    
}


static int bind_object_remove_collision_mask (LS)
    /* (object) : (nil on error) */
{
    object_t *obj;

    if (!istable (L, 1))
	goto error;
    if (!(obj = table_object (L, 1)))
	goto error;
	
    object_remove_collision_mask (obj);

    pushnumber (L, 1);
    return 1;

  error:

    pushnil (L);
    return 1;
}


/*----------------------------------------------------------------------*/
/* Module stuff.  							*/
/*----------------------------------------------------------------------*/


void bindings_init ()
{
    lua_State *L = lua_state;

    lregister (L, "store_load", bind_store_load);
    
    lregister (L, "objtype_register", bind_objtype_register);
    
    lregister (L, "object_add_layer", bind_object_add_layer);
    lregister (L, "object_replace_layer", bind_object_replace_layer);
    lregister (L, "object_move_layer", bind_object_move_layer);
    lregister (L, "object_remove_layer", bind_object_remove_layer);
    lregister (L, "object_remove_all_layers", bind_object_remove_all_layers);
    lregister (L, "object_add_light", bind_object_add_light);
    lregister (L, "object_replace_light", bind_object_replace_light);
    lregister (L, "object_move_light", bind_object_move_light);
    lregister (L, "object_remove_light", bind_object_remove_light);
    lregister (L, "object_remove_all_lights", bind_object_remove_all_lights);
    lregister (L, "object_set_collision_mask", bind_object_set_collision_mask);
    lregister (L, "object_remove_collision_mask", bind_object_remove_collision_mask);
}


void bindings_shutdown ()
{
}
