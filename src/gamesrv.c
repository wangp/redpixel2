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
#include "physics.h"
#include "timeout.h"
#include "yield.h"


typedef unsigned char uchar_t;
typedef unsigned long ulong_t;

typedef char *string_t;
#define string_init(var)	(var = NULL)
#define string_set(var,str)	({ free (var); var = ustrdup (str); })
#define string_free(var)	({ free (var); var = NULL; })


/*----------------------------------------------------------------------*/
/*			          Ticker	      			*/
/*----------------------------------------------------------------------*/

/* keep in sync with gameclt.c */
#define TICKS_PER_SECOND	(50)
#define MSECS_PER_TICK		(1000 / TICKS_PER_SECOND)

static ulong_t ticks;
static struct timeval ticks_last_update;

static void ticks_init ()
{
    ticks = 0;
    gettimeofday (&ticks_last_update, NULL);
}

static int ticks_poll ()
{
    struct timeval now;
    ulong_t elapsed_msec;

    gettimeofday (&now, NULL);

    elapsed_msec = (((now.tv_sec * 1000) + (now.tv_usec / 1000)) -
		    ((ticks_last_update.tv_sec * 1000) +
		     (ticks_last_update.tv_usec / 1000)));

    if (elapsed_msec > MSECS_PER_TICK) {
	ticks += (elapsed_msec / MSECS_PER_TICK);
	/* XXX this isn't completely accurate */
	ticks_last_update = now;
	return 1;
    }

    return 0;
}

static void ticks_shutdown ()
{
}


/*----------------------------------------------------------------------*/
/*			          Client nodes	      			*/
/*----------------------------------------------------------------------*/

typedef struct client client_t;

typedef enum {
    CLIENT_STATE_NONE = 0,
    CLIENT_STATE_JOINING,
    CLIENT_STATE_JOINED,
    CLIENT_STATE_STALE,
} client_state_t;

struct client {
    client_t *next;
    client_t *prev;
    NET_CONN *conn;
    int id;
    string_t name;
    client_state_t state;
    int flags;
    int controls;
    float aim_angle;
    timeout_t timeout;
    ulong_t pong_time;
    int lag;
};

#define client_object_id(c)	(- (c)->id)

#define client_set_state(c,s)	((c)->state = (s))

#define CLIENT_FLAG_READY	0x01
#define CLIENT_FLAG_WANTFEED	0x02
#define CLIENT_FLAG_CANTIMEOUT	0x04

#define client_ready(c)             ((c)->flags &   CLIENT_FLAG_READY)
#define client_set_ready(c)         ((c)->flags |=  CLIENT_FLAG_READY)
#define client_clear_ready(c)       ((c)->flags &=~ CLIENT_FLAG_READY)
#define client_wantfeed(c)          ((c)->flags &   CLIENT_FLAG_WANTFEED)
#define client_set_wantfeed(c)      ((c)->flags |=  CLIENT_FLAG_WANTFEED)
#define client_clear_wantfeed(c)    ((c)->flags &=~ CLIENT_FLAG_WANTFEED)
#define client_cantimeout(c)        ((c)->flags &   CLIENT_FLAG_CANTIMEOUT)
#define client_set_cantimeout(c)    ((c)->flags |=  CLIENT_FLAG_CANTIMEOUT)
#define client_clear_cantimeout(c)  ((c)->flags &=~ CLIENT_FLAG_CANTIMEOUT)

static list_head_t clients;
static int clients_next_id;

static client_t *client_create (NET_CONN *conn)
{
    client_t *c = alloc (sizeof *c);
    c->conn = conn;
    c->id = clients_next_id++;
    c->name = ustrdup ("(unknown)");
    list_append (clients, c);
    return c;
}

static void client_destroy (client_t *c)
{
    if (c) {
	list_remove (c);
	net_closeconn (c->conn);
	string_free (c);
	free (c);
    }
}

static void client_set_name (client_t *c, const char *name)
{
    string_set (c->name, name);
}

static void client_set_timeout (client_t *c, int secs)
{
    timeout_set (&c->timeout, secs * 1000);
}

static int client_timed_out (client_t *c)
{
    return client_cantimeout (c) && timeout_test (&c->timeout);
}

