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


/* Pseudo-string stuff. */

typedef char *string_t;
#define string_init(var)	(var = NULL)
#define string_set(var,str)	({ free (var); var = ustrdup (str); })
#define string_free(var)	({ free (var); var = NULL; })


/* Client nodes. */

typedef struct clt clt_t;

typedef enum {
    CLT_STATE_NONE = 0,
    CLT_STATE_JOINING,
    CLT_STATE_JOININGQUEUE,
    CLT_STATE_JOINED,
    CLT_STATE_STALE,
} clt_state_t;

struct clt {
    clt_t *next;
    clt_t *prev;
    NET_CONN *conn;
    int id;
    string_t name;
    clt_state_t state;
    int flags;
    int controls;
    timeout_t timeout;
};

#define clt_object_id(c)	(- (c)->id)

#define clt_state(c)		((c)->state)
#define clt_set_state(c,s)	((c)->state = (s))

#define CLT_FLAG_READY		0x02
#define CLT_FLAG_CANTIMEOUT	0x04

#define clt_test_flag(c,f)	((c)->flags & (f))
#define clt_set_flag(c,f)	((c)->flags |= (f))
#define clt_clear_flag(c,f)	((c)->flags &=~ (f))

#define clt_ready(c)		(clt_test_flag (c, CLT_FLAG_READY))
#define clt_set_ready(c)	(clt_set_flag (c, CLT_FLAG_READY))
#define clt_clear_ready(c)	(clt_clear_flag (c, CLT_FLAG_READY))

#define clt_cantimeout(c)	(clt_test_flag (c, CLT_FLAG_CANTIMEOUT))
#define clt_set_cantimeout(c)	(clt_set_flag (c, CLT_FLAG_CANTIMEOUT))
#define clt_clear_cantimeout(c)	(clt_clear_flag (c, CLT_FLAG_CANTIMEOUT))

static list_head_t clients;
static int clt_next_id;

static clt_t *clt_create (NET_CONN *conn)
{
    clt_t *c = alloc (sizeof *c);
    c->conn = conn;
    c->id = clt_next_id++;
    list_append (clients, c);
    return c;
}

static void clt_destroy (clt_t *c)
{
    if (c) {
	list_remove (c);
	net_closeconn (c->conn);
	string_free (c);
	free (c);
    }
}

static const char *clt_name (clt_t *c)
{
    return c->name;
}

