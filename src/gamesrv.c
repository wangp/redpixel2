/* gamesrv.c
 *
 * Peter Wang <tjaden@users.sourceforge.net>
 */


#include <math.h>
#include <string.h>
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
#include "sync.h"
#include "timeout.h"


typedef unsigned char uchar_t;
typedef unsigned long ulong_t;

typedef char *string_t;
#define string_init(var)	(var = NULL)
#define string_set(var,str)	({ free (var); var = ustrdup (str); })
#define string_free(var)	({ free (var); var = NULL; })



static void server_log (const char *fmt, ...);



/*----------------------------------------------------------------------*/
/*			          Ticker	      			*/
/*----------------------------------------------------------------------*/

/* Keep this in sync with gameclt.c. */
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
	long x;

	ticks += elapsed_msec / MSECS_PER_TICK;
	x = elapsed_msec % MSECS_PER_TICK;
	ticks_last_update = now;
	ticks_last_update.tv_sec -= x / 1000;
	ticks_last_update.tv_usec -= x * 1000;
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
    CLIENT_STATE_JOINING,
    CLIENT_STATE_JOINED,
    CLIENT_STATE_BITOFF, /* almost stale: we need a chance to
			    broadcast that this client's game object
			    should be destroyed */
    CLIENT_STATE_STALE,  /* stale: will delete the client object */
} client_state_t;

struct client {
    client_t *next;
    client_t *prev;
    NET_CONN *conn;
    client_state_t state;
    int id;
    object_t *client_object;
    string_t name;
    int flags;
    int controls;
    float aim_angle;
    float last_sent_aim_angle;
    timeout_t timeout;
    ulong_t pong_time;
    int lag;
};

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
    if (x < 0) {
	client_set_state (c, CLIENT_STATE_BITOFF);
	server_log ("Client %s was disconnected [send error]", c->name);
    }
    return x;
}

static int client_send_rdm_byte (client_t *c, uchar_t byte)
{
    return client_send_rdm (c, &byte, 1);
}

static int client_send_rdm_encode (client_t *c, const char *fmt, ...)
{
    va_list ap;
    uchar_t buf[NETWORK_MAX_PACKET_SIZE];
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
	if ((c->state != CLIENT_STATE_BITOFF) ||
	    (c->state != CLIENT_STATE_STALE))
	    client_send_rdm (c, buf, size);
}

static void clients_broadcast_rdm_byte (uchar_t c)
{
    clients_broadcast_rdm (&c, 1);
}

