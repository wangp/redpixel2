/* bitmaskg.c - bitmask generation
 *
 * Peter Wang <tjaden@users.sourceforge.net>
 */


#include <allegro.h>
#include "bitmask.h"
#include "bitmaskg.h"
#include "store.h"


bitmask_t *bitmask_create_from_bitmap (BITMAP *bmp)
{
    bitmask_t *mask;
    int x, y;

    mask = bitmask_create (bmp->w, bmp->h);
    if (mask)
	for (y = 0; y < bmp->h; y++) for (x = 0; x < bmp->w; x++)
	    bitmask_set_point (mask, x, y, getpixel (bmp, x, y) != bitmap_mask_color (bmp));
    return mask;
}


bitmask_t *bitmask_create_from_magic_bitmap (BITMAP *bmp)
{
    bitmask_t *mask;
    int x, y;

    mask = bitmask_create (bmp->w/3, bmp->h);
    if (mask)
	for (y = 0; y < bmp->h; y++) for (x = 0; x < bmp->w/3; x++)
	    bitmask_set_point (mask, x, y, (bmp->line[y][x*3] ||
					    bmp->line[y][x*3+1] ||
					    bmp->line[y][x*3+2]));
    return mask;
}
