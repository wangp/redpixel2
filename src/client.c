/* gameclt.c
 *
 * Peter Wang <tjaden@users.sourceforge.net>
 */


#include <math.h>
#include <allegro.h>
#include <libnet.h>
#include "blod.h"
#include "camera.h"
#include "error.h"
#include "fps.h"
#include "gameclt.h"
#include "list.h"
#include "magic4x4.h"
#include "map.h"
#include "mapfile.h"
#include "messages.h"
#include "netmsg.h"
#include "object.h"
#include "packet.h"
#include "particle.h"
#include "render.h"
#include "store.h"
#include "sync.h"
#include "timeout.h"


#ifndef M_PI
# define M_PI   3.14159265358979323846
#endif


#if 0
# define dbg(msg)	puts ("[client] " msg)
#else
# define dbg(msg)
#endif


typedef unsigned char uchar_t;
typedef unsigned long ulong_t;


/* our connection */
static NET_CONN *conn;
static int client_id;
static char *client_name;

/* for rendering */
static BITMAP *bmp;
static camera_t *cam;

static BITMAP *bkgd;
static int parallax_x = 2;
static int parallax_y = 2;

static BITMAP *crosshair;

/* the game state */
static map_t *map;
static object_t *local_object;
static object_t *tracked_object;

/* network stuff */
static int pinging;
static ulong_t last_ping_time;
static int lag;
static int last_controls;
static float aim_angle;
static float last_aim_angle;

/* misc */
static int backgrounded;


/*----------------------------------------------------------------------*/


/* keep in sync with gamesrv.c */
#define TICKS_PER_SECOND	(50)
#define MSECS_PER_TICK		(1000 / TICKS_PER_SECOND)


static volatile ulong_t ticks;


static void ticker ()
{
    ticks++;
}

END_OF_STATIC_FUNCTION (ticker);


static void ticks_init ()
{
    LOCK_VARIABLE (ticks);
    LOCK_FUNCTION (ticker);
    install_int_ex (ticker, BPS_TO_TIMER (TICKS_PER_SECOND));
    ticks = 0;
}


static void ticks_shutdown ()
{
    remove_int (ticker);
}


/*----------------------------------------------------------------------*/


static int net_send_rdm_byte (NET_CONN *conn, uchar_t c)
{
    return net_send_rdm (conn, &c, 1);
}


static int net_send_rdm_encode (NET_CONN *conn, const char *fmt, ...)
{
    va_list ap;
    uchar_t buf[NETWORK_MAX_PACKET_SIZE];
    size_t size;

    va_start (ap, fmt);
    size = packet_encode_v (buf, fmt, ap);
    va_end (ap);
    return net_send_rdm (conn, buf, size);
}


/*----------------------------------------------------------------------*/


static void perform_simple_physics (ulong_t curr_ticks, int delta_ticks)
{
    list_head_t *object_list;
    object_t *obj;
    int i;
    
    object_list = map_object_list (map);
    list_for_each (obj, object_list) {
	if (!object_is_client_processed (obj)) {
	    object_do_simulation (obj, curr_ticks);
	    continue;
	}

	/*
	 * For CLIENT PROCESSED objects, we have to do some thinking
	 * about the physics ourselves, as if we were the server.  But
	 * we do not have accurate data to work with, so ONLY use
	 * proxy-only objects when:
	 *
	 *  (1) inaccurate results for this object don't matter, and
	 *  (2) this object will not influence other objects,
	 *	e.g. in collisions
	 *
	 * In short, this means eye-candy only.
	 */
	for (i = 0; i < delta_ticks; i++)
	    object_do_physics (obj, map);
    }

    for (i = 0; i < delta_ticks; i++)
	particles_update (map_particles (map), map);
}


/*----------------------------------------------------------------------*/


static void poll_update_hooks (int elapsed_msecs)
{
    list_head_t *object_list;
    object_t *obj;

    if (elapsed_msecs <= 0)
	return;
    
    object_list = map_object_list (map);
    list_for_each (obj, object_list)
	if (!object_stale (obj))
	    object_poll_update_hook (obj, elapsed_msecs);
}