static void clt_set_name (clt_t *c, const char *name)
{
    string_set (c->name, name);
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

static int clt_send_rdm_byte (clt_t *c, char byte)
{
    return net_send_rdm (c->conn, &byte, 1);
}

static int clt_send_rdm_encode (clt_t *c, const char *fmt, ...)
{
    va_list ap;
    char buf[NET_MAX_PACKET_SIZE];
    int size;

    va_start (ap, fmt);
    size = packet_encode_v (buf, fmt, ap);
    va_end (ap);
    return net_send_rdm (c->conn, buf, size);
}

static int clt_receive_rdm (clt_t *c, void *buf, int size)
{
    if (!net_query_rdm (c->conn))
	return 0;
    return net_receive_rdm (c->conn, buf, size);
}

static void clt_init ()
{
    list_init (clients);
    clt_next_id = 1;  /* not zero! (see clt_object_id) */
}

static void clt_shutdown ()
{
    list_free (clients, clt_destroy);
}

#define for_each_clt(c)		list_for_each(c, &clients)

static void clt_broadcast_rdm (const void *buf, int size)
{
    clt_t *c;

    for_each_clt (c)
	if (clt_state (c) != CLT_STATE_STALE)
	    clt_send_rdm (c, buf, size);
}

static void clt_broadcast_rdm_byte (char c)
{
    clt_broadcast_rdm (&c, 1);
}

static void clt_broadcast_rdm_encode (const char *fmt, ...)
{
    va_list ap;
    char buf[NET_MAX_PACKET_SIZE];
    int size;

    va_start (ap, fmt);
    size = packet_encode_v (buf, fmt, ap);
    clt_broadcast_rdm (buf, size);
    va_end (ap);
}

static int clt_count ()
{
    clt_t *c;
    int n = 0;
    for_each_clt (c) n++;
    return n;
}

static clt_t *clt_find (int id)
{
    clt_t *c;

    for_each_clt (c)
	if (c->id == id) return c;

    return NULL;
}


/*----------------------------------------------------------------------*/
/*			Next generation game server			*/
/*----------------------------------------------------------------------*/


/* XXX not happy with this system.  Too much duplicated code in the
   transition states, and possible problems can occur during the
   transitions (e.g. client joins during the transition) */
typedef enum {
    SRV_STATE_LOBBY,
    SRV_STATE_LOBBY_TO_GAME,
    SRV_STATE_LOBBY_TO_QUIT,
    SRV_STATE_GAME,
    SRV_STATE_FEEDNEWCLIENT,
    SRV_STATE_GAME_TO_GAME,
    SRV_STATE_GAME_TO_LOBBY,
    SRV_STATE_GAME_TO_QUIT,
    SRV_STATE_QUIT
} srv_state_t;

static srv_state_t srv_state;
static game_server_interface_t *interface;
static NET_CONN *listen;
static clt_t *client_to_feed;
static map_t *map;
static physics_t *physics;
static string_t current_map_file;
static string_t next_map_file;


/* Helper for logging things. */

static void srv_log (const char *fmt, ...)
{
    char buf[1024];
    va_list ap;

    va_start (ap, fmt);
    uvszprintf (buf, sizeof buf, fmt, ap);
    interface->add_log (NULL, buf);
    va_end (ap);
}


/* Check new connections on the listening conn. */

static void ng_check_new_connections ()
{
    NET_CONN *conn;
    clt_t *c;

    conn = net_poll_listen (listen);
    if (!conn)
	return;
    c = clt_create (conn);
    clt_set_state (c, CLT_STATE_JOINING);
    clt_send_rdm_encode (c, "cl", MSG_SC_JOININFO, c->id);
}


/* Handling complex incoming network packets. */

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


/* Handle incoming network traffic from clients. */

static void ng_poll_clients_state_joining (clt_t *c)
{
    char buf[NET_MAX_PACKET_SIZE];
    char name[NET_MAX_PACKET_SIZE];
    long len;

    if (clt_receive_rdm (c, buf, sizeof buf) <= 0)
	return;
	    
    switch (buf[0]) {

	case MSG_CS_JOININFO:
	    packet_decode (buf+1, "s", &len, &name);
	    clt_set_name (c, name);
	    srv_log ("Client %s joined\n", clt_name (c));
	    switch (srv_state) {
		case SRV_STATE_GAME:
		    client_to_feed = c;
		    srv_state = SRV_STATE_FEEDNEWCLIENT;
		    break;
		case SRV_STATE_FEEDNEWCLIENT:
		    clt_set_state (c, CLT_STATE_JOININGQUEUE);
		    break;
		default:
		    clt_set_state (c, CLT_STATE_JOINED);
		    break;
	    }
	    break;

	case MSG_CS_DISCONNECT_ASK:
	    clt_send_rdm_byte (c, MSG_SC_DISCONNECTED);
	    clt_set_state (c, CLT_STATE_STALE);
	    srv_log ("Client %s was disconnected by request", clt_name (c));
	    break;
    }
}

static void ng_poll_clients_state_joined (clt_t *c)
{
    char buf[NET_MAX_PACKET_SIZE];
    int size;

    size = clt_receive_rdm (c, buf, sizeof buf);
    if (size <= 0)
	return;

    switch (buf[0]) {

	case MSG_CS_GAMEINFO:
	    if (srv_state == SRV_STATE_GAME)
		process_cs_gameinfo_packet (c, buf+1, size-1);
	    break;

	case MSG_CS_DISCONNECT_ASK:
	    clt_send_rdm_byte (c, MSG_SC_DISCONNECTED);
	    clt_set_state (c, CLT_STATE_STALE);
	    srv_log ("Client %s was disconnected by request", clt_name (c));

	    clt_broadcast_rdm_encode ("ccl", MSG_SC_GAMEINFO,
				      MSG_SC_GAMEINFO_OBJECTDESTROY,
				      clt_object_id (c));
	    break;
    }
}

static void ng_poll_clients ()
{
    clt_t *c;

    for_each_clt (c) switch (clt_state (c)) {

	case CLT_STATE_NONE:
	    error ("internal error: clt should never be set to CLT_STATE_NONE\n");

	case CLT_STATE_JOINING:
	    ng_poll_clients_state_joining (c);
	    break;

	case CLT_STATE_JOININGQUEUE:
	    switch (srv_state) {
		case SRV_STATE_GAME:
		    client_to_feed = c;
		    srv_state = SRV_STATE_FEEDNEWCLIENT;
		    break;
		case SRV_STATE_FEEDNEWCLIENT:
		    break;
		default:
		    clt_set_state (c, CLT_STATE_JOINED);
		    break;
	    }
	    break;

	case CLT_STATE_JOINED:
	    ng_poll_clients_state_joined (c);
	    break;

	case CLT_STATE_STALE:
	    break;
    }
}


/* Remove stale clients from the client list. */

static void ng_remove_stale_clients ()
{
    clt_t *c, *next;
    
    for (c = clients.next; list_neq (c, &clients); c = next) {
	next = list_next (c);
	if (clt_state (c) == CLT_STATE_STALE)
	    clt_destroy (c);
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

static void ng_poll_interface_kick (char **last)
{
    char *word;
    int id;
    clt_t *c;

    word = ustrtok_r (NULL, whitespace, last);
    if (!word) {
	srv_log ("KICK requires an argument");
	return;
    }
    
    id = ustrtol (word, NULL, 10);
    if (!id) {
	srv_log ("Invalid argument to KICK");
	return;
    }

    c = clt_find (id);
    if (!c) {
	srv_log ("No client with id %d", id);
	return;
    }

    if (clt_state (c) == CLT_STATE_STALE) {
	srv_log ("Client %s already disconnected", clt_name (c));
	return;
    }

    clt_send_rdm_byte (c, MSG_SC_DISCONNECTED);
    clt_set_state (c, CLT_STATE_STALE);
    srv_log ("Client %s was kicked", clt_name (c));
}

static void ng_poll_interface ()
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
	    srv_log ("Commands:");
	    srv_log ("  MAP <filename>           - select a map");
	    srv_log ("  MAP                      - display current map");
	    srv_log ("  START                    - enter game mode");
	    srv_log ("  STOP                     - return to the lobby");
	    srv_log ("  RESTART                  - restart game mode (with new map)");
	    srv_log ("  QUIT                     - quit completely");
	    srv_log ("  LIST                     - list clients");
	    srv_log ("  KICK <clientid>          - forcefully disconnect a client");
	    /* XXX 'kick' needs to take an optional 'reason' argument */
	    srv_log ("  MSG <message>            - broadcast text message to clients");
	    srv_log ("  CONTEXT                  - show current context");
	}

	else if (wordis ("map")) {
	    word = ustrtok_r (NULL, whitespace, &last);
	    if (!word) {
		srv_log ("Current map: %s", current_map_file);
		srv_log ("Selected map: %s", next_map_file);
	    } else {
		string_set (next_map_file, word);
		srv_log ("Setting map to %s", next_map_file);
	    }
	}
	
	else if (wordis ("start")) {
	    if (srv_state == SRV_STATE_LOBBY)
		srv_state = SRV_STATE_LOBBY_TO_GAME;
	    else
		srv_log ("START not available in this context");
	}

	else if (wordis ("stop")) {
	    if (srv_state == SRV_STATE_GAME)
		srv_state = SRV_STATE_GAME_TO_LOBBY;
	    else
		srv_log ("STOP not available in this context");
	}

	else if (wordis ("restart")) {
	    if (srv_state == SRV_STATE_GAME)
		srv_state = SRV_STATE_GAME_TO_GAME;
	    else
		srv_log ("RESTART not available in this context");
	}

	else if (wordis ("quit")) {
	    if (srv_state == SRV_STATE_LOBBY)
		srv_state = SRV_STATE_LOBBY_TO_QUIT;
	    else if (srv_state == SRV_STATE_GAME)
		srv_state = SRV_STATE_GAME_TO_QUIT;
	    else
		srv_log ("QUIT not available in this context");
	}

	else if (wordis ("list")) {
	    clt_t *c;
	    srv_log ("Number of clients: %d", clt_count ());
	    for_each_clt (c) srv_log ("%4d  %s", c->id, clt_name (c));
	}

	else if (wordis ("kick")) {
	    ng_poll_interface_kick (&last);
	}

	else if (wordis ("context")) {
	    srv_log ((srv_state == SRV_STATE_LOBBY) ? "In the lobby" :
		     (srv_state == SRV_STATE_GAME) ? "Playing a game" :
		     "Unknown context (probably in a transition)");
	}

	else {
	    srv_log ("Unrecognised command: %s\n", word);
	}
    }

    free (copy);

#undef wordis
}


