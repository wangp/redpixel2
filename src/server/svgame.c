/* svgame.c
 * 
 * Peter Wang <tjaden@users.sourceforge.net>
 */


#include <math.h>
#include <stdarg.h>
#include <allegro.h>
#include "alloc.h"
#include "error.h"
#include "list.h"
#include "map.h"
#include "mapfile.h"
#include "netmsg.h"
#include "object.h"
#include "objtypes.h"
#include "packet.h"
#include "sync.h"
#include "server.h"
#include "svclient.h"
#include "svintern.h"
#include "svstats.h"
#include "svticker.h"


/* Keep this in sync with the client. */
#define TICKS_PER_SECOND	(50)
#define MSECS_PER_TICK		(1000 / TICKS_PER_SECOND)


static map_t *map;
static svticker_t *ticker;



/*
 *----------------------------------------------------------------------
 *	Creating new player objects
 *----------------------------------------------------------------------
 */


static start_t *pick_random_start (void)
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


static object_t *spawn_player (objid_t id)
{
    start_t *start;
    object_t *obj;
    svclient_t *c;

    start = pick_random_start ();
    obj = object_create_ex ("player", id);
    object_set_xy (obj, map_start_x (start), map_start_y (start));
    object_set_collision_tag (obj, id); /* XXX temp */
    map_link_object (map, obj);
    object_run_init_func (obj);

    if ((c = svclients_find_by_id (id)))
	c->last_sent_aim_angle = 0;
    
    return obj;
}



/*
 *----------------------------------------------------------------------
 *	Handle packets that the main server module gives to us
 *	(network input)
 *----------------------------------------------------------------------
 */


void svgame_process_cs_gameinfo_packet (svclient_t *c, const char *buf,
					 size_t size)
{
    const void *end = buf + size;

    while (buf != end) {
	switch (*buf++) {
	    case MSG_CS_GAMEINFO_CONTROLS:
		c->old_controls = c->controls;
		buf += packet_decode (buf, "cf", &c->controls, &c->aim_angle);
		break;

	    case MSG_CS_GAMEINFO_WEAPON_SWITCH: {
		short len;
		char name[NETWORK_MAX_PACKET_SIZE];
		
		buf += packet_decode (buf, "s", &len, name);
		if (c->client_object) {
		    lua_pushstring (lua_state, name);
		    object_call (c->client_object, "switch_weapon", 1);
		}
		break;
	    }

	    default:
		error ("error: unknown code in gameinfo packet (server)\n");
	}
    }
}


void svgame_process_cs_ping (svclient_t *c)
{
    c->pong_time = svticker_ticks (ticker);
    svclient_send_rdm_byte (c, MSG_SC_PONG);
}


void svgame_process_cs_boing (svclient_t *c)
{
    c->lag = (svticker_ticks (ticker) - c->pong_time) / 2;
}



/*
 *----------------------------------------------------------------------
 *	Helpers to send large gameinfo packets
 *	This will split a gameinfo packet into multiple if necessary
 *	(network output)
 *----------------------------------------------------------------------
 */


static char gameinfo_packet_buf[NETWORK_MAX_PACKET_SIZE];
static int gameinfo_packet_size;
static svclient_t *gameinfo_packet_dest;


static void start_gameinfo_packet (svclient_t *c)
{
    gameinfo_packet_buf[0] = MSG_SC_GAMEINFO;
    gameinfo_packet_size = 1;
    gameinfo_packet_dest = c;
}


