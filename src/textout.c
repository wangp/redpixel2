/* textout.c -- text routines for magic bitmaps
 * 
 * Peter Wang <tjaden@users.sourceforge.net>
 */


#include <allegro.h>
#include "magic4x4.h"
#include "textout.h"


enum { LEFT, RIGHT, CENTRE };

static void _do_it (
    BITMAP *bmp, FONT *font, const char *buf,
    int x, int y, int color,
    void (*drawer) (BITMAP *, BITMAP *, int, int),
    int align)
{
    int len;
    BITMAP *tmp;

    len = text_length (font, buf);
    tmp = create_magic_bitmap (len, text_height (font));
    clear_bitmap (tmp);
    textout (tmp, font, buf, 0, 0, color);

    switch (align) {
	case LEFT:   drawer (bmp, tmp, x, y); break;
	case RIGHT:  drawer (bmp, tmp, x - len, y); break;
	case CENTRE: drawer (bmp, tmp, x - tmp->w/3/2, y); break;
    }

    destroy_bitmap (tmp);
}


/* plain string */

#define MK_THINGO(NAME, DRAWER, ALIGN)				\
void NAME (BITMAP *bmp, FONT *font, const char *buf,		\
	   int x, int y, int color)				\
{								\
    _do_it (bmp, font, buf, x, y, color, DRAWER, ALIGN);	\
}

MK_THINGO(textout_magic, draw_magic_sprite, LEFT)
MK_THINGO(textout_right_magic, draw_magic_sprite, RIGHT)
MK_THINGO(textout_centre_magic, draw_magic_sprite, CENTRE)

MK_THINGO(textout_trans_magic, draw_trans_magic_sprite, LEFT)
MK_THINGO(textout_right_trans_magic, draw_trans_magic_sprite, RIGHT)
MK_THINGO(textout_centre_trans_magic, draw_trans_magic_sprite, CENTRE)


/* format string */

#define MK_THINGO_F(NAME, DRAWER, ALIGN)			\
void NAME (BITMAP *bmp, FONT *font, int x, int y,		\
	   int color, const char *fmt, ...)			\
{								\
    va_list ap;							\
    char buf[1024];						\
								\
    va_start (ap, fmt);						\
    uvszprintf (buf, sizeof buf, fmt, ap);			\
    va_end (ap);						\
								\
    _do_it (bmp, font, buf, x, y, color, DRAWER, ALIGN);	\
}

MK_THINGO_F (textprintf_trans_magic, draw_trans_magic_sprite, LEFT)
MK_THINGO_F (textprintf_right_trans_magic, draw_trans_magic_sprite, RIGHT)
MK_THINGO_F (textprintf_centre_trans_magic, draw_trans_magic_sprite, CENTRE)