static void clients_broadcast_rdm_encode (const char *fmt, ...)
{
    va_list ap;
    uchar_t buf[NETWORK_MAX_PACKET_SIZE];
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

static client_t *clients_find_by_name (const char *name)
{
    client_t *c;

    for_each_client (c)
	if (!ustricmp (c->name, name)) return c;

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
    clients_next_id = 1;
}

static void clients_shutdown ()
{
    list_free (clients, client_destroy);
}


/*----------------------------------------------------------------------*/
/*			          Game server      			*/
/*----------------------------------------------------------------------*/


static void add_to_gameinfo_packet_queue (void *packet, size_t size);


typedef enum {
    /* Keep this in sync with server_state_procs. */
    SERVER_STATE_LOBBY,
    SERVER_STATE_GAME,
    SERVER_STATE_QUIT
} server_state_t;

static server_state_t curr_state;
static server_state_t next_state;
static game_server_interface_t *interface;
static NET_CONN *listen;
static map_t *map;
static string_t current_map_file;
static string_t next_map_file;

static int single_hack = 0;
/* XXX single hack: waits for one client, then starts, and quits when
   that client leaves.  For testing the client-server */


/* Helper for logging things. */

static void server_log (const char *fmt, ...)
{
    char buf[1024];
    va_list ap;

    va_start (ap, fmt);
    uvszprintf (buf, sizeof buf, fmt, ap);
    if (interface)
	interface->add_log (NULL, buf);
    else
	puts (buf); /* XXX temp */
    va_end (ap);
}


/* Upgrade ``bit off'' clients to ``stale'' status. */

static void server_handle_bit_off_clients ()
{
    client_t *c;

    for_each_client (c) {
	if (c->state != CLIENT_STATE_BITOFF)
	    continue;
	if ((curr_state == SERVER_STATE_GAME) && (c->client_object))
	    object_set_stale (c->client_object);
	client_set_state (c, CLIENT_STATE_STALE);
    }
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
    uchar_t buf[NETWORK_MAX_PACKET_SIZE];
    char name[NETWORK_MAX_PACKET_SIZE];
    long len;
    char version;

    if (client_receive_rdm (c, buf, sizeof buf) <= 0)
	return;
	    
    switch (buf[0]) {

	case MSG_CS_JOININFO:
	    packet_decode (buf+1, "cs", &version, &len, &name);
	    client_set_name (c, name);
	    client_set_state (c, CLIENT_STATE_JOINED);
	    if (curr_state == SERVER_STATE_GAME)
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
    uchar_t buf[NETWORK_MAX_PACKET_SIZE];
    int size;

    size = client_receive_rdm (c, buf, sizeof buf);
    if (size <= 0)
	return;

    switch (buf[0]) {

	case MSG_CS_GAMEINFO:
	    if (curr_state == SERVER_STATE_GAME)
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
	    client_set_state (c, CLIENT_STATE_BITOFF);
	    server_log ("Client %s was disconnected by request", c->name);
	    break;
    }
}

static void server_poll_clients ()
{
    client_t *c;

    for_each_client (c) switch (c->state) {

	case CLIENT_STATE_JOINING:
	    server_poll_clients_state_joining (c);
	    break;

	case CLIENT_STATE_JOINED:
	    server_poll_clients_state_joined (c);
	    break;

	case CLIENT_STATE_BITOFF:
	case CLIENT_STATE_STALE:
	    break;
    }
}


/* Processing commands from the game server interface. */

static const char *whitespace = " \t";

static void poll_interface_command_list (char **last)
{
    int n = clients_count ();
    client_t *c;	

    server_log ("Clients: %d", n);

    for_each_client (c) {
	switch (c->state) {
	    case CLIENT_STATE_BITOFF:
	    case CLIENT_STATE_STALE:
		server_log ("%4d  %s (stale)", c->id, c->name);
		break;
	    default:
		server_log ("%4d  %s (lag: %d x %d msecs)", c->id, c->name, 
			    c->lag, MSECS_PER_TICK);
		break;
	}
    }
}

/* XXX 'kick' needs to take an optional 'reason' argument */
static void poll_interface_command_kick (char **last)
{
    char *word;
    client_t *c;

    word = ustrtok_r (NULL, whitespace, last);
    if (!word) {
	server_log ("KICK requires an argument");
	return;
    }

    if (uisdigit (ugetc (word))) {
	objid_t id = ustrtol (word, NULL, 10);
	c = clients_find_by_id (id);
	if (!c) {
	    server_log ("No client with id %d", id);
	    return;
	}
    }
    else {
	c = clients_find_by_name (word);
	if (!c) {
	    server_log ("No client with name %s", word);
	    return;
	}
    }

    if ((c->state == CLIENT_STATE_BITOFF) || (c->state == CLIENT_STATE_STALE)) {
	server_log ("Client %s already disconnected", c->name);
	return;
    }

    client_send_rdm_byte (c, MSG_SC_DISCONNECTED);
    client_set_state (c, CLIENT_STATE_BITOFF);
    server_log ("Client %s was kicked", c->name);
}

static void poll_interface_command_msg (char **last)
{
    if (!*last)
	server_log ("MSG requires an argument");
    else {
	clients_broadcast_rdm_encode ("cs", MSG_SC_TEXT, *last);
	server_log (*last);
    }
}

static void server_poll_interface ()
{
#define wordis(test)	(0 == ustricmp (word, test))

    const char *cmd;
    char *copy, *word, *last;

    if ((!interface) || !(cmd = interface->poll ()))
	return;

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
	    server_log ("  KICK <id|name> [reason]  - forcefully disconnect a client");
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
	    next_state = SERVER_STATE_GAME;
	}

	else if (wordis ("stop")) {
	    next_state = SERVER_STATE_LOBBY;
	}

	else if (wordis ("quit")) {
	    next_state = SERVER_STATE_QUIT;
	}

	else if (wordis ("list")) {
	    poll_interface_command_list (&last);
	}

	else if (wordis ("kick")) {
	    poll_interface_command_kick (&last);
	}

	else if (wordis ("msg")) {
	    poll_interface_command_msg (&last);
	}

	else if (wordis ("context")) {
	    server_log ((curr_state == SERVER_STATE_LOBBY) ? "In the lobby" :
			(curr_state == SERVER_STATE_GAME) ? "Playing a game" :
			"Unknown context (probably in a transition)");
	}

	else if (wordis ("yom")) {
	    server_log ("You can't use yom like that!");
	}

	else {
	    server_log ("Unrecognised command: %s", word);
	}
    }

    free (copy);

#undef wordis
}


