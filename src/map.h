/* This file is shared between the map editor and the game. */
#ifndef __included_map_h
#define __included_map_h


#include "bitmask.h"
#include "list.h"
#include "object.h"


typedef struct light {
    struct light *next;
    struct light *prev;
    int x, y, lightmap;
} light_t;


typedef struct start {
    struct start *next;
    struct start *prev;
    int x, y;
} start_t;


typedef struct map {
    int width, height;
    int **tile;
    bitmask_t *tile_mask;
    struct list_head lights;
    struct list_head objects;
    struct list_head starts;
} map_t;


extern map_t *map;

extern char map_filename[1024];


map_t *map_create (void);
void map_destroy (map_t *map);
int map_resize (map_t *map, int width, int height);

void map_generate_tile_mask (map_t *map);

void map_link_object (map_t *map, object_t *obj);
void map_unlink_object (object_t *obj);
object_t *map_find_object (map_t *map, int id);

light_t *map_light_create (map_t *map, int x, int y, int lightmap);
void map_light_destroy (light_t *light);

start_t *map_start_create (map_t *map, int x, int y);
void map_start_destroy (start_t *start);


#endif