static int client_send_rdm (client_t *c, const void *buf, int size)
{
    int x = net_send_rdm (c->conn, buf, size);
    if (x < 0)
	error ("internal error: exceeded conn outqueue\n");
    return x;
}

static int client_send_rdm_byte (client_t *c, uchar_t byte)
{
    return client_send_rdm (c, &byte, 1);
}

static int client_send_rdm_encode (client_t *c, const char *fmt, ...)
{
    va_list ap;
    uchar_t buf[NET_MAX_PACKET_SIZE];
    int size;

    va_start (ap, fmt);
    size = packet_encode_v (buf, fmt, ap);
    va_end (ap);
    return client_send_rdm (c, buf, size);
}

static int client_receive_rdm (client_t *c, void *buf, int size)
{
    if (!net_query_rdm (c->conn))
	return 0;
    return net_receive_rdm (c->conn, buf, size);
}

#define for_each_client(c)		list_for_each(c, &clients)

static void clients_broadcast_rdm (const void *buf, int size)
{
    client_t *c;

    for_each_client (c)
	if (c->state != CLIENT_STATE_STALE)
	    client_send_rdm (c, buf, size);
}

static void clients_broadcast_rdm_byte (uchar_t c)
{
    clients_broadcast_rdm (&c, 1);
}

static void clients_broadcast_rdm_encode (const char *fmt, ...)
{
    va_list ap;
    uchar_t buf[NET_MAX_PACKET_SIZE];
    int size;

    va_start (ap, fmt);
    size = packet_encode_v (buf, fmt, ap);
    clients_broadcast_rdm (buf, size);
    va_end (ap);
}

static int clients_count ()
{
    client_t *c;
    int n = 0;
    for_each_client (c) n++;
    return n;
}

static client_t *clients_find_by_id (int id)
{
    client_t *c;

    for_each_client (c)
	if (c->id == id) return c;

    return NULL;
}

static void clients_remove_stale ()
{
    client_t *c, *next;
    
    for (c = clients.next; list_neq (c, &clients); c = next) {
	next = list_next (c);
	if (c->state == CLIENT_STATE_STALE)
	    client_destroy (c);
    }
}

static void clients_init ()
{
    list_init (clients);
    clients_next_id = 1;  /* not zero! (see client_object_id) */
}

static void clients_shutdown ()
{
    list_free (clients, client_destroy);
}


/*----------------------------------------------------------------------*/
/*			          Game server      			*/
/*----------------------------------------------------------------------*/


typedef enum {
    /* Keep this in sync with server_state_procs. */
    SERVER_STATE_LOBBY,
    SERVER_STATE_GAME,
    SERVER_STATE_QUIT
} server_state_t;

static server_state_t server_state;
static game_server_interface_t *interface;
static NET_CONN *listen;
static map_t *map;
static physics_t *physics;
static string_t current_map_file;
static string_t next_map_file;


/* Helper for logging things. */

static void server_log (const char *fmt, ...)
{
    char buf[1024];
    va_list ap;

    va_start (ap, fmt);
    uvszprintf (buf, sizeof buf, fmt, ap);
    interface->add_log (NULL, buf);
    va_end (ap);
}


/* Check new connections on the listening conn. */

static void server_check_new_connections ()
{
    NET_CONN *conn;
    client_t *c;

    conn = net_poll_listen (listen);
    if (!conn)
	return;
    c = client_create (conn);
    client_set_state (c, CLIENT_STATE_JOINING);
    client_send_rdm_encode (c, "cl", MSG_SC_JOININFO, c->id);
}


/* Handling complex incoming network packets. */

static void server_process_cs_gameinfo_packet (client_t *c, const uchar_t *buf, int size)
{
    const void *end = buf + size;

    while (buf != end) {
	switch (*buf++) {
	    case MSG_CS_GAMEINFO_CONTROLS:
		buf += packet_decode (buf, "cf", &c->controls, &c->aim_angle);
		break;

	    default:
		error ("error: unknown code in gameinfo packet (server)\n");
	}
    }
}


/* Handle incoming network traffic from clients. */

