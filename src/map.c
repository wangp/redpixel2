/* map.c
 *
 * Peter Wang <tjaden@users.sourceforge.net>
 */


#include <stdlib.h>
#include <string.h>
#include "alloc.h"
#include "bitmask.h"
#include "blast.h"
#include "explo.h"
#include "list.h"
#include "map.h"
#include "object.h"
#include "particle.h"
#include "store.h"


struct map {
    int width, height;
    int **tile;
    bitmask_t *tile_mask;
    list_head_t lights;
    list_head_t objects;
    list_head_t starts;
    particles_t *particles;
    list_head_t explosions;
    list_head_t blasts;
};


map_t *map_create (int is_client)
{
    map_t *map;

    map = alloc (sizeof *map);

    list_init (map->lights);
    list_init (map->objects);
    list_init (map->starts);

    if (is_client)
	map->particles = particles_create ();

    list_init (map->explosions);
    list_init (map->blasts);

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

    list_free (map->lights, map_light_destroy);
    list_free (map->objects, object_destroy);
    list_free (map->starts, map_start_destroy);

    particles_destroy (map->particles);

    list_free (map->explosions, explosion_destroy);
    list_free (map->blasts, blast_destroy);
    
    free (map);
}


int map_width (map_t *map)
{
    return map->width;
}


int map_height (map_t *map)
{
    return map->height;
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



/*
 *----------------------------------------------------------------------
 *	Tiles
 *----------------------------------------------------------------------
 */


int map_tile (map_t *map, int x, int y)
{
    return map->tile[y][x];
}


void map_set_tile (map_t *map, int x, int y, int tile)
{
    map->tile[y][x] = tile;
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


bitmask_t *map_tile_mask (map_t *map)
{
    return map->tile_mask;
}



/*
 *----------------------------------------------------------------------
 *	Objects
 *----------------------------------------------------------------------
 */


void map_link_object (map_t *map, object_t *p)
{
    list_append (map->objects, p);
}


void map_link_object_bottom (map_t *map, object_t *p)
{
    list_add (map->objects, p);
}


void map_unlink_object (object_t *p)
{
    list_remove (p);
}


void map_destroy_stale_objects (map_t *map)
{
    object_t *obj, *next;

    for (obj = map->objects.next; list_neq (obj, &map->objects); obj = next) {
	next = list_next (obj);
	if (object_stale (obj)) {
	    map_unlink_object (obj);
	    object_destroy (obj);
	}
    }
}


object_t *map_find_object (map_t *map, int id)
{
    object_t *p;

    list_for_each (p, &map->objects)
	if (object_id (p) == id)
	    return p;

    return NULL;
}


list_head_t *map_object_list (map_t *map)
{
    return &map->objects;
}



/*
 *----------------------------------------------------------------------
 *	Lights
 *----------------------------------------------------------------------
 */


struct light {
    struct light *next;
    struct light *prev;
    int x, y, lightmap;
};


light_t *map_light_create (map_t *map, int x, int y, int lightmap)
{
    light_t *p;

    p = alloc (sizeof *p);

    p->x = x;
    p->y = y;
    p->lightmap = lightmap;
    list_append (map->lights, p);

    return p;
}


void map_light_destroy (light_t *light)
{
    list_remove (light);
    free (light);
}


int map_light_x (light_t *light)
{
    return light->x;
}


int map_light_y (light_t *light)
{
    return light->y;
}


int map_light_lightmap (light_t *light)
{
    return light->lightmap;
}


void map_light_move (light_t *light, int x, int y)
{
    light->x = x;
    light->y = y;
}


list_head_t *map_light_list (map_t *map)
{
    return &map->lights;
}



/*
 *----------------------------------------------------------------------
 *	Starts
 *----------------------------------------------------------------------
 */


struct start {
    struct start *next;
    struct start *prev;
    int x, y;
};


start_t *map_start_create (map_t *map, int x, int y)
{
    start_t *p;

    p = alloc (sizeof *p);
    
    p->x = x;
    p->y = y;
    list_append (map->starts, p);

    return p;
}

void map_start_destroy (start_t *start)
{
    list_remove (start);
    free (start);
}


int map_start_x (start_t *start)
{
    return start->x;
}


int map_start_y (start_t *start)
{
    return start->y;
}


void map_start_move (start_t *start, int x, int y)
{
    start->x = x;
    start->y = y;
}


list_head_t *map_start_list (map_t *map)
{
    return &map->starts;
}



/*
 *----------------------------------------------------------------------
 *	Particles
 *----------------------------------------------------------------------
 */


particles_t *map_particles (map_t *map)
{
    return map->particles;
}



/*
 *----------------------------------------------------------------------
 *	Explosions
 *----------------------------------------------------------------------
 */


void map_explosion_create (map_t *map, const char *name, int x, int y)
{
    explosion_t *e = explosion_create (name, x, y);

    if (e)
	list_add (map->explosions, e);
}


void map_explosions_update (map_t *map)
{
    explosion_t *e, *next;
    
    for (e = map->explosions.next; list_neq (e, &map->explosions); e = next) {
	next = list_next (e);
	if (explosion_update (e) < 0) {
	    list_remove (e);
	    explosion_destroy (e);
	}
    }
}


void map_explosions_draw (map_t *map, BITMAP *dest, int offset_x, int offset_y)
{
    explosion_t *e;

    list_for_each (e, &map->explosions)
	explosion_draw (dest, e, offset_x, offset_y);
}


void map_explosions_draw_lights (map_t *map, BITMAP *dest,
				 int offset_x, int offset_y)
{
    explosion_t *e;

    list_for_each (e, &map->explosions)
	explosion_draw_lights (dest, e, offset_x, offset_y);
}



/*
 *----------------------------------------------------------------------
 *	Blasts
 *----------------------------------------------------------------------
 */


void map_blast_create (map_t *map, float x, float y, float radius,
		       int damage, int visual_only)
{
    blast_t *b;
    b = blast_create (x, y, radius, damage, visual_only);
    list_add (map->blasts, b);
}


void map_blasts_update (map_t *map)
{
    blast_t *b, *next;

    for (b = map->blasts.next; list_neq (b, &map->blasts); b = next) {
	next = list_next (b);
	if (blast_update (b, &map->objects) < 0) {
	    list_remove (b);
	    blast_destroy (b);
	}
    }
}


void map_blasts_draw (map_t *map, BITMAP *dest, int offset_x, int offset_y)
{
    blast_t *b;

    list_for_each (b, &map->blasts)
	blast_draw (dest, b, offset_x, offset_y);
}
