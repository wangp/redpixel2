/* gamesrv.c
 *
 * Peter Wang <tjaden@users.sourceforge.net>
 */


#include <allegro.h>
#include <libnet.h>
#include "alloc.h"
#include "error.h"
#include "gamesrv.h"
#include "list.h"
#include "map.h"
#include "mapfile.h"
#include "netmsg.h"
#include "object.h"
#include "objtypes.h"
#include "packet.h"
#include "yield.h"


#if 1
# define dbg(msg)	puts ("[server] " msg)
#else
# define dbg(msg)
#endif


/*------------------------------------------------------------*/


static NET_CONN *listen;
static struct list_head clients;

static map_t *map;
static const char *map_filename = "test.pit"; /* XXX */


/*------------------------------------------------------------*/


typedef struct clt clt_t;

struct clt {
    clt_t *next;
    clt_t *prev;
    NET_CONN *conn;
    int id;
    int flags;
    int controls;
};


static int clt_next_id;


#define CLT_FLAG_READY		0x01
#define CLT_FLAG_DELETED	0x02

#define clt_is_ready(c)		((c)->flags & CLT_FLAG_READY)
#define clt_set_ready(c)	((c)->flags |= CLT_FLAG_READY)
#define clt_clear_ready(c)	((c)->flags &=~ CLT_FLAG_READY)

#define clt_is_deleted(c)	((c)->flags & CLT_FLAG_DELETED)
#define clt_set_deleted(c)	((c)->flags |= CLT_FLAG_DELETED)

#define clt_is_alive(c)		(!clt_is_deleted (c))


#define clt_object_id(c)	(- (c)->id)


static clt_t *clt_create (NET_CONN *conn)
{
    clt_t *c = alloc (sizeof *c);
    c->conn = conn;
    c->id = clt_next_id++;
    return c;
}


static void clt_destroy (clt_t *c)
{
    if (c) {
	net_closeconn (c->conn);
	free (c);
    }
}


/*------------------------------------------------------------*/


static void send_post_join_info (clt_t *c)
{
    char buf[5];

    packet_encode (buf, "cl", MSG_SC_POST_JOIN, c->id);
    net_send_rdm (c->conn, buf, sizeof buf);
}


static clt_t *check_new_connection (int auto_add)
{
    NET_CONN *conn;
    clt_t *c;

    if (!(conn = net_poll_listen (listen)))
	return NULL;

    c = clt_create (conn);
    if (auto_add)
	list_add (clients, c);

    send_post_join_info (c);

    return c;
}


/*------------------------------------------------------------*/


/*static int count_clients ()
{
    clt_t *c;
    int n = 0;

    list_for_each (c, &clients)
	if (clt_is_alive (c)) n++;

    return n;
}*/


static void clear_clients_readiness ()
{
    clt_t *c;

    list_for_each (c, &clients)
	clt_clear_ready (c);
}


static int all_clients_ready ()
{
    clt_t *c;

    list_for_each (c, &clients)
	if (clt_is_alive (c) && !clt_is_ready (c))
	    return 0;

    return 1;
}


static void clear_clients_controls ()
{
    clt_t *c;

    list_for_each (c, &clients)
	c->controls = 0;
}


/*------------------------------------------------------------*/


static void broadcast_rdm (const void *buf, int size)
{
    clt_t *c;

    list_for_each (c, &clients)
	if (clt_is_alive (c))
	    net_send_rdm (c->conn, buf, size);
}


static void broadcast_rdm_byte (unsigned char c)
{
    broadcast_rdm (&c, 1);
}


/*------------------------------------------------------------*/


static void destroy_deleted_clients ()
{
    clt_t *c, *next;

    for (c = clients.next; list_neq (c, &clients); c = next) {
	next = list_next (c);
	if (clt_is_deleted (c)) {
	    list_remove (c);
	    clt_destroy (c);
	}
    }
}


/*------------------------------------------------------------*/


static void feed_game_state (NET_CONN *conn)
{
    char buf[NET_MAX_PACKET_SIZE] = { MSG_SC_GAMEINFO };
    char *p = buf+1;

    /* map */
    p += packet_encode (p, "cs", MSG_SC_GAMEINFO_MAPLOAD, map_filename);

    /* objects */
    {
	struct list_head *list;
	object_t *obj;
	const char *typename;

	list = map_object_list (map);
	list_for_each (obj, list) {
	    typename = objtype_name (object_type (obj));

	    /* check for packet overflow */
	    /* XXX this is not nice */ 
	    if ((p - buf) + 16 + strlen (typename) > sizeof buf) {
		net_send_rdm (conn, buf, p - buf);
		p = buf+1;
    	    }

	    p += packet_encode (p, "cslff", MSG_SC_GAMEINFO_OBJECTCREATE,
				typename, object_id (obj), object_x (obj),
				object_y (obj));
	}
    }

    if (p - buf > 1)
	net_send_rdm (conn, buf, p - buf);

    /* end marker */
    dbg ("feed game state done");
    buf[0] = MSG_SC_GAMEINFO_DONE;
    net_send_rdm (conn, buf, 1);

    /* Note: after the client receives the game state, it will
     * automatically switch to paused mode.  */
}


