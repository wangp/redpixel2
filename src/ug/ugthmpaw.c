/* ugthmpaw.c - The Paw theme for UG
 *
 * Peter Wang <tjaden@psynet.net>
 */


#include <allegro.h>
#include "ugtheme.h"


static int fg, bg, textcol;
static BITMAP *tile;


static int theme_init ()
{
    fg = makecol (0xa0, 0xa0, 0xa0);
    bg = makecol (0x40, 0x40, 0x40);
    textcol = makecol (0xb0, 0xd0, 0xb0);

    tile = load_bitmap ("tile.bmp", 0);
   
    return 0;
}


static void theme_shutdown ()
{
    destroy_bitmap (tile);
}


static void theme_tile (BITMAP *bmp, int x, int y, int w, int h, int type)
{
    int u, v;
 
    if (!tile) {
	rectfill (bmp, x, y, x + w - 1, y + h - 1, makecol (0, 0, 0));
	return;
    }
    
    if (type == UG_THEME_FG)
	for (v = y; v < y + h; v += tile->h)
	    for (u = x; u < x + w; u += tile->w)
		blit (tile, bmp, 0, 0, u, v, MIN (w - (u - x), tile->w), MIN (h - (v - y), tile->h));
}


static void theme_bevel (BITMAP *bmp, int x, int y, int w, int h, int invert)
{
    int a, b;

    if (!invert) 
	a = fg, b = bg;
    else
	a = bg, b = fg;

    hline (bmp, x, y, 	      x + w - 1, a);
    hline (bmp, x, y + h - 1, x + w - 1, b);

    vline (bmp, x,         y, y + h - 2, a);
    vline (bmp, x + w - 1, y, y + h - 1, b);
}


static void theme_arrow (BITMAP *bmp, int x, int y, int w, int h, int type)
{
    /*
          0
        /   \ 
       /     \
      1 ----- 2

      1 ----- 2
       \     /
        \   / 
          0
    */
    
    int x0, y0, x1, y1, x2, y2;
    int c01, c02, c12;

    switch (type) {
	case UG_THEME_ARROW_UP:
	    x0 = x + (w / 2);
	    y0 = y;

	    x1 = x;
	    y1 = y + h - 1;

	    x2 = x + w - 1;
	    y2 = y + h - 1;

	    c01 = fg;
	    c12 = c02 = bg;
	    break;

	case UG_THEME_ARROW_DOWN:
	    x0 = x + (w / 2);
	    y0 = y + h - 1;

	    x1 = x;
	    y1 = y;

	    x2 = x + w - 1;
	    y2 = y;

	    c12 = fg;
	    c01 = fg;
	    c02 = bg;
	    break;

	default: return;
    }

    line (bmp, x1, y1, x2, y2, c12);
    line (bmp, x0, y0, x2, y2, c02);
    line (bmp, x0, y0, x1, y1, c01);
}


static FONT *theme_font (int type)
{
    return font;
}


static void theme_text (BITMAP *bmp, FONT *font, int x, int y, const char *str)
{
    textout (bmp, font, (char *) str, x, y, textcol);
}


struct ug_theme ug_theme_thepaw = {
    theme_init,
    theme_shutdown,

    theme_tile,
    theme_bevel,
    theme_arrow,
    theme_font,
    theme_text
};