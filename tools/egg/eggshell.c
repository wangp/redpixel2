/* 
 *  EGGSHELL.C - part of the EGG system.
 *
 *  The main script evaluation utility.
 *
 *  By Shawn Hargreaves.
 */


#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <float.h>
#include <time.h>
#include <allegro.h>

#include "egg.h"



char *in_file = NULL;
char *out_file = NULL;
char *pal_file = NULL;

int frames = -1;
int frame = 0;

int bpp = -1;

int size_x = -1;
int size_y = -1;

int mode_x = -1;
int mode_y = -1;

int step = FALSE;

int modesel = FALSE;

int pregen = FALSE;

char error[256];

EGG *the_egg;

PALETTE pal;

BITMAP **bmp[4096];

int alpha = FALSE;

int shoot_from = 0;
int shoot_freq = 1;

int nplanes = 0;
double *planes = NULL;

void usage()
{
   allegro_message(
     "\n\n"
     "        EGG (Explosion Graphics Generator) script rendering utility\n"
     "                             Version %d.%d\n\n"
     "                    By Shawn Hargreaves, October 1998\n\n\n\n"
     " Usage: 'eggshell filename.egg [options]'\n\n\n"
     " Options:\n\n"
     "  '-o name' sets the output file basename (without number or"
                 " .tga extension)\n"
     "  '-pal filename.[pcx|bmp|lbm|tga]' specifies a palette"
                 " for color reduction)\n"
     "  '-frames num' specifies the number of animation frames to generate\n"
     "  '-bpp depth' specifies the output color depth (8, 15, 16, 24, or 32)\n"
     "  '-size x y' specifies the output image size\n"
     "  '-mode x y' specifies the desired screen resolution\n"
     "  '-step' selects single-step mode when outputting to the screen\n"
     "  '-modesel' brings up the Allegro video mode selection dialog\n"
     "  '-pregen' precalculates graphic frames for smoother playback\n"
     "  '-alpha' allows the animation to be saved with alpha channel\n"
     "  '-shoot from freq' specifies what frames to save\n"
     "  '-plane <z>' allows two or more animations to be generated\n"
     "\n",
     EGG_VERSION_MAJOR,EGG_VERSION_MINOR
  );
}



int set_video_mode()
{
   int card = GFX_AUTODETECT;
   int ret = 0;

   if (modesel) {
      set_gfx_mode(GFX_SAFE, 320, 200, 0, 0);
      set_palette(desktop_palette);

      if (!gfx_mode_select_ex(&card, &mode_x, &mode_y, &bpp))
	 return FALSE;
   }

   if ((mode_x <= 0) || (mode_y <= 0)) {
      mode_x = 640;
      mode_y = 480;
   }

   if (bpp <= 0) {
      if (pal_file) {
	 set_color_depth(8);
	 ret = set_gfx_mode(card, mode_x, mode_y, 0, 0);
      }
      else {
	 set_color_depth(32);
	 if (set_gfx_mode(card, mode_x, mode_y, 0, 0) != 0)
	    set_color_depth(24);
	    if (set_gfx_mode(card, mode_x, mode_y, 0, 0) != 0)
	       set_color_depth(16);
	       if (set_gfx_mode(card, mode_x, mode_y, 0, 0) != 0)
		  set_color_depth(15);
		  if (set_gfx_mode(card, mode_x, mode_y, 0, 0) != 0)
		     set_color_depth(8);
		     if (set_gfx_mode(card, mode_x, mode_y, 0, 0) != 0)
			ret = 1;
      }
   }
   else { 
      set_color_depth(bpp);
      ret = set_gfx_mode(card, mode_x, mode_y, 0, 0);
   }

   if (ret != 0) {
      set_gfx_mode(GFX_TEXT, 0, 0, 0, 0);
      allegro_message("Error setting %dx%d %d bpp video mode:\n%s\n",
        mode_x, mode_y, bpp, allegro_error);
      return FALSE;
   }

   return TRUE;
}



volatile int counter = 0;


void counter_proc(void)
{
   counter++;
}

END_OF_FUNCTION(counter_proc);



