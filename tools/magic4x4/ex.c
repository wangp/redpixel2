/* 
 *  Was exlights.c
 */


#include <allegro.h>
#include "magic4x4.h"


/* double buffer bitmap */
BITMAP *buffer;


/* the two moving bitmap images */
BITMAP *image1;
BITMAP *image2;


/* the light map, drawn wherever the mouse pointer is */
BITMAP *lightmap;



/* creates the light graphic for the mouse pointer, in our magic format */
BITMAP *create_light_graphic()
{
   BITMAP *bmp;
   int x, y;
   int dx, dy;
   int dist, dir;
   int r, g, b;

   bmp = create_bitmap_ex(8, 256*3, 256);

   /* draw the light (a circular color gradient) */
   for (y=0; y<256; y++) {
      for (x=0; x<256; x++) {
	 dx = x-128;
	 dy = y-128;

	 dist = fixtoi(fsqrt(itofix(MID(-32768, dx*dx+dy*dy, 32767))));

	 dir = fixtoi(fatan2(itofix(dy), itofix(dx)));

	 hsv_to_rgb(dir*320.0/256.0, MID(0, dist/128.0, 1), 1, &r, &g, &b);

	 r = r * (128-dist) / 1024;
	 g = g * (128-dist) / 1024;
	 b = b * (128-dist) / 1024;

//	 bmp->line[y][x*3  ] = MID(0, r, 15) << 4;
//	 bmp->line[y][x*3+1] = MID(0, g, 15) << 4;
//	 bmp->line[y][x*3+2] = MID(0, b, 15) << 4;

	 bmp->line[y][x*3  ] = 0xf0;
	 bmp->line[y][x*3+1] = 0xf0;
	 bmp->line[y][x*3+2] = 0xf0;
      }
   }
   
#if 0
   /* draw some solid pixels as well (a cross in the middle).
    * This code doesn't work quite right in 4.4 format, but
    * it doesn't really matter.
    */
   #define magic_putpix(x, y, r, g, b)       \
   {                                         \
      bmp->line[y][(x)*3] &= 0xF0;           \
      bmp->line[y][(x)*3+1] &= 0xF0;         \
      bmp->line[y][(x)*3+2] &= 0xF0;         \
					     \
      bmp->line[y][(x)*3] |= r;              \
      bmp->line[y][(x)*3+1] |= g;            \
      bmp->line[y][(x)*3+2] |= b;            \
   }

   for (x=-15; x<=15; x++) {
      for (y=-2; y<=2; y++) {
	 magic_putpix(128+x, 128+y, x+15, 15-x, 0);
	 magic_putpix(128+y, 128+x, x+15, x+15, 15-x);
      }
   }
#endif

   return bmp;
}



int main(int argc, char *argv[])
{
   BITMAP *tmp;
   PALETTE pal;
   char buf[256];
   int x, y, xc, yc, xl, yl, c, l;

   allegro_init();
   install_keyboard();
   install_timer();
   install_mouse();
   set_color_conversion(COLORCONV_NONE);

   /* set a 15 or 16 bpp video mode */
   set_color_depth(16);
   if (set_gfx_mode(GFX_AUTODETECT, 640, 480, 0, 0) != 0) {
      set_color_depth(15);
      if (set_gfx_mode(GFX_AUTODETECT, 640, 480, 0, 0) != 0) {
	 set_gfx_mode(GFX_TEXT, 0, 0, 0, 0);
	 allegro_message("Error setting a 15 or 16 bpp 640x480 video mode\n%s\n", allegro_error);
	 return 1;
      }
   }

   /* create the double buffer, 8 bpp and three times as wide as the screen */
   buffer = create_bitmap_ex(8, SCREEN_W*3, SCREEN_H);

   /* load the first picture */
   replace_filename(buf, argv[0], "allegro.pcx", sizeof(buf));
   tmp = load_bitmap(buf, pal);
   if (!tmp) {
      destroy_bitmap(buffer);
      set_gfx_mode(GFX_TEXT, 0, 0, 0, 0);
      allegro_message("Error reading %s!\n", buf);
      return 1;
   }

   /* convert it into our special format */
   image1 = get_magic_bitmap_format(tmp, pal);
   destroy_bitmap(tmp);

   /* load the second picture */
   replace_filename(buf, argv[0], "mysha.pcx", sizeof(buf));
   tmp = load_bitmap(buf, pal);
   if (!tmp) {
      destroy_bitmap(buffer);
      destroy_bitmap(image1);
      set_gfx_mode(GFX_TEXT, 0, 0, 0, 0);
      allegro_message("Error reading %s!\n", buf);
      return 1;
   }

   /* convert it into our special format */
   image2 = get_magic_bitmap_format(tmp, pal);
   destroy_bitmap(tmp);

   /* create the light map image */
   lightmap = create_light_graphic();

   /* create our custom color blending map, which does translucency in the
    * bottom five bits and adds the light levels in the top three bits.
    * This version just does a 50% translucency if you are drawing sprites
    * with it, but you could easily make other color maps for different 
    * alpha levels, or for doing additive color, which can work happily
    * in parallel with the light blending.
    */
   color_map = malloc(sizeof(COLOR_MAP));

   for (x=0; x<256; x++) {
      for (y=0; y<256; y++) {
	 xc = x&15;
	 yc = y&15;

	 xl = x>>4;
	 yl = y>>4;

	 if (xc)
	    c = (xc+yc)/2;
	 else
	    c = yc;

	 l = xl+yl;
	 if (l > 15)
	    l = 15;

	 color_map->data[x][y] = c | (l<<4);
      }
   }

   /* generate tables for converting pixels from magic->screen format */
   generate_magic_conversion_tables();

   /* display the animation */
   while (!keypressed()) {
      poll_mouse();

      clear(buffer);

      /* we can draw the graphics using normal calls, just as if they were
       * regular 256 color images. Everything is just three times as wide 
       * as it would usually be, so we need to make sure that we only ever
       * draw to an x coordinate that is a multiple of three (otherwise
       * all the colors would get shifted out of phase with each other).
       */
      blit(image1, buffer, 0, 0, 0, retrace_count%(SCREEN_H+image1->h)-image1->h, image1->w, image1->h);
//      blit(image2, buffer, 0, 0, buffer->w-image2->w, buffer->h-(retrace_count*2/3)%(SCREEN_H+image2->h), image2->w, image2->h);
      draw_sprite(buffer, image2, buffer->w-image2->w, 
		  buffer->h-(retrace_count*2/3)%(SCREEN_H+image2->h));

      /* now we overlay translucent graphics and lights. Having set up a
       * suitable color blending table, these can be done at the same time,
       * either drawing a series of images some of which are translucent
       * sprites and some of which are light maps, or if we want, we can
       * just draw a single bitmap containing both color and light data 
       * with a single call, like this! You could also use graphics 
       * primitives like circlefill() to draw the lights, as long as you
       * do it in a translucent mode.
       */
      draw_trans_sprite(buffer, lightmap, (mouse_x-lightmap->w/6)*3, mouse_y-lightmap->h/2);

      /* this function is the key to the whole thing, converting from our
       * weird 5+3 interleaved format into a regular hicolor pixel that
       * can be displayed on your monitor.
       */
      blit_magic_format_to_screen(buffer);
   }

   clear_keybuf();

   destroy_bitmap(buffer);
   destroy_bitmap(image1);
   destroy_bitmap(image2);
   destroy_bitmap(lightmap);

   free(color_map);

   return 0;
}

END_OF_MAIN();
