/* stub.c
 * 
 * Peter Wang <tjaden@psynet.net>
 */


#include <stdio.h>
#include <unistd.h>
#include <stone.h>
#include <allegro.h>
#include "export.h"


static int setup_video (int w, int h, int d)
{
    int depths[] = { 16, 15, 32, 24, 0 }, *i;
    
    if (d != -1) {
	set_color_depth (d);
	if (set_gfx_mode (GFX_AUTODETECT, w, h, 0, 0) == 0)
	    return 0;
    }
    else {
        for (i = depths; *i; i++) {
	    set_color_depth (*i);
	    if (set_gfx_mode (GFX_AUTODETECT, w, h, 0, 0) == 0)
		return 0;
	}
    }
    
    return -1;
}


static void setup_allegro (int w, int h, int d)
{
    allegro_init ();
    install_timer ();
    install_keyboard ();

    if (install_mouse () < 0) {
	allegro_message ("Error installing mouse.\n");
	exit (1);
    }
    
    if (setup_video (w, h, d) < 0) {
	allegro_message ("Error setting video mode.\n");
	exit (1);
    }
}


static int entry (char *name, int argc, char *argv[])
{
    int (*entry) (int argc, char *argv[]);
    
    entry = stone_sym (name);
    if (!entry) 
	return 1;
    
    return entry (argc, argv);
}


static char *stub_compiler_default_step [] = {
    "&prefix;gcc -c &src-dir; -I. -W -Wall -Werror -Wno-unused -O3 "
    "-Ieditor -Iug -Igui -Istore "
    "-ffast-math -fomit-frame-pointer -m486 -o &obj-dir;&option;&suffix;"
};

static stone_compiler stub_compiler_default = {
    NULL, "gcc", "gcc", 1, stub_compiler_default_step, ".c", ".o"
};


static char *dirs[] = {
    "gui", "ug", "editor", "store", NULL
};


int main (int argc, char *argv[])
{
    int w = 640, h = 480, d = -1, c;
    char path[1024];
    int i;
    
    opterr = 0;
    
    while ((c = getopt (argc, argv, ":w:h:d:")) != -1) {
	switch (c) {
	    case 'w':
		w = atoi (optarg);
		break;
	    case 'h':
		h = atoi (optarg);
		break;
	    case 'd':
		d = atoi (optarg);
		break;
	    case ':':
	    	fprintf (stderr, "Option `%c' missing argument.\n", optopt);
		return 1;
	    case '?':
	    	fprintf (stderr, "Unrecognised option `%c'.\n", optopt);
		return 1;
	    default:
	    	abort ();
	}
    }
            
    export_symbols ();

    stone_compiler_list = &stub_compiler_default;

    for (i = 0; dirs[i]; i++) 
	stone_add_search (replace_filename (path, argv[0], dirs[i], sizeof path));
    
    for (i = 0; dirs[i]; i++)
	if (!stone_load_dir (replace_filename (path, argv[0], dirs[i], sizeof path)))
	    return 1;

    if (!stone_link_all ())
	return 1;

    setup_allegro (w, h, d);

    return entry ("editor", argc, argv);
}

END_OF_MAIN();
