/* render.c
 *
 * Peter Wang <tjaden@psynet.net>
 */


#include <allegro.h>
#include "magic4x4.h"
#include "magicrot.h"
#include "map.h"
#include "object.h"
#include "store.h"
#include "render.h"


static void render_tiles (BITMAP *bmp, map_t *map, int offx, int offy)
{
    int w, h, x1, y1, x2, y2;
    int x, y, xx, yy, t;

    w = bmp->w / 3 / 16;
    h = bmp->h / 16;

    x1 = (offx / 16);
    y1 = (offy / 16);
    x2 = MIN (x1 + w + 1, map->width - 1);
    y2 = MIN (y1 + h + 1, map->height - 1);

    for (y = y1, yy = -(offy % 16); y < y2; y++, yy += 16)
	for (x = x1, xx = -(offx % 16); x < x2; x++, xx += 16)
	    if ((y >= 0) && (x >= 0) && (t = map->tile[y][x]))
		draw_sprite(bmp, store[t]->dat, xx * 3, yy);
}


static void render_lights (BITMAP *bmp, map_t *map, int offx, int offy)
{
    light_t *p;
    BITMAP *b;

    for (p = map->lights.next; p; p = p->next) {
	b = store[p->lightmap]->dat;
	draw_trans_sprite (bmp, b,
			   ((p->x - offx) - (b->w / 6)) * 3,
			    (p->y - offy) - (b->h / 2));
    }
}


static void render_objects (BITMAP *bmp, map_t *map, int offx, int offy)
{
    object_t *p;
    BITMAP *b;
    object_layer_t *l;
    int i;

    for (p = map->objects.next; p; p = p->next)
	switch (p->render) {
	    case OBJECT_RENDER_MODE_BITMAP:
		if ((b = p->bitmap))
		    draw_magic_sprite (bmp, b, p->x - offx, p->y - offy);
		break;

	    case OBJECT_RENDER_MODE_IMAGE:
		for (i = 0; i < p->image->num; i++) {
		    l = p->image->layer[i];
		    if (l->angle == OBJECT_LAYER_NO_ANGLE)
			draw_magic_sprite (bmp, l->bitmap,
					   p->x + l->offsetx - offx,
					   p->y + l->offsety - offy);
		    else
			rotate_magic_sprite (bmp, l->bitmap,
					     p->x + l->offsetx - offx,
					     p->y + l->offsety - offy, l->angle);
		}
		break;

	    case OBJECT_RENDER_MODE_ANIM: 
		draw_magic_sprite (bmp, p->anim->bitmap[p->anim->current],
				   p->x - offx, p->y - offy);
		break;
	}
}


void render (BITMAP *bmp, map_t *map, camera_t *cam)
{
    render_tiles (bmp, map, cam->x, cam->y);
    render_objects (bmp, map, cam->x, cam->y);
    render_lights (bmp, map, cam->x, cam->y);
}
