/* gamesrv.c
 *
 * Peter Wang <tjaden@users.sourceforge.net>
 */


#include <allegro.h>
#include "comm.h"
#include "commmsg.h"
#include "game.h"
#include "gamesrv.h"
#include "list.h"
#include "map.h"
#include "mapfile.h"
#include "object.h"
#include "yield.h"


static comm_server_t *comm;
static map_t *map;


static void game_server_process_msg (int type, void *buf, int size)
{
    object_t *obj;

    switch (type) {

	case MSG_CS_KEY_RIGHT:
	    obj = map_find_object (map, msg_get_long (buf));
	    object_set_xv (obj, object_xv (obj) + 1.4);
	    break;
	    
	case MSG_CS_KEY_LEFT:
	    obj = map_find_object (map, msg_get_long (buf));
	    object_set_xv (obj, object_xv (obj) - 1.4);
	    break;
	    
	case MSG_CS_KEY_UP:
	    obj = map_find_object (map, msg_get_long (buf));
	    if (object_get_number (obj, "jump") > 0) {
		object_set_yv (obj, object_yv (obj) - MIN (8, 20 / object_get_number (obj, "jump")));
		if (object_get_number (obj, "jump") < 10)
		    object_set_number (obj, "jump", object_get_number (obj, "jump") + 1);
		else	    
		    object_set_number (obj, "jump", 0);
	    }
	    else if ((object_get_number (obj, "jump") == 0) && (object_yv (obj) == 0) &&
		     (object_supported_at (obj, map, object_x (obj), object_y (obj)))) {
		object_set_yv (obj, object_yv (obj) - 4);
		object_set_number (obj, "jump", 1);
	    }
	    /*}
	     else {
	     object_set_number (obj, "jump", 0);*/
	    break;
    }
}


static void move_object_x (object_t *obj)
{
    if (object_move_x_with_ramp (obj, ((object_xv (obj) < 0) ? 3 : 4), map,
				 object_xv (obj), object_ramp (obj)) < 0)
	object_set_xv (obj, 0);

    object_set_xv (obj, object_xv (obj) * 0.75);
    if (ABS (object_xv (obj)) < 0.25)
	object_set_xv (obj, 0);
}


static void move_object_y (object_t *obj)
{
    object_set_yv (obj, object_yv (obj) + object_mass (obj));

    if (object_move (obj, ((object_yv (obj) < 0) ? 1 : 2), map, 0, object_yv (obj)) < 0) {
	object_set_yv (obj, 0);
	object_set_number (obj, "jump", 0);
    }

    object_set_yv (obj, object_yv (obj) * 0.75);
    if (ABS (object_xv (obj)) < 0.25)
	object_set_xv (obj, 0);
}


static void game_server_do_physics ()
{
    struct list_head *list;
    object_t *obj;

    list = map_object_list (map);
    list_for_each (obj, list) {
	/* NETWORK */
	float oldx = object_x (obj);
	float oldy = object_y (obj);
	/* END */

	move_object_x (obj);
	move_object_y (obj);
	
	/* NETWORK */
	if (oldx != object_x (obj) || oldy != object_y (obj)) {
	    char buf[12];
	    msg_put_long (buf, object_id (obj));
	    msg_put_float (buf+4, object_x (obj));
	    msg_put_float (buf+8, object_y (obj));
	    /* msg_encode (buf, "lff", object_id (obj), object_x (obj), object_y (obj)) */
	    comm_server_broadcast (comm, MSG_SC_MOVE_OBJECT, buf, sizeof buf);
	}
	/* END */
    }
}


static void game_server_loop ()
{
    while (!key[KEY_Q]) {
	comm_server_reset_input (comm);
	while (!comm_server_poll_input (comm, game_server_process_msg))
	    yield ();
	game_server_do_physics ();
	comm_server_broadcast_tick (comm);
    }
}


static int game_server_init (const char *mapfile)
{
    /* This is temporary until a menu system is in place and some
       proper networking.  */

    map = map_load (mapfile, 1, NULL);
    if (!map)
	return -1;

    {
	struct list_head *list;
	start_t *start;
	object_t *obj;

	list = map_start_list (map);
	list_for_each (start, list) {
	    obj = object_create ("player");
	    object_set_xy (obj, map_start_x (start), map_start_y (start));
	    map_link_object_bottom (map, obj);
	    break;
	}
    }

    return 0;
}


static void game_server_shutdown ()
{
    map_destroy (map);
}


int game_server (const char *mapfile, int num_clients)
{
    comm = comm_server_init ();
    if (!comm)
	return -1;

    while (1) {
	if (comm_server_check_new_clients (comm) &&
	    (comm_server_count_clients (comm) == num_clients))
	    goto start;
	if (key[KEY_Q]) 
	    goto end;
	yield ();
    }
    
  start:
    
    printf ("[server starting]\n");

    if (game_server_init (mapfile) == 0) {
	game_server_loop ();
	game_server_shutdown ();
    }

  end:
    
    comm_server_shutdown (comm);

    return 0;
}
