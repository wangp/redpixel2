/* cursor.c
 *
 * Peter Wang <tjaden@users.sourceforge.net>
 */


#include <allegro.h>
#include "cursor.h"
#include "magic4x4.h"


#define HIGHLIGHT	(makecol (0xf0, 0xa0, 0xa0))


static BITMAP *dynamic_cursor;
static BITMAP *dot_cursor;


static void destroy_dynamic_cursor (void)
{
    if (dynamic_cursor) {
	destroy_bitmap (dynamic_cursor);
	dynamic_cursor = 0;
    }
}


int cursor_init (void)
{
    dot_cursor = create_bitmap (1, 1);
    putpixel (dot_cursor, 0, 0, HIGHLIGHT);
    cursor_set_default ();
    return 0;
}


void cursor_shutdown (void)
{
    destroy_dynamic_cursor ();
    destroy_bitmap (dot_cursor);
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

    putpixel (tmp, hotx, hoty, HIGHLIGHT);

    change (tmp, hotx, hoty);

    destroy_dynamic_cursor ();
    dynamic_cursor = tmp;
}


void cursor_set_dot (void)
{
    change (dot_cursor, 0, 0);
    destroy_dynamic_cursor ();
}


void cursor_set_default (void)
{
    change (0, 0, 0);
    destroy_dynamic_cursor ();
}
