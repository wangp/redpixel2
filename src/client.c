/* gameclt.c
 *
 * Peter Wang <tjaden@users.sourceforge.net>
 */


#include <math.h>
#include <allegro.h>
#include <libnet.h>
#include "blood.h"
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
    uchar_t buf[NET_MAX_PACKET_SIZE];
    int size;

    va_start (ap, fmt);
    size = packet_encode_v (buf, fmt, ap);
    va_end (ap);
    return net_send_rdm (conn, buf, size);
}


/*----------------------------------------------------------------------*/


static void perform_simple_physics ()
{
    list_head_t *object_list;
    object_t *obj;
    ulong_t t = ticks;
    
    object_list = map_object_list (map);
    list_for_each (obj, object_list)
	object_do_simulation (obj, t);

    blood_particles_update (map_blood_particles (map), map);
    /* XXX no good because blood particles can only be updated at a
       regular rate */
}


/*----------------------------------------------------------------------*/


static void send_gameinfo_controls ()
{
    int controls = 0;
    int update = 0;

    if (key[KEY_A]) controls |= CONTROL_LEFT;
    if (key[KEY_D]) controls |= CONTROL_RIGHT;
    if (key[KEY_W]) controls |= CONTROL_UP;
    if (mouse_b & 1) controls |= CONTROL_FIRE;

    if (controls != last_controls)
	update = 1;

    if (last_aim_angle != aim_angle) {
	if (!update)
	    update = ((controls & CONTROL_FIRE)
		      ? (ABS (aim_angle - last_aim_angle) > (M_PI/256))
		      : (ABS (aim_angle - last_aim_angle) > (M_PI/8)));
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


/*----------------------------------------------------------------------*/


static void process_sc_gameinfo_packet (const uchar_t *buf, int size)
{
    const void *end = buf + size;

    dbg ("process gameinfo packet");
    
    while (buf != end) {

	switch (*buf++) {

	    case MSG_SC_GAMEINFO_MAPLOAD:
	    {
		char filename[NET_MAX_PACKET_SIZE];
		long len;
		
		buf += packet_decode (buf, "s", &len, filename);
		if (map)
		    map_destroy (map);
		map = map_load (filename, 1, NULL);
		break;
	    }

	    case MSG_SC_GAMEINFO_OBJECT_CREATE:
	    {
		char type[NET_MAX_PACKET_SIZE];
		long len;
		objid_t id;
		float x, y;
		float xv, yv;
		int ctag;
		object_t *obj;

		buf += packet_decode (buf, "slffffc", &len, type, &id, &x, &y, &xv, &yv, &ctag);
		obj = object_create_proxy (type, id);
		object_set_auth_info (obj, ticks - lag, x, y, xv, yv, 0, 0);
		object_set_xy (obj, x, y);
		object_set_collision_tag (obj, ctag);
		if (id == client_id) {
		    local_object = obj;
		    object_set_number (obj, "is_local", 1);
		}
		object_run_init_func (obj);
		map_link_object (map, obj);
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

	    case MSG_SC_GAMEINFO_BLOOD_CREATE:
	    {
		float x;
		float y;
		long nparticles;
		long spread;

		buf += packet_decode (buf, "ffll", &x, &y, &nparticles, &spread);
		blood_particles_spawn (map_blood_particles (map), x, y, nparticles, spread);
		break;
	    }

	    default:
		error ("error: unknown code in gameinfo packet (client)\n");
	}
    }

    dbg ("done process gameinfo packet");
}


/*----------------------------------------------------------------------*/


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

    if (!local_object)
	return 0;
    
    oldx = camera_x (cam);
    oldy = camera_y (cam);

    camera_track_object_with_mouse (cam, local_object, mouse_x, mouse_y, 96);

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
	draw_lit_magic_sprite (bmp, crosshair, mouse_x, 
			       mouse_y, 
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
	uchar_t buf[NET_MAX_PACKET_SIZE];

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
	uchar_t buf[NET_MAX_PACKET_SIZE];

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
	uchar_t buf[NET_MAX_PACKET_SIZE];
	int size;

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
	uchar_t buf[NET_MAX_PACKET_SIZE];
	int size;

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
	ticks_init ();

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
		uchar_t buf[NET_MAX_PACKET_SIZE];
		int size;
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
			    char string[NET_MAX_PACKET_SIZE];
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
	    
	    dbg ("send gameinfo");
	    send_gameinfo_controls ();

	    dbg ("do physics");
	    perform_simple_physics ();

	    if (local_object)
		object_call (local_object, "_client_update_hook");

	    map_destroy_stale_objects (map);

	    messages_poll_input ();

	    dbg ("update screen");
	    update_camera ();
	    update_screen ();
	    
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
