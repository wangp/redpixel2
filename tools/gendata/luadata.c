/*
 * Highly specialised hack.
 * Now with an embedded Lua interpreter.
 */


#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <allegro.h>
#include <lua.h>
#include "datedit.h"

#ifndef NO_LOADPNG
    #include <loadpng.h>
#endif



/*--------------------------------
 * Begin ripped out of dat.c 
 *--------------------------------*/

static int err = 0;


/* callback for outputting messages */
void datedit_msg(char *fmt, ...)
{
   va_list args;
   char buf[1024];

   va_start(args, fmt);
   vsprintf(buf, fmt, args);
   va_end(args);

   printf("%s\n", buf);
}



/* callback for starting a 2-part message output */
void datedit_startmsg(char *fmt, ...)
{
   va_list args;
   char buf[1024];

   va_start(args, fmt);
   vsprintf(buf, fmt, args);
   va_end(args);

   printf("%s", buf);
   fflush(stdout);
}



/* callback for ending a 2-part message output */
void datedit_endmsg(char *fmt, ...)
{
   va_list args;
   char buf[1024];

   va_start(args, fmt);
   vsprintf(buf, fmt, args);
   va_end(args);

   printf("%s\n", buf);
}



/* callback for printing errors */
void datedit_error(char *fmt, ...)
{
   va_list args;
   char buf[1024];

   va_start(args, fmt);
   vsprintf(buf, fmt, args);
   va_end(args);

   fprintf(stderr, "%s\n", buf);

   err = 1;
}



/* callback for asking questions */
int datedit_ask(char *fmt, ...)
{
   va_list args;
   char buf[1024];
   int c;

   static int all = FALSE;

   if (all)
      return 'y';

   va_start(args, fmt);
   vsprintf(buf, fmt, args);
   va_end(args);

   printf("%s? (y/n/a/q) ", buf);
   fflush(stdout);

   for (;;) {
      #ifdef HAVE_CONIO_H

	 /* raw keyboard input for platforms that have conio functions */
	 c = getch();
	 if ((c == 0) || (c == 0xE0))
	    getch();

      #else

	 /* stdio version for other systems */
	 fflush(stdin);
	 c = getchar();

      #endif

      switch (c) {

	 case 'y':
	 case 'Y':
	    #ifdef HAVE_CONIO_H
	       printf("%c\n", c);
	    #endif
	    return 'y';

	 case 'n':
	 case 'N':
	    #ifdef HAVE_CONIO_H
	       printf("%c\n", c);
	    #endif
	    return 'n';

	 case 'a':
	 case 'A':
	    #ifdef HAVE_CONIO_H
	       printf("%c\n", c);
	    #endif
	    all = TRUE;
	    return 'y';

	 case 'q':
	 case 'Q':
	    #ifdef HAVE_CONIO_H
	       printf("%c\n", c);
	    #endif
	    return 27;

	 case 27:
	    #ifdef HAVE_CONIO_H
	       printf("\n");
	    #endif
	    return 27;
      }
   }
}

/*--------------------------------
 * End ripped out of dat.c 
 *--------------------------------*/



/*--------------------------------
 * Begin export functions
 *--------------------------------*/

#define pop_string(x)	(lua_getstring (lua_getparam (x)))
#define pop_userdata(x)	(lua_getuserdata (lua_getparam (x)))
#define pop_number(x)	(lua_getnumber (lua_getparam (x)))



static DATAFILE *new_datafile (void)
{
   DATAFILE *dat = malloc (sizeof (DATAFILE));

   dat->dat = NULL;
   dat->type = DAT_END;
   dat->size = 0;
   dat->prop = NULL;

   return dat;
}
    
static void _new_datafile (void)
    /* (no input) : (dat) */
{
   lua_pushuserdata (new_datafile ());
}



static void _save_datafile (void)
    /* (filename, dat, strip, compression, verbose, write_msg, backup) : (no output) */
{
    char *filename  = pop_string (1);
    DATAFILE *dat   = pop_userdata (2);
    int strip       = pop_number (3);
    int compression = pop_number (4);
    int verbose     = pop_number (5);
    int write_msg   = pop_number (6);
    int backup      = pop_number (7);

    datedit_sort_datafile (dat);
    datedit_save_datafile (dat, filename, strip, compression, verbose, write_msg, backup, NULL);
}



