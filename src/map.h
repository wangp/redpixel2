/* This file is shared between the map editor and the game. */
#ifndef __included_map_h
#define __included_map_h


#include <allegro.h>


typedef struct light {
    int x, y, lightmap;
    struct light *next;
} light_t;


typedef struct object {		/* XXX: need to think about this */
    int x, y;
    int icon;
    struct object *prev, *next;
} object_t;


typedef struct map {
    int width, height;
    int **tile;
    light_t lights;
    object_t objects;
} map_t;


extern map_t *map;


map_t *map_create (void);
void map_destroy (map_t *map);
int map_resize (map_t *map, int width, int height);

light_t *map_light_create (map_t *map, int x, int y, int lightmap);
void map_light_destroy (map_t *map, light_t *light);

object_t *map_object_create (map_t *map);
void map_object_destroy (map_t *map, object_t *obj);


#endif