/*----------------------------------------------------------------------*/


static void send_gameinfo_controls ()
{
    int controls = 0;
    int update = 0;

    if (key[KEY_A]) controls |= CONTROL_LEFT;
    if (key[KEY_D]) controls |= CONTROL_RIGHT;
    if (key[KEY_W]) controls |= CONTROL_UP;
    if (key[KEY_S]) controls |= CONTROL_DOWN;
    if (key[KEY_SPACE]) controls |= CONTROL_RESPAWN;
    if (mouse_b & 1) controls |= CONTROL_FIRE;

    if (controls != last_controls)
	update = 1;

    if (last_aim_angle != aim_angle) {
	if (!update)
	    update = ((controls & CONTROL_FIRE)
		      ? (ABS (aim_angle - last_aim_angle) > (M_PI/256))
		      : (ABS (aim_angle - last_aim_angle) > (M_PI/16)));
	if (local_object)
	    object_set_number (local_object, "aim_angle", aim_angle);
    }

    if (update) {
	net_send_rdm_encode (conn, "cccf", MSG_CS_GAMEINFO,
			      MSG_CS_GAMEINFO_CONTROLS, controls, aim_angle);

	last_controls = controls;
	last_aim_angle = aim_angle;
    }
}


static void send_gameinfo_weapon_switch () /* XXX stuff in the same packet as _CONTROLS? */
{
    static int last_key[10] = {0,0,0,0,0,0,0,0,0,0};
    lua_State *L = lua_state;
    int i;

    for (i = 0; i < 10; i++) {
	int K = KEY_1 + i;
	int is_down = key[K];

	if (is_down && !last_key[K]) {
	    last_key[K] = 1;
	    lua_getglobal (L, "weapon_order");
	    lua_pushnumber (L, i+1);
	    lua_rawget (L, -2);
	    if (lua_isstring (L, -1)) {
		net_send_rdm_encode (conn, "ccs", MSG_CS_GAMEINFO,
				     MSG_CS_GAMEINFO_WEAPON_SWITCH,
				     lua_tostring (L, -1));
	    }
	    lua_pop (L, 1);
	    return;
	}

	last_key[K] = is_down;
    }
}


/*----------------------------------------------------------------------*/


