/* render.c
 *
 * Peter Wang <tjaden@users.sourceforge.net>
 */


#include <allegro.h>
#include "camera.h"
#include "explo.h"
#include "list.h"
#include "magic4x4.h"
#include "map.h"
#include "object.h"
#include "particle.h"
#include "store.h"
#include "render.h"


static void render_tiles (BITMAP *bmp, map_t *map, int offx, int offy,
			  int width, int height)
{
    int w, h, x1, y1, x2, y2;
    int x, y, xx, yy, tile;

    w = width / 16;
    h = height / 16;

    x1 = (offx / 16);
    y1 = (offy / 16);
    x2 = MIN (x1 + w + 1, map_width (map));
    y2 = MIN (y1 + h + 2, map_height (map));

    for (y = y1, yy = -(offy % 16); y < y2; y++, yy += 16)
    for (x = x1, xx = -(offx % 16); x < x2; x++, xx += 16)
	if ((y >= 0) && (x >= 0) && (tile = map_tile (map, x, y)))
	    draw_magic_sprite(bmp, store[tile]->dat, xx, yy);
}


void render_lights (BITMAP *bmp, map_t *map, int offx, int offy)
{
    list_head_t *list;
    light_t *l;
    BITMAP *b;
    
    list = map_light_list (map);
    list_for_each (l, list) {
	b = store[map_light_lightmap (l)]->dat;
	draw_trans_magic_sprite (bmp, b,
				 (map_light_x (l) - offx) - (b->w / 3 / 2),
				 (map_light_y (l) - offy) - (b->h / 2));
    }
}


static void render_object_layers_and_names (BITMAP *bmp, map_t *map,
					    int offx, int offy)
{
    list_head_t *list;
    object_t *obj;

    list = map_object_list (map);
    list_for_each (obj, list) {
	object_draw_layers (bmp, obj, offx, offy);
	object_draw_trans_name (bmp, obj, offx, offy);
    }
}


static void render_object_lights (BITMAP *bmp, map_t *map, int offx, int offy)
{
    list_head_t *list;
    object_t *obj;

    list = map_object_list (map);
    list_for_each (obj, list)
	object_draw_lights (bmp, obj, offx, offy);
}


static void render_particles (BITMAP *bmp, map_t *map, int offx, int offy)
{
    particles_draw (bmp, map_particles (map), offx, offy);
}


void render (BITMAP *bmp, map_t *map, camera_t *cam)
{
    int x = camera_x (cam);
    int y = camera_y (cam);
    int w = camera_view_width (cam);
    int h = camera_view_height (cam);

    render_tiles (bmp, map, x, y, w, h);
    render_object_layers_and_names (bmp, map, x, y);
    render_particles (bmp, map, x, y);
    map_explosions_draw (map, bmp, x, y);
    map_blasts_draw (map, bmp, x, y);
    render_lights (bmp, map, x, y);
    render_object_lights (bmp, map, x, y);
    map_explosions_draw_lights (map, bmp, x, y);
}
