/* gameclt.c
 *
 * Peter Wang <tjaden@users.sourceforge.net>
 */


#include <allegro.h>
#include <libnet.h>
#include "camera.h"
#include "error.h"
#include "fps.h"
#include "gameclt.h"
#include "list.h"
#include "magic4x4.h"
#include "magicrt.h"
#include "map.h"
#include "mapfile.h"
#include "netmsg.h"
#include "object.h"
#include "packet.h"
#include "physics.h"
#include "render.h"
#include "store.h"
#include "timeout.h"
#include "yield.h"


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
#define local_object_id	(-client_id)

/* for rendering */
static BITMAP *bmp;
static camera_t *cam;

static BITMAP *bkgd;
static int parallax_x = 2;
static int parallax_y = 2;

static BITMAP *crosshair;

/* the game state */
static map_t *map;
static physics_t *physics;
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
    int i;
    
    object_list = map_object_list (map);
    list_for_each (obj, object_list) {
	for (i = 0; i < 1 + object_catchup (obj); i++)
	    physics_interpolate_proxy (physics, obj);
	object_set_catchup (obj, 0);
    }
}


/*----------------------------------------------------------------------*/


static void send_gameinfo_controls ()
{
    uchar_t buf[NET_MAX_PACKET_SIZE];
    int size;
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
	object_set_number (local_object, "aim_angle", aim_angle);
    }

    if (update) {
	size = packet_encode (buf, "cccf", MSG_CS_GAMEINFO,
			      MSG_CS_GAMEINFO_CONTROLS, controls, aim_angle);
	net_send_rdm (conn, buf, size);

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
		if (physics)
		    physics_destroy (physics);
		if (map)
		    map_destroy (map);
		map = map_load (filename, 0, NULL);
		physics = physics_create (NULL);
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

		buf += packet_decode (buf, "slffffl", &len, type, &id, &x, &y, &xv, &yv, &ctag);
		obj = object_create_proxy (type, id);
		object_set_xy (obj, x, y);
		object_set_xvyv (obj, xv, yv);
		object_set_collision_tag (obj, ctag);
		if (id == local_object_id) {
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
		object_t *obj;

		buf += packet_decode (buf, "lffff", &id, &x, &y, &xv, &yv);
		if ((obj = map_find_object (map, id))) {
		    object_set_xy (obj, x, y);
		    object_set_xvyv (obj, xv, yv);
		    object_set_catchup (obj, lag);
		}
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
    clear (tmp);
    textout (tmp, font, buf, x, y, color);
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
    
    clear (bmp);

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
    dbg ("connecting (stage 2)");
    {
	uchar_t buf[NET_MAX_PACKET_SIZE];

	while (1) {
	    if (net_receive_rdm (conn, buf, sizeof buf) <= 0) {
		yield ();		
		continue;
	    }

	    switch (buf[0]) {
		case MSG_SC_JOININFO:
		    packet_decode (buf+1, "l", &client_id);
		    net_send_rdm_encode (conn, "ccs", MSG_CS_JOININFO,
					 NETWORK_PROTOCOL_VERSION, client_name);
		    goto lobby;

		case MSG_SC_DISCONNECTED:
		    goto end;
	    }
	}
    }
    
  lobby:
    
    dbg ("lobby");
    {
	uchar_t buf[NET_MAX_PACKET_SIZE];

	while (1) {
	    if (key[KEY_Q])
		goto disconnect;

	    if (net_receive_rdm (conn, buf, sizeof buf) <= 0) {
		yield ();		
		continue;
	    }

	    switch (buf[0]) {
		case MSG_SC_GAMESTATEFEED_REQ:
		    goto receive_game_state;

		case MSG_SC_DISCONNECTED:
		    goto end;
	    }
	}
    }

  receive_game_state:

    dbg ("receive game state");
    {
	uchar_t buf[NET_MAX_PACKET_SIZE];
	int size;

	net_send_rdm_byte (conn, MSG_CS_GAMESTATEFEED_ACK);

	while (1) {
	    if (key[KEY_Q])
		goto disconnect;

	    size = net_receive_rdm (conn, buf, sizeof buf);
	    if (size <= 0) {
		yield ();
		continue;
	    }

	    switch (buf[0]) {
		case MSG_SC_GAMEINFO:
		    process_sc_gameinfo_packet (buf+1, size-1);
		    break;

		case MSG_SC_GAMESTATEFEED_DONE:
		    goto pause;

		case MSG_SC_DISCONNECTED:
		    goto end;
	    }
	}
    }
	
  pause:
        
    dbg ("pause");
    {
	uchar_t buf[NET_MAX_PACKET_SIZE];
	int size;

	while (1) {
	    if (key[KEY_Q])
		goto disconnect;

	    size = net_receive_rdm (conn, buf, sizeof buf);
	    if (size <= 0) {
		yield ();
		continue;
	    }

	    switch (buf[0]) {
		case MSG_SC_GAMESTATEFEED_REQ:
		    goto receive_game_state;

		case MSG_SC_GAMEINFO:
		    process_sc_gameinfo_packet (buf+1, size-1);
		    break;

		case MSG_SC_RESUME:
		    goto game;

		case MSG_SC_DISCONNECTED:
		    goto end;
	    }
	}
    }

  game:
    
    dbg ("game");
    {
	ulong_t last_ticks = 0;
	int redraw = 1;

	ticks_init ();

	pinging = 0;
	last_ping_time = 0;

	while (1) {
	    if (key[KEY_Q])
		goto disconnect;

	    if (ticks != last_ticks) {
		dbg ("send gameinfo");
		send_gameinfo_controls ();
	    }
    
	    while (ticks != last_ticks) {
		dbg ("perform simple physics");
		perform_simple_physics ();
		last_ticks++;
		redraw = 1;
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
			    redraw = 1;
			    break;

			case MSG_SC_LOBBY:
			    lobby_later = 1;
			    break;

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

		if (receive_game_state_later) goto receive_game_state;
		if (pause_later) goto pause;
		if (lobby_later) goto lobby;
		if (end_later) goto end;
	    }

	    dbg ("handling pinging");
	    if ((!pinging) && (ticks > last_ping_time + (2 * TICKS_PER_SECOND))) {
		pinging = 1;
		last_ping_time = ticks;
		net_send_rdm_byte (conn, MSG_CS_PING);
	    }

	    if (local_object)
		object_call (local_object, "_client_update_hook");

	    map_destroy_stale_objects (map);

	    if (update_camera ())
		redraw = 1;

	    if (redraw) {
		dbg ("update screen");
		update_screen ();
		redraw = 0;
	    }

	    yield ();
	}

	ticks_shutdown ();
    }
    
  disconnect:

    dbg ("disconnect");
    {
	timeout_t timeout;
	uchar_t c;

	net_send_rdm_byte (conn, MSG_CS_DISCONNECT_ASK);

	timeout_set (&timeout, 2000);

	while (!timeout_test (&timeout)) {
	    if (net_receive_rdm (conn, &c, 1) > 0) 
		if (c == MSG_SC_DISCONNECTED) {
		    dbg ("server confirmed disconnect");
		    break;
		}
	    yield ();
	}

	goto end;
    }

  end:

    dbg ("end");
}


/*----------------------------------------------------------------------*/


int game_client_init (const char *name, const char *addr)
{
    int status;

    dbg ("connecting (stage 1)");
    
    if (!(conn = net_openconn (NET_DRIVER_SOCKETS, NULL)))
	return -1;

    net_connect (conn, addr);
    while (!(status = net_poll_connect (conn))) {
	if (key[KEY_Q])
	    goto error;
	yield ();
    }

    if (status < 1)
	goto error;

    dbg ("connected (stage 1)");
    
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
    physics = NULL;
    local_object = NULL;

    fps_init ();

    set_display_switch_callback (SWITCH_IN, switch_in_callback);
    set_display_switch_callback (SWITCH_OUT, switch_out_callback);
    backgrounded = 0;

    last_controls = 0;

    return 0;

  error:

    net_closeconn (conn);
    return -1;
}


void game_client_shutdown ()
{
    remove_display_switch_callback (switch_out_callback);
    remove_display_switch_callback (switch_in_callback);
    fps_shutdown ();
    if (physics) {
	physics_destroy (physics);
	physics = NULL;
    }
    if (map) {
	map_destroy (map);
	map = NULL;
    }
    destroy_bitmap (crosshair);
    destroy_bitmap (bkgd);
    camera_destroy (cam);
    destroy_bitmap (bmp);
    free (client_name);
    net_closeconn (conn);
}