/* Create and free the game state. */

static int ng_init_game_state ()
{
    start_t *start;
    object_t *obj;
    clt_t *c;

    map = map_load (next_map_file, 1, 0);
    if (!map)
	return -1;
    string_set (current_map_file, next_map_file);

    start = map_start_list (map)->next;

    for_each_clt (c) {
	if (clt_state (c) == CLT_STATE_JOINED) {
	    obj = object_create_ex ("player", clt_object_id (c), 1);
	    object_set_xy (obj, map_start_x (start), map_start_y (start));
	    map_link_object_bottom (map, obj);

	    start = list_next (start);
	    if (list_eq (start, map_start_list (map)))
		start = list_next (start);
	}
    }

    physics = physics_create (map);
    if (!physics) {
	map_destroy (map);
	return -1;
    }

    return 0;
}

static void ng_shutdown_game_state ()
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

static void ng_feed_game_state_to (clt_t *c)
{
    char buf[NET_MAX_PACKET_SIZE] = { MSG_SC_GAMEINFO };
    char *p = buf+1;

    /* map */
    p += packet_encode (p, "cs", MSG_SC_GAMEINFO_MAPLOAD, current_map_file);

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
    
    clt_send_rdm_byte (c, MSG_SC_GAMESTATEFEED_DONE);

    /* After the client receives the game state, it will automatically
     * switch to 'paused' mode. */
}