/* Spawn an object (Lua binding). */

int game_server_spawn_object (const char *typename, float x, float y)
{
    object_t *obj;

    if (!(obj = object_create (typename)))
	return -1;

    object_set_xy (obj, x, y);
    object_set_replication_flag (obj, OBJECT_REPLICATE_CREATE);
    object_run_init_func (obj);
    map_link_object_bottom (map, obj);
    return 0;
}


/* Spawn a projectile (Lua binding). */

int game_server_spawn_projectile (const char *typename, object_t *owner, float speed)
{
    client_t *c;
    object_t *obj;
    float xv, yv;

    if (!(c = clients_find_by_id (object_id (owner))))
	return -1;

    if (!(obj = object_create (typename)))
	return -1;

    xv = speed * cos (c->aim_angle);
    yv = speed * sin (c->aim_angle);

    object_set_xy (obj, object_x (owner), object_y (owner));
    object_set_xvyv (obj, xv, yv);
    object_set_collision_tag (obj, object_collision_tag (owner));
    object_set_replication_flag (obj, OBJECT_REPLICATE_CREATE);
    object_set_number (obj, "angle", c->aim_angle);
    object_add_creation_field (obj, "angle");
    object_run_init_func (obj);
    map_link_object_bottom (map, obj);
    return 0;
}


/* Spawn some blood (Lua binding). */

int game_server_spawn_blood (float x, float y, long nparticles, float spread)
{
    char buf[NETWORK_MAX_PACKET_SIZE];
    int size;
    
    size = packet_encode (buf, "cfflf", MSG_SC_GAMEINFO_BLOOD_CREATE,
			  x, y, nparticles, spread);
    add_to_gameinfo_packet_queue (buf, size);
    return 0;
}


/* Create and free the game state. */

static start_t *server_pick_random_start ()
{
    list_head_t *list;
    start_t *start;
    int n = 0;
    
    list = map_start_list (map);
    list_for_each (start, list) n++;

    if (n > 0) {
	n = rand () % n;	/* XXX temp */
	list_for_each (start, list)
	    if (!n--) return start;
    }

    return NULL;
}

static object_t *server_spawn_player (objid_t id)
{
    start_t *start;
    object_t *obj;
    client_t *c;

    start = server_pick_random_start ();
    obj = object_create_ex ("player", id);
    object_set_xy (obj, map_start_x (start), map_start_y (start));
    object_set_collision_tag (obj, id); /* XXX temp */
    object_run_init_func (obj);
    map_link_object_bottom (map, obj);

    if ((c = clients_find_by_id (id)))
	c->last_sent_aim_angle = 0;
    
    return obj;
}

static int server_init_game_state ()
{
    client_t *c;

    map = map_load (next_map_file, 0, 0);
    if (!map) {
	server_log ("Couldn't load map %s", next_map_file);
	return -1;
    }
    string_set (current_map_file, next_map_file);

    for_each_client (c)
	if (c->state == CLIENT_STATE_JOINED)
	    c->client_object = server_spawn_player (c->id);

    return 0;
}

static void server_free_game_state ()
{
    if (map) {
	map_destroy (map);
	map = NULL;
    }
}


/* Helpers to send large gameinfo packets.  This will split a gameinfo
   packet into multiple if necessary. */

static char gameinfo_packet_buf[NETWORK_MAX_PACKET_SIZE];
static int gameinfo_packet_size;
static client_t *gameinfo_packet_dest;

static void start_gameinfo_packet (client_t *c)
{
    gameinfo_packet_buf[0] = MSG_SC_GAMEINFO;
    gameinfo_packet_size = 1;
    gameinfo_packet_dest = c;
}

static void add_to_gameinfo_packet_raw (void *buf, size_t size)
{
    if (gameinfo_packet_size + size > sizeof gameinfo_packet_buf) {
	if (gameinfo_packet_dest)
	    client_send_rdm (gameinfo_packet_dest, gameinfo_packet_buf,
			     gameinfo_packet_size);
	else
	    clients_broadcast_rdm (gameinfo_packet_buf, gameinfo_packet_size);
	gameinfo_packet_size = 1;
    }
    
    memcpy (gameinfo_packet_buf + gameinfo_packet_size, buf, size);
    gameinfo_packet_size += size;
}