static void process_sc_gameinfo_packet (const uchar_t *buf, size_t size)
{
    const void *end = buf + size;

    dbg ("process gameinfo packet");
    
    while (buf != end) {

	switch (*buf++) {

	    case MSG_SC_GAMEINFO_MAPLOAD:
	    {
		char filename[NETWORK_MAX_PACKET_SIZE];
		long len;
		
		buf += packet_decode (buf, "s", &len, filename);
		if (map)
		    map_destroy (map);
		map = map_load (filename, 1, NULL);
		break;
	    }

	    case MSG_SC_GAMEINFO_OBJECT_CREATE:
	    {
		lua_State *L = lua_state;
		int top = lua_gettop (L);
		char type[NETWORK_MAX_PACKET_SIZE];
		long len;
		objid_t id;
		char hidden;
		float x, y;
		float xv, yv;
		int ctag;
		object_t *obj;
		const char *realtype;

		/* decode the start of the packet */
		buf += packet_decode (buf, "slcffffc", &len, type, &id, &hidden,
				      &x, &y, &xv, &yv, &ctag);

		/* look up the object type alias */
		lua_getglobal (L, "reverse_object_alias");
		lua_pushstring (L, type);
		lua_rawget (L, -2);
		if (lua_isstring (L, -1))
		    realtype = lua_tostring (L, -1);
		else
		    realtype = type;

		/* create proxy object */
		obj = object_create_proxy (realtype, id);
		if (!obj)
		    error ("error: unable to create a proxy object (unknown type?)");
		if (hidden)
		    object_hide (obj);
		else
		    object_show (obj);
		object_set_auth_info (obj, ticks - lag, x, y, xv, yv, 0, 0);
		object_set_xy (obj, x, y);
		object_set_collision_tag (obj, ctag);
		if (id == client_id) {
		    tracked_object = local_object = obj;
		    object_set_number (obj, "is_local", 1);
		}

		/* decode optional extra fields */
		{
		    char type;
		    char name[NETWORK_MAX_PACKET_SIZE];
		    long len;
		    float f;
		    
		    do {
			buf += packet_decode (buf, "c", &type);
			if (type == 'f') {
			    buf += packet_decode (buf, "sf", &len, name, &f);
			    object_set_number (obj, name, f);
			}
			else if (type) {
			    error ("error: unknown field type in object creation packet (client)\n");
			}
		    } while (type);
		}

		/* link and init */
		map_link_object (map, obj);
		object_run_init_func (obj);

		/* hack to get the camera to track a player's corpse */
		if (object_get_number (obj, "_internal_stalk_me") == client_id)
		    tracked_object = obj;

		lua_settop (L, top);
		break;
	    }

	    case MSG_SC_GAMEINFO_OBJECT_DESTROY:
	    {
		objid_t id;
		object_t *obj;

		buf += packet_decode (buf, "l", &id);
		if ((obj = map_find_object (map, id))) {
		    object_set_stale (obj);
		    if (obj == local_object)
			local_object = NULL;
		    if (obj == tracked_object)
			tracked_object = NULL;
		}
		break;
	    }

	    case MSG_SC_GAMEINFO_OBJECT_UPDATE:
	    {
		objid_t id;
		float x, y;
		float xv, yv;
		float xa, ya;
		object_t *obj;

		buf += packet_decode (buf, "lffffff", &id, &x, &y, &xv, &yv, &xa, &ya);
		if ((obj = map_find_object (map, id)))
		    object_set_auth_info (obj, ticks - lag, x, y, xv, yv, xa, ya);
		break;
	    }

	    case MSG_SC_GAMEINFO_OBJECT_HIDDEN:
	    {
		objid_t id;
		char hidden;
		object_t *obj;

		buf += packet_decode (buf, "lc", &id, &hidden);
		if ((obj = map_find_object (map, id))) {
		    if (hidden)
			object_hide (obj);
		    else
			object_show (obj);
		}
		break;
	    }

	    case MSG_SC_GAMEINFO_OBJECT_CALL:
	    {
		long id;
		long method_len;
		char method[NETWORK_MAX_PACKET_SIZE];
		long arg_len;
		char arg[NETWORK_MAX_PACKET_SIZE];
		object_t *obj;
		
		buf += packet_decode (buf, "lss", &id, &method_len, method, &arg_len, arg);
		if ((obj = map_find_object (map, id))) {
		    lua_pushstring (lua_state, arg);
		    object_call (obj, method, 1);
		}
		break;
	    }

	    case MSG_SC_GAMEINFO_CLIENT_AIM_ANGLE:
	    {
		long id;
		float angle;
		object_t *obj;

		buf += packet_decode (buf, "lf", &id, &angle);
		if ((id != client_id) && (obj = map_find_object (map, id)))
		    object_set_number (obj, "aim_angle", angle);
		break;
	    }

	    case MSG_SC_GAMEINFO_PARTICLES_CREATE:
	    {
		char type;
		float x;
		float y;
		long nparticles;
		float spread;

		buf += packet_decode (buf, "cfflf", &type, &x, &y, &nparticles, &spread);
		switch (type) {
		    case 'b':
			particles_spawn_blood (map_particles (map), x, y, nparticles, spread);
			break;
		    case 's':
			particles_spawn_spark (map_particles (map), x, y, nparticles, spread);
			break;
		    default:
			error ("error: unknown particle type in gameinfo packet (client)\n");
		}
		break;
	    }

	    case MSG_SC_GAMEINFO_BLOD_CREATE:
	    {
		float x;
		float y;
		long nparticles;
		
		buf += packet_decode (buf, "ffl", &x, &y, &nparticles);
		blod_spawn (map, x, y, nparticles);
		break;
	    }

	    default:
		error ("error: unknown code in gameinfo packet (client)\n");
	}
    }

    dbg ("done process gameinfo packet");
}


/*----------------------------------------------------------------------*/


void game_client_set_camera (int pushable, int max_dist)
{
    camera_set (cam, pushable, max_dist);
}