static void ng_perform_mass_game_state_feed ()
{
    clt_t *c;
    int done;
    char byte;

    clt_broadcast_rdm_byte (MSG_SC_GAMESTATEFEED_REQ);

    for_each_clt (c) {
	if (clt_state (c) == CLT_STATE_JOINED) {
	    clt_clear_ready (c);
	    clt_set_cantimeout (c);
	    clt_set_timeout (c, 10);
	}
    }

    do {
	yield ();

	done = 1;
	for_each_clt (c) {
	    if (clt_state (c) != CLT_STATE_JOINED)
		continue;
	    
	    if (clt_ready (c))
		continue;

	    if (clt_timed_out (c)) {
		clt_set_state (c, CLT_STATE_STALE);
		srv_log ("Client %s timed out during game state feed", clt_name (c));
		continue;
	    }
	    
	    if (clt_receive_rdm (c, &byte, 1) <= 0) {
		done = 0;
		continue;
	    }

	    if (byte == MSG_CS_GAMESTATEFEED_ACK) {
		ng_feed_game_state_to (c);
		clt_set_ready (c);
		clt_clear_cantimeout (c);
		continue;
	    }
	}
    } while (!done);

    clt_broadcast_rdm_byte (MSG_SC_RESUME);
}

static void ng_perform_single_game_state_feed (clt_t *c)
{
    char byte;

    clt_broadcast_rdm_byte (MSG_SC_PAUSE);
    clt_send_rdm_byte (c, MSG_SC_GAMESTATEFEED_REQ);

    clt_set_cantimeout (c);
    clt_set_timeout (c, 10);

    while (1) {
	yield ();

	if (clt_timed_out (c)) {
	    clt_set_state (c, CLT_STATE_STALE);
	    srv_log ("Client %s timed out during game state feed", clt_name (c));
	    continue;
	}

	if (clt_receive_rdm (c, &byte, 1) <= 0)
	    continue;

	if (byte == MSG_CS_GAMESTATEFEED_ACK) {
	    ng_feed_game_state_to (c);
	    clt_clear_cantimeout (c);
	    break;
	}
    }

    clt_broadcast_rdm_byte (MSG_SC_RESUME);
}


