/*  Modified version of exlights.c, by Shawn Hargreaves.
 *  (included in the Allegro distribution)
 *
 *  This version uses a 4.4 "magic format" instead of the 5.3 as seen in
 *  exlights.c, trading off image quality (12 bpp instead of 15 bpp) for a few
 *  extra lighting levels (16 instead of 8).
 *
 *  Peter Wang <tjaden@psynet.net>
 */


#include <allegro.h>
#include "magic4x4.h"


/* Give images some light of their own.  Make this zero for total black.  */
#define AMBIENT_LIGHT	0x00


/* Converts a bitmap from the normal Allegro format into our magic layout.  */
BITMAP *get_magic_bitmap_format(BITMAP *orig, PALETTE pal)
{
   BITMAP *bmp;
   int c, r, g, b, a;
   int x, y; 
   int bpp; 

   /* Create an 8 bpp image, three times as wide as the original.  */
   bmp = create_bitmap_ex(8, orig->w * 3, orig->h);

   /* Store info about the original bitmap format.  */
   bpp = bitmap_color_depth(orig);
   if (pal) select_palette(pal);

   /* Convert the data. */
   for (y = 0; y < orig->h; y++) {
      for (x = 0; x < orig->w; x++) {
	 c = getpixel(orig, x, y);

	 r = getr_depth(bpp, c);
	 g = getg_depth(bpp, c);
	 b = getb_depth(bpp, c);

	 /* Hack: for some reason drawing some pictures with no
	  * lighting at all messes up the colours (XXX: check why)
	  * Therefore we give pictures the minimum amount of white
	  * light possible.  This means that you will be able to see
	  * the pictures, even if there is no light, by turning the
	  * monitor brightness and contrast up.
	  */
	 if (AMBIENT_LIGHT)
	     a = AMBIENT_LIGHT;
	 else 
	     a = (c) ? 0x10 : 0;

	 bmp->line[y][x*3  ] = r * 15/255 | a;
	 bmp->line[y][x*3+1] = g * 15/255 | a;
	 bmp->line[y][x*3+2] = b * 15/255 | a;
      }
   }

   if (pal) unselect_palette();

   /* Return our new, magic format version of the image.  */
   return bmp;
}


/* Converts a bitmap from out magic layout to the normal Allegro format.  */
BITMAP *unget_magic_bitmap_format(BITMAP *orig)
{
   BITMAP *bmp;
   int x, y;

   bmp = create_bitmap(orig->w / 3, orig->h);

   for (y = 0; y < bmp->h; y++)
      for (x = 0; x < bmp->w; x++)
	  putpixel(bmp, x, y, makecol((orig->line[y][x*3  ] & 0xf) * 255/15,
				      (orig->line[y][x*3+1] & 0xf) * 255/15,
				      (orig->line[y][x*3+2] & 0xf) * 255/15));
   
   return bmp;
}


/* Create our custom color blending map, which does translucency in the
 * bottom four bits and adds the light levels in the top four bits.
 * This version just does a 50% translucency if you are drawing sprites
 * with it, but you could easily make other color maps for different 
 * alpha levels, or for doing additive color, which can work happily
 * in parallel with the light blending.
 */
void generate_magic_color_map()
{
   int x, y, c, xc, yc, l, xl, yl;

   color_map = malloc(sizeof(COLOR_MAP));

   for (x = 0; x < 256; x++) {
      for (y = 0; y < 256; y++) {
	 xc = x & 15;
	 yc = y & 15;

	 xl = x >> 4;
	 yl = y >> 4;

	 if (xc)
	    c = (xc + yc) / 2;
	 else
	    c = yc;

	 l = xl + yl;
	 if (l > 15)
	    l = 15;

	 color_map->data[x][y] = c | (l << 4);
      }
   }

   /* XXX: check memory leak with color_map */
}
 

/* Lookup tables for speeding up the color conversion.  */
static unsigned short rgtable[0x10000];
static unsigned long  brtable[0x10000];
static unsigned short gbtable[0x10000];


/* Builds some helper tables for doing color conversions.  */
void generate_magic_conversion_tables()
{
   int r, g, b;
   int cr, cg, cb;

   /* This table combines a 16 bit red + green value into a screen pixel.  */
   for (r = 0; r < 256; r++) {
      cr = (r & 15) * (r >> 4) * 255/225;
      for (g = 0; g < 256; g++) {
	 cg = (g & 15) * (g >> 4) * 255/225;
	 rgtable[r + g * 256] = makecol(cr, cg, 0);
      }
   }

   /* This table combines a 16 bit blue + red value into a screen pixel.  */
   for (b = 0; b < 256; b++) {
      cb = (b & 15) * (b >> 4) * 255/225;
      for (r = 0; r < 256; r++) {
	 cr = (r & 15) * (r >> 4) * 255/225;
	 brtable[b + r * 256] = makecol(0, 0, cb) | (makecol(cr, 0, 0) << 16);
      }
   }

   /* This table combines a 16 bit green + blue value into a screen pixel.  */
   for (g = 0; g < 256; g++) {
      cg = (g & 15) * (g >> 4) * 255/225;
      for (b = 0; b < 256; b++) {
	 cb = (b & 15) * (b >> 4) * 255/225;
	 gbtable[g + b * 256] = makecol(0, cg, cb);
      }
   }
}