/*------------------------------------------------------------*/


static void process_cs_gameinfo_packet (clt_t *c, const unsigned char *buf, int size)
{
    const void *end = buf + size;

    while (buf != end) {
	switch (*buf++) {
	    case MSG_CS_GAMEINFO_CONTROLS:
		c->controls = *buf++;
		break;

	    default:
		error ("error: unknown code in gameinfo packet (server)\n");
	}
    }
}


/*------------------------------------------------------------*/


static void handle_client_controls ()
{
    clt_t *c;
    object_t *obj;

    dbg ("handle client controls");

    list_for_each (c, &clients) {
	if (!clt_is_alive (c))
	    continue;
	
	obj = map_find_object (map, clt_object_id (c));
	if (!obj)
	    error ("error: server missing object\n");
	
	/* left */
	if (c->controls & 0x01)
	    object_set_xv (obj, object_xv (obj) - 1.4);

	/* right */
	if (c->controls & 0x02)
	    object_set_xv (obj, object_xv (obj) + 1.4);
	    
	/* up */
	if (c->controls & 0x04) {
	    float jump = object_get_number (obj, "jump");

	    if (jump > 0) {
		object_set_yv (obj, object_yv (obj) - MIN (8, 20 / jump));
		object_set_number (obj, "jump", (jump < 10) ? (jump + 1) : 0);
	    }
	    else if ((jump == 0) && (object_yv (obj) == 0) && (object_supported (obj, map))) {
		object_set_yv (obj, object_yv (obj) - 4);
		object_set_number (obj, "jump", 1);
	    }
	}
	else {
	    object_set_number (obj, "jump", 0);
	}
    }
}


static void move_object_x (object_t *obj)
{
    float xv = object_xv (obj);

    if (object_move_x_with_ramp (obj, ((xv < 0) ? 3 : 4), map, xv, object_ramp (obj)) < 0)
	object_set_xv (obj, 0);
    else {
	xv *= 0.75;
	object_set_xv (obj, (ABS (xv) < 0.25) ? 0 : xv);
    }
}


static void move_object_y (object_t *obj)
{
    float yv;

    yv = object_yv (obj) + object_mass (obj);
    object_set_yv (obj, yv);

    if (object_move (obj, ((yv < 0) ? 1 : 2), map, 0, yv) < 0) {
	object_set_yv (obj, 0);
	object_set_number (obj, "jump", 0);
    }
    else {
	yv *= 0.75;
	object_set_yv (obj, ((ABS (yv) < 0.25) ? 0 : yv));
    }
}


/* XXX also mixed in with send gameinfo stuff right now */
static void handle_physics ()
{
    list_head_t *list;
    object_t *obj;
    char buf[NET_MAX_PACKET_SIZE] = { MSG_SC_GAMEINFO };
    char *p = buf+1;

    handle_client_controls ();

    list = map_object_list (map);
    list_for_each (obj, list) {
	float oldx = object_x (obj);
	float oldy = object_y (obj);

	move_object_x (obj);
	move_object_y (obj);

	if ((oldx != object_x (obj)) || (oldy != object_y (obj)))
	    p += packet_encode (p, "clff", MSG_SC_GAMEINFO_OBJECTMOVE,
				object_id (obj), object_x (obj), object_y (obj));
    }

    if ((p - buf) > 1)
	broadcast_rdm (buf, p - buf);
}


/*------------------------------------------------------------*/