/* Handling game physics. */

static void handle_client_controls ()
{
    clt_t *c;
    object_t *obj;

    for_each_clt (c) {
	if (clt_state (c) != CLT_STATE_JOINED)
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
	    float jump = object_jump (obj);

	    if (jump > 0) {
		object_set_yv (obj, object_yv (obj) - MIN (8, 20 / jump));
		object_set_jump (obj, (jump < 10) ? (jump + 1) : 0);
	    }
	    else if ((jump == 0) && (object_yv (obj) == 0) && (object_supported (obj, map))) {
		object_set_yv (obj, object_yv (obj) - 4);
		object_set_jump (obj, 1);
	    }
	}
	else {
	    object_set_jump (obj, 0);
	}
    }
}

static void ng_perform_physics ()
{
    char buf[NET_MAX_PACKET_SIZE] = { MSG_SC_GAMEINFO };
    char *p = buf+1;
    list_head_t *object_list;
    object_t *obj;

    handle_client_controls ();

    object_list = map_object_list (map);
    list_for_each (obj, object_list) {
	if (physics_move_object (physics, obj)) {
/* The client should be able to do this itself soon.  */
/*  	    p += packet_encode (p, "clff", MSG_SC_GAMEINFO_OBJECTMOVE, */
/*  				object_id (obj), object_x (obj), */
/*  				object_y (obj)); */
	}
    }

    if ((p - buf) > 1)
	clt_broadcast_rdm (buf, p - buf);
}


/* Sending important game state changes to clients. */

static void ng_send_game_updates ()
{
}


/* Finishing up before the game server quits for good. */

static void ng_finalise ()
{
    if (clt_count () > 0) {
	srv_log ("Disconnecting clients");
	clt_broadcast_rdm_byte (MSG_SC_DISCONNECTED);
    }
}


/* Count number of ticks we need to perform.  We use `gettimeofday'
 * instead of Allegro timers because we might be using SYSTEM_NONE. */

#define TICKS_PER_SECOND  50

static unsigned long ticks_interval;
static struct timeval ticks_lasttime;
static int ticks_remaining;

static void ng_init_ticks ()
{
    ticks_interval = 1000 / TICKS_PER_SECOND;
    gettimeofday (&ticks_lasttime, NULL);
    ticks_remaining = 0;
}

static int ng_update_ticks ()
{
    struct timeval now;
    unsigned long elapsed;

    if (ticks_remaining == 0) {
	gettimeofday (&now, NULL);
	elapsed = (((now.tv_sec * 1000) + (now.tv_usec / 1000)) -
		   ((ticks_lasttime.tv_sec * 1000) + (ticks_lasttime.tv_usec / 1000)));
	if (elapsed > ticks_interval) {
	    ticks_remaining = elapsed / ticks_interval;
	    /* XXX this isn't completely accurate */
	    ticks_lasttime = now;
	}
    }

    return ticks_remaining;
}

static void ng_done_tick ()
{
    --ticks_remaining;
    if (ticks_remaining < 0)
	error ("internal error: ticks_remaining less than zero\n");
}