static void add_to_gameinfo_packet (const char *fmt, ...)
{
    va_list ap;
    char buf[NETWORK_MAX_PACKET_SIZE];
    size_t size;
    
    va_start (ap, fmt);
    size = packet_encode_v (buf, fmt, ap);
    va_end (ap);

    add_to_gameinfo_packet_raw (buf, size);
}

static void done_gameinfo_packet ()
{
    if (gameinfo_packet_size > 1) {
	if (gameinfo_packet_dest)
	    client_send_rdm (gameinfo_packet_dest, gameinfo_packet_buf,
			     gameinfo_packet_size);
	else
	    clients_broadcast_rdm (gameinfo_packet_buf, gameinfo_packet_size);
    }
}


/* A queue for gameinfo packets.  The packets that are put in here are
 * generally generated before we are ready to send them.  */

typedef struct node {
    struct node *next;
    struct node *prev;
    void *packet;
    size_t size;
} node_t;

static list_head_t packet_queue;

static void gameinfo_packet_queue_init (void)
{
    list_init (packet_queue);
}

static void add_to_gameinfo_packet_queue (void *packet, size_t size)
{
    node_t *node = alloc (sizeof *node);
    node->packet = alloc (size);
    memcpy (node->packet, packet, size);
    node->size = size;
    list_append (packet_queue, node);
}

static void node_freer (node_t *node)
{
    free (node->packet);
    free (node);
}

static void gameinfo_packet_queue_shutdown (void)
{
    list_free (packet_queue, node_freer);
}

static void gameinfo_packet_queue_flush (void)
{
    node_t *node;

    list_for_each (node, &packet_queue)
	add_to_gameinfo_packet_raw (node->packet, node->size);

    list_free (packet_queue, node_freer);
}


/* Make a MSG_SC_GAMEINFO_OBJECT_CREATE packet. */

/* XXX lots of potention buffer overflows */
static size_t make_object_creation_packet (object_t *obj, char *buf)
{
    char *p;
    list_head_t *list;
    creation_field_t *f;
    
    p = buf + packet_encode (buf, "cslcffffc", MSG_SC_GAMEINFO_OBJECT_CREATE,
			     objtype_name (object_type (obj)), 
			     object_id (obj), object_hidden (obj),
			     object_x (obj), object_y (obj),
			     object_xv (obj), object_yv (obj),
			     object_collision_tag (obj));

    /* creation fields */
    /* XXX this only supports fields of type float right now */
    list = object_creation_fields (obj);
    list_for_each (f, list)
	p += packet_encode (p, "csf", 'f', f->name,
			    object_get_number (obj, f->name));

    p += packet_encode (p, "c", 0); /* terminator */
    return p - buf;
}


/* Feeding the game state to clients. */

static void server_feed_game_state_to (client_t *c)
{
    start_gameinfo_packet (c);

    /* map */
    add_to_gameinfo_packet ("cs", MSG_SC_GAMEINFO_MAPLOAD, current_map_file);

    /* objects */
    {
	list_head_t *list;
	object_t *obj;
	char buf[NETWORK_MAX_PACKET_SIZE];
	size_t size;

	list = map_object_list (map);
	list_for_each (obj, list) {
	    size = make_object_creation_packet (obj, buf);
	    add_to_gameinfo_packet_raw (buf, size);
	}
    }

    done_gameinfo_packet ();

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

    sync_server_unlock ();
    do {
	sync_server_lock ();

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

	sync_server_unlock ();
    } while (!done);
    sync_server_lock ();

    clients_broadcast_rdm_byte (MSG_SC_RESUME);
}

static void server_perform_single_game_state_feed (client_t *c)
{
    uchar_t byte;

    client_send_rdm_byte (c, MSG_SC_GAMESTATEFEED_REQ);

    client_set_cantimeout (c);
    client_set_timeout (c, 5);

    sync_server_unlock ();
    while (1) {
	sync_server_lock ();

	if (client_timed_out (c)) {
	    client_set_state (c, CLIENT_STATE_STALE);
	    server_log ("Client %s timed out during game state feed", c->name);
	    break;
	}

	if (client_receive_rdm (c, &byte, 1) <= 0)
	    continue;

	if (byte == MSG_CS_GAMESTATEFEED_ACK) {
	    server_feed_game_state_to (c);
	    client_set_ready (c);
	    client_clear_cantimeout (c);
	    break;
	}

	sync_server_unlock ();
    }
    sync_server_lock ();
}


/* Handling game physics. */

