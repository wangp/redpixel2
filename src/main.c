/* main.c
 * 
 * Peter Wang <tjaden@users.sourceforge.net>
 */


#include <stdio.h>
#include <allegro.h>
#include "editor.h"
#include "gameinit.h"
#include "gameclt.h"
#include "gamesrv.h"
#include "textface.h"


/* XXX remove this when glibc is properly upgraded */
int atexit(void (*fnc)(void))
{
    return 0;
}


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
    
    if (set_display_switch_mode (SWITCH_BACKAMNESIA) < 0)
	set_display_switch_mode (SWITCH_BACKGROUND);
}


static void setup_minimal_allegro ()
{
    install_allegro (SYSTEM_NONE, &errno, atexit);

    set_color_conversion (COLORCONV_NONE);
    set_color_depth (16);
}


int main (int argc, char *argv[])
{
    int w = 320, h = 200, d = -1;
    int run_editor = 0;
    int run_server = 0;
    const char *name = "noname";
    const char *addr = "127.0.0.1";
    int c;
    
    opterr = 0;
    
    while ((c = getopt (argc, argv, ":sa:n:ew:h:d:")) != -1) {
	switch (c) {
	    case 's':
		run_server = 1;
		break;
	    case 'a':
		addr = optarg;
		break;
	    case 'n':
		name = optarg;
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

    if (run_server)
	setup_minimal_allegro ();
    else
	setup_allegro (w, h, d);

    game_init ();

    if (run_editor) {
	editor ();
    }
    else if (run_server) {
	if (game_server_init (&game_server_text_interface) < 0) {
	    allegro_message ("Error initialising game server.  Perhaps another\n"
			     "game server is already running on the same port?\n");
	} else {
	    game_server_run ();
	    game_server_shutdown ();
	}
    }
    else {
	if (game_client_init (name, addr) == 0) {
	    game_client_run ();
	    game_client_shutdown ();
	}
    }
    
    game_shutdown ();

    return 0;
}

END_OF_MAIN();
