/* hub.c - central launch point 
 *
 * Peter Wang <tjaden@psynet.net>
 */


#include <allegro.h>
#include "console.h"
#include "editor.h"
#include "gameloop.h"
#include "gamenet.h"
#include "hub.h"
#include "lobby.h"
#include "magic4x4.h"
#include "map.h"
#include "mapfile.h"
#include "net.h"
#include "vars.h"
#include "yield.h"


#define MODE_NONE		0
#define MODE_LOBBY		1
#define MODE_GAME_START		2
#define MODE_GAME_LOOP		3


static BITMAP *dbuf;
static int mode;
static int quit;


#define con	console_printf


/* Start a game server.  */
static void __export__start_server ()
{
    if (server || client) {
	con ("Server or client already running");
	return;
    }

    if (mode != MODE_NONE) {
	con ("Quit whatever you are doing first.");
	return;
    }

    if (net_server_init (gamenet_server_process_message) < 0)
	con ("Error starting server");
    else {
	server = 1;
	con ("Server started");

	lobby_init ();		/* XXX: check error */
	mode = MODE_LOBBY;	    
    }
}


/* Stop game server.  */
static void __export__stop_server ()
{
    if (server) {
	net_server_ask_quit ();
	con ("Dropping clients");
    }
    else
    	con ("Server not running");
}


/* Check the number of clients connected to the server.  */
static void __export__num_clients ()
{
    if (!server)
	con ("Server not started");
    else
	con ("%d clients connected", net_server_num_clients);
}


/* Select a new map on server.  */
static void __export__map ()
    /* (map) : (none) */
{
    lua_Object obj;
    const char *filename;

    if (!server) {
	con ("You must be server to select a map");
	return;
    }

    obj = lua_getparam (1);
    if (!lua_isstring (obj)) {
	con ("Requires a map filename");
	return;
    }
    else
	filename = lua_getstring (obj);

    if (map) 
	map_destroy (map);

    map = map_load (filename, 1);
    if (!map) {
	con ("Error loading map");
	return;
    }

    ustrncpy (map_filename, filename, sizeof map_filename);    
}


/* Issue start game signal on server.  */
static void __export__start_game ()
{
    if (!server)
	con ("You must be server to start a game");
    else if (!net_server_num_clients)
	con ("You don't have any clients");
    else if (mode != MODE_LOBBY) 
	con ("You have to be in the lobby");
    else {
	lobby_shutdown ();
	mode = MODE_GAME_START;
    }
}


/* Connect as client to a game server.  */
static void __export__connect ()
    /* (server) : (none) */
{
    lua_Object obj;
    int drv;
    const char *target;
    
    if (client) {
	con ("Client already started");
	return;
    }

    obj = lua_getglobal ("drv");
    if (!lua_isnumber (obj)) {
	con ("Select driver type with `drv' variable");
	return;
    }
    else
	drv = lua_getnumber (obj);

    obj = lua_getparam (1);
    if (!lua_isstring (obj)) {
	con ("Requires server address");
	return;
    }
    else
	target = lua_getstring (obj);

    if (net_client_init (drv, target, gamenet_client_process_message) < 0)
	con ("Error initialising connection");
    else {
	con ("Connecting");
	client = 1;
    }
}


/* Disconnect from game server.  */
static void __export__disconnect ()
{
    if (client) {
	net_client_disconnect ();
	con ("Disconnecting");
    }
    else
	con ("Not connected");
}


/* Start editor.  */
static void __export__editor ()
{
    if (mode != MODE_NONE) {
	con ("You are already doing something else");
	return;
    }
	
    if (editor () < 0)
	con ("Error starting editor");
}


/* Print out current mode.  */
static void __export__mode ()
{
    switch (mode) {
	case MODE_NONE: con ("None"); break;
	case MODE_LOBBY: con ("Lobby"); break;
	case MODE_GAME_LOOP: con ("Game loop"); break;
	default: con ("Unknown"); break;
    }
}


/* Print out some help.  */
static void __export__help ()
{
    con ("(not yet)");
}


/* Quit.  */
static void __export__quit ()
{
    if (client)
	__export__disconnect ();
    if (server)
	__export__stop_server ();

    quit = 1;
    con ("Quitting");
}


static void export_functions ()
{
#define e(func)	(lua_register (#func, __export__##func))

    e (start_server);
    e (stop_server);
    e (num_clients);
    e (map);
    e (start_game);
    /* stop game */
    e (connect);
    e (disconnect);
    e (editor);
    e (mode);
    e (help);
    e (quit);

#undef e
}


void hub ()
{
    while (!quit || (quit && (server || client))) {
	if (server)
	    if (net_server_poll () < 0) {
		net_server_shutdown ();
		server = 0;
		console_printf ("Server stopped");

		if (mode == MODE_LOBBY)
		    lobby_shutdown ();
		else if (mode == MODE_GAME_LOOP) 
		    game_loop_shutdown ();
		mode = MODE_NONE;
	    }

	if (client) {
	    int x = net_client_connected ();
	    
	    if (net_client_poll () < 0) {
		net_client_shutdown ();
		client = 0;
		console_printf ("Client stopped");
		
		if (mode == MODE_LOBBY)
		    lobby_shutdown ();
		else if (mode == MODE_GAME_LOOP) 
		    game_loop_shutdown ();
		mode = MODE_NONE;
	    }
	    else if (!x && net_client_connected ()) {
		console_printf ("Client connected");
		lobby_init ();
		mode = MODE_LOBBY;
	    }
	}

	console_update ();

	clear (dbuf);

	switch (mode) {
	    case MODE_NONE:
		textout_centre (dbuf, font, "Red Pixel II",
				dbuf->w / 6, dbuf->h / 2,
				makecol24 (0xff, 0xaa, 0xaa));
		break;

	    case MODE_LOBBY: {
		int x = lobby_update (dbuf);
		
		if (x < 0) {
		    lobby_shutdown ();
		    mode = MODE_NONE;
		}
		else if (x > 0) {
		    lobby_shutdown ();
		    mode = MODE_GAME_START;
		}

		break;
	    }

	    case MODE_GAME_START: {
		if (server) {
		    gamenet_server_send_game_state ();
		    gamenet_server_send_game_start ();
		}

		game_loop_init ();
		mode = MODE_GAME_LOOP;
		break;
	    }

	    case MODE_GAME_LOOP:
		if (game_loop_update (dbuf) < 0) {
		    game_loop_shutdown ();
		    mode = MODE_NONE;
		}
		break;
	}

	console_draw (dbuf);
	blit_magic_format (dbuf, screen, SCREEN_W, SCREEN_H);

	yield ();
    }
}


int hub_init ()
{
    mode = MODE_NONE;
    quit = 0;

    dbuf = create_magic_bitmap (SCREEN_W, SCREEN_H);
    if (!dbuf)
	return -1;

    if (console_init () < 0)
	return -1;
    export_functions ();

    net_main_init ();

    return 0;
}


void hub_shutdown ()
{
    net_main_shutdown ();
    console_shutdown ();
    destroy_bitmap (dbuf);
}