static void server_handle_client_controls ()
{
    client_t *c;
    object_t *obj;

    for_each_client (c) {
	if (c->state != CLIENT_STATE_JOINED)
	    continue;

	if (!(obj = c->client_object)) {
	    if (c->controls & CONTROL_RESPAWN) {
		c->client_object = server_spawn_player (c->id);
		object_set_replication_flag (c->client_object, OBJECT_REPLICATE_CREATE);
	    }
	    continue;
	}

	/* left */
	if (c->controls & CONTROL_LEFT) {
/*  	    object_set_xv (obj, object_xv (obj) - 1.4); */
/*  	    object_set_replication_flag (obj, OBJECT_REPLICATE_UPDATE); */
	    object_set_xa (obj, -2);
	}

	/* right */
	else if (c->controls & CONTROL_RIGHT) {
/*  	    object_set_xv (obj, object_xv (obj) + 1.4); */
/*  	    object_set_replication_flag (obj, OBJECT_REPLICATE_UPDATE); */
	    object_set_xa (obj, +2);
	}

	else
	    object_set_xa (obj, 0);
	    
	/* up */
	if (c->controls & CONTROL_UP) {
	    float jump = object_jump (obj);

	    if (jump > 0) {
/*  		object_set_yv (obj, object_yv (obj) - MIN (8, 20 / jump)); */
/*  		object_set_jump (obj, (jump < 10) ? (jump + 1) : 0); */
/*  		object_set_replication_flag (obj, OBJECT_REPLICATE_UPDATE); */

		/* new jump based on accel */
/*  		object_set_ya (obj, object_ya (obj) - object_mass (obj) - .7); */
		object_set_jump (obj, (jump < 10) ? (jump + 1) : 0);
		object_set_replication_flag (obj, OBJECT_REPLICATE_UPDATE); /* XXX --- maybe this can be avoided */
	    }
	    else if ((jump == 0) && (object_yv (obj) == 0) && (object_supported (obj, map))) {
/*  		object_set_yv (obj, object_yv (obj) - 4); */
/*  		object_set_jump (obj, 1); */
/*  		object_set_replication_flag (obj, OBJECT_REPLICATE_UPDATE); */

		/* new jump based on accel */
		object_set_ya (obj, object_ya (obj) - 15);
		object_set_jump (obj, 1);
		object_set_replication_flag (obj, OBJECT_REPLICATE_UPDATE);
	    }
	}
	else {
	    object_set_jump (obj, 0);
	}
	
	/* fire */
	if (c->controls & CONTROL_FIRE)
	    object_call (obj, "_fire_hook");
    }
}

static void server_perform_physics ()
{
    list_head_t *object_list;
    object_t *obj;

    server_handle_client_controls ();

    object_list = map_object_list (map);
    list_for_each (obj, object_list)
	object_do_physics (obj, map); /* XXX find better name */
}


/* Poll objects' update hooks.  */

static void server_poll_update_hooks (int elapsed_msecs)
{
    list_head_t *list;
    object_t *obj;

    list = map_object_list (map);
    list_for_each (obj, list)
	object_poll_update_hook (obj, elapsed_msecs);
}


/* Sending aim angles to clients. */

static void server_send_client_aim_angles ()
{
    client_t *c;

    for_each_client (c) {
	if (ABS (c->aim_angle - c->last_sent_aim_angle) > (M_PI/16)) {
	    add_to_gameinfo_packet ("clf", MSG_SC_GAMEINFO_CLIENT_AIM_ANGLE,
				    c->id, c->aim_angle);
	    c->last_sent_aim_angle = c->aim_angle;
	}
    }
}


/* Sending important object changes to clients. */