/* Copies from our magic format data onto a normal Allegro memory/screen bitmap.  */
void blit_magic_format(BITMAP *bmp, BITMAP *dest, int w, int h)
{
   unsigned long addr;
   unsigned long *data;
   unsigned long in1, in2, in3;
   unsigned long out1, out2;
   int x, y;
    
   /* Warning: this function contains some rather hairy optimisations :-)
    * The fastest way to copy large amounts of data is in aligned 32 bit 
    * chunks. If we expand it out to process 4 pixels per cycle, we can 
    * do this by reading 12 bytes (4 pixels) from the 24 bit source data, 
    * and writing 8 bytes (4 pixels) to the 16 bit destination. Then the 
    * only problem is how to convert our 12 bytes of data into a suitable 
    * 8 byte format, once we've got it loaded into registers. This is done 
    * by some lookup tables, which are arranged so they can process 2 color 
    * components in a single lookup, and allow me to precalculate the 
    * makecol() operation.
    *
    * Warning #2: this code is non endianess-safe. It will not port to
    * a big-endian platform without some major rewriting.
    *
    * Here is a (rather confusing) attempt to diagram the logic of the
    * lookup table lighting conversion from 24 to 16 bit format:
    *
    *
    *  inputs: |     (dword 1)     |     (dword 2)     |     (dword 3)     |
    *  pixels: |   (pixel1)   |   (pixel2)   |   (pixel3)   |   (pixel4)   |
    *  bytes:  | r1   g1   b1   r2   g2   b2   r3   g3   b3   r4   g4   b4 |
    *          |    |         |         |         |         |         |    |
    *  lookup: | rgtable   brtable   gbtable   rgtable   brtable   gbtable |
    *          |    |         |         |         |         |         |    |
    *  pixels: |   (pixel1)   |   (pixel2)   |   (pixel3)   |   (pixel4)   |
    *  outputs |          (dword 1)          |          (dword 2)          |
    *
    *
    * For reference, here is the original, non-optimised but much easier
    * to understand version of this routine:
    *
    *    _farsetsel(screen->seg); 
    *
    *    for (y=0; y<SCREEN_H; y++) { 
    *       addr = bmp_write_line(screen, y); 
    *   
    *       for (x=0; x<SCREEN_W; x++) { 
    *  	       r = bmp->line[y][x*3]; 
    *  	       g = bmp->line[y][x*3+1];
    *  	       b = bmp->line[y][x*3+2]; 
    *
    *  	       r = (r&15) * (r>>4) * 255/225;
    *  	       g = (g&15) * (g>>4) * 255/225;
    *  	       b = (b&15) * (b>>4) * 255/225; 
    *
    *  	       _farnspokew(addr, makecol(r, g, b)); 
    *
    *  	       addr += 2; 
    *       } 
    *    } 
    */

   w /= 4;

   bmp_select(dest);

   for (y = 0; y < h; y++) {
      addr = bmp_write_line(dest, y);
      data = (unsigned long *)bmp->line[y];

      for (x = 0; x < w; x++) {
	 in1 = *(data++);
	 in2 = *(data++);
	 in3 = *(data++);

	 /* trust me, this does make sense, really :-) */
	 out1 = ((rgtable[in1 & 0xFFFF]) | 
		 (brtable[in1 >> 16]) | 
		 (gbtable[in2 & 0xFFFF] << 16));

	 out2 = ((rgtable[in2 >> 16]) | 
		 (brtable[in3 & 0xFFFF]) | 
		 (gbtable[in3 >> 16] << 16));

	 bmp_write32(addr, out1);
	 bmp_write32(addr+4, out2);
	 
	 addr += 8;
      }
   }
   
   bmp_unwrite_line(dest);
}


/* Set uniform lighting over a magic bitmap.  Values range 0 - 15.  */
void set_magic_bitmap_brightness(BITMAP *bmp, int r, int g, int b)
{
    int x, y;
    unsigned char *c;

    r <<= 4, g <<= 4, b <<= 4;

    for (y = 0; y < bmp->h; y++) {
	c = bmp->line[y];
	for (x = 0; x < bmp->w / 3; x++)
	    *c++ |= r, *c++ |= g, *c++ |= b;
    }
}


/* Convenience routines (so the code doesn't look to weird).  */

BITMAP *create_magic_bitmap(int w, int h)
{
    return create_bitmap_ex(8, w * 3, h);
}

void draw_magic_sprite(BITMAP *bmp, BITMAP *sprite, int x, int y)
{
    draw_sprite(bmp, sprite, x * 3, y);
}
