/* game.c
 *
 * Peter Wang <tjaden@psynet.net>
 */


#include <allegro.h>
#include "camera.h"
#include "fps.h"
#include "game.h"
#include "magic4x4.h"
#include "map.h"
#include "mapfile.h"
#include "render.h"


static BITMAP *bmp;
static map_t *map;
static camera_t cam;

static objid_t local_player;


static int do_init ()
{
    fps_init ();

    bmp = create_magic_bitmap (SCREEN_W, SCREEN_H);
    if (!bmp)
	return -1;

    cam.view_width = SCREEN_W;
    cam.view_height = SCREEN_H;

    /* XXX */
    /* This is temporary until a menu system is in place and some
       proper networking.  */

    map = map_load ("test.pit", 1, NULL);
    if (!map)
	return -1;

    {
	start_t *start;
	object_t *obj;

	foreach (start, map->starts) {
	    obj = object_create ("player");
	    obj->cvar.x = start->x;
	    obj->cvar.y = start->y;
	    add_to_list (map->objects, obj);
	    local_player = obj->id;
	    break;
	}
    }

    /* end XXX */

    return 0;
}


static void do_shutdown ()
{
    map_destroy (map);
    destroy_bitmap (bmp);

    fps_shutdown ();
}


static void do_input ()
{
    object_t *obj;

    foreach (obj, map->objects)
	if (obj->id == local_player)
	    break;

    if (key[KEY_RIGHT])
	obj->cvar.xv += 1.4;
    if (key[KEY_LEFT])
	obj->cvar.xv -= 1.4;
    if (key[KEY_DOWN])
	obj->cvar.y += 4;
    if (key[KEY_UP])
	obj->cvar.y -= 4;

    camera_track_object (&cam, obj);
}


static void do_physics ()
{
    object_t *obj;

    foreach (obj, map->objects) {
	obj->cvar.x += obj->cvar.xv;
	obj->cvar.y += obj->cvar.yv;
	obj->cvar.xv *= 0.75;
    }
}


static void trans_textprintf (BITMAP *bmp, FONT *font, int x, int y,
			      int color, const char *fmt, ...)
{
    va_list ap;
    BITMAP *tmp;
    char buf[1024];

    va_start (ap, fmt);
    uvsprintf (buf, fmt, ap);
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
    render (bmp, map, &cam);

    if (1) {
	BITMAP *t = create_magic_bitmap (320, 200);
	clear (t);
	pivot_magic_sprite (t, store_dat ("/player/torch"), 160, 100, 0, 36,
			     fatan2 (mouse_y - 100, mouse_x - 160));
	draw_trans_sprite (bmp, t, 0, 0);
	destroy_bitmap (t);
    }

    text_mode (-1);
    trans_textprintf (bmp, font, 0, 0, makecol24 (0x88, 0x88, 0xf8),
		      "%d FPS", fps);
    blit_magic_format (bmp, screen, SCREEN_W, SCREEN_H);

    frames++;
}


static int do_loop ()
{
    do_input ();
    do_physics ();
    do_render ();

    return key[KEY_Q];
}


int game ()
{
    if (do_init () < 0)
	return -1;

    while (do_loop () == 0);

    do_shutdown ();
    return 0;
}