static void add_to_gameinfo_packet_raw (void *buf, size_t size)
{
    if (gameinfo_packet_size + size > sizeof gameinfo_packet_buf) {
	if (gameinfo_packet_dest)
	    svclient_send_rdm (gameinfo_packet_dest, gameinfo_packet_buf,
			     gameinfo_packet_size);
	else
	    svclients_broadcast_rdm (gameinfo_packet_buf,
				     gameinfo_packet_size);
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


static void done_gameinfo_packet (void)
{
    if (gameinfo_packet_size > 1) {
	if (gameinfo_packet_dest)
	    svclient_send_rdm (gameinfo_packet_dest, gameinfo_packet_buf,
			     gameinfo_packet_size);
	else
	    svclients_broadcast_rdm (gameinfo_packet_buf,
				     gameinfo_packet_size);
    }
}



/*
 *----------------------------------------------------------------------
 *	Queue for gameinfo packets
 *	The packets that are put in here are generally generated
 *	before we are ready to send them (network output)
 *----------------------------------------------------------------------
 */


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



/*
 *----------------------------------------------------------------------
 *	Make a MSG_SC_GAMEINFO_OBJECT_CREATE packet (network output)
 *----------------------------------------------------------------------
 */


/* XXX lots of potention buffer overflows */
static size_t make_object_creation_packet (object_t *obj, char *buf)
{
    lua_State *L = lua_state;
    int top = lua_gettop (L);
    const char *type;
    list_head_t *list;
    creation_field_t *f;
    char *p;

    /* look up object type alias */
    type = objtype_name (object_type (obj));
    lua_getglobal (L, "object_alias");
    lua_pushstring (L, type);
    lua_rawget (L, -2);
    if (lua_isstring (L, -1))
	type = lua_tostring (L, -1);
    
    /* create the packet */
    p = buf + packet_encode (buf, "cslcffffc", MSG_SC_GAMEINFO_OBJECT_CREATE,
			     type,
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

    lua_settop (L, top);

    return p - buf;
}



/*
 *----------------------------------------------------------------------
 *	Feeding the game state to clients (network output)
 *----------------------------------------------------------------------
 */


static void feed_game_state_to (svclient_t *c)
{
    start_gameinfo_packet (c);

    /* map */
    add_to_gameinfo_packet ("cs", MSG_SC_GAMEINFO_MAPLOAD,
			    server_current_map_file);

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

    svclient_send_rdm_byte (c, MSG_SC_GAMESTATEFEED_DONE);

    /* After the svclient receives the game state, it will automatically
     * switch to 'paused' mode. */
}


static void perform_mass_game_state_feed (void)
{
    svclient_t *c;
    object_t *obj;
    int done;
    char byte;

    svclients_broadcast_rdm_byte (MSG_SC_GAMESTATEFEED_REQ);

    for_each_svclient (c) {
	if (c->state == SVCLIENT_STATE_JOINED) {
	    svclient_clear_ready (c);
	    svclient_set_cantimeout (c);
	    svclient_set_timeout (c, 5);
	}
    }

    sync_server_unlock ();
    do {
	sync_server_lock ();

	done = 1;
	for_each_svclient (c) {
	    if (c->state != SVCLIENT_STATE_JOINED)
		continue;
			    
	    if (svclient_ready (c))
		continue;

	    if (svclient_timed_out (c)) {
		svclient_set_state (c, SVCLIENT_STATE_STALE);
		server_log ("Client %s timed out during game state feed",
			    c->name);
		continue;
	    }
	    
	    if (svclient_receive_rdm (c, &byte, 1) <= 0) {
		done = 0;
		continue;
	    }

	    if (byte == MSG_CS_GAMESTATEFEED_ACK) {
		feed_game_state_to (c);
		svclient_set_ready (c);
		svclient_clear_cantimeout (c);
	    }
	}

	sync_server_unlock ();
    } while (!done);
    sync_server_lock ();

    list_for_each (obj, map_object_list (map))
	object_clear_replication_flags (obj);
	
    svclients_broadcast_rdm_byte (MSG_SC_RESUME);
}


static void perform_single_game_state_feed (svclient_t *c)
{
    char byte;

    svclient_send_rdm_byte (c, MSG_SC_GAMESTATEFEED_REQ);

    svclient_set_cantimeout (c);
    svclient_set_timeout (c, 5);

    sync_server_unlock ();
    while (1) {
	sync_server_lock ();

	if (svclient_timed_out (c)) {
	    svclient_set_state (c, SVCLIENT_STATE_STALE);
	    server_log ("Client %s timed out during game state feed", c->name);
	    break;
	}

	if (svclient_receive_rdm (c, &byte, 1) <= 0)
	    continue;

	if (byte == MSG_CS_GAMESTATEFEED_ACK) {
	    feed_game_state_to (c);
	    svclient_set_ready (c);
	    svclient_clear_cantimeout (c);
	    break;
	}

	sync_server_unlock ();
    }
    sync_server_lock ();
}



/*
 *----------------------------------------------------------------------
 *	Sending in-game state packets (network output)
 *----------------------------------------------------------------------
 */


static void send_svclient_aim_angles (void)
{
    svclient_t *c;

    for_each_svclient (c) {
	if (ABS (c->aim_angle - c->last_sent_aim_angle) > (M_PI/16)) {
	    add_to_gameinfo_packet ("clf", MSG_SC_GAMEINFO_CLIENT_AIM_ANGLE,
				    c->id, c->aim_angle);
	    c->last_sent_aim_angle = c->aim_angle;
	}
    }
}


static void send_object_updates (void)
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



/*
 *----------------------------------------------------------------------
 *	Stuff that makes the game go around
 *----------------------------------------------------------------------
 */


static void handle_svclient_controls (void)
{
    svclient_t *c;
    object_t *obj;

    for_each_svclient (c) {
	if (c->state != SVCLIENT_STATE_JOINED)
	    continue;

	if (!(obj = c->client_object)) {
	    if (c->controls & CONTROL_RESPAWN) {
		c->client_object = spawn_player (c->id);
		object_set_replication_flag (c->client_object,
					     OBJECT_REPLICATE_CREATE);
	    }
	    continue;
	}

	object_update_ladder_state (obj, map);

	/*
	 * Left and right.
	 */
	switch (c->controls & (CONTROL_LEFT | CONTROL_RIGHT)) {
	    case CONTROL_LEFT:
		object_set_xa (obj, -2);
		break;
	    case CONTROL_RIGHT:
		object_set_xa (obj, +2);
		break;
	    default:
		object_set_xa (obj, 0);
	}
	    
	/*
	 * Up and down.
	 */
	switch (c->controls & (CONTROL_UP | CONTROL_DOWN)) {

	    case CONTROL_UP:
		if (object_head_above_ladder (obj)) {
		    object_set_ya (obj, -8);
		    object_set_jump (obj, 4);
		}
		else if (object_standing_on_ladder (obj)) {
		    object_set_ya (obj, -4);
		    object_set_jump (obj, 2);
		}
		else if (object_in_ladder (obj)) {
		    object_set_ya (obj, -4);
		    object_set_jump (obj, 0);
		}
		else {
		    int jump = object_jump (obj);
		    if (jump > 0) {
			object_set_ya (obj, (object_ya (obj)
					     - object_mass (obj)
					     - 4/object_jump(obj)));
			object_set_jump (obj, (jump < 10) ? (jump + 1) : 0);
		    }
		    else if ((jump == 0) && (object_supported (obj, map))) {
			float yv = object_yv (obj);
			if ((yv >= 0.0) && (yv < 0.005))
			    object_set_jump (obj, 1);
		    }
		}
		break;

	    case CONTROL_DOWN:
		if (object_standing_on_ladder (obj) ||
		    object_head_above_ladder (obj) ||
		    object_in_ladder (obj)) {
		    object_set_number (obj, "_internal_down_ladder", 1);
		    object_set_ya (obj, 4);
		}
		break;

	    default:
		object_set_jump (obj, 0);
		object_set_number (obj, "_internal_down_ladder", 0);
		break;
	}

	/*
	 * Fire.
	 */
	if (c->controls & CONTROL_FIRE)
	    object_call (obj, "_internal_fire_hook", 0);
    }
}


static void perform_physics (void)
{
    list_head_t *object_list;
    object_t *obj;

    handle_svclient_controls ();

    object_list = map_object_list (map);
    list_for_each (obj, object_list)
	object_do_physics (obj, map); /* XXX find better name */

    map_blasts_update (map);
}


static void poll_update_hooks (int elapsed_msecs)
{
    list_head_t *list;
    object_t *obj;

    list = map_object_list (map);
    list_for_each (obj, list)
	object_poll_update_hook (obj, elapsed_msecs);
}



/*
 *----------------------------------------------------------------------
 *	Lua bindings (stuff that makes the game go around part II)
 *----------------------------------------------------------------------
 */


object_t *svgame_spawn_object (const char *typename, float x, float y)
{
    object_t *obj;

    /* For when an object's init function calls spawn_object() while
     * the map is still being loaded.  */
    if (!map)
	return NULL;

    if (!(obj = object_create (typename)))
	return NULL;

    object_set_xy (obj, x, y);
    object_set_replication_flag (obj, OBJECT_REPLICATE_CREATE);
    map_link_object (map, obj);
    object_run_init_func (obj);
    return obj;
}


object_t *svgame_spawn_projectile (const char *typename, object_t *owner,
				   float speed, float delta_angle)
{
    svclient_t *c;
    object_t *obj;
    float angle;
    float xv, yv;

    if (!(c = svclients_find_by_id (object_id (owner))))
	return NULL;

    if (!(obj = object_create (typename)))
	return NULL;

    angle = c->aim_angle + delta_angle;
    
    xv = speed * cos (angle);
    yv = speed * sin (angle);

    object_set_xy (obj, object_x (owner), object_y (owner));
    object_set_xvyv (obj, xv, yv);
    object_set_collision_tag (obj, object_collision_tag (owner));
    object_set_replication_flag (obj, OBJECT_REPLICATE_CREATE);
    object_set_number (obj, "angle", angle);
    object_add_creation_field (obj, "angle");
    map_link_object (map, obj);
    object_run_init_func (obj);
    return obj;
}


static void queue_particle_packet (char type, float x, float y,
				   long nparticles, float spread)
{
    char buf[NETWORK_MAX_PACKET_SIZE];
    size_t size;
    
    size = packet_encode (buf, "ccfflf", MSG_SC_GAMEINFO_PARTICLES_CREATE,
			  type, x, y, nparticles, spread);
    add_to_gameinfo_packet_queue (buf, size);
}

void svgame_spawn_blood (float x, float y, long nparticles, float spread)
{
    queue_particle_packet ('b', x, y, nparticles, spread);
}

void svgame_spawn_sparks (float x, float y, long nparticles, float spread)
{
    queue_particle_packet ('s', x, y, nparticles, spread);
}

void svgame_spawn_respawn_particles (float x, float y, long nparticles,
				     float spread)
{
    queue_particle_packet ('r', x, y, nparticles, spread);
}


void svgame_spawn_blod (float x, float y, long nparticles)
{
    char buf[NETWORK_MAX_PACKET_SIZE];
    size_t size;
    
    size = packet_encode (buf, "cffl", MSG_SC_GAMEINFO_BLOD_CREATE,
			  x, y, nparticles);
    add_to_gameinfo_packet_queue (buf, size);
}


void svgame_spawn_explosion (const char *name, float x, float y)
{
    char buf[NETWORK_MAX_PACKET_SIZE];
    size_t size;

    size = packet_encode (buf, "csff", MSG_SC_GAMEINFO_EXPLOSION_CREATE,
			  name, x, y);
    add_to_gameinfo_packet_queue (buf, size);
}


void svgame_spawn_blast (float x, float y, float radius, int damage)
{
    char buf[NETWORK_MAX_PACKET_SIZE];
    size_t size;

    size = packet_encode (buf, "cfffl", MSG_SC_GAMEINFO_BLAST_CREATE,
			  x, y, radius, damage);
    add_to_gameinfo_packet_queue (buf, size);

    map_blast_create (map, x, y, radius, damage, 0);
}


/* XXX we only allow a single string arg at the moment
   XXX add more as they are required, somehow */
void svgame_call_method_on_clients (object_t *obj, const char *method,
				    const char *arg)
{
    char buf[NETWORK_MAX_PACKET_SIZE];
    size_t size;

    size = packet_encode (buf, "clss", MSG_SC_GAMEINFO_OBJECT_CALL,
			  object_id (obj), method, arg);
    add_to_gameinfo_packet_queue (buf, size);
}


/* Check if an object would collide with other objects, if it were to
 * be unhidden.  */
int svgame_object_would_collide_with_objects (object_t *obj)
{
    return (!object_stale (obj) &&
	    object_collide_with_objects_raw (obj, OBJECT_MASK_MAIN, map,
					     object_x (obj), object_y (obj)));
}



/*
 *----------------------------------------------------------------------
 *	Main game loop
 *----------------------------------------------------------------------
 */


static void handle_new_svclient_feeds (void)
{
    svclient_t *c;

    for_each_svclient (c)
	if (svclient_wantfeed (c))
	    goto need_feeding;

    return;

  need_feeding:

    svclients_broadcast_rdm_byte (MSG_SC_PAUSE);

    for_each_svclient (c) if (svclient_wantfeed (c)) {
	server_log ("Feeding new client %s", c->name);
	perform_single_game_state_feed (c);

	/* XXX */
	{
	    object_t *obj;
	    start_t *start;
	    
	    start = pick_random_start ();
	    obj = object_create_ex ("player", c->id);
	    object_set_xy (obj, map_start_x (start), map_start_y (start));
	    object_set_collision_tag (obj, c->id); /* XXX temp */
	    map_link_object (map, obj);
	    object_run_init_func (obj);

	    {
		char buf[NETWORK_MAX_PACKET_SIZE+1] = { MSG_SC_GAMEINFO };
		size_t size = make_object_creation_packet (obj, buf+1);
		svclients_broadcast_rdm (buf, size+1);
	    }

	    c->client_object = obj;
	}

    	svclient_clear_wantfeed (c);
    }

    svclients_broadcast_rdm_byte (MSG_SC_RESUME);
}


static void purge_stale_objects (void)
{
    svclient_t *c;
    
    for_each_svclient (c)
	if ((c->client_object) && (object_stale (c->client_object)))
	    c->client_object = NULL;
    
    map_destroy_stale_objects (map);
}


static void svgame_poll (void)
{
    unsigned long t = svticker_ticks (ticker);
    long dt, i;

    if (!svticker_poll (ticker))
	return;
    if ((dt = svticker_ticks (ticker) - t) <= 0)
	return;
    
    handle_new_svclient_feeds ();

    for (i = 0; i < dt; i++)
	perform_physics ();

    poll_update_hooks (MSECS_PER_TICK * dt);

    start_gameinfo_packet (NULL);
    send_svclient_aim_angles ();
    send_object_updates ();
    gameinfo_packet_queue_flush ();
    done_gameinfo_packet ();

    purge_stale_objects ();

    if (svstats_poll ()) {
	char buf[1024];
	snprintf (buf, sizeof buf,
		  "Incoming %.1f, outgoing %.1f  (avg bytes per sec)",
		  svstats_average_incoming_bytes,
		  svstats_average_outgoing_bytes);
	server_interface_set_status (buf);
    }
}



/*
 *----------------------------------------------------------------------
 *	Init and shutdown
 *----------------------------------------------------------------------
 */


static int init_game_state (void)
{
    svclient_t *c;

    map = map_load (server_next_map_file, 0, 0);
    if (!map) {
	server_log ("Couldn't load map %s", server_next_map_file);
	return -1;
    }
    string_set (server_current_map_file, server_next_map_file);

    for_each_svclient (c)
	if (c->state == SVCLIENT_STATE_JOINED)
	    c->client_object = spawn_player (c->id);

    return 0;
}


static void free_game_state (void)
{
    if (map) {
	map_destroy (map);
	map = NULL;
    }
}


static int svgame_init (void)
{
    server_log ("Entering game");

    gameinfo_packet_queue_init ();

    if (init_game_state () < 0) {
	server_log ("Error initialising game state");
	server_set_next_state (SERVER_STATE_LOBBY);
	return -1;
    }

    perform_mass_game_state_feed ();

    ticker = svticker_create (TICKS_PER_SECOND);
    svstats_init ();

    return 0;
}


static void svgame_shutdown (void)
{
    server_log ("Leaving game");

    server_interface_set_status (NULL);

    svstats_shutdown ();
    svticker_destroy (ticker);

    free_game_state ();

    gameinfo_packet_queue_shutdown ();
}


server_state_procs_t the_procs =
{
    svgame_init,
    svgame_poll,
    svgame_shutdown
};

server_state_procs_t *svgame_procs = &the_procs;
