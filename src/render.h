#ifndef __included_render_h
#define __included_render_h


#include "map.h"


typedef struct camera {
    int x, y;
} camera_t;


void render (BITMAP *bmp, map_t *map, camera_t *cam);


#endif