static void server_poll_clients_state_joining (client_t *c)
{
    uchar_t buf[NET_MAX_PACKET_SIZE];
    char name[NET_MAX_PACKET_SIZE];
    long len;
    char version;

    if (client_receive_rdm (c, buf, sizeof buf) <= 0)
	return;
	    
    switch (buf[0]) {

	case MSG_CS_JOININFO:
	    packet_decode (buf+1, "cs", &version, &len, &name);
	    client_set_name (c, name);
	    client_set_state (c, CLIENT_STATE_JOINED);
	    if (server_state == SERVER_STATE_GAME)
		client_set_wantfeed (c);
	    server_log ("Client %s joined", c->name);
	    if (version != NETWORK_PROTOCOL_VERSION) {
		client_send_rdm_byte (c, MSG_SC_DISCONNECTED);
		client_set_state (c, CLIENT_STATE_STALE);
		server_log ("Client %s was disconnected for compatibility reasons", c->name);
	    }
	    break;

	case MSG_CS_DISCONNECT_ASK:
	    client_send_rdm_byte (c, MSG_SC_DISCONNECTED);
	    client_set_state (c, CLIENT_STATE_STALE);
	    server_log ("Client %s was disconnected by request", c->name);
	    break;
    }
}

static void server_poll_clients_state_joined (client_t *c)
{
    uchar_t buf[NET_MAX_PACKET_SIZE];
    int size;

    size = client_receive_rdm (c, buf, sizeof buf);
    if (size <= 0)
	return;

    switch (buf[0]) {

	case MSG_CS_GAMEINFO:
	    if (server_state == SERVER_STATE_GAME)
		server_process_cs_gameinfo_packet (c, buf+1, size-1);
	    break;

	case MSG_CS_PING:
	    c->pong_time = ticks;
	    client_send_rdm_byte (c, MSG_SC_PONG);
	    break;

	case MSG_CS_BOING:
	    c->lag = (ticks - c->pong_time) / 2;
	    break;

	case MSG_CS_DISCONNECT_ASK:
	    client_send_rdm_byte (c, MSG_SC_DISCONNECTED);
	    client_set_state (c, CLIENT_STATE_STALE);
	    server_log ("Client %s was disconnected by request", c->name);

	    if (server_state == SERVER_STATE_GAME) {
		object_t *obj = map_find_object (map, client_object_id (c));
		if (obj) {
		    clients_broadcast_rdm_encode ("ccl", MSG_SC_GAMEINFO,
						  MSG_SC_GAMEINFO_OBJECT_DESTROY,
						  client_object_id (c));
		    map_unlink_object (obj);
		    object_destroy (obj);
		}
	    }
	    break;
    }
}

static void server_poll_clients ()
{
    client_t *c;

    for_each_client (c) switch (c->state) {

	case CLIENT_STATE_NONE:
	    error ("internal error: client should never be set to CLIENT_STATE_NONE\n");

	case CLIENT_STATE_JOINING:
	    server_poll_clients_state_joining (c);
	    break;

	case CLIENT_STATE_JOINED:
	    server_poll_clients_state_joined (c);
	    break;

	case CLIENT_STATE_STALE:
	    break;
    }
}


/* Processing commands from the game server interface. */

static const char *whitespace = " \t";

/* ustrtok_r:
 *  Unicode-aware version of the POSIX strtok_r() function.
 *  Modified from the Allegro ustrtok() function.
 */
static char *ustrtok_r(char *s, AL_CONST char *set, char **last)
{
   char *prev_str, *tok;
   AL_CONST char *setp;
   int c, sc;

   if (!s) {
      s = *last;

      if (!s)
	 return NULL;
   }

   skip_leading_delimiters:

   prev_str = s;
   c = ugetxc((AL_CONST char **)&s);

   setp = set;

   while ((sc = ugetxc(&setp)) != 0) {
      if (c == sc)
	 goto skip_leading_delimiters;
   }

   if (!c) {
      *last = NULL;
      return NULL;
   }

   tok = prev_str;

   for (;;) {
      prev_str = s;
      c = ugetxc((AL_CONST char **)&s);

      setp = set;

      do {
	 sc = ugetxc(&setp);
	 if (sc == c) {
	    if (!c) {
	       *last = NULL;
	       return tok;
	    }
	    else {
	       s += usetat(prev_str, 0, 0);
	       *last = s;
	       return tok;
	    }
	 }
      } while (sc);
   }
}