void view_animation()
{
   int f = 0;
   int fps = 30;
   int lastfps = 30;
   int play = FALSE;
   int n;

   clear(screen);

   LOCK_VARIABLE(counter);
   LOCK_FUNCTION(counter_proc);

   install_int_ex(counter_proc, BPS_TO_TIMER(fps));

   for (;;) {
      vsync();
      for (n=0; n<nplanes-1; n++) {
         blit(bmp[f][n], screen, 0, 0,
              (SCREEN_W-(((nplanes-1)/2-n)*size_x*2)-size_x)/2,
              (SCREEN_H-size_y)/2, size_x, size_y);
      }
      textprintf(screen, font, 0, 0, makecol(128, 128, 128),
        "Frame %d, %d fps, arrows/home/end/pgup/pgdn/space to control        ",
        f+1, fps);

      if (play) {
	 while (counter > 0) {
	    if (f < frame-1)
	       f++;
	    else
	       play = FALSE;

	    counter--;
	 }
      }
      else
	 counter = 0;

      while (keypressed()) {

	 switch (readkey()>>8) {

	    case KEY_LEFT:
	    case KEY_DOWN:
	       if (f > 0)
		  f--;
	       break;

	    case KEY_RIGHT:
	    case KEY_UP:
	       if (f < frame-1)
		  f++;
	       break;

	    case KEY_HOME:
	       f = 0;
	       break;

	    case KEY_END:
	       f = MAX(frame-1, 0);
	       break;

	    case KEY_PGDN:
	       if (fps > 5)
		  fps--;
	       break;

	    case KEY_PGUP:
	       if (fps < 120)
		  fps++;
	       break;

	    case KEY_SPACE:
	       if (play) {
		  play = FALSE;
	       }
	       else {
		  play = TRUE;
		  if (f >= frame-1)
		     f = 0;
	       }
	       break;

	    case KEY_ESC:
	       return;
	 }
      }

      if (fps != lastfps) {
	 install_int_ex(counter_proc, BPS_TO_TIMER(fps));
	 lastfps = fps;
      }
   }
}


/*
 *   Puts magic pink where alpha is 0 (thus an invisible pixel)
 */
void clean_bitmap(BITMAP *bitmap)
{
   int x, y;
   int pix;
   int alpha;

   /* Do nothing for other color depthes */
   if (bitmap_color_depth(bitmap) != 32) return;

   for (y=0; y<bitmap->h; y++) for (x=0; x<bitmap->w; x++) {
      pix = getpixel(bitmap, x, y);
      alpha = geta32(pix);
      if (alpha == 0) {
         pix = makeacol32(255, 0, 255, 0);
         putpixel(bitmap, x, y, pix);
      }
   }
}


void save_animation()
{
   char name[256];
   int i;
   int file_id;
   int plane;

   clear(screen);

   file_id = 0;
   for (i=shoot_from; i<frame; i+=shoot_freq) {
      for (plane=0; plane<nplanes-1; plane++) {
         if (nplanes==2)
            sprintf(name, "%s%03d.tga", out_file, file_id);
         else
            sprintf(name, "%s%03d%02d.tga", out_file, file_id, plane);
         strlwr(name);

         textprintf_centre(screen, font, SCREEN_W/2, SCREEN_H/2,
           makecol(128, 128, 128), "Writing %s...", name);

         clean_bitmap(bmp[i][plane]);
         if (save_bitmap(name, bmp[i][plane], pal) != 0) {
	    set_gfx_mode(GFX_TEXT, 0, 0, 0, 0);
	    allegro_message("Error writing '%s'\n", name);
	    return;
         }
      }
      file_id++;
   }
}



static int plane_compare(const void *e1, const void *e2)
{
   if ((*((double*)e1))>(*((double*)e2))) return 1;
   return -1;
}