static void ng_shutdown_ticks ()
{
}


/* The main loop of the game server. */

void ng_game_server ()
{
    while (1) {
	yield ();
	ng_check_new_connections ();
	ng_poll_clients ();
	ng_remove_stale_clients ();
	ng_poll_interface ();

	switch (srv_state) {

	    case SRV_STATE_LOBBY:
		break;

	    case SRV_STATE_LOBBY_TO_GAME:
		srv_log ("Entering game");
		if (ng_init_game_state () < 0)
		    srv_state = SRV_STATE_LOBBY;
		else {
		    ng_perform_mass_game_state_feed ();
		    ng_init_ticks ();
		    srv_state = SRV_STATE_GAME;
		}
		break;

	    case SRV_STATE_LOBBY_TO_QUIT:
		srv_state = SRV_STATE_QUIT;
		break;

	    case SRV_STATE_GAME:
		while (ng_update_ticks ()) {
		    ng_perform_physics ();
		    ng_send_game_updates ();
		    ng_done_tick ();
		}
		break;

	    case SRV_STATE_FEEDNEWCLIENT:
		srv_log ("Feeding new client %s", client_to_feed->name);
		ng_perform_single_game_state_feed (client_to_feed);

		/* XXX temp */
		{
		    object_t *obj;
		    start_t *start;
		    clt_t *c = client_to_feed;

		    start = map_start_list (map)->next;
		    obj = object_create_ex ("player", clt_object_id (c), 1);
		    object_set_xy (obj, map_start_x (start), map_start_y (start));
		    map_link_object_bottom (map, obj);

		    clt_broadcast_rdm_encode ("ccslff", MSG_SC_GAMEINFO,
					  MSG_SC_GAMEINFO_OBJECTCREATE,
					  objtype_name (object_type (obj)),
					  object_id (obj), object_x (obj), object_y (obj));
		}

		clt_set_state (client_to_feed, CLT_STATE_JOINED);
		client_to_feed = NULL;
		srv_state = SRV_STATE_GAME;
		break;

	    case SRV_STATE_GAME_TO_GAME:
		srv_log ("Leaving game");
		ng_shutdown_ticks ();
		ng_shutdown_game_state ();
		clt_broadcast_rdm_byte (MSG_SC_LOBBY);
		srv_state = SRV_STATE_LOBBY_TO_GAME;
		break;

	    case SRV_STATE_GAME_TO_LOBBY:
		/* Let's all go to the lobby,
		   Let's all go to the lobby,
		   Let's all go to the lobbiie,
		   Get ourselves some snacks. */
		srv_log ("Leaving game");
		ng_shutdown_ticks ();
		ng_shutdown_game_state ();
		clt_broadcast_rdm_byte (MSG_SC_LOBBY);
		srv_state = SRV_STATE_LOBBY;
		break;

	    case SRV_STATE_GAME_TO_QUIT:
		/* XXX too much duplicated */
		srv_log ("Leaving game");
		ng_shutdown_ticks ();
		ng_shutdown_game_state ();
		srv_state = SRV_STATE_QUIT;
		break;

	    case SRV_STATE_QUIT:
		ng_finalise ();
		srv_log ("Quitting");
		return;
	}
    }
}


/* Initialisation and shutdown. */

int ng_game_server_init (game_server_interface_t *iface)
{
    listen = net_openconn (NET_DRIVER_SOCKETS, "");
    if (!listen)
	return -1;
    net_listen (listen);

    interface = iface;
    interface->init ();

    clt_init ();
    client_to_feed = NULL;

    string_init (current_map_file);
    string_init (next_map_file);
    string_set (current_map_file, "test.pit");
    string_set (next_map_file,  "test.pit");
    srv_state = SRV_STATE_LOBBY;

    return 0;    
}

void ng_game_server_shutdown ()
{
    string_free (next_map_file);
    string_free (current_map_file);
    clt_shutdown ();
    interface->shutdown ();
    net_closeconn (listen);
}
