/* This file is shared between the map editor and the game. */
#ifndef __included_map_h
#define __included_map_h


#include "bitmask.h"
#include "object.h"


typedef struct light {
    int x, y, lightmap;
    struct light *next;
} light_t;


typedef struct start {
    int x, y;
    /* add multiplayer support,
       team support, etc. */
    struct start *next;
} start_t;


typedef struct map {
    int width, height;
    int **tile;
    bitmask_t *tile_mask;
    light_t lights;
    object_t objects;
    start_t starts;
} map_t;


extern map_t *map;


map_t *map_create (void);
void map_destroy (map_t *map);
int map_resize (map_t *map, int width, int height);

void map_generate_tile_mask (map_t *map);

void map_link_object (map_t *map, object_t *obj);
void map_unlink_object (map_t *map, object_t *obj);

light_t *map_light_create (map_t *map, int x, int y, int lightmap);
void map_light_destroy (map_t *map, light_t *light);

start_t *map_start_create (map_t *map, int x, int y);
void map_start_destroy (map_t *map, start_t *start);


#endif
