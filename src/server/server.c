/* server.c
 *
 * Peter Wang <tjaden@users.sourceforge.net>
 */


#include <string.h>
#include <allegro.h>
#include "libnet.h"
#include "error.h"
#include "list.h"
#include "netmsg.h"
#include "object.h"
#include "packet.h"
#include "server.h"
#include "svclient.h"
#include "svgame.h"
#include "svintern.h"
#include "svlobby.h"
#include "sync.h"


static server_interface_t *interface;
static NET_CONN *listen;

static server_state_t curr_state;
static server_state_t next_state;

string_t server_current_map_file;
string_t server_next_map_file;

/* If we are running as a client-server, we don't want two copies of
 * every text message (one for client, one for server).  */
static int inhibit_double_message;

/* Client-server mode: if this client quits, the server quits too.  */
static int client_to_quit_with;



/*
 *----------------------------------------------------------------------
 *	Helper for logging things
 *----------------------------------------------------------------------
 */


void server_log (const char *fmt, ...)
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



/*
 *----------------------------------------------------------------------
 *	Process incoming network traffic from clients
 *----------------------------------------------------------------------
 */


static void poll_client_joining (svclient_t *c)
{
    char buf[NETWORK_MAX_PACKET_SIZE];
    char name[NETWORK_MAX_PACKET_SIZE];
    short len;
    char version;

    if (svclient_receive_rdm (c, buf, sizeof buf) <= 0)
	return;
	    
    switch (buf[0]) {

	case MSG_CS_JOININFO:
	    packet_decode (buf+1, "cs", &version, &len, &name);
	    svclient_set_name (c, name);
	    if (version != NETWORK_PROTOCOL_VERSION) {
		svclient_send_rdm_byte (c, MSG_SC_DISCONNECTED);
		svclient_set_state (c, SVCLIENT_STATE_STALE);
		server_log ("Client %s joined but was disconnected for "
			    "compatibility reasons", c->name);
	    }
	    else {
		svclient_t *other;

		/* Give this client a face icon.  */
		{
		    lua_State *L = server_lua_namespace;
		    const char *s;

		    lua_getglobal (L, "_internal_get_random_face");
		    if (!lua_isfunction (L, -1))
			error ("Missing _internal_get_random_face\n");
		    lua_call (L, 0, 1);

		    if (!lua_isstring (L, -1))
			error ("Expecting string return type from _internal_get_random_face\n");
		    s = lua_tostring (L, -1);

		    svclient_set_face_icon (c, s);

		    lua_pop (L, 1);
		}

		/* Tell existing clients about this new client.  */
		for_each_svclient (other)
		    if (other->state == SVCLIENT_STATE_JOINED)
			svclient_send_rdm_encode (
			    other, "clsss", MSG_SC_CLIENT_ADD,
			    c->id, c->name, c->face_icon, c->score);

		svclient_set_state (c, SVCLIENT_STATE_JOINED);

		/* Tell the new client about existing clients.  */
		for_each_svclient (other)
		    if (other->state == SVCLIENT_STATE_JOINED)
			svclient_send_rdm_encode (
			    c, "clsss", MSG_SC_CLIENT_ADD,
			    other->id, other->name, other->face_icon,
			    other->score);

		if (curr_state == SERVER_STATE_GAME)
		    svclient_set_wantfeed (c);
		server_log ("Client %s joined", c->name);
	    }
	    break;

	case MSG_CS_DISCONNECT_ASK:
	    svclient_send_rdm_byte (c, MSG_SC_DISCONNECTED);
	    svclient_set_state (c, SVCLIENT_STATE_STALE);
	    server_log ("Client %s was disconnected by request", c->name);
	    break;
    }
}


static void poll_client_joined (svclient_t *c)
{
    char buf[NETWORK_MAX_PACKET_SIZE];
    size_t size;

    size = svclient_receive_rdm (c, buf, sizeof buf);
    if (size <= 0)
	return;

    switch (buf[0]) {

	case MSG_CS_GAMEINFO:
	    if (curr_state == SERVER_STATE_GAME)
		svgame_process_cs_gameinfo_packet (c, buf+1, size-1);
	    break;

	case MSG_CS_TEXT: {
	    short len;
	    char text[NETWORK_MAX_PACKET_SIZE];
	    char tmp[NETWORK_MAX_PACKET_SIZE];

	    packet_decode (buf+1, "s", &len, text);
	    uszprintf (tmp, sizeof tmp, "<%s> %s", c->name, text);
	    svclients_broadcast_rdm_encode ("cs", MSG_SC_TEXT, tmp);
	    if (!inhibit_double_message)
		server_log (tmp);
	    break;
	}

	case MSG_CS_PING:
	    if (curr_state == SERVER_STATE_GAME)
		svgame_process_cs_ping (c);
	    break;

	case MSG_CS_BOING:
	    if (curr_state == SERVER_STATE_GAME)
		svgame_process_cs_boing (c);
	    break;

	case MSG_CS_DISCONNECT_ASK:
	    svclient_send_rdm_byte (c, MSG_SC_DISCONNECTED);
	    svclient_set_state (c, SVCLIENT_STATE_BITOFF);
	    server_log ("Client %s was disconnected by request", c->name);
	    break;
    }
}


