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
void datedit_msg(const char *fmt, ...)
{
   va_list args;
   char buf[1024];

   va_start(args, fmt);
   vsprintf(buf, fmt, args);
   va_end(args);

   printf("%s\n", buf);
}



/* callback for starting a 2-part message output */
void datedit_startmsg(const char *fmt, ...)
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
void datedit_endmsg(const char *fmt, ...)
{
   va_list args;
   char buf[1024];

   va_start(args, fmt);
   vsprintf(buf, fmt, args);
   va_end(args);

   printf("%s\n", buf);
}



/* callback for printing errors */
void datedit_error(const char *fmt, ...)
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
int datedit_ask(const char *fmt, ...)
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

#define pop_string(L, x)	(lua_tostring (L, x))
#define pop_userdata(L, x)	(lua_touserdata (L, x))
#define pop_number(L, x)	(lua_tonumber (L, x))



static DATAFILE *new_datafile (void)
{
    DATAFILE *dat = malloc (sizeof (DATAFILE));
    
    dat->dat = NULL;
    dat->type = DAT_END;
    dat->size = 0;
    dat->prop = NULL;
    
    return dat;
}
    
static int _new_datafile (lua_State *L)
    /* (no input) : (dat) */
{
    lua_pushuserdata (L, new_datafile ());
    return 1;
}



static int _save_datafile (lua_State *L)
    /* (filename, dat, compression) : (no output) */
{
    const char *filename  = pop_string (L, 1);
    DATAFILE *dat   = pop_userdata (L, 2);
    int compression = pop_number (L, 3);

    datedit_sort_datafile (dat);
    datedit_save_datafile (dat, filename, 0, compression, 1, 1, 0, NULL);

    return 0;
}



static int _unload_datafile (lua_State *L)
    /* (dat) : (no output) */
{
    unload_datafile (pop_userdata (L, 1));
    return 0;
}



static int _add_to_datafile_bitmap (lua_State *L)
    /* (dat, name, bmp) : (dat) */
{
    DATAFILE *dat    = pop_userdata (L, 1);
    const char *name = pop_string (L, 2);
    BITMAP *bmp      = pop_userdata (L, 3);

    dat = datedit_insert (dat, NULL, name, DAT_BITMAP, bmp, 0);

    lua_pushuserdata (L, dat);
    return 1;
}



static int _add_to_datafile_magic_bitmap (lua_State *L)
    /* (dat, name, magic-bmp) : (dat) */
{
    DATAFILE *dat    = pop_userdata (L, 1);
    const char *name = pop_string (L, 2);
    BITMAP *bmp      = pop_userdata (L, 3);
    DATAFILE *new;

    dat = datedit_insert (dat, &new, name, DAT_BITMAP, bmp, 0);
    datedit_set_property (new, DAT_ID ('M','A','G','K'), "Magical lightbulb");

    lua_pushuserdata (L, dat);
    return 1;
}



static int empty (BITMAP *b)
{
    int x, y, blank;

    blank = makecol_depth (bitmap_color_depth (b), 0, 0, 0);
    
    for (y = 0; y < b->h; y++)
	for (x = 0; x < b->w; x++) 
	    if (getpixel (b, x, y) != blank)
		return 0;

    return 1;
}

static int _add_to_datafile_grab_from_grid (lua_State *L)
    /* (dat, prefix, filename, depth, gridx, gridy) : (dat) */
{
    DATAFILE *dat	 = pop_userdata (L, 1);
    const char *name 	 = pop_string   (L, 2);
    const char *filename = pop_string   (L, 3);
    int depth		 = pop_number   (L, 4);
    int gridx      	 = pop_number   (L, 5);
    int gridy      	 = pop_number   (L, 6);

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

    lua_pushuserdata (L, dat);
    return 1;
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



static BITMAP *create_simple_coloured_lightmap (int radius, int hue, float brightness, float pinpoint)
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
       
	hsv_to_rgb (hue,	/* hue */
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

static BITMAP *create_simple_mono_lightmap (int radius, float brightness,
					    float pinpoint)
{
    BITMAP *light;
    BITMAP *magic;
    int i, l, x, y;
    float dist;
   
    light = create_bitmap_ex (16, radius * 2, radius * 2);
    clear (light);

    for (i = radius - 1; i > 0; i--) {
	dist = MID (pinpoint, (float)i / (float)radius, 1.0);
       
	l = MIN (255, (1 - dist) * brightness);
	circlefill (light, radius, radius, i, makecol16 (l, l, l));
    }

    magic = create_bitmap_ex (8, light->w * 3, light->h);

    for (y = 0; y < light->h; y++)
	for (x = 0; x < light->w; x++) {
	    magic->line[y][x*3  ] = getr_depth (16, getpixel (light, x, y)) & 0xf0;
	    magic->line[y][x*3+1] = getr_depth (16, getpixel (light, x, y)) & 0xf0;
	    magic->line[y][x*3+2] = getr_depth (16, getpixel (light, x, y)) & 0xf0;
	}

    destroy_bitmap (light);

    return magic;
}

static int _create_simple_lightmap (lua_State *L)
    /* (radius, hue, brightness, pinpoint) : (bmp) */
{
    int radius       = pop_number (L, 1);
    int hue 	     = pop_number (L, 2);
    float brightness = pop_number (L, 3);
    float pinpoint   = pop_number (L, 4);

    lua_pushuserdata (L, ((hue > 0)
			  ? (create_simple_coloured_lightmap (radius, hue, brightness, pinpoint))
			  : (create_simple_mono_lightmap (radius, brightness, pinpoint))));
    return 1;
}



static int _create_lightmap_icon (lua_State *L)
    /* (radius, hue) : (bmp) */
{
    int radius = pop_number (L, 1);
    int hue = pop_number (L, 2);
    BITMAP *bmp;
    int r, g, b;

    bmp = create_bitmap_ex (16, 32, 32);
    
    if (hue > 0) {
	hsv_to_rgb (hue, 1.0, 1.0, &r, &g, &b);
	clear_to_color (bmp, makecol16 (r, g, b));
    }
    else
	clear_to_color (bmp, makecol16 (255, 255, 255));

    text_mode (-1);
    textprintf_centre (bmp, font, bmp->w / 2, bmp->h / 2,
		       makecol16 (0, 0, 0), "%d", radius);

    lua_pushuserdata (L, bmp);
    return 1;
}



static int _destroy_bitmap (lua_State *L)
    /* (bmp) : (no output) */
{
    destroy_bitmap (pop_userdata (L, 1));
    return 0;
}



static void export_functions (lua_State *L)
{
#define e(x)	lua_register (L, #x, _##x)

    e (new_datafile);
    e (save_datafile);
    e (unload_datafile);
    e (add_to_datafile_bitmap);
    e (add_to_datafile_magic_bitmap);
    e (add_to_datafile_grab_from_grid);
    e (create_simple_lightmap);
    e (create_lightmap_icon);
    e (destroy_bitmap);

#undef e
}

/*--------------------------------
 * End export functions
 *--------------------------------*/



int main (int argc, char *argv[])
{
    lua_State *L;

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

    L = lua_open (0);
    export_functions (L);
    lua_dofile (L, argv[1]);

    return 0;
}

END_OF_MAIN ();