static void _unload_datafile (void)
    /* (dat) : (no output) */
{
    unload_datafile (pop_userdata (1));
}



static void _add_to_datafile_bitmap (void)
    /* (dat, name, bmp) : (dat) */
{
    DATAFILE *dat = pop_userdata (1);
    char *name	  = pop_string (2);
    BITMAP *bmp   = pop_userdata (3);

    dat = datedit_insert (dat, NULL, name, DAT_BITMAP, bmp, 0);

    lua_pushuserdata (dat);
}



static int empty (BITMAP *b)
{
    int x, y;
    
    for (y = 0; y < b->h; y++)
	for (x = 0; x < b->w; x++)
	    if (getpixel (b, x, y) != bitmap_mask_color (b))
		return 0;
    return 1;
}

static void _add_to_datafile_grab_from_grid (void)
    /* (dat, prefix, filename, depth, gridx, gridy) : (dat) */
{
    DATAFILE *dat  = pop_userdata (1);
    char *name	   = pop_string   (2);
    char *filename = pop_string   (3);
    int depth      = pop_number   (4);
    int gridx      = pop_number   (5);
    int gridy      = pop_number   (6);

    BITMAP *bmp;
    PALETTE pal;
    DATAFILE *file;
		
    bmp = load_bitmap (filename, pal);
    if (!bmp) {
	fprintf (stderr, "Error loading %s\n", filename);
	goto end;
    }

    select_palette (pal);

    file = new_datafile ();

    {
	BITMAP *b;
	int x, y, i = 0;
	char name[20];
	
	for (y = 0; y < bmp->h; y += gridy)
	    for (x = 0; x < bmp->w; x += gridx) {
		b = create_bitmap_ex (depth, gridx, gridy);
		blit (bmp, b, x, y, 0, 0, gridx, gridy);
		
		if (empty (b)) {
		    destroy_bitmap (b);
		    continue;
		}
			    
		sprintf (name, "%03d", i++);
		file = datedit_insert (file, NULL, name, DAT_BITMAP, b, 0);
	    }
    }

    dat = datedit_insert (dat, NULL, name, DAT_FILE, file, 0);
    destroy_bitmap (bmp);

  end:

    lua_pushuserdata (dat);
}



static BITMAP *combine (BITMAP *col, BITMAP *light)
{
    BITMAP *bmp;
    int x, y;
    int depth;
    int c, l, r, g, b;

    bmp = create_bitmap_ex (8, col->w * 3, col->h);
    depth = bitmap_color_depth (col);

    for (y = 0; y < col->h; y++)
	for (x = 0; x < col->w; x++) {
	    c = getpixel (col, x, y);
	    r = getr_depth (depth, c);
	    g = getg_depth (depth, c);
	    b = getb_depth (depth, c);

	    /* The lightmap is drawn monochrome, so the red value
               represents green and blue as well.  */
	    l = getr_depth (depth, getpixel (light, x, y));

	    /* We only want lighting information, so that although we
	       have coloured lighting, it will only tint the pixels
	       that are already present, not add anymore itself.  */
	    bmp->line[y][x*3  ] = (int) ((r / 255.0) * l) & 0xf0;
	    bmp->line[y][x*3+1] = (int) ((g / 255.0) * l) & 0xf0;
	    bmp->line[y][x*3+2] = (int) ((b / 255.0) * l) & 0xf0;
	}
    
    return bmp;
}



static BITMAP *create_simple_lightmap (int radius, int hue, float brightness, float pinpoint)
{
   BITMAP *bmp, *light;
   BITMAP *magic;
   int i;
   float dist;
   int r, g, b, l;
   
   bmp = create_bitmap_ex (16, radius * 2, radius * 2);
   light = create_bitmap_ex (16, radius * 2, radius * 2);
   clear (bmp);
   clear (light);

   for (i = radius - 1; i > 0; i--) {
       dist = MID (pinpoint, (float)i / (float)radius, 1.0);
       
       hsv_to_rgb (hue,		/* hue */
		   dist,	/* saturation: how much tinting */
		   1,		/* value: brightness */
		   &r, &g, &b);

       circlefill (bmp, radius, radius, i, makecol16 (r, g, b));

       l = MIN (255, (1 - dist) * brightness);
       circlefill (light, radius, radius, i, makecol16 (l, l, l));
   }

   magic = combine (bmp, light);

   destroy_bitmap (light);
   destroy_bitmap (bmp);

   return magic;
}

