/* textout.c
 * 
 * Peter Wang <tjaden@users.sourceforge.net>
 */


#include <allegro.h>
#include "magic4x4.h"
#include "textout.h"


void textout_magic (BITMAP *bmp, FONT *font, const char *buf,
		    int x, int y, int color)
{
    int len = text_length (font, buf);
    BITMAP *tmp = create_magic_bitmap (len, text_height (font));
    clear_bitmap (tmp);
    textout (tmp, font, buf, 0, 0, color);
    draw_magic_sprite (bmp, tmp, x, y);
    destroy_bitmap (tmp);
}


void textout_right_magic (BITMAP *bmp, FONT *font, const char *buf,
			  int x, int y, int color)
{
    int len = text_length (font, buf);
    BITMAP *tmp = create_magic_bitmap (len, text_height (font));
    clear_bitmap (tmp);
    textout (tmp, font, buf, 0, 0, color);
    draw_trans_magic_sprite (bmp, tmp, x - len, y);
    destroy_bitmap (tmp);
}


void textout_centre_trans_magic (BITMAP *bmp, FONT *font, const char *s,
				 int x, int y, int color)
{
    BITMAP *tmp = create_magic_bitmap (text_length (font, s),
				       text_height (font));
    clear_bitmap (tmp);
    textout (tmp, font, s, 0, 0, color);
    draw_trans_magic_sprite (bmp, tmp, x - tmp->w/3/2, y);
    destroy_bitmap (tmp);
}


void textprintf_trans_magic (BITMAP *bmp, FONT *font, int x, int y,
			     int color, const char *fmt, ...)
{
    va_list ap;
    BITMAP *tmp;
    char buf[1024];

    va_start (ap, fmt);
    uvszprintf (buf, sizeof buf, fmt, ap);
    va_end (ap);

    tmp = create_magic_bitmap (text_length (font, buf), text_height (font));
    clear_bitmap (tmp);
    textout (tmp, font, buf, 0, 0, color);
    draw_trans_magic_sprite (bmp, tmp, x, y);
    destroy_bitmap (tmp);
}


void textprintf_right_trans_magic (BITMAP *bmp, FONT *font, int x, int y,
				   int color, const char *fmt, ...)
{
    va_list ap;
    BITMAP *tmp;
    char buf[1024];

    va_start (ap, fmt);
    uvszprintf (buf, sizeof buf, fmt, ap);
    va_end (ap);

    tmp = create_magic_bitmap (text_length (font, buf), text_height (font));
    clear_bitmap (tmp);
    textout (tmp, font, buf, 0, 0, color);
    draw_trans_magic_sprite (bmp, tmp, x - tmp->w/3, y);
    destroy_bitmap (tmp);
}