static void poll_clients (void)
{
    svclient_t *c;

    for_each_svclient (c) switch (c->state) {

	case SVCLIENT_STATE_JOINING:
	    poll_client_joining (c);
	    break;

	case SVCLIENT_STATE_JOINED:
	    poll_client_joined (c);
	    break;

	case SVCLIENT_STATE_BITOFF:
	case SVCLIENT_STATE_STALE:
	    break;
    }
}



/*
 *----------------------------------------------------------------------
 *	Process commands from the game server interface
 *----------------------------------------------------------------------
 */


static const char *whitespace = " \t";


static void command_list (char **last)
{
    int n = svclients_count ();
    svclient_t *c;	

    server_log ("Clients: %d", n);

    for_each_svclient (c) {
	switch (c->state) {
	    case SVCLIENT_STATE_BITOFF:
	    case SVCLIENT_STATE_STALE:
		server_log ("%4d  %s (stale)", c->id, c->name);
		break;
	    default:
		if (curr_state == SERVER_STATE_GAME)
		    server_log ("%4d  %s (lag: %d)", c->id, c->name, c->lag);
		else
		    server_log ("%4d  %s", c->id, c->name);
		break;
	}
    }
}


/* XXX 'kick' needs to take an optional 'reason' argument */
static void command_kick (char **last)
{
    char *word;
    svclient_t *c;

    word = strtok_r (NULL, whitespace, last);
    if (!word) {
	server_log ("KICK requires an argument");
	return;
    }

    if (uisdigit (ugetc (word))) {
	objid_t id = strtol (word, NULL, 10);
	c = svclients_find_by_id (id);
	if (!c) {
	    server_log ("No client with id %d", id);
	    return;
	}
    }
    else {
	c = svclients_find_by_name (word);
	if (!c) {
	    server_log ("No client with name %s", word);
	    return;
	}
    }

    if ((c->state == SVCLIENT_STATE_BITOFF) || (c->state == SVCLIENT_STATE_STALE)) {
	server_log ("Client %s already disconnected", c->name);
	return;
    }

    svclient_send_rdm_byte (c, MSG_SC_DISCONNECTED);
    svclient_set_state (c, SVCLIENT_STATE_BITOFF);
    server_log ("Client %s was kicked", c->name);
}


static void command_msg (char **last)
{
    char buf[NETWORK_MAX_PACKET_SIZE];

    if (!*last) {
	server_log ("MSG requires an argument");
	return;
    }

    uszprintf (buf, sizeof buf, "[server] %s", *last);
    svclients_broadcast_rdm_encode ("cs", MSG_SC_TEXT, buf);
    if (!inhibit_double_message)
	server_log (buf);
}


