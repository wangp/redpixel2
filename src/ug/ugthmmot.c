/* ugthmmot.c - Motif theme for UG
 *
 * Peter Wang <tjaden@users.sourceforge.net>
 */


#include <allegro.h>
#include "ugtheme.h"


static int fg, mg, bg, textcol;


static int theme_init (void)
{
    fg = makecol (0xf0, 0xf0, 0xf0);
    mg = makecol (0xc0, 0xc0, 0xc0);
    bg = makecol (0x80, 0x80, 0x80);
    textcol = makecol (0x20, 0x20, 0x20);
   
    return 0;
}


static void theme_shutdown (void)
{
}


static void theme_tile (BITMAP *bmp, int x, int y, int w, int h, int type)
{
    rectfill (bmp, x, y, x + w - 1, y + h - 1, mg);
}


static void theme_bevel (BITMAP *bmp, int x, int y, int w, int h, int invert)
{
    int a, b;

    if (!invert) 
	a = fg, b = bg;
    else
	a = bg, b = fg;

    hline (bmp, x, y, 	      x + w - 1, a);	hline (bmp, x + 1, y + 1,     x + w - 2, a);
    hline (bmp, x, y + h - 1, x + w - 1, b);	hline (bmp, x + 1, y + h - 2, x + w - 2, b);

    vline (bmp, x,         y, y + h - 2, a); 	vline (bmp, x + 1,     y + 1, y + h - 3, a);
    vline (bmp, x + w - 1, y, y + h - 1, b);	vline (bmp, x + w - 2, y + 1, y + h - 2, b);
}


static void theme_arrow (BITMAP *bmp, int x, int y, int w, int h, int type)
{
}


static FONT *theme_font (int type)
{
    return font;
}


static void theme_text (BITMAP *bmp, FONT *font, int x, int y, const char *str)
{
    textout (bmp, font, (char *) str, x, y, textcol);
}


struct ug_theme ug_theme_motif = {
    theme_init,
    theme_shutdown,

    theme_tile,
    theme_bevel,
    theme_arrow,
    theme_font,
    theme_text
};
