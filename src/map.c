/* map.c
 *
 * Peter Wang <tjaden@users.sourceforge.net>
 */


#include <stdlib.h>
#include "alloc.h"
#include "map.h"
#include "object.h"
#include "store.h"


/* This is only here because both the game and the editor use it, and
 * I didn't know where to put it.  */
map_t *map;

    
map_t *map_create (void)
{
    map_t *map;

    map = alloc (sizeof *map);

    init_list (map->lights);
    init_list (map->objects);
    init_list (map->starts);

    return map;
}


void map_destroy (map_t *map)
{
    int i;

    for (i = 0; i < map->height; i++)
	free (map->tile[i]);
    free (map->tile);
    
    if (map->tile_mask)
	bitmask_destroy (map->tile_mask);

    free_list (map->lights, map_light_destroy);
    free_list (map->objects, object_destroy);
    free_list (map->starts, map_start_destroy);
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


void map_generate_tile_mask (map_t *map)
{
    BITMAP *b;
    int x, y, xx, yy, t;

    if (map->tile_mask)
	bitmask_destroy (map->tile_mask);

    map->tile_mask = bitmask_create (map->width * 16, map->height * 16);

    for (y = 0; y < map->height; y++) for (x = 0; x < map->width; x++)
	if ((t = map->tile[y][x])) {
	    b = store[t]->dat;
	    for (yy = 0; yy < 16; yy++) for (xx = 0; xx < 16; xx++)
		bitmask_set_point (map->tile_mask,
				   (x * 16 + xx), (y * 16 + yy),
				   (b->line[yy][xx * 3]
				    || b->line[yy][xx * 3 + 1]
				    || b->line[yy][xx * 3 + 2]));
	}
}


void map_link_object (map_t *map, object_t *p)
{
    append_to_list (map->objects, p);
}


void map_link_object_bottom (map_t *map, object_t *p)
{
    add_to_list (map->objects, p);
}


void map_unlink_object (object_t *p)
{
    del_from_list (p);
    p->next = p->prev = NULL;
}


object_t *map_find_object (map_t *map, int id)
{
    object_t *p;

    foreach (p, map->objects)
	if (p->id == id)
	    return p;

    return NULL;
}


light_t *map_light_create (map_t *map, int x, int y, int lightmap)
{
    light_t *p;

    p = alloc (sizeof *p);

    p->x = x;
    p->y = y;
    p->lightmap = lightmap;
    append_to_list (map->lights, p);

    return p;
}


void map_light_destroy (light_t *light)
{
    del_from_list (light);
    free (light);
}


start_t *map_start_create (map_t *map, int x, int y)
{
    start_t *p;

    p = alloc (sizeof *p);
    
    p->x = x;
    p->y = y;
    append_to_list (map->starts, p);

    return p;
}


void map_start_destroy (start_t *start)
{
    del_from_list (start);
    free (start);
}