static void _create_simple_lightmap (void)
    /* (radius, hue, brightness, pinpoint) : (bmp) */
{
    lua_pushuserdata (create_simple_lightmap (pop_number (1),
					      pop_number (2),
					      pop_number (3),
					      pop_number (4)));
}



static BITMAP *create_mono_lightmap (int radius, float brightness, float pinpoint)
{
   BITMAP *bmp, *light;
   BITMAP *magic;
   int i;
   float dist;
   int r, g, b, l;
   
   bmp = create_bitmap_ex (16, radius * 2, radius * 2);
   light = create_bitmap_ex (16, radius * 2, radius * 2);
   clear (bmp);
   clear (light);

   for (i = radius - 1; i > 0; i--) {
       dist = MID (0.0, (float)i / (float)radius, 1.0);
       r = g = b = (255 * dist);
       
       circlefill (bmp, radius, radius, i, makecol16 (r, g, b));

       l = MIN (255, (1 - dist) * brightness);
       circlefill (light, radius, radius, i, makecol16 (l, l, l));
   }

   magic = combine (bmp, light);

   destroy_bitmap (light);
   destroy_bitmap (bmp);

   return magic;
}

static void _create_mono_lightmap (void)
    /* (radius, brightness, pinpoint) : (bmp) */
{
    lua_pushuserdata (create_mono_lightmap (pop_number (1),
					    pop_number (2),
					    pop_number (3)));
}



/* Converts a bitmap from the normal Allegro format into our magic layout.  */
/* from magic4x4.c - XXX */
#define AMBIENT_LIGHT 0xf0

static BITMAP *get_magic_bitmap_format(BITMAP *orig, PALETTE pal)
{
   BITMAP *bmp;
   int c, r, g, b;
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

	 bmp->line[y][x*3  ] = r * 15/255 | AMBIENT_LIGHT;
	 bmp->line[y][x*3+1] = g * 15/255 | AMBIENT_LIGHT;
	 bmp->line[y][x*3+2] = b * 15/255 | AMBIENT_LIGHT;
      }
   }

   if (pal) unselect_palette();

   /* Return our new, magic format version of the image.  */
   return bmp;
}


static void _create_lightmap_icon (void)
    /* (radius, hue) : (bmp) */
{
    int radius = pop_number (1);
    int hue = pop_number (2);
    BITMAP *bmp, *magic;
    int r, g, b;

    hsv_to_rgb (hue, 1.0, 1.0, &r, &g, &b);

    bmp = create_bitmap_ex (16, 32, 32);
    clear_to_color (bmp, makecol16 (r, g, b));

    text_mode (-1);
    textprintf_centre (bmp, font, bmp->w / 2, bmp->h / 2,
		       makecol16 (0, 0, 0), "%d", radius);

    magic = get_magic_bitmap_format (bmp, 0);
    destroy_bitmap (bmp);
    
    lua_pushuserdata (magic);
}



static void _destroy_bitmap (void)
    /* (bmp) : (no output) */
{
    destroy_bitmap (pop_userdata (1));
}



static void export_functions ()
{
#define e(x)	lua_register (#x, _##x)

    e (new_datafile);
    e (save_datafile);
    e (unload_datafile);
    e (add_to_datafile_bitmap);
    e (add_to_datafile_grab_from_grid);
    e (create_simple_lightmap);
    e (create_mono_lightmap);
    e (create_lightmap_icon);
    e (destroy_bitmap);

#undef e
}

/*--------------------------------
 * End export functions
 *--------------------------------*/



int main (int argc, char *argv[])
{
    if (argc != 2) {
	fprintf (stderr, "usage: %s script.lua\n", argv[0]);
	return 1;
    }

    install_allegro (SYSTEM_NONE, &errno, atexit);
    set_color_conversion (COLORCONV_NONE);
    datedit_init ();

#ifndef NO_LOADPNG
    register_png_file_type ();
#endif

    lua_open ();
    export_functions ();
    lua_dofile (argv[1]);

    return 0;
}

END_OF_MAIN ();
