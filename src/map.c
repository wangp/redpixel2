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
    
    if (map->tile_mask)
	bitmask_destroy (map->tile_mask);

    while (map->lights.next)
	map_light_destroy (map, map->lights.next);

    while (map->objects.next) {
	object_t *p = map->objects.next;
	map_unlink_object (map, p);
	object_destroy (p);
    }

    while (map->starts.next)
	map_start_destroy (map, map->starts.next);

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

    for (y = 0; y < map->height; y++)
	for (x = 0; x < map->width; x++)
	    if ((t = map->tile[y][x])) {
		b = store[t]->dat;
		for (yy = 0; yy < 16; yy++)
		    for (xx = 0; xx < 16; xx++)
			bitmask_set_point (map->tile_mask, (x * 16 + xx), (y * 16 + yy),
					   (b->line[yy][xx * 3]
					    || b->line[yy][xx * 3 + 1]
					    || b->line[yy][xx * 3 + 2]));
	    }
}


void map_link_object (map_t *map, object_t *p)
{
    object_t *q;

    for (q = &map->objects; q->next; q = q->next)
	;
    p->next = 0;
    p->prev = (q != &map->objects) ? q :  0;
    q->next = p;
}


void map_unlink_object (map_t *map, object_t *p)
{
    if (map->objects.next == p)
	map->objects.next = p->next;
    if (p->prev) p->prev->next = p->next;
    if (p->next) p->next->prev = p->prev;

    p->prev = p->next = 0;
}


object_t *map_find_object (map_t *map, int id)
{
    object_t *p;

    for (p = map->objects.next; p; p = p->next)
	if (p->id == id)
	    break;

    return p;
}


light_t *map_light_create (map_t *map, int x, int y, int lightmap)
{
    light_t *p, *q;

    p = alloc (sizeof *p);

    if (p) {
	p->x = x;
	p->y = y;
	p->lightmap = lightmap;
	
	for (q = &map->lights; q->next; q = q->next)
	    ;
	
	p->next = 0;
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

    free (light);
}


start_t *map_start_create (map_t *map, int x, int y)
{
    start_t *p, *q;

    p = alloc (sizeof *p);

    if (p) {
	p->x = x;
	p->y = y;
	
	for (q = &map->starts; q->next; q = q->next)
	    ;
	
	p->next = 0;
	q->next = p;
    }

    return p;
}


void map_start_destroy (map_t *map, start_t *start)
{
    start_t *p;

    for (p = &map->starts; p->next; p = p->next)
	if (p->next == start) {
	    p->next = start->next;
	    break;
	}

    free (start);
}
