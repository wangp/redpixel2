/* render.c
 *
 * Peter Wang <tjaden@psynet.net>
 */


#include <allegro.h>
#include "alloc.h"
#include "list.h"
#include "magic4x4.h"
#include "map.h"
#include "object.h"
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
    x2 = MIN (x1 + w + 1, map->width);
    y2 = MIN (y1 + h + 2, map->height);

    for (y = y1, yy = -(offy % 16); y < y2; y++, yy += 16)
    for (x = x1, xx = -(offx % 16); x < x2; x++, xx += 16)
	if ((y >= 0) && (x >= 0) && (tile = map->tile[y][x]))
	    draw_magic_sprite(bmp, store[tile]->dat, xx, yy);
}


/*----------------------------------------------------------------------*/


static void render_lights (BITMAP *bmp, map_t *map, int offx, int offy)
{
    BITMAP *b;
    light_t *l;
    
    foreach (l, map->lights) {
	b = store[l->lightmap]->dat;
	draw_trans_magic_sprite (bmp, b,
				 (l->x - offx) - (b->w / 3 / 2),
				 (l->y - offy) - (b->h / 2));
    }
}


/*----------------------------------------------------------------------*/


/*
 * Old code.
 */
#if 0
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
#endif


static void render_object_layers (BITMAP *bmp, map_t *map, int offx, int offy)
{
    object_t *obj;
    struct object_layer *layer;

    foreach (obj, map->objects) {
	foreach (layer, obj->cvar.layers) 
	    draw_magic_sprite (bmp, layer->bmp,
			       obj->cvar.x - offx + layer->offset_x,
			       obj->cvar.y - offy + layer->offset_y);
    }
}


static void render_object_lights (BITMAP *bmp, map_t *map, int offx, int offy)
{
    object_t *obj;
    struct object_light *light;

    foreach (obj, map->objects) {
	foreach (light, obj->cvar.lights)
	    draw_trans_magic_sprite (bmp, light->bmp,
				     obj->cvar.x - offx + light->offset_x,
				     obj->cvar.y - offy + light->offset_y);
    }
}


/*----------------------------------------------------------------------*/


void render (BITMAP *bmp, map_t *map, camera_t *cam)
{
    int x = cam->x;
    int y = cam->y;
    int w = cam->view_width;
    int h = cam->view_height;

    render_tiles (bmp, map, x, y, w, h);
    render_object_layers (bmp, map, x, y);
    render_lights (bmp, map, x, y);
    render_object_lights (bmp, map, x, y);
}
