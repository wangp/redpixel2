/* map.c
 *
 * Peter Wang <tjaden@psynet.net>
 */


#include <stdlib.h>
#include <lua.h>
#include "alloc.h"
#include "map.h"
#include "objtypes.h"
#include "store.h"


/* This is only here because both the game and the editor use it, and
 * I didn't know where to put it.  */
map_t *map;

    
map_t *map_create (void)
{
    return alloc (sizeof (map_t));
}


void map_destroy (map_t *map)
{
    int i;

    for (i = 0; i < map->height; i++)
	free (map->tile[i]);
    free (map->tile);
    
    free (map);
}


int map_resize (map_t *map, int width, int height)
{
    void *p;
    int i;

    p = realloc (map->tile, height * sizeof (int *));
    if (!p)
	return -1;
    else
	map->tile = p;
    
    for (i = 0; i < height; i++) {
	if (i >= map->height)
	    map->tile[i] = 0;
	
	p = realloc (map->tile[i], width * sizeof (int));
	if (!p)
	    return -1;
	else
	    map->tile[i] = p;
	    
	if (width > map->width)
	    memset (map->tile[i] + map->width, 0, (width - map->width) * sizeof (int));
    }
    
    map->width = width;
    map->height = height;

    return 0;
}


light_t *map_light_create (map_t *map, int x, int y, int lightmap)
{
    light_t *p, *q;

    p = alloc (sizeof *p);

    if (p) {
	p->x = x;
	p->y = y;
	p->lightmap = lightmap;
	
	q = &map->lights;
	while (q->next)
	    q = q->next;
	
	p->next = q->next;
	q->next = p;
    }

    return p;
}


void map_light_destroy (map_t *map, light_t *light)
{
    light_t *p;

    for (p = &map->lights; p->next; p = p->next)
	if (p->next == light) {
	    p->next = light->next;
	    break;
	}
}


static void link_object (map_t *map, object_t *p)
{
    object_t *q;

    q = &map->objects;
    while (q->next)
	q = q->next;
	
    p->next = q->next;
    if (p->next)
	p->next->prev = p;
    q->next = p;
    if (q != &map->objects)
	p->prev = q;
}


static void unlink_object (map_t *map, object_t *p)
{
    if (map->objects.next == p)
	map->objects.next = p->next;
    if (p->prev) p->prev->next = p->next;
    if (p->next) p->next->prev = p->prev;
}


object_t *map_object_create (map_t *map, const char *type_name)
{
    object_t *p;
    object_type_t *type;

    type = object_type (type_name);
    if (!type) return 0;

    p = alloc (sizeof (object_t));
    
    if (p) {
	p->type = type;

	lua_pushobject (lua_createtable ());
	p->self = lua_ref (1);

	{
	    lua_pushobject (lua_getref (p->self));
	    lua_pushstring ("_parent");
	    lua_pushuserdata (p);
	    lua_rawsettable ();
	}

	p->render = OBJECT_RENDER_MODE_BITMAP;
	p->bitmap = store_dat (p->type->icon);

	link_object (map, p);

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
    }
	
    return p;
}


void map_object_destroy (map_t *map, object_t *p)
{
    unlink_object (map, p);
    if (p->layer)
	object_layer_list_destroy (p->layer);
    lua_unref (p->self);
    free (p);
}