static void poll_interface_command_list (char **last)
{
    int n = clients_count ();
    client_t *c;	

    server_log ("Clients: %d", n);

    for_each_client (c) {
	switch (c->state) {
	    case CLIENT_STATE_STALE:
		server_log ("%4d  %s (stale)", c->id, c->name);
		break;
	    default:
		server_log ("%4d  %s (lag: %d)", c->id, c->name, c->lag);
		break;
	}
    }
}

static void poll_interface_command_kick (char **last)
{
    char *word;
    int id;
    client_t *c;

    word = ustrtok_r (NULL, whitespace, last);
    if (!word) {
	server_log ("KICK requires an argument");
	return;
    }
    
    id = ustrtol (word, NULL, 10);
    if (!id) {
	server_log ("Invalid argument to KICK");
	return;
    }

    c = clients_find_by_id (id);
    if (!c) {
	server_log ("No client with id %d", id);
	return;
    }

    if (c->state == CLIENT_STATE_STALE) {
	server_log ("Client %s already disconnected", c->name);
	return;
    }

    client_send_rdm_byte (c, MSG_SC_DISCONNECTED);
    client_set_state (c, CLIENT_STATE_STALE);
    server_log ("Client %s was kicked", c->name);
    
    /* XXX can share this */
    if (server_state == SERVER_STATE_GAME) {
	object_t *obj = map_find_object (map, client_object_id (c));
	if (obj) {
	    clients_broadcast_rdm_encode ("ccl", MSG_SC_GAMEINFO,
					  MSG_SC_GAMEINFO_OBJECT_DESTROY,
					  client_object_id (c));
	    map_unlink_object (obj);
	    object_destroy (obj);
	}
    }
}

static void server_poll_interface ()
{
#define wordis(test)	(0 == ustricmp (word, test))

    const char *cmd;
    char *copy, *word, *last;

    cmd = interface->poll ();
    if (!cmd) return;

    copy = ustrdup (cmd);
    word = ustrtok_r (copy, whitespace, &last);

    if (word) {
	if (wordis ("help") || wordis ("?")) {
	    server_log ("Commands:");
	    server_log ("  MAP <filename>           - select a map");
	    server_log ("  MAP                      - display current map");
	    server_log ("  START                    - enter game mode");
	    server_log ("  STOP                     - return to the lobby");
	    server_log ("  RESTART                  - restart game mode (with new map)");
	    server_log ("  QUIT                     - quit completely");
	    server_log ("  LIST                     - list clients");
	    server_log ("  KICK <clientid>          - forcefully disconnect a client");
	    /* XXX 'kick' needs to take an optional 'reason' argument */
	    server_log ("  MSG <message>            - broadcast text message to clients");
	    server_log ("  CONTEXT                  - show current context");
	}

	else if (wordis ("map")) {
	    word = ustrtok_r (NULL, whitespace, &last);
	    if (!word) {
		server_log ("Current map: %s", current_map_file);
		server_log ("Selected map: %s", next_map_file);
	    } else {
		string_set (next_map_file, word);
		server_log ("Setting map to %s", next_map_file);
	    }
	}
	
	else if (wordis ("start")) {
	    server_state = SERVER_STATE_GAME;
	}

	else if (wordis ("stop")) {
	    server_state = SERVER_STATE_LOBBY;
	}

	else if (wordis ("quit")) {
	    server_state = SERVER_STATE_QUIT;
	}

	else if (wordis ("list")) {
	    poll_interface_command_list (&last);
	}

	else if (wordis ("kick")) {
	    poll_interface_command_kick (&last);
	}

	else if (wordis ("context")) {
	    server_log ((server_state == SERVER_STATE_LOBBY) ? "In the lobby" :
		     (server_state == SERVER_STATE_GAME) ? "Playing a game" :
		     "Unknown context (probably in a transition)");
	}

	else {
	    server_log ("Unrecognised command: %s", word);
	}
    }

    free (copy);

#undef wordis
}


/* Create and free the game state. */

