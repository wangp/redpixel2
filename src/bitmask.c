/* bitmask.c - based heavily off PPCol, by Ivan Baldo
 *
 * Peter Wang <tjaden@psynet.net>
 */


#include <stdlib.h>
#include <string.h>
#include <allegro.h>
#include "bitmask.h"


/* Set this to a 32 bit unsigned integer on your machine, or
 * alternatively, check the code works with other sizes.  */
typedef unsigned long u32_t;

#define u32_bytes	(sizeof (u32_t))
#define u32_bits	(u32_bytes * 8) 	/* Assume byte == 8 bits. */


struct bitmask {
    int w, h;
    int line_w;		/* width of a line[] in bytes */
    void *dat;
    u32_t *line[0];
};


bitmask_t *bitmask_create (int width, int height)
{
    bitmask_t *p;
    int ww, size, i;

    p = malloc (sizeof (bitmask_t) + (sizeof (u32_t *) * height));
    if (!p) goto error;

    p->w = width;
    p->h = height;

    ww = p->line_w = (((width % u32_bits) == 0)
		      ? (width / u32_bits)
		      : (width / u32_bits) + 1);

    size = u32_bytes * ww * height;
    p->dat = malloc (size);
    if (!p->dat)
	goto error;
    memset (p->dat, 0, size);

    for (i = 0; i < height; i++)
	p->line[i] = p->dat + (i * u32_bytes * ww);

    return p;

  error:

    if (p) {
	free (p->dat);
	free (p);
    }

    return 0;
}


void bitmask_destroy (bitmask_t *p)
{
    if (p) {
	free (p->dat);
	free (p);
    }
}


void bitmask_set_point (bitmask_t *p, int x, int y, int value)
{
    if (!((x < 0) || (y < 0) || (x >= p->w) || (y >= p->h))) {
	int u = x / u32_bits;

	if (value) 
	    p->line[y][u] |=  1 << ((u32_bits - 1) - (x - (u * u32_bits)));
	else
	    p->line[y][u] &=~ 1 << ((u32_bits - 1) - (x - (u * u32_bits)));
    }
}


int bitmask_point (bitmask_t *p, int x, int y)
{
    if ((x < 0) || (y < 0) || (x >= p->w) || (y >= p->h))
	return -1;
    else {    
	int u = x / u32_bits;
	return  p->line[y][u] & (1 << ((u32_bits - 1) - (x - (u * u32_bits))));
    }
}


/* This routine was taken almost directly from PPCol.  No offence to
 * Ivan, but I reformatted his code and stripped the comments.  */

#define check_bb_collision(m1, m2, x1, y1, x2, y2)			\
	(!(((x1) >= (x2) + (m2->w)) || ((x2) >= (x1) + (m1->w)) || 	\
	   ((y1) >= (y2) + (m2->h)) || ((y2) >= (y1) + (m1->h))))

int bitmask_check_collision (bitmask_t *mask1, bitmask_t *mask2, int x1, int y1, int x2, int y2)
{
    int dx1, dx2, dy1, dy2;
    int y, height;
    int chunkm1, chunkm2;	/* chunk numbers */
    int chunkm1p, chunkm2p;	/* chunk positions (shifts) */
    
    if (!check_bb_collision (mask1, mask2, x1, y1, x2, y2))
	return 0;

    /* Shift masks.  */
    if (x1 > x2)
	dx1 = 0, dx2 = x1 - x2;
    else
	dx2 = 0, dx1 = x2 - x1;

    if (y1 > y2)
	dy1 = 0, dy2 = y1 - y2;
    else
	dy2 = 0, dy1 = y2 - y1;

    /* Initialise chunk values.  */
    chunkm1 = dx1 / u32_bits;
    chunkm2 = dx2 / u32_bits;
    chunkm1p = dx1 - (chunkm1 * u32_bits);
    chunkm2p = dx2 - (chunkm2 * u32_bits);

    /* Calculate height that we need to check.  */
    if ((mask1->h - dy1) > (mask2->h - dy2))
	height = mask2->h - dy2;
    else 
	height = mask1->h - dy1;
    height--;

    /* Loop until collision detected or no chunks remaining.  */
    while ((chunkm1 < mask1->line_w) && (chunkm2 < mask2->line_w)) {

	/* Do collision check row by row.  */
	for (y = height; y >= 0; y--)
	    if ((mask1->line[dy1 + y][chunkm1] << chunkm1p) & (mask2->line[dy2 + y][chunkm2] << chunkm2p))
		return 1;

	/* Advance chunk.  */
	if (!(chunkm1p) && !(chunkm2p))
	    chunkm1++, chunkm2++;
	else if (!chunkm1p) {
	    chunkm2++;
	    chunkm1p = u32_bits - chunkm2p;
	    chunkm2p = 0;
	}
	else if (!chunkm2p) {
	    chunkm1++;
	    chunkm2p = u32_bits - chunkm1p;
	    chunkm1p = 0;
	}
    }
    
    return 0;
}
