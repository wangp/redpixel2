/* map.c
 *
 * Peter Wang <tjaden@psynet.net>
 */


#include <stdlib.h>
#include "map.h"
#include "alloc.h"


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
