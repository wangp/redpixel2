/* ugtheme.c
 *
 * Peter Wang <tjaden@psynet.net>
 */


#include <stdarg.h>
#include <stdio.h>
#include <allegro.h>
#include "ugtheme.h"


extern struct ug_theme ug_theme_thepaw;
extern struct ug_theme ug_theme_motif;

static struct ug_theme *theme = &ug_theme_thepaw;


int ug_theme_init ()
{
    return theme->init ();
}

void ug_theme_shutdown ()
{
    theme->shutdown ();
}

void ug_theme_tile (BITMAP *bmp, int x, int y, int w, int h, int type)
{
    theme->tile (bmp, x, y, w, h, type);
}

void ug_theme_bevel (BITMAP *bmp, int x, int y, int w, int h, int invert)
{
    theme->bevel (bmp, x, y, w, h, invert);
}


void ug_theme_arrow (BITMAP *bmp, int x, int y, int w, int h, int type)
{
    theme->arrow (bmp, x, y, w, h, type);
}


struct clip {
    int cl, cr, ct, cb;
};

static void save_clip (BITMAP *b, struct clip *c)
{
    c->cl = b->cl;
    c->cr = b->cr;
    c->ct = b->ct;
    c->cb = b->cb;
}

static void restore_clip (BITMAP *b, struct clip *c)
{
    set_clip (b, c->cl, c->ct, c->cr, c->cb);
}


void ug_theme_text_centre (BITMAP *bmp, int x, int y, int w, int h, int type, const char *fmt, ...)
{
    va_list ap;
    char str[1024];
    struct clip clip;
    FONT *f;

    va_start (ap, fmt);
    vsnprintf (str, sizeof str, fmt, ap); /* XXX: Allegro needs uvsnprintf */
    va_end (ap);

    f = ug_theme_font (type);

    save_clip (bmp, &clip);
    set_clip (bmp, x + 1, y + 1, x + w - 2, y + h - 2);

    text_mode (-1);
    theme->text (bmp, f,
		 x + (w - text_length (f, str)) / 2,
		 y + (h - text_height (f)) / 2,
		 str);

    restore_clip (bmp, &clip);
}


FONT *ug_theme_font (int type)
{
    return theme->font (type);
}