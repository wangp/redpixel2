/* dirty.c : dirty rectangle functions, taken from The Paw, which was in
 * turn based on code pinched from Allegro demo game
 * Also uses code from `Zen of Graphics Programming' by Michael Abrash.
 */

#include <allegro.h>


#define MAX_DIRTY	300


typedef struct rect {
    int x, y, w, h;
} rect_t;


typedef struct list {
    int count;
    int size;
    rect_t rect[MAX_DIRTY];
} list_t;


static list_t dirty, old_dirty;


/* See whether we want to check if dirty rectangles overlap */
#define CHECK_OVERLAP


/* add_rect:
 *  Adds a rectangle to dirty rect list.
 */
static inline void add_rect(list_t *list, int x, int y, int w, int h)
{
#ifdef CHECK_OVERLAP
    rect_t *dr;
    int i, tx, ty;
#endif

    if (list->count >= MAX_DIRTY) 
      return;

    /* from Zen of Graphics Programming, chapter 31 */
#ifdef CHECK_OVERLAP   		       

    for (i=0; i<list->count; i++) 
    {
	dr = &list->rect[i];

	if (dr->x < (x+w) && dr->x+dr->w > x &&
	    dr->y < (y+h) && dr->y+dr->h > y)
	{
	    /* We've found an overlapping rectangle. Calculate the
	     * rectangles, if any, remaining after subtracting out the
	     * overlapped areas, and add them to the dirty list
	     */
	    
	    /* Check for a nonoverlapped left portion */
	    if (dr->x > x) 
	    {
		/* There's definitely a nonoverlapped portion at the left; add
		 * it, but only to at most the top and bottom of the overlapping
		 * rect; top and bottom strips are taken care of below
		 */
		ty = MAX(y, dr->y);
		add_rect(list, x, ty, dr->x-x, MIN(y+h, dr->y+dr->h)-ty);
	    }

	    /* Check for a nonoverlapped right portion */
	    if (dr->x+dr->w < x + w) 
	    {
		/* There's definitely a nonoverlapped portion at the right; add
		 * it, but only to at most the top and bottom of the overlapping
		 * rect; top and bottom strips are taken care of below
		 */
		tx = dr->x+dr->w;
		ty = MAX(y, dr->y);
		add_rect(list, tx, ty, (x+w) - (dr->x+dr->w), 
			  MIN(y+h, dr->y+dr->h)-ty);
	    }

	    /* Check for a nonoverlapped top portion */
	    if (dr->y > y) 
	    {
		/* There's a top portion that's not overlapped */
		add_rect(list, x, y, w, dr->y-y);
	    }

	    /* Check for a nonoverlapped bottom portion */
	    if (dr->y+dr->h < (y + h)) 
	    {
		/* There's a bottom portion that's not overlapped */
		ty = dr->y+dr->h;
		add_rect(list, x, ty, w, (y+h) - (dr->y+dr->h));
	    }

	    /* We've added all non-overlapped portions to the dirty list */
	    return;
	}
    } 
#endif /* CHECK_OVERLAP */

    list->rect[list->count].x = x;
    list->rect[list->count].y = y;
    list->rect[list->count].w = w;
    list->rect[list->count].h = h;
    list->count++;
    list->size += w*h;
}


int not_dirty = 1;


/* mark_dirty:
 *  Add a region to the dirty rectangle list.
 */
void mark_dirty(int x, int y, int w, int h)
{
    if (w==0 && h==0)
      return;
    add_rect(&dirty, x, y, w, h);
    not_dirty = 0;
}


/* clear_dirty:
 *  Erases areas previously marked as dirty.
 */
void clear_dirty(BITMAP *dest, BITMAP *src)
{
    if (dirty.count >= MAX_DIRTY || dirty.size > dest->w * dest->h * 0.75) {
	if (src)	  
	  blit(src, dest, 0, 0, 0, 0, src->w, src->h);
	else
	  clear(dest);
    } else {
	int c, col;
	
	if (src) {
	    for (c=0; c<dirty.count; c++) {
		blit(src, dest, dirty.rect[c].x, dirty.rect[c].y,
		     dirty.rect[c].x, dirty.rect[c].y,
		     dirty.rect[c].w, dirty.rect[c].h);
	    }
	} else {
	    col = makecol(0,0,0);
	    for (c=0; c<dirty.count; c++) {
		rectfill(dest, dirty.rect[c].x, dirty.rect[c].y,
			 dirty.rect[c].x + dirty.rect[c].w,
			 dirty.rect[c].y + dirty.rect[c].h, col);
	    }
	}
    }
    
    old_dirty = dirty;
    dirty.count = 0;
    dirty.size = 0;
}


/* draw_dirty:
 *  Draws all bits of SRC that are marked as dirty to DEST.
 */
static int rect_cmp(const void *p1, const void *p2)
{
    return (*(rect_t *)p1).y - (*(rect_t *)p2).y;
}

void draw_dirty(BITMAP *dest, BITMAP *src)
{
    int c;
    
    not_dirty = 1;
    
    /* for dirty rectangle animation, only copy the areas that changed */
    for (c=0; c<dirty.count; c++)
      add_rect(&old_dirty, dirty.rect[c].x, dirty.rect[c].y,
	       dirty.rect[c].w, dirty.rect[c].h);
    
    /* use src for check below cause dest is prolly screen, and
     * screen->w h includes off screen mem
     */
    if (old_dirty.count >= MAX_DIRTY || old_dirty.size > src->w * src->h * 0.75) {
	blit(src, dest, 0, 0, 0, 0, src->w, src->h);
	return;
    }

    /* sorting the objects really cuts down on bank switching */
    if (!gfx_driver->linear)
      qsort(old_dirty.rect, old_dirty.count, sizeof(rect_t), rect_cmp);

    for (c=0; c<old_dirty.count; c++)
    {
	/* for testing purposes only */
	#if 0
	rect(src, old_dirty.rect[c].x, old_dirty.rect[c].y,
	old_dirty.rect[c].x+old_dirty.rect[c].w-1,
	old_dirty.rect[c].y+old_dirty.rect[c].h-1, random());
	#endif
	  
	blit(src, dest, old_dirty.rect[c].x, old_dirty.rect[c].y,
	    old_dirty.rect[c].x, old_dirty.rect[c].y,
	    old_dirty.rect[c].w, old_dirty.rect[c].h);
    }
}


/* reset_dirty:
 *  Does that.
 */
void reset_dirty()
{
    dirty.count = old_dirty.count = 0;
    dirty.size = old_dirty.size = 0;
    not_dirty = 1;
}