static void server_send_object_updates ()
{
    list_head_t *object_list;
    object_t *obj;
    char buf[NETWORK_MAX_PACKET_SIZE];
    size_t size;
    
    object_list = map_object_list (map);
    list_for_each (obj, object_list) {
	if (object_stale (obj)) {
	    add_to_gameinfo_packet ("cl", MSG_SC_GAMEINFO_OBJECT_DESTROY,
				    object_id (obj));
	    continue;
	}

	if (object_need_replication (obj, OBJECT_REPLICATE_CREATE)) {
	    size = make_object_creation_packet (obj, buf);
	    add_to_gameinfo_packet_raw (buf, size);
	}

	if (object_need_replication (obj, OBJECT_REPLICATE_UPDATE))
	    add_to_gameinfo_packet ("clffffff", MSG_SC_GAMEINFO_OBJECT_UPDATE,
				    object_id (obj), 
				    object_x (obj), object_y (obj),
				    object_xv (obj), object_yv (obj),
				    object_xa (obj), object_ya (obj));

	if (object_need_replication (obj, OBJECT_REPLICATE_HIDDEN))
	    add_to_gameinfo_packet ("clc", MSG_SC_GAMEINFO_OBJECT_HIDDEN,
				    object_id (obj), object_hidden (obj));

	object_clear_replication_flags (obj);
    }
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
	server_log ("Error initialising game state");
	next_state = SERVER_STATE_LOBBY;
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
	    obj = object_create_ex ("player", c->id);
	    object_set_xy (obj, map_start_x (start), map_start_y (start));
	    object_set_collision_tag (obj, c->id); /* XXX temp */
	    object_run_init_func (obj);
	    map_link_object_bottom (map, obj);

	    {
		char buf[NETWORK_MAX_PACKET_SIZE+1] = { MSG_SC_GAMEINFO };
		size_t size = make_object_creation_packet (obj, buf+1);
		clients_broadcast_rdm (buf, size+1);
	    }

	    c->client_object = obj;
	}

    	client_clear_wantfeed (c);
    }

    clients_broadcast_rdm_byte (MSG_SC_RESUME);
}

static void server_purge_stale_objects ()
{
    client_t *c;
    
    for_each_client (c)
	if ((c->client_object) && (object_stale (c->client_object)))
	    c->client_object = NULL;
    
    map_destroy_stale_objects (map);
}

static void server_state_game_poll ()
{
    ulong_t t = ticks;
    long dt, i;
    
    if (!ticks_poll ())
	return;
    if ((dt = ticks - t) <= 0)
	return;
    
    server_handle_wantfeeds ();

    for (i = 0; i < dt; i++)
	server_perform_physics ();

    server_poll_update_hooks (MSECS_PER_TICK * dt);

    start_gameinfo_packet (NULL);
    server_send_client_aim_angles ();
    server_send_object_updates ();
    gameinfo_packet_queue_flush ();
    done_gameinfo_packet ();

    server_purge_stale_objects ();
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
    server_state_procs_t *p = NULL;

    curr_state = -1;
    next_state = SERVER_STATE_LOBBY;

    do {
	sync_server_lock ();

	while (curr_state != next_state) {
	    p = server_state_procs + next_state;
	    if ((p->init) && (p->init () < 0))
		curr_state = -1;
	    else
		curr_state = next_state;
	}

	/* Do this before p->poll so that this will set "a bit off"
	 * client objects to "stale", which server_state_game_poll can
	 * see to tell other clients to destroy the stale object.
	 */
	server_handle_bit_off_clients ();

	if (p->poll)
	    p->poll ();

	if (curr_state == next_state) {
	    server_check_new_connections ();
	    server_poll_clients ();
	    clients_remove_stale ();
	    server_poll_interface ();
	}

	if ((single_hack) && (curr_state == next_state)) {
	    client_t *c;
	    int n = 0;
	    for_each_client (c) if (c->state == CLIENT_STATE_JOINED) n++; 

	    if (single_hack == 1) {
		if (n > 0) {
		    next_state = SERVER_STATE_GAME;
		    single_hack = 2;
		}
	    }
	    else if (single_hack == 2) {
		if (n < 1) {
		    next_state = SERVER_STATE_QUIT;
		    single_hack = 3;
		}
	    }
	}

	if (curr_state != next_state) {
	    if (p->shutdown)
		p->shutdown ();
	}

	sync_server_unlock ();
    } while ((next_state != SERVER_STATE_QUIT) && 
	     (!sync_server_stop_requested ()));
}


/* Initialisation and shutdown. */

int game_server_init (game_server_interface_t *iface, int net_driver)
{
    listen = net_openconn (net_driver, "");
    if (!listen)
	return -1;
    net_listen (listen);

    interface = iface;
    if (interface)
	interface->init ();

    clients_init ();

    gameinfo_packet_queue_init ();

    string_init (current_map_file);
    string_init (next_map_file);
    string_set (current_map_file, "test.pit");
    string_set (next_map_file,  "test.pit");

    return 0;    
}

void game_server_enable_single_hack ()
{
    single_hack = 1;
}

void game_server_shutdown ()
{
    server_log ("Disconnecting clients");
    clients_broadcast_rdm_byte (MSG_SC_DISCONNECTED);
    server_log ("Quitting");

    string_free (next_map_file);
    string_free (current_map_file);
    gameinfo_packet_queue_shutdown ();
    clients_shutdown ();
    if (interface)
	interface->shutdown ();
    net_closeconn (listen);
}
