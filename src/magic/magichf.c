/*  draw_magic_sprite_h_flip
 *
 *  Hacked from Michael Bukin's C version of Allegro sprite routines.
 */


#include <allegro.h>
#include <allegro/internal/aintern.h>



#define PIXEL_PTR              unsigned char*
#define OFFSET_PIXEL_PTR(p,x)  ((PIXEL_PTR) (p) + 3 * (x))
#define INC_PIXEL_PTR(p)       ((p) += 3)
#define DEC_PIXEL_PTR(p)       ((p) -= 3)

#define PUT_MEMORY_PIXEL(p,c)  ((*(p) = (c)),                           \
				(*(p + 1) = ((c) >> 8)),                \
				(*(p + 2) = ((c) >> 16)))
#define GET_MEMORY_PIXEL(p)    (((uint32_t) (*(p)))			\
				| ((uint32_t) (*((p) + 1)) << 8)	\
				| ((uint32_t) (*((p) + 2)) << 16))	\

#define IS_SPRITE_MASK(b,c)    ((uint32_t) (c) == MASK_COLOR_8)



/* draw_magic_sprite_h_flip:
 *  Draws a sprite to a linear bitmap, flipping horizontally.
 */
void draw_magic_sprite_h_flip(BITMAP *dst, BITMAP *src, int dx, int dy)
{
   int x, y, w, h;
   int dxbeg, dybeg;
   int sxbeg, sybeg;

   if (dst->clip) {
      int tmp;

      tmp = dst->cl/3 - dx;
      sxbeg = ((tmp < 0) ? 0 : tmp);
      dxbeg = sxbeg + dx;

      tmp = dst->cr/3 - dx;
      w = ((tmp > src->w/3) ? src->w/3 : tmp) - sxbeg;
      if (w <= 0)
	 return;

      dxbeg += w - 1;

      tmp = dst->ct - dy;
      sybeg = ((tmp < 0) ? 0 : tmp);
      dybeg = sybeg + dy;

      tmp = dst->cb - dy;
      h = ((tmp > src->h) ? src->h : tmp) - sybeg;
      if (h <= 0)
	 return;
   }
   else {
      w = src->w/3;
      h = src->h;
      sxbeg = 0;
      sybeg = 0;
      dxbeg = dx + w - 1;
      dybeg = dy;
   }

   {
      for (y = 0; y < h; y++) {
	 PIXEL_PTR s = OFFSET_PIXEL_PTR(src->line[sybeg + y], sxbeg);
	 PIXEL_PTR d = OFFSET_PIXEL_PTR(dst->line[dybeg + y], dxbeg);

	 for (x = w - 1; x >= 0; INC_PIXEL_PTR(s), DEC_PIXEL_PTR(d), x--) {
	    uint32_t c = GET_MEMORY_PIXEL(s);
	    if (!IS_SPRITE_MASK(src, c)) {
	       PUT_MEMORY_PIXEL(d, c);
	    }
	 }
      }
   }
}