static void trans_textprintf (BITMAP *bmp, FONT *font, int x, int y,
			      int color, const char *fmt, ...)
{
    va_list ap;
    BITMAP *tmp;
    char buf[1024];

    va_start (ap, fmt);
    uvszprintf (buf, sizeof buf, fmt, ap);
    va_end (ap);
    
    tmp = create_magic_bitmap (text_length (font, buf), text_height (font));
    clear_bitmap (tmp);
    textout (tmp, font, buf, 0, 0, color);
    draw_trans_magic_sprite (bmp, tmp, x, y);
    destroy_bitmap (tmp);
}


static int update_camera ()
{
    int oldx, oldy;

    if (backgrounded)
	return 0;

    if (!tracked_object)
	return 0;
    
    oldx = camera_x (cam);
    oldy = camera_y (cam);

    camera_track_object_with_mouse (cam, tracked_object, mouse_x, mouse_y);

    return (oldx != camera_x (cam)) || (oldy != camera_y (cam));
}


static void update_screen ()
{
    if (backgrounded)
	return;
    
    clear_bitmap (bmp);

    if (bkgd) {
	int x = -(camera_x (cam) / (3*parallax_x));
	int y = -(camera_y (cam) / parallax_y);
	int w = bkgd->w;
	int h = bkgd->h;
	
	blit (bkgd, bmp, 0, 0, x, y, w, h);
	blit (bkgd, bmp, 0, 0, x, y+h, w, h);
	blit (bkgd, bmp, 0, 0, x+w, y, w, h);
	blit (bkgd, bmp, 0, 0, x+w, y+h, w, h);
    }

    render (bmp, map, cam);

    if (local_object) {
	int x = object_x (local_object) - camera_x (cam);
	int y = object_y (local_object) - camera_y (cam);
	
	aim_angle = atan2 (mouse_y - y, mouse_x - x);

 	pivot_trans_magic_sprite (bmp, store_dat ("/basic/player/torch"),
 				  x, y, 0, 115/2,
 				  fatan2 (mouse_y - y, mouse_x - x));
    }

    if (crosshair) {
	draw_lit_magic_sprite (bmp, crosshair, mouse_x-2, mouse_y-2,
			       makecol24 (0xff, 0xff, 0xff));
    }

    messages_render (bmp);

    text_mode (-1);
    trans_textprintf (bmp, font, 0, 0, makecol24 (0x88, 0x88, 0xf8),
		      "%d FPS", fps);
    scare_mouse ();
    acquire_screen ();
    blit_magic_format (bmp, screen, SCREEN_W, SCREEN_H);
    release_screen ();
    unscare_mouse ();

    frames++;
}


/*----------------------------------------------------------------------*/


static void switch_in_callback ()
{
    backgrounded = 0;
}


static void switch_out_callback ()
{
    backgrounded = 1;
}


/*----------------------------------------------------------------------*/


