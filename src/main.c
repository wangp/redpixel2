/* main.c
 * 
 * Peter Wang <tjaden@users.sourceforge.net>
 */


#include <stdio.h>
#include <allegro.h>
#include "editor.h"
#include "game.h"
#include "gameinit.h"
#include "gameclt.h"
#include "gamesrv.h"


static int setup_video (int w, int h, int d)
{
    int depths[] = { 16, 15, 0 }, *i;
    
    set_color_conversion (COLORCONV_NONE);
   
    if (d > 0) {
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
        allegro_message ("Error initialising mouse.\n");
	exit (1);
    }
        
    set_window_title ("Red Pixel II");

    if (setup_video (w, h, d) < 0) {
        allegro_message ("Error setting video mode.\n%s\n", allegro_error);
	exit (1);
    }
}


int main (int argc, char *argv[])
{
    int w = 320, h = 200, d = -1;
    int run_editor = 0;
    int run_server = 0;
    int num_clients = 1;
    const char *map = "test.pit";
    const char *addr = "127.0.0.1";
    int c;
    
    opterr = 0;
    
    while ((c = getopt (argc, argv, "a:e:w:h:d:m:n:s")) != -1) {
	switch (c) {
	    case 'a':
		addr = optarg;
		break;
	    case 'e':
		run_editor = 1;
		break;
	    case 'w':
		w = atoi (optarg);
		break;
	    case 'h':
		h = atoi (optarg);
		break;
	    case 'd':
		d = atoi (optarg);
		if ((d != 15) && (d != 16)) {
		    fprintf (stderr, "Only 15 and 16 bpp are valid.\n");
		    return 1;
		}
		break;
	    case 'm':
		map = optarg;
		break;
	    case 'n':
		num_clients = atoi (optarg);
		break;
	    case 's':
		run_server = 1;
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

    setup_allegro (w, h, d);

    game_init ();

    if (run_editor)
	editor ();
    else if (run_server)
	game_server (map, num_clients);
    else
	game_client (map, addr);
    
    game_shutdown ();

    return 0;
}

END_OF_MAIN();
