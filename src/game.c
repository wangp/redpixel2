/* game.c
 *
 * Peter Wang <tjaden@psynet.net>
 */


#include <allegro.h>
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
    bmp = create_magic_bitmap (SCREEN_W, SCREEN_H);
    if (!bmp)
	return -1;

    /* XXX */
    /* This is temporary until a menu system is in place and some
       proper networking.  */

    map = map_load ("test.pit", 1);
    if (!map)
	return -1;

    {
	start_t *start;
	object_t *obj;

	foreach (start, map->starts) {
	    obj = object_create ("player");
	    obj->x = start->x;
	    obj->y = start->y;
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
}


static int do_loop ()
{
    render (bmp, map, &cam);
    draw_trans_sprite (bmp, store_dat ("/player/torch"), 0, 0);
    blit_magic_format (bmp, screen, SCREEN_W, SCREEN_H);

    return 0;
}


int game ()
{
    if (do_init () < 0)
	return -1;

    while (do_loop () == 0);

    do_shutdown ();
    return 0;
}
