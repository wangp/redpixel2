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


static unsigned int next_id;


object_t *object_create (const char *type_name)
{
    lua_State *L = lua_state;
    objtype_t *type;
    object_t *obj;
    
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
    pushstring (L, "object");
    pushuserdata (L, obj);
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
