/* This file is shared between the map editor and the game. */
#ifndef __included_map_h
#define __included_map_h


struct object;
struct blood_particles;
struct spark_particles;
struct BITMAP;


typedef struct light light_t;
typedef struct start start_t;
typedef struct map map_t;


map_t *map_create (int is_client);
void map_destroy (map_t *map);
int map_width (map_t *map);
int map_height (map_t *map);
int map_resize (map_t *map, int width, int height);

int map_tile (map_t *map, int x, int y);
void map_set_tile (map_t *map, int x, int y, int tile);
void map_generate_tile_mask (map_t *map);
struct bitmask *map_tile_mask (map_t *map);

void map_link_object (map_t *map, struct object *obj);
void map_link_object_bottom (map_t *map, struct object *obj);
void map_unlink_object (struct object *obj);
void map_destroy_stale_objects (map_t *map);
struct object *map_find_object (map_t *map, int id);
struct list_head *map_object_list (map_t *map);

light_t *map_light_create (map_t *map, int x, int y, int lightmap);
void map_light_destroy (light_t *light);
int map_light_x (light_t *light);
int map_light_y (light_t *light);
int map_light_lightmap (light_t *light);
void map_light_move (light_t *light, int x, int y);
struct list_head *map_light_list (map_t *map);

start_t *map_start_create (map_t *map, int x, int y);
void map_start_destroy (start_t *start);
int map_start_x (start_t *start);
int map_start_y (start_t *start);
void map_start_move (start_t *start, int x, int y);
struct list_head *map_start_list (map_t *map);

struct particles *map_particles (map_t *map);


void map_explosion_create (map_t *map, const char *name, int x, int y);
void map_explosions_update (map_t *map);
void map_explosions_draw (map_t *map, struct BITMAP *dest, int offset_x, int offset_y);
void map_explosions_draw_lights (map_t *map, struct BITMAP *dest, int offset_x, int offset_y);


void map_blast_create (map_t *map, float x, float y, float radius, int damage, int owner, int visual_only);
void map_blasts_update (map_t *map);
void map_blasts_draw (map_t *map, struct BITMAP *dest, int offset_x, int offset_y);


#endif
