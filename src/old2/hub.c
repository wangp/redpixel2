/* hub.c - central launch point 
 *
 * Peter Wang <tjaden@psynet.net>
 */


#include <allegro.h>
#include <lua.h>
#include "console.h"
#include "editor.h"
#include "gameloop.h"
#include "gamenet.h"
#include "hub.h"
#include "lobby.h"
#include "luahelp.h"
#include "magic4x4.h"
#include "map.h"
#include "mapfile.h"
#include "net.h"
#include "scripts.h"
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
static int __export__start_server ()
{
    if (server || client) {
	con ("Server or client already running");
	return 0;
    }

    if (mode != MODE_NONE) {
	con ("Quit whatever you are doing first.");
	return 0;
    }

    if (net_server_init (gamenet_server_process_message) < 0)
	con ("Error starting server");
    else {
	server = 1;
	con ("Server started");

	lobby_init ();		/* XXX: check error */
	mode = MODE_LOBBY;	    
    }

    return 0;
}


/* Stop game server.  */
static int __export__stop_server ()
{
    if (server) {
	net_server_ask_quit ();
	con ("Dropping clients");
    }
    else
    	con ("Server not running");
    return 0;
}


/* Check the number of clients connected to the server.  */
static int __export__num_clients ()
{
    if (!server)
	con ("Server not started");
    else
	con ("%d clients connected", net_server_num_clients);
    return 0;
}


/* Select a new map on server.  */
static int __export__map (LS)
    /* (map) : (none) */
{
    const char *filename;

    if (!server) {
	con ("You must be server to select a map");
	return 0;
    }

    if (!lua_isstring (L, 1)) {
	con ("Requires a map filename");
	return 0;
    }
    else
	filename = lua_tostring (L, 1);

    if (map) 
	map_destroy (map);

    map = map_load (filename, 1);
    if (!map) {
	con ("Error loading map");
	return 0;
    }

    ustrncpy (map_filename, filename, sizeof map_filename);
    return 0;
}


/* Issue start game signal on server.  */
static int __export__start_game ()
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
    return 0;
}


/* Connect as client to a game server.  */
static int __export__connect (LS)
    /* (server) : (none) */
{
    int drv;
    const char *target;
    
    if (client) {
	con ("Client already started");
	return 0;
    }

    lua_getglobal (L, "drv");
    if (!lua_isnumber (L, -1)) {
	con ("Select driver type with `drv' variable");
	return 0;
    }
    else
	drv = lua_tonumber (L, -1);

    if (!lua_isstring (L, 1)) {
	con ("Requires server address");
	return 0;
    }
    else
	target = lua_tostring (L, 1);

    if (net_client_init (drv, target, gamenet_client_process_message) < 0)
	con ("Error initialising connection");
    else {
	con ("Connecting");
	client = 1;
    }

    return 0;
}


/* Disconnect from game server.  */
static int __export__disconnect (LS)
{
    if (client) {
	net_client_disconnect ();
	con ("Disconnecting");
    }
    else
	con ("Not connected");
    return 0;
}


/* Start editor.  */
static int __export__editor (LS)
{
    if (mode != MODE_NONE)
	con ("You are already doing something else");
    else if (editor () < 0)
	con ("Error starting editor");
    return 0;
}


/* Print out current mode.  */
static int __export__mode (LS)
{
    switch (mode) {
	case MODE_NONE: con ("None"); break;
	case MODE_LOBBY: con ("Lobby"); break;
	case MODE_GAME_LOOP: con ("Game loop"); break;
	default: con ("Unknown"); break;
    }
    return 0;
}


/* Print out some help.  */
static int __export__help ()
{
    con ("(not yet)");
    return 0;
}


/* Quit.  */
static int __export__quit (LS)
{
    if (client)
	__export__disconnect (L);
    if (server)
	__export__stop_server (L);

    quit = 1;
    con ("Quitting");

    return 0;
}


static void export_functions (LS)
{
#define e(func)	(lua_register (L, #func, __export__##func))

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
    while ((!quit) || ((quit) && (server || client))) {
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
    export_functions (lua_state);

    net_main_init ();

    return 0;
}


void hub_shutdown ()
{
    net_main_shutdown ();
    console_shutdown ();
    destroy_bitmap (dbuf);
}
