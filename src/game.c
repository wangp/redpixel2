/* game.c
 *
 * Peter Wang <tjaden@users.sourceforge.net>
 */


#include <allegro.h>
#include "camera.h"
#include "fps.h"
#include "game.h"
#include "list.h"
#include "magic4x4.h"
#include "magicrt.h"
#include "map.h"
#include "mapfile.h"
#include "object.h"
#include "render.h"
#include "store.h"
#include "yield.h"


static BITMAP *bmp;
static map_t *map;
static camera_t *cam;

static objid_t local_player;

    
static int do_init (const char *filename)
{
    fps_init ();

    bmp = create_magic_bitmap (SCREEN_W, SCREEN_H);
    if (!bmp)
	return -1;

    cam = camera_create (SCREEN_W, SCREEN_H);

    /* This is temporary until a menu system is in place and some
       proper networking.  */

    map = map_load (filename, 1, NULL);
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
    
/*      show_mouse (screen); */

    /* end XXX */

    return 0;
}


static void do_shutdown ()
{
    map_destroy (map);
    camera_destroy (cam);
    destroy_bitmap (bmp);
    fps_shutdown ();
}


static void do_input ()
{
    object_t *obj;

    obj = map_find_object (map, local_player);

    if (key[KEY_RIGHT])
	object_set_xv (obj, object_xv (obj) + 1.4);
    if (key[KEY_LEFT])
	object_set_xv (obj, object_xv (obj) - 1.4);
/*      if (key[KEY_DOWN]) */
/*  	obj->yv += 1.4; */

    if (key[KEY_UP]) {
	if (object_get_number (obj, "jump") > 0) {
	    object_set_yv (obj, object_yv (obj) - MIN (8, 20 / object_get_number (obj, "jump")));
	    if (object_get_number (obj, "jump") < 10)
		object_set_number (obj, "jump", object_get_number (obj, "jump") + 1);
	    else	    
		object_set_number (obj, "jump", 0);
	}
	else if ((object_get_number (obj, "jump") == 0) && (object_yv (obj) == 0) &&
		 (object_supported_at (obj, map, object_x (obj), object_y (obj)))) {
	    object_set_yv (obj, object_yv (obj) - 4);
	    object_set_number (obj, "jump", 1);
	}
    }
    else {
	object_set_number (obj, "jump", 0);
    }

    object_call (obj, "walk_hook");

    camera_track_object_with_mouse (cam, obj, mouse_x, mouse_y, 80);
}


static void move_object_x (object_t *obj)
{
    if (object_move_x_with_ramp (obj, ((object_xv (obj) < 0) ? 3 : 4), map,
				 object_xv (obj), object_ramp (obj)) < 0)
	object_set_xv (obj, 0);

    object_set_xv (obj, object_xv (obj) * 0.75);
    if (ABS (object_xv (obj)) < 0.25)
	object_set_xv (obj, 0);
}


static void move_object_y (object_t *obj)
{
    object_set_yv (obj, object_yv (obj) + object_mass (obj));

    if (object_move (obj, ((object_yv (obj) < 0) ? 1 : 2), map, 0, object_yv (obj)) < 0) {
	object_set_yv (obj, 0);
	object_set_number (obj, "jump", 0);
    }

    object_set_yv (obj, object_yv (obj) * 0.75);
    if (ABS (object_xv (obj)) < 0.25)
	object_set_xv (obj, 0);
}


static void do_physics ()
{
    struct list_head *list;
    object_t *obj;

    list = map_object_list (map);
    list_for_each (obj, list) {
	move_object_x (obj);
	move_object_y (obj);
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


static void do_render ()
{
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


static int do_loop ()
{
    do_input ();
    do_physics ();
    do_render ();

    return key[KEY_Q];
}


int game (const char *filename)
{
    if (do_init (filename) < 0)
	return -1;

    while (do_loop () == 0)
	yield ();

    do_shutdown ();
    return 0;
}
