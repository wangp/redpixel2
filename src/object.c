/* object.c
 *
 * Peter Wang <tjaden@psynet.net>
 */


#include <stdlib.h>
#include <lua.h>
#include "alloc.h"
#include "bitmask.h"
#include "object.h"
#include "store.h"


static bitmask_t *bitmask_create_from_magic_bitmap (BITMAP *bmp);


object_t *object_create (const char *type_name)
{
    object_t *p;
    object_type_t *type;
    
    type = object_types_lookup (type_name);
    if (!type) return 0;

    p = alloc (sizeof (object_t));
    
    if (p) {
	p->type = type;

	lua_beginblock ();

	lua_pushobject (lua_createtable ());
	p->self = lua_ref (1);

	lua_pushobject (lua_getref (p->self));
	lua_pushstring ("_parent");
	lua_pushuserdata (p);
	lua_rawsettable ();

	p->render = OBJECT_RENDER_MODE_BITMAP;
	p->bitmap = store_dat (p->type->icon);
	p->mask = bitmask_create_from_magic_bitmap (p->bitmap);
		/* XXX: memory wastage and leak */

    	/* XXX */
	/* init (self, type-name) : (no output) */
	if (lua_istable (lua_getref (p->type->table))) {
	    lua_Object init;

	    lua_pushobject (lua_getref (p->type->table));
	    lua_pushstring ("init");
	    init = lua_gettable ();

	    if (lua_isfunction (init)) {
		lua_pushobject (lua_getref (p->self));
		lua_pushstring (p->type->name);
		lua_callfunction (init);
	    }
	}

	lua_endblock ();
    }
	
    return p;
}


void object_destroy (object_t *p)
{
    object_free_render_data (p);
    lua_unref (p->self);
    free (p);
}


void object_set_render_mode (object_t *p, int mode, void *data)
{
    object_free_render_data (p);

    p->render = mode;
    switch (mode) {
	case OBJECT_RENDER_MODE_BITMAP: p->bitmap = data; break;
	case OBJECT_RENDER_MODE_IMAGE: p->image = data; break;
	case OBJECT_RENDER_MODE_ANIM: p->anim = data; break;
    }
}


void object_free_render_data (object_t *p)
{
    if (p->bitmap)
	p->bitmap = 0;

    if (p->image)
	object_image_destroy (p->image), p->image = 0;

    if (p->anim)
	object_anim_destroy (p->anim), p->anim = 0;
}



static bitmask_t *bitmask_create_from_magic_bitmap (BITMAP *bmp)
{
    bitmask_t *mask;
    int x, y;

    mask = bitmask_create (bmp->w / 3, bmp->h);

    if (mask)
	for (y = 0; y < bmp->h; y++)
	    for (x = 0; x < bmp->w / 3; x++)
		bitmask_set_point (mask, x, y,
				   (bmp->line[y][x * 3    ] ||
				    bmp->line[y][x * 3 + 1] ||
				    bmp->line[y][x * 3 + 2]));

    return mask;
}
