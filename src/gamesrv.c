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
#include "timeout.h"
#include "yield.h"


static NET_CONN *listen;

static game_server_interface_t *interface;

static struct list_head clients;

static map_t *map;
static char *map_filename;


/* Client nodes.  */


typedef struct clt clt_t;

struct clt {
    clt_t *next;
    clt_t *prev;
    NET_CONN *conn;
    int id;
    int flags;
    int controls;
    timeout_t timeout;
};


#define clt_object_id(c)	(- (c)->id)


#define CLT_FLAG_STALE		0x01
#define CLT_FLAG_READY		0x02
#define CLT_FLAG_CANTIMEOUT	0x04

#define clt_test_flag(c,f)	((c)->flags & (f))
#define clt_set_flag(c,f)	((c)->flags |= (f))
#define clt_clear_flag(c,f)	((c)->flags &=~ (f))

#define clt_stale(c)		(clt_test_flag (c, CLT_FLAG_STALE))
#define clt_set_stale(c)	(clt_set_flag (c, CLT_FLAG_STALE))

#define clt_ready(c)		(clt_test_flag (c, CLT_FLAG_READY))
#define clt_set_ready(c)	(clt_set_flag (c, CLT_FLAG_READY))
#define clt_clear_ready(c)	(clt_clear_flag (c, CLT_FLAG_READY))

#define clt_cantimeout(c)	(clt_test_flag (c, CLT_FLAG_CANTIMEOUT))
#define clt_set_cantimeout(c)	(clt_set_flag (c, CLT_FLAG_CANTIMEOUT))
#define clt_clear_cantimeout(c)	(clt_clear_flag (c, CLT_FLAG_CANTIMEOUT))


static int clt_next_id;


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

static void clt_set_timeout (clt_t *c, int secs)
{
    timeout_set (&c->timeout, secs * 1000);
}


static int clt_timed_out (clt_t *c)
{
    return clt_cantimeout (c) && timeout_test (&c->timeout);
}


static int clt_send_rdm (clt_t *c, const void *buf, int size)
{
    return net_send_rdm (c->conn, buf, size);
}


static int clt_receive_rdm (clt_t *c, void *buf, int size)
{
    if (!net_query_rdm (c->conn))
	return 0;
    return net_receive_rdm (c->conn, buf, size);
}


/* Helper for logging things.  */


static void srv_log (const char *fmt, ...)
{
    char buf[1024];
    va_list ap;

    va_start (ap, fmt);
    uvszprintf (buf, sizeof buf, fmt, ap);
    va_end (ap);

    interface->add_log (NULL, buf);
}


/* Working with the client list.  */


#define for_each_client(c)		list_for_each(c, &clients)
#define for_each_fresh_client(c)	list_for_each (c, &clients) if (!clt_stale (c))


static void broadcast_rdm (const void *buf, int size)
{
    clt_t *c;

    for_each_fresh_client (c)
	clt_send_rdm (c, buf, size);
}


static void broadcast_rdm_byte (char c)
{
    broadcast_rdm (&c, 1);
}


/* Handle new connections.  */


static void send_post_join_info (clt_t *c)
{
    char buf[5];

    packet_encode (buf, "cl", MSG_SC_POST_JOIN, c->id);
    clt_send_rdm (c, buf, sizeof buf);
}


static clt_t *check_new_connection (int auto_add)
{
    NET_CONN *conn;
    clt_t *c;

    conn = net_poll_listen (listen);
    if (!conn)
	return NULL;

    c = clt_create (conn);
    if (auto_add)
	list_add (clients, c);

    send_post_join_info (c);

    return c;
}


/* Creating and freeing the game state.  */


static int create_initial_game_state ()
{
    start_t *start;
    object_t *obj;
    clt_t *c;

    map = map_load (map_filename, 1, 0);
    if (!map)
	return -1;

    start = map_start_list (map)->next;

    for_each_fresh_client (c) {
	obj = object_create_ex ("player", clt_object_id (c));
	object_set_xy (obj, map_start_x (start), map_start_y (start));
	map_link_object_bottom (map, obj);

	start = list_next (start);
	if (list_eq (start, map_start_list (map)))
	    start = list_next (start);
    }

    return 0;
}


static void destroy_game_state ()
{
    map_destroy (map);
    map = NULL;
}


/* Feeding game state.  */


static void feed_game_state (clt_t *c)
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
		clt_send_rdm (c, buf, p - buf);
		p = buf+1;
    	    }

	    p += packet_encode (p, "cslff", MSG_SC_GAMEINFO_OBJECTCREATE,
				typename, object_id (obj), object_x (obj),
				object_y (obj));
	}
    }

    if (p - buf > 1)
	clt_send_rdm (c, buf, p - buf);

    /* end marker */
    buf[0] = MSG_SC_GAMEINFO_DONE;
    clt_send_rdm (c, buf, 1);

    /* Note: after the client receives the game state, it will
     * automatically switch to paused mode.  */
}


