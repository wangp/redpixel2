/* object.c
 *
 * Peter Wang <tjaden@psynet.net>
 */


#include <stdlib.h>
#include "alloc.h"
#include "bitmask.h"
#include "mylua.h"
#include "object.h"
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

    if (!(obj = alloc (sizeof (object_t))))
	return NULL;

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
    free (obj);
}


int object_set_light_source (object_t *obj, const char *key)
{
    BITMAP *bmp = NULL;

    if ((key) && !(bmp = store_dat (key)))
	return -1;
    obj->cvar.light_source = bmp;
    return 0;
}


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
