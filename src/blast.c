/* blast.c
 * 
 * Peter Wang <tjaden@users.sourceforge.net>
 */


#include <math.h>
#include <allegro.h>
#include <allegro/internal/aintern.h>
#include "alloc.h"
#include "blast.h"
#include "fastsqrt.h"
#include "list.h"
#include "mylua.h"
#include "object.h"


#define SPREAD_SPEED  4


struct blast {
    blast_t *next;
    blast_t *prev;
    float x;
    float y;
    float max_radius;
    int max_damage;
    float r;
    int visual_only;
};


blast_t *blast_create (float x, float y, float radius, int damage,
		       int visual_only)
{
    blast_t *b = alloc (sizeof *b);

    b->x = x;
    b->y = y;
    b->max_radius = radius;
    b->max_damage = damage;
    b->visual_only = visual_only;

    return b;
}


void blast_destroy (blast_t *blast)
{
    free (blast);
}


static inline void do_blast_check (blast_t *blast, list_head_t *object_list)
{
    object_t *obj;
    float dx, dy;
    float dist;
    int dmg;

    list_for_each (obj, object_list) {
	if (object_stale (obj) || object_hidden (obj))
	    continue;

	dx = object_x (obj) - blast->x;
	dy = object_y (obj) - blast->y;
	dist = fast_fsqrt ((dx * dx) + (dy * dy));
	
	if ((dist < blast->r) && (dist >= blast->r - SPREAD_SPEED)) {
	    dmg = blast->max_damage * (1.5 - dist/blast->max_radius);
	    dmg = MIN (dmg, blast->max_damage);
	    if (dmg > 0) {
		lua_pushnumber (lua_state, dmg);
		object_call (obj, "receive_damage", 1);
	    }
	}
    }
}


int blast_update (blast_t *blast, list_head_t *object_list)
{
    blast->r += SPREAD_SPEED;

    if (!blast->visual_only)
	do_blast_check (blast, object_list);

    return (blast->r >= blast->max_radius) ? -1 : 0;
}


void blast_draw (BITMAP *dest, blast_t *blast, int offset_x, int offset_y)
{
    /* XXX this whole function is trash */

    dest->vtable->hfill = _linear_hline24;
    dest->vtable->putpixel = _linear_putpixel24;
    set_add_blender (0xf,0,0,5);
    drawing_mode (DRAW_MODE_TRANS,0,0,0);

    {
	int br = (int) (15 * (1. - blast->r/blast->max_radius)) << 4;
	br |= 0xf;

	dest->cl /= 3;
	dest->cr /= 3;
    
	circlefill (dest, blast->x - offset_x, blast->y - offset_y, blast->r,
		   makecol24 (br, br, br));

	circle (dest, blast->x - offset_x, blast->y - offset_y, blast->r,
	       makecol24 (br, br/2, 0));

	dest->cl *= 3;
	dest->cr *= 3;
    }
    
    drawing_mode (DRAW_MODE_SOLID,0,0,0);
}
