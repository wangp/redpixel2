/* render.c
 *
 * Peter Wang <tjaden@psynet.net>
 */


#include <allegro.h>
#include "alloc.h"
#include "list.h"
#include "magic4x4.h"
#include "magicrot.h"
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
    x2 = MIN (x1 + w + 1, map->width - 1);
    y2 = MIN (y1 + h + 2, map->height - 1);

    for (y = y1, yy = -(offy % 16); y < y2; y++, yy += 16)
    for (x = x1, xx = -(offx % 16); x < x2; x++, xx += 16)
	if ((y >= 0) && (x >= 0) && (tile = map->tile[y][x]))
	    draw_magic_sprite(bmp, store[tile]->dat, xx, yy);
}


/*----------------------------------------------------------------------*/


struct extra_light {
    struct extra_light *next;
    struct extra_light *prev;
    BITMAP *bmp;
    int x, y;
};


static struct list_head extra_lights;


static inline void add_extra_light (BITMAP *bmp, int x, int y)
{
    struct extra_light *p;

    if ((p = alloc (sizeof *p))) {
	p->bmp = bmp;
	p->x = x;
	p->y = y;
	append_to_list (extra_lights, p);
    }
}


static void render_lights (BITMAP *bmp, map_t *map, int offx, int offy)
{
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

    {
	struct extra_light *p;

	foreach (p, extra_lights)
	    draw_trans_magic_sprite (bmp, p->bmp, p->x, p->y);

	free_list (extra_lights, free);
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


static void render_objects (BITMAP *bmp, map_t *map, int offx, int offy)
{
    object_t *p;
    BITMAP *b;

    foreach (p, map->objects) {
	if ((b = store_dat (p->type->icon)))
	    draw_magic_sprite (bmp, b, p->cvar.x - offx, p->cvar.y - offy);

	if (p->cvar.light_source) {
	    b = p->cvar.light_source;
	    add_extra_light (b, p->cvar.x - offx - (b->w / 3 / 2),
			     p->cvar.y - offy - (b->h / 2));
	}
    }
}


/*----------------------------------------------------------------------*/


void render (BITMAP *bmp, map_t *map, camera_t *cam)
{
    int x = cam->x;
    int y = cam->y;
    int w = cam->view_width;
    int h = cam->view_height;

    init_list (extra_lights);
	
    render_tiles (bmp, map, x, y, w, h);
    render_objects (bmp, map, x, y);
    render_lights (bmp, map, x, y);
}
