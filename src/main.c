/* main.c
 * 
 * Peter Wang <tjaden@users.sourceforge.net>
 */


#include <stdio.h>
#include <allegro.h>
#include "libnet.h"
#include "client.h"
#include "clsvface.h"
#include "editor.h"
#include "error.h"
#include "fe-main.h"
#include "fe-options.h"
#include "gameinit.h"
#include "getoptc.h"
#include "messages.h"
#include "music.h"
#include "screen.h"
#include "server.h"
#include "sync.h"
#include "textface.h"
#include "timeval.h"


/* XXX */
#ifdef TARGET_WINDOWS
# define INET_DRIVER	NET_DRIVER_WSOCK_WIN
#else
# define INET_DRIVER	NET_DRIVER_SOCKETS
#endif


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


static void setup_allegro (int w, int h, int d, int stretch_method)
{
    if (allegro_init () != 0) {
        puts ("Error initialising Allegro.");
	exit (1);
    }
    install_timer ();
    install_keyboard ();

    if (install_mouse () < 0)
        error ("Error initialising mouse.\n");

#ifdef TARGET_WINDOWS
    /* Hack: The default DirectX mixer driver used by Allegro/Windows works
     * very badly with our background music thread (stutters and clicks and
     * pops), so we force the software mixer.
     */
    set_config_id ("sound", "digi_card", DIGI_DIRECTAMX (0));
#endif

    install_sound (DIGI_AUTODETECT, MIDI_NONE, 0);

    set_window_title ("Red Pixel II");

    load_config (&stretch_method);

    if (w < 0) w = desired_menu_screen_w;
    if (h < 0) h = desired_menu_screen_h;
    if (d < 0) d = 16;

    if (setup_video (w, h, d) < 0)
        errorv ("Error setting video mode.\n%s\n", allegro_error);

    screen_blitter_init (stretch_method, d);
    
    if (set_display_switch_mode (SWITCH_BACKAMNESIA) < 0)
	set_display_switch_mode (SWITCH_BACKGROUND);
}


static void unsetup_allegro (void)
{
    screen_blitter_shutdown ();
    save_config ();
}


static void setup_minimal_allegro (void)
{
    if (install_allegro (SYSTEM_NONE, &errno, atexit) != 0) {
        allegro_message ("Error initialising Allegro.\n");
	exit (1);
    }

    set_color_conversion (COLORCONV_NONE);
    set_color_depth (16);
}


static void *server_thread (void *arg)
{
    server_run ();
    return NULL;
}


static void do_run_server (void)
{
    if (server_init (server_text_interface, INET_DRIVER, "") < 0) {
	error ("Error initialising game server.  Perhaps another\n"
	       "game server is already running on the same port?\n");
    } else {
	sync_init (NULL);
	server_run ();
	sync_shutdown ();
	server_shutdown ();
    }
}


int main (int argc, char *argv[])
{
    int w = -1, h = -1, d = -1;
    int stretch_method = STRETCH_METHOD_NONE;
    int run_server = 0;
    int run_editor = 0;
    int c;
    
    opterr = 0;
    
    while ((c = getopt (argc, argv, ":sew:h:d:")) != -1) {
	switch (c) {
	    case 's':
		run_server = 1;
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

    if ((run_server + run_editor) > 1) {
	fprintf (stderr, "Incompatible operation modes.\n");
	return 1;
    }

    if (run_server)
	setup_minimal_allegro ();
    else
	setup_allegro (w, h, d, stretch_method);

    music_init();

    game_init ();

    if (run_server) {
	do_run_server ();
    }
    else if (run_editor) {
	if (editor_init () == 0) {
	    editor_run ();
	    editor_shutdown ();
	}
    }
    else {
	if (gamemenu_init () == 0) {
	    gamemenu_run ();
	    gamemenu_shutdown ();
	}
    }

    game_shutdown ();

    music_shutdown ();

    if (!run_server)
	unsetup_allegro ();

    return 0;
}

END_OF_MAIN()
