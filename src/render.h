#ifndef __included_render_h
#define __included_render_h


#include <allegro.h>
#include "camera.h"
#include "map.h"


void render_lights (BITMAP *bmp, map_t *map, int offx, int offy);
void render (BITMAP *bmp, map_t *map, camera_t *cam);


#endif