static start_t *server_pick_random_start ()
{
    start_t *start;
    int n = random () % 32;	/* XXX temp */

    start = map_start_list (map)->next;
    while (n--) {
	start = list_next (start);
	if (list_eq (start, map_start_list (map)))
	    start = list_next (start);
    }

    return start;
}

static int server_init_game_state ()
{
    client_t *c;

    map = map_load (next_map_file, 1, 0);
    if (!map)
	return -1;
    string_set (current_map_file, next_map_file);

    for_each_client (c) if (c->state == CLIENT_STATE_JOINED) {
	start_t *start;
	object_t *obj;

	start = server_pick_random_start ();
	obj = object_create_ex ("player", client_object_id (c));
	object_set_xy (obj, map_start_x (start), map_start_y (start));
	map_link_object_bottom (map, obj);
    }

    physics = physics_create (map);
    if (!physics) {
	map_destroy (map);
	return -1;
    }

    return 0;
}

static void server_free_game_state ()
{
    if (physics) {
	physics_destroy (physics);
	physics = NULL;
    }

    if (map) {
	map_destroy (map);
	map = NULL;
    }
}


/* Feeding the game state to clients. */

static void server_feed_game_state_to (client_t *c)
{
    uchar_t buf[NET_MAX_PACKET_SIZE] = { MSG_SC_GAMEINFO };
    uchar_t *p = buf+1;

    /* map */
    p += packet_encode (p, "cs", MSG_SC_GAMEINFO_MAPLOAD, current_map_file);

    /* objects */
    {
	list_head_t *list;
	object_t *obj;
	const char *typename;

	list = map_object_list (map);
	list_for_each (obj, list) {
	    typename = objtype_name (object_type (obj));

	    /* XXX check for packet overflow */
	    /* XXX this is not nice */ 
	    if ((p - buf) + 1 + 4 + strlen (typename) + 20 > sizeof buf) {
		client_send_rdm (c, buf, p - buf);
		p = buf+1;
    	    }

	    p += packet_encode (p, "cslffff", MSG_SC_GAMEINFO_OBJECT_CREATE,
				typename, object_id (obj),
				object_x (obj), object_y (obj),
				object_xv (obj), object_yv (obj));
	}
    }

    if (p - buf > 1)
	client_send_rdm (c, buf, p - buf);

    client_send_rdm_byte (c, MSG_SC_GAMESTATEFEED_DONE);

    /* After the client receives the game state, it will automatically
     * switch to 'paused' mode. */
}

static void server_perform_mass_game_state_feed ()
{
    client_t *c;
    int done;
    uchar_t byte;

    clients_broadcast_rdm_byte (MSG_SC_GAMESTATEFEED_REQ);

    for_each_client (c) {
	if (c->state == CLIENT_STATE_JOINED) {
	    client_clear_ready (c);
	    client_set_cantimeout (c);
	    client_set_timeout (c, 5);
	}
    }

    do {
	yield ();

	done = 1;
	for_each_client (c) {
	    if (c->state != CLIENT_STATE_JOINED)
		continue;
	    
	    if (client_ready (c))
		continue;

	    if (client_timed_out (c)) {
		client_set_state (c, CLIENT_STATE_STALE);
		server_log ("Client %s timed out during game state feed", c->name);
		continue;
	    }
	    
	    if (client_receive_rdm (c, &byte, 1) <= 0) {
		done = 0;
		continue;
	    }

	    if (byte == MSG_CS_GAMESTATEFEED_ACK) {
		server_feed_game_state_to (c);
		client_set_ready (c);
		client_clear_cantimeout (c);
	    }
	}
    } while (!done);

    clients_broadcast_rdm_byte (MSG_SC_RESUME);
}

static void server_perform_single_game_state_feed (client_t *c)
{
    uchar_t byte;

    client_send_rdm_byte (c, MSG_SC_GAMESTATEFEED_REQ);

    client_set_cantimeout (c);
    client_set_timeout (c, 5);

    while (1) {
	yield ();

	if (client_timed_out (c)) {
	    client_set_state (c, CLIENT_STATE_STALE);
	    server_log ("Client %s timed out during game state feed", c->name);
	    continue;
	}

	if (client_receive_rdm (c, &byte, 1) <= 0)
	    continue;

	if (byte == MSG_CS_GAMESTATEFEED_ACK) {
	    server_feed_game_state_to (c);
	    client_set_ready (c);
	    client_clear_cantimeout (c);
	    break;
	}
    }
}


