/* screen.c
 *
 * Peter Wang <tjaden@users.sourceforge.net>
 */


#include <allegro.h>
#include "2xsai.h"
#include "error.h"
#include "magic4x4.h"
#include "screen.h"


static int stretch_method;
static BITMAP *plain_bmp;
static BITMAP *big_bmp;


void screen_blitter_init (int method, int colour_depth)
{
    switch (method) {
	case STRETCH_METHOD_NONE:
	    stretch_method = method;
	    break;

	case STRETCH_METHOD_PLAIN:
	    stretch_method = method;
	    plain_bmp = create_bitmap (screen_width, screen_height);
	    break;

	case STRETCH_METHOD_SUPER2XSAI:
	case STRETCH_METHOD_SUPEREAGLE:
	    if ((colour_depth != 16) && (colour_depth != 15))
		error ("internal error: cannot use non-15/16-bpp with 2xSaI\n");

	    stretch_method = method;
	    plain_bmp = create_bitmap (screen_width, screen_height);
	    big_bmp = create_bitmap (SCREEN_W, SCREEN_H);
	    Init_2xSaI (colour_depth);
	    break;

	default:
	    error ("internal error: bad stretch method\n");
    }
}


void screen_blitter_shutdown (void)
{
    if (big_bmp) {
	destroy_bitmap (big_bmp);
	big_bmp = NULL;
    }

    if (plain_bmp) {
	destroy_bitmap (plain_bmp);
	plain_bmp = NULL;
    }
}


void blit_magic_bitmap_to_screen (BITMAP *bmp)
{
    switch (stretch_method) {

	case STRETCH_METHOD_NONE:
	    acquire_screen ();
	    blit_magic_format (bmp, screen, screen_width, screen_height);
	    release_screen ();
	    break;

	case STRETCH_METHOD_PLAIN:
	    blit_magic_format (bmp, plain_bmp, plain_bmp->w, plain_bmp->h);
	    stretch_blit (plain_bmp, screen,
			  0, 0, plain_bmp->w, plain_bmp->h,
			  0, 0, SCREEN_W, SCREEN_H);
	    break;

	case STRETCH_METHOD_SUPER2XSAI:
	    blit_magic_format (bmp, plain_bmp, plain_bmp->w, plain_bmp->h);
	    Super2xSaI (plain_bmp, big_bmp, 0, 0, 0, 0, plain_bmp->w, plain_bmp->h);
	    blit (big_bmp, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);
	    break;

	case STRETCH_METHOD_SUPEREAGLE:
	    blit_magic_format (bmp, plain_bmp, plain_bmp->w, plain_bmp->h);
	    SuperEagle (plain_bmp, big_bmp, 0, 0, 0, 0, plain_bmp->w, plain_bmp->h);
	    blit (big_bmp, screen, 0, 0, 0, 0, SCREEN_W, SCREEN_H);
	    break;
    }
}