/* cursor.c
 *
 * Peter Wang <tjaden@psynet.net>
 */


#include <allegro.h>
#include "cursor.h"
#include "magic4x4.h"


#define HIGHLIGHT	(makecol (0xf0, 0xa0, 0xa0))


static BITMAP *cursor;


static void destroy_cursor ()
{
    if (cursor) {
	destroy_bitmap (cursor);
	cursor = 0;
    }
}


int cursor_init ()
{
    cursor_set_default ();
    return 0;
}


void cursor_shutdown ()
{
    destroy_cursor ();
}


static void change (BITMAP *bmp, int x, int y)
{
    scare_mouse ();
    set_mouse_sprite (bmp);
    set_mouse_sprite_focus (x, y);
    unscare_mouse ();
}


void cursor_set_magic_bitmap (BITMAP *bmp, int hotx, int hoty)
{
    BITMAP *tmp;
    int x, y;

    tmp = unget_magic_bitmap_format (bmp);
    if (!tmp) return;
    
    for (y = 0; y < tmp->h; y++)
	for (x = 0; x < tmp->w; x++)
	    if (getpixel (tmp, x, y) == makecol (0, 0, 0))
		putpixel (tmp, x, y, bitmap_mask_color (screen));

    hotx /= 3;
    putpixel (tmp, hotx, hoty, HIGHLIGHT);

    change (tmp, hotx, hoty);

    destroy_cursor ();
    cursor = tmp;
}


void cursor_set_dot ()
{
    BITMAP *tmp = create_bitmap (1, 1);
    if (!tmp) return;

    putpixel (tmp, 0, 0, HIGHLIGHT);
    change (tmp, 0, 0);

    destroy_cursor ();
    cursor = tmp;
}


void cursor_set_default ()
{
    if (cursor) {
	change (0, 0, 0);
	destroy_cursor ();
    }
}
