/* game.c
 *
 * Peter Wang <tjaden@users.sourceforge.net>
 */


#include <allegro.h>
#include <libnet.h>
#include "game.h"
#include "gameloop.h"
#include "gamenet.h"
#include "map.h"
#include "mapfile.h"
#include "net.h"
#include "yield.h"


int server, client;


int game ()
{
#if 0
/****************************************************/    

    printf ("Do you want to be server?\n");

    if ((readkey () >> 8) == KEY_Y) {
	server = 1;
	printf ("Yes\n");
    }
    else {
	client = 1;
	printf ("No\n");
    }

/****************************************************/
#else
    server = client = 1;
#endif

    net_main_init ();
    
    if (server && net_server_init (gamenet_server_process_message) < 0)
	return 1;
//    if (client && net_client_init (NET_DRIVER_SOCKETS, "127.0.0.1", gamenet_client_process_message) < 0)
    if (client && net_client_init (NET_DRIVER_LOCAL, "0", gamenet_client_process_message) < 0)
	return 1;

    while (1) {
	int end = 1;

	if (server) {
	    net_server_poll ();
	    if (!net_server_num_clients) end = 0;
	}

	if (client) {
	    net_client_poll ();
	    if (!net_client_connected ()) end = 0;
	}

	if (end) break;
    }

    map = map_load ("test.pit", server);
    game_loop ();
    map_destroy (map);
    
    if (server) net_server_ask_quit ();
    if (client) net_client_disconnect ();

    while ((server && net_server_poll () != -1) |
	   (client && net_client_poll () != -1))
	yield ();

    if (client) net_client_shutdown ();
    if (server) net_server_shutdown ();
    net_main_shutdown ();
	
    return 0;
}