void game_client_run ()
{
    dbg ("connecting (state 1)");
    {
	int status;

	do {
	    if (key[KEY_Q])
		return;
	    sync_client_lock ();
	    status = net_poll_connect (conn);
	    sync_client_unlock ();
	} while (!status);

	if (status < 1)
	    return;
    }

    dbg ("connecting (stage 2)");
    {
	uchar_t buf[NETWORK_MAX_PACKET_SIZE];

	while (1) {
	    sync_client_lock ();

	    if (net_receive_rdm (conn, buf, sizeof buf) <= 0) {
		sync_client_unlock ();
		continue;
	    }

	    switch (buf[0]) {
		case MSG_SC_JOININFO:
		    packet_decode (buf+1, "l", &client_id);
		    net_send_rdm_encode (conn, "ccs", MSG_CS_JOININFO,
					 NETWORK_PROTOCOL_VERSION, client_name);
		    sync_client_unlock ();
		    goto lobby;

		case MSG_SC_DISCONNECTED:
		    sync_client_unlock ();
		    goto end;
	    }

	    sync_client_unlock ();
	}
    }
    
  lobby:
    
    dbg ("lobby");
    {
	uchar_t buf[NETWORK_MAX_PACKET_SIZE];

	while (1) {
	    sync_client_lock ();

	    if (key[KEY_Q]) {
		sync_client_unlock ();
		goto disconnect;
	    }

	    if (net_receive_rdm (conn, buf, sizeof buf) <= 0) {
		sync_client_unlock ();
		continue;
	    }

	    switch (buf[0]) {
		case MSG_SC_GAMESTATEFEED_REQ:
		    sync_client_unlock ();
		    goto receive_game_state;

		case MSG_SC_DISCONNECTED:
		    sync_client_unlock ();
		    goto end;
	    }

	    sync_client_unlock ();
	}
    }

  receive_game_state:

    dbg ("receive game state");
    {
	uchar_t buf[NETWORK_MAX_PACKET_SIZE];
	size_t size;

	sync_client_lock ();
	net_send_rdm_byte (conn, MSG_CS_GAMESTATEFEED_ACK);
	sync_client_unlock ();

	while (1) {
	    sync_client_lock ();

	    if (key[KEY_Q]) {
		sync_client_unlock ();
		goto disconnect;
	    }

	    size = net_receive_rdm (conn, buf, sizeof buf);
	    if (size <= 0) {
		sync_client_unlock ();
		continue;
	    }

	    switch (buf[0]) {
		case MSG_SC_GAMEINFO:
		    process_sc_gameinfo_packet (buf+1, size-1);
		    break;

		case MSG_SC_GAMESTATEFEED_DONE:
		    dbg ("game state feed done");
		    sync_client_unlock ();
		    goto pause;

		case MSG_SC_DISCONNECTED:
		    sync_client_unlock ();
		    goto end;
	    }

	    sync_client_unlock ();
	}
    }
	
  pause:
        
    dbg ("pause");
    {
	uchar_t buf[NETWORK_MAX_PACKET_SIZE];
	size_t size;

	while (1) {
	    sync_client_lock ();
	    
	    if (key[KEY_Q]) {
		sync_client_unlock ();
		goto disconnect;
	    }

	    size = net_receive_rdm (conn, buf, sizeof buf);
	    if (size <= 0) {
		sync_client_unlock ();
		continue;
	    }

	    switch (buf[0]) {
		case MSG_SC_GAMESTATEFEED_REQ:
		    sync_client_unlock ();
		    goto receive_game_state;

		case MSG_SC_GAMEINFO:
		    process_sc_gameinfo_packet (buf+1, size-1);
		    break;

		case MSG_SC_RESUME:
		    sync_client_unlock ();
		    goto game;

		case MSG_SC_DISCONNECTED:
		    sync_client_unlock ();
		    goto end;
	    }

	    sync_client_unlock ();
	}
    }

  game:
    
    dbg ("game");
    {
	ulong_t last_ticks, t;

	ticks_init ();
	last_ticks = ticks;

	pinging = 0;
	last_ping_time = 0;

	while (1) {
	    sync_client_lock ();

	    if (key[KEY_Q]) {
		sync_client_unlock ();
		goto disconnect;
	    }

	    dbg ("process network input");
	    {
		uchar_t buf[NETWORK_MAX_PACKET_SIZE];
		size_t size;
		int receive_game_state_later = 0;
		int pause_later = 0;
		int lobby_later = 0;
		int end_later = 0;

		while ((size = net_receive_rdm (conn, buf, sizeof buf)) > 0) {
		    switch (buf[0]) {
			case MSG_SC_GAMESTATEFEED_REQ:
			    receive_game_state_later = 1;
			    break;

			case MSG_SC_PAUSE:
			    pause_later = 1;
			    break;

			case MSG_SC_RESUME:
			    pause_later = 0;
			    break;

			case MSG_SC_GAMEINFO:
			    process_sc_gameinfo_packet (buf+1, size-1);
			    break;

			case MSG_SC_LOBBY:
			    lobby_later = 1;
			    break;

			case MSG_SC_TEXT: {
			    char string[NETWORK_MAX_PACKET_SIZE];
			    long len;
			    
			    packet_decode (buf+1, "s", &len, string);
			    messages_add ("%s", string);
			    break;
			}

			case MSG_SC_PONG:
			    pinging = 0;
			    lag = (ticks - last_ping_time) / 2;
			    net_send_rdm_byte (conn, MSG_CS_BOING);
			    break;

			case MSG_SC_DISCONNECTED:
			    end_later = 1;
			    break;
		    }
		}

		if (receive_game_state_later) { sync_client_unlock (); goto receive_game_state; }
		if (pause_later) { sync_client_unlock (); goto pause; }
		if (lobby_later) { sync_client_unlock (); goto lobby; }
		if (end_later) { sync_client_unlock (); goto end; }
	    }
	    
	    t = ticks;
	    if (last_ticks != t) {
		dbg ("send gameinfo");
		send_gameinfo_controls ();
		send_gameinfo_weapon_switch ();

		dbg ("do physics");
		perform_simple_physics (t, t - last_ticks);

		dbg ("poll update hooks");
		poll_update_hooks ((t - last_ticks) * MSECS_PER_TICK);

		map_destroy_stale_objects (map);

		messages_poll_input ();

		dbg ("update camera");
		{
		    int n = t - last_ticks;
		    while ((n--) && update_camera ());
		}

		dbg ("update screen");
		update_screen ();

		last_ticks = t;
	    }
	    
	    dbg ("handling pinging");
	    if ((!pinging) && (ticks > last_ping_time + (2 * TICKS_PER_SECOND))) {
		pinging = 1;
		last_ping_time = ticks;
		net_send_rdm_byte (conn, MSG_CS_PING);
	    }

	    sync_client_unlock ();
	}

	ticks_shutdown ();
    }
    
  disconnect:

    dbg ("disconnect");
    {
	timeout_t timeout;
	uchar_t c;

	sync_client_lock ();
	net_send_rdm_byte (conn, MSG_CS_DISCONNECT_ASK);
	sync_client_unlock ();

	timeout_set (&timeout, 2000);

	while (!timeout_test (&timeout)) {
	    sync_client_lock ();
	    if (net_receive_rdm (conn, &c, 1) > 0) 
		if (c == MSG_SC_DISCONNECTED) {
		    dbg ("server confirmed disconnect");
		    sync_client_unlock ();
		    break;
		}
	    sync_client_unlock ();
	}

	goto end;
    }

  end:

    dbg ("end");
}


