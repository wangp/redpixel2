#include <string.h>
#include <stdio.h>
#include <allegro.h>
#include "store.h"


#define rectfill_wh(b, x, y, w, h, c)	rectfill (b, x, y, x + w - 1, y + h - 1, c)
#define change_font		"font"
#define horizontal_line		"hline"
#define line_break		""


static char *credits[] = {
    change_font, "times-new-roman-14-bold", "RED PIXEL II",
    change_font, "times-new-roman-12-bold", "DEMO II",
    horizontal_line,
    line_break,
    line_break,
    change_font, "times-new-roman-10-bold", "CODE",
    change_font, "times-new-roman-14-bold", "PETER WANG",
    line_break,
    line_break,
    change_font, "times-new-roman-10-bold", "GRAPHICS",
    change_font, "times-new-roman-14-bold", "DAVID WANG",
    line_break,
    line_break,
    change_font, "times-new-roman-10-bold", "3D GRAPHICS",
    change_font, "times-new-roman-14-bold", "KAUPO ERME",
    line_break,
    line_break,
    change_font, "times-new-roman-10-bold", "MUSIC",
    change_font, "times-new-roman-14-bold", "OSKAR AITAJA",
    line_break,
    line_break,
    change_font, "times-new-roman-10-bold", "DOODLES",
    change_font, "times-new-roman-14-bold", "PETER WANG",
    line_break,
    line_break,
    change_font, "times-new-roman-10-bold", "FRONTEND CODE",
    change_font, "times-new-roman-14-bold", "DAVID WANG",
    line_break,
    line_break,
    change_font, "times-new-roman-10-bold", "SOUNDS STOLEN FROM",
    line_break,
    change_font, "times-new-roman-14-bold", "COMMANDO",
    "CYBERDOGS",
    "GOLGOLTHA",
    "KING PIN",
    "MECHWARRIOR II",
    "STARCRAFT",
    line_break,
    line_break,
    change_font, "times-new-roman-10-bold", "LIBRARIES",
    change_font, "times-new-roman-14-bold", "ALLEGRO",
    "DUMB",
    "LIBNET",
    "LUA",
    line_break,
    line_break,
    line_break,
    line_break,
    line_break,
    NULL
};


void do_credits (void)
{
    BITMAP *background = store_get_dat ("/frontend/menu/credits");
    BITMAP *dbuf = create_bitmap (250, SCREEN_H);
    FONT *fnt = font;
    char str[64];
    int end = 0;
    int gray = makecol (0xa0, 0xa0, 0xa0);
    int i, y;
    int offy = SCREEN_H + 20;

    set_palette (store_get_dat ("/frontend/menu/times-new-roman-pal"));
    blit (background, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);

    do {
	blit (background, dbuf, SCREEN_W - dbuf->w, 0, 0, 0, dbuf->w, dbuf->h);

	for (i = 0, y = 0; credits[i]; i++) {
	    if (strcmp (credits[i], change_font) == 0) {
		i++;
		sprintf (str, "/frontend/menu/%s", credits[i]);
		fnt = store_get_dat (str);
		if (!fnt)
		    fnt = font;
	    }
	    else if (strcmp (credits[i], horizontal_line) == 0) {
		rectfill_wh (dbuf, 25, offy + y + 4, 50, 2, gray);
		y += 8;
	    }
	    else if (strcmp (credits[i], line_break) == 0) {
		y += 10;
	    }
	    else {
		if (offy + y >= SCREEN_H) break;
		if (offy + y + text_height (fnt) < 0) {
		    y += text_height (fnt);
		    continue;
		}

		textout_centre (dbuf, fnt, credits[i], dbuf->w/2, offy+y, -1);
		y += text_height (fnt);
	    }
	}

	blit (dbuf, screen, 0, 0, SCREEN_W - dbuf->w, 0, dbuf->w, dbuf->h);

	offy--;

	/* XXX */
	rest (10);

	if ((key[KEY_Q]) || (key[KEY_ESC]) || (offy + y < 0))
	    end = 1;
    } while (!end);
}

