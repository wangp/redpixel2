#include <string.h>
#include <stdio.h>
#include <allegro.h>
#include "jpgalleg.h"
#include "music.h"
#include "store.h"
#include "timeval.h"


#define rectfill_wh(b, x, y, w, h, c)   (rectfill (b, x, y, x + w - 1, y + h - 1, c))
#define streq(s1, s2)                   (0 == strcmp (s1, s2))

#define change_font             "~f"
#define horizontal_line         "~h"
#define line_break              "~b"

#define horizontal_line_height  8
#define line_break_height       10

/* How long data/music/credits-new.xm is. */
#define music_secs		25.5


static char *credits[] = {
    change_font, "times-new-roman-14-bold", "RED PIXEL II",
    change_font, "times-new-roman-12-bold", "DEMO 2",
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
    change_font, "times-new-roman-10-bold", "SOUND EFFECTS",
    change_font, "times-new-roman-14-bold", "VARIOUS PLACES",
    line_break,
    horizontal_line,
    line_break, line_break,
    NULL
};


static int calc_total_credits_height (void)
{
    FONT *fnt = font;
    char str[64];
    int i, y;

    for (i = 0, y = 0; credits[i]; i++) {
	if (streq (credits[i], change_font)) {
	    i++;
	    sprintf (str, "/frontend/menu/%s", credits[i]);
	    fnt = store_get_dat (str);
	    if (!fnt)
		fnt = font;
	}
	else if (streq (credits[i], horizontal_line))
	    y += horizontal_line_height;
	else if (streq (credits[i], line_break))
	    y += line_break_height;
	else
	    y += text_height (fnt);
    }

    return y;
}


void do_credits (void)
{
    BITMAP *darkener_top;
    BITMAP *darkener_bot;
    BITMAP *dbuf;
    FONT *fnt = font;
    char str[64];
    int gray = makecol (0xa0, 0xa0, 0xa0);
    int i, y;
    int offy;
    int total_height;
    struct pwtimeval start;
    struct pwtimeval now;

    /* Create a double buffer for the scrolling text part. */
    dbuf = create_bitmap (250, SCREEN_H);

    /* Prepare the bitmaps that will fade the text on and off the
     * screen.
     */
    darkener_bot = load_bitmap ("data/frontend/frontend-darkener.tga", NULL);
    darkener_top = create_bitmap_ex (bitmap_color_depth (darkener_bot),
				     darkener_bot->w, darkener_bot->h);
    draw_sprite_v_flip (darkener_top, darkener_bot, 0, 0);

    /* Set the palette used by the fonts. */
    set_palette (store_get_dat ("/frontend/menu/times-new-roman-pal"));

    /* Clear the screen and draw the skull bitmap. */
    clear_bitmap (screen);
    {
	BITMAP *skull = load_jpg ("data/frontend/frontend-skull.jpg", NULL);
	blit (skull, screen, 0, 0, 0, SCREEN_H - skull->h, skull->w, skull->h);
	destroy_bitmap (skull);
    }

    /* Calculate the total height of the scrolling text. */
    total_height = calc_total_credits_height ();

    /* Begin */

    music_select_playlist ("data/music/music-credits.txt");

    clear_keybuf ();
    gettimeofday (&start, NULL);

    do {

	/* Calculate elapsed time, and the position of the text. */
	gettimeofday (&now, NULL);
	{
	    float t = ((now.tv_sec - start.tv_sec) * 1000L +
		       (now.tv_usec - start.tv_usec) / 1000L) / 1000.;
	    float percent = t / music_secs;

	    offy = SCREEN_H - percent * (SCREEN_H + total_height);
	}

	/* Draw the text onto the double buffer. */
	clear_bitmap (dbuf);

	for (i = 0, y = 0; credits[i]; i++) {
	    if (streq (credits[i], change_font)) {
		i++;
		sprintf (str, "/frontend/menu/%s", credits[i]);
		fnt = store_get_dat (str);
		if (!fnt)
		    fnt = font;
	    }
	    else if (streq (credits[i], horizontal_line)) {
		rectfill_wh (dbuf, 25, offy + y + 4, dbuf->w - 50, 2, gray);
		y += horizontal_line_height;
	    }
	    else if (streq (credits[i], line_break)) {
		y += line_break_height;
	    }
	    else {
		textout_centre (dbuf, fnt, credits[i], dbuf->w/2, offy+y, -1);
		y += text_height (fnt);
	    }
	}

	/* Fade the top and bottom of the double buffer. */
	set_alpha_blender ();
	draw_trans_sprite (dbuf, darkener_top, 0, 0);
	draw_trans_sprite (dbuf, darkener_bot, 0, SCREEN_H - darkener_bot->h);

	/* Blit the double buffer to the right part of the screen. */
	blit (dbuf, screen, 0, 0, SCREEN_W - dbuf->w, 0, dbuf->w, dbuf->h);

	rest (10);

    } while (!keypressed () && (offy + total_height >= 0));

    /* End */

    music_stop_playlist ();

    clear_keybuf ();

    destroy_bitmap (darkener_top);
    destroy_bitmap (darkener_bot);
}
