/* gameclt.c
 *
 * Peter Wang <tjaden@users.sourceforge.net>
 */


#include <allegro.h>
#include "camera.h"
#include "comm.h"
#include "commmsg.h"
#include "fps.h"
#include "game.h"
#include "gameclt.h"
#include "list.h"
#include "magic4x4.h"
#include "magicrt.h"
#include "map.h"
#include "mapfile.h"
#include "object.h"
#include "render.h"
#include "store.h"
#include "yield.h"


static comm_client_t *comm;
static map_t *map;
static BITMAP *bmp;
static camera_t *cam;
static objid_t local_player;


static void game_client_send_input ()
{
    char buf[4];
    
    msg_put_long (buf, local_player);

    if (key[KEY_RIGHT])
	comm_client_send (comm, MSG_CS_KEY_RIGHT, buf, 4);
    if (key[KEY_LEFT])
	comm_client_send (comm, MSG_CS_KEY_LEFT, buf, 4);
    if (key[KEY_UP])
	comm_client_send (comm, MSG_CS_KEY_UP, buf, 4);
}


static void game_client_process_msg (int type, void *buf, int size)
{
    switch (type) {

	case MSG_SC_MOVE_OBJECT: {
	    object_t *obj;
	    objid_t id;
	    float x;
	    float y;
	    	    
	    /* msg_decode (buf, "lff", &id, &x, &y); */
	    id = msg_get_float (buf);
	    x = msg_get_float (buf+4);
	    y = msg_get_float (buf+8);
	    
	    obj = map_find_object (map, /*id*/ local_player);
	    if (obj)
		object_set_xy (obj, x, y);
	    break;
	}
	    
	default:
	    break;
    }
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
    clear (tmp);
    textout (tmp, font, buf, x, y, color);
    draw_trans_magic_sprite (bmp, tmp, x, y);
    destroy_bitmap (tmp);
}


static void game_client_render ()
{
    camera_track_object_with_mouse (cam, map_find_object (map, local_player), mouse_x, mouse_y, 80);

    clear (bmp);
    render (bmp, map, cam);

    {
	object_t *obj = map_find_object (map, local_player);
	pivot_trans_magic_sprite (bmp, store_dat ("/player/torch"),
				  object_x (obj) - camera_x (cam),
				  object_y (obj) - camera_y (cam), 0, 36,
				  fatan2 (mouse_y - 100, mouse_x - 160));
    }

    text_mode (-1);
    trans_textprintf (bmp, font, 0, 0, makecol24 (0x88, 0x88, 0xf8),
		      "%d FPS", fps);
    scare_mouse ();
    blit_magic_format (bmp, screen, SCREEN_W, SCREEN_H);
    unscare_mouse ();

    frames++;
}


static void game_client_loop ()
{
    while (!key[KEY_Q]) {
	game_client_send_input (comm);
	comm_client_send_over (comm);
	while (!comm_client_poll (comm, game_client_process_msg)) 
	    yield ();
	game_client_render (comm);
    }
}


static int game_client_init (const char *mapfile)
{
    bmp = create_magic_bitmap (SCREEN_W, SCREEN_H);
    cam = camera_create (SCREEN_W, SCREEN_H);

    /* This is temporary until a menu system is in place and some
       proper networking.  */

    map = map_load (mapfile, 1, NULL);
    if (!map)
	return -1;

    {
	struct list_head *list;
	start_t *start;
	object_t *obj;

	list = map_start_list (map);
	list_for_each (start, list) {
	    obj = object_create ("player");
	    object_set_xy (obj, map_start_x (start), map_start_y (start));
	    map_link_object_bottom (map, obj);
	    local_player = object_id (obj);
	    break;
	}
    }

    fps_init ();

    return 0;
}


static void game_client_shutdown ()
{
    fps_shutdown ();
    map_destroy (map);
    camera_destroy (cam);
    destroy_bitmap (bmp);
}


int game_client (const char *mapfile, const char *addr)
{
    int status;

    comm = comm_client_init ();
    if (!comm)
	return -1;

    comm_client_connect (comm, addr);
    while (1) {
	status = comm_client_poll_connect (comm);
	if ((status < 0) || (key[KEY_Q])) 
	    goto end;
	if (status > 0)
	    break;
	yield ();
    }

    if (game_client_init (mapfile) == 0) {
	game_client_loop ();
	game_client_shutdown ();
    }

  end:

    comm_client_shutdown (comm);

    return 0;
}