/*----------------------------------------------------------------------*/


int game_client_init (const char *name, int net_driver, const char *addr)
{
    if (!(conn = net_openconn (net_driver, NULL)))
	return -1;
    net_connect (conn, addr);
    
    if (messages_init () < 0) {
	net_closeconn (conn);
	return -1;
    }
    
    client_name = ustrdup (name);

    bmp = create_magic_bitmap (SCREEN_W, SCREEN_H);
    cam = camera_create (SCREEN_W, SCREEN_H);

    /* XXX */ {
	PALETTE pal;
	BITMAP *tmp;

	bkgd = load_bitmap ("data/basic/basic-bkgd/fluorescence.jpg", pal);
	if (bkgd) {
	    tmp = get_magic_bitmap_format (bkgd, pal);
	    destroy_bitmap (bkgd);
	    bkgd = tmp;
	}
    }

    crosshair = store_dat ("/basic/crosshair/000");

    map = NULL;
    local_object = NULL;
    tracked_object = NULL;

    fps_init ();

    set_display_switch_callback (SWITCH_IN, switch_in_callback);
    set_display_switch_callback (SWITCH_OUT, switch_out_callback);
    backgrounded = 0;

    last_controls = 0;

    return 0;
}


void game_client_shutdown ()
{
    remove_display_switch_callback (switch_out_callback);
    remove_display_switch_callback (switch_in_callback);
    fps_shutdown ();
    if (map) {
	map_destroy (map);
	map = NULL;
    }
    destroy_bitmap (crosshair);
    destroy_bitmap (bkgd);
    camera_destroy (cam);
    destroy_bitmap (bmp);
    free (client_name);
    messages_shutdown ();
    net_closeconn (conn);
}
