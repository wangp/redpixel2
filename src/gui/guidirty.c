/* guidirty.c - yet another hack of `demo.c'
 *
 * Peter Wang <tjaden@psynet.net>
 */


#include <allegro.h>
#include "gui.h"
#include "guiinter.h"


#define MAX_RECT	150


struct rect {
    int x, y, w, h;
};

struct list {
    int count;
    struct rect rect[MAX_RECT];
};


static struct list dirty, old_dirty;


void gui_dirty_init ()
{
    dirty.count = old_dirty.count = 0;
}


void gui_dirty_shutdown ()
{
}


static void add_rect (struct list *list, int x, int y, int w, int h)
{
    /*
     * Overlap checking from chapter 31 of
     * `Zen of Graphics Programming' by Michael Abrash.
     */
    
    struct rect *dr;
    int i, tx, ty;

    for (i = 0; i < list->count; i++) {
	dr = &list->rect[i];

	if ((dr->x < (x + w)) && ((dr->x + dr->w) > x) &&
	    (dr->y < (y + h)) && ((dr->y + dr->h) > y)) {
	    /* We've found an overlapping rectangle. Calculate the
	     * rectangles, if any, remaining after subtracting out the
	     * overlapped areas, and add them to the dirty list.
	     */

	    /* Check for a nonoverlapped left portion */
	    if (dr->x > x) {
		/* There's definitely a nonoverlapped portion at the left; add
		 * it, but only to at most the top and bottom of the overlapping
		 * rect; top and bottom strips are taken care of below
		 */
		ty = MAX (y, dr->y);
		add_rect (list, x, ty, dr->x - x, MIN (y + h, dr->y + dr->h) - ty);
	    }

	    /* Check for a nonoverlapped right portion */
	    if (dr->x + dr->w < x + w) {
		/* There's definitely a nonoverlapped portion at the right; add
		 * it, but only to at most the top and bottom of the overlapping
		 * rect; top and bottom strips are taken care of below
		 */
		tx = dr->x + dr->w;
		ty = MAX (y, dr->y);
		add_rect (list, tx, ty,
			  (x + w) - (dr->x + dr->w), 
			  MIN (y + h, dr->y + dr->h) - ty);
	    }

	    /* Check for a nonoverlapped top portion */
	    if (dr->y > y) {
		/* There's a top portion that's not overlapped */
		add_rect(list, x, y, w, dr->y - y);
	    }

	    /* Check for a nonoverlapped bottom portion */
	    if (dr->y+dr->h < (y + h)) {
		/* There's a bottom portion that's not overlapped */
		ty = dr->y + dr->h;
		add_rect (list, x, ty, w, (y  +h) - (dr->y + dr->h));
	    }

	    /* We've added all non-overlapped portions to the dirty list */
	    return;
	}
    }

    /*
     * End of overlap checking
     */

    /* XXX: need to handle case of too many rectangles */
    list->rect[list->count].x = x;
    list->rect[list->count].y = y;
    list->rect[list->count].w = w;
    list->rect[list->count].h = h;
    list->count++;
}


void gui_dirty_add_rect (int x, int y, int w, int h)
{
    add_rect (&dirty, x, y, w, h);
}


void gui_dirty_clear (BITMAP *bmp)
{
    int c;

    for (c = 0; c < dirty.count; c++) 
	rectfill (bmp,
		  dirty.rect[c].x, dirty.rect[c].y, 
		  dirty.rect[c].x + dirty.rect[c].w, 
		  dirty.rect[c].y + dirty.rect[c].h, 0);

    old_dirty = dirty;
    dirty.count = 0;
}


#if 0
static int rect_cmp (const void *p1, const void *p2)
{
    struct rect *r1 = (struct rect *) p1;
    struct rect *r2 = (struct rect *) p2;
    
    return (r1->y - r2->y);
}
#endif


void gui_dirty_blit (BITMAP *src, BITMAP *dest)
{
    int c, vid;

    for (c = 0; c < dirty.count; c++)
	add_rect (&old_dirty,
		  dirty.rect[c].x, dirty.rect[c].y,
		  dirty.rect[c].w, dirty.rect[c].h);

    vid = is_video_bitmap (dest);
    
    if (vid) {
#if 0
	if (!gfx_driver->linear)
	    qsort (old_dirty.rect, old_dirty.count, sizeof (struct rect), rect_cmp);
#endif

	acquire_bitmap (dest);
    }

    for (c = 0; c < old_dirty.count; c++) 
	blit (src, dest,
	      old_dirty.rect[c].x, old_dirty.rect[c].y,
	      old_dirty.rect[c].x, old_dirty.rect[c].y, 
	      old_dirty.rect[c].w, old_dirty.rect[c].h);

    if (vid) 
	release_bitmap (dest);
}