/* Handling game physics. */

static void server_handle_client_controls ()
{
    client_t *c;
    object_t *obj;

    for_each_client (c) {
	if (c->state != CLIENT_STATE_JOINED)
	    continue;

	obj = map_find_object (map, client_object_id (c));
	if (!obj)
	    error ("error: server missing object\n");
	
	/* left */
	if (c->controls & CONTROL_LEFT) {
	    object_set_xv (obj, object_xv (obj) - 1.4);
	    object_set_need_replication (obj);
	}

	/* right */
	if (c->controls & CONTROL_RIGHT) {
	    object_set_xv (obj, object_xv (obj) + 1.4);
	    object_set_need_replication (obj);
	}
	    
	/* up */
	if (c->controls & CONTROL_UP) {
	    float jump = object_jump (obj);

	    if (jump > 0) {
		object_set_yv (obj, object_yv (obj) - MIN (8, 20 / jump));
		object_set_jump (obj, (jump < 10) ? (jump + 1) : 0);
		object_set_need_replication (obj);
	    }
	    else if ((jump == 0) && (object_yv (obj) == 0) && (object_supported (obj, map))) {
		object_set_yv (obj, object_yv (obj) - 4);
		object_set_jump (obj, 1);
		object_set_need_replication (obj);
	    }
	}
	else {
	    object_set_jump (obj, 0);
	}
	
	/* fire */
	if (c->controls & CONTROL_FIRE) {
	    /* XXX testing */
	    object_t *pl;
	    object_t *shell;
	    
	    pl = map_find_object (map, client_object_id (c));
	    if (pl) {
		shell = object_create ("basic-arrow-projectile");
		object_set_xy (shell, object_x (pl)+10, object_y (pl));
		object_set_xv (shell, 25 * cos (c->aim_angle));
		object_set_yv (shell, 25 * sin (c->aim_angle));
		map_link_object_bottom (map, shell);

		{
		    object_t *obj = shell;
		    char buf[NET_MAX_PACKET_SIZE];
		    int size;
		    size = packet_encode
			(buf, "ccslffff", MSG_SC_GAMEINFO,
			 MSG_SC_GAMEINFO_OBJECT_CREATE,
			 "basic-arrow-projectile", object_id (obj),
			 object_x (obj), object_y (obj),
			 object_xv (obj), object_yv (obj));
		    clients_broadcast_rdm (buf, size);
		}
	    }
	    
	    c->controls &=~ CONTROL_FIRE;
	}
    }
}

static void server_perform_physics ()
{
    list_head_t *object_list;
    object_t *obj;

    server_handle_client_controls ();

    object_list = map_object_list (map);
    list_for_each (obj, object_list)
	physics_move_object (physics, obj);
}


/* Sending important object changes to clients. */

static void server_send_object_updates ()
{
    uchar_t buf[NET_MAX_PACKET_SIZE] = { MSG_SC_GAMEINFO };
    uchar_t *p = buf+1;
    list_head_t *object_list;
    object_t *obj;

    object_list = map_object_list (map);
    list_for_each (obj, object_list) {
	if (object_need_replication (obj)) {
	    /* XXX check for packet overflow */
	    /* XXX this is not nice */ 
	    if ((p - buf) + 1 + 20 > sizeof buf) {
		clients_broadcast_rdm (buf, p - buf);
		p = buf+1;
    	    }

	    p += packet_encode (p, "clffff", MSG_SC_GAMEINFO_OBJECT_UPDATE,
				object_id (obj), object_x (obj), object_y (obj),
				object_xv (obj), object_client_yv (obj));

	    object_clear_need_replication (obj);
	}
    }

    if ((p - buf) > 1)
	clients_broadcast_rdm (buf, p - buf);
}


/* Lobby state. */
  