void game_server ()
{
  lobby:

    dbg ("lobby");
    {
	while (1) {
	    check_new_connection (1);
	    destroy_deleted_clients ();

	    if (key[KEY_S])
		goto create_initial_game_state;

	    if (key[KEY_X])
		goto end;

	    yield ();
	}
    }

  create_initial_game_state:

    dbg ("create initial game state");
    {
	start_t *start;
	object_t *obj;
	clt_t *c;

	map = map_load (map_filename, 1, NULL);

	start = map_start_list (map)->next;

	list_for_each (c, &clients) {
	    if (!clt_is_alive (c))
		continue;

	    obj = object_create_ex ("player", clt_object_id (c));
	    object_set_xy (obj, map_start_x (start), map_start_y (start));
	    map_link_object_bottom (map, obj);

	    start = list_next (start);
	    if (list_eq (start, map_start_list (map)))
		start = list_next (start);
	}
    }

    dbg ("feed game state to clients");
    {
	unsigned char ch;
	clt_t *c;

	broadcast_rdm_byte (MSG_SC_GAMESTATEFEED_REQ);

	clear_clients_readiness ();

	while (!all_clients_ready ()) {
	    list_for_each (c, &clients) {
		if ((!clt_is_alive (c)) || (clt_is_ready (c)))
		    continue;

		if (net_receive_rdm (c->conn, &ch, 1) <= 0)
		    continue;

		if (ch == MSG_CS_GAMESTATEFEED_ACK) {
		    feed_game_state (c->conn);
		    clt_set_ready (c);
		}
	    }

	    yield ();
	}

	broadcast_rdm_byte (MSG_SC_RESUME);
    }
    
    dbg ("game");
    {
	while (1) {
	    if (key[KEY_Q]) {
	      go_lobby:
		/* XXX free game state properly */
		map_destroy (map);
		map = NULL;
		goto lobby;
	    }

	    destroy_deleted_clients ();

	    dbg ("check new connection");
	    {
		clt_t *c;
		object_t *obj;
		char buf[NET_MAX_PACKET_SIZE];
		int size;

		if ((c = check_new_connection (0))) {				

		    /* Joining mid-game.  */

		    /* tell other clients to pause */
		    broadcast_rdm_byte (MSG_SC_PAUSE);

		    /* tell new client we're going to send the game state */
		    buf[0] = MSG_SC_GAMESTATEFEED_REQ;
		    net_send_rdm (c->conn, buf, 1);

		    /* wait for ack */
		    while (1) {
			yield ();
			if (net_receive_rdm (c->conn, buf, sizeof buf) <= 0)
			    continue;
			if (buf[0] == MSG_CS_GAMESTATEFEED_ACK)
			    break;
		    }

		    /* feed game state to the new client */
		    feed_game_state (c->conn);

		    /* add new client to list */
		    list_add (clients, c);

		    /* create a new player object for new client */
		    {
			start_t *start = map_start_list (map)->next;

			obj = object_create_ex ("player", clt_object_id (c));
			object_set_xy (obj, map_start_x (start), map_start_y (start));
			map_link_object_bottom (map, obj);
		    }

		    /* resume the game */
		    broadcast_rdm_byte (MSG_SC_RESUME);

		    /* tell all clients to create the new player object */
		    size = packet_encode (buf, "ccslff", MSG_SC_GAMEINFO,
					  MSG_SC_GAMEINFO_OBJECTCREATE,
					  objtype_name (object_type (obj)),
					  object_id (obj), object_x (obj),
					  object_y (obj));
		    broadcast_rdm (buf, size);
		}
	    }
	    
	    dbg ("receive input");
	    {
		char buf[NET_MAX_PACKET_SIZE];
		int size;
		clt_t *c;

		clear_clients_controls ();
		clear_clients_readiness ();

		while (!all_clients_ready ()) {
		    if (key[KEY_Q])
			goto go_lobby;

		    list_for_each (c, &clients) {
			if (!clt_is_alive (c) || clt_is_ready (c))
			    continue;

			size = net_receive_rdm (c->conn, buf, sizeof buf);
			if (size <= 0)
			    continue;

			switch (buf[0]) {
			    case MSG_CS_GAMEINFO:
				process_cs_gameinfo_packet (c, buf+1, size-1);
				break;

			    case MSG_CS_GAMEINFO_DONE:
				clt_set_ready (c);
				break;

			    case MSG_CS_DISCONNECT_ASK: {
				object_t *obj;
				
				buf[0] = MSG_SC_DISCONNECTED;
				net_send_rdm (c->conn, buf, 1);
				clt_set_deleted (c);

				if ((obj = map_find_object (map, clt_object_id (c)))) {
				    map_unlink_object (obj);
				    object_destroy (obj);

				    packet_encode (buf, "ccl", MSG_SC_GAMEINFO,
						   MSG_SC_GAMEINFO_OBJECTDESTROY,
						   clt_object_id (c));
				    broadcast_rdm (buf, 6);
				}
			    } break;
			}
		    }

		    yield ();
		}
	    }
	    
	    dbg ("handle physics");
	    handle_physics ();
	    broadcast_rdm_byte (MSG_SC_GAMEINFO_DONE);
	}
    }

  end:

    dbg ("end");
    broadcast_rdm_byte (MSG_SC_DISCONNECTED);
}


/*------------------------------------------------------------*/


int game_server_init ()
{
    if (!(listen = net_openconn (NET_DRIVER_SOCKETS, "")))
	return -1;
    net_listen (listen);
    list_init (clients);
    clt_next_id = 1;  /* not zero! (see clt_object_id) */
    return 0;
}


void game_server_shutdown ()
{
    list_free (clients, clt_destroy);
    net_closeconn (listen);
}



/*	Let's all go to the lobby,
	Let's all go to the lobby,
	Let's all go to the lobbiie,
	Get ourselves some snacks.	*/

