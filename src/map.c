/* map.c
 *
 * Peter Wang <tjaden@psynet.net>
 */


#include <stdlib.h>
#include "map.h"
#include "alloc.h"


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


object_t *map_object_create (map_t *map)
{
    object_t *p, *q;

    p = alloc (sizeof (object_t));

    if (p) {
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
	
    return p;
}


void map_object_destroy (map_t *map, object_t *obj)
{
    if (map->objects.next == obj)
	map->objects.next = obj->next;
    if (obj->prev) obj->prev->next = obj->next;
    if (obj->next) obj->next->prev = obj->prev;
    free (obj);
}
