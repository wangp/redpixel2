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
#include "gameinit.h"
#include "getoptc.h"
#include "messages.h"
#include "server.h"
#include "sync.h"
#include "textface.h"


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


static void setup_allegro (int w, int h, int d)
{
    allegro_init ();
    install_timer ();
    install_keyboard ();

    if (install_mouse () < 0) {
        allegro_message ("Error initialising mouse.\n");
	exit (1);
    }

    install_sound (DIGI_AUTODETECT, MIDI_NONE, 0);

    set_window_title ("Red Pixel II");

    if (setup_video (w, h, d) < 0) {
        allegro_message ("Error setting video mode.\n%s\n", allegro_error);
	exit (1);
    }
    
    if (set_display_switch_mode (SWITCH_BACKAMNESIA) < 0)
	set_display_switch_mode (SWITCH_BACKGROUND);
}


static void setup_minimal_allegro (void)
{
    install_allegro (SYSTEM_NONE, &errno, atexit);

    set_color_conversion (COLORCONV_NONE);
    set_color_depth (16);
}


static void *server_thread (void *arg)
{
    server_run ();
    return NULL;
}


static void do_run_parallel (const char *name)
{
    messages_init ();

    if ((server_init (NULL, NET_DRIVER_LOCAL) < 0) ||
	(client_init (name, NET_DRIVER_LOCAL, "0") < 0)) {
	allegro_message (
	    "Error initialising game server or client.  Perhaps another\n"
	    "game server is already running on the same port?\n");
    } else {
	server_enable_single_hack ();

	sync_init (server_thread);
	client_run (0);
	sync_shutdown ();

	client_shutdown ();
	server_shutdown ();

	allegro_errno = &errno;	/* errno is thread-specific */
    }

    messages_shutdown ();
}


static void do_run_client_server (const char *name)
{
    messages_init ();
	
    /* XXX should make server support multiple network types
       then use NET_DRIVER_LOCAL for the client */
    if ((server_init (client_server_interface, INET_DRIVER) < 0) ||
	(client_init (name, INET_DRIVER, "127.0.0.1") < 0)) {
	allegro_message (
	    "Error initialising game server or client.  Perhaps another\n"
	    "game server is already running on the same port?\n");
    } else {
	sync_init (server_thread);
	client_run (1);
	sync_server_stop_requested ();
	sync_shutdown ();

	client_shutdown ();
	server_shutdown ();

	allegro_errno = &errno;	/* errno is thread-specific */
    }

    messages_shutdown ();
}


static void do_run_server (void)
{
    if (server_init (server_text_interface, INET_DRIVER) < 0) {
	allegro_message ("Error initialising game server.  Perhaps another\n"
			 "game server is already running on the same port?\n");
    } else {
	sync_init (NULL);
	server_run ();
	sync_shutdown ();
	server_shutdown ();
    }
}


static void do_run_client (const char *name, const char *addr)
{
    messages_init ();
    if (client_init (name, INET_DRIVER, addr) == 0) {
	sync_init (NULL);
	client_run (0);
	sync_shutdown ();
	client_shutdown ();
    }
    messages_shutdown ();
}


int main (int argc, char *argv[])
{
    int w = 320, h = 200, d = -1;
    int run_server = 0;
    int run_client_server = 0;
    int run_parallel = 0;
    int run_editor = 0;
    const char *name = "noname";
    const char *addr = "127.0.0.1";
    int c;
    
    opterr = 0;
    
    while ((c = getopt (argc, argv, ":scpa:n:ew:h:d:")) != -1) {
	switch (c) {
	    case 's':
		run_server = 1;
		break;
	    case 'c':
		run_client_server = 1;
		break;
	    case 'p':
		run_parallel = 1;
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

    if ((run_server + run_client_server + run_parallel + run_editor) > 1) {
	fprintf (stderr, "Incompatible operation modes.\n");
	return 1;
    }

    if (run_server)
	setup_minimal_allegro ();
    else
	setup_allegro (w, h, d);

    game_init ();

    if (run_editor)
	editor ();
    else if (run_parallel)
	do_run_parallel (name);
    else if (run_client_server)
	do_run_client_server (name);
    else if (run_server)
	do_run_server ();
    else
	do_run_client (name, addr);
    
    game_shutdown ();

    return 0;
}

END_OF_MAIN();