static int server_state_lobby_init ()
{
    /*	Let's all go to the lobby,
	Let's all go to the lobby,
	Let's all go to the lobbiie,
	Get ourselves some snacks.   */

    server_log ("Entering lobby");
    clients_broadcast_rdm_byte (MSG_SC_LOBBY);
    return 0;
}

static void server_state_lobby_poll ()
{
}

static void server_state_lobby_shutdown ()
{
    server_log ("Leaving lobby");
}


/* Game state. */

static int server_state_game_init ()
{
    server_log ("Entering game");

    if (server_init_game_state () < 0) {
	server_state = SERVER_STATE_LOBBY;
	return -1;
    }

    server_perform_mass_game_state_feed ();
    ticks_init ();
    return 0;
}

static void server_handle_wantfeeds ()
{
    client_t *c;

    for_each_client (c)
	if (client_wantfeed (c))
	    goto need_feeding;

    return;

  need_feeding:

    clients_broadcast_rdm_byte (MSG_SC_PAUSE);

    for_each_client (c) if (client_wantfeed (c)) {
	server_log ("Feeding new client %s", c->name);
	server_perform_single_game_state_feed (c);

	/* XXX */
	{
	    object_t *obj;
	    start_t *start;
	    
	    start = server_pick_random_start ();
	    obj = object_create_ex ("player", client_object_id (c));
	    object_set_xy (obj, map_start_x (start), map_start_y (start));
	    map_link_object_bottom (map, obj);

	    clients_broadcast_rdm_encode
		("ccslffff", MSG_SC_GAMEINFO, MSG_SC_GAMEINFO_OBJECT_CREATE,
		 objtype_name (object_type (obj)),
		 object_id (obj), object_x (obj), object_y (obj),
		 object_xv (obj), object_yv (obj));
	}

    	client_clear_wantfeed (c);
    }

    clients_broadcast_rdm_byte (MSG_SC_RESUME);
}

static void server_state_game_poll ()
{
    if (!ticks_poll ())
	return;
    server_handle_wantfeeds ();
    server_perform_physics ();
    server_send_object_updates ();
}

static void server_state_game_shutdown ()
{
    server_log ("Leaving game");
    ticks_shutdown ();
    server_free_game_state ();
}


/* The main loop of the game server. */

typedef struct {
    int (*init) ();
    void (*poll) ();
    void (*shutdown) ();
} server_state_procs_t;

static server_state_procs_t server_state_procs[] =
{
    /* Keep this in sync with server_state_t. */
    { server_state_lobby_init,
      server_state_lobby_poll,
      server_state_lobby_shutdown },
    { server_state_game_init,
      server_state_game_poll,
      server_state_game_shutdown },
    { NULL, NULL, NULL } /* quit */
};

void game_server_run ()
{
    server_state_t curr_state = -1;
    server_state_procs_t *p = NULL;

    while (curr_state != SERVER_STATE_QUIT) {
	yield ();

	while (curr_state != server_state) {
	    p = server_state_procs + server_state;
	    if ((p->init) && (p->init () < 0))
		curr_state = -1;
	    else
		curr_state = server_state;
	}

	if (p->poll)
	    p->poll ();

	if (curr_state == server_state) {
	    server_check_new_connections ();
	    server_poll_clients ();
	    clients_remove_stale ();
	    server_poll_interface ();
	}

	if (curr_state != server_state) {
	    if (p->shutdown)
		p->shutdown ();
	}
    }

    server_log ("Disconnecting clients");
    clients_broadcast_rdm_byte (MSG_SC_DISCONNECTED);
    server_log ("Quitting");
}


/* Initialisation and shutdown. */

int game_server_init (game_server_interface_t *iface)
{
    listen = net_openconn (NET_DRIVER_SOCKETS, "");
    if (!listen)
	return -1;
    net_listen (listen);

    interface = iface;
    interface->init ();

    clients_init ();

    string_init (current_map_file);
    string_init (next_map_file);
    string_set (current_map_file, "test.pit");
    string_set (next_map_file,  "test.pit");
    server_state = SERVER_STATE_LOBBY;

    return 0;    
}

void game_server_shutdown ()
{
    string_free (next_map_file);
    string_free (current_map_file);
    clients_shutdown ();
    interface->shutdown ();
    net_closeconn (listen);
}

