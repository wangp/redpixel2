/* 
 *  RENDER.C - part of the EGG system.
 *
 *  Renders the current animation frame into an Allegro bitmap.
 *
 *  By Shawn Hargreaves.
 */


#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <allegro.h>

#include "egg.h"



/* putpix:
 *  Draws a pixel onto the output buffer.
 */
static void putpix(double *image, double r, double g, double b, double a, int add, int mul, int sub)
{
   if (add) {
      /* additive color */
      image[0] += r*a;
      image[1] += g*a;
      image[2] += b*a;
      image[3] += 256.0*a;
   }
   else if (mul) {
      /* multiplicative color */
      image[0] *= r*a;
      image[1] *= g*a;
      image[2] *= b*a;
      image[3] *= 256.0*a;
   }
   else if (sub) {
      /* subtractive color */
      image[0] -= r*a;
      image[1] -= g*a;
      image[2] -= b*a;
      image[3] -= 256.0*a;
   }
   else {
      /* interpolated color blending */
      image[0] = image[0]*(1.0-a) + r*a;
      image[1] = image[1]*(1.0-a) + g*a;
      image[2] = image[2]*(1.0-a) + b*a;
      image[3] = image[3]*a;
   }
}



/* draw_particle:
 *  This is where it all happens...
 */
static void draw_particle(EGG_PARTICLE *part, EGG *egg, double *image, int w, int h)
{
   double x = get_egg_variable(part, NULL, egg, "x");
   double y = get_egg_variable(part, NULL, egg, "y");
   double z = get_egg_variable(part, NULL, egg, "z");

   double size = get_egg_variable(part, NULL, egg, "size");
   double focus = get_egg_variable(part, NULL, egg, "focus");
   double aa = get_egg_variable(part, NULL, egg, "aa");

   double wrapx = get_egg_variable(part, NULL, egg, "wrapx");
   double wrapy = get_egg_variable(part, NULL, egg, "wrapy");

   double r = get_egg_variable(part, NULL, egg, "r");
   double g = get_egg_variable(part, NULL, egg, "g");
   double b = get_egg_variable(part, NULL, egg, "b");
   double a = get_egg_variable(part, NULL, egg, "a");

   double scale = get_egg_variable(part, NULL, egg, "_scale");
   double dist = get_egg_variable(part, NULL, egg, "_dist");
   double fov = get_egg_variable(part, NULL, egg, "_fov");

   int add = get_egg_variable(part, NULL, egg, "add");
   int mul = get_egg_variable(part, NULL, egg, "mul");
   int sub = get_egg_variable(part, NULL, egg, "sub");

   int drawn = FALSE;

   double dx, dy, d, cvg;
   int xx, yy;

   /* 3d projection */
   z = (z + dist) / 100.0 * fov;

   if (z < 0.0001)
      return;

   x = (x * scale / z) + w/2;
   y = (y * scale / z) + h/2;

   size = size * scale / z;

   a /= 255.0;

   if (focus)
      focus = 1.0 / focus;

   /* scan through the blob */
   for (yy=(int)(y-size/2.0); yy<=(int)(y+size/2.0); yy++) {
      for (xx=(int)(x-size/2.0); xx<=(int)(x+size/2.0); xx++) {
	 if ((((xx >= 0) && (xx < w)) || wrapx)
          && (((yy >= 0) && (yy < h)) || wrapy)) {
	    dx = ABS(xx-x);
	    dy = ABS(yy-y);

	    /* approximate a circle as an octagon */
	    if (dx > dy)
	       d = dy/2.0 + dx;
	    else
	       d = dx/2.0 + dy;

	    /* coverage calculation */
	    cvg = 1.0-d/size*2.0;

	    /* plot the pixel */
	    if (cvg > 0) {
	       if (!aa)
		  cvg = a;
	       else if (focus != 1.0)
		  cvg = pow(cvg*a, focus);
	       else
		  cvg *= a;

	       putpix(image+((xx+256*w)%w+((yy+256*h)%h)*w)*4, r, g, b,
                      cvg, add, mul, sub);

	       drawn = TRUE;
	    }
	 }
      }
   }

   if ((!drawn) && (!aa)) {
      xx = (int)(x+0.5);
      yy = (int)(y+0.5);

      if ((xx >= 0) && (xx < w) && (yy >= 0) && (yy < h))
	 putpix(image+((xx+256*w)%w+((yy+256*h)%h)*w)*4, r, g, b,
                a, add, mul, sub);
   }
}



/* part_cmp:
 *  qsort() callback for comparing particle depths.
 */
static int part_cmp(const void *e1, const void *e2)
{
   EGG_PARTICLE **p1 = (EGG_PARTICLE **)e1;
   EGG_PARTICLE **p2 = (EGG_PARTICLE **)e2;

   double d1 = get_egg_variable(*p1, NULL, NULL, "z");
   double d2 = get_egg_variable(*p2, NULL, NULL, "z");

   if (d1 > d2)
      return -1;
   else if (d1 < d2)
      return 1;
   else
      return 0;
}



/* lay_egg:
 *  Graphical output routine.
 */
void lay_egg(EGG *egg, BITMAP **bmp, int nplanes, double *planes)
{
   EGG_PARTICLE *part, **part_list=NULL;
   double *image, *im;
   int i, x, y, bpp;
   int plane;
   double z;

   image = malloc(sizeof(double)*bmp[0]->w*bmp[0]->h*4);

   if (egg->part_count > 0) {
      part_list = malloc(egg->part_count * sizeof(EGG_PARTICLE *));

      part = egg->part;
      for (i=0; i<egg->part_count; i++) {
         part_list[i] = part;
         part = part->next;
      }

      if (egg->part_count > 1)
         qsort(part_list, egg->part_count, sizeof(EGG_PARTICLE *), part_cmp);
   }

   for (plane=0; plane<nplanes-1; plane++) {
      for (i=0; i<bmp[plane]->w*bmp[plane]->h*4; i++)
         image[i] = 0;

      if (egg->part_count > 0) {
         for (i=0; i<egg->part_count; i++) {
            z = get_egg_variable(part_list[i], NULL, egg, "z");
            if (planes[plane]<=z && z<planes[plane+1]) {
               draw_particle(part_list[i], egg, image,
                             bmp[plane]->w, bmp[plane]->h);
            }
         }
      }

      solid_mode();

      bpp = bitmap_color_depth(bmp[plane]);

      im = image;

      for (y=0; y<bmp[plane]->h; y++) {
         for (x=0; x<bmp[plane]->w; x++) {
	    int r = MID(0, im[0], 255);
	    int g = MID(0, im[1], 255);
	    int b = MID(0, im[2], 255);
	    int a = MID(0, im[3], 255);

	    putpixel(bmp[plane], x, y, makeacol_depth(bpp, r, g, b, a));

	    im += 4;
         }
      }
   }

   if (egg->part_count > 0) free(part_list);
   free(image);
}


