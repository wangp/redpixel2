/* This file is shared between the map editor and the game. */
#ifndef __included_map_h
#define __included_map_h


typedef struct light {
    int x, y, lightmap;
    struct light *next;
} light_t;


typedef struct map {
    int width, height;

    int **tile;

    int brightness;		/* ambient lighting */
    light_t lights;
} map_t;


map_t *map_create (void);
void map_destroy (map_t *map);
int map_resize (map_t *map, int width, int height);

light_t *map_light_create (map_t *map, int x, int y, int lightmap);
void map_light_destroy (map_t *map, light_t *light);


#endif
