/* gamma.c
 *
 * Peter Wang <tjaden@users.sourceforge.net>
 */


#include <math.h>
#include <allegro.h>
#include "gamma.h"


/* Although the numbers can really be anything, we've decided that the
 * range of the gamma_factor is 0.5 to 1.5.
 *
 *   0.5 = brightest
 *   1.0 = "normal"
 *   1.5 = darkest
 */
float gamma_factor = 1.0;


static int gamma_func (float gamma, int i)
{
    /* from Quake */
    int inf = 255 * pow ((i + 0.5) / 255.5, gamma) + 0.5;

    return MID (0, inf, 255);
}


void apply_gamma (BITMAP *bmp, float gamma)
{
    int depth; 
    int x, y;

    depth = bitmap_color_depth (bmp);

    for (y = 0; y < bmp->h; y++) {
	for (x = 0; x < bmp->w; x++) {
	    int c = getpixel (bmp, x, y);

	    if (c) {
		int r = gamma_func (gamma, getr_depth (depth, c));
		int g = gamma_func (gamma, getg_depth (depth, c));
		int b = gamma_func (gamma, getb_depth (depth, c));

		putpixel (bmp, x, y, makecol_depth (depth, r, g, b));
	    }
	}
    }
}