/* Handle stale clients and objects.  */


static void destroy_stale_object (objid_t id)
{
    object_t *obj;
    char buf[6];

    if (!(obj = map_find_object (map, id)))
	return;

    map_unlink_object (obj);
    object_destroy (obj);

    /* tell clients to destroy it as well */
    packet_encode (buf, "ccl", MSG_SC_GAMEINFO,
		   MSG_SC_GAMEINFO_OBJECTDESTROY, id);
    broadcast_rdm (buf, 6);
}


static void handle_stale_clients ()
{
    clt_t *c, *next;

    for (c = clients.next; list_neq (c, &clients); c = next) {
	next = list_next (c);
	if (clt_stale (c)) {
	    list_remove (c);
	    clt_destroy (c);
	}
    }
}


/* Special types of incoming network packets.  */


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


/* Handle game physics.  */


static void handle_client_controls ()
{
    clt_t *c;
    object_t *obj;

    for_each_fresh_client (c) {
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


/* The game server.
 *
 * This is written as a state machine.  Each state returns the next
 * state the server should switch to.  Each state is also responsible
 * for calling interface->poll () if it can handle commands.
 */


typedef enum {
    STATE_LOBBY,
    STATE_GAME_STATE_FEED,
    STATE_GAME,
    STATE_END
} state_t;


static void process_command (const char *cmdline)
{
#define is(x)  (0 == ustricmp (cmd, x))

    const char *sep = " \t";
    char *cmd, *p;

    cmd = ustrdup (cmdline);
    if ((p = ustrpbrk (cmd, sep)))
	 usetc (p, 0);

    if (is ("HELP") || is ("?")) {
	srv_log ("");
        srv_log ("  MAP <filename>          - select a map");
        srv_log ("  CLIENTS                 - list clients");
        srv_log ("  KICK <client> [reason]  - kick a client");
	srv_log ("  MSG <message>           - broadcast text message to clients");
        srv_log ("  QUIT                    - quit completely");
        srv_log ("  START                   - enter game mode");
        srv_log ("  STOP                    - return to the lobby");
        srv_log ("  CONTEXT                 - show current context");
	srv_log ("");
    }
    else if is ("MAP") {
	p = ustrpbrk (cmdline, sep);
	if (!p)
	    srv_log ("MAP missing argument");
	else {
	    ugetx (&p);
	    free (map_filename);
	    map_filename = ustrdup (p);
	    srv_log ("Setting map to %s.", map_filename);
	}
    }
    else if is ("CLIENTS") {
	clt_t *c;
	int n;
	
	n = 0; list_for_each (c, &clients) n++;
	srv_log ("Number of clients: %d", n);
    }
    else if is ("KICK")
	srv_log ("KICK unsupported at this time");
    else if is ("QUIT")
	srv_log ("QUIT unsupported in this context");
    else if is ("START")
	srv_log ("START unavailable in this context");
    else if is ("STOP")
	srv_log ("STOP unavailable in this context");
    else if is ("CONTEXT")
	srv_log ("CONTEXT unavailable in this context (this is a bug :-)");
    else
	srv_log ("Unrecognised command: %s\n", cmd);
    
    free (cmd);

#undef is
}


static state_t state_lobby ()
{
    const char *cmd;

    srv_log ("Entering lobby");

    while (1) {
	yield ();

	if (check_new_connection (1))
	    srv_log ("New client connected");

	if (!(cmd = interface->poll ()))
	    continue;

	if (0 == ustrcmp (cmd, "start")) {
	    srv_log ("Creating initial game state");
	    if (create_initial_game_state () < 0) {
		srv_log ("An error occurred, aborting game start");
		continue;
	    }
	    return STATE_GAME_STATE_FEED;
	}
	else if (0 == ustrcmp (cmd, "quit"))
	    return STATE_END;
	else
	    process_command (cmd);
    }
}


static state_t state_game_state_feed ()
{
    char ch;
    clt_t *c;
    int done;

    srv_log ("Feeding game state");

    broadcast_rdm_byte (MSG_SC_GAMESTATEFEED_REQ);

    for_each_fresh_client (c) {
	clt_clear_ready (c);
	clt_set_cantimeout (c);
	clt_set_timeout (c, 10);
    }

    do {
	yield ();
	done = 1;

	for_each_fresh_client (c) {
	    if (clt_ready (c))
		continue;

	    done = 0;

	    if (clt_receive_rdm (c, &ch, 1) <= 0) {
		if (clt_timed_out (c))
		    clt_set_stale (c);
		continue;
	    }

	    if (ch == MSG_CS_GAMESTATEFEED_ACK) {
		feed_game_state (c);
		clt_set_ready (c);
		clt_clear_cantimeout (c);
		continue;
	    }
	}
    } while (!done);

    broadcast_rdm_byte (MSG_SC_RESUME);

    return STATE_GAME;
}


static state_t state_game ()
{
    const char *cmd;

    srv_log ("Entering game");

    while (1) {
	if ((cmd = interface->poll ())) {
	    if (0 == ustrcmp (cmd, "stop"))
		goto stop;
	    else if (0 == ustrcmp (cmd, "quit"))
		goto quit;
	    else
		process_command (cmd);
	}

	handle_stale_clients ();

	/* check new connection */
	{
	    clt_t *c;
	    object_t *obj;
	    char buf[NET_MAX_PACKET_SIZE];
	    int size;

	    if ((c = check_new_connection (0))) {
		srv_log ("New client connected.  Feeding game state...");

		/* Joining mid-game.  */

		/* tell other clients to pause */
		broadcast_rdm_byte (MSG_SC_PAUSE);

		/* tell new client we're going to send the game state */
		buf[0] = MSG_SC_GAMESTATEFEED_REQ;
		clt_send_rdm (c, buf, 1);

		/* wait for ack */
		while (1) {
		    yield ();
		    if (clt_receive_rdm (c, buf, sizeof buf) <= 0)
			continue;
		    if (buf[0] == MSG_CS_GAMESTATEFEED_ACK)
			break;
		}

		/* feed game state to the new client */
		feed_game_state (c);

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

		srv_log ("Resuming game");
	    }
	}
	    
	/* receive input */
	{
	    char buf[NET_MAX_PACKET_SIZE];
	    int size;
	    clt_t *c;
	    int done;

	    for_each_fresh_client (c) {
		clt_clear_ready (c);
		clt_set_cantimeout (c);
		clt_set_timeout (c, 10);
	    }

	    do {  /* ... } while (!done); */
		yield ();

		if ((cmd = interface->poll ())) {
		    if (0 == ustrcmp (cmd, "stop"))
			goto stop;
		    else if (0 == ustrcmp (cmd, "quit"))
			goto quit;
		    else
			process_command (cmd);
		}

		done = 1;

		for_each_fresh_client (c) {
		    if (clt_ready (c))
			continue;

		    done = 0;

		    size = clt_receive_rdm (c, buf, sizeof buf);
		    if (size <= 0) {
			if (clt_timed_out (c)) {
			    clt_set_stale (c);
			    destroy_stale_object (clt_object_id (c));
			}
			continue;
		    }			    

		    switch (buf[0]) {
			case MSG_CS_GAMEINFO:
			    process_cs_gameinfo_packet (c, buf+1, size-1);
			    break;

			case MSG_CS_GAMEINFO_DONE:
			    clt_set_ready (c);
			    clt_clear_cantimeout (c);
			    break;

			case MSG_CS_DISCONNECT_ASK:
			    buf[0] = MSG_SC_DISCONNECTED;
			    clt_send_rdm (c, buf, 1);
			    clt_set_stale (c);
			    destroy_stale_object (clt_object_id (c));
			    srv_log ("Client disconnected");
			    break;
		    }
		}
	    } while (!done);
	}
	    
	handle_physics ();
	broadcast_rdm_byte (MSG_SC_GAMEINFO_DONE);
    }

  stop:

    destroy_game_state ();
    broadcast_rdm_byte (MSG_SC_LOBBY);
    return STATE_LOBBY;

  quit:

    destroy_game_state ();
    return STATE_END;
}


static void state_end ()
{
    srv_log ("Disconnecting clients");
    broadcast_rdm_byte (MSG_SC_DISCONNECTED);
}


void game_server ()
{
    state_t state = STATE_LOBBY;

    while (1) switch (state) {

	case STATE_LOBBY:
	    state = state_lobby ();
	    break;

	case STATE_GAME_STATE_FEED:
	    state = state_game_state_feed ();
	    break;

	case STATE_GAME:
	    state = state_game ();
	    break;

	case STATE_END:
	    state_end ();
	    srv_log ("Quitted");
	    return;
    }
}


int game_server_init (game_server_interface_t *iface)
{
    if (!(listen = net_openconn (NET_DRIVER_SOCKETS, "")))
	return -1;
    net_listen (listen);

    interface = iface;
    if (interface->init () < 0) {
	net_closeconn (listen);
	return -1;
    }

    list_init (clients);

    map_filename = ustrdup ("test.pit");

    clt_next_id = 1;  /* not zero! (see clt_object_id) */

    return 0;
}


void game_server_shutdown ()
{
    free (map_filename);
    list_free (clients, clt_destroy);
    interface->shutdown ();
    net_closeconn (listen);
}



/*	Let's all go to the lobby,
	Let's all go to the lobby,
	Let's all go to the lobbiie,
	Get ourselves some snacks.	*/

