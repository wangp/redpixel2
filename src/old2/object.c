/* object.c
 *
 * Peter Wang <tjaden@users.sourceforge.net>
 */


#include <stdlib.h>
#include <lua.h>
#include "alloc.h"
#include "bitmask.h"
#include "luahelp.h"
#include "object.h"
#include "store.h"


static bitmask_t *bitmask_create_from_magic_bitmap (BITMAP *bmp);


static unsigned int id;


static void set_role (LS, object_t *p, int role)
{
    lua_getref (L, p->self);
    lua_pushstring (L, "role");
 
    p->role = role;
    if (role == OBJECT_ROLE_PROXY)
	lua_pushstring (L, "dumb");
    else if (role == OBJECT_ROLE_AUTHORITY)
	lua_pushstring (L, "authority");
    else
	lua_pushnil (L);

    lua_settable (L, -3);
}


object_t *object_create (const char *type_name, int role)
{
    object_t *p;
    object_type_t *type;
    
    type = object_types_lookup (type_name);
    if (!type) return 0;

    p = alloc (sizeof (object_t));
    if (!p) return NULL;
    
    p->type = type;
    p->id = id++;
    set_role (p, role);
    
    lua_newtable (L);
    p->self = lua_ref (L, 1);
    
    lua_getref (L, p->self);
    lua_pushstring (L, "_parent");
    lua_pushuserdata (L, p);
    lua_settable (L, -3);
    
    p->render = OBJECT_RENDER_MODE_BITMAP;
    p->bitmap = store_dat (p->type->icon);
    p->mask = bitmask_create_from_magic_bitmap (p->bitmap);
    /* XXX: memory wastage and leak */
    
    /* call object init (self, type-name) */
    lua_getref (L, p->type->table);
    if (lua_istable (L, -1)) {
	lua_pushstring (L, "init");
	lua_gettable (L, -2);
	/* init sits on top of stack */
	
	if (lua_isfunction (L, -1)) {
	    lua_getref (L, p->self);
	    lua_pushstring (L, p->type->name);
	    lua_call (L, 2, 0);
	}
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