static void poll_interface (void)
{
#define wordis(test)	(0 == stricmp (word, test))

    const char *cmd;
    char *copy, *word, *last;

    if ((!interface) || !(cmd = interface->poll ()))
	return;

    copy = strdup (cmd);
    word = strtok_r (copy, whitespace, &last);

    if (word) {
	if (wordis ("help") || wordis ("?")) {
	    server_log ("Commands:");
            server_log ("  MAP <filename> - select a map (effective next START command)");
            server_log ("  MAP            - display current map");
            server_log ("  START          - enter game mode");
            server_log ("  STOP           - return to the lobby");
	    /* server_log ("  RESTART        - restart game mode (with new map)"); */
            server_log ("  QUIT           - quit completely");
            server_log ("  LIST           - list clients");
            server_log ("  KICK <id|name> - forcefully disconnect a client");
            server_log ("  MSG <message>  - broadcast text message to clients");
            server_log ("  CONTEXT        - show current context");
	}

	else if (wordis ("map")) {
	    word = strtok_r (NULL, whitespace, &last);
	    if (!word) {
		server_log ("Current map: %s", server_current_map_file);
		server_log ("Selected map: %s", server_next_map_file);
	    } else {
		string_set (server_next_map_file, word);
		server_log ("Setting map to %s", server_next_map_file);

		if (!file_exists (server_next_map_file,
				  FA_RDONLY|FA_HIDDEN|FA_SYSTEM|FA_ARCH, NULL))
		    server_log ("Warning: %s doesn't exist", server_next_map_file);
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
	    command_list (&last);
	}

	else if (wordis ("kick")) {
	    command_kick (&last);
	}

	else if (wordis ("msg")) {
	    command_msg (&last);
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



/*
 *----------------------------------------------------------------------
 *	The outer loop
 *----------------------------------------------------------------------
 */


static server_state_procs_t **server_state_procs[] =
{
    /* Keep this in sync with server_state_t in server.h. */
    &svlobby_procs,
    &svgame_procs,
    NULL  /* quit */
};


static void check_new_connections (void)
{
    NET_CONN *conn;
    svclient_t *c;

    conn = net_poll_listen (listen);
    if (!conn)
	return;
    c = svclient_create (conn);
    svclient_set_state (c, SVCLIENT_STATE_JOINING);
    svclient_send_rdm_encode (c, "cl", MSG_SC_JOININFO, c->id);
}


/* Upgrade ``bit off'' clients to ``stale'' status. */
static void handle_bit_off_clients (void)
{
    svclient_t *c;

    for_each_svclient (c) {
	if (c->state != SVCLIENT_STATE_BITOFF)
	    continue;
	if ((curr_state == SERVER_STATE_GAME) && (c->client_object))
	    object_set_stale (c->client_object);
	svclient_set_state (c, SVCLIENT_STATE_STALE);
	svclients_broadcast_rdm_encode ("cl", MSG_SC_CLIENT_REMOVE, c->id);
    }
}


void server_run (void)
{
    server_state_procs_t *p = NULL;

    curr_state = -1;
    next_state = SERVER_STATE_LOBBY;

    do {
	sync_server_lock ();

	while (curr_state != next_state) {
	    p = (*server_state_procs[next_state]);
	    if ((p->init) && (p->init () < 0))
		curr_state = -1;
	    else
		curr_state = next_state;
	}

	/* Do this before p->poll so that this will set "a bit off"
	 * client objects to "stale", which game state poll can
	 * see to tell other clients to destroy the stale object.
	 */
	handle_bit_off_clients ();

	if (client_to_quit_with) {
	    svclient_t *c = svclients_find_by_id (client_to_quit_with);
	    if (c->state == SVCLIENT_STATE_STALE)
		next_state = SERVER_STATE_QUIT;
	}

	if (p->poll)
	    p->poll ();

	if (curr_state == next_state) {
	    check_new_connections ();
	    poll_clients ();
	    svclients_remove_stale ();
	    poll_interface ();
	}

	if (curr_state != next_state) {
	    if (p->shutdown)
		p->shutdown ();
	}

	sync_server_unlock ();
    } while ((next_state != SERVER_STATE_QUIT) && 
	     (!sync_server_stop_requested ()));
}



/*
 *----------------------------------------------------------------------
 *	Initialisation and shutdown
 *----------------------------------------------------------------------
 */


int server_init (server_interface_t *iface, int net_driver)
{
    listen = net_openconn (net_driver, "");
    if (!listen)
	return -1;
    net_listen (listen);

    interface = iface;
    if (interface)
	interface->init ();

    svclients_init ();

    string_init (server_current_map_file);
    string_init (server_next_map_file);
    string_set (server_current_map_file, "test.pit");
    string_set (server_next_map_file,  "test.pit");

    inhibit_double_message = 0;

    client_to_quit_with = 0;

    gettimeofday_init ();

    /* Call new session hook.  */
    {
	lua_State *L = server_lua_namespace;
	lua_getglobal (L, "_internal_new_session_hook");
	if (!lua_isfunction (L, -1))
	    error ("Missing _internal_new_session_hook\n");
	lua_getglobal (L, "Game_Type_Deathmatch"); /* hardwired for now */
	lua_call (L, 1, 0);
    }

    return 0;    
}


void server_inhibit_double_message (void)
{
    inhibit_double_message = 1;
}


void server_set_client_to_quit_with (client_id_t id)
{
    client_to_quit_with = id;
}


void server_shutdown (void)
{
    gettimeofday_shutdown ();

    server_log ("Disconnecting clients");
    svclients_broadcast_rdm_byte (MSG_SC_DISCONNECTED);
    server_log ("Quitting");

    string_free (server_next_map_file);
    string_free (server_current_map_file);

    svclients_shutdown ();

    if (interface)
	interface->shutdown ();

    net_closeconn (listen);
}



/*
 *----------------------------------------------------------------------
 *	Things for other server modules
 *----------------------------------------------------------------------
 */


void server_set_next_state (server_state_t state)
{
    next_state = state;
}


void server_interface_set_status (const char *text)
{
    if (interface)
	interface->set_status (text);
}