int main(int argc, char *argv[])
{
   int ret = 0;
   int i, plane;
   char *endptr;

   allegro_init();
   install_keyboard();
   install_mouse();
   install_timer();

   srand(time(NULL));

   for (i=1; i<argc; i++) {

      if (stricmp(argv[i], "-bpp") == 0) {
	 if ((i >= argc-1) || (bpp > 0)) {
	    usage();
	    return 1;
	 }

	 bpp = atoi(argv[++i]);
      }
      else if (stricmp(argv[i], "-frames") == 0) {
	 if ((i >= argc-1) || (frames > 0)) {
	    usage();
	    return 1;
	 }

	 frames = atoi(argv[++i]);
      }
      else if (stricmp(argv[i], "-size") == 0) {
	 if ((i >= argc-2) || (size_x > 0) || (size_y > 0)) {
	    usage();
	    return 1;
	 }

	 size_x = atoi(argv[++i]);
	 size_y = atoi(argv[++i]);
      }
      else if (stricmp(argv[i], "-mode") == 0) {
	 if ((i >= argc-2) || (mode_x > 0) || (mode_y > 0)) {
	    usage();
	    return 1;
	 }

	 mode_x = atoi(argv[++i]);
	 mode_y = atoi(argv[++i]);
      }
      else if (stricmp(argv[i], "-step") == 0) {
	 step = TRUE;
      }
      else if (stricmp(argv[i], "-modesel") == 0) {
	 modesel = TRUE;
      }
      else if (stricmp(argv[i], "-pregen") == 0) {
	 pregen = TRUE;
      }
      else if (stricmp(argv[i], "-o") == 0) {
	 if ((i >= argc-1) || (out_file)) {
	    usage();
	    return 1;
	 }

	 out_file = argv[++i];
      }
      else if (stricmp(argv[i], "-pal") == 0) {
	 if ((i >= argc-1) || (pal_file)) {
	    usage();
	    return 1;
	 }

	 pal_file = argv[++i];
      }
      else if (stricmp(argv[i], "-shoot") == 0) {
        if ((i >= argc-2) /*|| (shoot_from > 0) || (shoot_freq > 0)*/) {
           usage();
           return 1;
        }

        shoot_from = atoi(argv[++i]);
        shoot_freq = atoi(argv[++i]);
      }
      else if (stricmp(argv[i], "-alpha") == 0) {
	 alpha = TRUE;
      }
      else if (stricmp(argv[i], "-plane") == 0) {
         planes=(double*)realloc(planes,(nplanes+1)*sizeof(double));
         if (!planes) {
            allegro_message("Not enough memory");
            return 1;
         }
	 if ((i >= argc-1)) {
	    usage();
	    return 1;
	 }
         planes[nplanes++]=strtod(argv[++i],&endptr);
         if (*endptr) {
            usage();
            return 1;
         }
      }
      else {
	 if ((*argv[i] == '-') || (in_file)) {
	    usage();
	    return 1;
	 }

	 in_file = argv[i];
      }
   }

   if (!in_file) {
      usage();
      return 1;
   }

   if ((pregen) && ((frames <= 0) || (step))) {
      allegro_message("The '-pregen' option requires '-frames num' to be"
        " specified,\nand cannot be used at the same time as '-step'\n");
      return 1;
   }

   planes=realloc(planes, (nplanes+2)*sizeof(double));
   planes[nplanes++]=-DBL_MAX;
   planes[nplanes++]=DBL_MAX;
   qsort(planes,nplanes,sizeof(double),plane_compare);

   if (!set_video_mode()) {
      ret = 1;
      goto getout;
   }

   the_egg = load_egg(in_file, error);

   if (!the_egg) {
      set_gfx_mode(GFX_TEXT, 0, 0, 0, 0);
      allegro_message("%s", error);
      return 1;
   }

   if (size_x <= 0)
      size_x = 128;

   if (size_y <= 0)
      size_y = 128;

   if (pal_file) {
      BITMAP *tmp = load_bitmap(pal_file, pal);

      if (!tmp) {
	 set_gfx_mode(GFX_TEXT, 0, 0, 0, 0);
	 allegro_message("Error reading palette from '%s'\n", pal_file);
	 ret = 1;
	 goto getout;
      }

      destroy_bitmap(tmp);
   }
   else
      generate_332_palette(pal);

   clear(screen);

   set_palette(pal);

   text_mode(0);

   do {
      if (pregen) {
	 textprintf_centre(screen, font, SCREEN_W/2, SCREEN_H/2-24,
           makecol(128, 128, 128), "Rendering frame %d/%d",
           the_egg->frame+1, frames);
	 textprintf_centre(screen, font, SCREEN_W/2, SCREEN_H/2+24,
           makecol(128, 128, 128), "Please Wait");
      }

      if (update_egg(the_egg, error) != 0) {
	 allegro_exit();
	 printf("Error running EGG script:\n%s\n\n", error);
	 ret = 1;
	 goto getout;
      }

      bmp[frame]=malloc((nplanes-1)*sizeof(BITMAP*));
      for (plane=0; plane<nplanes-1; plane++) {
         if (!alpha) {
            bmp[frame][plane] = create_bitmap(size_x, size_y);
         }
         else {
            bmp[frame][plane] = create_bitmap_ex(32, size_x, size_y);
         }
         if (!bmp[frame][plane]) {
            allegro_message("Not enough memory");
            goto getout;
         }
      }

      lay_egg(the_egg, bmp[frame], nplanes, planes);

      if (!pregen) {
	 vsync();
         for (plane=0; plane<nplanes-1; plane++) {
	    blit(bmp[frame][plane], screen, 0, 0,
              (SCREEN_W-(((nplanes-1)/2-plane)*size_x*2)-size_x)/2,
              (SCREEN_H-size_y)/2, size_x, size_y);
         }
	 textprintf(screen, font, 0, 0, makecol(128, 128, 128),
           "Frame %d, %d particles        ",
           the_egg->frame, the_egg->part_count);
      }

      if ((!pregen) && (!out_file)) {
         for (plane=0; plane<nplanes-1; plane++) {
	    destroy_bitmap(bmp[frame][plane]);
	       bmp[frame][plane] = NULL;
         }
      }

      frame++;

      if ((step) || (keypressed())) {
	 if ((readkey()&0xFF) == 27)
	    break;

	 clear_keybuf();
      }

   } while (((frames <= 0) || (frame < frames)) && (frame < 4096));

   if (pregen)
      view_animation();

   if (out_file)
      save_animation();

   if (planes)
      free(planes);

   getout:

   if (the_egg)
      destroy_egg(the_egg);

   for (i=0; i<frame; i++) {
      if (bmp[frame]) {
         for (plane=0; plane<nplanes-1; plane++) {
            if (bmp[frame][plane])
	       destroy_bitmap(bmp[frame][plane]);
         }
      }
      if (bmp[frame]) free(bmp[frame]);
   }

   return ret;
}

END_OF_MAIN();

