#ifndef __included_render_h
#define __included_render_h


struct BITMAP;
struct map;
struct camera;


void render_lights (struct BITMAP *bmp, struct map *map, int offx, int offy);
void render (struct BITMAP *bmp, struct map *map, struct camera *cam);


#endif
