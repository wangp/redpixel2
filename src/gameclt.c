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
#include "render.h"
#include "store.h"
#include "timeout.h"
#include "yield.h"


#if 0
# define dbg(msg)	puts ("[client] " msg)
#else
# define dbg(msg)
#endif


/*----------------------------------------------------------------------*/


static NET_CONN *conn;
static int client_id;

static map_t *map;
static BITMAP *bmp;
static camera_t *cam;

#define local_object_id	(-client_id)

static int last_controls;

static int backgrounded;


/*----------------------------------------------------------------------*/


static void net_send_rdm_byte (NET_CONN *conn, unsigned char c)
{
    net_send_rdm (conn, &c, 1);
}


/*----------------------------------------------------------------------*/


static void send_gameinfo_controls ()
{
    int controls = 0;

    if (key[KEY_LEFT])
	controls |= 0x01;
    if (key[KEY_RIGHT])
	controls |= 0x02;
    if (key[KEY_UP])
	controls |= 0x04;

    if (controls != last_controls) {
	char buf[] = { MSG_CS_GAMEINFO, MSG_CS_GAMEINFO_CONTROLS, controls };
	net_send_rdm (conn, buf, sizeof buf);
	last_controls = controls;
    }
}


/*----------------------------------------------------------------------*/


static void process_sc_gameinfo_packet (const unsigned char *buf, int size)
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
		map = map_load (filename, 0, NULL);
		break;
	    }

	    case MSG_SC_GAMEINFO_OBJECTCREATE:
	    {
		char type[NET_MAX_PACKET_SIZE];
		long len;
		objid_t id;
		float x, y;
		object_t *obj;

		buf += packet_decode (buf, "slff", &len, type, &id, &x, &y);
		obj = object_create_ex (type, id);
		object_set_xy (obj, x, y);
		map_link_object (map, obj);
		break;
	    }

	    case MSG_SC_GAMEINFO_OBJECTDESTROY:
	    {
		objid_t id;
		object_t *obj;

		buf += packet_decode (buf, "l", &id);
		if ((obj = map_find_object (map, id))) {
		    map_unlink_object (obj);
		    object_destroy (obj);
		}
		break;
	    }

	    case MSG_SC_GAMEINFO_OBJECTMOVE:
	    {
		objid_t id;
		float x, y;
		object_t *obj;

		buf += packet_decode (buf, "lff", &id, &x, &y);
		if ((obj = map_find_object (map, id)))
		    object_set_xy (obj, x, y);
		break;
	    }

	    default:
		error ("error: unknown code in gameinfo packet (client)\n");
	}
    }
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


static void update_screen ()
{
    object_t *obj;

    obj = map_find_object (map, local_object_id);
    if (obj)
	camera_track_object_with_mouse (cam, obj, mouse_x, mouse_y, 80);
    
    if (!backgrounded) {
	clear (bmp);
	render (bmp, map, cam);

	if (obj)
	    pivot_trans_magic_sprite (bmp, store_dat ("/player/torch"),
				      object_x (obj) - camera_x (cam),
				      object_y (obj) - camera_y (cam), 0, 36,
				      fatan2 (mouse_y - 100, mouse_x - 160));

	text_mode (-1);
	trans_textprintf (bmp, font, 0, 0, makecol24 (0x88, 0x88, 0xf8),
			  "%d FPS", fps);
	scare_mouse ();
	acquire_screen ();
	blit_magic_format (bmp, screen, SCREEN_W, SCREEN_H);
	release_screen ();
	unscare_mouse ();
    }

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


void game_client ()
{

  lobby:
    
    dbg ("lobby");
    {
	char buf[NET_MAX_PACKET_SIZE];
	int size;

	while (1) {
	    size = net_receive_rdm (conn, buf, sizeof buf);
	    if (size <= 0) {
		yield ();
		continue;
	    }

	    switch (buf[0]) {
		case MSG_SC_POST_JOIN:
		    packet_decode (buf+1, "l", &client_id);
		    break;

		case MSG_SC_GAMESTATEFEED_REQ:
		    net_send_rdm_byte (conn, MSG_CS_GAMESTATEFEED_ACK);
		    goto receive_game_state;

		case MSG_SC_DISCONNECTED:
		    goto end;
	    }
	}
    }

  receive_game_state:

    dbg ("receive game state");
    {
	char buf[NET_MAX_PACKET_SIZE];
	int size;

	while (1) {
	    size = net_receive_rdm (conn, buf, sizeof buf);
	    if (size <= 0) {
		yield ();
		continue;
	    }

	    switch (buf[0]) {
		case MSG_SC_GAMEINFO:
		    process_sc_gameinfo_packet (buf+1, size-1);
		    break;

		case MSG_SC_GAMEINFO_DONE:
		    goto pause;

		case MSG_SC_DISCONNECTED:
		    goto end;
	    }
	}
    }
	
  pause:
        
    dbg ("pause");
    {
	unsigned char c;

	while (1) {
	    if (net_receive_rdm (conn, &c, 1) <= 0) {
		yield ();
		continue;
	    }

	    switch (c) {
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
	while (1) {
	    if (key[KEY_Q])
		goto disconnect;
    
	    dbg ("send gameinfo");
	    send_gameinfo_controls ();

	    dbg ("send gameinfo_done");
	    net_send_rdm_byte (conn, MSG_CS_GAMEINFO_DONE);

	    dbg ("receive game updates");
	    {
		char buf[NET_MAX_PACKET_SIZE];
		int size;
		int done = 0;
		int pause_later = 0;
      
		while (!done) {
		    size = net_receive_rdm (conn, buf, sizeof buf);
		    if (size <= 0) {
			if (pause_later)
			    goto pause;
			yield ();
			continue;
		    }

		    switch (buf[0]) {
			case MSG_SC_PAUSE:
			    pause_later = 1;
			    break;

			case MSG_SC_GAMEINFO:
			    process_sc_gameinfo_packet (buf+1, size-1);
			    break;

			case MSG_SC_GAMEINFO_DONE:
			    done = 1;
			    break;

			case MSG_SC_LOBBY:
			    goto lobby;

			case MSG_SC_DISCONNECTED:
			    goto end;
		    }
		}
	    }

	    dbg ("update screen");
	    update_screen ();
	}
    }
    
  disconnect:

    dbg ("disconnect");
    {
	timeout_t timeout;
	unsigned char c;

	net_send_rdm_byte (conn, MSG_CS_DISCONNECT_ASK);

	timeout_set (&timeout, 5000);

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


int game_client_init (const char *addr)
{
    int status;

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

    bmp = create_magic_bitmap (SCREEN_W, SCREEN_H);
    cam = camera_create (SCREEN_W, SCREEN_H);
    map = NULL;
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
    map_destroy (map);
    camera_destroy (cam);
    destroy_bitmap (bmp);
    net_closeconn (conn);
}
